// Buttons
#define BTN_BL          0b0001
#define BTN_TL          0b0010
#define BTN_TR          0b0100
#define BTN_BR          0b1000

// Flipped
#define BTN_F_TR        0b0001
#define BTN_F_BR        0b0010
#define BTN_F_BL        0b0100
#define BTN_F_TL        0b1000

// Screen Dimensions
#define WIDTH           96
#define HEIGHT          64
#define FIBSH           5
#define FIBSW           8
#define NBYHEIGHT       (HEIGHT/FIBSH)
#define NBYWIDTH        (WIDTH/FIBSW)

// Comment the below define if screen is right side up
#define REVERSE
// Comment the below define to remove borders
#define BORDERS

#if NBYHEIGHT<NBYWIDTH
#define PIXPERFIB      NBYHEIGHT
#else
#define PIXPERFIB      NBYWIDTH
#endif

#define TOPMARGIN  (HEIGHT%(PIXPERFIB*FIBSH)>>1)
#define LEFTMARGIN (WIDTH%(PIXPERFIB*FIBSW)>>1)

// Box Locations
#define BOX1ATOP   (TOPMARGIN+(1*PIXPERFIB))
#define BOX1ALEFT  (LEFTMARGIN+(2*PIXPERFIB))
#define BOX1BTOP   (TOPMARGIN+(0*PIXPERFIB))
#define BOX1BLEFT  (LEFTMARGIN+(2*PIXPERFIB))
#define BOX2TOP    (TOPMARGIN+(0*PIXPERFIB))
#define BOX2LEFT   (LEFTMARGIN+(0*PIXPERFIB))
#define BOX3TOP    (TOPMARGIN+(2*PIXPERFIB))
#define BOX3LEFT   (LEFTMARGIN+(0*PIXPERFIB))
#define BOX5TOP    (TOPMARGIN+(0*PIXPERFIB))
#define BOX5LEFT   (LEFTMARGIN+(3*PIXPERFIB))

#define BOX1SIZE   PIXPERFIB
#define BOX2SIZE   (2*PIXPERFIB)
#define BOX3SIZE   (3*PIXPERFIB)
#define BOX5SIZE   (5*PIXPERFIB)
#define BOX1ABIT   0b00001
#define BOX1BBIT   0b00010
#define BOX2BIT    0b00100
#define BOX3BIT    0b01000
#define BOX5BIT    0b10000

#define NUMBOXES        5
#define DISPLAY_TIME    12500
#define DEBOUNCE_TIME   50
#define TOTAL_PALETTES  10

typedef enum _DisplayMode
{
  DMOff,
  DMFibTime,
  DMTextTime,
  DMMenu,
} DisplayMode;

typedef struct _ScreenColor
{
  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
} ScreenColor;

typedef struct _Palette
{
  ScreenColor Colors[4]; // off, hour, minute, both
} Palette;

typedef enum _PalNums
 {
  PAL_RGB,
  PAL_MONDRIAN,
  PAL_BASBRUN,
  PAL_80S,
  PAL_PASTEL,
  PAL_MODERN,
  PAL_COLD,
  PAL_WARM,
  PAL_EARTH,
  PAL_DARK,
 } PalNum;


