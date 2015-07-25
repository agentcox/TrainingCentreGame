#include <list>
using namespace std;
#include <time.h>
#include <stdlib.h>
#include "stdafx.h"
#include "resource.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "Particle.h"

extern int WindowWidth;
extern int WindowHeight;
extern CDisplay TheDisplay;
bool AlphaBlending = true; //ON BY DEFAULT

HDC MainDC;

list<Particle*> ParticleList;

void BeginDrawingParticles()
{
	TheDisplay.GetFrontBuffer()->GetDC(&MainDC);
}

void EndDrawingParticles()
{
	TheDisplay.GetFrontBuffer()->ReleaseDC(MainDC);
}

void UpdateAndDrawAllParticles(int TimeDiff)
{
	
	Particle* Noooo;
	list<Particle*>::iterator i = ParticleList.begin();
	if(i == ParticleList.end())
		return;
	BeginDrawingParticles();
	for(; i != ParticleList.end(); i++)
	{
		(*i)->Update(TimeDiff);
		if((*i)->IsDead()){
			Noooo = (*i);
			i = ParticleList.erase(i);
			delete Noooo;
		}
		else{
		(*i)->Draw(MainDC);
		}
	}
	EndDrawingParticles();
}

void InitParticleSystem()
{
}

void FreeParticleSystem()
{
	ParticleList.clear();
}

void Particle::Init(POINT Location, FPOINT Speed, FPOINT Acceleration, DWORD Color, UCHAR AlphaLevel, double AlphaShift, int TimeToLive)
{
	mLoc = Location;
	mAccel = Acceleration;
	mSpeed = Speed;
	mColor = Color;
	mAlphaLevel = AlphaLevel;
	mAlphaShift = AlphaShift;
	mTimeToLive = mMaxTimeToLive = TimeToLive;
}

bool Particle::IsDead()
{
	if(mTimeToLive <= 0)
		return true;
	return false;
}

void Particle::Update(int TimeDiff)
{
	double TimeScalar = TimeDiff / 8.0;
	mLoc.x += mSpeed.x * TimeScalar;
	mLoc.y += mSpeed.y * TimeScalar;
	mSpeed.x += mAccel.x * TimeScalar;
	mSpeed.y += mAccel.y * TimeScalar;
	mAlphaLevel += TimeScalar * mAlphaShift;
	mTimeToLive -= TimeDiff;
}

void Particle::Draw(HDC LockedDC)
{
	//ALPHA BLEND HERE?
	if(mAlphaLevel >= 255 || mAlphaLevel < 0)
		return;
	if(mLoc.x < 0 || mLoc.x >= WindowWidth || mLoc.y < 0 || mLoc.y >= WindowHeight)
		return;
	DrawParticlePixel(LockedDC, (POINT)mLoc, mColor, mAlphaLevel);
}

void GlobParticle::Draw(HDC LockedDC)
{
	//ALPHA BLEND HERE?
	if(mAlphaLevel >= 255 || mAlphaLevel < 0)
		return;
	if(mLoc.x < 0 || mLoc.x >= WindowWidth || mLoc.y < 0 || mLoc.y >= WindowHeight)
		return;

	//START AT THE GLOB'S TOP LEFT AND RIGHT AND DOWN.
	POINT Loc = (POINT)mLoc;
	int Oldx = Loc.x;
	for(int i = 0; i < 2; i++)
	{
		for(int k = 0; k < 2; k++)
		{
			if(Loc.x < 0 || Loc.x >= WindowWidth || Loc.y < 0 || Loc.y >= WindowHeight)
				continue;
			DrawParticlePixel(LockedDC, Loc, mColor, mAlphaLevel);
			Loc.x++;
		}
		Loc.x = Oldx;
		Loc.y++;
	}
}

void GlobParticleSpawner::Update(int TimeDiff)
{
	FPOINT zero;
	zero.x = 0;
	zero.y = 0;
	double TimeScalar = TimeDiff / 8.0;
	mLoc.x += mSpeed.x * TimeScalar;
	mLoc.y += mSpeed.y * TimeScalar;
	mSpeed.x += mAccel.x * TimeScalar;
	mSpeed.y += mAccel.y * TimeScalar;
	mAlphaLevel += TimeScalar * mAlphaShift;
	double nextalphalevel = mAlphaLevel + (TimeScalar * mAlphaShift);
	if(nextalphalevel > 255)
		nextalphalevel = 255;
	Particle* P = new GlobParticle;
	P->Init((POINT)mLoc, zero, zero, mColor, nextalphalevel, mAlphaShift * 2.0, mMaxTimeToLive);
	AddParticle(P);
	mTimeToLive -= TimeDiff;
}

void ParticleSpawner::Update(int TimeDiff)
{
	FPOINT zero;
	zero.x = 0;
	zero.y = 0;
	double TimeScalar = TimeDiff / 8.0;
	mLoc.x += mSpeed.x * TimeScalar;
	mLoc.y += mSpeed.y * TimeScalar;
	mSpeed.x += mAccel.x * TimeScalar;
	mSpeed.y += mAccel.y * TimeScalar;
	mAlphaLevel += TimeScalar * mAlphaShift;
	double nextalphalevel = mAlphaLevel + (TimeScalar * mAlphaShift);
	if(nextalphalevel > 255)
		nextalphalevel = 255;
	Particle* P = new Particle;
	P->Init((POINT)mLoc, zero, zero, mColor, nextalphalevel, mAlphaShift * 2.0, mMaxTimeToLive);
	AddParticle(P);
	mTimeToLive -= TimeDiff;
}

void DrawParticlePixel(HDC dc, POINT At, DWORD InputColor, double Alpha)
{
	DWORD OldPixel = GetPixel(dc, At.x, At.y);
	if(AlphaBlending)
	SetPixel(dc, At.x, At.y, BlendAlphaPixel(OldPixel, InputColor, Alpha));
	else
		SetPixel(dc, At.x, At.y, InputColor);
}

DWORD BlendAlphaPixel(DWORD OldColor, DWORD InputColor, UCHAR AlphaLevel)
{
	if(!AlphaLevel)
		return OldColor;
	UCHAR OldRed, OldGreen, OldBlue, InRed, InBlue, InGreen, OutRed, OutGreen, OutBlue;
	OldRed = RGB_GETRED(OldColor);
	OldGreen = RGB_GETGREEN(OldColor);
	OldBlue = RGB_GETBLUE(OldColor);
	InRed = RGB_GETRED(InputColor);
	InGreen = RGB_GETGREEN(InputColor);
	InBlue = RGB_GETBLUE(InputColor);

	OutRed = (InRed * AlphaLevel + (OldRed)*(255-AlphaLevel))/255;
	OutBlue = (InBlue * AlphaLevel + (OldBlue)*(255-AlphaLevel))/255;
	OutGreen = (InGreen * AlphaLevel + (OldGreen)*(255-AlphaLevel))/255;
	return RGB_MAKE(OutRed, OutGreen, OutBlue);
}

void AddParticle(Particle* P)
{
	ParticleList.push_front(P);
}

void AddBloodSparkParticle(POINT pt)
{
	Particle* P;
	if(rand()%2)
	 P = new GlobParticle;
	else
		P = new Particle;

	FPOINT Speed;
	Speed.x = (double)(rand()%RAND_MAX) / (double)RAND_MAX;
	Speed.y = (double)(rand()%RAND_MAX) / (double)RAND_MAX;
	Speed.x += rand()% 25;
	Speed.y += rand()% 6;
	if(rand()%2)
		Speed.x *= -1.0;
	if(rand()%2)
		Speed.y *= -1.0;
	FPOINT Accel;
	Accel.x = 0;
	Accel.y = 0.1;
	P->Init(pt, Speed, Accel, RGB(rand()%50 + 200, 0,0), 255, -1.5, 3200);
	AddParticle(P);
}

void AddArmorSparkParticle(POINT pt)
{
	Particle* P;
	if(rand()%2)
	 P = new GlobParticleSpawner;
	else
		P = new ParticleSpawner;

	FPOINT Speed;
	Speed.x = (double)(rand()%RAND_MAX) / (double)RAND_MAX;
	Speed.y = (double)(rand()%RAND_MAX) / (double)RAND_MAX;
	Speed.x += rand()% 4;
	Speed.y += rand()% 4;
	if(rand()%2)
		Speed.x *= -1.0;
	if(rand()%2)
		Speed.y *= -1.0;
	FPOINT Accel;
	Accel.x = 0;
	Accel.y = 0.1;
	P->Init(pt, Speed, Accel, RGB(50, rand()%50 + 200, rand()%50 + 200), 255, -3.5, 200);
	AddParticle(P);
}

DWORD AddToColor(UCHAR Constant, DWORD Color)
{
	char R,G,B;
	R = RGB_GETRED(Color);
	G = RGB_GETGREEN(Color);
	B = RGB_GETBLUE(Color);
	R += Constant;
	G += Constant;
	B += Constant;
	return RGB_MAKE(R, G, B);
}