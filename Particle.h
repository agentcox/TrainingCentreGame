class Particle
{
protected:
	FPOINT mLoc;
	FPOINT mSpeed;
	FPOINT mAccel;
	DWORD mColor;
	double mAlphaLevel;
	double mAlphaShift;
	int mTimeToLive;
	int mMaxTimeToLive;
public:
	virtual void Init(POINT Location, FPOINT Speed, FPOINT Acceleration, DWORD Color, UCHAR AlphaLevel = 0, double AlphaShift = 0.5, int TimeToLive = 5000);
	virtual void Update(int TimeDiff);
	virtual void Draw(HDC LockedDC);
	virtual bool IsDead();
};

class ParticleSpawner : public Particle
{
public:
	void Update(int TimeDiff);
};

class GlobParticle : public Particle //HAS A RADIUS OF THREE
{
public:
	void Draw(HDC LockedDC);
};

class GlobParticleSpawner : public GlobParticle
{
public:
	void Update(int TimeDiff);
};


void InitParticleSystem();
void ClearParticleMap();
void FreeParticleSystem();
void BeginDrawingParticles();
void EndDrawingParticles();
DWORD BlendAlphaPixel(DWORD OldColor, DWORD InputColor, UCHAR AlphaLevel);
void UpdateAndDrawAllParticles(int TimeDiff);
void AddParticle(Particle* P);
void AddBloodSparkParticle(POINT pt);
void AddArmorSparkParticle(POINT pt);
void DrawParticlePixel(HDC dc, POINT At, DWORD InputColor, double Alpha);