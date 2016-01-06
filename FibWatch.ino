#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <Timezone.h>

//#define DEBUG


// Colors
#define	BLACK           0x00
#define	BLUE            0xE0
#define	RED             0x03
#define	GREEN           0x1C
#define	DGREEN          0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define	BROWN           0x32

#define HOURCOLOR       RED
#define MINUTECOLOR     GREEN
#define BOTHCOLOR       BLUE
#define OFFCOLOR        WHITE

// Buttons
#define BTN_BL          0b0001
#define BTN_TL          0b0010
#define BTN_TR          0b0100
#define BTN_BR          0b1000

// Flipped
#define BTN_F_BL        0b0001
#define BTN_F_TL        0b0010
#define BTN_F_TR        0b0100
#define BTN_F_BR        0b1000

// Screen Dimensions
#define WIDTH           96
#define HEIGHT          64
#define FIBSH			5
#define FIBSW			8
#define NBYHEIGHT       (HEIGHT/FIBSH)
#define NBYWIDTH        (WIDTH/FIBSW)

// Comment the below define if screen is right side up
#define REVERSE
// Comment the below define to remove borders
#define BORDERS

#if NBYHEIGHT<NBYWIDTH
#define PixPerFib      NBYHEIGHT
#else
#define PixPerFib      NBYWIDTH
#endif

#define TopMargin (HEIGHT%(PixPerFib*FIBSH)>>1)
#define LeftMargin (WIDTH%(PixPerFib*FIBSW)>>1)

// Box Locations
#define BOX1ATOP (TopMargin+(1*PixPerFib))
#define BOX1ALEFT (LeftMargin+(2*PixPerFib))
#define BOX1BTOP (TopMargin+(0*PixPerFib))
#define BOX1BLEFT (LeftMargin+(2*PixPerFib))
#define BOX2TOP (TopMargin+(0*PixPerFib))
#define BOX2LEFT (LeftMargin+(0*PixPerFib))
#define BOX3TOP (TopMargin+(2*PixPerFib))
#define BOX3LEFT (LeftMargin+(0*PixPerFib))
#define BOX5TOP (TopMargin+(0*PixPerFib))
#define BOX5LEFT (LeftMargin+(3*PixPerFib))

#define BOX1SIZE PixPerFib
#define BOX2SIZE (2*PixPerFib)
#define BOX3SIZE (3*PixPerFib)
#define BOX5SIZE (5*PixPerFib)
#define BOX1ABIT 0b00001
#define BOX1BBIT 0b00010
#define BOX2BIT  0b00100
#define BOX3BIT  0b01000
#define BOX5BIT  0b10000

#define numcolors 4
#define numboxes 5
#define DISPLAY_TIME 12500
#define DEBOUNCE_TIME 50

uint8_t clockcolors[]={OFFCOLOR,HOURCOLOR,MINUTECOLOR,BOTHCOLOR};
uint8_t bits[]={BOX1ABIT,BOX1BBIT,BOX2BIT,BOX3BIT,BOX5BIT};
uint8_t shifts[]={0,1,2,3,4};
uint8_t lefts[numboxes]={BOX1ALEFT,BOX1BLEFT,BOX2LEFT,BOX3LEFT,BOX5LEFT};
uint8_t tops[numboxes]={BOX1ATOP,BOX1BTOP,BOX2TOP,BOX3TOP,BOX5TOP};
uint8_t sizes[numboxes]={BOX1SIZE,BOX1SIZE,BOX2SIZE,BOX3SIZE,BOX5SIZE};

// ways to display numbers
// arrays of bitmaps where the bits represent the state of boxes 5,3,2,1B,1A
uint8_t   zeros[]={0b00000};
uint8_t    ones[]={0b00010,0b00001};
uint8_t    twos[]={0b00100,0b00011};
uint8_t  threes[]={0b01000,0b00110,0b00101};
uint8_t   fours[]={0b01010,0b01001,0b00111};
uint8_t   fives[]={0b10000,0b01100,0b01011};
uint8_t   sixes[]={0b10010,0b10001,0b01110,0b01101};
uint8_t  sevens[]={0b10100,0b10011,0b01111};
uint8_t  eights[]={0b11000,0b10110,0b10101};
uint8_t   nines[]={0b11010,0b11001,0b10111};
uint8_t    tens[]={0b11100,0b11011};
uint8_t elevens[]={0b11110,0b11101};
uint8_t twelves[]={0b11111};

uint8_t *waystodraw[]={zeros,ones,twos,threes,fours,fives,sixes,sevens,eights,nines,tens,elevens,twelves};
uint8_t numwaystodraw[]={
	sizeof(zeros)/sizeof(uint8_t),
	sizeof(ones)/sizeof(uint8_t),
	sizeof(twos)/sizeof(uint8_t),
	sizeof(threes)/sizeof(uint8_t),
	sizeof(fours)/sizeof(uint8_t),
	sizeof(fives)/sizeof(uint8_t),
	sizeof(sixes)/sizeof(uint8_t),
	sizeof(sevens)/sizeof(uint8_t),
	sizeof(eights)/sizeof(uint8_t),
	sizeof(nines)/sizeof(uint8_t),
	sizeof(tens)/sizeof(uint8_t),
	sizeof(elevens)/sizeof(uint8_t),
	sizeof(twelves)/sizeof(uint8_t),
};

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

uint8_t ChooseRepresentation ( uint8_t number )
{
  uint8_t possibilities, whichway, *ways;
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

TinyScreen display = TinyScreen(0);

void FillFibBox(uint8_t box, uint8_t color)
{
  if (box<numboxes)
  {
#ifdef BORDERS
   	display.drawRect(lefts[box]+1,tops[box]+1,sizes[box]-2,sizes[box]-2,true,color);
#else
   	display.drawRect(lefts[box],tops[box],sizes[box],sizes[box],true,color);
#endif
  }
}

//Function to return the compile date and time as a time_t value
time_t compileTime(void)
{
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)

    char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[3], *m;
    int d, y;
    tmElements_t tm;
    time_t t;

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
  uint8_t hrsr,minsr,b;

  hrsr=ChooseRepresentation(hour(loc)%12);
  minsr=ChooseRepresentation(minute(loc)/5);

  for (b=0;b<numboxes;b++)
  {
  	uint8_t col,colnum,XXX,YYY;
  	XXX=(hrsr&bits[b])>>shifts[b];
  	YYY=((minsr&bits[b])>>shifts[b])<<1;
  	colnum=XXX+YYY;
    col=clockcolors[colnum];
  	FillFibBox(b,col);
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
  static uint8_t RememberedButtons;
  static bool ButtonInProgress;
  static unsigned long RememberedTime;
  uint8_t Buttons;
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

// Globals

typedef enum _DisplayMode
{
	DMOff,
	DMFibTime,
	DMTextTime,
	DMMenu,
} DisplayMode;

time_t utc;
bool Flipped;
bool BtnPressNoted;
unsigned long TimeDisplayOn;
uint8_t BtnDisplay;
uint8_t BtnFlip;
DisplayMode DM;

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
  BtnDisplay = BTN_BR;
  BtnFlip = BTN_BL;
#else
  BtnDisplay = BTN_TL;
  BtnFlip = BTN_TR;
#endif
  display.clearWindow(0,0,WIDTH,HEIGHT);
  DM=DMOff;
}

void loop()
{
  static time_t loc;
  static TimeChangeRule *tcr;
  uint8_t btn;

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
#ifdef DEBUG
  	Serial.println(DM);
#endif
  	if ( DM != DMOff ) display.off();
  	if ( Flipped )
  	{
      display.setFlip(0);
      Flipped=false;
      BtnDisplay = BTN_TL;
      BtnFlip = BTN_TR;
  	}
  	else
  	{
      display.setFlip(1);
      Flipped=true;
      BtnDisplay = BTN_BR;
      BtnFlip = BTN_BL;
  	}
  	if ( DM != DMOff )
  	{
#ifdef DEBUG
  	  Serial.println(DM);
#endif
      display.clearWindow(0,0,WIDTH,HEIGHT);
  	  if ( DM == DMFibTime ) DisplayTime(loc);
  	  else if ( DM == DMTextTime ) DisplayTextDateTime(loc,tcr);
  	  display.on();
  	  TimeDisplayOn = millis();
  	}
  }
  else if ( millis() - TimeDisplayOn > DISPLAY_TIME )
  {
    display.off();
    DM=DMOff;
  }
}
