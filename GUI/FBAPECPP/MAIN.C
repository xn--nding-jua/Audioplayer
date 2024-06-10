/****************************************************************************
    f/bape Config Tool v1.0.3 built on 20.05.2024 - 09.05.2024
    (c) 2024 by Dr.-Ing. Christian Nöding
    https://www.github.com/xn--nding-jua/Audioplayer
    
    This application is just for fun and a test for my C++ skills under
    Windows 3.x. The main goal was to get the f/bape Config Tool running
    under OS/2 Warp 3 using the WLO (Windows Library for OS/2) to run
    the software natively under OS/2. It took me a couple of days to find
    the right compiler and the right setup. First I tried to patch the
    original f/bape-EXE compiled with Borland Delphi 1. But Delphi 1 was
    created in 1995 - way too yound, as it uses functions from KERNEL.DLL,
    USER.DLL and GDI.DLL that were not available when WLO was released.
    After this I tested Lazarus with cross-compiling from Win32 to Win16,
    but I had problems getting the cross-compiler to run.
    Finally I installed Microsoft Visual C++ v1.52 and wrote this app here.
    Quite crazy you thing? Yes it is - but it was fun 8-)
    
    More information about ancient programming see
    https://www.transmissionzero.co.uk/computing/win16-apps-in-c

****************************************************************************/

#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "FBAPECPP.h"

#include "dialogs.c"
#include "controls.c"
#include "uart.c"
#include "math.c"

// main-entry-function for Windows-Application
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    if (!hPrevInstance)	// check if other instance is running
    if (!InitApplication(hInstance)) // init shared things
        return (FALSE); // error occured

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)){
	    TranslateMessage(&msg); // virtual key-codes
	    DispatchMessage(&msg); // dispatch messages to window
    }
    return (msg.wParam);
}

// init function
BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_APPICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE);
    wc.lpszMenuName =  "MainMenu";
    wc.lpszClassName = "GenericWClass";

    return (RegisterClass(&wc));
}

BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
    HWND            hWnd;
    hInst = hInstance;

    // create main-window
    hWnd = CreateWindow(
        "GenericWClass",
        "f/bape Config Tool",   		// text in title bar
        WS_OVERLAPPEDWINDOW,            // window-style
        CW_USEDEFAULT,                  // x-position
        CW_USEDEFAULT,                  // y-position
        600,//CW_USEDEFAULT,            // width
        550,//CW_USEDEFAULT,            // heigth
        NULL,                           // overlapped window has no parent
        NULL,                           // use window class menu
        hInstance,                      // owner
        NULL                            // pointer
    );

    if (!hWnd)
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return (TRUE);
}
                 
void DrawMainWindow(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;

	// begin painting
	hdc = BeginPaint(hwnd, &ps);

/*
    // objects
	HPEN hPenNull, hPenBlack;
	HBRUSH hbrRed;
	          
	// initialization
    hPenNull = GetStockObject(NULL_PEN);
	hPenBlack = GetStockObject(BLACK_PEN);
	hbrRed = CreateSolidBrush( RGB( 255, 0, 0) );
	
	// get size of current window
    GetClientRect(hwnd, &gAppWindowSize);
                                 

	// draw a rectangle
    SelectObject(hdc, hbrRed);
	Rectangle(hdc, 100, 100, 200, 200);	// handle, x1, y1, x2, y2
	                          
	// draw a line
	SelectObject(hdc, hPenBlack);
	MoveTo(hdc, 0, 0); // handle, x, y
	LineTo(hdc, 75, 75); // handle, x, y
	


	// destruct objects
	DeleteObject(hPenNull);
	DeleteObject(hPenBlack);
	DeleteObject(hbrRed);
*/
	// end painting
	EndPaint(hwnd, &ps);
}                 

void TimerCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{           
	char s[50];
	float value;
	float gate_threshold;
	float gate_range;
	float gate_attack_ms;
	float gate_hold_ms;
	float gate_release_ms;
	float crossover;

	// updateVolumes
	if (g_updateVolumes>0) {
		if (g_updateVolumes == 1) {
		    value = (60.0f - GetScrollPos(GetDlgItem(hWnd, IDC_VOL_ANALOG), SB_CTL)) / 10.0f;
			SendVolume(1, value);
			SendVolume(2, value);

			sprintf(s, "%+.*f", 1, value);
			SetDlgItemText(hWnd, IDC_VOL_ANALOG_DB, s);
		}else if (g_updateVolumes == 3) {
		    value = (60.0f - GetScrollPos(GetDlgItem(hWnd, IDC_VOL_SDCARD), SB_CTL)) / 10.0f;
			SendVolume(3, value);
			SendVolume(4, value);

			sprintf(s, "%+.*f", 1, value);
			SetDlgItemText(hWnd, IDC_VOL_SDCARD_DB, s);
		}else if (g_updateVolumes == 5) {
		    value = (60.0f - GetScrollPos(GetDlgItem(hWnd, IDC_VOL_SPDIF), SB_CTL)) / 10.0f;
			SendVolume(5, value);
			SendVolume(6, value);

			sprintf(s, "%+.*f", 1, value);
			SetDlgItemText(hWnd, IDC_VOL_SPDIF_DB, s);
		}else if (g_updateVolumes == 100) {
		    value = (60.0f - GetScrollPos(GetDlgItem(hWnd, IDC_VOL_MAIN), SB_CTL)) / 10.0f;
			SendVolume(100, value);

			sprintf(s, "%+.*f", 1, value);
			SetDlgItemText(hWnd, IDC_VOL_MAIN_DB, s);
		}else if (g_updateVolumes == 101) {
		    value = (60.0f - GetScrollPos(GetDlgItem(hWnd, IDC_VOL_SUB), SB_CTL)) / 10.0f;
			SendVolume(101, value);

			sprintf(s, "%+.*f", 1, value);
			SetDlgItemText(hWnd, IDC_VOL_SUB_DB, s);
		}                   
		
		g_updateVolumes = 0;
	}
                                      
	// UpdateEQ
	if (g_recalcEQ) {
		g_recalcEQ = FALSE;  
		                
		// get values from GUI
		EQs[g_currentEQ].eq_type = (WORD)SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_GETCURSEL, 0, 0);
		EQs[g_currentEQ].eq_freq = GetScrollPos(GetDlgItem(hWnd, IDC_EQ_FREQ), SB_CTL);
		EQs[g_currentEQ].eq_freq = lin2log(EQs[g_currentEQ].eq_freq,0,10000,15,24000);
		EQs[g_currentEQ].eq_q = (GetScrollPos(GetDlgItem(hWnd, IDC_EQ_Q), SB_CTL) / 100.0f);
		EQs[g_currentEQ].eq_gain = ((GetScrollPos(GetDlgItem(hWnd, IDC_EQ_GAIN), SB_CTL) / 10000.0f) - 0.5f) * 40.0f;
		if ((EQs[g_currentEQ].eq_gain<0.0001f) && (EQs[g_currentEQ].eq_gain>-0.0001f)) {
			EQs[g_currentEQ].eq_gain = 0.0f;
		}
                              
		// send command
		sprintf(s, "mixer:eq:peq%i@%i,%g,%g,%g\r\n", g_currentEQ+1, EQs[g_currentEQ].eq_type, EQs[g_currentEQ].eq_freq, EQs[g_currentEQ].eq_q, EQs[g_currentEQ].eq_gain);
		SendCommand(s);
        
        // update dialog                     
        g_updateEQ = TRUE;
	}

	if (g_updateEQ) {
		g_updateEQ = FALSE;

		// update controls
		SendDlgItemMessage(hWnd, IDC_EQ_TYPE, CB_SETCURSEL, EQs[g_currentEQ].eq_type, 0);
		SetScrollPos(GetDlgItem(hWnd, IDC_EQ_FREQ), SB_CTL, (WORD)log2lin(EQs[g_currentEQ].eq_freq,0,10000,15,24000), TRUE);
		SetScrollPos(GetDlgItem(hWnd, IDC_EQ_Q), SB_CTL, (WORD)(EQs[g_currentEQ].eq_q * 100.0f), TRUE);
		SetScrollPos(GetDlgItem(hWnd, IDC_EQ_GAIN), SB_CTL, (WORD)(((EQs[g_currentEQ].eq_gain / 40.0f) + 0.5f) * 10000), TRUE);

		// update labels
		if (EQs[g_currentEQ].eq_freq>=1000) {
			sprintf(s, "%.*f", 2, EQs[g_currentEQ].eq_freq/1000.f);
			strcat(s, " kHz");
		}else{
			sprintf(s, "%.*f", 1, EQs[g_currentEQ].eq_freq);
			strcat(s, " Hz");
		}
		SetDlgItemText(hWnd, IDC_LBL_FREQ, s);
		sprintf(s, "%.*f", 1, EQs[g_currentEQ].eq_q);
		SetDlgItemText(hWnd, IDC_LBL_Q, s);
		sprintf(s, "%+.*f dB", 1, EQs[g_currentEQ].eq_gain);
		SetDlgItemText(hWnd, IDC_LBL_GAIN, s);
	}

	// UpdateCOMP
	if (g_recalcCOMP) {
		g_recalcCOMP = FALSE;
		
		// get values from GUI
		COMPs[g_currentCOMP].threshold = (GetScrollPos(GetDlgItem(hWnd, IDC_COMP_THRESH), SB_CTL) - 600) / 10.0f;
		COMPs[g_currentCOMP].ratio = (WORD)SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_GETCURSEL, 0, 0);     
		if (COMPs[g_currentCOMP].ratio < 9) {
			// regular ratio
			COMPs[g_currentCOMP].ratio = (WORD)round((float)pow(2, COMPs[g_currentCOMP].ratio));
		}else{
			// limiter
			COMPs[g_currentCOMP].ratio = (WORD)(1.0f/1000000.0f);
		}
		COMPs[g_currentCOMP].makeup = (WORD)(SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_GETCURSEL, 0, 0) * 6);
		COMPs[g_currentCOMP].attack = GetScrollPos(GetDlgItem(hWnd, IDC_COMP_ATTACK), SB_CTL) / 1000.0f;
		COMPs[g_currentCOMP].hold = GetScrollPos(GetDlgItem(hWnd, IDC_COMP_HOLD), SB_CTL) / 1000.0f;
		COMPs[g_currentCOMP].release = GetScrollPos(GetDlgItem(hWnd, IDC_COMP_RELEASE), SB_CTL) / 1000.0f;

		// send command
		sprintf(s, "mixer:comp%i@%g,%i,%i,%g,%g,%g\r\n", g_currentCOMP+1, COMPs[g_currentCOMP].threshold,
			COMPs[g_currentCOMP].ratio, COMPs[g_currentCOMP].makeup, COMPs[g_currentCOMP].attack,
			COMPs[g_currentCOMP].hold, COMPs[g_currentCOMP].release);
		SendCommand(s);

		// update dialog
		g_updateCOMP = TRUE;
	}                       
	
	if (g_updateCOMP) {
		g_updateCOMP = FALSE;
		
		// update controls                
		// at the moment we are not supporting more than one compressor...
		/*
		SetScrollPos(GetDlgItem(hWnd, IDC_COMP_THRESH, SB_CTL, 0, TRUE);
		SendDlgItemMessage(hWnd, IDC_COMP_RATIO, CB_SETCURSEL, 0, 0);
		SendDlgItemMessage(hWnd, IDC_COMP_MAKEUP, CB_SETCURSEL, 0, 0);
		SetScrollPos(GetDlgItem(hWnd, IDC_COMP_ATTACK), SB_CTL, 0, TRUE);
		SetScrollPos(GetDlgItem(hWnd, IDC_COMP_HOLD), SB_CTL, 0, TRUE);
		SetScrollPos(GetDlgItem(hWnd, IDC_COMP_RELEASE), SB_CTL, 0, TRUE);
		*/
		
		// update Labels
		sprintf(s, "%.*f dB", 1, COMPs[g_currentCOMP].threshold);
		SetDlgItemText(hWnd, IDC_LBL_THRESH, s);
		sprintf(s, "%g ms", COMPs[g_currentCOMP].attack * 1000); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_ATTACK, s);
		sprintf(s, "%g ms", COMPs[g_currentCOMP].hold * 1000); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_HOLD, s);
		sprintf(s, "%g ms", COMPs[g_currentCOMP].release * 1000); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_RELEASE, s);
	}

	// UpdateGATE  
	if (g_recalcGATE) {
		g_recalcGATE = FALSE;
		
		// get values from GUI
		gate_threshold = (GetScrollPos(GetDlgItem(hWnd, IDC_GATE_THRESH), SB_CTL) - 600) / 10.0f;
		gate_range = GetScrollPos(GetDlgItem(hWnd, IDC_GATE_RANGE), SB_CTL) / 10.0f;
		gate_attack_ms = GetScrollPos(GetDlgItem(hWnd, IDC_GATE_ATTACK), SB_CTL);
		gate_hold_ms = GetScrollPos(GetDlgItem(hWnd, IDC_GATE_HOLD), SB_CTL);
		gate_release_ms = GetScrollPos(GetDlgItem(hWnd, IDC_GATE_RELEASE), SB_CTL);

		// send command
		sprintf(s, "mixer:gate%i@%g,%g,%g,%g,%g,%g\r\n", 1, gate_threshold,
			gate_range, gate_attack_ms, gate_hold_ms, gate_release_ms);
		SendCommand(s);

		// update dialog
		g_updateGATE = TRUE;
	}                       
	
	if (g_updateGATE) {
		g_updateGATE = FALSE;
		
		// update Labels
		sprintf(s, "%.*f dB", 1, gate_threshold);
		SetDlgItemText(hWnd, IDC_LBL_GTHRESH, s);
		sprintf(s, "%.*f dB", 1, gate_range);
		SetDlgItemText(hWnd, IDC_LBL_GRANGE, s);
		sprintf(s, "%g ms", gate_attack_ms); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_GATTACK, s);
		sprintf(s, "%g ms", gate_hold_ms); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_GHOLD, s);
		sprintf(s, "%g ms", gate_release_ms); // just take integers here
		SetDlgItemText(hWnd, IDC_LBL_GRELEASE, s);
	}
	
	// Crossover
	if (g_recalcCROSSOVER) {
		g_recalcCROSSOVER = FALSE;

		// main = HighPass = LowCut
		crossover = GetScrollPos(GetDlgItem(hWnd, IDC_LOWCUT), SB_CTL);
		crossover = lin2log(crossover,0,10000,15,24000);

		// send command
		sprintf(s, "mixer:eq:hp@%g\r\n", crossover);
		SendCommand(s);

		// update label
		if (crossover>=1000) {
			sprintf(s, "%.*f", 2, crossover/1000.f);
			strcat(s, " kHz");
		}else{
			sprintf(s, "%.*f", 1, crossover);
			strcat(s, " Hz");
		}
		SetDlgItemText(hWnd, IDC_LBL_LOWCUT, s);

		// sub = LowPass = HighCut
		crossover = GetScrollPos(GetDlgItem(hWnd, IDC_HIGHCUT), SB_CTL);
		crossover = lin2log(crossover,0,10000,15,24000);

		// send command
		sprintf(s, "mixer:eq:lp@%g\r\n", crossover);
		SendCommand(s);

		// update label
		if (crossover>=1000) {
			sprintf(s, "%.*f", 2, crossover/1000.f);
			strcat(s, " kHz");
		}else{
			sprintf(s, "%.*f", 1, crossover);
			strcat(s, " Hz");
		}
		SetDlgItemText(hWnd, IDC_LBL_HIGHCUT, s);
	}
}
                 
void initValues()
{            
	int i;

	// initialize EQs
	g_currentEQ = 0;
	for (i=0; i<5; i++) {
		EQs[i].eq_type = 0;
		EQs[i].eq_freq = 1000;
		EQs[i].eq_q = 2;
		EQs[i].eq_gain = 0.0f;
	}
	
	// initialize COMPs
	g_currentCOMP = 0;
	for (i=0; i<1; i++) {
		COMPs[i].threshold = 0;
		COMPs[i].ratio = 0;
		COMPs[i].makeup = 0;
		COMPs[i].attack = 1000;
		COMPs[i].hold = 1000;
		COMPs[i].release = 1000;
	}
	
	g_recalcEQ = TRUE;
	g_recalcCOMP = TRUE;
	g_recalcGATE = TRUE; 
	g_recalcCROSSOVER = TRUE;
}

long CALLBACK __export MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{           
	int i;
	WORD w;
	char sb[30];   
	char sb2[30];
	WORD comport;
	UINT baudrate;

    switch (message)
    {                   
    	case WM_CREATE:          
    		initValues();
    	
            initControls(hInst, hWnd);
    		
    		SetTimer(hWnd, FBAPE_MAIN_TIMER, 100, NULL);
    		break;
    	case WM_CLOSE:
    		if (MessageBox(hWnd, "Do you really want to leave the f/bape-Config-Tool?", "Closing...", MB_OKCANCEL) == IDOK) {
    			DestroyWindow(hWnd);
    		}                       
    		break;                                     
    	case WM_VSCROLL:                                  
			if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_ANALOG))
			{
				g_updateVolumes = 1; // this is the program-internal channel number for analog input
			}else if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_SDCARD))
			{
				g_updateVolumes = 3; // this is the program-internal channel number for sdcard input
			}else if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_SPDIF))
			{
				g_updateVolumes = 5; // this is the program-internal channel number for spdif input
			}else if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_MAIN))
			{
				g_updateVolumes = 100; // this is the program-internal channel number for main L/R
			}else if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_SUB))
			{
				g_updateVolumes = 101; // this is the program-internal channel number for subwoofer
			}else{
				for (i=1; i<=16; i++)
				{
					if (HIWORD(lParam) == GetDlgItem(hWnd, IDC_VOL_ULTRANET+(i-1))) {
						SendVolume(7+(i-1), (60 - GetScrollPos(HIWORD(lParam), SB_CTL))/10.0f);
						break;
					}
				}
			}

    		switch (LOWORD(wParam))
    		{                                                                                           
    			case SB_LINEUP:
		    		SetScrollPos(HIWORD(lParam), SB_CTL, GetScrollPos(HIWORD(lParam), SB_CTL)-30, TRUE);
    				break;                                                   
    			case SB_LINEDOWN:
		    		SetScrollPos(HIWORD(lParam), SB_CTL, GetScrollPos(HIWORD(lParam), SB_CTL)+30, TRUE);
    				break;    
    			case SB_THUMBPOSITION:   
    				SetScrollPos(HIWORD(lParam), SB_CTL, LOWORD(lParam), TRUE);
		    		break;
    			case SB_THUMBTRACK:
    				SetScrollPos(HIWORD(lParam), SB_CTL, LOWORD(lParam), TRUE);
    				break;
    			default:
    				break;
    		}
    		break;
    	case WM_HSCROLL:                                  
			if ((HIWORD(lParam) == GetDlgItem(hWnd, IDC_EQ_FREQ)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_EQ_Q)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_EQ_GAIN)))
			{
				g_recalcEQ = TRUE;
			}
			if ((HIWORD(lParam) == GetDlgItem(hWnd, IDC_COMP_THRESH)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_COMP_ATTACK)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_COMP_HOLD)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_COMP_RELEASE)))
			{
				g_recalcCOMP = TRUE;
			}
			if ((HIWORD(lParam) == GetDlgItem(hWnd, IDC_GATE_THRESH)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_GATE_RANGE)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_GATE_ATTACK)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_GATE_HOLD)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_GATE_RELEASE)))
			{
				g_recalcGATE = TRUE;
			}
			if ((HIWORD(lParam) == GetDlgItem(hWnd, IDC_LOWCUT)) |
				(HIWORD(lParam) == GetDlgItem(hWnd, IDC_HIGHCUT)))
			{
				g_recalcCROSSOVER = TRUE;
			}

    		switch (LOWORD(wParam))
    		{                                                                                           
    			case SB_LINEUP:
		    		SetScrollPos(HIWORD(lParam), SB_CTL, GetScrollPos(HIWORD(lParam), SB_CTL)-10, TRUE);
    				break;                                                   
    			case SB_LINEDOWN:
		    		SetScrollPos(HIWORD(lParam), SB_CTL, GetScrollPos(HIWORD(lParam), SB_CTL)+10, TRUE);
    				break;
    			case SB_THUMBPOSITION:
    				SetScrollPos(HIWORD(lParam), SB_CTL, LOWORD(lParam), TRUE);
		    		break;
    			case SB_THUMBTRACK:
    				SetScrollPos(HIWORD(lParam), SB_CTL, LOWORD(lParam), TRUE);
    				break;
    		}
    		break;
    	case WM_TIMER:
    		switch (wParam)
    		{
    			case FBAPE_MAIN_TIMER:
    				// Do something every 100ms
    				TimerCallback(hWnd, message, wParam, lParam);
    				break;
    		}
    		break;     
    	case WM_KEYDOWN:
    		switch (wParam)
    		{
    			case VK_LEFT:     
    				break;
    			case VK_RIGHT:
    				break;
    			case VK_UP:
    				break;
    			case VK_DOWN:
    				break;
    		}
    		break;
    	case WM_LBUTTONDOWN:
    		//MessageBox(NULL, "Left Mouse Button", "Title", 0);
    		break;
    	case WM_RBUTTONDOWN:
    		//MessageBox(NULL, "Right Mouse Button", "Title", 0);
    		break;
        case WM_PAINT:
        	DrawMainWindow(hWnd);
        	break;

		case WM_SIZE:
			InvalidateRect(hWnd, NULL, TRUE);
			break;

        case WM_COMMAND:
            switch (wParam)
            {                      
            	case IDC_CONNECT:              
            		comport = (WORD)SendDlgItemMessage(hWnd, IDC_COMPORT, CB_GETCURSEL, 0, 0) + 1;
            		baudrate = (WORD)SendDlgItemMessage(hWnd, IDC_BAUDRATE, CB_GETCURSEL, 0, 0);
            		
            		// setting the baudrate is special as Win16 API only supports 16-bit UINT-values
            		// for the dcb.BaudRate. See the following website for detailed information:
            		// https://jeffpar.github.io/kbarchive/kb/108/Q108928
            		
            		// for special HEX-values see
            		// https://www.github.com/Alexpux/mingw-w64/blob/master/mingw-w64-tools/widl/include/winbase.h
            		switch (baudrate)
            		{
            			case 0:
            				// 9600 = CBR_9600
            				baudrate = 9600; // 0xFF16, 9600
            				break;
            			case 1:
            				// 19200 = CBR_19200
            				baudrate = 19200; //0xFF18, 19200
            				break;
            			case 2:        
            				// 38400 = CBR_38400
            				baudrate = 38400; // 0xFF1B, 38400
            				break;
            			case 3:
            				// 57600 = CBR_56000
            				baudrate = 57600; //0xFF20, 57600
            				break;
            			case 4:
            				// 115200 = no CBR-equivalent
            				// this is a special thing. Any baudrate above CBR_56000 = 57600 is interpreted as 115200
            				// as this is the maximum baudrate of the 16550-UART-chips.
            				baudrate = 57600+1;//0xFF21, 115200
            				break;                  
            			default:  
            				baudrate = 38400; // 38400
            				break;
            		}
       				if (connectComPort(comport, baudrate)) {
       					// connected successful
	       				ShowWindow(GetDlgItem(hWnd, IDC_CONNECT), SW_HIDE);
	       				ShowWindow(GetDlgItem(hWnd, IDC_DISCONNECT), SW_SHOW);
       				}

            		break;
            	case IDC_DISCONNECT:
            		disconnectComPort();  
       				ShowWindow(GetDlgItem(hWnd, IDC_CONNECT), SW_SHOW);
       				ShowWindow(GetDlgItem(hWnd, IDC_DISCONNECT), SW_HIDE);
            		break;
            	case IDC_PLAY:
            		SendCommand("player:play\r\n");
            		break;
            	case IDC_PAUSE:
            		SendCommand("player:pause\r\n");
            		break;
            	case IDC_STOP:
            		SendCommand("player:stop\r\n");
            		break;
            	case IDC_LOAD:
            		strcpy(sb, "player:file@");
            		i = GetDlgItemText(hWnd, IDC_FILENAME, sb2, 30);
            		strncat(sb, sb2, i);
            		strcat(sb, "\r\n");
            		SendCommand(sb);
            		SendCommand("player:loop@0\r\n");
            		break;
            	case IDC_LOOP:
            		SendCommand("player:loop@1\r\n");
            		break;
            	case IDC_EQ_SET1:
            		g_currentEQ = 0;
            		g_updateEQ = TRUE;
            		break;
            	case IDC_EQ_SET2:
            		g_currentEQ = 1;
            		g_updateEQ = TRUE;
            		break;
            	case IDC_EQ_SET3:
            		g_currentEQ = 2;
            		g_updateEQ = TRUE;
            		break;
            	case IDC_EQ_SET4:
            		g_currentEQ = 3;
            		g_updateEQ = TRUE;
            		break;
            	case IDC_EQ_SET5:
            		g_currentEQ = 4;
            		g_updateEQ = TRUE;
            		break;
            	case IDC_EQ_TYPE:
            	{   
            		switch HIWORD(lParam)
            		{                   
            			case LBN_SELCHANGE:
			            	g_recalcEQ = TRUE;
            				break;
            		}
            		break;
            	}
            	case IDC_COMP_RATIO:
            	{   
            		switch HIWORD(lParam)
            		{                   
            			case LBN_SELCHANGE:
			            	g_recalcCOMP = TRUE;
            				break;
            		}   
            		break;
            	}
            	case IDC_COMP_MAKEUP:
            	{   
            		switch HIWORD(lParam)
            		{                   
            			case LBN_SELCHANGE:
			            	g_recalcCOMP = TRUE;
            				break;
            		}   
            		break;
            	}
            	case IDC_SYNCSOURCE:
            	{
            		w = (WORD)SendDlgItemMessage(hWnd, IDC_SYNCSOURCE, CB_GETCURSEL, 0, 0);            
					sprintf(sb, "mixer:sync@%i\r\n", w);
					SendCommand(sb);
               		break;
            	}
            	case IDC_SAMPLERATE:
            	{                                    
            		w = (WORD)SendDlgItemMessage(hWnd, IDC_SAMPLERATE, CB_GETCURSEL, 0, 0);            
            		switch (w)
            		{
            			case 0:
		            		SendCommand("mixer:samplerate@44100\r\n");
            				break;
            			case 1:
		            		SendCommand("mixer:samplerate@48000\r\n");
            				break;
            			case 2:
		            		SendCommand("mixer:samplerate@96000\r\n");
            				break;  
            			default:
		            		SendCommand("mixer:samplerate@48000\r\n");
            				break;
	           		}
               		break;
            	}
            	case ID_FILE_EXIT:
            	{         
					DestroyWindow(hWnd);
            		break;
            	}
/*
            	case ID_CONTROLS_VOLUME:
	            {                      
	            	// configure scrollbars
	            	
	            	// show dialog
	                DialogBox(hInst, "VolumeBox", hWnd, VolumeDlgProc);
	                break;     
	            }
*/
            	case ID_HELP_ABOUT:
	            {
	                DialogBox(hInst, "AboutBox", hWnd, AboutDlgProc);
	                break;
            	}
            	default:
            		// let Windows process the message
                	return (DefWindowProc(hWnd, message, wParam, lParam));
            }
			break;
        case WM_DESTROY:
        	disconnectComPort();
            PostQuitMessage(0);
            break;

        default:
        	// pass this message as unprocessed
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
