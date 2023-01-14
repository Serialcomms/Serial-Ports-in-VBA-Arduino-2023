

/*
  TickTock Serial Port
  Toggles the on-board LED on/off every second
  Sends Arduino milliseconds to com port suffixed with alternating Tick / Tock string 
*/

String startup;
String RX_Data;
String TX_Data;
char timestamp[10];
unsigned long LastToggle;
const long ToggleTime = 500;

void setup() {
  Serial.end();
  Serial.setTimeout(0);
  Serial.begin(115200, SERIAL_8N1);
 // pinMode(8, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  startup = "Tick-Tock Starting, ";
  startup = startup + "Milliseconds = ";
  sprintf(timestamp, "%010u", millis());
  Serial.println(startup + timestamp);
  Serial.flush();
 // Flash_LED(8, 4, 120);
  Flash_LED(LED_BUILTIN, 4, 250);
}

void loop() {

  if (millis() > (LastToggle + ToggleTime))

  {
    Toggle_LED();
    Toggle_TIME();

    LastToggle = millis();
  }
}

void Toggle_LED()

{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void Toggle_TIME()

{
  if (digitalRead(LED_BUILTIN))

  {
    Toggle_DATA(" Tick");
  }

  else

  {
    Toggle_DATA(" Tock");
  }
}

void Toggle_DATA(String dataString) {

  sprintf(timestamp, "%010u", millis());

  if (RX_Data.length() < 1)

  {
    TX_Data = " : No Data ";
  }

  else

  {
    TX_Data = " : Data = " + RX_Data;
  }

  Serial.println(timestamp + dataString + TX_Data);

  Serial.flush();

  RX_Data = "";
}

void serialEvent()

{
  while (Serial.available())
  RX_Data += (char)Serial.read();
}


void Flash_LED(int LED_Pin, int FlashCount, int FlashTime)

{

  for (int flash = 0; flash < FlashCount; flash++)

  {
    delay(FlashTime);
    digitalWrite(LED_Pin, HIGH);
    delay(FlashTime);
    digitalWrite(LED_Pin, LOW);
  }
}
