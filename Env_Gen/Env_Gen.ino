const int attackTime = 500;   // milliseconds
const int decayTime = 300;    // milliseconds
const int sustainLevel = 128; // 0-255
const int releaseTime = 400;  // milliseconds

const int OUTPUT_PIN = 9;
const int BUTTON_PIN = 2;

enum EnvelopeState { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
EnvelopeState envState = IDLE;

float envelopeValue = 0;
unsigned long phaseStartTime = 0;
bool isPressed = false;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentTime = millis();

  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  // State transitions based on button input
  if (buttonPressed && envState == IDLE) {
    isPressed = true;
    envState = ATTACK;
    phaseStartTime = currentTime;
  }

  if (!buttonPressed && isPressed) {
    isPressed = false;
    if (envState != IDLE && envState != RELEASE) {
      envState = RELEASE;
      phaseStartTime = currentTime;
    }
  }

  // Handle ADSR envelope phases
  switch (envState) {
    case IDLE:
      envelopeValue = 0;
      analogWrite(OUTPUT_PIN, (int)envelopeValue);
      break;

    case ATTACK:
      envelopeValue = (255.0 * (currentTime - phaseStartTime)) / attackTime;
      if (envelopeValue >= 255.0) {
        envelopeValue = 255.0;
        envState = DECAY;
        phaseStartTime = currentTime;
      }
      analogWrite(OUTPUT_PIN, (int)envelopeValue);
      break;

    case DECAY:
      envelopeValue = 255.0 - ((255.0 - sustainLevel) * (currentTime - phaseStartTime) / decayTime);
      if (envelopeValue <= sustainLevel) {
        envelopeValue = sustainLevel;
        envState = SUSTAIN;
      }
      analogWrite(OUTPUT_PIN, (int)envelopeValue);
      break;

    case SUSTAIN:
      envelopeValue = sustainLevel;
      analogWrite(OUTPUT_PIN, (int)envelopeValue);
      break;

    case RELEASE:
      envelopeValue = sustainLevel - (sustainLevel * (currentTime - (float)phaseStartTime) / releaseTime);
      if (envelopeValue <= 0) {
        envelopeValue = 0;
        envState = IDLE;
      }
      analogWrite(OUTPUT_PIN, (int)envelopeValue);
      break;
  }
}
