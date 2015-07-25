#include <stdio.h>
#include <list>
using namespace std;
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "globaldata.h"
#include "font.h"
#include "sounds.h"
#include "timerunit.h"

extern CDisplay TheDisplay;
extern SoundQueue GameSounds;
#define TUPDATE_VAL	15
int TUpdate = 15;

void TimerUnit::Initialize(int LargeNumberSpriteID, int SmallNumberSpriteID, int CasingSpriteID, int CaptionFontID)
{
	LargeNumberSprite.InitializeSpriteCopy(GetGlobalSpriteData(LargeNumberSpriteID));
	SmallNumberSprite.InitializeSpriteCopy(GetGlobalSpriteData(SmallNumberSpriteID));
	CasingSprite.InitializeSpriteCopy(GetGlobalSpriteData(CasingSpriteID));
	CaptionFont = GetFontBySizeIndex(CaptionFontID);

	

	//NOW WE MUST FIGURE OUT WHERE TO PUT EVERYTHING.
	//FIRST, THE CASING.
	RECT Client;
	GetClientRect(TheDisplay.GetHWnd(), &Client);
	AbsCasingOrigin.x = ((Client.right - Client.left) / 2) - (CasingSprite.GetWidth() / 2) + 100;
	AbsCasingOrigin.y = CasingSprite.GetHeight() * -1;
	CasingSprite.SetLoc(AbsCasingOrigin); 
	RelCaptionOrigin.y = CasingSprite.GetHeight();
	SetCaption("TIMER");
	//TIME HEIGHT AND WIDTH
	RelTimeOrigin.y = 13;
	//5 BIG DIGITS AND 3 SMALL ONES
	int TimeWidth = 5 * LargeNumberSprite.GetWidth() + 3 * SmallNumberSprite.GetWidth();
	RelTimeOrigin.x = (CasingSprite.GetWidth() / 2);
	RelTimeOrigin.x -= (TimeWidth / 2);
	//ALL WIDTHS AND HEIGHTS ACCOUNTED FOR.
	Status = 3;
	
}

void TimerUnit::SetCaption(char* NewCaption)
{
	if(Caption)
		delete [] Caption;
	Caption = new char[strlen(NewCaption) + 1];
	strcpy(Caption, NewCaption);
	RelCaptionOrigin.x = (CasingSprite.GetWidth() / 2) - (CaptionFont->GetStringWidth(Caption) / 2);

}

bool TimerUnit::IsTimeUp()
{
	return TimeUp;
}

void TimerUnit::Freeze()
{
	Running = false;
}

void TimerUnit::Unfreeze()
{
	Running = true;
}

void TimerUnit::Reset()
{
	CurrentMinutes = StoredMinutes;
	CurrentSeconds = StoredSeconds;
	CurrentSplit = StoredSplit;
	TimeUp = false;
}

void TimerUnit::SetTime(int Minutes, int Seconds, int Split)
{
	StoredMinutes = Minutes;
	StoredSeconds = Seconds;
	StoredSplit = Split;
	Reset();
	WrapAround();
}

void TimerUnit::Open()
{
	if(Status != 0)
	Status = 1;
}

void TimerUnit::Close()
{
	if(Status != 2)
		Status = 2;
}

bool TimerUnit::IsReadyToBeginCounting()
{
	return (Status == 0);
}

void TimerUnit::Update(int TimeDiff, CDisplay& DisplayRef)
{
	switch(Status)
	{
	case 1:
		AbsCasingOrigin.y += TimeDiff * 2;
		if(AbsCasingOrigin.y >= 0)
		{
			AbsCasingOrigin.y = 0;
			Status = 0;
		}
		break;
	case 2:
		AbsCasingOrigin.y -= TimeDiff * 2;
		if(AbsCasingOrigin.y <= CasingSprite.GetHeight() * -1)
		{
			AbsCasingOrigin.y = CasingSprite.GetHeight() * -1;
			Status = 3;
		}
		break;
	}

	CasingSprite.SetLoc(AbsCasingOrigin);

	if(Running)
	{
		CurrentSplit -= (double) (TimeDiff) / 10.0;
		if(CurrentSplit <= 0){
			if(CurrentSeconds <= 10)
				GameSounds.InsertSound(*GetGlobalSoundData(507));
			else
				GameSounds.InsertSound(*GetGlobalSoundData(506));
		}
		WrapAround();
		UpdateDigits();
	}
	Draw(DisplayRef);
}

void TimerUnit::UpdateDigits()
{
	char TimeString[20];
	char Holder[2];
	sprintf(TimeString, "%2.2d%2.2d%2.2d", CurrentMinutes, CurrentSeconds, (int)CurrentSplit);
	Holder[1] = '\0';
	for(int i = 0; i < 6; i++)
	{
		Holder[0] = TimeString[i];
		StoredDigitArray[i] = atoi(Holder);
	}
}

void TimerUnit::Draw(CDisplay& DisplayRef)
{
	//30 SECONDS, YELLOW
	//10 SECONDS, RED
	if(CurrentMinutes <= 0)
	{
		if(CurrentSeconds < 10)
		{
			LargeNumberSprite.SetState(2);
			SmallNumberSprite.SetState(2);
		}
		else if(CurrentSeconds < 30)
		{
			LargeNumberSprite.SetState(1);
			SmallNumberSprite.SetState(1);
		}
		else
		{
			SmallNumberSprite.SetState(0);
			LargeNumberSprite.SetState(0);
		}
	}
	else
	{
		SmallNumberSprite.SetState(0);
		LargeNumberSprite.SetState(0);
	}
	

	//CASING FIRST
	CasingSprite.Draw(DisplayRef);
	POINT FPoint;
	//CAPTION
	if(Status == 0){
	FPoint.x = AbsCasingOrigin.x + RelCaptionOrigin.x;
	FPoint.y = AbsCasingOrigin.y + RelCaptionOrigin.y;
	CaptionFont->DrawString(Caption, FPoint);
	}
	//BEGIN WITH MINUTES.
	FPoint.x = RelTimeOrigin.x + AbsCasingOrigin.x;
	FPoint.y = RelTimeOrigin.y + AbsCasingOrigin.y;
	LargeNumberSprite.SetFrame(StoredDigitArray[0]);
	LargeNumberSprite.SetLoc(FPoint);
	LargeNumberSprite.Draw(DisplayRef);

	FPoint.x += LargeNumberSprite.GetWidth();
	LargeNumberSprite.SetFrame(StoredDigitArray[1]);
	LargeNumberSprite.SetLoc(FPoint);
	LargeNumberSprite.Draw(DisplayRef);

	FPoint.x += LargeNumberSprite.GetWidth();
	LargeNumberSprite.SetFrame(10); //THE COLON
	LargeNumberSprite.SetLoc(FPoint);
	LargeNumberSprite.Draw(DisplayRef);

	//SECONDS
	FPoint.x += LargeNumberSprite.GetWidth();
	LargeNumberSprite.SetFrame(StoredDigitArray[2]);
	LargeNumberSprite.SetLoc(FPoint);
	LargeNumberSprite.Draw(DisplayRef);

	FPoint.x += LargeNumberSprite.GetWidth();
	LargeNumberSprite.SetFrame(StoredDigitArray[3]);
	LargeNumberSprite.SetLoc(FPoint);
	LargeNumberSprite.Draw(DisplayRef);

	//SPLIT
	FPoint.x += LargeNumberSprite.GetWidth();
	FPoint.y += LargeNumberSprite.GetHeight() - SmallNumberSprite.GetHeight();
	SmallNumberSprite.SetFrame(10);
	SmallNumberSprite.SetLoc(FPoint);
	SmallNumberSprite.Draw(DisplayRef);

	FPoint.x += SmallNumberSprite.GetWidth();
	SmallNumberSprite.SetFrame(StoredDigitArray[4]);
	SmallNumberSprite.SetLoc(FPoint);
	SmallNumberSprite.Draw(DisplayRef);

	FPoint.x += SmallNumberSprite.GetWidth();
	SmallNumberSprite.SetFrame(StoredDigitArray[5]);
	SmallNumberSprite.SetLoc(FPoint);
	SmallNumberSprite.Draw(DisplayRef);

}

void TimerUnit::WrapAround()
{
	while(CurrentSplit < 0){
		CurrentSplit += 99;
		CurrentSeconds--;
	}
	while(CurrentSplit >= 100){
		CurrentSplit -= 100;
		CurrentSeconds++;
	}
	while(CurrentSeconds < 0){
		CurrentSeconds += 60;
		CurrentMinutes--;
	}
	while(CurrentSeconds >= 60){
		CurrentSeconds -= 60;
		CurrentMinutes++;
	}

	if(CurrentMinutes < 0){
		CurrentSeconds = 0;
		CurrentSplit =0;
		CurrentMinutes = 0;
		Freeze();
		TimeUp = true;
	}
}