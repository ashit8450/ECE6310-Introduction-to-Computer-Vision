
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

		// Display flags
int		ShowPixelCoords;
int     color_r;
int     color_b;
int     color_g;
int     color_y;
int     p_mode;
int		s_mode;
int     ans; 
int     k;
int     flag_p;
int     predicate1;
int     predicate2;
BOOL    flag;
		// Image data
unsigned char	*OriginalImage;
int				ROWS,COLS;

#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK DialogCallback(HWND, UINT, WPARAM, LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void ExplosionThread(void *);		/* passes address of window */
void Segment(void*);		/* passes address of window */
void RegionGrow(void*);		/* passes address of window */