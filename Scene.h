void DrawCursorSprite(CDisplay& CS, int CursorX, int CursorY);
void InitCursor(CDisplay& Displayref);
void FreeCursor();
void SwitchToReloadCursor(CDisplay& Displayref);
void SwitchToFireCursor(CDisplay& Displayref);
void SwitchToOutCursor(CDisplay& Displayref);

class TypewriterText
{
private:
	//SETTABLE
	char* mString;
	char* mBackupString;
	Sprite mCursorSprite;
	int mTypeSpeed;
	int mXOrigin;
	int mYOrigin;
	int mWidth;
	int mFontNumber;
	int mCursorSpriteNumber;
	int mCursorFlashDelay;
	int mFirstLine;
	int mTimeToCursorDisappearAfterFinished;
	int mSoundID;
	int mEndlineSoundID;
	int mFreezeTimer;
	bool mFrozen;
	int mMaxLinesToDraw;
	//CALCULATED INTERNALLY
	int mCurrentStringPosition;
	int mCursorDisappearTimer;
	int mTypeTimer;
	int mCursorFlashTimer;
	bool mIsFinished;
	bool mCursorOn;
	int mStringLength;
	int mNumberOfLines;
	//UTILITY METHODS
	int GetWidthOfNextWord(char* String, int CurrentPos);
	void WrapString(char* String);
	int GetNumLines(char* String);
public:
	TypewriterText() : mString(NULL), mBackupString(NULL), mFrozen(false), mTypeSpeed(0), mFreezeTimer(0), mXOrigin(0), mWidth(0),mFontNumber(0), mCursorSpriteNumber(-1), mCursorFlashDelay(-1), mFirstLine(0), mIsFinished(false), mCursorOn(false), mStringLength(0), mSoundID(-1), mEndlineSoundID(-1){}
	~TypewriterText() { if(mString)delete [] mString; if(mBackupString)delete [] mBackupString; }
	void Initialize(char* String, POINT Origin, int TypeSpeed, int Width, int FontNumber = 0, int CursorDisplaySpriteID = -1, int CursorFlashDelay = -1, int TimeToCursorDisappearAfterFinished = 200, int SoundID = -1, int EndlineSoundID = -1, int MaxLinesToDraw = 1);
	void SetString(char* NewString);
	void Update(int timediff);
	void Draw(CDisplay& DisplayRef);
	bool IsFinished(){return mIsFinished;}
	void DescendNumberOfLines(int NumLines);
	void AscendNumberOfLines(int NumLines);
	int GetNumberOfLines();
	int GetCurrentLine();
	void SetLoc(POINT Loc);
	void SetWidth(int Width);
	void SetLinesToDraw(int NewLinesToDraw);
	bool IsOnNewLine();
	void FreezeForTime(int TimeToFreeze);
	void Freeze();
	void Unfreeze();
	void SetSpeed(int Speed);
};

class BriefingBox
{
	TypewriterText mCurrentText;
	Sprite mBoxSprite;
	//STATUS
	//0. FULL OPEN
	//1. OPENING
	//2. CLOSING
	//3. FULL CLOSED
	int mStatus;
	int mCenterToStop;
	int mCenterToClose;
	bool mWaitForSpaceBar;
	int mTimeAfterFinished;
	POINT mOrigin;
public:
	BriefingBox() : mStatus(3) {}
	void Init();
	void Activate(char* Text, bool WaitForSpaceBarToClose, int TimeToWaitAfterFinished);
	void Deactivate();
	void Update(int timediff, bool SpaceBarDown = false, bool EnterDown = false);
	void Draw(CDisplay& DisplayRef);
	bool IsActive();
};

class CursorPhysicsMachine
{
	POINT mSystemCursor;
	POINT mSystemDelta;
	FPOINT mGameCursor;
	FPOINT mGameDeltaSpeed;
	FPOINT mGameDeltaAccel;
	double mDrag;
	double mAccel;
	bool mPhysicsActive;
	RECT mBounds;
	RECT mClientRect;
	bool mBoundsActive;
	void StopGameCursorAtBounds();
public:
	CursorPhysicsMachine() : mPhysicsActive(0), mBoundsActive(false) {}
	void Initialize(RECT ClientRect, POINT SystemCursor, double Drag, double Accel);
	void Update(int TimeDiff, POINT SystemCursor);
	POINT GetGameCursorPoint();
	bool ArePhysicsActive();
	void SetInMotion(FPOINT Vector);
	void ActivateBounds();
	void DeactivateBounds();
	void SetBounds(RECT Bounds);
	void SetDrag(double Drag);
	void SetAccel(double Accel);
	void ActivatePhysics();
	void DeactivatePhysics();
	void MoveGameCursorTo(POINT To);
};