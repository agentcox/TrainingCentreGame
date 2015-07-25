struct GunShotEvent
{

	bool MuzzleFlashDone;
	Sprite* MuzzleFlash;
	UCHAR FlashLevel;
	
	GunShotEvent() : MuzzleFlashDone(true), MuzzleFlash(NULL)
	{
	}
	void GunShotGo(POINT cursorpos);
	bool ReloadGo(POINT cursorpos);
	bool Update(int timediff, POINT cursorpos, bool LMBDOWN);
	~GunShotEvent()
	{
		if(MuzzleFlash)
			delete MuzzleFlash;
	}
};

struct NotificationStringEvent
{

	//STATUS
	//0. FULL OUT (ACTIVATED)
	//1. OPENING
	//2. CLOSING
	//3. FULL IN (DEACTIVATED)
	char* String;
	int mStatus;
	int mXOrigin;
	int mYOrigin;
	int mXStop; //CALCULATED WHEN STRING CHANGED.
	int mXCloseStop;
	int mHoldTimer;

	NotificationStringEvent() : mStatus(0), String(NULL), mHoldTimer(0) {}
	void Update(int timediff);
	void ChangeString(char* Str, int HoldTime);
	void Draw();
	~NotificationStringEvent() {if(String)delete [] String;}

};

struct GameEvents
{
	bool mGunShotActive;
	bool mGunShotsAllowed;
	GunShotEvent mGunShot;
	NotificationStringEvent mNotification;
	DWORD mScreenFlashColor;
	char* CursorDrawString;
	

	GameEvents() : mGunShotActive(false), CursorDrawString(NULL) {}
	void Update(int timediff, POINT cursorpos, bool LMBDOWN, bool RMBDOWN);
	void ChangeCursorDrawString(char* Str)
	{
		if(CursorDrawString)
			delete [] CursorDrawString;
		CursorDrawString = new char[strlen(Str)+1];
		strcpy(CursorDrawString, Str);
	}
	void DrawCursorString(POINT cursor);
	void UpdateNotificationString(int timediff);
	void ChangeNotificationString(char* Str, int HoldTime);
	void AllowGunShots();
	void DisallowGunShots();
	~GameEvents() {if(CursorDrawString)delete [] CursorDrawString;}
};

void GunShot(POINT cursorpos);