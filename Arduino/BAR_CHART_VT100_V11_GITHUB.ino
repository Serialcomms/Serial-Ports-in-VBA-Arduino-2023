/*
  6-channel Analogue Bar Chart Plotter
  Sends VT100-formatted results to serial port.
  Use with terminal emulator only. 
  Does not work with IDE Serial Monitor or Plotter.

*/

int SCREEN_ROW;
int CHANNEL_NUM;
bool DRAW_BLOCK;

char analoguevals[160];
unsigned int Analogue[6];
unsigned long NextToggle;
unsigned long milliSeconds;

const char BOX_TOP[] PROGMEM = "╔═════════════════════════════════════════════════════════════════════════════╗";
const char BOX_MID[] PROGMEM = "╠════════════╤════════════╤════════════╤════════════╤════════════╤════════════╣";
const char BOX_END[] PROGMEM = "╚════════════╧════════════╧════════════╧════════════╧════════════╧════════════╝";

const char BLOCK_LIT[] = "▐██████████▌";    // three separate block characters used here : 1 * half-right block , 10 * full-blocks, 1* half-left block
const char BLOCK_OFF[] = "            ";    // 12 spaces

const char CURSOR_HOME[]="\x1B[H";
const String ASCII_ESCAPE = "\x1B";

// User settings
bool ANALOGUE_PULLUPS = false;      // sets all analogue inputs
unsigned int BAR_HEIGHT = 20;       // recommended value = 20 or 30
// User settings

void setup() {

  Serial.setTimeout(0);
  Serial.begin(115200, SERIAL_8N1);
  delay(100);
  ClearScreen();
  SetAnalogueInputs();
  pinMode(LED_BUILTIN, OUTPUT);

  NextToggle = (1000 * (millis() / 1000)) + 1000;
}

void loop() {

  milliSeconds = millis();

  if (milliSeconds < NextToggle)

  {
    Read_Analogue_Inputs();
  }

  else

  {

    Build_VT100_Screen();

    Toggle_Builtin_LED();

    NextToggle = (1000 * milliSeconds / 1000) + 1000;

  }
}

void Toggle_Builtin_LED()

{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void Read_Analogue_Inputs() {


  delay(50);
  Analogue[0] = (Analogue[0] + analogRead(A0)) / 2;

  delay(50);
  Analogue[1] = (Analogue[1] + analogRead(A1)) / 2;

  delay(50);
  Analogue[2] = (Analogue[2] + analogRead(A2)) / 2;

  delay(50);
  Analogue[3] = (Analogue[3] + analogRead(A3)) / 2;

  delay(50);
  Analogue[4] = (Analogue[4] + analogRead(A4)) / 2;

  delay(50);
  Analogue[5] = (Analogue[5] + analogRead(A5)) / 2;

}

String Build_BAR_BLOCK(bool DRAW_BLOCK)

{
  if (DRAW_BLOCK)

  {
   return BLOCK_LIT;
  }

  else

  {
   return BLOCK_OFF;
  }
}


void Build_VT100_Screen()

{
  
  Serial.print(CURSOR_HOME);        // cursor home top-left
  
  PrintFlashString(BOX_TOP,true);
 
  for (SCREEN_ROW = 1; SCREEN_ROW < BAR_HEIGHT + 1 ; SCREEN_ROW++)

  {

    Serial.print("║");

    for (CHANNEL_NUM = 0; CHANNEL_NUM < 6; CHANNEL_NUM++)

    {

      DRAW_BLOCK = (((Analogue[CHANNEL_NUM] / BAR_HEIGHT) > (BAR_HEIGHT - SCREEN_ROW)) ? true : false);

      Serial.print(Build_BAR_BLOCK(DRAW_BLOCK));

      if (CHANNEL_NUM < 5) { Serial.print(" "); }

    }

    Serial.println("║");

  }
 
    PrintFlashString(BOX_MID,true);

    sprintf(analoguevals, "║    %04d    │    %04d    │    %04d    │    %04d    │    %04d    │    %04d    ║", Analogue[0], Analogue[1], Analogue[2], Analogue[3], Analogue[4], Analogue[5]);

    Serial.println(analoguevals);

    PrintFlashString(BOX_END,false);
   
}

void SetAnalogueInputs() {

  pinMode(A0, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
  pinMode(A1, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
  pinMode(A2, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
  pinMode(A3, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
  pinMode(A4, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
  pinMode(A5, ANALOGUE_PULLUPS ? INPUT_PULLUP : INPUT);
}

void ClearScreen()

{

  Serial.print(ASCII_ESCAPE + "[H");      // cursor home
  Serial.print(ASCII_ESCAPE + "[2J");     // clear screen
  Serial.print(ASCII_ESCAPE + "[3J");     // clear scrollback
  Serial.print(ASCII_ESCAPE + "[1m");     // bold text
  Serial.print(ASCII_ESCAPE + "[36m");    // cyan text
  Serial.print(ASCII_ESCAPE + "[40m");    // black background
  
 // Serial.print(ASCII_ESCAPE + "[31m");
  Serial.print(ASCII_ESCAPE + "[?25l");   // hide cursor to reduce flicker
 
}

void PrintFlashString(const char * FLASH_STRING, bool LINE_END)

{

  Serial.print( (__FlashStringHelper *) FLASH_STRING) ;

  if(LINE_END) {Serial.println(); }
  
}

