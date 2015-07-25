#include "stdafx.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "Sprite.h"
#include <list>
using namespace std;
#include "sounds.h"
#include "scene.h"
#include "weapon.h"
#include "PhysicalObject.h"

#include "globaldata.h"


extern SoundQueue GameSounds;
extern CDisplay TheDisplay;

int UpdateSpeed = 15;
int ScaleDown = 1;
double PhysicsTimeScaleUp  = 8.0;
int UpdateTimer = UpdateSpeed;
int GameObject::NextIndex = 0;

list <BounceObject*> BounceObjectList;
//list <Sprite> TravelBulletList;

void UpdateBounceObjects(int timediff)
{
	bool Update = false;
	UpdateTimer -= timediff;
	int NumTimes = 0;
	timediff /= ScaleDown;
	if(UpdateTimer < 0)
		Update = true;

	BounceObject* TheObj;
	
	list<BounceObject*>::iterator i;
	for(i = BounceObjectList.begin(); i != BounceObjectList.end(); i++, NumTimes++)
	{
		TheObj = (*i);
		if(Update){
		TheObj->GetSprite()->UpdateFrames(timediff);
		TheObj->Update(timediff);
		}
		if(TheObj->IsDead())
		{
			i = BounceObjectList.erase(i);
			delete TheObj;
		}
		else
			TheObj->GetSprite()->Draw(TheDisplay);
	}

	if(Update)
		UpdateTimer = UpdateSpeed;
	
}

void ChangeShellCasingUpdateSpeed(int Speed)
{
	ScaleDown = Speed;
}

int GetShellCasingUpdateSpeed()
{
	return ScaleDown;
}

void PhysObject::PhysicsUpdate(int timediff)
{
	mYAccel += (mGravity * (double)timediff) / PhysicsTimeScaleUp;
	mYSpeed += (mYAccel * (double) timediff) / PhysicsTimeScaleUp;
	mXSpeed += (mXAccel * (double) timediff) / PhysicsTimeScaleUp;
}

bool BounceObject::BounceUpdate(int timediff)
{
	if(PhysicsAffected){
	PhysicsUpdate(timediff);
	}
	PositionUpdate(timediff);

	bool bounced = false;
	if(GetSprite()->GetWidth() + GetSprite()->mXOrigin > RightBound - 1)
	{ mXSpeed *= -1; bounced = true; }
	if(GetSprite()->GetHeight() + GetSprite()->mYOrigin > LowerBound - 1)
	{ mYSpeed *= -1; bounced = true; }
	if(GetSprite()->mXOrigin < LeftBound + 1)
	{ mXSpeed *= -1; bounced = true; }
	if(GetSprite()->mYOrigin < TopBound + 1)
	{ mYSpeed *= -1; bounced = true; }
	
	return bounced;
}

void BounceObject::Update(int timediff)
{
	if(BounceUpdate(timediff))
	{
		NumBounces++;
		if(BounceSound)
		GameSounds.InsertSound(*BounceSound);
		if(NumBounces > 1)
			SetTimeToLive(0);
	}
	
}

void BounceObject::SetBounds(RECT& BoundRect)
{
	LeftBound = BoundRect.left;
	RightBound = BoundRect.right;
	TopBound = BoundRect.top;
	LowerBound = BoundRect.bottom;
}

void GameObject::PositionUpdate(int timediff)
{
	GetSprite()->mXOrigin += (mXSpeed * (double)timediff) / PhysicsTimeScaleUp;
	GetSprite()->mYOrigin += (mYSpeed * (double)timediff) / PhysicsTimeScaleUp;
	TimeToLive -= timediff;
}

void GameObject::Update(int timediff)
{

}

bool GameObject::IsDead()
{
	return (TimeToLive <= 0);
}

void SpawnShellCasing(POINT spawn, Weapon* Weap)
{
	Bullet* TheBullet = GetGlobalBulletData(Weap->mBulletID);
	if(!TheBullet)
		return;
	BounceObject* SC = new BounceObject(*GetGlobalSoundData(TheBullet->mShellCasingBounceSoundID));
	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);
	SC->SetBounds(Client);
	SC->SetPhysics(true);
	SC->mGravity = 0.2;
	SC->mXSpeed = 15.5 + rand()%10; 
	SC->mYSpeed = -16.5 - rand()%10;
	SC->SetTimeToLive(rand()%200 + 800);
	SC->GetSprite()->InitializeSpriteCopy(GetGlobalSpriteData(TheBullet->mShellCasingGlobalGraphicsID));
	spawn.x -= SC->GetSprite()->GetWidth() / 2;
	spawn.y -= SC->GetSprite()->GetHeight() / 2;
	SC->GetSprite()->SetLoc(spawn);
	SC->GetSprite()->SetFrame(rand()%SC->GetSprite()->GetNumFramesInCurrentState());
	SC->GetSprite()->SetDelay(rand()%10 + 5);
	BounceObjectList.push_front(SC);
}
