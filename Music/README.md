# MIDI to Motor Script (Quick Usage)

Script: `midi_to_motor.py`

## Requirements
- Python 3
- `mido` package

Install once:

```powershell
pip install mido
```

## Basic usage
From this folder (`BLControl/Music`):

```powershell
python .\midi_to_motor.py .\takefive.mid
```

This prints track/channel info, prompts for selection, and writes `<input>_motor.h`.

## Useful commands
List track/channel info only:

```powershell
python .\midi_to_motor.py .\takefive.mid --list-tracks
```

Convert a specific track + channel:

```powershell
python .\midi_to_motor.py .\takefive.mid --track 0 --channel 0
```

Transpose one octave down/up:

```powershell
python .\midi_to_motor.py .\takefive.mid --track 0 --channel 0 --transpose-octaves -1
python .\midi_to_motor.py .\takefive.mid --track 0 --channel 0 --transpose-octaves 1
```

Trim silence at the very start:

```powershell
python .\midi_to_motor.py .\takefive.mid --track 0 --channel 0 --trim-leading-rests
```

Set explicit output file:

```powershell
python .\midi_to_motor.py .\takefive.mid -o .\takefive_motor.h
```

## Notes
- `--out-of-range clamp` (default) keeps notes within octave 0-8.
- `--out-of-range skip` drops notes outside octave 0-8.
- Output headers are generated for Arduino use and include playback helpers.
