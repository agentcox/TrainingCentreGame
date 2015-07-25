#define TransparentColor 0xFFFF00FF
#define CrappyTransparentColor 0x00FF00FF

class Target
{
protected:
	BITFIELD32 TotalHits; //FOR QUERYING AFTER DEATH
	Sprite* TargetSprite;
	Sprite* HitDetectSprite;
	int mSoundID; //ALSO MAY NEED SPECIAL IMPACT SOUNDS FOR DERIVED TARGETS.
	FPOINT mOrigin;
	bool HasImpacted(Bullet* ToImpact, POINT ImpactOrigin);
	BITFIELD32 InternalBulletHit(Bullet* ToImpact, POINT ImpactOrigin);
public:
	Target() : TotalHits(0) { TargetSprite = new Sprite; HitDetectSprite = new Sprite;}
	virtual ~Target() { delete TargetSprite; delete HitDetectSprite;}
	virtual void Init(int SpriteNumber, int HitDetectSpriteNumber, POINT Location, int SoundID, int SpriteState = 0, int SpriteDelay = -1);
	virtual int BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin); //SCORE IS RETURNED HERE.
	virtual int QueryForFinalScoreUponDeath(); //SCORE IS RETURNED HERE.
	virtual void Update(int Timediff);
	virtual void Draw(CDisplay& DisplayRef);
	virtual bool CheckIsDead();
};

class TimedTarget : public Target
{
protected:
	int mTimeToLive;
public:
	TimedTarget() : Target(), mTimeToLive(5000) {}
	void SetTime(int TimeToLive);
	virtual void Update(int Timediff);
	void Draw(CDisplay& DisplayRef);
	virtual bool CheckIsDead();
};

class ClayPigeon : public Target
{
private:
	bool mStruck;
	int mXToDieAt;
	int mYToDieAt;
	double xSpeed;
	double ySpeed;
	double yGravity;
public:
	ClayPigeon() : Target(), mStruck(false), yGravity(0.05) { TargetSprite->SetDelay(20);}
	void Launch(POINT From, POINT ToDieAt, double XSpeed, double YSpeed);
	bool CheckIsDead();
	void Update(int Timediff);
	int BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin);
	bool IsStruck();
};

class PopupTarget : public TimedTarget
{
private:
	bool mStruck;
public:
	void Update(int Timediff);
	bool CheckIsDead();
	virtual int BulletImpact(Bullet* BulletToImpact, POINT ImpactOrigin);
};

inline bool IsPointInRect(POINT& PRef, RECT& RRef)
{
	if(PRef.x >= RRef.left && PRef.x < RRef.right && PRef.y >= RRef.top && PRef.y < RRef.bottom)
		return true;
	return false;
}
void JinkBulletImpact(POINT* Origin);
void ClearTargets();
void AddTarget(Target* NewTarget);
void UpdateAllTargets(int Timediff, CDisplay& DisplayRef);
int CheckForBulletImpacts(Bullet* ToImpact, POINT ImpactOrigin);
void RectClipping(RECT& BigRect, RECT& SmallRect, RECT& ClippingOffsets);
BITFIELD32 OverlayBulletHitSprite(LPDDSURFACEDESC2 TSSData, LPDDSURFACEDESC2 HSSData, LPDDSURFACEDESC2 HDSData, RECT& TargetZeroBased, RECT& HitZeroBased, RECT& ClipOffsets);

inline void ChangeHitOverlayColors(DWORD* Target, DWORD* Hit)
{
	if((*Hit) == TransparentColor || (*Hit) == CrappyTransparentColor || (*Target) == CrappyTransparentColor || (*Target) == TransparentColor)
		return;
	if((*Hit) == 0)
		*Target = TransparentColor;
	else
		*Target = *Hit;
}

inline int GetHitDetectionPixelValue(DWORD* PixelPtr)
{
	//WE GET ONE OF THE 8 COLOR TYPES
	int ColorValue = -1;
	if(*PixelPtr == 0xFF000000 || *PixelPtr == 0x000000)
		ColorValue = 0;
	else if(*PixelPtr == 0xFFFF0000 || *PixelPtr == 0xFF0000)
		ColorValue = 1;
	else if(*PixelPtr == 0xFF00FF00 || *PixelPtr == 0x00FF00)
		ColorValue = 2;
	else if(*PixelPtr == 0xFFFFFFFF || *PixelPtr == 0xFFFFFF)
		ColorValue = 3;
	else if(*PixelPtr == 0xFFFF00FF || *PixelPtr == 0xFF00FF)
		ColorValue = 4;
	else if(*PixelPtr == 0xFFFFFF00 || *PixelPtr == 0xFFFF00)
		ColorValue = 5;
	else if(*PixelPtr == 0xFF00FFFF || *PixelPtr == 0x00FFFF)
		ColorValue = 6;
	else if(*PixelPtr == 0xFF0000FF || *PixelPtr == 0x0000FF)
		ColorValue = 7;
	//THEN SHIFT A VALUE OF 1 UP THAT FAR AND RETURN.
	if(ColorValue != -1)
	return (1 << ColorValue);
	return 0;
}


//TARGET ADDING
void AddLargeCenteredPaperTarget(CDisplay& DisplayRef);
void AddMediumTimedPaperTarget(CDisplay& DisplayRef);
void AddClayPigeon(CDisplay& DisplayRef, double xSpeed, double ySpeed);
void AddStandardPopupTarget(CDisplay& DisplayRef);
void AddBodyArmorPopupTarget(CDisplay& DisplayRef);
void AddHeadArmorPopupTarget(CDisplay& DisplayRef);
