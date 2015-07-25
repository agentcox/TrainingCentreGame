class Challenge
{
private:
	char* mBriefing;
	char* mRecap;
	int mInternalTimer;
	bool mTimerRunning;
protected:
	void SetInternalTimer(int Time) { if(Time > 0){mInternalTimer = Time; mTimerRunning = true;}}
	void SetBriefing(char* Brief){ if(mBriefing) delete [] mBriefing; mBriefing = new char[strlen(Brief) + 1]; strcpy(mBriefing, Brief);}
	void SetRecap(char* Recap){ if(mRecap) delete [] mRecap; mRecap = new char[strlen(Recap) + 1]; strcpy(mRecap, Recap);}
public:
	Challenge() : mTimerRunning(false) , mBriefing(NULL), mRecap(NULL) {SetBriefing("SUK IT"); SetRecap("DOWN!");}
	char* GetBriefing(){onBriefingRequest(); return mBriefing;}
	char* GetRecap(){onRecapRequest(); return mRecap;}
	virtual ~Challenge() {if(mBriefing)delete [] mBriefing; if(mRecap)delete [] mRecap;};
	void ChallengeUpdate(int TimeDiff);
	virtual void onRoundStart() = 0;
	virtual void onTargetDeath(Target* TheTarget, bool EndOfRound) = 0;
	virtual void onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score) = 0;
	virtual void onRoundEnd() = 0;
	virtual void onInternalTimerExpire() = 0;
	virtual void onRecapRequest() = 0;
	virtual void onBriefingRequest() = 0;
};

class Challenge_DuckHunt : public Challenge
{
private:
	int DucksLaunched;
	int DucksHit;
	int Percent;
	void RecalcPercentage();
public:
	void onRoundStart();
	void onTargetDeath(Target* TheTarget, bool EndOfRound);
	void onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score);
	void onRoundEnd();
	void onInternalTimerExpire();
	void onBriefingRequest();
	void onRecapRequest();
};

class Challenge_Bullseye : public Challenge
{
private:
	int PointsPossible;
	int PointsGained;
public:
	void onRoundStart();
	void onTargetDeath(Target* TheTarget, bool EndOfRound);
	void onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score);
	void onRoundEnd();
	void onInternalTimerExpire();
	void onBriefingRequest();
	void onRecapRequest();
};

class Challenge_Popups : public Challenge
{
private:
	int PoppedUp;
	int ShotDown;
	int Percent;
	void RecalcPercentage();
public:
	void onRoundStart();
	void onTargetDeath(Target* TheTarget, bool EndOfRound);
	void onBulletHit(Target* TheTarget, Bullet* TheBullet, int Score);
	void onRoundEnd();
	void onInternalTimerExpire();
	void onBriefingRequest();
	void onRecapRequest();
};

void MakeNewChallenge(Challenge** ChallengeToFill, int* CurrentChallengeNumber);