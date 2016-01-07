// Colors
#define TOTAL_PALETTES 10
#define BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN          0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

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
#define FIBSH     5
#define FIBSW     8
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

