#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include <list>
using namespace std;
#include "sounds.h"
#include "globaldata.h"
#include "font.h"
#include "scene.h"

//OUR SPRITES!
Sprite* CursorSprite;
Sprite* FireGunSprite;
Sprite* ReloadSprite;
Sprite* OutSprite;

extern CDisplay TheDisplay;

#define BRIEFINGBOX_SPRITE_NUMBER	807
#define BRIEFINGBOX_TEXT_XOFFSET	12
#define BRIEFINGBOX_TEXT_YOFFSET	5
#define BRIEFINGBOX_TEXT_FONTNUMBER	2
#define BRIEFINGBOX_TEXT_BOTTOM		25
#define BRIEFINGBOX_CURSOR_SPRITE_NUMBER	808
#define BRIEFINGBOX_CURSOR_SOUND_NUMBER		504
#define BRIEFINGBOX_CURSOR_ENDLINESOUND_NUMBER	505

#define BRIEFINGBOX_OPEN_START_SOUND		500
#define BRIEFINGBOX_OPEN_LOCK_SOUND			503
#define BRIEFINGBOX_CLOSE_START_SOUND		500

extern SoundQueue GameSounds;

void DrawCursorSprite(CDisplay& CS, int CursorX, int CursorY)
{
	POINT loc;
	loc.x = CursorX - (CursorSprite->GetWidth() / 2);
	loc.y = CursorY - (CursorSprite->GetHeight() / 2);
	CursorSprite->SetLoc(loc);
	CursorSprite->Draw(CS);
}

void InitCursor(CDisplay& Displayref)
{
	OutSprite = new Sprite;
	OutSprite->Initialize(Displayref, "images\\util\\outcrosshair.bmp", 50,50,50,50,1,NULL);
	FireGunSprite = new Sprite;
	FireGunSprite->Initialize(Displayref, "images\\util\\crosshair.bmp", 50,50,50,50,1, NULL);
	ReloadSprite = new Sprite;
	ReloadSprite->Initialize(Displayref, "images\\util\\reloadcrosshair.bmp", 50,50,50,50,1, NULL);
	CursorSprite = new Sprite;
	CursorSprite->InitializeSpriteCopy(ReloadSprite);
}

void SwitchToOutCursor(CDisplay& Displayref)
{
	CursorSprite->InitializeSpriteCopy(OutSprite);
}

void SwitchToReloadCursor(CDisplay& Displayref)
{
	CursorSprite->InitializeSpriteCopy(ReloadSprite);
}

void SwitchToFireCursor(CDisplay& Displayref)
{
	CursorSprite->InitializeSpriteCopy(FireGunSprite);
}

void FreeCursor()
{
	delete OutSprite;
	delete FireGunSprite;
	delete ReloadSprite;
	delete CursorSprite;
}
/*
class TypewriterText
{
private:
	//SETTABLE
	char* mString;
	int mTypeSpeed;
	int mXOrigin;
	int mYOrigin;
	int mWidth;
	int mFontNumber;
	int mCursorSpriteNumber;
	int mCursorFlashDelay;
	//CALCULATED INTERNALLY
	int mCurrentStringPosition;
	int mCurrentXGraphicOffset;
	int mCurrentYGraphicOffset;
	int mTypeTimer;
	int mCursorFlashTimer;
	//UTILITY METHODS
	int GetWidthOfNextWord(char* String, int CurrentPos);
	void WrapString(char* String);
public:
	TypewriterText() : mString(NULL), mTypeSpeed(0), mXOrigin(0), mYOrigin(0), mXBound(0), mYBound(0), mFontNumber(0), mCursorSpriteNumber(-1), mCursorFlashDelay(-1) {}
	~TypewriterText() { if(mString)delete [] mString; }
	void Initialize(char* String, POINT Origin, int Width, int FontNumber = 0, int CursorDisplaySpriteID = -1, int CursorFlashDelay = -1);
	void SetString(char* NewString);
	void Update(int timediff);
	void Draw(CDisplay& DisplayRef);
};
*/

int TypewriterText::GetWidthOfNextWord(char* String, int CurrentPos)
{
	int Len = 0;
	Font* Ft = GetFontBySizeIndex(mFontNumber);
	if(!Ft)
		return 0;
	for(;;CurrentPos++)
	{
		Len += Ft->GetCharWidth(String[CurrentPos]);
		if(String[CurrentPos] == '\n' || String[CurrentPos] == ' ' || String[CurrentPos] == '\0')
		{
			break;
		}
	}
	return Len;
}

void TypewriterText::WrapString(char* String)
{
	
	int PreviousFirstLetter = 0;
	int LineLen = 0;
	for(int CurrentPos = 0; String[CurrentPos] != '\0'; CurrentPos++)
	{
		if(String[CurrentPos] == ' ' || String[CurrentPos] == '\n')
		{
			if(String[CurrentPos] == '\n')
			{
				LineLen = 0;
			}
			else{
			LineLen += GetWidthOfNextWord(String, PreviousFirstLetter);
			if(LineLen >= mWidth)
			{
				if((PreviousFirstLetter - 1) > 0){
				String[PreviousFirstLetter - 1] = '\n';
				LineLen = 0;
				}
			}
			}
			PreviousFirstLetter = CurrentPos + 1;
			
		}
	}
}

int TypewriterText::GetNumLines(char* String)
{
	int Count = 0;
	for(int i = 0; i < mStringLength; i++)
	{
		if(String[i] == '\n')
			Count++;
	}
	return Count;
}

void TypewriterText::SetString(char* String)
{
	if(mString)
		delete [] mString;
	if(mBackupString)
		delete [] mBackupString;
	mBackupString = new char[strlen(String) + 1];
	mString = new char[strlen(String) + 1];
	strcpy(mBackupString, String);
	strcpy(mString, String);
	mIsFinished = false;
	mFirstLine = 0;
	mTypeTimer = 0;
	mCursorFlashTimer = 0;
	mCurrentStringPosition = 0;
	mFreezeTimer = 0;
	mCursorOn = true;
	mStringLength = strlen(String);
	mNumberOfLines = GetNumLines(String);
	WrapString(mString);
}

void TypewriterText::Initialize(char* String, POINT Origin, int TypeSpeed, int Width, int FontNumber, int CursorDisplaySpriteID, int CursorFlashDelay, int TimeToCursorDisappearAfterFinished, int SoundID, int EndlineSoundID, int LinesToDraw)
{
	if(mString)
		delete [] mString;
	if(mBackupString)
		delete [] mBackupString;

	Sprite* Blah;
	SetString(String);
	mXOrigin = Origin.x;
	mYOrigin = Origin.y;
	mWidth = Width;
	mFontNumber = FontNumber;
	mCursorSpriteNumber = CursorDisplaySpriteID;
	mCursorFlashDelay = CursorFlashDelay;
	mTimeToCursorDisappearAfterFinished = TimeToCursorDisappearAfterFinished;
	mCursorDisappearTimer = mTimeToCursorDisappearAfterFinished;
	mTypeSpeed = TypeSpeed;
	if(CursorDisplaySpriteID != -1)
		 Blah = GetGlobalSpriteData(CursorDisplaySpriteID);
	if(!Blah)
		mCursorSpriteNumber = -1;
	else
		mCursorSprite.InitializeSpriteCopy(Blah);
	mSoundID = SoundID;
	mEndlineSoundID = EndlineSoundID;
	mMaxLinesToDraw = LinesToDraw;
}

void TypewriterText::Update(int timediff)
{
	if(mCursorSpriteNumber != -1 && mCursorFlashDelay != -1)
	{
		mCursorFlashTimer -= timediff;
		if(mCursorFlashTimer <= 0){
			mCursorOn ^= true;
			mCursorFlashTimer = mCursorFlashDelay;
		}
	}


	if(!mTypeSpeed){
		mCurrentStringPosition = mStringLength;
		mIsFinished = true;
	}
	
	if(mFreezeTimer > 0)
		mFreezeTimer -= timediff;

	else if(!mIsFinished && mFreezeTimer <= 0 && !mFrozen){
		mTypeTimer -= timediff;
		if(mTypeTimer <= 0)
		{
			mTypeTimer = mTypeSpeed;
			mCurrentStringPosition++;
			if(mSoundID != -1)
				GameSounds.InsertSound(*GetGlobalSoundData(mSoundID));
			if(mCurrentStringPosition == mStringLength){
				mIsFinished = true;
				if(mEndlineSoundID != -1)
					GameSounds.InsertSound(*GetGlobalSoundData(mEndlineSoundID));
			}
		}
	}

	if(mTimeToCursorDisappearAfterFinished != -1 && mCursorDisappearTimer <= 0)
	{
		mCursorOn = false;
	}
	else{
		mCursorDisappearTimer -= timediff;
	}

}

void TypewriterText::Draw(CDisplay& DisplayRef)
{
	for(int i = 0, k = 0; k < mFirstLine && i < mStringLength; i++)
	{
		if(mString[i] == '\n')
			k++;
	}
	if(i >= mStringLength){
		return; //UH, WHAT?
	}
	Font* OurFont = GetFontBySizeIndex(mFontNumber);
	if(!OurFont)
		return;
	//WE'RE AT OUR STARTING POINT.
	POINT Origin;
	Origin.x = mXOrigin;
	Origin.y = mYOrigin;
	int DrawnLines = 1;
	for(; i < mCurrentStringPosition && DrawnLines <= mMaxLinesToDraw; i++)
	{
		if(mString[i] == '\n'){
			Origin.y += OurFont->GetCharHeight();
			DrawnLines++;
			Origin.x = mXOrigin;
		}
		else{
			Origin.x += OurFont->DrawLetter(mString[i], Origin);
		}
	}
	//WE'RE AT THE CURSOR POSITION.
	if(mCursorSpriteNumber == -1 || mCursorFlashDelay == -1){
		return;
	}
	if(Origin.x + mCursorSprite.GetWidth() > mWidth + mXOrigin){
		Origin.y += OurFont->GetCharHeight();
	}
	mCursorSprite.SetLoc(Origin);
	if(mCursorOn)
	mCursorSprite.Draw(DisplayRef);

	

}

void TypewriterText::SetLinesToDraw(int NumLinesToDraw)
{
	mMaxLinesToDraw = NumLinesToDraw;
}

void TypewriterText::DescendNumberOfLines(int NumLines)
{
	if(NumLines + mFirstLine >= mNumberOfLines)
		mFirstLine = mNumberOfLines - 1;
	else{
		mFirstLine += NumLines;
	}
}

void TypewriterText::AscendNumberOfLines(int NumLines)
{
	if(mFirstLine - NumLines <= 0){
		mFirstLine = 0;
	}
	else{
		mFirstLine -= NumLines;
	}
}

int TypewriterText::GetNumberOfLines()
{
	return mNumberOfLines;
}

int TypewriterText::GetCurrentLine()
{
	return mFirstLine;
}

void TypewriterText::SetLoc(POINT loc)
{
	mXOrigin = loc.x;
	mYOrigin = loc.y;
}
void TypewriterText::SetWidth(int Width)
{
	mWidth = Width;
	strcpy(mString, mBackupString);
	WrapString(mString);
}

bool TypewriterText::IsOnNewLine()
{
	return mString[mCurrentStringPosition] == '\n';
}

void TypewriterText::FreezeForTime(int TimeToFreeze)
{
	mFreezeTimer = TimeToFreeze;
}

void TypewriterText::Freeze()
{
	mFrozen = true;
}

void TypewriterText::Unfreeze()
{
	mFrozen = false;
}

void TypewriterText::SetSpeed(int Speed)
{
	mTypeSpeed = Speed;
}


void BriefingBox::Init()
{
	mBoxSprite.InitializeSpriteCopy(GetGlobalSpriteData(BRIEFINGBOX_SPRITE_NUMBER));
	mBoxSprite.SetDelay(800);
	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);
	mCenterToStop = ((Client.right - Client.left) / 2) - (mBoxSprite.GetWidth() / 2);
	int MaxLines = 10;
	Font* TheFont = GetFontBySizeIndex(BRIEFINGBOX_TEXT_FONTNUMBER);
	if(TheFont){
		int Height = (mBoxSprite.GetHeight() - BRIEFINGBOX_TEXT_BOTTOM) - BRIEFINGBOX_TEXT_YOFFSET;
		MaxLines = Height / TheFont->GetCharHeight();
	}
	mCenterToClose = mBoxSprite.GetWidth() * -1;
	mOrigin.y = ((Client.bottom - Client.top) / 2) - (mBoxSprite.GetHeight() / 2);
	mOrigin.x = mCenterToClose;
	mCurrentText.Initialize("TESTING!", mOrigin, 20, mBoxSprite.GetWidth() - (3 * BRIEFINGBOX_TEXT_XOFFSET), BRIEFINGBOX_TEXT_FONTNUMBER,
		BRIEFINGBOX_CURSOR_SPRITE_NUMBER, 80, -1, BRIEFINGBOX_CURSOR_SOUND_NUMBER, BRIEFINGBOX_CURSOR_ENDLINESOUND_NUMBER, MaxLines);
}

void BriefingBox::Activate(char* Text, bool WaitForSpaceBarToClose, int TimeToWaitAfterFinished)
{
	mCurrentText.SetSpeed(20);
	GameSounds.InsertSound(*GetGlobalSoundData(BRIEFINGBOX_OPEN_START_SOUND));
	mCurrentText.SetString(Text);
	mCurrentText.Freeze();
	mWaitForSpaceBar = WaitForSpaceBarToClose;
	mTimeAfterFinished = TimeToWaitAfterFinished;
	mStatus = 1;
}

void BriefingBox::Deactivate()
{
	GameSounds.InsertSound(*GetGlobalSoundData(BRIEFINGBOX_CLOSE_START_SOUND));
	mCurrentText.Freeze();
	mStatus = 2;
}

void BriefingBox::Update(int timediff, bool SpaceBarDown, bool EnterDown)
{
	if(EnterDown && !mCurrentText.IsFinished()){
		mCurrentText.SetSpeed(0);
	}

	switch(mStatus)
	{
	case 0:
		if(!mWaitForSpaceBar && mCurrentText.IsFinished()){
			mTimeAfterFinished -= timediff;
			if(mTimeAfterFinished <= 0)
				Deactivate();
		}
		if(SpaceBarDown && mCurrentText.IsFinished())
			Deactivate();
		break;
	case 1:
		mOrigin.x += timediff * 2;
		if(mOrigin.x >= mCenterToStop){
			mOrigin.x = mCenterToStop;
			mStatus = 0;
			//PLAY SOUND?
			GameSounds.InsertSound(*GetGlobalSoundData(BRIEFINGBOX_OPEN_LOCK_SOUND));
			mCurrentText.Unfreeze();
		}
		break;
	case 2:
		mOrigin.x -= timediff * 2;
		if(mOrigin.x <= mCenterToClose){
			mOrigin.x = mCenterToClose;
			mStatus = 3;
		}
		break;
	case 3:
		return;
	}
	mBoxSprite.SetLoc(mOrigin);
	mCurrentText.Update(timediff);
	mBoxSprite.UpdateFrames(timediff);
}

bool BriefingBox::IsActive()
{
	return (mStatus == 1 || mStatus == 0);
}

void BriefingBox::Draw(CDisplay& DisplayRef)
{
	/*
	#define BRIEFINGBOX_SPRITE_NUMBER	807
#define BRIEFINGBOX_TEXT_XOFFSET	3
#define BRIEFINGBOX_TEXT_YOFFSET	3
#define BRIEFINGBOX_TEXT_FONTNUMBER	2
#define BRIEFINGBOX_TEXT_BOTTOM		25
	*/
	//DRAW THE BOX.
	mBoxSprite.Draw(DisplayRef);
	//DRAW THE TEXT.
	//FIND STARTING PLACE.
	POINT Loc = mOrigin;
	Loc.x += BRIEFINGBOX_TEXT_XOFFSET;
	Loc.y += BRIEFINGBOX_TEXT_YOFFSET;
	mCurrentText.SetLoc(Loc);
	mCurrentText.Draw(DisplayRef);
}

///THE CURSOR MACHINE

void CursorPhysicsMachine::StopGameCursorAtBounds()
{
	if(!mBoundsActive)
	{
		if(mGameCursor.x > mClientRect.right){
			mGameCursor.x = mClientRect.right;
		}
		if(mGameCursor.x < mClientRect.left){
			mGameCursor.x = mClientRect.left;
		}
		if(mGameCursor.y > mClientRect.bottom){
			mGameCursor.y = mClientRect.bottom;
		}
		if(mGameCursor.y < mClientRect.top){
			mGameCursor.y = mClientRect.top;
		}
	}
	else
	{
			if(mGameCursor.x > mBounds.right){
			mGameCursor.x = mBounds.right;
		}
		if(mGameCursor.x < mBounds.left){
			mGameCursor.x = mBounds.left;
		}
		if(mGameCursor.y > mBounds.bottom){
			mGameCursor.y = mBounds.bottom;
		}
		if(mGameCursor.y < mBounds.top){
			mGameCursor.y = mBounds.top;
		}
	}
}

void CursorPhysicsMachine::MoveGameCursorTo(POINT To)
{
	mGameCursor.x = To.x;
	mGameCursor.y = To.y;
}
void CursorPhysicsMachine::Initialize(RECT ClientRect, POINT SystemCursor, double Drag, double Accel)
{
	mBoundsActive = false;
	mClientRect = ClientRect;
	mPhysicsActive = false;
	mAccel = Accel;
	mDrag = Drag;
	mSystemCursor = SystemCursor;
	mSystemDelta.x = mSystemDelta.y = 0;
	mGameCursor.x = mSystemCursor.x;
	mGameCursor.y = mSystemCursor.y;
	mGameDeltaSpeed.x = mGameDeltaSpeed.y = 0;
	mGameDeltaAccel.x = mGameDeltaAccel.y = 0;
}
POINT CursorPhysicsMachine::GetGameCursorPoint()
{
	POINT Pt;
	Pt.x = mGameCursor.x;
	Pt.y = mGameCursor.y;
	return Pt;
}
void CursorPhysicsMachine::ActivateBounds()
{
	mBoundsActive = true;
}
void CursorPhysicsMachine::DeactivateBounds()
{
	mBoundsActive = false;
}
void CursorPhysicsMachine::SetBounds(RECT Bounds)
{
	mBounds = Bounds;
}
void CursorPhysicsMachine::ActivatePhysics()
{
	mPhysicsActive = true;
}
void CursorPhysicsMachine::DeactivatePhysics()
{
	mPhysicsActive = false;
}
void CursorPhysicsMachine::Update(int TimeDiff, POINT SystemCursor)
{
	//CALCULATE SYSTEM DELTA.
	mSystemDelta.x = (mSystemCursor.x - SystemCursor.x);
	mSystemDelta.y = (mSystemCursor.y - SystemCursor.y);

	//DEAL W/ NO PHYSICS.
	if(!mPhysicsActive){
		mGameCursor.x = mSystemCursor.x - mSystemDelta.x;
		mGameCursor.y = mSystemCursor.y - mSystemDelta.y;
	}

	else{
	//ELSE, PHYSICS BEGINS. FIRST, TRANSLATE THE SYSTEM DELTA INTO ACCEL DELTA.
	mGameDeltaAccel.x = (double)(mSystemDelta.x * TimeDiff) * mAccel;
	mGameDeltaAccel.y = (double)(mSystemDelta.y * TimeDiff) * mAccel;

	//NOW, UPLOAD THEM TO THE SPEED.
	mGameDeltaSpeed.x -= mGameDeltaAccel.x;
	mGameDeltaSpeed.y -= mGameDeltaAccel.y;

	//NOW UPLOAD THE SPEED TO THE POSITION.
	mGameCursor.x += mGameDeltaSpeed.x;
	mGameCursor.y += mGameDeltaSpeed.y;

	//NOW BLEED OFF THE DRAG.
	if(mGameDeltaSpeed.x < 0){
		mGameDeltaSpeed.x += mDrag;
		if(mGameDeltaSpeed.x > 0)
			mGameDeltaSpeed.x = 0;
	}
	else if(mGameDeltaSpeed.x > 0){
		mGameDeltaSpeed.x -= mDrag;
		if(mGameDeltaSpeed.x < 0)
			mGameDeltaSpeed.x = 0;
	}

	if(mGameDeltaSpeed.y < 0){
		mGameDeltaSpeed.y += mDrag;
		if(mGameDeltaSpeed.y > 0)
			mGameDeltaSpeed.y = 0;
	}
	else if(mGameDeltaSpeed.y > 0){
		mGameDeltaSpeed.y -= mDrag;
		if(mGameDeltaSpeed.y < 0)
			mGameDeltaSpeed.y = 0;
	}
	}

	//STORE NEW CURSOR POS.
	if(mPhysicsActive)
	StopGameCursorAtBounds();

	mSystemCursor = GetGameCursorPoint();
	POINT mTo = mSystemCursor;
	ClientToScreen(TheDisplay.GetHWnd(), &mTo);
	SetCursorPos(mTo.x, mTo.y);
}

void CursorPhysicsMachine::SetDrag(double Drag)
{
	mDrag = Drag;
}

void CursorPhysicsMachine::SetAccel(double Accel)
{
	mAccel = Accel;
}

void CursorPhysicsMachine::SetInMotion(FPOINT Vector)
{
	mGameDeltaSpeed.x += Vector.x;
	mGameDeltaSpeed.y += Vector.y;
}

bool CursorPhysicsMachine::ArePhysicsActive()
{
	return mPhysicsActive;
}