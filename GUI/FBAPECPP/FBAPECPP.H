#include "resource.h"

#ifndef WINVER
/* Pre-3.1 windows.h */

#define CALLBACK _far _pascal
typedef unsigned int UINT;
typedef UINT WPARAM;
typedef LONG LPARAM;   

#endif

HANDLE hInst;
RECT gAppWindowSize;
int g_updateVolumes;
BOOL g_recalcEQ;
BOOL g_updateEQ;
BOOL g_recalcCOMP;
BOOL g_updateCOMP; 
BOOL g_recalcGATE;
BOOL g_updateGATE;
BOOL g_recalcCROSSOVER;
int g_currentEQ;
int g_currentCOMP;

struct sEQ {             
	WORD eq_type;
	float eq_freq;
	float eq_q;
	float eq_gain;
} EQs[5];

struct sCOMP {
	float threshold;
	WORD ratio;
	WORD makeup;
	float attack;
	float hold;
	float release;
} COMPs[1];

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long CALLBACK __export MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL __export CALLBACK VolumeDlgProc(HWND, unsigned, WORD, LONG);
BOOL __export CALLBACK AboutDlgProc(HWND, unsigned, WORD, LONG);

/* Color defines */
#define RED_COLORREF RGB( 255, 0, 0 )
#define GREEN_COLORREF RGB( 0, 255, 0 )
#define BLUE_COLORREF RGB( 0, 0, 255 )

/* General defines */
#define FBAPE_MAIN_TIMER IDT_FBAPE_MAIN
