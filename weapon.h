#define NUMWEAPONCLASSES	5
#define FIRSTWEAPONCLASSSPRITEID	802
struct Bullet
{
	char* mName;
	int mBulletDataSpriteGlobalGraphicsID;
	int mFullBulletSpriteID;
	int mTravelDelayPerFrame;
	int mTravelGlobalGraphicsID;
	int mHitGlobalGraphicsID;
	int mHitGlobalSoundID;
	int mHitWidth;
	int mHitHeight;
	int mShellCasingGlobalGraphicsID;
	int mShellCasingBounceSoundID;

public:
	Bullet() : mName(NULL) {}
	Bullet(const Bullet& ref)
	{
		*this = ref;
		if(ref.mName){
		mName = new char[strlen(ref.mName) + 1];
		strcpy(mName, ref.mName);
		}
	}
	~Bullet()
	{
		if(mName)
			delete [] mName;
	}
};

struct Weapon
{
	char* mName;
	int mClass;
	int mWeaponDataSpriteGlobalGraphicsID;
	bool mUseShotEffects;
	bool mSingleShot;
	bool mTriggerDown;
	int mTimeToNextShot;
	int mNextShotTimer;
	bool mClipLoaded;
	bool mEjectRoundWhenFired;
	bool mEjectAllRoundsWhenReloaded;
	int mMaxRounds;
	int mCurrentRounds;
	int mTimeToReload;
	int mReloadTimer;
	double mHorizRecoil;
	double mVertRecoil;
	double mWeight;
	int mGunshotGlobalSoundID;
	int mReloadGlobalSoundID;
	int mReloadRackGlobalSoundID;
	int mBulletID;
	bool mBoltBack;
	bool mActionCycled;

	Weapon() : mName(NULL) {}
	Weapon(const Weapon& ref)
	{
		*this = ref; //HA HA NEAT TRIXXOR
		if(ref.mName){
		mName = new char[strlen(ref.mName) + 1];
		strcpy(mName, ref.mName);
		}
	}
	~Weapon()
	{
		if(mName)
			delete [] mName;
	}
};

class WeaponsSelector
{
	//STATUS TYPES:
	//0. STATIONARY OPEN
	//1. EXTENDING
	//2. RETRACTING
	//3. STATIONARY CLOSED
	
	Sprite WeaponClassSprites[NUMWEAPONCLASSES];
	Sprite* BaseSelector;
	Sprite* WeaponSprite;
	int mXOrigin;
	int mYOrigin;
	int mStatus;
	int mActiveTime;
	int mActiveTimer;
public:
	TypewriterText* BulletName;
	WeaponsSelector() : BulletName(NULL), mXOrigin(0), mYOrigin(0), BaseSelector(NULL), WeaponSprite(NULL), mActiveTime(3500), mActiveTimer(3500) {}
	~WeaponsSelector() { if(BaseSelector)delete BaseSelector; if(WeaponSprite)delete WeaponSprite; if(BulletName) delete BulletName;}
	bool Init(Weapon* TheWeapon);
	void Update(Weapon* TheWeapon, int timediff, CDisplay& DisplayRef);
	void Draw(Weapon* TheWeapon, CDisplay& DisplayRef);
	int GetStatus() {return mStatus;}
	void SetStatus(int status) {mStatus = status;}
	void UpdateWithNewData(Weapon* NewWeapon);
};


bool InitializeGameBullets();
bool InitializeGameWeapons();
void DestroyGameWeaponsAndBullets();
bool ChangeWeapon(int To, Weapon** ToFill);
bool CanWeaponFire(Weapon* ToFire);
void WeaponUpdate(Weapon* ToUpdate, int timediff);
FPOINT Recoil(POINT oldpoint, Weapon* weap);
Weapon* GetGlobalWeaponData(int index);
Bullet* GetGlobalBulletData(int index);
void DrawBulletsRemaining(Weapon* DrawMe, int timediff);
bool InitializeWeaponSelector(Weapon* TheWeapon);
void UpdateWeaponsSelector(Weapon* TheWeapon, int timediff, CDisplay& DisplayRef);
void ActivateWeaponSelector(Weapon* TheWeapon);
void DeactivateWeaponSelector();
void AllowAllWeapons();
bool IsWeaponAllowed(Weapon* TheWeapon);
void DisallowWeaponClass(int TypeToDisallow, int* CurrentWeaponID, Weapon** CurrentWeaponToFill);
void AllowWeaponClass(int TypeToAllow);
int ChangeToNextWeapon(int Current, Weapon** ToFill, bool Prev = false);
void ShowWeaponSelector();
double GetWeaponClassAccelRate(int Class);
double GetWeaponClassDragRate(int Class);
