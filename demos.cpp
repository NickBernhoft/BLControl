#include "Music/music.h"
#include "control.h"
#include <Arduino.h>



//ramp DEMO
void rampDemo()
{
  ramp(10, 2000, FWD);
  ramp(2000, 10, FWD);
  
  Serial.println("Done!");

  ramp(10, 5000, REV);
  ramp(5000, 10, REV);
  Serial.println("Done!");
}


void ramp2Demo()
{
  ramp2(0, 4500, REV, 10, 0);
  Serial.println("DOne");
  ramp2(4500, 0, REV, 10, 1);

  delay(1000);
}





void meglovania()
{
  // the first 4 notes of meglovania lmao
  music_bpm = 240;
  playNote(D, 5, 8);
  delay(10);
  playNote(D, 5, 8);
  delay(10);
  playNote(D, 6, 4);
  delay(10);
  playNote(A, 5, 2);
  delay(10);
  playNote(Ab, 5, 4);
  delay(10);
  playNote(G, 5, 4);
  delay(10);
  playNote(F, 5, 4);
  delay(10);
  playNote(D, 5, 8);
  delay(10);
  playNote(F, 5, 8);
  delay(10);
  playNote(G, 5, 8);
  delay(10);
  
  playNote(C, 5, 8);
  delay(10);
  playNote(C, 5, 8);
  delay(10);
  playNote(D, 6, 4);
  delay(10);
  playNote(A, 5, 2);
  delay(10);
  playNote(Ab, 5, 4);
  delay(10);
  playNote(G, 5, 4);
  delay(10);
  playNote(F, 5, 4);
  delay(10);
  playNote(D, 5, 8);
  delay(10);
  playNote(F, 5, 8);
  delay(10);
  playNote(G, 5, 8);
  delay(10);

  playNote(B, 4, 8);
  delay(10);
  playNote(B, 4, 8);
  delay(10);
  playNote(D, 6, 4);
  delay(10);
  playNote(A, 5, 2);
  delay(10);
  playNote(Ab, 5, 4);
  delay(10);
  playNote(G, 5, 4);
  delay(10);
  playNote(F, 5, 4);
  delay(10);
  playNote(D, 5, 8);
  delay(10);
  playNote(F, 5, 8);
  delay(10);
  playNote(G, 5, 8);
  delay(10);

  playNote(A, 4, 8);
  delay(10);
  playNote(A, 4, 8);
  delay(10);
  playNote(D, 6, 4);
  delay(10);
  playNote(A, 5, 2);
  delay(10);
  playNote(Ab, 5, 4);
  delay(10);
  playNote(G, 5, 4);
  delay(10);
  playNote(F, 5, 4);
  delay(10);
  playNote(D, 5, 8);
  delay(10);
  playNote(F, 5, 8);
  delay(10);
  playNote(G, 5, 8);
  delay(10);

  delay(1000);

}

  //rotate clockwise test code
void SpinDemo()
{
    for(int i = 0; i < 6 * 7; i++)
  {
    step(1, 0);
    delay(24);
  }

  delay(1000);
}



//left -> right test code 
void spinDemo2()
{
  int temp = 1;
  for(int i = 0; i < 6 * 7; i++)
  {
    step(temp, 0);
    delay(5);
  }

  delay(1000);
  temp *= -1;
}




//ramp to 3500 rpm test code
void oldRampDemo()
{
  step(-1, 0);
  float mrs = 3000;
  delayMicroseconds(mrs);
  mrs--;

  if(mrs < 400)
  {
    mrs = 400;
  }
}
