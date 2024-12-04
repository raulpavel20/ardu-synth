#define MOZZI_CONTROL_RATE 256
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_analogue512_int8.h>

Oscil<SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> sinWave(SIN2048_DATA);
Oscil<TRIANGLE2048_NUM_CELLS, MOZZI_AUDIO_RATE> triangleWave(TRIANGLE2048_DATA);
Oscil<SQUARE_ANALOGUE512_NUM_CELLS, MOZZI_AUDIO_RATE> sqareWave(SQUARE_ANALOGUE512_DATA);
Oscil<SAW2048_NUM_CELLS, MOZZI_AUDIO_RATE> sawWave(SAW2048_DATA);

enum oscTypes { SIN,
                SQR,
                TRI,
                SAW };

enum oscTypes oscType;
String dataInput = "";
int readAux;

void setup() {
  Serial.begin(9600);

  sinWave.setFreq(440);
  triangleWave.setFreq(440);
  sqareWave.setFreq(440);
  sawWave.setFreq(440);

  startMozzi();
}

void updateControl() {
  // int analogValue = analogRead(ENV_INPUT_PIN);
  // envelopeValue = (float)analogValue / 1023.0;
}

AudioOutput updateAudio() {
  int scaledOutput = (int)(getOutput1());
  return MonoOutput::from8Bit(scaledOutput);
}

void loop() {
  audioHook();
  readData();
}

int8_t getOutput1() {
  switch (oscType) {
    case SIN:
      return sinWave.next();
    case SQR:
      return sqareWave.next();
    case TRI:
      return triangleWave.next();
    case SAW:
      return sawWave.next();
  }
}

void readData() {
  if (Serial.available()) {
    dataInput = Serial.readStringUntil('\n');
    sscanf(dataInput.c_str(), "%d", &readAux);
    oscType = readAux;
  }
}
