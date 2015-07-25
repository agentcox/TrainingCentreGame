struct PhysObject
{
	double mGravity;
	double mXAccel;
	double mYAccel;
	double mXSpeed;
	double mYSpeed;

	PhysObject() : mGravity(0.0), mXAccel(0.0), mYAccel(0.0), mXSpeed(0.0), mYSpeed(0.0) {}
	void PhysicsUpdate(int timediff);
//	virtual void UpdatePhysics(int timediff);
//	virtual void Update(int timediff);
};



class GameObject : public PhysObject
{
	Sprite* Spr;
	int Index;
	static int NextIndex;
	int TimeToLive;
public:
	GameObject() : Index(NextIndex), TimeToLive(5000) {NextIndex++; Spr = new Sprite;}
	GameObject(const GameObject& ref)
	{
		*this = ref;
		Spr = new Sprite;
		Spr->InitializeSpriteCopy(ref.Spr);
	}
	~GameObject() {delete Spr;}
	bool operator == (const GameObject& ref) {return (ref.Index == Index);}
	void SetTimeToLive(int ttl) {TimeToLive = ttl;}
	void PositionUpdate(int timediff);
	Sprite* GetSprite(){return Spr;}
	virtual void Update(int timediff);
	virtual bool IsDead();
};

class BounceObject : public GameObject
{
	ASound* BounceSound;
	bool PhysicsAffected;
	int LowerBound;
	int LeftBound;
	int RightBound;
	int TopBound;
	int NumBounces;
public:
	BounceObject(const ASound& ref) : PhysicsAffected(false), LowerBound(0), LeftBound(0), RightBound(0), TopBound(0), NumBounces(0)
	{
		BounceSound = new ASound(ref);
	}
	BounceObject(const BounceObject& ref) : GameObject(ref)
	{
		PhysicsAffected = ref.PhysicsAffected;
		LowerBound = ref.LowerBound;
		LeftBound = ref.LeftBound;
		RightBound = ref.RightBound;
		TopBound = ref.TopBound;
		NumBounces = ref.NumBounces;
		BounceSound = new ASound(*ref.BounceSound);
	}
	bool BounceUpdate(int timediff);
	virtual void Update(int timediff);
	void SetBounds(RECT& BoundRect);
	~BounceObject()
	{
		if(BounceSound)
			delete BounceSound;
	}
	void SetPhysics(bool switchedon) {PhysicsAffected = switchedon;}
};



void UpdateBounceObjects(int timediff);
void SpawnShellCasing(POINT spawn, Weapon* Weap);
void ChangeShellCasingUpdateSpeed(int Speed);
int GetShellCasingUpdateSpeed();
