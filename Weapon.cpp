#include <list>
using namespace std;

#include "stdafx.h"
#include "resource.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "sounds.h"
#include "scene.h"
#include "weapon.h"
#include "physicalobject.h"

#include "gameevents.h"
#include "globaldata.h"
#include "font.h"
#include "datafilepro.h"
#include "covertops.h"

Weapon* GameWeapons;
Bullet* GameBullets;
WeaponsSelector GameWeaponSelector;
extern GameEvents Events;
extern CursorPhysicsMachine CursorMachine;
int ClassesAllowed; //BITFIELD

#define WCLASS_I_BIT	1
#define WCLASS_II_BIT	2
#define WCLASS_III_BIT	4
#define WCLASS_IV_BIT	8
#define WCLASS_V_BIT	16

#define WCLASS_I	0
#define WCLASS_II	1
#define WCLASS_III	2
#define WCLASS_IV	3
#define WCLASS_V	4

#define WEAPONSELECTOR_BULLET_FONT_ID	2
#define WEAPONSELECTOR_BULLET_CURSOR_ID	808
#define WEAPONSELECTOR_BULLET_TYPESPEED	40
#define WEAPONSELECTOR_BULLET_CURSORDELAY	60
#define WEAPONSELECTOR_BULLET_CURSORDROPOFF	2000
#define WEAPONSELECTOR_BULLET_TYPESOUND		504
#define WEAPONSELECTOR_BULLET_ENDLINETYPESOUND	505

int GameWeaponCount = 0;
int GameBulletCount = 0;

extern SoundQueue GameSounds;
extern CDisplay TheDisplay;

#define DATALINESPERWEAPON	17
#define DATALINESPERBULLET	11

void AllowAllWeapons()
{
	ClassesAllowed |= WCLASS_I_BIT | WCLASS_II_BIT | WCLASS_III_BIT | WCLASS_IV_BIT | WCLASS_V_BIT;
}

void DisallowWeaponClass(int TypeToDisallow, int* CurrentWeaponID, Weapon** CurrentWeaponToFill)
{
	int BitToCheck = 1 << TypeToDisallow;
	ClassesAllowed &= ~BitToCheck;
	if((*CurrentWeaponToFill)->mClass == TypeToDisallow)
		*CurrentWeaponID = ChangeToNextWeapon(*CurrentWeaponID, CurrentWeaponToFill);
}

void AllowWeaponClass(int TypeToAllow)
{
	int BitToCheck = 1 << TypeToAllow;
	ClassesAllowed |= BitToCheck;
}

bool IsWeaponAllowed(Weapon* TheWeapon)
{
	int Class = TheWeapon->mClass;
	int BitToCheck = 1 << Class;

	if(ClassesAllowed & BitToCheck)
		return true;
	return false;
}

bool InitializeGameBullets()
{
	DataFilePro* DFP = DFPOpenDataFile("bullets.dat", DFP_FILE_READONLY);
	if(!DFP)
		return false;
	int NumLines = DFP->GetNumLines();
	if(NumLines % DATALINESPERBULLET){
		DFPCloseDataFile(DFP);
		return false;
	}
	GameBulletCount = NumLines / DATALINESPERBULLET;
	GameBullets = new Bullet[GameBulletCount];
	for(int i = 0; i < GameBulletCount; i++)
	{
		DFP->t_GetUnboundedString(&GameBullets[i].mName);
		GameBullets[i].mBulletDataSpriteGlobalGraphicsID = DFP->t_GetInteger();
		GameBullets[i].mFullBulletSpriteID = DFP->t_GetInteger();
		GameBullets[i].mTravelGlobalGraphicsID = DFP->t_GetInteger();
		GameBullets[i].mTravelDelayPerFrame = DFP->t_GetInteger();
		GameBullets[i].mHitWidth = DFP->t_GetInteger();
		GameBullets[i].mHitHeight = DFP->t_GetInteger();
		GameBullets[i].mHitGlobalGraphicsID = DFP->t_GetInteger();
		GameBullets[i].mHitGlobalSoundID = DFP->t_GetInteger();
		GameBullets[i].mShellCasingGlobalGraphicsID = DFP->t_GetInteger();
		GameBullets[i].mShellCasingBounceSoundID = DFP->t_GetInteger();
	}
	DFPCloseDataFile(DFP);
	return true;
}
/*

	char* mName;
	int mWeaponDataSpriteGlobalGraphicsID;
	bool mUseShotEffects;
	bool mSingleShot;
	int mTimeToNextShot;
	int mNextShotTimer;
	bool mClipLoaded;
	int mMaxRounds;
	int mTimeToReload;
	int mReloadTimer;
	double mHorizRecoil;
	double mVertRecoil;
	int mShellCasingGlobalGraphicsID;
	int mShellCasingBounceSoundID;
	int mGunshotGlobalSoundID;
	int mReloadGlobalSoundID;
	int mReloadRackGlobalSoundID;
	int mBulletID;
	*/
bool InitializeGameWeapons()
{
		DataFilePro* DFP = DFPOpenDataFile("weapons.dat", DFP_FILE_READONLY);
		if(!DFP)
			return false;
	int NumLines = DFP->GetNumLines();
	if(NumLines % DATALINESPERWEAPON){
		DFPCloseDataFile(DFP);
		return false;
	}
	GameWeaponCount = NumLines / DATALINESPERWEAPON;
	GameWeapons = new Weapon[GameWeaponCount];
	for(int i = 0; i < GameWeaponCount; i++)
	{
		DFP->t_GetUnboundedString(&GameWeapons[i].mName);
		GameWeapons[i].mClass = DFP->t_GetInteger();
		GameWeapons[i].mWeaponDataSpriteGlobalGraphicsID = DFP->t_GetInteger();
		GameWeapons[i].mUseShotEffects = DFP->t_GetBool();
		GameWeapons[i].mSingleShot = DFP->t_GetBool();
		GameWeapons[i].mTimeToNextShot = DFP->t_GetInteger();
		GameWeapons[i].mNextShotTimer = 0;
		GameWeapons[i].mClipLoaded = DFP->t_GetBool();
		GameWeapons[i].mMaxRounds = DFP->t_GetInteger();
		GameWeapons[i].mCurrentRounds = GameWeapons[i].mMaxRounds;
		GameWeapons[i].mTimeToReload = DFP->t_GetInteger();
		GameWeapons[i].mReloadTimer = GameWeapons[i].mTimeToReload;
		GameWeapons[i].mHorizRecoil = DFP->t_GetDouble();
		GameWeapons[i].mVertRecoil = DFP->t_GetDouble();
		//GameWeapons[i].mWeight = DFP->t_GetDouble();
		GameWeapons[i].mGunshotGlobalSoundID = DFP->t_GetInteger();
		GameWeapons[i].mReloadGlobalSoundID = DFP->t_GetInteger();
		GameWeapons[i].mReloadRackGlobalSoundID = DFP->t_GetInteger();
		GameWeapons[i].mBulletID = DFP->t_GetInteger();
		GameWeapons[i].mEjectRoundWhenFired = DFP->t_GetBool();
		GameWeapons[i].mEjectAllRoundsWhenReloaded = DFP->t_GetBool();
		GameWeapons[i].mActionCycled = false;
	}
	DFPCloseDataFile(DFP);
	return true;
}

void DestroyGameWeaponsAndBullets()
{
	delete [] GameWeapons;
	delete [] GameBullets;
}

bool ChangeWeapon(int To, Weapon** ToFill)
{
	To %= GameWeaponCount;
	
	if(*ToFill)
		delete *ToFill;
	*ToFill = new Weapon(GameWeapons[To]);
	//GameSounds.InsertSound(*GetGlobalSoundData((*ToFill)->mReloadGlobalSoundID));
	CursorMachine.SetAccel(GetWeaponClassAccelRate((*ToFill)->mClass));
	CursorMachine.SetDrag(GetWeaponClassDragRate((*ToFill)->mClass));
	ActivateWeaponSelector(*ToFill);
	SwitchToReloadCursor(TheDisplay);
	return true;
}

double GetWeaponClassAccelRate(int Class)
{
	switch(Class)
	{
		default:
	case WCLASS_I:
		return 0.007;
	case WCLASS_II:
		return 0.006;
	case WCLASS_III:
		return 0.006;
	case WCLASS_IV:
		return 0.005;
	case WCLASS_V:
		return 0.004;
	}
}

double GetWeaponClassDragRate(int Class)
{
	switch(Class)
	{
		default:
	case WCLASS_I:
		return 0.4;
	case WCLASS_II:
		return 0.3;
	case WCLASS_III:
		return 0.2;
	case WCLASS_IV:
		return 0.15;
	case WCLASS_V:
		return 0.1;
	}
}

int ChangeToNextWeapon(int Current, Weapon** ToFill, bool Prev)
{
	int Next = Current;
	do
	{
		if(Prev)
			Next--;
		else
			Next++;
		if(Next < 0)
			Next = GameWeaponCount - 1;
		else
		Next %= GameWeaponCount;
		if(IsWeaponAllowed(GetGlobalWeaponData(Next)))
		{
			ChangeWeapon(Next, ToFill);
			return Next;
		}
	}while(Next != Current);
	return Current;
}

bool CanWeaponFire(Weapon* ToFire)
{
	if(ToFire->mCurrentRounds < 1)
		return false;
	if(ToFire->mReloadTimer > 0)
		return false;
	if(ToFire->mNextShotTimer > 0)
		return false;
	if(ToFire->mSingleShot && ToFire->mActionCycled)
		return false;
	return true;
}

void WeaponUpdate(Weapon* ToUpdate, int timediff)
{
	if(ToUpdate->mReloadTimer > 0){
		ToUpdate->mNextShotTimer = 0;
		ToUpdate->mReloadTimer -= timediff;
		//CHECK IF THIS MADE IT RUN OUT.
		if(ToUpdate->mReloadTimer <= 0){
			ToUpdate->mReloadTimer = 0;
			ToUpdate->mCurrentRounds = ToUpdate->mMaxRounds;
			if(ToUpdate->mBoltBack)
				GameSounds.InsertSound(*GetGlobalSoundData(ToUpdate->mReloadRackGlobalSoundID));
			Events.ChangeCursorDrawString("");
			SwitchToFireCursor(TheDisplay);
		}
	}

	else if(ToUpdate->mNextShotTimer > 0){
		ToUpdate->mNextShotTimer -= timediff;
		if(ToUpdate->mNextShotTimer <= 0){
			ToUpdate->mReloadTimer = 0;
		}
	}

	if(!ToUpdate->mTriggerDown)
	{
		ToUpdate->mActionCycled = false;
	}

	DrawBulletsRemaining(ToUpdate, timediff);
}

void DrawBulletsRemaining(Weapon* DrawMe, int timediff)
{
	Sprite* BulletSprite;
	Sprite* MySprite;
	Bullet* TheBullet;
	char* Name = "INVALID BULLETS";
	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);

	
	TheBullet = GetGlobalBulletData(DrawMe->mBulletID);
	if(!TheBullet)
		return;
	Name = TheBullet->mName;
	BulletSprite = GetGlobalSpriteData(TheBullet->mFullBulletSpriteID);
	if(!BulletSprite)
		return;

	MySprite = new Sprite;
	MySprite->InitializeSpriteCopy(BulletSprite);
	POINT FontOrigin;
	FontOrigin.x = Client.right - DrawMe->mMaxRounds * BulletSprite->GetWidth();
	int StrWidth = GetFontBySizeIndex(WEAPONSELECTOR_BULLET_FONT_ID)->GetStringWidth(Name);
	if(FontOrigin.x + StrWidth > Client.right)
		FontOrigin.x -= (FontOrigin.x + StrWidth) - Client.right + 20;
	FontOrigin.y = Client.bottom - BulletSprite->GetHeight() - GetFontBySizeIndex(WEAPONSELECTOR_BULLET_FONT_ID)->GetCharHeight() - 5;
	int FullWidth = DrawMe->mCurrentRounds * BulletSprite->GetWidth();
	if(FullWidth > Client.right - Client.left)
		return;
	GameWeaponSelector.BulletName->SetLoc(FontOrigin);
	GameWeaponSelector.BulletName->SetWidth(Client.right - Client.left);
	int FullHeight = BulletSprite->GetHeight();
	POINT Loc;
	Loc.x = Client.right - FullWidth;
	Loc.y = Client.bottom - FullHeight;
	for(int i = 0; i < DrawMe->mCurrentRounds; i++)
	{
		MySprite->SetLoc(Loc);
		MySprite->Draw(TheDisplay);
		Loc.x += BulletSprite->GetWidth();
	}
	GameWeaponSelector.BulletName->Update(timediff);
	GameWeaponSelector.BulletName->Draw(TheDisplay);
	delete MySprite;
}

FPOINT Recoil(POINT oldpoint, Weapon* weap)
{
	FPOINT NewPoint;
	double xscalar;
	double yscalar;
	if(rand()%2)
		xscalar = -1.0;
	else
		xscalar = 1.0;

	if(rand()%2)
		yscalar = -1.0;
	else
		yscalar = 1.0;

	NewPoint.x = ((weap->mHorizRecoil / 1.5) * xscalar);
	NewPoint.y = ((weap->mVertRecoil / 1.5) * yscalar);

	return NewPoint;
}

Weapon* GetGlobalWeaponData(int Windex)
{
	if(Windex >= 0 && Windex < GameWeaponCount)
	return &GameWeapons[Windex];
	return NULL;
}

Bullet* GetGlobalBulletData(int Bindex)
{
	if(Bindex >= 0 && Bindex < GameBulletCount)
	return &GameBullets[Bindex];
	return NULL;
}

//void ActivateWeaponsSelector
void UpdateWeaponsSelector(Weapon* TheWeapon, int timediff, CDisplay& DisplayRef)
{
	GameWeaponSelector.Update(TheWeapon, timediff, DisplayRef);
}

bool InitializeWeaponSelector(Weapon* TheWeapon)
{
	bool returnval;
	returnval = GameWeaponSelector.Init(TheWeapon);
	AllowAllWeapons();
	return returnval;
}

void ActivateWeaponSelector(Weapon* TheWeapon)
{
	GameWeaponSelector.UpdateWithNewData(TheWeapon);
	Events.ChangeCursorDrawString("ACTIVATING WEAPON");
	ShowWeaponSelector();
}

void ShowWeaponSelector()
{
	if(GameWeaponSelector.GetStatus() != 1){
		GameWeaponSelector.SetStatus(1);
	}
}

void DeactivateWeaponSelector()
{
	if(GameWeaponSelector.GetStatus() != 2){
		GameWeaponSelector.SetStatus(2);
		//PLAY SOUND!
		GameSounds.InsertSound(*GetGlobalSoundData(502));
	}
}

void WeaponsSelector::UpdateWithNewData(Weapon* NewWeapon)
{
	
	if(WeaponSprite)
		delete WeaponSprite;
	WeaponSprite = new Sprite;
	WeaponSprite->InitializeSpriteCopy(GetGlobalSpriteData(NewWeapon->mWeaponDataSpriteGlobalGraphicsID));
	if(BulletName)
		delete BulletName;
	BulletName = new TypewriterText;
	Bullet* GB = GetGlobalBulletData(NewWeapon->mBulletID);
	char* Name = "INVALID BULLETS";
	if(GB)
		Name = GB->mName;
	POINT Z; Z.x = Z.y = 0;
	BulletName->Initialize(Name, Z, WEAPONSELECTOR_BULLET_TYPESPEED, 0, WEAPONSELECTOR_BULLET_FONT_ID, WEAPONSELECTOR_BULLET_CURSOR_ID, 
	WEAPONSELECTOR_BULLET_CURSORDELAY, WEAPONSELECTOR_BULLET_CURSORDROPOFF, WEAPONSELECTOR_BULLET_TYPESOUND,WEAPONSELECTOR_BULLET_ENDLINETYPESOUND);
}

bool WeaponsSelector::Init(Weapon* TheWeapon)
{
	
	for(int i = 0; i < NUMWEAPONCLASSES; i++)
	{
		WeaponClassSprites[i].InitializeSpriteCopy(GetGlobalSpriteData(FIRSTWEAPONCLASSSPRITEID + i));
	}
	AddError("WeaponSelector: All Class Sprite Copies Initialized");
	BaseSelector = new Sprite;
	AddError("WeaponSelector: Selector Base Sprite Allocated");
	Sprite* Blah = GetGlobalSpriteData(801);
	if(!Blah){
		AddError("WeaponSelector: Selector Base Sprite NOT FOUND IN GLOBAL DATA");
		return false;
	}
			AddError("WeaponSelector: Selector Base Sprite FOUND");
	BaseSelector->InitializeSpriteCopy(Blah);
	AddError("WeaponSelector: Selector Base Sprite INITIALIZED");
	
	WeaponSprite = new Sprite;
	AddError("WeaponSelector: Weapon Sprite Allocated");
	Sprite* WeapBlah = GetGlobalSpriteData(TheWeapon->mWeaponDataSpriteGlobalGraphicsID);
	if(!WeapBlah){
		AddError("WeaponSelector: Weapon Sprite NOT FOUND IN GLOBAL DATA");
		return false;
	}

	AddError("WeaponSelector: Weapon Sprite FOUND");
	WeaponSprite->InitializeSpriteCopy(WeapBlah);
	AddError("WeaponSelector: Weapon Sprite INITIALIZED");
	


	mStatus = 1;
	mXOrigin = (BaseSelector->GetWidth() * -1);
	mYOrigin = 0;

	BulletName = new TypewriterText;
	Bullet* GB = GetGlobalBulletData(TheWeapon->mBulletID);
	char* Name = "INVALID BULLETS";
	if(GB){
		Name = GB->mName;
		AddError("WeaponSelector: Bullet Name Set");
	}
	else{
		AddError("WeaponSelector: Bullet Name NOT FOUND");
	}
	POINT Z; Z.x = Z.y = 0;
	BulletName->Initialize(Name, Z, WEAPONSELECTOR_BULLET_TYPESPEED, 0, WEAPONSELECTOR_BULLET_FONT_ID, WEAPONSELECTOR_BULLET_CURSOR_ID, 
	WEAPONSELECTOR_BULLET_CURSORDELAY, WEAPONSELECTOR_BULLET_CURSORDROPOFF, WEAPONSELECTOR_BULLET_TYPESOUND,WEAPONSELECTOR_BULLET_ENDLINETYPESOUND);
	AddError("WeaponSelector: Bullet Name Graphical Text Initialized OK");
	return true;
}
void WeaponsSelector::Update(Weapon* TheWeapon, int timediff, CDisplay& DisplayRef)
{
	POINT Loc;
	switch(mStatus)
	{
	case 0:
		mActiveTimer -= timediff;
		if(mActiveTimer <= 0)
			DeactivateWeaponSelector();
		Draw(TheWeapon,DisplayRef);
		break;
	case 3:
		Draw(TheWeapon, DisplayRef);
		break;
	case 1:
		if(mXOrigin >= 0)
		{
			GameSounds.InsertSound(*GetGlobalSoundData(501));
			mXOrigin = 0;
			mStatus = 0;
		}
		else{
		mXOrigin += timediff * 2;
		Loc.x = mXOrigin;
	Loc.y = mYOrigin;
	BaseSelector->SetLoc(Loc);
		}
		
		Draw(TheWeapon, DisplayRef);
		mActiveTimer = mActiveTime;
		break;
	case 2:
		mXOrigin -= timediff * 2;
		Loc.x = mXOrigin;
	Loc.y = mYOrigin;
	BaseSelector->SetLoc(Loc);
		if(mXOrigin < (BaseSelector->GetWidth() * -1))
		{
			//GameSounds.InsertSound(*GetGlobalSoundData(503));
			mStatus = 3;
			mXOrigin = (BaseSelector->GetWidth() * -1);
		}
		else
			Draw(TheWeapon,DisplayRef);
		break;
	}
	
}

void WeaponsSelector::Draw(Weapon* TheWeapon, CDisplay& DisplayRef)
{
	//DRAW INITIAL SELECTOR
	POINT Loc;
	Loc.x = mXOrigin;
	Loc.y = mYOrigin;
	BaseSelector->SetLoc(Loc);
	BaseSelector->Draw(DisplayRef);
	//DRAW TEXT
	int HCenter = mXOrigin + (BaseSelector->GetWidth() / 2);
	int ActualCenter = HCenter - (GetFontBySizeIndex(0)->GetStringWidth(TheWeapon->mName) / 2);
	int YCenter = mYOrigin + BaseSelector->GetHeight() - 40 - GetFontBySizeIndex(0)->GetCharHeight();
	
	Loc.x = ActualCenter;
	Loc.y = YCenter;
	GetFontBySizeIndex(0)->DrawString(TheWeapon->mName, Loc);
	//DRAW WEAPON
	POINT NewLoc;
	NewLoc.x = (HCenter - WeaponSprite->GetWidth() / 2) - 10;
	NewLoc.y = mYOrigin + 60;
	WeaponSprite->SetLoc(NewLoc);
	WeaponSprite->Draw(DisplayRef);
	int XOriginClass = BaseSelector->GetWidth() - 68;
	int YOriginClass = BaseSelector->GetHeight() - 201;
	NewLoc.x = XOriginClass + mXOrigin;
	NewLoc.y = YOriginClass + mYOrigin;
	if(TheWeapon->mClass >= 0 && TheWeapon->mClass < NUMWEAPONCLASSES)
	{
		WeaponClassSprites[TheWeapon->mClass].SetLoc(NewLoc);
		WeaponClassSprites[TheWeapon->mClass].Draw(DisplayRef);
	}
}
