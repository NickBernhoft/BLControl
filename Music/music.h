// music.h
#ifndef MUSIC_H
#define MUSIC_H

// function prototypes
void playFreq(float hertz, float time);
void playNote(char note, int octave, int beatFrac);

// variables
extern int music_bpm;
// tnis supports all notes + thair sharp in 9 octaves
extern const float music_noteTable[12][9]; // int[note][octave], returns frequency

enum music_note{
C = 0,
Cs = 1, Db = 1,
D = 2,
Ds = 3, Eb = 3,
E = 4,
F = 5,
Fs = 6, Gb = 6,
G = 7,
Gs = 8, Ab = 8,
A = 9,
As = 10, Bb = 10, 
B = 11,
};



#endif // MUSIC_H