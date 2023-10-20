

// each channel behaves like a button
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getter1, setter1),
  ZUNO_SWITCH_BINARY(getter2, setter2),
  ZUNO_SWITCH_BINARY(getter3, setter3),
  ZUNO_SWITCH_BINARY(getter4, setter4),
  ZUNO_SWITCH_BINARY(getter5, setter5));

const byte numPorts = 5;
const byte ledPin = 13;

// port mapping
// 0 - close step (moving while pressed, stops on release)
// 1 - close (momentary press start, another press stop)
// 2 - open (momentary press start, another press stop)
// 3 - open step (moving while pressed, stops on release)
// 4 - stop
const unsigned long keepPressedMs[numPorts] = {1500, 500, 500, 1500, 200};
const byte portToPinMap[numPorts] = {9, 10, 11, 12, 14};

int pressedButton = -1;
unsigned long whenToReleaseMs = 0;

bool isButtonPressed(byte idx)
{
  return (pressedButton == idx);
}

void releaseButtonNow() {
  if (pressedButton == -1) return;

  digitalWrite(portToPinMap[pressedButton], LOW);
  // use led pin for debug
  digitalWrite(ledPin, LOW);

  byte wasPressed = pressedButton;

  pressedButton = -1;
  whenToReleaseMs = 0;

  // send report (fixme - how to figure out whether it's unsolicited release?
  zunoSendReport(wasPressed + 1);
}

void checkAndReleaseButton() {
  if (pressedButton == -1) return;

  unsigned long curTime = millis();

  // check 1) too early to release 2) time overlow
  if (curTime < whenToReleaseMs || curTime > whenToReleaseMs + 10000)
    return;

  releaseButtonNow();

}

void pressButton(byte idx) {

  releaseButtonNow();

  digitalWrite(portToPinMap[idx], HIGH);
  // use led pin for debug
  digitalWrite(ledPin, HIGH);

  pressedButton = idx;
  whenToReleaseMs = millis() + keepPressedMs[idx];

}

// z-wave bindings

byte getter1() {
  return isButtonPressed(0);
}

void setter1(byte value) {
  pressButton(0);
}

byte getter2() {
  return isButtonPressed(1);
}

void setter2(byte value) {
  pressButton(1);
}

byte getter3() {
  return isButtonPressed(2);
}

void setter3(byte value) {
  pressButton(2);
}

byte getter4() {
  return isButtonPressed(3);
}

void setter4(byte value) {
  pressButton(3);
}

byte getter5() {
  return isButtonPressed(4);
}

void setter5(byte value) {
  pressButton(4);
}

// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(ledPin, OUTPUT);

  // write to transistor base
  for (int i = 0; i < numPorts; ++i)
  {
    pinMode(portToPinMap[i], OUTPUT);
    digitalWrite(portToPinMap[i], LOW);
  }
}

// the loop function runs over and over again forever
void loop() {
  checkAndReleaseButton();
}
