#include "music.h"
#include "control.h"

// TODO
// fix the stretched out time on playNote()
// add a wait / delay function
// add support for other time signatures and remove the magic numbers in playNote()

// note this only plays 4/4 for now
int music_bpm = 120;
const float music_noteTable[12][9] = {
  {16.35	,		32.70	,		65.41	,		130.81	,		261.63	,		523.25	,		1046.50	,		2093.00	,		4186.01	},	// C
  {17.32	,		34.65	,		69.30	,		138.59	,		277.18	,		554.37	,		1108.73	,		2217.46	,		4434.92	},	// C#/Db
  {18.35	,		36.71	,		73.42	,		146.83	,		293.66	,		587.33	,		1174.66	,		2349.32	,		4698.63	},	// D
  {19.45	,		38.89	,		77.78	,		155.56	,		311.13	,		622.25	,		1244.51	,		2489.02	,		4978.03	},	// D#/Eb
  {20.60	,		41.20	,		82.41	,		164.81	,		329.63	,		659.25	,		1318.51	,		2637.02	,		5274.04	},	// E
  {21.83	,		43.65	,		87.31	,		174.61	,		349.23	,		698.46	,		1396.91	,		2793.83	,		5587.65	},	// F
  {23.12	,		46.25	,		92.50	,			185 	,		369.99	,		739.99	,		1479.98	,		2959.96	,		5919.91	},	// F#/Gb
  {24.50	,		49		,		98		,		196	  	,		392		  ,		783.99	,		1567.98	,		3135.96	,		6271.93	},	// G
  {25.96	,		51.91	,		103.83,		207.65	,		415.30	,		830.61	,		1661.22	,		3322.44	,		6644.88	},	// G#/Ab
  {27.50	,		55		,		110		,		220		  ,		440		  ,		880		  ,		1760	  ,		3520  	,		7040  	},	// A
  {29.14	,		58.27	,		116.54,		233.08	,		466.16	,		932.33	,		1864.66	,		3729.31	,		7458.62	},	// A#/Bb
  {30.87	,		61.74	,		123.47,		246.94	,		493.88	,		987.77	,		1975.53	,		3951.07	,		7902.13	}	  // B
};

// im guessing a 1000 steps/second would make a 1000hz tone
// frequency in hz and time in seconds
void playFreq(float hertz, float time)
{
  int numSteps = (int)(hertz * time);
  long unsigned int microDelay = (1.0/hertz) * 1000000.0;
  
  for(int i = 0; i < numSteps; i++)
  {
    step(1, 0);
    // delayMicroseconds is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}

// to play an 8th note playNote(a, 3, 8)
void playNote(char note, int octave, int noteFrac)
{
  playFreq(music_noteTable[note][octave], 60.0/( ( (float)music_bpm/4.0 ) * noteFrac) );
}

