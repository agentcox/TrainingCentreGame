// CovertOps.cpp : Defines the entry point for the application.
//
#include <list>
using namespace std;
#include <time.h>
#include <stdlib.h>
#include "stdafx.h"
#include <zmouse.h>
#include "resource.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "sounds.h"
#include "scene.h"
#include "weapon.h"
#include "physicalobject.h"
#include "target.h"

#include "gameevents.h"
#include "globaldata.h"
#include "font.h"
#include "scoreunit.h"
#include "timerunit.h"
#include "challenge.h"
#include "Particle.h"
#include "covertops.h"
#include "errorlog.h"


#define STANDARDSHELLCASINGSPEED	1
#define CINEMATICSHELLCASINGSPEED	4


#define GAMEMODE_BRIEFING	0
#define GAMEMODE_SELECTION	1
#define GAMEMODE_SHOOTING	2
#define GAMEMODE_RECAP		3
#define GAMEMODE_WAIT		4
#define MAX_LOADSTRING 100

int CurrentChallengeNumber = 0;



// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING] = "C3OS";								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
HWND hwnd;
// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ResoStruct GetFinalResolution();


HRESULT WINAPI ResCallBack(LPDDSURFACEDESC2 lpddsd, LPVOID lpContext);

int GameInitialize();
void GameFree();
void CheckDisplayMemory();
CDisplay TheDisplay;
CSoundManager SoundManager;
SoundQueue GameSounds;
extern GameEvents Events;
int WindowWidth = 800;
int WindowHeight = 600;
Font* DebugFont;

//TIMERS
LARGE_INTEGER Timer1;
LARGE_INTEGER Timer2;
int TimeDiff = 0;
int CkTime = 20;
LARGE_INTEGER QFreqN;
int QFreq;
POINT Cursor;
POINT SystemCursor;
POINT SPt;
bool LMBDOWN = false;
bool RMBDOWN = false;
bool SlowMotion = false;
bool SpaceBarDown = false;
bool EnterPressed = false;
bool HelpPressed = false;
bool WeaponCheck = false;
int SlowMotionCoefficient = 3;

int GameMode = GAMEMODE_BRIEFING; //BRIEFING, WEAPON SELECTION, SHOOTING, RECAP
int OldGameMode = GAMEMODE_SHOOTING;
int WaitToMode = GAMEMODE_RECAP;

bool GameInProgress = false;

//OUR BOYS WEAPON
Weapon* PlayerWeapon = NULL;
int CurrentWeaponID = 0;

//THE BRIEFING UNIT
BriefingBox TheBox;

//THE SCORE UNIT
ScoreUnit ScoreBox;

//THE TIMER UNIT
TimerUnit GameTimer;

//THE CURSOR MACHINE
CursorPhysicsMachine CursorMachine;

//THE CHALLENGE
Challenge* TheChallenge = NULL;

//WAITING TIMER
int WaitTimer = 0;

void ShootingModeUpdate(int TimeDiff);
void SelectionModeUpdate(int TimeDiff);
void BriefingModeUpdate(int TimeDiff);
bool PressedOnce(LPARAM lParam);
void HelpScreen();
void Wait(int Time, int WaitToMode);
bool WaitFinished();
void GoToSelectionMode();
void GoToShootingMode();
void GoToRecapMode();
void GoToBriefingMode();
void WaitMode(int TimeDiff);
void RecapMode(int TimeDiff);

//ERROR MACHINE
ErrorLog* ELog;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	srand(time(NULL));
	
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_COVERTOPS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_COVERTOPS);
	
	if(!GameInitialize()){
		//uh oh.
		MessageBox(NULL, "Failure To Initialize. My apologies.", "ERROR", MB_OK);
		return msg.wParam;
	}
	else{
		AddError("Game Initialization SUCCESS");
	}
	//TheBox.Activate("Welcome to THE TRAINING CENTRE\n\n\nWe at the centre would like to make sure you take some time to familiarize yourself with the equipment you have at your disposal\n\nCategory I  Pistols\nCategory II  Sub Machine Guns\nCategory III  Shotguns\nCategory IV  Automatic Rifles\nCategory V  Precision Rifles\n\nPlease feel free to use any of the weapons at your discretion\nGood Hunting\n Charles Cox\n Director\n\nYou may press enter to speed up briefings and space to close them when you are finished reading\n\nPress H for help at any time\n\nPress space now to begin", true, 5000);
	GoToBriefingMode();

	while(1)
	{
		QueryPerformanceCounter(&Timer1);
		if (PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
		{
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT){
				break;
			}
		}
		if(CursorMachine.ArePhysicsActive()){
			CursorMachine.Update(TimeDiff, SystemCursor);
				Cursor = CursorMachine.GetGameCursorPoint();
		}
			else
				Cursor = SystemCursor;

			switch(GameMode)
			{
			case GAMEMODE_BRIEFING:
				BriefingModeUpdate(TimeDiff);
				break;
			case GAMEMODE_SELECTION:
				if(GameTimer.IsTimeUp())
				{
					GoToShootingMode();
				}
				if(GameTimer.IsReadyToBeginCounting()){
					GameTimer.Unfreeze();
				}
				SelectionModeUpdate(TimeDiff);
				break;
			case GAMEMODE_SHOOTING:
				ShootingModeUpdate(TimeDiff);
				break;
			case GAMEMODE_RECAP:
				RecapMode(TimeDiff);
				break;
			case GAMEMODE_WAIT:
				if(WaitFinished()){
					GameMode = WaitToMode;
				}
				else
				WaitMode(TimeDiff);
				break;
			}
			
			TheDisplay.Present();
			Sleep(3);
		QueryPerformanceCounter(&Timer2);
		TimeDiff = (int)(Timer2.QuadPart - Timer1.QuadPart) / QFreq;
		if(SlowMotion){
		TimeDiff /= SlowMotionCoefficient;
		}
	}

	//END THE GAME
	GameFree();
	AddError("Game Proper Shutdown");
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_COVERTOPS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	
	hInst = hInstance; // Store instance handle in our global variable
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP,
		CW_USEDEFAULT, 0, WindowWidth, WindowHeight, NULL,NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}
	hwnd = hWnd;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	return TRUE;
}

void ShootingModeUpdate(int TimeDiff)
{
	
	if(HelpPressed){
		HelpScreen();
		HelpPressed = false;
	}
	if(WeaponCheck){
		ShowWeaponSelector();
		WeaponCheck = false;
	}
	TheDisplay.Clear(Events.mScreenFlashColor);
	UpdateAllTargets(TimeDiff, TheDisplay);
	TheChallenge->ChallengeUpdate(TimeDiff);
	Events.Update(TimeDiff, Cursor, LMBDOWN, RMBDOWN);
	GameSounds.Update(TimeDiff);
	WeaponUpdate(PlayerWeapon, TimeDiff);
	UpdateBounceObjects(TimeDiff);
	UpdateAndDrawAllParticles(TimeDiff);
	UpdateWeaponsSelector(PlayerWeapon, TimeDiff, TheDisplay);
	DrawCursorSprite(TheDisplay, Cursor.x, Cursor.y);
	Events.UpdateNotificationString(TimeDiff);
	TheBox.Update(TimeDiff);
	TheBox.Draw(TheDisplay);
	ScoreBox.Draw(TheDisplay);
	GameTimer.Update(TimeDiff, TheDisplay);
	if(GameTimer.IsTimeUp()){
		GoToRecapMode();
	}

	CheckDisplayMemory();
}

void SelectionModeUpdate(int TimeDiff)
{
		if(EnterPressed){
		CurrentWeaponID = ChangeToNextWeapon(CurrentWeaponID, &PlayerWeapon, false);
		EnterPressed = false;
	}
	if(HelpPressed){
		HelpScreen();
		HelpPressed = false;
	}
	if(WeaponCheck){
		ShowWeaponSelector();
		WeaponCheck = false;
	}
	TheDisplay.Clear(Events.mScreenFlashColor);
	UpdateAllTargets(TimeDiff, TheDisplay);
	Events.Update(TimeDiff, Cursor, LMBDOWN, RMBDOWN);
	GameSounds.Update(TimeDiff);
	WeaponUpdate(PlayerWeapon, TimeDiff);
	UpdateBounceObjects(TimeDiff);
	UpdateAndDrawAllParticles(TimeDiff);
	UpdateWeaponsSelector(PlayerWeapon, TimeDiff, TheDisplay);
	DrawCursorSprite(TheDisplay, Cursor.x, Cursor.y);
	Events.UpdateNotificationString(TimeDiff);
	TheBox.Update(TimeDiff);
	TheBox.Draw(TheDisplay);
//	ScoreBox.Draw(TheDisplay);
	GameTimer.Update(TimeDiff, TheDisplay);
	CheckDisplayMemory();
}

void BriefingModeUpdate(int TimeDiff)
{
	TheDisplay.Clear(0);
	//Events.Update(TimeDiff, Cursor, LMBDOWN, RMBDOWN);
	GameSounds.Update(TimeDiff);
	//WeaponUpdate(PlayerWeapon, TimeDiff);
	UpdateBounceObjects(TimeDiff);
	UpdateAndDrawAllParticles(TimeDiff);
	//UpdateWeaponsSelector(PlayerWeapon, TimeDiff, TheDisplay);
	//DrawCursorSprite(TheDisplay, Cursor.x, Cursor.y);
	Events.UpdateNotificationString(TimeDiff);
	TheBox.Update(TimeDiff, SpaceBarDown, EnterPressed);
	TheBox.Draw(TheDisplay);
	if(!TheBox.IsActive())
	{
		if(!GameInProgress){
		GoToSelectionMode();
		}
		else{
			GameMode = OldGameMode;
		}
	}
	CheckDisplayMemory();
}

void RecapMode(int TimeDiff)
{
	TheDisplay.Clear(0);
	GameSounds.Update(TimeDiff);
	UpdateBounceObjects(TimeDiff);
	UpdateAndDrawAllParticles(TimeDiff);
	Events.UpdateNotificationString(TimeDiff);
	TheBox.Update(TimeDiff, SpaceBarDown, EnterPressed);
	TheBox.Draw(TheDisplay);
	if(!TheBox.IsActive())
	{
		GoToBriefingMode();
	}
	CheckDisplayMemory();
	
}

void Wait(int Time, int ToGameMode)
{
	WaitTimer = Time;
	ToGameMode = GameMode;
	GameMode = GAMEMODE_WAIT;
	Events.DisallowGunShots();
}

bool WaitFinished()
{
	return (WaitTimer <= 0);
}

void GoToRecapMode()
{
	TheBox.Activate(TheChallenge->GetRecap(), true, 5000);
	GameSounds.InsertSound(*GetGlobalSoundData(508));
	SlowMotion = false;
	GameTimer.Close();
	ClearTargets();
	GameInProgress = false;
	Events.DisallowGunShots();
	Events.ChangeNotificationString("ROUND OVER", 2000);
	Wait(3000, GAMEMODE_RECAP);
}

void GoToSelectionMode()
{
		ScoreBox.SetScore(0);
		GameTimer.Open();
		GameTimer.Freeze();
		SelectionModeUpdate(TimeDiff);
		GameInProgress = true;
		ChangeWeapon(0, &PlayerWeapon);
		ActivateWeaponSelector(PlayerWeapon);
		GameTimer.SetCaption("SELECTION TIME");
		GameTimer.SetTime(0, 20, 0);
		Events.ChangeNotificationString("20 Seconds To Select Firearm", 2000);
		Events.DisallowGunShots();
		GameMode = GAMEMODE_SELECTION;
}

void GoToShootingMode()
{
	//ADD ALL TARGETS NOW!
	TheChallenge->onRoundStart();
	GameSounds.InsertSound(*GetGlobalSoundData(508));
		GameTimer.Unfreeze();
		GameTimer.SetCaption("ROUND TIME");
		GameTimer.SetTime(1, 0, 0);
		Events.AllowGunShots();
		GameMode = GAMEMODE_SHOOTING;
}

void GoToBriefingMode()
{
	if(TheChallenge)
		delete TheChallenge;
	MakeNewChallenge(&TheChallenge, &CurrentChallengeNumber);
	TheBox.Activate(TheChallenge->GetBriefing(), true, 5000);
	GameMode = GAMEMODE_BRIEFING;
}

void WaitMode(int TimeDiff)
{

	if(HelpPressed){
		HelpScreen();
		HelpPressed = false;
	}

	TheDisplay.Clear(Events.mScreenFlashColor);
	UpdateAllTargets(TimeDiff, TheDisplay);
	Events.Update(TimeDiff, Cursor, LMBDOWN, RMBDOWN);
	GameSounds.Update(TimeDiff);
	WeaponUpdate(PlayerWeapon, TimeDiff);
	UpdateBounceObjects(TimeDiff);
	UpdateWeaponsSelector(PlayerWeapon, TimeDiff, TheDisplay);
	DrawCursorSprite(TheDisplay, Cursor.x, Cursor.y);
	Events.UpdateNotificationString(TimeDiff);
	TheBox.Update(TimeDiff);
	TheBox.Draw(TheDisplay);
	ScoreBox.Draw(TheDisplay);
	GameTimer.Update(TimeDiff, TheDisplay);
	WaitTimer -= TimeDiff;
	CheckDisplayMemory();
}

void HelpScreen()
{
	SlowMotion = false;
	OldGameMode = GameMode;
	GameMode = GAMEMODE_BRIEFING;
	char HelpText[5000];
	switch(OldGameMode)
	{
	case GAMEMODE_SELECTION:
		strcpy(HelpText, "you are currently in weapons selection mode\nthis mode allows you to select an appropriate weapon for the upcoming challenge\n\nControls are as follows\nTo change to the next available weapon press enter or click the mouse wheel\nadditionally you may roll the mouse wheel up and down to move forward and backward through your weapon selections\nGood luck\n\nPress SPACE TO Continue");
		break;
	case GAMEMODE_SHOOTING:
		sprintf(HelpText, "you are now in control of the %s one of the most sophisticated personal firearms in the world\nyour interface is simple so that you may focus on the important tasks of shooting	and hitting your targets\n\nYou will see the aiming sight that represents where your bullets will land when you fire your weapon\nyour current ammunition level is shown at the bottom right of your screen\n\nYour controls are as follows\n\nWeapon Controls\nMouse Movement  Move Aiming Sight\nLeft Click  Fire Weapon\nRight Click  Reload\n\nSpecial Controls\nS  Slow Motion\nW  Weapon Check\n\nPress SPACE TO Continue", PlayerWeapon->mName);
			break;
	case GAMEMODE_WAIT:
	case GAMEMODE_RECAP:
		break;
	}
	TheBox.Activate(HelpText, true, 5000);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	short Delta;
	switch (message) 
	{
	case WM_CREATE:
		ShowCursor(false);
		break;
	case WM_LBUTTONDOWN:
		SystemCursor.x = LOWORD(lParam);
		SystemCursor.y = HIWORD(lParam);
		LMBDOWN = true;
		break;
	case WM_KEYUP:
		switch((int)wParam)
		{
		case VK_RETURN:
			EnterPressed = false;
			break;
		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, 0,0);
			break;
		case 'C':
		case 'c':
			if(GetShellCasingUpdateSpeed() == STANDARDSHELLCASINGSPEED){
				ChangeShellCasingUpdateSpeed(CINEMATICSHELLCASINGSPEED);
				Events.ChangeNotificationString("CINEMATIC SHELL CASING SPEED", 2500);
			}
			else{
				ChangeShellCasingUpdateSpeed(STANDARDSHELLCASINGSPEED);
				Events.ChangeNotificationString("STANDARD SHELL CASING SPEED", 2500);
			}
			break;
		case 'S':
		case 's':
			if(SlowMotion){
				SlowMotion = false;
				Events.ChangeNotificationString("STANDARD TIME CYCLE", 2500);
			}
			else if (GameMode == GAMEMODE_SHOOTING){
				SlowMotion = true;
				Events.ChangeNotificationString("SLOW MOTION TIME CYCLE", 1000);
			}
			break;
		case 'H':
		case 'h':
			HelpPressed = true;
			break;
		case 'W':
		case 'w':
			WeaponCheck = true;
			break;
		case 'P':
		case 'p':
			if(CursorMachine.ArePhysicsActive())
			{
				CursorMachine.DeactivatePhysics();
				Events.ChangeNotificationString("CURSOR PHYSICS DEACTIVATED", 2500);
			}
			else
			{
				CursorMachine.ActivatePhysics();
				Events.ChangeNotificationString("CURSOR PHYSICS ACTIVATED", 2500);
			}
		case VK_SPACE:
			SpaceBarDown = false;
			break;
		}
		break;
		case WM_KEYDOWN:
			switch((int)wParam)
			{
			case VK_SPACE:
				SpaceBarDown = true;
				break;
			case VK_RETURN:
				EnterPressed = true;
				break;
			}
			break;
		case WM_MOUSEWHEEL:
			Delta = (short)(HIWORD(wParam));
			if(Delta > 0 && GameMode == GAMEMODE_SELECTION)
				CurrentWeaponID = ChangeToNextWeapon(CurrentWeaponID, &PlayerWeapon, false);
			if(Delta < 0 && GameMode == GAMEMODE_SELECTION)
				CurrentWeaponID = ChangeToNextWeapon(CurrentWeaponID, &PlayerWeapon, true);
			break;
		case WM_MBUTTONDOWN:
			if(GameMode == GAMEMODE_SELECTION)
			CurrentWeaponID = ChangeToNextWeapon(CurrentWeaponID, &PlayerWeapon, false);
			break;
	case WM_LBUTTONUP:
		LMBDOWN = false;
		break;
	case WM_RBUTTONDOWN:
		RMBDOWN = true;
		break;
	case WM_RBUTTONUP:
		RMBDOWN = false;
		break;
	case WM_MOUSEMOVE:
		SystemCursor.x = LOWORD(lParam);
		SystemCursor.y = HIWORD(lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool PressedOnce(LPARAM lParam)
{
	if(lParam & (1 << 30))
		return false;
	return true;
}

int GameInitialize()
{
	HRESULT DisplayResult;
	bool Debug = true;
	bool PhysicsOn = false;
	#if NDEBUG
	Debug = false;
	PhysicsOn = true ;
		#else
	ELog = new ErrorLog;
	#endif


	char stuff[500];
	
	ResoStruct rs = GetFinalResolution();
	sprintf(stuff, "Fullscreen Res: %d x %d at 32 Bit Color", rs.width, rs.height);
	AddError(stuff);
	
	if(Debug){
		DisplayResult = TheDisplay.CreateWindowedDisplay(hwnd, WindowWidth, WindowHeight);
		if(DisplayResult != S_OK){
			TheDisplay.DestroyObjects();
			AddError("Failure to initialize display debug windowed display");
			return false;
		}
		AddError("Debug Windowed SUCCESS");
	}
	else{
		if(rs.bpp){
			DisplayResult = TheDisplay.CreateFullScreenDisplay(hwnd, rs.width, rs.height, 32);
			if(DisplayResult != S_OK){
				AddError("Failure to initialize fullscreen display. Attempting windowed mode.");
				DisplayResult = TheDisplay.CreateWindowedDisplay(hwnd, WindowWidth, WindowHeight);
				if(DisplayResult != S_OK){
					TheDisplay.DestroyObjects();
					AddError("Failure to initialize windowed display after trying fullscreen.");
					return false;
				}
				AddError("Fullscreen failure - divert to windowed SUCCESS");
			}
			else{
			AddError("Fullscreen SUCCESS");
			}
		}
		else{
			DisplayResult = TheDisplay.CreateWindowedDisplay(hwnd, WindowWidth, WindowHeight);
			if(DisplayResult != S_OK){
				TheDisplay.DestroyObjects();
				AddError("Failure to initialize display upon no-acceptable-fullscreen-mode fallback.");
				return false;
			}
			MessageBox(NULL,"GOOD NEWS AND BAD NEWS.\nThe good news is that Training Centre will run on your system. The bad news is that your system doesn't support 32-bit color. If you're in 16-bit color mode...this might look and behave VERY STRANGELY. Be warned. If you are in 16-bit color mode, we highly recommend you switch to 32-bit mode through your Display Controls.\n\nAlso, due to the fact that you'll be playing in a window, the cursor physics system has been disabled for your protection.\n\nI apologize for the inconvenience.\n-Charles Cox, Training Centre Developer", "Display Contingency", MB_OK);
			AddError("No-Acceptable-Fullscreen-Mode Fallback to Windowed SUCCESS.");
			PhysicsOn = false;
		}
	}
	
	
	
	DisplayResult = SoundManager.Initialize(hwnd, DSSCL_PRIORITY, 1, 22050, 16);
	if(DisplayResult != S_OK){
		AddError("Sound Manager Initialization FAILURE");
		return false;
	}
	AddError("Sound Manager Initialization SUCCESS");
	
	if(!InitGlobalData()){
		AddError("Cannot Initialize GlobalData");
		return false;
	}
	if(!InitializeGameBullets()){
		AddError("Cannot Initialize Global Bullet Data");
		return false;
	}
	if(!InitializeGameWeapons()){
		AddError("Cannot Initialize Global Weapons Data");
		return false;
	}
	if(!LoadGameFonts()){
		AddError("Cannot Initialize Global Fonts");
		return false;
	}
	AddError("All Data Is Clear And Loaded");

	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);
	POINT Pt;
	GetCursorPos(&Pt);
	SystemCursor = Pt;
	ScreenToClient(TheDisplay.GetHWnd(), &Pt);
	CursorMachine.Initialize(Client, Pt, 0.4, 0.006);
	AddError("Cursor Machine Active");
	if(PhysicsOn){
		CursorMachine.ActivatePhysics();
		AddError("Cursor Machine Physics ON");
	}
	
	InitCursor(TheDisplay);
	AddError("Cursor Sprite OK");
	ChangeWeapon(0, &PlayerWeapon);
	AddError("Player Weapon Set To Zero");
	if(InitializeWeaponSelector(PlayerWeapon)){
	AddError("Weapon Selector Initialized OK");
	}
	else{
		AddError("Weapon Selector INITIALIZATION FAILED");
		return false;
	}
	QueryPerformanceFrequency(&QFreqN);
	QFreq = (int)(QFreqN.QuadPart / 1000);
	AddError("Performance Frequency Queried - OK");
	TheBox.Init();
	AddError("Briefing Box Initialization OK");
	ScoreBox.Initialize(1, 0, 2);
	AddError("Score Box Initialization OK");
	ScoreBox.SetDigitsToDisplay(3);
	ScoreBox.SetCaption("POINTS");
	ScoreBox.SetTitle("CURRENT SCORE");
	ScoreBox.SetScore(0);
	AddError("Score Box Zeroed Out OK");
	GameTimer.Initialize(810, 811, 812, 2);
	AddError("Game Timer INITIALIZED");
	GameTimer.SetCaption("TIME REMAINING");
	GameTimer.SetTime(0,20, 0);
	GameTimer.Open();
	AddError("Game Timer Zeroed Out to 0-30-0 OK");
	DebugFont = GetFontBySizeIndex(0);
	InitParticleSystem();
	AddError("Particle System is ACTIVE");
	return true;
}


void GameFree()
{
	delete PlayerWeapon;
	DestroyGlobalData();
	DestroyGameWeaponsAndBullets();
	DestroyGameFonts();
	FreeCursor();
	FreeParticleSystem();
	delete ELog;
}

void CheckDisplayMemory()
{
#if NDEBUG
	return;
#else
POINT Z;
Z.x = 0;
Z.y = 0;
char buf[250];
sprintf(buf, "%d ALLOC", GetNumSurfacesTotal());
DebugFont->DrawString(buf, Z);
#endif
}

list<ResoStruct> ResList;

ResoStruct GetFinalResolution()
{
	ResoStruct rs;
	memset(&rs, 0, sizeof(rs));
	LPDIRECTDRAW7 dd;
	DirectDrawCreate(NULL, (IDirectDraw**)&dd, NULL);
	if(!dd)
		return rs;
	dd->EnumDisplayModes(0, NULL, NULL, ResCallBack);
	//WE HAVE THE LIST.
	list<ResoStruct>::iterator i = ResList.begin();
	if(ResList.empty()){
		dd->Release();
		return rs; //NOTHING SUPPORTED!
	}
	int topres = 0;
	int tempres = 0;
	for(; i != ResList.end(); i++)
	{
		tempres = i->height * i->width;
		if(tempres > topres){
			topres = tempres;
			rs = (*i);
		}
	}
	dd->Release();
	return rs;
}

HRESULT WINAPI ResCallBack(LPDDSURFACEDESC2 lpddsd, LPVOID lpContext)
{
	ResoStruct rs;
	rs.bpp = lpddsd->ddpfPixelFormat.dwRGBBitCount;
	if(rs.bpp != 32) //WE DON'T WANT THIS THING
		return DDENUMRET_OK;
	rs.height = lpddsd->dwHeight;
	rs.width = lpddsd->dwWidth;
		if(rs.width > 1024) //WE DON'T WANT IT TOO BIG
		return DDENUMRET_OK;
	ResList.push_front(rs);
	return DDENUMRET_OK;
}

void AddError(char* ErrorMessage)
{
#if NDEBUG
	return;
#endif
	ELog->AddError(ErrorMessage);
}