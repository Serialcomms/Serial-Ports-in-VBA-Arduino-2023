/*
  6-channel Analogue Bar Chart terminal plotter.
  ==============================================

  Use with ANSI/VT100/UTF-8 compatible terminal emulator only.
  
  Runs continually and updates screen and inbuilt LED every second.
  
  Sketch will not work as intended with Arduino IDE Serial Monitor or Plotter.


  Sample output
  =============

  ╔═════════════════════════════════════════════════════════════════════════════╗
  ║                                                                 ▐██████████▌║
  ║                                                                 ▐██████████▌║
  ║                                                                 ▐██████████▌║
  ║▐██████████▌                                                     ▐██████████▌║
  ║▐██████████▌                                                     ▐██████████▌║
  ║▐██████████▌              ▐██████████▌                           ▐██████████▌║
  ║▐██████████▌              ▐██████████▌                           ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ║▐██████████▌              ▐██████████▌ ▐██████████▌ ▐██████████▌ ▐██████████▌║
  ╠════════════╤════════════╤════════════╤════════════╤════════════╤════════════╣
  ║    0957    │    0000    │    0824    │    0746    │    0747    │    1021    ║
  ╚════════════╧════════════╧════════════╧════════════╧════════════╧════════════╝

  Adjust terminal emulator font settings for artefact-free display. 
  Lucida Console (16-point, non anti-aliased) works well with PuTTy.

*/

// User-defined settings
// =================================================================
bool ANALOGUE_PULLUPS = false;      // sets all analogue inputs
unsigned int BAR_HEIGHT = 20;       // recommended value = 20 or 30
// =================================================================

unsigned int SCREEN_ROW;
unsigned int CHANNEL_NUM;

char analoguevals[160];
unsigned int Analogue[6];
unsigned long NextToggle;

const char BOX_TOP[] PROGMEM = "╔═════════════════════════════════════════════════════════════════════════════╗";
const char BOX_MID[] PROGMEM = "╠════════════╤════════════╤════════════╤════════════╤════════════╤════════════╣";
const char BOX_END[] PROGMEM = "╚════════════╧════════════╧════════════╧════════════╧════════════╧════════════╝";

const char BOX_VERTICAL[] = "║";

const char BLOCK_LIT[] = "▐██████████▌";    // three separate block characters : 1 * half-right block, 10 * full-blocks, 1* half-left block
const char BLOCK_OFF[] = "            ";    // 12 spaces

const char CURSOR_HOME[]="\x1B[H";
const String ASCII_ESCAPE = "\x1B";


void setup() {

  Serial.begin(115200, SERIAL_8N1);
  pinMode(LED_BUILTIN, OUTPUT);
  SetAnalogueInputs();
  delay(100);
  ClearScreen();
 
  NextToggle = (1000 * (millis() / 1000)) + 1000;

}

void loop() {

  if (millis() < NextToggle)

  {
    Read_Analogue_Inputs();
  }

  else

  {

    Build_VT100_Screen();

    Toggle_Builtin_LED();

    NextToggle = (1000 * millis() / 1000) + 1000;

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

void Build_VT100_Screen()

{
  
  Serial.print(CURSOR_HOME);        // cursor home top-left
 
  PrintFlashString(BOX_TOP,true);

  for (SCREEN_ROW = 0; SCREEN_ROW < BAR_HEIGHT  ; SCREEN_ROW++)

  {

    Serial.print(BOX_VERTICAL);

    for (CHANNEL_NUM = 0; CHANNEL_NUM < 6; CHANNEL_NUM++)

    {

      DrawBarBlock(CHANNEL_NUM, SCREEN_ROW);

      if (CHANNEL_NUM < 5) { Serial.print(" "); }

    }

    Serial.println(BOX_VERTICAL);

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

  Serial.print(ASCII_ESCAPE + "[H");        // cursor home
  Serial.print(ASCII_ESCAPE + "[2J");       // clear screen
  Serial.print(ASCII_ESCAPE + "[3J");       // clear scrollback
  Serial.print(ASCII_ESCAPE + "[1m");       // bold text
  Serial.print(ASCII_ESCAPE + "[36m");      // cyan text
  Serial.print(ASCII_ESCAPE + "[40m");      // black background
  Serial.print(ASCII_ESCAPE + "[?25l");     // hide cursor (reduce flicker)

  /*

  Serial.print(ASCII_ESCAPE + "[?25h");     // show cursor 
  
  Serial.print(ASCII_ESCAPE + "[0m");       // default settings
  Serial.print(ASCII_ESCAPE + "[1m");       // text bold = on
  Serial.print(ASCII_ESCAPE + "[2m");       // text bold = off

  Serial.print(ASCII_ESCAPE + "[31m");      // text = red
  Serial.print(ASCII_ESCAPE + "[32m");      // text = green
  Serial.print(ASCII_ESCAPE + "[33m");      // text = yellow
  Serial.print(ASCII_ESCAPE + "[34m");      // text = blue
  Serial.print(ASCII_ESCAPE + "[35m");      // text = magenta
  Serial.print(ASCII_ESCAPE + "[36m");      // text = cyan
  Serial.print(ASCII_ESCAPE + "[37m");      // text = white

  */
}

void DrawBarBlock(int CHANNEL, int ROW)

{

if (Analogue[CHANNEL] / (1020/BAR_HEIGHT) >= BAR_HEIGHT - ROW)

  {
    Serial.print(BLOCK_LIT);
  }

else 

  {
    Serial.print(BLOCK_OFF);
  }
}

void PrintFlashString(const char * FLASH_STRING, bool LINE_END)

{
  Serial.print( (__FlashStringHelper *) FLASH_STRING) ;

  if(LINE_END) {Serial.println(); }
}

