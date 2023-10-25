

ZUNO_ENABLE(SKETCH_VERSION=0x0103)

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
const byte portToPinMap[numPorts] = {9, 10, 11, 12, 14};

// enum for parameter numbers
enum {
  KEEP_PRESSED_PORT_0 = 64,
  KEEP_PRESSED_PORT_1,
  KEEP_PRESSED_PORT_2,
  KEEP_PRESSED_PORT_3,
  KEEP_PRESSED_PORT_4
};

// Device's configuration parametrs definitions
ZUNO_SETUP_CONFIGPARAMETERS(
  ZUNO_CONFIG_PARAMETER("Pressed time 0 (close step)", 20, 3000, 1500),
  ZUNO_CONFIG_PARAMETER("Pressed time 1 (close)", 20, 3000, 500),
  ZUNO_CONFIG_PARAMETER("Pressed time 2 (open)", 20, 3000, 500),
  ZUNO_CONFIG_PARAMETER("Pressed time 3 (open step)", 20, 3000, 1500),
  ZUNO_CONFIG_PARAMETER("Pressed time 4 (stop)", 20, 3000, 200));

ZUNO_SETUP_CFGPARAMETER_HANDLER(configParameterChanged);

unsigned long keepPressedMs[numPorts] = {0};
int pressedButton = -1;
unsigned long whenToReleaseMs = 0;

bool isButtonPressed(byte idx)
{
   // report seems to create issues with latest z-uno 2 boards
   // so we just always return false there
  return false;
  
  //return (pressedButton == idx);
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
  // report seems to create issues with latest z-uno 2 boards
  //zunoSendReport(wasPressed + 1);
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

void configParameterChanged(uint8_t param, uint32_t value) {

  if (param >= KEEP_PRESSED_PORT_0 + numPorts)
    return;

  byte idx = param - KEEP_PRESSED_PORT_0;
  keepPressedMs[idx] = value;
}

// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(ledPin, OUTPUT);

  // write to transistor base
  for (int i = 0; i < numPorts; ++i)
  {
    pinMode(portToPinMap[i], OUTPUT);
    digitalWrite(portToPinMap[i], LOW);

    // set default press duration
    keepPressedMs[i] = zunoLoadCFGParam(KEEP_PRESSED_PORT_0 + i);
  }



}

// the loop function runs over and over again forever
void loop() {
  checkAndReleaseButton();
}
