/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

    COMMENTS:

    No initialization is needed for this particular dialog box, but TRUE
    must be returned to Windows.

    Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL __export CALLBACK VolumeDlgProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
	char sb[30];   

    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
  			SetDlgItemText(hDlg, IDC_EDIT, "Demotext");

   			SendDlgItemMessage(hDlg, IDC_COMPORTS, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM1");
   			SendDlgItemMessage(hDlg, IDC_COMPORTS, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM2");
   			SendDlgItemMessage(hDlg, IDC_COMPORTS, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM3");
   			SendDlgItemMessage(hDlg, IDC_COMPORTS, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"COM4");

   			SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR)"Textitem 1");
   			SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR)"Textitem 2");
   			SendDlgItemMessage(hDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR)"Textitem 3");

        	//SendDlgItemMessage(hDlg, IDC_VOLUME_MAIN, );
            return (TRUE);

        case WM_COMMAND:               /* message: received a command */
            switch LOWORD(wParam)
            {
	            case IDOK:
            	{                       
            		int len = GetDlgItemText(hDlg, IDC_EDIT, sb, 30);                
					//int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT));
					if (len>0) {                      
						
            	    	MessageBox(NULL, sb, "Title", MB_OK);
            	    }
	                EndDialog(hDlg, TRUE);
                	return (TRUE);
            	}
            	case IDC_VOLUME_MAIN:
            		MessageBox(NULL, "IDC_VOLUME_MAIN", "TADAA", MB_OK);
            		switch HIWORD(lParam)
            		{                   
            			case EN_VSCROLL:
			            	MessageBox(NULL, "Scrollbar clicked", "TADAA", MB_OK);
            				break;
            		}
            		break;
            	case IDC_VOLUME_SUB:
            		break;
            	case IDC_LISTBOX:
            	{   
            		switch HIWORD(lParam)
            		{                   
            			case LBN_SELCHANGE:
			            	MessageBox(NULL, "Listbox Change", "TADAA", MB_OK);
            				break;
            		}
            	}
            	break;
            }
    }
    return (FALSE);               /* Didn't process a message    */
}

BOOL __export CALLBACK AboutDlgProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:               /* message: received a command */
            if (wParam == IDOK         /* "OK" box selected?          */
                || wParam == IDCANCEL) /* System menu close command?  */
            {
                EndDialog(hDlg, TRUE); /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);               /* Didn't process a message    */
}
