
ZUNO_ENABLE(SKETCH_VERSION=0x0103);

// each channel behaves like a button
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getterCloseStep, setterCloseStep),
  ZUNO_SWITCH_BINARY(getterClose, setterClose),
  ZUNO_SWITCH_BINARY(getterOpen, setterOpen),
  ZUNO_SWITCH_BINARY(getterOpenStep, setterOpenStep),
  ZUNO_SWITCH_BINARY(getterStop, setterStop));

const byte numPorts = 5;
const byte ledPin = 13;

// port mapping
enum {
  PORT_CLOSE_STEP = 0, // 0 - close step (moving while pressed, stops on release)
  PORT_CLOSE, // 1 - close (momentary press start, another press stop)
  PORT_OPEN, // 2 - open (momentary press start, another press stop)
  PORT_OPEN_STEP, // 3 - open step (moving while pressed, stops on release)
  PORT_STOP // 4 - stop
};

const byte portToPinMap[numPorts] = {9, 10, 11, 12, 14};

// enum for parameter numbers
enum {
  KEEP_PRESSED_PORT_0 = 64,
  KEEP_PRESSED_PORT_1,
  KEEP_PRESSED_PORT_2,
  KEEP_PRESSED_PORT_3,
  KEEP_PRESSED_PORT_4,
  USE_STEPS_FOR_CONT
};

// Device's configuration parametrs definitions
ZUNO_SETUP_CONFIGPARAMETERS(
  ZUNO_CONFIG_PARAMETER("Pressed time 0 (close step)", 20, 60000, 1500),
  ZUNO_CONFIG_PARAMETER("Pressed time 1 (close)", 20, 60000, 500),
  ZUNO_CONFIG_PARAMETER("Pressed time 2 (open)", 20, 60000, 500),
  ZUNO_CONFIG_PARAMETER("Pressed time 3 (open step)", 20, 60000, 1500),
  ZUNO_CONFIG_PARAMETER("Pressed time 4 (stop)", 20, 60000, 200),
  ZUNO_CONFIG_PARAMETER_1B("Use steps for cont move", 0, 1, 0));

ZUNO_SETUP_CFGPARAMETER_HANDLER(configParameterChanged);

unsigned long keepPressedMs[numPorts] = {0};
int pressedButton = -1;
unsigned long whenToReleaseMs = 0;
bool useStepsForCont = false;

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

void pressButton(byte idx, int keepPressedMs) {

  releaseButtonNow();

  digitalWrite(portToPinMap[idx], HIGH);
  // use led pin for debug
  digitalWrite(ledPin, HIGH);

  pressedButton = idx;
  whenToReleaseMs = millis() + keepPressedMs;

}

// z-wave bindings

byte getterCloseStep() {
  return isButtonPressed(PORT_CLOSE_STEP);
}

void setterCloseStep(byte value) {
  pressButton(PORT_CLOSE_STEP, keepPressedMs[PORT_CLOSE_STEP]);
}

byte getterClose() {
  return isButtonPressed(PORT_CLOSE);
}

void setterClose(byte value) {
  byte port = PORT_CLOSE;
  if (useStepsForCont) port = PORT_CLOSE_STEP;

  pressButton(port, keepPressedMs[PORT_CLOSE]);
}

byte getterOpen() {
  return isButtonPressed(PORT_OPEN);
}

void setterOpen(byte value) {
  byte port = PORT_OPEN;
  if (useStepsForCont) port = PORT_OPEN_STEP;

  pressButton(port, keepPressedMs[PORT_OPEN]);
}

byte getterOpenStep() {
  return isButtonPressed(PORT_OPEN_STEP);
}

void setterOpenStep(byte value) {
  pressButton(PORT_OPEN_STEP, keepPressedMs[PORT_OPEN_STEP]);
}

byte getterStop() {
  return isButtonPressed(PORT_STOP);
}

void setterStop(byte value) {
  pressButton(PORT_STOP, keepPressedMs[PORT_STOP]);
}

void configParameterChanged(uint8_t param, uint32_t value) {

  if (param >= KEEP_PRESSED_PORT_0 + numPorts) {
    if (param == USE_STEPS_FOR_CONT)
      useStepsForCont = (param != 0);
    return;
  }
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

  useStepsForCont = zunoLoadCFGParam(USE_STEPS_FOR_CONT);

}

// the loop function runs over and over again forever
void loop() {
  checkAndReleaseButton();
}
