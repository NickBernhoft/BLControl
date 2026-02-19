#!/usr/bin/env python3
"""Convert a MIDI file into motor-friendly note events for the Arduino codebase."""

from __future__ import annotations

import argparse
import os
import sys
from bisect import bisect_right
from dataclasses import dataclass
from typing import List, Tuple, TYPE_CHECKING

try:
    import mido
except ImportError:
    mido = None

if TYPE_CHECKING:
    import mido as mido_types


PITCH_CLASS_NAMES = ["C", "Cs", "D", "Ds", "E", "F", "Fs", "G", "Gs", "A", "As", "B"]
MAX_EVENT_DURATION_MS = 65535


@dataclass
class TempoPoint:
    tick: int
    tempo: int
    sec: float


@dataclass
class NoteEvent:
    note: int
    octave: int
    duration_ms: int


def append_event(events: List[NoteEvent], note: int, octave: int, duration_ms: int) -> None:
    remaining = max(1, int(duration_ms))
    while remaining > MAX_EVENT_DURATION_MS:
        events.append(NoteEvent(note, octave, MAX_EVENT_DURATION_MS))
        remaining -= MAX_EVENT_DURATION_MS
    events.append(NoteEvent(note, octave, remaining))


@dataclass
class TrackInfo:
    index: int
    name: str
    note_count: int
    channels: List[int]
    programs: List[int]
    note_min: int | None
    note_max: int | None
    duration_sec: float
    channel_infos: List["ChannelInfo"]


@dataclass
class ChannelInfo:
    channel: int
    note_count: int
    note_min: int | None
    note_max: int | None
    avg_note: float | None
    program: int | None


def format_note_name(note_number: int | None) -> str:
    if note_number is None:
        return "-"
    pitch = PITCH_CLASS_NAMES[note_number % 12]
    octave = (note_number // 12) - 1
    return f"{pitch}{octave} ({note_number})"


def gm_family_name(program: int | None) -> str:
    if program is None:
        return "Unknown"
    families = [
        "Piano",
        "Chromatic Percussion",
        "Organ",
        "Guitar",
        "Bass",
        "Strings",
        "Ensemble",
        "Brass",
        "Reed",
        "Pipe",
        "Synth Lead",
        "Synth Pad",
        "Synth FX",
        "Ethnic",
        "Percussive",
        "Sound FX",
    ]
    family_idx = int(program) // 8
    if 0 <= family_idx < len(families):
        return families[family_idx]
    return "Unknown"


def channel_role_hint(channel: int, program: int | None, avg_note: float | None) -> str:
    if channel == 9:
        return "drums/percussion"
    if program is not None and 32 <= program <= 39:
        return "bass"
    if program is not None and 80 <= program <= 87:
        return "lead"
    if program is not None and 88 <= program <= 95:
        return "pad"
    if avg_note is not None and avg_note < 52:
        return "likely bass"
    if avg_note is not None and avg_note >= 67:
        return "likely lead"
    return "harmony/rhythm"


def track_display_name(track: "mido_types.MidiTrack", fallback: str) -> str:
    for msg in track:
        if msg.type == "track_name" and getattr(msg, "name", ""):
            return str(msg.name)
    return fallback


def collect_track_info(
    mid: "mido_types.MidiFile",
    tempo_points: List[TempoPoint],
) -> List[TrackInfo]:
    infos: List[TrackInfo] = []
    for index, track in enumerate(mid.tracks):
        abs_tick = 0
        channels: set[int] = set()
        programs: set[int] = set()
        note_numbers: List[int] = []
        channel_notes: dict[int, List[int]] = {}
        channel_program: dict[int, int] = {}

        for msg in track:
            abs_tick += msg.time
            if hasattr(msg, "channel"):
                channels.add(int(msg.channel))
            if msg.type == "program_change":
                programs.add(int(msg.program))
                channel_program[int(msg.channel)] = int(msg.program)
            if msg.type == "note_on" and msg.velocity > 0:
                note_numbers.append(int(msg.note))
                channel_notes.setdefault(int(msg.channel), []).append(int(msg.note))

        channel_infos: List[ChannelInfo] = []
        for ch in sorted(channels):
            notes_for_channel = channel_notes.get(ch, [])
            avg_note = (sum(notes_for_channel) / float(len(notes_for_channel))) if notes_for_channel else None
            channel_infos.append(
                ChannelInfo(
                    channel=ch,
                    note_count=len(notes_for_channel),
                    note_min=min(notes_for_channel) if notes_for_channel else None,
                    note_max=max(notes_for_channel) if notes_for_channel else None,
                    avg_note=avg_note,
                    program=channel_program.get(ch),
                )
            )

        duration_sec = tick_to_seconds(abs_tick, tempo_points, mid.ticks_per_beat)
        info = TrackInfo(
            index=index,
            name=track_display_name(track, f"Track {index}"),
            note_count=len(note_numbers),
            channels=sorted(channels),
            programs=sorted(programs),
            note_min=min(note_numbers) if note_numbers else None,
            note_max=max(note_numbers) if note_numbers else None,
            duration_sec=duration_sec,
            channel_infos=channel_infos,
        )
        infos.append(info)

    return infos


def print_track_summary(infos: List[TrackInfo]) -> None:
    print("MIDI track summary:")
    for info in infos:
        channel_text = ",".join(str(c) for c in info.channels) if info.channels else "-"
        program_text = ",".join(str(p) for p in info.programs) if info.programs else "-"
        note_range = f"{format_note_name(info.note_min)} .. {format_note_name(info.note_max)}"
        print(
            f"  [{info.index:02d}] {info.name} | notes={info.note_count} "
            f"| channels={channel_text} | programs={program_text} "
            f"| range={note_range} | duration={info.duration_sec:.2f}s"
        )
        for channel_info in info.channel_infos:
            if channel_info.note_count == 0:
                continue
            program_text = "-"
            if channel_info.program is not None:
                program_text = f"{channel_info.program} ({gm_family_name(channel_info.program)})"
            avg_text = f"{channel_info.avg_note:.1f}" if channel_info.avg_note is not None else "-"
            role = channel_role_hint(channel_info.channel, channel_info.program, channel_info.avg_note)
            channel_range = f"{format_note_name(channel_info.note_min)} .. {format_note_name(channel_info.note_max)}"
            print(
                f"      ch{channel_info.channel:02d} | notes={channel_info.note_count} "
                f"| program={program_text} | range={channel_range} | avg={avg_text} | role={role}"
            )


def parse_track_indices(text: str) -> List[int]:
    values: List[int] = []
    for chunk in text.split(","):
        raw = chunk.strip()
        if not raw:
            continue
        try:
            value = int(raw)
        except ValueError as exc:
            raise argparse.ArgumentTypeError(f"Invalid track index: {raw}") from exc
        if value < 0:
            raise argparse.ArgumentTypeError("Track index must be >= 0")
        values.append(value)
    if not values:
        raise argparse.ArgumentTypeError("Track list cannot be empty")
    return sorted(set(values))


def parse_channel_indices(text: str) -> List[int]:
    values = parse_track_indices(text)
    for value in values:
        if value > 15:
            raise argparse.ArgumentTypeError("MIDI channel must be in range 0-15")
    return values


def prompt_for_tracks(track_infos: List[TrackInfo]) -> List[int]:
    valid = {info.index for info in track_infos}
    while True:
        raw = input("Choose track index(es) to convert (e.g. 2 or 1,3) or 'all': ").strip()
        if not raw:
            print("Please enter a track index, a comma list, or 'all'.")
            continue
        if raw.lower() == "all":
            return sorted(valid)
        try:
            selected = parse_track_indices(raw)
        except argparse.ArgumentTypeError as exc:
            print(f"Invalid input: {exc}")
            continue

        invalid = sorted(set(selected) - valid)
        if invalid:
            print(f"Invalid track index(es): {invalid}. Valid range is 0 to {max(valid)}.")
            continue
        return selected


def prompt_for_channels(track_info: TrackInfo) -> List[int] | None:
    active_channels = [channel_info.channel for channel_info in track_info.channel_infos if channel_info.note_count > 0]
    if len(active_channels) <= 1:
        return None

    print(f"Track {track_info.index} contains multiple active MIDI channels.")
    while True:
        raw = input("Choose channel(s) to convert (e.g. 0 or 0,3) or 'all': ").strip()
        if not raw:
            print("Please enter channel index(es) or 'all'.")
            continue
        if raw.lower() == "all":
            return None
        try:
            selected = parse_channel_indices(raw)
        except argparse.ArgumentTypeError as exc:
            print(f"Invalid input: {exc}")
            continue
        invalid = sorted(set(selected) - set(active_channels))
        if invalid:
            print(f"Invalid channel(s): {invalid}. Active channels are {active_channels}.")
            continue
        return selected


def build_tempo_map(mid: "mido_types.MidiFile") -> List[TempoPoint]:
    assert mido is not None
    tempo = 500000
    abs_tick = 0
    abs_sec = 0.0
    points = [TempoPoint(0, tempo, 0.0)]
    for msg in mido.merge_tracks(mid.tracks):
        abs_tick += msg.time
        if msg.type == "set_tempo":
            last = points[-1]
            delta_ticks = abs_tick - last.tick
            abs_sec = last.sec + mido.tick2second(delta_ticks, mid.ticks_per_beat, last.tempo)
            tempo = msg.tempo
            points.append(TempoPoint(abs_tick, tempo, abs_sec))
    return points


def tick_to_seconds(tick: int, points: List[TempoPoint], ticks_per_beat: int) -> float:
    assert mido is not None
    ticks = [p.tick for p in points]
    idx = bisect_right(ticks, tick) - 1
    if idx < 0:
        idx = 0
    point = points[idx]
    delta_ticks = tick - point.tick
    return point.sec + mido.tick2second(delta_ticks, ticks_per_beat, point.tempo)


def seconds_to_millis(duration_sec: float) -> int:
    return max(1, int(round(duration_sec * 1000.0)))


def extract_notes(track: "mido_types.MidiTrack", channels: set[int] | None = None) -> List[Tuple[int, int, int]]:
    abs_tick = 0
    active: dict[int, List[int]] = {}
    notes: List[Tuple[int, int, int]] = []
    for msg in track:
        abs_tick += msg.time
        if not hasattr(msg, "channel"):
            continue
        message_channel = int(msg.channel)
        if channels is not None and message_channel not in channels:
            continue

        if msg.type == "note_on" and msg.velocity > 0:
            active.setdefault(msg.note, []).append(abs_tick)
        elif msg.type == "note_off" or (msg.type == "note_on" and msg.velocity == 0):
            starts = active.get(msg.note)
            if starts:
                start_tick = starts.pop(0)
                notes.append((start_tick, abs_tick, msg.note))
    return notes


def make_monophonic(notes: List[Tuple[int, int, int]]) -> List[Tuple[int, int, int]]:
    notes.sort(key=lambda n: (n[0], n[1], n[2]))
    mono: List[Tuple[int, int, int]] = []
    for start, end, note in notes:
        if end <= start:
            continue
        if mono:
            prev_start, prev_end, prev_note = mono[-1]
            if start < prev_end:
                mono[-1] = (prev_start, start, prev_note)
        mono.append((start, end, note))
    return [n for n in mono if n[1] > n[0]]


def map_note_to_pitch(note_number: int, out_of_range: str) -> Tuple[int, int] | None:
    pitch_class = note_number % 12
    octave = (note_number // 12) - 1
    if 0 <= octave <= 8:
        return pitch_class, octave
    if out_of_range == "clamp":
        octave = max(0, min(8, octave))
        return pitch_class, octave
    return None


def build_events(
    notes: List[Tuple[int, int, int]],
    points: List[TempoPoint],
    ticks_per_beat: int,
    out_of_range: str,
    transpose_octaves: int,
    trim_initial_rest: bool,
) -> List[NoteEvent]:
    events: List[NoteEvent] = []
    if not notes:
        return events

    mono = make_monophonic(notes)
    current_sec = 0.0
    for start_tick, end_tick, note_number in mono:
        start_sec = tick_to_seconds(start_tick, points, ticks_per_beat)
        end_sec = tick_to_seconds(end_tick, points, ticks_per_beat)
        if start_sec > current_sec + 1e-6:
            if not (trim_initial_rest and len(events) == 0):
                rest_dur = start_sec - current_sec
                append_event(events, -1, 0, seconds_to_millis(rest_dur))
        dur = max(0.0, end_sec - start_sec)
        if dur <= 0.0:
            current_sec = max(current_sec, end_sec)
            continue
        mapped = map_note_to_pitch(note_number, out_of_range)
        if mapped is None:
            current_sec = max(current_sec, end_sec)
            continue
        pitch_class, octave = mapped
        octave += transpose_octaves
        if octave < 0 or octave > 8:
            if out_of_range == "clamp":
                octave = max(0, min(8, octave))
            else:
                current_sec = max(current_sec, end_sec)
                continue
        append_event(events, pitch_class, octave, seconds_to_millis(dur))
        current_sec = max(current_sec, end_sec)

    return events


def trim_leading_rests(events: List[NoteEvent]) -> List[NoteEvent]:
    start_idx = 0
    while start_idx < len(events) and events[start_idx].note == -1:
        start_idx += 1
    return events[start_idx:]


def generate_header(
    out_path: str,
    events_per_track: List[Tuple[int, str, List[NoteEvent]]],
    source_path: str,
    transpose_octaves: int,
) -> None:
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("#pragma once\n\n")
        f.write("#include <stddef.h>\n")
        f.write("#include <stdint.h>\n")
        f.write("#include <string.h>\n")
        f.write("#include \"music.h\"\n")
        f.write("#include \"../control.h\"\n\n")
        f.write("#if defined(__AVR__)\n")
        f.write("#include <avr/pgmspace.h>\n")
        f.write("#define MOTOR_PROGMEM PROGMEM\n")
        f.write("#define MOTOR_READ_EVENT(dst, src_ptr) memcpy_P(&(dst), (src_ptr), sizeof(MotorNoteEvent))\n")
        f.write("#else\n")
        f.write("#define MOTOR_PROGMEM\n")
        f.write("#define MOTOR_READ_EVENT(dst, src_ptr) (dst = *(src_ptr))\n")
        f.write("#endif\n\n")
        f.write("// Auto-generated from: ")
        f.write(source_path.replace("\\", "/"))
        f.write("\n")
        f.write("// NOTE: Durations are absolute milliseconds from MIDI timing (tempo map honored).\n\n")
        f.write(f"// Octave transpose: {transpose_octaves:+d}\n\n")
        f.write("static const int8_t MOTOR_REST = -1;\n\n")
        f.write("struct MotorNoteEvent {\n")
        f.write("  int8_t note;\n")
        f.write("  int8_t octave;\n")
        f.write("  uint16_t duration_ms;\n")
        f.write("} __attribute__((packed));\n\n")
        f.write("static inline void playMotorEvent(const MotorNoteEvent &ev) {\n")
        f.write("  if (ev.note == MOTOR_REST) {\n")
        f.write("    longDelayMicroseconds((unsigned long)(ev.duration_ms * 1000UL));\n")
        f.write("    return;\n")
        f.write("  }\n")
        f.write("  playFreq(music_noteTable[(int)ev.note][(int)ev.octave], (float)ev.duration_ms / 1000.0f);\n")
        f.write("}\n\n")

        for track_index, track_name, events in events_per_track:
            if not events:
                continue
            f.write(f"// Source track {track_index}: {track_name}\n")
            f.write(f"static const MotorNoteEvent motor_track_{track_index}[] MOTOR_PROGMEM = {{\n")
            for ev in events:
                if ev.note == -1:
                    f.write(f"  {{MOTOR_REST, 0, {ev.duration_ms}}},\n")
                else:
                    name = PITCH_CLASS_NAMES[ev.note]
                    f.write(f"  {{{name}, {ev.octave}, {ev.duration_ms}}},\n")
            f.write("};\n\n")
            f.write(f"static inline void playMotorTrack_{track_index}() {{\n")
            f.write(f"  for (size_t i = 0; i < sizeof(motor_track_{track_index}) / sizeof(motor_track_{track_index}[0]); ++i) {{\n")
            f.write("    MotorNoteEvent ev;\n")
            f.write(f"    MOTOR_READ_EVENT(ev, &motor_track_{track_index}[i]);\n")
            f.write("    playMotorEvent(ev);\n")
            f.write("  }\n")
            f.write("}\n\n")


def main() -> int:
    if mido is None:
        print("Missing dependency: mido. Install with 'pip install mido'.", file=sys.stderr)
        return 1

    parser = argparse.ArgumentParser(description="Convert MIDI to motor note events.")
    parser.add_argument("input", help="Path to MIDI file")
    parser.add_argument("-o", "--out", default=None, help="Output header path (default: <input>_motor.h)")
    parser.add_argument("--out-of-range", choices=["clamp", "skip"], default="clamp", help="Handle notes outside 0-8 octaves")
    parser.add_argument("--transpose-octaves", type=int, default=0, help="Transpose all notes by whole octaves (e.g. -1, +1)")
    parser.add_argument("--trim-leading-rests", action="store_true", help="Remove initial rest events from each output track")
    parser.add_argument("--track", type=parse_track_indices, default=None, help="Convert only these track index(es), e.g. 2 or 1,3")
    parser.add_argument("--channel", type=parse_channel_indices, default=None, help="Optional MIDI channel filter (0-15), e.g. 0 or 0,3")
    parser.add_argument("--list-tracks", action="store_true", help="Print track summary and exit")
    args = parser.parse_args()

    mid = mido.MidiFile(args.input)
    tempo_map = build_tempo_map(mid)
    track_infos = collect_track_info(mid, tempo_map)
    print_track_summary(track_infos)

    if args.list_tracks:
        return 0

    selected_indices = [info.index for info in track_infos]
    selected_channels = args.channel
    if args.track is not None:
        requested = set(args.track)
        valid = {info.index for info in track_infos}
        invalid = sorted(requested - valid)
        if invalid:
            print(f"Error: invalid track index(es): {invalid}", file=sys.stderr)
            return 1
        selected_indices = [idx for idx in selected_indices if idx in requested]
    elif sys.stdin.isatty():
        selected_indices = prompt_for_tracks(track_infos)
    else:
        print("No --track provided and stdin is not interactive; converting all tracks.", file=sys.stderr)

    if selected_channels is None and sys.stdin.isatty() and len(selected_indices) == 1:
        selected_channels = prompt_for_channels(track_infos[selected_indices[0]])

    output_path = args.out
    if output_path is None:
        in_dir = os.path.dirname(args.input)
        in_stem = os.path.splitext(os.path.basename(args.input))[0]
        output_path = os.path.join(in_dir, f"{in_stem}_motor.h")

    events_per_track: List[Tuple[int, str, List[NoteEvent]]] = []
    channel_filter = set(selected_channels) if selected_channels is not None else None
    for track_index in selected_indices:
        track = mid.tracks[track_index]
        notes = extract_notes(track, channel_filter)
        events = build_events(
            notes,
            tempo_map,
            mid.ticks_per_beat,
            args.out_of_range,
            args.transpose_octaves,
            args.trim_leading_rests,
        )
        if args.trim_leading_rests:
            events = trim_leading_rests(events)
        track_name = track_infos[track_index].name
        if selected_channels is not None:
            track_name = f"{track_name} [channels {','.join(str(ch) for ch in selected_channels)}]"
        events_per_track.append((track_index, track_name, events))

    generate_header(output_path, events_per_track, args.input, args.transpose_octaves)

    non_empty = sum(1 for _, _, events in events_per_track if events)
    print(f"Wrote {output_path} with {non_empty} non-empty track(s) from {len(selected_indices)} selected track(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
