#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "stdafx.h"
#include "scene.h"
#include <list>
using namespace std;
#include "sounds.h"
#include "weapon.h"
#include "physicalobject.h"
#include "font.h"
#include "GameEvents.h"
#include "Target.h"


GameEvents Events;
extern SoundQueue GameSounds;
extern CDisplay TheDisplay;
extern HWND hwnd;
extern Weapon* PlayerWeapon;
extern CursorPhysicsMachine CursorMachine;

void GameEvents::AllowGunShots()
{
	mGunShotsAllowed = true;
}

void GameEvents::DisallowGunShots()
{
	mGunShotsAllowed = false;
}

void GameEvents::DrawCursorString(POINT cursor)
{
	if(CursorDrawString && strcmp(CursorDrawString, ""))
		GetFontBySizeIndex(2)->DrawString(CursorDrawString, cursor);
}

void GameEvents::Update(int timediff, POINT cursorpos, bool LMBDOWN, bool RMBDOWN)
{
	if(RMBDOWN)
	{
		
		if(mGunShot.ReloadGo(cursorpos))
			ChangeCursorDrawString("RELOADING WEAPON");
	}
	if(LMBDOWN && mGunShotsAllowed)
		PlayerWeapon->mTriggerDown = true;
	else
		PlayerWeapon->mTriggerDown = false;
	
	if(mGunShotActive){
		mGunShotActive = mGunShot.Update(timediff, cursorpos, LMBDOWN);
		mScreenFlashColor = RGB_TORGBA(RGB(mGunShot.FlashLevel, mGunShot.FlashLevel,mGunShot.FlashLevel));
	}
	else if(LMBDOWN && mGunShotsAllowed)
		GunShot(cursorpos);
	else{
		PlayerWeapon->mTriggerDown = false;
		mScreenFlashColor = 0;
	}
	DrawCursorString(cursorpos);
}


void GameEvents::UpdateNotificationString(int timediff)
{
	mNotification.Update(timediff);
}

void GameEvents::ChangeNotificationString(char* Str, int HoldTime)
{
	mNotification.ChangeString(Str, HoldTime);
}

///////////////PLAYER GUN SHOT
bool GunShotEvent::Update(int timediff, POINT cursorpos, bool LMBDOWN)
{
	int toremove = timediff;
	int NFlash = FlashLevel;
	FlashLevel -= toremove;
	if(FlashLevel - toremove < 0)
	{
		FlashLevel = 0;
		return false;
	}
	
	if(!MuzzleFlashDone){
		MuzzleFlash->UpdateFrames(timediff);
		if(MuzzleFlash->GetFrame() >= MuzzleFlash->GetNumFramesInCurrentState() - 1)
		{
			MuzzleFlashDone = true;
		}
		if(!MuzzleFlashDone || MuzzleFlash->GetFrame() != 0)
		{
			MuzzleFlash->Draw(TheDisplay);
		}
	}
	if(PlayerWeapon && PlayerWeapon->mTriggerDown && CanWeaponFire(PlayerWeapon))
	{
		GunShotGo(cursorpos);
	}
	return true;
}

bool GunShotEvent::ReloadGo(POINT cursorpos)
{
	if(PlayerWeapon->mReloadTimer < 1 && PlayerWeapon->mMaxRounds != PlayerWeapon->mCurrentRounds){
		GameSounds.InsertSound(*GetGlobalSoundData(PlayerWeapon->mReloadGlobalSoundID));
		PlayerWeapon->mReloadTimer = PlayerWeapon->mTimeToReload;
		if(PlayerWeapon->mCurrentRounds > 0){
			PlayerWeapon->mReloadTimer -= 500; //NO BOLT BACK OR ANYTHING.
			PlayerWeapon->mBoltBack = false;
		}
		else{
			PlayerWeapon->mBoltBack = true;
		}
		if(PlayerWeapon->mEjectAllRoundsWhenReloaded){
			for(int i = 0; i < PlayerWeapon->mMaxRounds; i++){
				SpawnShellCasing(cursorpos, PlayerWeapon);
			}
		}
		SwitchToReloadCursor(TheDisplay);
		return true;
	}
	return false;
}

void GunShotEvent::GunShotGo(POINT cursorpos) //FIRE WEAPON!
{
	if(PlayerWeapon->mUseShotEffects){
		if(MuzzleFlash)
			delete MuzzleFlash;

		MuzzleFlash = new Sprite;
		MuzzleFlash->InitializeSpriteCopy(GetGlobalSpriteData(800));
		MuzzleFlash->SetDelay(40);
		POINT CP;
		CP.x = cursorpos.x - MuzzleFlash->GetWidth() / 2;
		CP.y = cursorpos.y - MuzzleFlash->GetHeight() / 2;
		MuzzleFlash->SetLoc(CP);
		MuzzleFlashDone = false;
		if(PlayerWeapon->mEjectRoundWhenFired)
		SpawnShellCasing(CP, PlayerWeapon);
	}
	DeactivateWeaponSelector();
	PlayerWeapon->mTriggerDown = true;
	PlayerWeapon->mActionCycled = true;
	PlayerWeapon->mNextShotTimer = PlayerWeapon->mTimeToNextShot;
	PlayerWeapon->mCurrentRounds--;
	if(!PlayerWeapon->mCurrentRounds)
	{
		Events.ChangeCursorDrawString("WEAPON EMPTY"); 
		SwitchToOutCursor(TheDisplay);
	}
	POINT OldPt;
	GetCursorPos(&OldPt);

	Bullet* TheBullet = GetGlobalBulletData(PlayerWeapon->mBulletID);

	POINT BulletOrigin;
	BulletOrigin = OldPt;
	ScreenToClient(TheDisplay.GetHWnd(), &BulletOrigin);
	
	if(TheBullet){
		BulletOrigin.x -= TheBullet->mHitWidth / 2;
		BulletOrigin.y -= TheBullet->mHitHeight / 2;
	}

	
	
	CursorMachine.SetInMotion(Recoil(OldPt, PlayerWeapon));
	GameSounds.InsertSound(*GetGlobalSoundData(PlayerWeapon->mGunshotGlobalSoundID));

	CheckForBulletImpacts(TheBullet, BulletOrigin);
}

void GunShot(POINT cursorpos) //START TRYING TO SHOOT.
{
	
	if(PlayerWeapon && CanWeaponFire(PlayerWeapon))
	{
		Events.mGunShotActive = true;
		Events.mGunShot.FlashLevel = (UCHAR)0;
	}
}


void NotificationStringEvent::Update(int timediff)
{
	switch(mStatus)
	{
	case 0: //FULL OPEN.
		mHoldTimer -= timediff;
		if(mHoldTimer <= 0)
			mStatus = 2; //CLOSE IT.
		break;
	case 1:
		mXOrigin -= timediff * 3; //OPEN IT FROM THE RIGHT.
		if(mXOrigin <= mXStop)
		{
			mXOrigin = mXStop;
			mStatus = 0; //HOLD IT!
		}
		break;
	case 2:
		mXOrigin += timediff * 3;
		if(mXOrigin >= mXCloseStop)
		{
			mXOrigin = mXCloseStop;
			mStatus = 3;
		}
		break;
	case 3:
		break;
	}
	NotificationStringEvent::Draw();
}

void NotificationStringEvent::ChangeString(char* Str, int HoldTime)
{
	if(String)
		delete [] String;
	String = new char[strlen(Str)+1];
	strcpy(String, Str);
	mHoldTimer = HoldTime;

	//NOW CALCULATE mXStop.
	int StringWidth = GetFontBySizeIndex(1)->GetStringWidth(Str);
	int StringHeight = GetFontBySizeIndex(1)->GetCharHeight();
	//SINCE THE STRING CENTERS, WE WANT ABSOLUTE CENTER - 1/2 STRING WIDTH.
	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);
	int AbsoluteCenter = ((Client.right - Client.left) + Client.left) / 2;
	mXStop = AbsoluteCenter - (StringWidth / 2);
	mXCloseStop = Client.right;
	mXOrigin = Client.right;
	mYOrigin = ((Client.bottom - Client.top) + Client.top) / 2;
	mYOrigin -= StringHeight / 2;
	mStatus = 1; //SET TO OPENING.
	
}
void NotificationStringEvent::Draw()
{
	if(String)
	{
	POINT Loc;
	Loc.x = mXOrigin;
	Loc.y = mYOrigin;
	GetFontBySizeIndex(1)->DrawString(String, Loc);
	}
}
