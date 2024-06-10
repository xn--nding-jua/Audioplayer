#include "string.h"  
int hPort;

void SendCommand(cmd)
	char *cmd;
{
	int len = strlen(cmd);
	WriteComm(hPort, cmd, len);
}               

void SendVolume(int ch, float value)
{                 
    char charArray[30];
             
	if (ch==100) {
		sprintf(charArray, "mixer:volume:main@%g\r\n", value);
	}else if (ch==101) {
		sprintf(charArray, "mixer:volume:sub@%g\r\n", value);
	}else{
		sprintf(charArray, "mixer:volume:ch%i@%g\r\n", ch, value);
	}

	SendCommand(charArray);
}
               
void ReceiveAnswer()
{   
	//int len;               
	//char* buf;

	//len = ReadComm("COM1", buf, 2);
}    
                
BOOL connectComPort(WORD port, UINT baudrate)
{
	DCB dcb;
	char sPort[5];

	// open comPort
	sprintf(sPort, "COM%i", port);       
	hPort = OpenComm(sPort, 1024, 128);
	
	if (hPort >= 0)
	{                         
		// connected successful
		// configure port
		GetCommState(hPort, &dcb);              
		dcb.BaudRate = baudrate; // baudrate is UINT
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;      
		SetCommState(&dcb); 
		return TRUE;
	}else{           
		// error in connection
		return FALSE;
	}	
}                      

void disconnectComPort()
{
	CloseComm(hPort);
}