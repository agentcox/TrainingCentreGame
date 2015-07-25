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
#include "gameevents.h"
#include "challenge.h"


extern CDisplay TheDisplay;
extern SoundQueue GameSounds;
extern ScoreUnit ScoreBox;
extern GameEvents Events;

int NumChallenges = 3;

void Challenge::ChallengeUpdate(int TimeDiff)
{
	if(mTimerRunning)
	{
		mInternalTimer -= TimeDiff;
		if(mInternalTimer <= 0)
		{
			mTimerRunning = false;
			onInternalTimerExpire();
		}
	}
}


///////////DUCK HUNT CHALLENGE/////////////

void Challenge_DuckHunt::onRoundStart()
{
	DucksLaunched = DucksHit = 0;
	ScoreBox.ResetScore();
	ScoreBox.SetTitle("ACCURACY");
	ScoreBox.SetCaption("PCT");
	ScoreBox.SetDigitsToDisplay(3);
	AddClayPigeon(TheDisplay, 5.5, -2.5);
	
}

void Challenge_DuckHunt::RecalcPercentage()
{
	int PCt;
	if(DucksHit == 0)
		PCt = 0;
	else
		PCt = 100;
	
	if(DucksLaunched != 0){
		PCt = (DucksHit * 100) / DucksLaunched;
	}
	Percent = PCt;
	ScoreBox.SetScore(PCt);
}

void Challenge_DuckHunt::onTargetDeath(Target* TheTarget, bool EndOfRound)
{
	DucksLaunched++;
	RecalcPercentage();
	int TheTime = rand()%500 + 500;
	if(!(DucksLaunched % 5)){// EVERY FIFTH DUCK, HOLD FOR A RELOAD.
		TheTime += 2000;
		Events.ChangeNotificationString("RELOAD IMMEDIATELY", 1500);
	}
	
	if(!EndOfRound){
		SetInternalTimer(TheTime);
	}
}

void Challenge_DuckHunt::onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score)
{
	if(Score == 1)
		DucksHit++;
	RecalcPercentage();
}

void Challenge_DuckHunt::onRoundEnd()
{
	//DO NOTHING, SON.
}

void Challenge_DuckHunt::onInternalTimerExpire()
{
	AddClayPigeon(TheDisplay, 5.5, -2.5);
}

void Challenge_DuckHunt::onRecapRequest()
{
	char buf[500];
	char FinalAnalysis[500];
	if(Percent > 90)
		strcpy(FinalAnalysis, "Exemplary Shooting");
	else if(Percent > 70)
		strcpy(FinalAnalysis, "Excellent Work");
	else if(Percent > 50)
		strcpy(FinalAnalysis, "This Is Above Average");
	else if(Percent > 40)
		strcpy(FinalAnalysis, "This Constitutes Average Performance");
	else if(Percent > 30)
		strcpy(FinalAnalysis, "It Is Advised To Practice Further");
	else
		strcpy(FinalAnalysis, "More Practice Is Necessary");
	
	sprintf(buf, "FINAL SCORE\nOut of %d targets launched you managed to strike %d giving you a percentage total of %d\n\n%s\n\nCharles Cox\nDirector of The Training Centre\n\nPress SPACE to continue", DucksLaunched, DucksHit, Percent, FinalAnalysis);
	SetRecap(buf);
}

void Challenge_DuckHunt::onBriefingRequest()
{
	char buf[1500];
	sprintf(buf, "TRAINING MODULE 0002\nDUCK HUNT\n\nTargets will be lofted from the sides of the range across the target area\nYou must neutralize these targets as they pass\nAfter every fifth launch you will be given a short interval in which to reload\n\nYou may use as many bullets as you wish in neutralizing the targets but be aware of your reload times\n\nShould any of the targets fall to the ground or pass the end of the range unscathed there will be a penalty\n\nThis is a category III test\nShotguns only\n\nGood luck\n\nPress space to continue");
	SetBriefing(buf);
}


///////BULLSEYE CHALLENGE//////////

void Challenge_Bullseye::onInternalTimerExpire()
{
	//DO NOTHING.
}

void Challenge_Bullseye::onBriefingRequest()
{
	char buf[1500];
	sprintf(buf, "TRAINING MODULE 0001\nBULLSEYE\n\nTimed targets will appear\nThey will disappear after their timers expire\nYou must attempt to hit the bullseye on each target\nmultiple hits do not count as only the highest score on the target when the target is removed counts\nthere will always be 4 targets visible at any time\n\nGood luck\n\nPress space to continue");
	SetBriefing(buf);
}

void Challenge_Bullseye::onRoundStart()
{
	PointsGained = 0;
	AddMediumTimedPaperTarget(TheDisplay);
	AddMediumTimedPaperTarget(TheDisplay);
	AddMediumTimedPaperTarget(TheDisplay);
	AddMediumTimedPaperTarget(TheDisplay);
	ScoreBox.SetTitle("POINTS EARNED");
	ScoreBox.SetCaption("PTS");
	ScoreBox.SetDigitsToDisplay(4);
	PointsPossible = 40;
}

void Challenge_Bullseye::onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score)
{
	//DO NOTHING, SON
}

void Challenge_Bullseye::onTargetDeath(Target* TheTarget, bool EndOfRound)
{
	int Gained = TheTarget->QueryForFinalScoreUponDeath();
	PointsGained += Gained; 
	ScoreBox.AddToScore(Gained);
	if(!EndOfRound){
		AddMediumTimedPaperTarget(TheDisplay);
		PointsPossible += 10;
	}
}

void Challenge_Bullseye::onRoundEnd()
{
	//DO NOTHING, SON
}

void Challenge_Bullseye::onRecapRequest()
{
	char buf[1500];
	
	int Percent;
	if(PointsPossible){
		Percent = PointsGained * 100 / PointsPossible;
	}
	else{
		Percent = 0;
	}
	char FinalAnalysis[500];
	if(Percent > 90)
		strcpy(FinalAnalysis, "Exemplary Shooting");
	else if(Percent > 70)
		strcpy(FinalAnalysis, "Excellent Work");
	else if(Percent > 50)
		strcpy(FinalAnalysis, "This Is Above Average");
	else if(Percent > 40)
		strcpy(FinalAnalysis, "This Constitutes Average Performance");
	else if(Percent > 30)
		strcpy(FinalAnalysis, "It Is Advised To Practice Further");
	else
		strcpy(FinalAnalysis, "More Practice Is Necessary");
	
	sprintf(buf, "FINAL SCORE\nYou scored %d points out of a possible %d\nthis is calculated as a %d efficiency percentage\n\n%s\n\nCharles Cox\nDirector Of The Training Centre\n\nPress SPACE to continue",PointsGained, PointsPossible, Percent, FinalAnalysis);
	SetRecap(buf);
}


void Challenge_Popups::onInternalTimerExpire()
{
	//DO NOTHING.
	if(rand()%4)
	{
		if(rand()%2)
			AddBodyArmorPopupTarget(TheDisplay);
		else
			AddHeadArmorPopupTarget(TheDisplay);
	}
	else
		AddStandardPopupTarget(TheDisplay);
}

/////////////POPUPS CHALLENGE BEGIN//////////////

void Challenge_Popups::RecalcPercentage()
{
	int PCt;
	if(ShotDown == 0)
		PCt = 0;
	else
		PCt = 100;
	
	if(PoppedUp != 0){
		PCt = (ShotDown * 100) / PoppedUp;
	}
	Percent = PCt;
	ScoreBox.SetScore(PCt);
}

void Challenge_Popups::onBriefingRequest()
{
	char buf[1500];
	sprintf(buf, "TRAINING MODULE 0003\nPOPUPS\n\nTimed targets will appear\nThey will disappear after their timers expire\nThey may or may not be shielded by body or head armor which is shown in blue\nkill zones are shown in red\n\nyou must hit the kill zone to down the target\n\nThe target timers are extremely short and accuracy beyond hitting a kill zone does not count towards a final score\n\nGood luck\n\nPress space to continue");
	SetBriefing(buf);
}

void Challenge_Popups::onRoundStart()
{
	PoppedUp = ShotDown = 0;
	if(rand()%4)
	{
		if(rand()%2)
			AddBodyArmorPopupTarget(TheDisplay);
		else
			AddHeadArmorPopupTarget(TheDisplay);
	}
	else
		AddStandardPopupTarget(TheDisplay);
	ScoreBox.SetTitle("ACCURACY");
	ScoreBox.SetCaption("PCT");
	ScoreBox.SetDigitsToDisplay(3);
}

void Challenge_Popups::onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score)
{
	//DO NOTHING, SON
}

void Challenge_Popups::onTargetDeath(Target* TheTarget, bool EndOfRound)
{
	int Gained = TheTarget->QueryForFinalScoreUponDeath();
	if(Gained){
		ShotDown++;
	}
	if(!EndOfRound){
		SetInternalTimer(rand()% 2500 + 1);
	}
	PoppedUp++;
	RecalcPercentage();
}

void Challenge_Popups::onRoundEnd()
{
	//DO NOTHING, SON
}

void Challenge_Popups::onRecapRequest()
{
	char buf[1500];
	
	char FinalAnalysis[500];
	if(Percent > 90)
		strcpy(FinalAnalysis, "Exemplary Shooting");
	else if(Percent > 70)
		strcpy(FinalAnalysis, "Excellent Work");
	else if(Percent > 50)
		strcpy(FinalAnalysis, "This Is Above Average");
	else if(Percent > 40)
		strcpy(FinalAnalysis, "This Constitutes Average Performance");
	else if(Percent > 30)
		strcpy(FinalAnalysis, "It Is Advised To Practice Further");
	else
		strcpy(FinalAnalysis, "More Practice Is Necessary");
	
	sprintf(buf, "FINAL SCORE\n%d targets were popped up\nyou struck %d\nthis is calculated as a %d efficiency percentage\n\n%s\n\nCharles Cox\nDirector Of The Training Centre\n\nPress SPACE to continue",PoppedUp, ShotDown, Percent, FinalAnalysis);
	SetRecap(buf);
}

void MakeNewChallenge(Challenge** ChallengeToFill, int* CurrentChallengeNumber)
{
	switch(*CurrentChallengeNumber)
	{
	case 0:
		*ChallengeToFill = new Challenge_DuckHunt;
		break;
	case 1:
		*ChallengeToFill = new Challenge_Bullseye;
		break;
	case 2:
		*ChallengeToFill = new Challenge_Popups;
		break;
	}
	(*CurrentChallengeNumber)++;
	(*CurrentChallengeNumber) %= NumChallenges;
}
