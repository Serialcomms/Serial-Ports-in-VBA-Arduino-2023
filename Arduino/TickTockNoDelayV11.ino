
//#include <Wire.h>

/*
  TickTock Serial Port
  Toggles the on-board LED on/off every second
  Sends Arduino milliseconds to com port suffixed with alternating Tick / Tock string 
*/
String RX_Data;
String TX_Data;
int REGISTER;
int CONNECTED;
char timestamp[16];
char analoguevals[50];
unsigned int Analogue[6];
unsigned long NextToggle;

void setup() {

  Serial.end();
  Serial.setTimeout(0);
  Serial.begin(115200, SERIAL_8N1);
  Serial.println("Tick-Tock Starting");
  Serial.flush();
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  pinMode(A3,INPUT_PULLUP);
  pinMode(A4,INPUT_PULLUP);
  pinMode(A5,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Flash_LED(LED_BUILTIN, 4, 250);
  pinMode(8, OUTPUT);
  Flash_LED(8, 4, 120);
  //pinMode(8, INPUT);
  NextToggle = (1000 * (millis() / 1000 )) + 1000;
}

void loop() 
{

 if (millis() < NextToggle)

  {
    Read_Analogue_Inputs()  ; 

    //digitalWrite(8, ? bitRead(PIND,1):HI);

    
  }

  else 
  
  {
   
    Toggle_LED();
    Toggle_TIME();
    NextToggle = (1000 * (millis() / 1000 )) + 1000;
   
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
    Toggle_DATA(" Tick, ");
  }

  else

  {
    Toggle_DATA(" Tock, ");
  }
}

void Toggle_DATA(String dataString) {

 
  if (RX_Data.length() < 1)

  {
    TX_Data = "No Data";
  }

  else

  {
    TX_Data =  RX_Data;
  }

  sprintf(analoguevals,"A0=%04d, A1=%04d, A2=%04d, A3=%04d, A4=%04d, A5=%04d," , Analogue[0] , Analogue[1] , Analogue[2] , Analogue[3] , Analogue[4] , Analogue[5]) ;
 
  millis_to_timestamp();

  Serial.print(timestamp);
  Serial.print("UNO-R3, ");
  Serial.print(analoguevals);
  Serial.print(dataString);
  Serial.print(TX_Data);
  Serial.println();
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


void Read_Analogue_Inputs()
{

  Analogue[0] = (Analogue[0] + analogRead(A0)) / 2 ; //delayMicroseconds(20);
  Analogue[1] = (Analogue[1] + analogRead(A1)) / 2 ; //delayMicroseconds(20);
  Analogue[2] = (Analogue[2] + analogRead(A2)) / 2 ; //delayMicroseconds(20);
  Analogue[3] = (Analogue[3] + analogRead(A3)) / 2 ; //delayMicroseconds(20);
  Analogue[4] = (Analogue[4] + analogRead(A4)) / 2 ; //delayMicroseconds(20);
  Analogue[5] = (Analogue[5] + analogRead(A5)) / 2 ; //delayMicroseconds(20);


}

void millis_to_timestamp() 

{
 
  int DD;
  int HH;
  int MM;
  int SS;
  unsigned long MS;

  MS = millis();
  SS = MS /1000 ;
  MM = SS / 60;
  HH = MM / 60;
  DD = HH / 24;
  
  sprintf(timestamp,"%02i:%02i:%02i.%03i, " , HH%24 , MM%60 , SS%60 , MS%1000 ) ;

}

