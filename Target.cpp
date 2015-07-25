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
#include "font.h"
#include "globaldata.h"
#include "scoreunit.h"
#include "target.h"
#include "challenge.h"
#include "particle.h"

list <Target*> Targets;

extern CDisplay TheDisplay;
extern SoundQueue GameSounds;
extern ScoreUnit ScoreBox;
extern Challenge* TheChallenge;

#define BLACKSECTOR(i) (i & 1)
#define REDSECTOR(i) (i & (1 << 1))
#define GREENSECTOR(i) (i & (1 << 2))
#define WHITESECTOR(i) (i & (1 << 3))
#define MAGENTASECTOR(i) (i & (1 << 4))
#define YELLOWSECTOR(i) (i & (1 << 5))
#define CYANSECTOR(i) (i & (1 << 6))
#define BLUESECTOR(i) (i & (1 << 7))
#define NOSECTORS(i) (i == 0)


void Target::Init(int SpriteNumber, int HitDetectSpriteNumber, POINT Location, int SoundID, int SpriteState, int SpriteDelay)
{
	TargetSprite->InitializeSpriteCopy(GetGlobalSpriteData(SpriteNumber));
	if(SpriteState)
		TargetSprite->SetState(SpriteState);
	if(SpriteDelay != -1)
		TargetSprite->SetDelay(SpriteDelay);
	HitDetectSprite->InitializeSpriteCopy(GetGlobalSpriteData(HitDetectSpriteNumber));
	mSoundID = SoundID;
	mOrigin = Location;
}

bool Target::HasImpacted(Bullet* ToImpact, POINT ImpactOrigin)
{
	RECT ImpactArea;
	POINT NextPt;
	ImpactArea.left = mOrigin.x;
	ImpactArea.top = mOrigin.y;
	ImpactArea.bottom = ImpactArea.top + TargetSprite->GetHeight();
	ImpactArea.right = ImpactArea.left + TargetSprite->GetWidth();
	//TRIVIAL
	Sprite* HitSprite = GetGlobalSpriteData(ToImpact->mHitGlobalGraphicsID);
	if(!HitSprite)
		return false;
	//TOP LEFT
	NextPt = ImpactOrigin;
	if(IsPointInRect(NextPt, ImpactArea))
		return true;
	//TOP RIGHT
	NextPt.x += ToImpact->mHitWidth;
	if(IsPointInRect(NextPt, ImpactArea))
		return true;
	//BOTTOM RIGHT
	NextPt.y += ToImpact->mHitHeight;
	if(IsPointInRect(NextPt, ImpactArea))
		return true;
	//BOTTOM LEFT
	NextPt.x -= ToImpact->mHitWidth;
	if(IsPointInRect(NextPt, ImpactArea))
		return true;
	return false;
}

int Target::BulletImpact(Bullet* ToImpact, POINT ImpactOrigin)
{
	if(!HasImpacted(ToImpact, ImpactOrigin))
		return -1;
	if(TargetSprite->GetState() != 0 || TargetSprite->GetFrame() != 0)
		return -1;

	BITFIELD32 HitField = InternalBulletHit(ToImpact, ImpactOrigin);
	
	//PLAY SOUND
	ASound* TheSound = GetGlobalSoundData(mSoundID);
	if(TheSound)
		GameSounds.InsertSound(*TheSound);

		int ScoreToAdd = 0;

	if(REDSECTOR(HitField))
		ScoreToAdd = 10;
	else if(YELLOWSECTOR(HitField))
		ScoreToAdd = 8;
	else if(GREENSECTOR(HitField))
		ScoreToAdd = 6;
	else if(CYANSECTOR(HitField))
		ScoreToAdd = 4;

	return ScoreToAdd;
}

int Target::InternalBulletHit(Bullet* ToImpact, POINT ImpactOrigin)
{
	//WE HAVE AN IMPACT. IMMEDIATELY LOCK ALL THREE SPRITES.
	Sprite* HitSprite = GetGlobalSpriteData(ToImpact->mHitGlobalGraphicsID);
	
	LPDIRECTDRAWSURFACE7 HitSpriteSurface = HitSprite->GetSurface();
	LPDIRECTDRAWSURFACE7 TargetSurface = TargetSprite->GetSurface();
	LPDIRECTDRAWSURFACE7 HitDetectSurface = HitDetectSprite->GetSurface();

	DDSURFACEDESC2 HSSData;
	DDSURFACEDESC2 TSSData;
	DDSURFACEDESC2 HDSData;

	HSSData.dwSize = TSSData.dwSize = HDSData.dwSize = sizeof(DDSURFACEDESC2);

	RECT TotalSpriteRect;
	TotalSpriteRect.left = mOrigin.x;
	TotalSpriteRect.top = mOrigin.y;
	TotalSpriteRect.right = TotalSpriteRect.left + TargetSprite->GetWidth();
	TotalSpriteRect.bottom = TotalSpriteRect.top + TargetSprite->GetHeight();

	RECT HitSpriteRect;
	HitSpriteRect.left = HitSpriteRect.top = 0;
	HitSpriteRect.right = HitSpriteRect.left + HitSprite->GetWidth();
	HitSpriteRect.bottom = HitSpriteRect.top + HitSprite->GetHeight();

	RECT TargetLockRect;
	RECT TargetLockRectClippingOffsets;
	RECT TargetLockRectZeroBased;

	//THIS SPRITE'S LOCK SIZE IS THE SIZE OF THE HIT SURFACE. CLIP IT AND WATCH OUT!
	TargetLockRect.left = ImpactOrigin.x;
	TargetLockRect.top = ImpactOrigin.y;
	TargetLockRect.bottom = TargetLockRect.top + HitSprite->GetHeight();
	TargetLockRect.right = TargetLockRect.left + HitSprite->GetWidth();

	RectClipping(TotalSpriteRect, TargetLockRect, TargetLockRectClippingOffsets);

	TargetLockRectZeroBased.left = TargetLockRect.left - mOrigin.x;
	TargetLockRectZeroBased.right = TargetLockRect.right - mOrigin.x;
	TargetLockRectZeroBased.top = TargetLockRect.top - mOrigin.y;
	TargetLockRectZeroBased.bottom = TargetLockRect.bottom - mOrigin.y;
	
	HitSpriteSurface->Lock(NULL, &HSSData, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL ); //LOCK ALL OF THIS SPRITE
	TargetSurface->Lock(&TargetLockRectZeroBased, &TSSData, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL );
	HitDetectSurface->Lock(&TargetLockRectZeroBased, &HDSData, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL );
	
	//WE HAVE THE BITS!
	BITFIELD32 HitField = OverlayBulletHitSprite(&TSSData, &HSSData, &HDSData, TargetLockRectZeroBased, HitSpriteRect, TargetLockRectClippingOffsets);
	TotalHits |= HitField;

	//WE'RE FINISHED HERE.
	HitSpriteSurface->Unlock(NULL);
	TargetSurface->Unlock(&TargetLockRectZeroBased);
	HitDetectSurface->Unlock(&TargetLockRectZeroBased);

	int Width = ToImpact->mHitWidth;
	int Height = ToImpact->mHitHeight;

	return HitField;
}

int Target::QueryForFinalScoreUponDeath()
{
	int ScoreToAdd = 0;

	if(REDSECTOR(TotalHits))
		ScoreToAdd = 10;
	else if(YELLOWSECTOR(TotalHits))
		ScoreToAdd = 8;
	else if(GREENSECTOR(TotalHits))
		ScoreToAdd = 6;
	else if(CYANSECTOR(TotalHits))
		ScoreToAdd = 4;

	return ScoreToAdd;
}

void Target::Draw(CDisplay& DisplayRef)
{
	if(CheckIsDead())
		return;
	TargetSprite->SetLoc(mOrigin);
	TargetSprite->Draw(DisplayRef);
}

void Target::Update(int Timediff)
{
	TargetSprite->UpdateFrames(Timediff);
}

bool Target::CheckIsDead()
{
	if(TargetSprite->GetState() == 1 && TargetSprite->HasPlayedOnce()) //IT'S DYING
	{
		return true;
	}
	return false;
}

void AddTarget(Target* NewTarget)
{
	Targets.push_front(NewTarget);
}

void ClearTargets()
{
	Target* ToKill;
	list<Target*>::iterator i;
	for(i = Targets.begin(); i != Targets.end(); i++)
	{
		
		
			ToKill = *i;
			//ONLY IF CHALLENGE REQUIRES IT.
			//ScoreBox.AddToScore(ToKill->QueryForFinalScoreUponDeath());
			TheChallenge->onTargetDeath(ToKill, true);
			delete ToKill;
	}

	Targets.clear();
}

void UpdateAllTargets(int Timediff, CDisplay& DisplayRef)
{
	Target* ToKill;
	list<Target*>::iterator i;
	int NumTimes = 0;
	for(i = Targets.begin(); i != Targets.end(); i++, NumTimes++)
	{
		(*i)->Update(Timediff);
		if((*i)->CheckIsDead()){
			ToKill = *i;
			//ONLY IF CHALLENGE REQUIRES IT.
			TheChallenge->onTargetDeath(ToKill, false);
			i = Targets.erase(i);
			delete ToKill;
		}
		else
			(*i)->Draw(DisplayRef);
	}
	//_asm nop;
}

int CheckForBulletImpacts(Bullet* ToImpact, POINT ImpactOrigin)
{
	list<Target*>::iterator i;
	int RetCode;
	for(i = Targets.begin(); i != Targets.end(); i++)
	{
		//ADD ONLY IF CHALLENGE REQUIRES IT.
		RetCode = ((*i)->BulletImpact(ToImpact, ImpactOrigin));
		if(RetCode != -1)
		{
			TheChallenge->onBulletHit(*i, ToImpact, RetCode);
			//ALSO JINK THE IMPACT ORIGIN.
			JinkBulletImpact(&ImpactOrigin);
		}
	}
	return -1;
}

void JinkBulletImpact(POINT* Origin)
{
	if(rand()%2)
		Origin->x -= rand()%5;
	else
		Origin->x += rand()%5;

	if(rand()%2)
		Origin->y -= rand()%5;
	else
		Origin->y += rand()%5;
}

void AddMediumTimedPaperTarget(CDisplay& DisplayRef)
{
	int Time = (rand()%(6000)) + 2000;
	POINT RandPt;

	Sprite* TargSprite = GetGlobalSpriteData(1002);
	if(!TargSprite)
		return;

	TimedTarget* Blah = new TimedTarget;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
//	POINT Center;
	RandPt.x = rand()%(Client.right - TargSprite->GetWidth());
	RandPt.y = rand()%(Client.bottom - TargSprite->GetHeight());
	Blah->Init(1002, 1003, RandPt, 600);
	Blah->SetTime(Time);
	AddTarget(Blah);
}

void AddClayPigeon(CDisplay& DisplayRef, double xSpeed, double ySpeed)
{
	Sprite* TargSprite = GetGlobalSpriteData(1004);
	if(!TargSprite)
		return;
	ClayPigeon* Blah = new ClayPigeon;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
	POINT LaunchPoint;
	POINT DiePoint;
	if(rand()%2)
	{
		LaunchPoint.x = -1 * TargSprite->GetWidth();
		DiePoint.x = Client.right;
		if(xSpeed < 0)
			xSpeed *= -1.0;
	}
	else
	{
		LaunchPoint.x = Client.right;
		DiePoint.x = -1 * TargSprite->GetWidth();
		if(xSpeed > 0)
			xSpeed *= -1.0;
	}
	LaunchPoint.y = rand()%(Client.bottom - (2*TargSprite->GetHeight())) + TargSprite->GetHeight();
	DiePoint.y = Client.bottom;

	Blah->Init(1004, 1005, LaunchPoint, 601);
	Blah->Launch(LaunchPoint, DiePoint, xSpeed, ySpeed);
	AddTarget(Blah);

	//PLAY A SOUND HERE!!! *twang*
	GameSounds.InsertSound(*GetGlobalSoundData(509));
}

void AddLargeCenteredPaperTarget(CDisplay& DisplayRef)
{
	Sprite* TargSprite = GetGlobalSpriteData(1000);
	if(!TargSprite)
		return;

	Target* Blah = new Target;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
	POINT Center;
	Center.y = (Client.bottom - Client.top) / 2;
	Center.x = (Client.right - Client.left) / 2;
	Center.x -= TargSprite->GetWidth() / 2;
	Center.y -= TargSprite->GetHeight() / 2;
	Blah->Init(1000, 1001, Center, 600);
	AddTarget(Blah);
}

void AddStandardPopupTarget(CDisplay& DisplayRef)
{
	int Time = (rand()%(1000)) + 500;
	POINT RandPt;

	Sprite* TargSprite = GetGlobalSpriteData(1006);
	if(!TargSprite)
		return;

	PopupTarget* Blah = new PopupTarget;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
//	POINT Center;
	RandPt.x = rand()%(Client.right - TargSprite->GetWidth());
	RandPt.y = rand()%(Client.bottom - TargSprite->GetHeight());
	Blah->Init(1006, 1007, RandPt, 600, 2, 30);
	Blah->SetTime(Time);
	GameSounds.InsertSound(*GetGlobalSoundData(510));
	AddTarget(Blah);
}

void AddBodyArmorPopupTarget(CDisplay& DisplayRef)
{
	int Time = (rand()%(1000)) + 500;
	POINT RandPt;

	Sprite* TargSprite = GetGlobalSpriteData(1008);
	if(!TargSprite)
		return;

	PopupTarget* Blah = new PopupTarget;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
//	POINT Center;
	RandPt.x = rand()%(Client.right - TargSprite->GetWidth());
	RandPt.y = rand()%(Client.bottom - TargSprite->GetHeight());
	Blah->Init(1008, 1009, RandPt, 600, 2, 30);
	Blah->SetTime(Time);
	GameSounds.InsertSound(*GetGlobalSoundData(510));
	AddTarget(Blah);
}

void AddHeadArmorPopupTarget(CDisplay& DisplayRef)
{
	int Time = (rand()%(1000)) + 500;
	POINT RandPt;

	Sprite* TargSprite = GetGlobalSpriteData(1010);
	if(!TargSprite)
		return;

	PopupTarget* Blah = new PopupTarget;
	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);
//	POINT Center;
	RandPt.x = rand()%(Client.right - TargSprite->GetWidth());
	RandPt.y = rand()%(Client.bottom - TargSprite->GetHeight());
	Blah->Init(1010, 1011, RandPt, 600, 2, 30);
	Blah->SetTime(Time);
	GameSounds.InsertSound(*GetGlobalSoundData(510));
	AddTarget(Blah);
}

void RectClipping(RECT& BigRect, RECT& SmallRect, RECT& ClippingOffsets)
{
	ClippingOffsets.bottom = ClippingOffsets.top = ClippingOffsets.right = ClippingOffsets.left = 0;

	if(SmallRect.left < BigRect.left)
	{
		ClippingOffsets.left = BigRect.left - SmallRect.left;
		SmallRect.left += ClippingOffsets.left;
	}
	if(SmallRect.right >= BigRect.right)
	{
		ClippingOffsets.right = SmallRect.right - BigRect.right;
		SmallRect.right -= ClippingOffsets.right;
	}
	if(SmallRect.top < BigRect.top)
	{
		ClippingOffsets.top = BigRect.top - SmallRect.top;
		SmallRect.top += ClippingOffsets.top;
	}
	if(SmallRect.bottom >= BigRect.bottom)
	{
		ClippingOffsets.bottom = SmallRect.bottom - BigRect.bottom;
		SmallRect.bottom -= ClippingOffsets.bottom;
	}
}

BITFIELD32 OverlayBulletHitSprite(LPDDSURFACEDESC2 TSSData, LPDDSURFACEDESC2 HSSData, LPDDSURFACEDESC2 HDSData, RECT& TargetZeroBased, RECT& HitZeroBased, RECT& ClipOffsets)
{
	DWORD* TargetPixels;
	LONG TargetPitch;
	DWORD* HitSpritePixels;
	LONG HitSpritePitch;
	DWORD* HitDetectPixels;
	LONG HitDetectPitch;
	BITFIELD32 Scores = 0;
	

	TargetPixels = (DWORD*) TSSData->lpSurface;
	TargetPitch = TSSData->lPitch;
	HitSpritePixels = (DWORD*) HSSData->lpSurface;
	HitSpritePitch = HSSData->lPitch;
	HitDetectPixels = (DWORD*) HDSData->lpSurface;
	HitDetectPitch = HDSData->lPitch;

	

	//LOOP THROUGH THE HIT SPRITE PIXELS.
	//FIRST, GET TO THE REAL TOP LEFT.
	
	DWORD Top,Left,HSTopLeft = 0,TSTopLeft = 0;
	
	for(Left = 0; Left < ClipOffsets.left; Left++)
		HSTopLeft+= 1;
	for(Top = 1; Top < ClipOffsets.top; Top++)
		HSTopLeft += HitSpritePitch / 4;
		

	//WE NOW HAVE OUR OFFSETS INTO BOTH BITMAPS.

	DWORD CurrentHSLine = HSTopLeft;
	DWORD CurrentTSLine = TSTopLeft;
	DWORD CurrentHDLine = TSTopLeft;
	DWORD* CurrentTargetPixelPtr = TargetPixels + CurrentTSLine;
	DWORD* CurrentHDPixelPtr = TargetPixels + CurrentTSLine;
	DWORD* CurrentHitPixelPtr = HitSpritePixels + CurrentHSLine;
	
	int w, h;
	for(h = ClipOffsets.top; h < HitZeroBased.bottom - ClipOffsets.bottom; h++)
	{
		for(w = ClipOffsets.left; w < HitZeroBased.right - ClipOffsets.right; w++)
		{
			ChangeHitOverlayColors(CurrentTargetPixelPtr, CurrentHitPixelPtr);
			if(*CurrentHitPixelPtr != 0xFFFF00FF && *CurrentHitPixelPtr != 0x00FF00FF && *CurrentTargetPixelPtr != 0xFFFF00FF && *CurrentTargetPixelPtr != 0x00FF00FF)
			Scores |= GetHitDetectionPixelValue(CurrentHDPixelPtr);
			CurrentTargetPixelPtr++;
			CurrentHitPixelPtr++;
			CurrentHDPixelPtr++;
		}
		CurrentTSLine += TargetPitch / 4;
		CurrentHDLine += HitDetectPitch / 4;
		CurrentTargetPixelPtr = TargetPixels + CurrentTSLine;
		CurrentHDPixelPtr = HitDetectPixels + CurrentHDLine;
		CurrentHSLine += HitSpritePitch / 4;
		CurrentHitPixelPtr = HitSpritePixels + CurrentHSLine;
	}
	return Scores;
}


/*********************************
//TIMED TARGET BEGIN
**********************************/
/*
class TimedTarget : public Target
{
private:
	int mTimeToLive;
public:
	TimedTarget() : Target(), mTimeToLive(5000) {}
	void SetTime(int TimeToLive);
	void Update(int Timediff);
	void Draw(CDisplay& DisplayRef);
	bool CheckIsDead();
};
*/

void TimedTarget::SetTime(int TimeToLive)
{
	mTimeToLive = TimeToLive;
}

void TimedTarget::Update(int TimeDiff)
{
	TargetSprite->UpdateFrames(TimeDiff);
	mTimeToLive -= TimeDiff;
}

bool TimedTarget::CheckIsDead()
{
	if(mTimeToLive <= 0)
		return true;
	return false;
}

void TimedTarget::Draw(CDisplay& DisplayRef)
{
	if(CheckIsDead())
		return;
	TargetSprite->SetLoc(mOrigin);
	TargetSprite->Draw(DisplayRef);
	char buf[250];
	sprintf(buf, "%d", mTimeToLive);
	//DRAW THE TIME ABOVE.
	POINT DPt;
	DPt.x = mOrigin.x;
	DPt.y = mOrigin.y - GetFontBySizeIndex(0)->GetCharHeight() - 1;
	GetFontBySizeIndex(0)->DrawString(buf, DPt);
}


/***********************************
/////////CLAY PIGEON BEGIN//////////
***********************************/
/*
class ClayPigeon : public Target
{
private:
	int mXToDieAt;
	double xSpeed;
	double ySpeed;
public:
	bool CheckIsDead();
	void Update(int Timediff);
	int BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin);
};
*/

bool ClayPigeon::CheckIsDead()
{
	if((TargetSprite->GetState() == 1 && TargetSprite->HasPlayedOnce()) || (xSpeed < 0 && mOrigin.x < mXToDieAt) || (xSpeed >= 0 && mOrigin.x > mXToDieAt) || mOrigin.y > mYToDieAt)
		return true;
	return false;
}

void ClayPigeon::Update(int TimeDiff)
{
	mOrigin.x += (xSpeed * TimeDiff)/ 10.0;
	mOrigin.y += (ySpeed * TimeDiff) / 10.0;
	ySpeed += (yGravity * TimeDiff) / 10.0;
	TargetSprite->UpdateFrames(TimeDiff);
}

int ClayPigeon::BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin)
{
	if(!HasImpacted(BulletToImpact, ImpactOrigin))
		return -1;
	if(TargetSprite->GetState() != 0 || TargetSprite->GetFrame() != 0)
		return -1;

	BITFIELD32 HitField = InternalBulletHit(BulletToImpact, ImpactOrigin);
	
	

		int ScoreToAdd = 0;

	if(REDSECTOR(HitField))
	{
		ScoreToAdd = 1;
		TargetSprite->SetState(1); //SET TO KILL STATE.
		TargetSprite->SetDelay(rand()%20 + 30);
		mStruck = true;
		//PLAY SOUND
	ASound* TheSound = GetGlobalSoundData(mSoundID);
	if(TheSound)
		GameSounds.InsertSound(*TheSound);
	}
	return ScoreToAdd;
}

void ClayPigeon::Launch(POINT From, POINT ToDieAt, double XSpeed, double YSpeed)
{
	mOrigin = From;
	mXToDieAt = ToDieAt.x;
	mYToDieAt = ToDieAt.y;
	xSpeed = XSpeed;
	ySpeed = YSpeed;
}

bool ClayPigeon::IsStruck()
{
	return mStruck;
}

/***********************************
////////POPUP TARGET BEGIN//////////
***********************************/
/*
class PopupTarget : public TimedTarget
{
public:
	void Update(int Timediff);
	void Draw(CDisplay& DisplayRef);
	bool CheckIsDead();
};
*/

void PopupTarget::Update(int Timediff)
{
	TargetSprite->UpdateFrames(Timediff);
	if(TargetSprite->GetState() == 2)//IT'S STILL OPENING.
	{
		if(TargetSprite->HasPlayedOnce()){
			TargetSprite->SetState(0); //THIS WILL FREEZE THE SPRITE IN OPEN.
		}
		else{
			return; //IT'S NOT READY TO TIME OR SCORE.
		}
	}
	else if(TargetSprite->GetState() == 0)
	{
	mTimeToLive -= Timediff;
	if(mTimeToLive <= 0)
	{
		TargetSprite->SetState(1);
		TargetSprite->SetDelay(30);
	}
	}
}

bool PopupTarget::CheckIsDead()
{
	if(TargetSprite->GetState() == 1 && TargetSprite->HasPlayedOnce())
		return true;
	return false;
}

int PopupTarget::BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin)
{
		if(!HasImpacted(BulletToImpact, ImpactOrigin))
		return -1;
	if(TargetSprite->GetState() != 0 || TargetSprite->GetFrame() != 0)
		return -1;

	BITFIELD32 HitField = InternalBulletHit(BulletToImpact, ImpactOrigin);
	
	int ScoreToAdd = 0;

	if(REDSECTOR(HitField))
	{
		ScoreToAdd = 1;
		TargetSprite->SetState(1); //SET TO KILL STATE.
		TargetSprite->SetDelay(30);
		mStruck = true;
		//PLAY SOUND
	GameSounds.InsertSound(*GetGlobalSoundData(602));
	GameSounds.InsertSound(*GetGlobalSoundData(511));
	for(int i = 0; i < 500; i++)
	AddBloodSparkParticle(ImpactOrigin);
	}
	else if(BLUESECTOR(HitField))
	{
	GameSounds.InsertSound(*GetGlobalSoundData(rand()%5 + 302));
	GameSounds.InsertSound(*GetGlobalSoundData(511));
	for(int i = 0; i < 15; i++)
	AddArmorSparkParticle(ImpactOrigin);
	}
	return ScoreToAdd;
}