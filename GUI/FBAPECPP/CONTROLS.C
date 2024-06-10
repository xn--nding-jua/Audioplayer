#include "windows.h"

void initControls(HANDLE hInst, HWND hWnd)
{
	HWND hControl;
	int i;
	char c[2];

	// inputs
	CreateWindow("STATIC", "--- Stereo-Inputs ---", WS_VISIBLE | WS_CHILD, 24, 0, 120, 16, hWnd, 61847, hInst, NULL);

	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 24, 16, 16, 100, hWnd, (HMENU)IDC_VOL_ANALOG, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
	SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
	CreateWindow("STATIC", "-48.0", WS_VISIBLE | WS_CHILD, 14, 120, 64, 16, hWnd, (HMENU)IDC_VOL_ANALOG_DB, hInst, NULL);
	CreateWindow("STATIC", "Analog", WS_VISIBLE | WS_CHILD, 10, 136, 64, 16, hWnd, NULL, hInst, NULL);

	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 74, 16, 16, 100, hWnd, (HMENU)IDC_VOL_SDCARD, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
	SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
	CreateWindow("STATIC", "-48.0", WS_VISIBLE | WS_CHILD, 66, 120, 64, 16, hWnd, (HMENU)IDC_VOL_SDCARD_DB, hInst, NULL);
	CreateWindow("STATIC", "SD", WS_VISIBLE | WS_CHILD, 72, 136, 64, 16, hWnd, 61847, hInst, NULL);

	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 134, 16, 16, 100, hWnd, (HMENU)IDC_VOL_SPDIF, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
	SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
	CreateWindow("STATIC", "-48.0", WS_VISIBLE | WS_CHILD, 124, 120, 64, 16, hWnd, (HMENU)IDC_VOL_SPDIF_DB, hInst, NULL);
	CreateWindow("STATIC", "SPDIF", WS_VISIBLE | WS_CHILD, 120, 136, 64, 16, hWnd, 61847, hInst, NULL);

             
    // output
	CreateWindow("STATIC", "Main-Outputs", WS_VISIBLE | WS_CHILD, 188, 0, 120, 16, hWnd, NULL, hInst, NULL);

	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 194, 16, 16, 100, hWnd, (HMENU)IDC_VOL_MAIN, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
	SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
	CreateWindow("STATIC", "-48.0", WS_VISIBLE | WS_CHILD, 186, 120, 64, 16, hWnd, (HMENU)IDC_VOL_MAIN_DB, hInst, NULL);
	CreateWindow("STATIC", "Main", WS_VISIBLE | WS_CHILD, 186, 136, 64, 16, hWnd, NULL, hInst, NULL);

	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 254, 16, 16, 100, hWnd, (HMENU)IDC_VOL_SUB, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
	SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
	CreateWindow("STATIC", "-48.0", WS_VISIBLE | WS_CHILD, 248, 120, 64, 16, hWnd, (HMENU)IDC_VOL_SUB_DB, hInst, NULL);
	CreateWindow("STATIC", "Sub", WS_VISIBLE | WS_CHILD, 248, 136, 64, 16, hWnd, NULL, hInst, NULL);
        
    
    // ultranet inputs    
	CreateWindow("STATIC", "--- Behringer Ultranet Input Channels ---", WS_VISIBLE | WS_CHILD, 24, 156, 280, 16, hWnd, 61847, hInst, NULL);
	for (i=1; i<=16; i++) {
		hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, 24+(i-1)*16, 175, 16, 100, hWnd, (HMENU)(IDC_VOL_ULTRANET+(i-1)), hInst, NULL);
		SetScrollRange(hControl, SB_CTL, 0, 540, TRUE); // -48dBfs ... +6dBfs
		SetScrollPos(hControl, SB_CTL, 540, TRUE); // -48dBfs
		sprintf(c, "%i", i);
		CreateWindow("STATIC", c, WS_VISIBLE | WS_CHILD, 24+(i-1)*16, 275, 16, 16, hWnd, NULL, hInst, NULL);
	} 
	
    // sync-source and samplerate
	CreateWindow("STATIC", "---------------- Configuration ----------------", WS_VISIBLE | WS_CHILD, 16, 300, 300, 16, hWnd, NULL, hInst, NULL);

	CreateWindow("STATIC", "Sync:", WS_VISIBLE | WS_CHILD, 16, 320, 50, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 120, 320, 100, 7*20, hWnd, (HMENU)IDC_SYNCSOURCE, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Analog");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"SD Card");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Bluetooth");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"SPDIF");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Ultranet A");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Ultranet B");
	SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_SETCURSEL, 0, 0);

	CreateWindow("STATIC", "Samplerate:", WS_VISIBLE | WS_CHILD, 16, 350, 80, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 120, 350, 100, 4*20, hWnd, (HMENU)IDC_SAMPLERATE, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_SAMPLERATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"44.1 kHz");
	SendDlgItemMessage(hWnd, IDC_SAMPLERATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"48 kHz");
	SendDlgItemMessage(hWnd, IDC_SAMPLERATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"96 kHz");
	SendDlgItemMessage(hWnd, IDC_SAMPLERATE, CB_SETCURSEL, 1, 0);

	CreateWindow("STATIC", "---------------- Communication ----------------", WS_VISIBLE | WS_CHILD, 16, 380, 300, 16, hWnd, NULL, hInst, NULL);

	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 16, 400, 65, 5*20, hWnd, (HMENU)IDC_COMPORT, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM1");
	SendDlgItemMessage(hWnd, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM2");
	SendDlgItemMessage(hWnd, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM3");
	SendDlgItemMessage(hWnd, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM4");
	SendDlgItemMessage(hWnd, IDC_COMPORT, CB_SETCURSEL, 0, 0);

	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 90, 400, 80, (UINT)(5.5f*20), hWnd, (HMENU)IDC_BAUDRATE, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"9600");
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"19200");
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"38400");
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"57600");
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"115200");
	SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_SETCURSEL, 4, 0);

	CreateWindow("BUTTON","Connect",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 180, 400, 90, 25, hWnd, IDC_CONNECT, hInst, NULL);
	CreateWindow("BUTTON","Disconnect",WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON, 180, 400, 90, 25, hWnd, IDC_DISCONNECT, hInst, NULL);

	// playcontrol
	CreateWindow("STATIC", "---------------- Playcontrol ----------------", WS_VISIBLE | WS_CHILD, 16, 425, 300, 16, hWnd, NULL, hInst, NULL);
	CreateWindow("STATIC", "File:", WS_VISIBLE | WS_CHILD, 16, 443, 35, 16, hWnd, NULL, hInst, NULL);
	CreateWindow("EDIT","Test.mp3",WS_TABSTOP | WS_VISIBLE | WS_CHILD, 55, 443, 150, 25, hWnd, IDC_FILENAME, hInst, NULL);
	CreateWindow("BUTTON","Load",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 208, 443, 60, 25, hWnd, IDC_LOAD, hInst, NULL);
	
	CreateWindow("BUTTON","Play",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 16, 470, 60, 25, hWnd, IDC_PLAY, hInst, NULL);
	CreateWindow("BUTTON","Pause",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 80, 470, 60, 25, hWnd, IDC_PAUSE, hInst, NULL);
	CreateWindow("BUTTON","Stop",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 144, 470, 60, 25, hWnd, IDC_STOP, hInst, NULL);
	CreateWindow("BUTTON","Loop",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 208, 470, 60, 25, hWnd, IDC_LOOP, hInst, NULL);
	

	// EQ
	CreateWindow("STATIC", "------------------------ Equalizer ------------------------", WS_VISIBLE | WS_CHILD, 290, 0, 300, 16, hWnd, NULL, hInst, NULL);

	CreateWindow("STATIC", "Type:", WS_VISIBLE | WS_CHILD, 290, 22, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 360, 22, 150, 8*20, hWnd, (HMENU)IDC_EQ_TYPE, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Allpass");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Peak");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"LowShelf");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"HighShelf");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Bandpass");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Notch");                                                                         
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"LowPass");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"HighPass");
	SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_SETCURSEL, 0, 0);

	CreateWindow("STATIC", "Freq:", WS_VISIBLE | WS_CHILD, 290, 52, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 52, 150, 16, hWnd, (HMENU)IDC_EQ_FREQ, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 10000, TRUE);
	SetScrollPos(hControl, SB_CTL, 1000, TRUE);
	CreateWindow("STATIC", "1 kHz", WS_VISIBLE | WS_CHILD, 520, 52, 100, 16, hWnd, (HMENU)IDC_LBL_FREQ, hInst, NULL);

	CreateWindow("STATIC", "Q-Factor:", WS_VISIBLE | WS_CHILD, 290, 72, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 72, 150, 16, hWnd, (HMENU)IDC_EQ_Q, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 30, 1000, TRUE);
	SetScrollPos(hControl, SB_CTL, 200, TRUE);
	CreateWindow("STATIC", "2", WS_VISIBLE | WS_CHILD, 520, 72, 100, 16, hWnd, (HMENU)IDC_LBL_Q, hInst, NULL);

	CreateWindow("STATIC", "Gain:", WS_VISIBLE | WS_CHILD, 290, 92, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 92, 150, 16, hWnd, (HMENU)IDC_EQ_GAIN, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 10000, TRUE);
	SetScrollPos(hControl, SB_CTL, 5000, TRUE);
	CreateWindow("STATIC", "0 dB", WS_VISIBLE | WS_CHILD, 520, 92, 100, 16, hWnd, (HMENU)IDC_LBL_GAIN, hInst, NULL);

	CreateWindow("BUTTON","EQ1",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 310, 120, 40, 20, hWnd, IDC_EQ_SET1, hInst, NULL);
	CreateWindow("BUTTON","EQ2",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 360, 120, 40, 20, hWnd, IDC_EQ_SET2, hInst, NULL);
	CreateWindow("BUTTON","EQ3",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 410, 120, 40, 20, hWnd, IDC_EQ_SET3, hInst, NULL);
	CreateWindow("BUTTON","EQ4",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 460, 120, 40, 20, hWnd, IDC_EQ_SET4, hInst, NULL);
	CreateWindow("BUTTON","EQ5",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 510, 120, 40, 20, hWnd, IDC_EQ_SET5, hInst, NULL);

	// dynamic compressor
	CreateWindow("STATIC", "--------------- Dynamic compressor ---------------", WS_VISIBLE | WS_CHILD, 290, 150, 300, 16, hWnd, NULL, hInst, NULL);

	CreateWindow("STATIC", "Thresh:", WS_VISIBLE | WS_CHILD, 290, 22+150, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 22+150, 150, 16, hWnd, (HMENU)IDC_COMP_THRESH, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 600, TRUE);
	SetScrollPos(hControl, SB_CTL, 600, TRUE);
	CreateWindow("STATIC", "0 dB", WS_VISIBLE | WS_CHILD, 520, 22+150, 100, 16, hWnd, (HMENU)IDC_LBL_THRESH, hInst, NULL);

	CreateWindow("STATIC", "Ratio:", WS_VISIBLE | WS_CHILD, 290, 52+150, 48, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 360, 52+150, 75, 8*20, hWnd, (HMENU)IDC_COMP_RATIO, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:1");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:2");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:4");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:8");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:16");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:32");                                                                         
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1:64");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Limiter");
	SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_SETCURSEL, 0, 0);

	CreateWindow("STATIC", "Makeup:", WS_VISIBLE | WS_CHILD, 445, 52+150, 55, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 510, 52+150, 75, 9*20, hWnd, (HMENU)IDC_COMP_MAKEUP, hInst, NULL);
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"0dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+6dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+12dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+18dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+24dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+30dB");                                                                         
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+36dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+42dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"+48dB");
	SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_SETCURSEL, 0, 0);

	CreateWindow("STATIC", "Attack:", WS_VISIBLE | WS_CHILD, 290, 82+150, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 82+150, 150, 16, hWnd, (HMENU)IDC_COMP_ATTACK, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 82+150, 100, 16, hWnd, (HMENU)IDC_LBL_ATTACK, hInst, NULL);

	CreateWindow("STATIC", "Hold:", WS_VISIBLE | WS_CHILD, 290, 102+150, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 102+150, 150, 16, hWnd, (HMENU)IDC_COMP_HOLD, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 102+150, 100, 16, hWnd, (HMENU)IDC_LBL_HOLD, hInst, NULL);

	CreateWindow("STATIC", "Release:", WS_VISIBLE | WS_CHILD, 290, 122+150, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 122+150, 150, 16, hWnd, (HMENU)IDC_COMP_RELEASE, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 122+150, 100, 16, hWnd, (HMENU)IDC_LBL_RELEASE, hInst, NULL);


	// Noisegate
	CreateWindow("STATIC", "------------------------ Noise gate ------------------------", WS_VISIBLE | WS_CHILD, 290, 300, 300, 16, hWnd, NULL, hInst, NULL);

	CreateWindow("STATIC", "Thresh:", WS_VISIBLE | WS_CHILD, 290, 22+300, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 22+300, 150, 16, hWnd, (HMENU)IDC_GATE_THRESH, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 600, TRUE);
	SetScrollPos(hControl, SB_CTL, 0, TRUE);
	CreateWindow("STATIC", "-60 dB", WS_VISIBLE | WS_CHILD, 520, 22+300, 100, 16, hWnd, (HMENU)IDC_LBL_GTHRESH, hInst, NULL);
			
	CreateWindow("STATIC", "Range:", WS_VISIBLE | WS_CHILD, 290, 42+300, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 42+300, 150, 16, hWnd, (HMENU)IDC_GATE_RANGE, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 600, TRUE);
	SetScrollPos(hControl, SB_CTL, 600, TRUE);
	CreateWindow("STATIC", "60 dB", WS_VISIBLE | WS_CHILD, 520, 42+300, 100, 16, hWnd, (HMENU)IDC_LBL_GRANGE, hInst, NULL);

	CreateWindow("STATIC", "Attack:", WS_VISIBLE | WS_CHILD, 290, 62+300, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 62+300, 150, 16, hWnd, (HMENU)IDC_GATE_ATTACK, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 62+300, 100, 16, hWnd, (HMENU)IDC_LBL_GATTACK, hInst, NULL);

	CreateWindow("STATIC", "Hold:", WS_VISIBLE | WS_CHILD, 290, 82+300, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 82+300, 150, 16, hWnd, (HMENU)IDC_GATE_HOLD, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 82+300, 100, 16, hWnd, (HMENU)IDC_LBL_GHOLD, hInst, NULL);

	CreateWindow("STATIC", "Release:", WS_VISIBLE | WS_CHILD, 290, 102+300, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 102+300, 150, 16, hWnd, (HMENU)IDC_GATE_RELEASE, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 2000, TRUE);
	SetScrollPos(hControl, SB_CTL, 100, TRUE);
	CreateWindow("STATIC", "100 ms", WS_VISIBLE | WS_CHILD, 520, 102+300, 100, 16, hWnd, (HMENU)IDC_LBL_GRELEASE, hInst, NULL);


	// Crossover
	CreateWindow("STATIC", "------------------------ Crossover ------------------------", WS_VISIBLE | WS_CHILD, 290, 430, 300, 16, hWnd, NULL, hInst, NULL);

	CreateWindow("STATIC", "Main:", WS_VISIBLE | WS_CHILD, 290, 450, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 450, 150, 16, hWnd, (HMENU)IDC_LOWCUT, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 10000, TRUE);
	SetScrollPos(hControl, SB_CTL, 2700, TRUE);
	CreateWindow("STATIC", "110 Hz", WS_VISIBLE | WS_CHILD, 520, 450, 100, 16, hWnd, (HMENU)IDC_LBL_LOWCUT, hInst, NULL);

	CreateWindow("STATIC", "Sub:", WS_VISIBLE | WS_CHILD, 290, 470, 64, 16, hWnd, NULL, hInst, NULL);
	hControl = CreateWindow("SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_HORZ, 360, 470, 150, 16, hWnd, (HMENU)IDC_HIGHCUT, hInst, NULL);
	SetScrollRange(hControl, SB_CTL, 0, 10000, TRUE);
	SetScrollPos(hControl, SB_CTL, 2700, TRUE);
	CreateWindow("STATIC", "110 Hz", WS_VISIBLE | WS_CHILD, 520, 470, 100, 16, hWnd, (HMENU)IDC_LBL_HIGHCUT, hInst, NULL);
}