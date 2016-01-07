#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <Timezone.h>
#include "FibWatch.h"

#ifdef USE_PMEM
#define PMEM PMEM
#else
#define PMEM
#endif

// ******************************   D  A  T  A   *****************************

// Globals
time_t        utc;
bool          Flipped, BtnPressNoted;
unsigned long TimeDisplayOn;
uint8_t       BtnDisplay, BtnFlip, BtnBrighter, BtnDimmer;
uint8_t       Bright;
DisplayMode   DM;
PalNum        Pal;
TinyScreen    display = TinyScreen(0);

// Tables
const Palette clockcolors[TOTAL_PALETTES] PMEM =
{ // These are copied from the Fibonacci Clock code    120 bytes
  // -- off  --   -- hours --   --minutes--   --  both  --
  // R   G   B     R   G   B     R   G   B     R   G   B
  {{{255,255,255},{255, 10, 10},{ 10,255, 10},{ 10, 10,255}}}, //RGB
  {{{255,255,255},{255, 10, 10},{248,222,  0},{ 10, 10,255}}}, //Mondrian
  {{{255,255,255},{ 80, 40,  0},{ 20,200, 20},{255,100, 10}}}, //Basbrun
  {{{255,255,255},{245,100,201},{114,247, 54},{113,235,219}}}, //80's
  {{{255,255,255},{255,123,123},{143,255,112},{120,120,255}}}, //Pastel
  {{{255,255,255},{212, 49, 45},{145,210, 49},{141, 95,224}}}, //Modern
  {{{255,255,255},{209, 62,200},{ 69,232,224},{ 80, 70,202}}}, //Cold
  {{{255,255,255},{237, 20, 20},{246,243, 54},{255,126, 21}}}, //Warm
  {{{255,255,255},{ 70, 35,  0},{ 70,122, 10},{200,182,  0}}}, //Earth
  {{{255,255,255},{211, 34, 34},{ 80,151, 78},{ 16, 24,149}}}, //Dark
 };

// following use 25 bytes
const uint8_t bits[]          PMEM ={BOX1ABIT,BOX1BBIT,BOX2BIT,BOX3BIT,BOX5BIT};
const uint8_t shifts[]        PMEM ={0,1,2,3,4};
const uint8_t lefts[numboxes] PMEM ={BOX1ALEFT,BOX1BLEFT,BOX2LEFT,BOX3LEFT,BOX5LEFT};
const uint8_t tops[numboxes]  PMEM ={BOX1ATOP,BOX1BTOP,BOX2TOP,BOX3TOP,BOX5TOP};
const uint8_t sizes[numboxes] PMEM ={BOX1SIZE,BOX1SIZE,BOX2SIZE,BOX3SIZE,BOX5SIZE};

// ways to display numbers -- 32 bytes
// arrays of bitmaps where the bits represent the state of boxes 5,3,2,1B,1A
const uint8_t   zeros[] PMEM ={0b00000};
const uint8_t    ones[] PMEM ={0b00010,0b00001};
const uint8_t    twos[] PMEM ={0b00100,0b00011};
const uint8_t  threes[] PMEM ={0b01000,0b00110,0b00101};
const uint8_t   fours[] PMEM ={0b01010,0b01001,0b00111};
const uint8_t   fives[] PMEM ={0b10000,0b01100,0b01011};
const uint8_t   sixes[] PMEM ={0b10010,0b10001,0b01110,0b01101};
const uint8_t  sevens[] PMEM ={0b10100,0b10011,0b01111};
const uint8_t  eights[] PMEM ={0b11000,0b10110,0b10101};
const uint8_t   nines[] PMEM ={0b11010,0b11001,0b10111};
const uint8_t    tens[] PMEM ={0b11100,0b11011};
const uint8_t elevens[] PMEM ={0b11110,0b11101};
const uint8_t twelves[] PMEM ={0b11111};

const uint8_t * const waystodraw[] PMEM ={zeros,ones,twos,threes,fours,fives,sixes,sevens,eights,nines,tens,elevens,twelves}; // 13 ptrs, 26 bytes
const uint8_t numwaystodraw[] PMEM ={
	sizeof(zeros)  /sizeof(uint8_t),
	sizeof(ones)   /sizeof(uint8_t),
	sizeof(twos)   /sizeof(uint8_t),
	sizeof(threes) /sizeof(uint8_t),
	sizeof(fours)  /sizeof(uint8_t),
	sizeof(fives)  /sizeof(uint8_t),
	sizeof(sixes)  /sizeof(uint8_t),
	sizeof(sevens) /sizeof(uint8_t),
	sizeof(eights) /sizeof(uint8_t),
	sizeof(nines)  /sizeof(uint8_t),
	sizeof(tens)   /sizeof(uint8_t),
	sizeof(elevens)/sizeof(uint8_t),
	sizeof(twelves)/sizeof(uint8_t),
}; // 13 bytes

// Time Zones
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

// ******************************   C  O  D  E   *****************************

uint8_t ChooseFibReb ( uint8_t number )
{
  uint8_t        possibilities, whichway;
  const uint8_t *ways;
  
  if ( number>12 ) return 0;
  possibilities=numwaystodraw[number];
  ways=waystodraw[number];
  if ( possibilities>1 )
  {
    whichway=random(possibilities);
  }
  else
  {
  	whichway=0;
  }
  return ways[whichway];
}

void FillFibBox(uint8_t box, ScreenColor *color)
{
  if (box<numboxes)
  {
#ifdef BORDERS
   	display.drawRect(lefts[box]+1,tops[box]+1,sizes[box]-2,sizes[box]-2,true,color->Red,color->Green,color->Blue);
#else
   	display.drawRect(lefts[box],tops[box],sizes[box],sizes[box],true,color->Red,color->Green,color->Blue);
#endif
  }
}

//Function to return the compile date and time as a time_t value
time_t compileTime(void)
{
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)

    char        *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char         chMon[3], *m;
    int          d, y;
    tmElements_t tm;
    time_t       t;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    t = makeTime(tm);
    return t + FUDGE /*- (5*3600)*/;        //add fudge factor to allow for compile time and TZO
}

void DisplayTime (time_t loc)
{
  uint8_t h,hrsr,minsr,b;

  h=hour(loc)%12;
  if (h==0)
  {
    h=12;
  }
  hrsr=ChooseFibReb(h);
  minsr=ChooseFibReb(minute(loc)/5);

  for (b=0;b<numboxes;b++)
  {
  	uint8_t colnum,XXX,YYY;
    ScreenColor col;
    Palette p;
    
  	XXX=(hrsr&bits[b])>>shifts[b];
  	YYY=((minsr&bits[b])>>shifts[b])<<1;
  	colnum=XXX+YYY;
    p=clockcolors[Pal];
    col=p.Colors[colnum];
  	FillFibBox(b,&col);
  }
}

void DisplayTextDateTime(time_t t, TimeChangeRule *tcr)
{
  	display.setCursor(7,15);
    display.print(dayShortStr(weekday(t)));
    display.print(' ');
    sPrintI00(day(t));
    display.print(' ');
    display.print(monthShortStr(month(t)));
    display.print(' ');
    display.print(year(t));
  	display.setCursor(27,30);
    sPrintI00(hourFormat12(t));
    sPrintDigits(minute(t));
    display.print(' ');
    display.print(isAM(t)?"AM":"PM");
  	display.setCursor(37,45);
  	display.print(tcr->abbrev);
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
    if (val < 10) Serial.print('0');
    display.print(val, DEC);
    return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
    display.print(':');
    if(val < 10) display.print('0');
    display.print(val, DEC);
}

uint8_t CheckButtons()
{
  // return all buttons pressed after all buttons released
  static uint8_t       RememberedButtons;
  static bool          ButtonInProgress;
  static unsigned long RememberedTime;
  uint8_t              Buttons;
  
  Buttons = display.getButtons();
  Buttons &= 0xF;
  if ( !Buttons && !RememberedTime )
  {
  	RememberedTime = millis();
  }
  else if ( !Buttons && ButtonInProgress && millis()-RememberedTime > DEBOUNCE_TIME )
  {
  	unsigned long orb;
    
  	ButtonInProgress = false;
  	orb = RememberedButtons;
  	RememberedButtons = 0;
  	return orb;
  }
  else if ( Buttons )
  {
  	RememberedButtons |= Buttons;
  	RememberedTime = 0;
  	ButtonInProgress = true;
  }
  return 0;
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif
  setTime(usET.toUTC(compileTime()));
  Wire.begin();
  display.begin();
  display.setFont(liberationSans_10ptFontInfo);
#ifdef REVERSE
  display.setFlip(1);
  Flipped = true;
  BtnDisplay =  BTN_F_TL;
  BtnFlip =     BTN_F_TR;
  BtnBrighter = BTN_F_BR;
  BtnDimmer =   BTN_F_BL;
#else
  BtnDisplay =  BTN_TL;
  BtnFlip =     BTN_TR;
  BtnBrighter = BTN_BR;
  BtnDimmer =   BTN_BL;
#endif
  display.clearWindow(0,0,WIDTH,HEIGHT);
  DM=DMOff;
  Pal=PAL_RGB;
  Bright=7;
  display.setBrightness(Bright);
}

void loop()
{
  static time_t          loc;
  static TimeChangeRule *tcr;
  uint8_t                btn;

  btn=CheckButtons();
  if ( (btn & BtnDisplay) && (btn & BtnFlip) )
  {
  	// 2-button "chord" -- cheat -- display text date/time
    display.clearWindow(0,0,WIDTH,HEIGHT);
  	TimeDisplayOn = millis();
    utc = now();
    loc = usET.toLocal(utc,&tcr);
  	DisplayTextDateTime(loc,tcr);
  	if ( DM == DMOff )
  	{
      display.on();
  	}
    DM=DMTextTime;
  }
  else if ( btn & BtnDisplay )
  {
  	TimeDisplayOn = millis();
    utc = now();
    loc = usET.toLocal(utc);
    display.clearWindow(0,0,WIDTH,HEIGHT);
    DisplayTime(loc);
  	if ( DM == DMOff )
  	{
      display.on();
  	}
    DM=DMFibTime;
  }
  else if ( btn & BtnFlip )
  {
  	if ( DM != DMOff ) display.off();
  	if ( Flipped )
  	{
      display.setFlip(0);
      Flipped=false;
      BtnDisplay =  BTN_TL;
      BtnFlip =     BTN_TR;
      BtnBrighter = BTN_BR;
      BtnDimmer =   BTN_BL;
  	}
  	else
  	{
      display.setFlip(1);
      Flipped=true;
      BtnDisplay =  BTN_F_TL;
      BtnFlip =     BTN_F_TR;
      BtnBrighter = BTN_F_BR;
      BtnDimmer =   BTN_F_BL;
  	}
  	if ( DM != DMOff )
  	{
      display.clearWindow(0,0,WIDTH,HEIGHT);
  	  if ( DM == DMFibTime ) DisplayTime(loc);
  	  else if ( DM == DMTextTime ) DisplayTextDateTime(loc,tcr);
  	  display.on();
  	  TimeDisplayOn = millis();
  	}
  }
  else if ( btn & BtnBrighter )
  {
    if ( Bright < 15 ) Bright++;
    display.setBrightness(Bright);
  }
  else if ( btn & BtnDimmer )
  {
    if ( Bright > 0 ) Bright--;
    display.setBrightness(Bright);
  }
  if ( millis() - TimeDisplayOn > DISPLAY_TIME )
  {
    display.off();
    DM=DMOff;
  }
}
