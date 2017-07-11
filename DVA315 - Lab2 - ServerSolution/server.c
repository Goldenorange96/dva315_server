/********************************************************************\
* server.c                                                           *
*                                                                    *
* Desc: example of the server-side of an application                 *
* Revised: Dag Nystrom & Jukka Maki-Turja                     *
*                                                                    *
* Based on generic.c from Microsoft.                                 *
*                                                                    *
*  Functions:                                                        *
*     WinMain      - Application entry point                         *
*     MainWndProc  - main window procedure                           *
*                                                                    *
* NOTE: this program uses some graphic primitives provided by Win32, *
* therefore there are probably a lot of things that are unfamiliar   *
* to you. There are comments in this file that indicates where it is *
* appropriate to place your code.                                    *
* *******************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"
#include "List.h"

							/* the server uses a timer to periodically update the presentation window */
							/* here is the timer id and timer period defined                          */

#define UPDATE_FREQ     10	/* update frequency (in ms) for the timer */

							/* (the server uses a mailslot for incoming client requests) */

#define PTMDEATH "Your planet's lifetime expired"
#define PTMBOUNDS "Planet flied away"
#define DEATH_SIZE 31
#define BOUNDS_SIZE 18
#define DT 10
List* Plist;	
CRITICAL_SECTION Crit;

/*********************  Prototypes  ***************************/
/* NOTE: Windows has defined its own set of types. When the   */
/*       types are of importance to you we will write comments*/ 
/*       to indicate that. (Ignore them for now.)             */
/**************************************************************/

LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM, LPARAM );
DWORD WINAPI mailThread(LPVOID);
int LifeCycle(MsgStruct *params);




HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
				/* we need it to access the window for printing and drawin */

/********************************************************************\
*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
*                                                                    *
*   Purpose: Initializes Application                                 *
*                                                                    *
*  Comments: Register window class, create and display the main      *
*            window, and enter message loop.                         *
*                                                                    *
*                                                                    *
\********************************************************************/

							/* NOTE: This function is not too important to you, it only */
							/*       initializes a bunch of things.                     */
							/* NOTE: In windows WinMain is the start function, not main */

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) {
	
	InitializeCriticalSectionAndSpinCount(&Crit, 0x00000400);
	Plist = Create_List();
	HWND hWnd;
	DWORD threadID;
	MSG msg;
	


							/* Create the window, 3 last parameters important */
							/* The tile of the window, the callback function */
							/* and the backgrond color */

	hWnd = windowCreate (hPrevInstance, hInstance, nCmdShow, "Himmel", MainWndProc, COLOR_WINDOW+1);

							/* start the timer for the periodic update of the window    */
							/* (this is a one-shot timer, which means that it has to be */
							/* re-set after each time-out) */
							/* NOTE: When this timer expires a message will be sent to  */
							/*       our callback function (MainWndProc).               */
  
	windowRefreshTimer (hWnd, UPDATE_FREQ);
  

							/* create a thread that can handle incoming client requests */
							/* (the thread starts executing in the function mailThread) */
							/* NOTE: See online help for details, you need to know how  */ 
							/*       this function does and what its parameters mean.   */
							/* We have no parameters to pass, hence NULL				*/
  

	threadID = threadCreate (mailThread, NULL); 
  

							/* (the message processing loop that all windows applications must have) */
							/* NOTE: just leave it as it is. */
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}


/********************************************************************\
* Function: mailThread                                               *
* Purpose: Handle incoming requests from clients                     *
* NOTE: This function is important to you.                           *
/********************************************************************/
DWORD WINAPI mailThread(LPVOID arg) {

	planet_type ClientPT;
	MsgStruct *tParam = (MsgStruct*)malloc(sizeof(MsgStruct));
	//planet_type *ClientPT = (planet_type*)malloc(sizeof(planet_type));
	char buffer[1024];
	DWORD bytesRead = 0;
	static int posY = 0;
	HANDLE mailbox;
	*buffer = PTMDEATH;
							/* create a mailslot that clients can use to pass requests through   */
							/* (the clients use the name below to get contact with the mailslot) */
							/* NOTE: The name of a mailslot must start with "\\\\.\\mailslot\\"  */

	MessageBox(MB_APPLMODAL, "Creation of mailslot, HAJIME!", NULL, MB_OK);
	mailbox = mailslotCreate ("mailbox");
	if (mailbox == INVALID_HANDLE_VALUE) {
		MessageBox(MB_APPLMODAL, "Creation of mailslot failed!", NULL, MB_OK);
	}


	for(;;) {				
							/* (ordinary file manipulating functions are used to read from mailslots) */
							/* in this example the server receives strings from the client side and   */
							/* displays them in the presentation window                               */
							/* NOTE: binary data can also be sent and received, e.g. planet structures*/
 

	//	MessageBox(MB_APPLMODAL, buffer, NULL, MB_OK);
	bytesRead = mailslotRead (mailbox, &ClientPT, sizeof(planet_type)); 

	

	if(bytesRead != 0) {

		EnterCriticalSection(&Crit);
		Add_Item_Last(Plist, ClientPT);
		LeaveCriticalSection(&Crit);
		strcpy(tParam->Message, ClientPT.pid);
		strcpy(tParam->PlanName, ClientPT.name);

		char str[30];
		sprintf(str, "%s", ClientPT.name);
		MessageBox(MB_APPLMODAL, str, NULL, MB_OK);
	//	//add the buffer to the list, create a new thread with the buffer sent as it's data

	//	Add_Item_Last(Plist, &ClientPT);

		threadCreate((LPTHREAD_START_ROUTINE)LifeCycle, tParam);

	//						/* NOTE: It is appropriate to replace this code with something */
	//						/*       that match your needs here.                           */
	//	posY++;  
	//						/* (hDC is used reference the previously created window) */							
	//	TextOut(hDC, 10, 50+posY%200, buffer, bytesRead);
	}
	else {
							/* failed reading from mailslot                              */
							/* (in this example we ignore this, and happily continue...) */
    }
  }

  return 0;
}

int LifeCycle(MsgStruct* params)
{

	char str[100];
	MsgStruct *ClientM = (MsgStruct*)malloc(sizeof(MsgStruct));
	int messageID;
	char * message = "";
	double G = 6.67259, Atoty = 0, Atotx = 0, x1,x2,y1,y2,r, R, a1, ax,ay, VX_old, VX_new,VY_old,VY_new, SX_old, SX_new, SY_new, SY_old;
	G = G*pow(10, -11);
	planet_type *MainPlanet, *Iterator;
	MainPlanet = (planet_type*)malloc(sizeof(planet_type));
	HANDLE ReturnSlot, ThreadProcess;
	MainPlanet = GetPlanet(Plist, params->Message, params->PlanName);
	while (TRUE) {
		EnterCriticalSection(&Crit);
		VX_old = MainPlanet->vx;
		SX_old = MainPlanet->sx;
		VY_old = MainPlanet->vy;
		SY_old = MainPlanet->sy;
		Iterator = Plist->Head;
		for (int i = 0; i < Plist->Size; i++) {
			/*Loops through all the planets except it's own and starts calculating*/
			if (strcmp(Iterator->name, MainPlanet->name) == 0 && (strcmp(Iterator->pid, MainPlanet->pid) == 0)) { // Makes sure so the loop doesn't caluculate the main planet with itself
				Iterator = Iterator->next;
				continue;
			}
			else
			{
				x1 = Iterator->sx - MainPlanet->sx;
				y1 = Iterator->sy - MainPlanet->sy;
				r = sqrt((pow(x1, 2)) + (pow(y1, 2))); //<------- calculation 2
				R = r * r;
				a1 = G*(Iterator->mass / R); //<----- calculation 1
				ax = a1*((Iterator->sx - MainPlanet->sx) / r);//<----- calculation 3
				ay = a1*((Iterator->sy - MainPlanet->sy) / r); //<----- caluculation 4
				Atotx += ax;
				Atoty += ay;
				Iterator = Iterator->next;
			}
		}


		/*Calcualting new position for X value*/
		VX_new = VX_old + Atotx * DT; //<----- equation 5
		SX_new = SX_old + VX_new * DT; //

		/*calcualting new position for Y value*/
		VY_new = VY_old + Atoty * DT;
		SY_new = SY_old + VY_new * DT;

		/*Updating Phase*/
		MainPlanet->sx = SX_new;
		MainPlanet->vx = VX_new;
		MainPlanet->sy = SY_new;
		MainPlanet->vy = VY_new;
		Atotx = 0;
		Atoty = 0;

		MainPlanet->life--;		
		if (MainPlanet->life == 0) {
			LeaveCriticalSection(&Crit);
			messageID = 1;
			break;
		}
		else if ((MainPlanet->sx > 800 || MainPlanet->sx < 0) || (MainPlanet->sy > 600 || MainPlanet->sy < 0)) {
			LeaveCriticalSection(&Crit);
			messageID = 0;
			break;
		}
		else {
			LeaveCriticalSection(&Crit);
			Sleep(10);
		}
	}

	if (messageID == 1) {
		message = PTMDEATH;
	}

	else if (messageID == 0) {
		message = PTMBOUNDS;
	}
	else{}
	strcpy(ClientM->Message, message);
	strcpy(ClientM->PlanName, MainPlanet->name);
	ReturnSlot = mailslotConnect(MainPlanet->pid);
	mailslotWrite(ReturnSlot, ClientM, sizeof(MsgStruct));
	ThreadProcess = GetCurrentThread();
	CloseHandle(ThreadProcess);
	
	return 0;
}


/********************************************************************\
* Function: LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM) *
*                                                                    *
* Purpose: Processes Application Messages (received by the window)   *
* Comments: The following messages are processed                     *
*                                                                    *
*           WM_PAINT                                                 *
*           WM_COMMAND                                               *
*           WM_DESTROY                                               *
*           WM_TIMER                                                 *
*                                                                    *
\********************************************************************/
/* NOTE: This function is called by Windows when something happens to our window */

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
  
	PAINTSTRUCT ps;
	static int posX = 10;
	int posY;
	char str[50];
	HANDLE context;
	static DWORD color = 0;
	planet_type * Iterator;

	switch( msg ) {
							/**************************************************************/
							/*    WM_CREATE:        (received on window creation)
							/**************************************************************/
		case WM_CREATE:       
			hDC = GetDC(hWnd);  
			break;   
							/**************************************************************/
							/*    WM_TIMER:         (received when our timer expires)
							/**************************************************************/
		case WM_TIMER:

							/* NOTE: replace code below for periodic update of the window */
							/*       e.g. draw a planet system)                           */
							/* NOTE: this is referred to as the 'graphics' thread in the lab spec. */

							/* here we draw a simple sinus curve in the window    */
							/* just to show how pixels are drawn */    
							
			/*posX += 1;
			posY = (int) (20 * sin(posX / (double) 5) + 50);
			SetPixel(hDC, posX % 547, posY, (COLORREF) color);
			color += 12;*/
			//Simulation of rendering planets 
			
			EnterCriticalSection(&Crit);
			Iterator = Plist->Head;
				while(Iterator != NULL){
					posX = (int)Iterator->sx;
					posY = (int)Iterator->sy;
					char str[30];
					SetPixel(hDC, posX, posY, (COLORREF)color);
					SetPixel(hDC, posX+1, posY, (COLORREF)color);
					SetPixel(hDC, posX, posY+1, (COLORREF)color);
					SetPixel(hDC, posX, posY-1, (COLORREF)color);
					SetPixel(hDC, posX-1, posY, (COLORREF)color);
					color += 12;
					Iterator = Iterator->next;
				}
			LeaveCriticalSection(&Crit);

			

			/*posX = Test_Planet.sx;
			posY = Test_Planet.sy;
			SetPixel(hDC, posX % 547, posY, (COLORREF)color);*/
			//color += 12;

			windowRefreshTimer (hWnd, UPDATE_FREQ);
			break;
							/****************************************************************\
							*     WM_PAINT: (received when the window needs to be repainted, *
							*               e.g. when maximizing the window)                 *
							\****************************************************************/

		case WM_PAINT:
							/* NOTE: The code for this message can be removed. It's just */
							/*       for showing something in the window.                */
			context = BeginPaint( hWnd, &ps ); /* (you can safely remove the following line of code) */
			TextOut( context, 10, 10, "Hello, World!", 13 ); /* 13 is the string length */
			EndPaint( hWnd, &ps );
			break;
							/**************************************************************\
							*     WM_DESTROY: PostQuitMessage() is called                  *
							*     (received when the user presses the "quit" button in the *
							*      window)                                                 *
							\**************************************************************/
		case WM_DESTROY:
			PostQuitMessage( 0 );
							/* NOTE: Windows will automatically release most resources this */
     						/*       process is using, e.g. memory and mailslots.           */
     						/*       (So even though we don't free the memory which has been*/     
     						/*       allocated by us, there will not be memory leaks.)      */

			ReleaseDC(hWnd, hDC); /* Some housekeeping */
			break;

							/**************************************************************\
							*     Let the default window proc handle all other messages    *
							\**************************************************************/
		default:
			return( DefWindowProc( hWnd, msg, wParam, lParam )); 
   }
   return 0;
}




