#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "wrapper.h"

#define TIMERID			100  /* id for timer that is used by the thread that manages the window where graphics is drawn */
#define DEFAULT_STACK_SIZE	1024
#define TIME_OUT			MAILSLOT_WAIT_FOREVER

/* ATTENTION!!! calls that require a time out, use TIME_OUT constant, specifies that calls are blocked forever */


DWORD threadCreate (LPTHREAD_START_ROUTINE threadFunc, LPVOID threadParams) {

	/* Creates a thread running threadFunc */
	/* optional parameters (NULL otherwise)and returns its id! */
	DWORD thId;
	HANDLE h1 = CreateThread(NULL, 2048, threadFunc, threadParams, CREATE_SUSPENDED, &thId);
	if (h1 == INVALID_HANDLE_VALUE)
	{
		printf("Thread creation failed.\n");
		return 1;
	}

	else
	{
		ResumeThread(h1);
		printf("Thread creation succeded.\n");
		printf("Thread Id: %d \n", thId);
		
		return thId;
	}	
}


HANDLE mailslotCreate (char *name) {

	/* Creates a mailslot with the specified name and returns the handle */
	/* Should be able to handle a messages of any size */
	char mailslotPath[50];
	strcpy(mailslotPath, "\\\\.\\mailslot\\");
	strcat(mailslotPath, name);

	HANDLE hSlot = CreateMailslot(mailslotPath, 0, TIME_OUT, (LPSECURITY_ATTRIBUTES)NULL);
	if (hSlot == INVALID_HANDLE_VALUE)
	{
		printf("Mailslot creation failed.\n");
		return hSlot;
	}

	else
	{
		printf("Mailslot creation succeded.\n");
		return hSlot;
	}

}

HANDLE mailslotConnect (char * name) 
{
	/* Connects to an existing mailslot for writing */
	/* and returns the handle upon success     */
	char mailslotPath[50];
	strcpy(mailslotPath,"\\\\.\\mailslot\\" );
	strcat(mailslotPath, name);
	HANDLE mailslot = CreateFile(mailslotPath, (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_WRITE | FILE_SHARE_READ), (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	
	if (mailslot == INVALID_HANDLE_VALUE)
	{
		printf("Create File failed.");
	}

	return mailslot;
}

int mailslotWrite(HANDLE mailSlot, void *msg, int msgSize) {

	/* Write a msg to a mailslot, return nr */
	/* of successful bytes written         */
	
	BOOL fResult;
	DWORD bytesWritten;
	fResult = WriteFile(mailSlot, msg, (DWORD)(msgSize), &bytesWritten, NULL);
	if (!fResult)
	{
		printf("Write to mailslot failed. Errorcode: %d. \n", GetLastError());
		return (int)bytesWritten;
	}
	else
	{
		printf("Write to slot succeded. \n");
		return (int)bytesWritten;
	}
}

int	mailslotRead (HANDLE mailbox, void *msg, int msgSize) {

	/* Read a msg from a mailslot, return nr */
	/* of successful bytes read              */
	DWORD fResult;
	DWORD bytesToRead;
	DWORD noOfMsgs;
	//MsgStruct msgRead;

	fResult = GetMailslotInfo(mailbox, NULL, &bytesToRead, &noOfMsgs, NULL);
	if (bytesToRead == MAILSLOT_NO_MESSAGE)
	{
		return 0;
	}
	//msgRead.msg = (char *)malloc(sizeof(char) * bytesToRead + 1);
	fResult = ReadFile(mailbox, msg, bytesToRead, &msgSize, NULL);
	if (fResult == 0)
	{
 		printf("Read from mailslot failed. Error: %d \n", GetLastError());
		Sleep(400);
	}
	else
	{
		printf("Message: %s\n", msg);
	}

	return msgSize;
}

int mailslotClose(HANDLE mailSlot){
	
	/* close a mailslot, returning whatever the service call returns */
	return CloseHandle(mailSlot);
}


/******************** Wrappers for window management, used for lab 2 and 3 ***********************/
/******************** DONT CHANGE!!! JUST FYI ******************************************************/


HWND windowCreate (HINSTANCE hPI, HINSTANCE hI, int ncs, char *title, WNDPROC callbackFunc, int bgcolor) {

  HWND hWnd;
  WNDCLASS wc; 

  /* initialize and create the presentation window        */
  /* NOTE: The only important thing to you is that we     */
  /*       associate the function 'MainWndProc' with this */
  /*       window class. This function will be called by  */
  /*       windows when something happens to the window.  */
  if( !hPI) {
	 wc.lpszClassName = "GenericAppClass";
	 wc.lpfnWndProc = callbackFunc;          /* (this function is called when the window receives an event) */
	 wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	 wc.hInstance = hI;
	 wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	 wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	 wc.hbrBackground = (HBRUSH) bgcolor;
	 wc.lpszMenuName = "GenericAppMenu";

	 wc.cbClsExtra = 0;
	 wc.cbWndExtra = 0;

	 RegisterClass( &wc );
  }

  /* NOTE: This creates a window instance. Don't bother about the    */
  /*       parameters to this function. It is sufficient to know     */
  /*       that this function creates a window in which we can draw. */
  hWnd = CreateWindow( "GenericAppClass",
				 title,
				 WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
				 0,
				 0,
				 CW_USEDEFAULT,
				 CW_USEDEFAULT,
				 NULL,
				 NULL,
				 hI,
				 NULL
				 );

  /* NOTE: This makes our window visible. */
  ShowWindow( hWnd, ncs );
  /* (window creation complete) */

  return hWnd;
}

void windowRefreshTimer (HWND hWnd, int updateFreq) {

  if(SetTimer(hWnd, TIMERID, updateFreq, NULL) == 0) {
	 /* NOTE: Example of how to use MessageBoxes, see the online help for details. */
	 MessageBox(NULL, "Failed setting timer", "Error!!", MB_OK);
	 exit (1);
  }
}


/******************** Wrappers for window management, used for lab  3 ***********************/
/*****  Lab 3: Check in MSDN GetOpenFileName and GetSaveFileName  *********/
/**************  what the parameters mean, and what you must call this function with *********/


HANDLE OpenFileDialog(char* string, DWORD accessMode, DWORD howToCreate)
{

	OPENFILENAME opf;
	char szFileName[_MAX_PATH]="";

	opf.Flags				= OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
	opf.lpstrDefExt			= "dat";
	opf.lpstrCustomFilter	= NULL;
	opf.lStructSize			= sizeof(OPENFILENAME);
	opf.hwndOwner			= NULL;
	opf.lpstrFilter			= NULL;
	opf.lpstrFile			= szFileName;
	opf.nMaxFile			= _MAX_PATH;
	opf.nMaxFileTitle		= _MAX_FNAME;
	opf.lpstrInitialDir		= NULL;
	opf.lpstrTitle			= string;
	opf.lpstrFileTitle		= NULL ; 
	
	if(accessMode == GENERIC_READ)
		GetOpenFileName(&opf);
	else
		GetSaveFileName(&opf);

	return CreateFile(szFileName, 
		accessMode, 
		0, 
		NULL, 
		howToCreate, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);


}

