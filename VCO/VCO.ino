#define MOZZI_CONTROL_RATE 256
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>

Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> sinWave(SIN2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, MOZZI_AUDIO_RATE> triangleWave(SIN2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, MOZZI_AUDIO_RATE> sqareWave(SQUARE_NO_ALIAS_2048_DATA);

void setup(){
  Serial.begin(9600);

  sinWave.setFreq(440);
  triangleWave.setFreq(440);
  sqareWave.setFreq(440);

  startMozzi();
}

void updateControl(){
  // int analogValue = analogRead(ENV_INPUT_PIN);
  // envelopeValue = (float)analogValue / 1023.0;
}

AudioOutput updateAudio(){
  int scaledOutput = (int)(sinWave.next() * 1);
  return MonoOutput::from8Bit(scaledOutput);
}


void loop(){
  audioHook();
}