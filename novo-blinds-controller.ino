

// each channel behaves like a button
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getter1, setter1),
  ZUNO_SWITCH_BINARY(getter2, setter2),
  ZUNO_SWITCH_BINARY(getter3, setter3),
  ZUNO_SWITCH_BINARY(getter4, setter4));


const byte portStart = 9;
const byte numPorts = 4;
const byte ledPin = 13;

// port mapping
// 0 - close (momentary press start, another press stop)
// 1 - close step (moving while pressed, stops on release)
// 2 - open step (moving while pressed, stops on release)
// 3 - open (momentary press start, another press stop)

// we keep millis() when button was pressed or 0 if its released
unsigned long values[numPorts] = {0, 0, 0, 0};
const unsigned long keepPressedMs[numPorts] = {1000, 2000, 2000, 1000};

// 0 is released, 255 is pressed
// we alware return 0, so buttons seems always released
byte getter(byte idx) {
  return values[idx] > 0 ? 255 : 0;
}

void setter(byte idx, byte value) {
  values[idx] = (value > 0 ? millis() : 0);
  digitalWrite(portStart + idx, value > 0 ? HIGH : LOW);
  // use led pin for debug
  digitalWrite(ledPin, value > 0 ? HIGH : LOW);
}

void autoRelease(byte idx) {
  if (values[idx] == 0) return; // released already

  unsigned long curTime = millis();

  if (curTime < values[idx]) // time wrapped, lets re-set it to current time
  {
    values[idx] = curTime;
  }
  if (values[idx] + keepPressedMs[idx] < curTime)
    setter(idx, 0);
}

void autoReleaseAll()
{
  for (int i = 0; i < numPorts; ++i)
    autoRelease(i);
}

// z-wave bindings

byte getter1() {
  return 0;
}

void setter1(byte value) {
  setter(0, 255);
}

byte getter2() {
  return 0;
}

void setter2(byte value) {
  setter(1, 255);
}

byte getter3() {
  return 0;
}

void setter3(byte value) {
  setter(2, 255);
}

byte getter4() {
  return 0;
}

void setter4(byte value) {
  setter(3, 255);
}


// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(ledPin, OUTPUT);

  // write to transistor base
  for (int i = 0; i < numPorts; ++i)
  {
    pinMode(portStart + i, OUTPUT);
    setter(i, 0);
  }
}

// the loop function runs over and over again forever
void loop() {
  delay(20); // no need to check too often
  autoReleaseAll();
}
