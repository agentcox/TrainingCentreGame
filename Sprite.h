struct FPOINT
{
	double x;
	double y;
public:
	operator POINT()
	{
		POINT ret;
		ret.x = (long)x;
		ret.y = (long)y;
		return ret;
	}
	operator = (const POINT& ref)
	{
		x = ref.x;
		y = ref.y;
	}
};


class Sprite
{
protected:
	friend class Font;
	CSurface* mFullImage;
	LPDIRECTDRAWSURFACE7 mSurf;
	bool mImageAlloc;
	bool mHasPlayedStateOnce;
	int mCurrentFrame;
	int mCurrentState;
	int mTimeToNextFrame;
	int mTimer;
	int mFrameWidth;
	int mFrameHeight;
	int mFullHeight;
	int mFullWidth;
	int mNumStates;
	int* mStateFrames;
	int mGlobalIndex;
public:
	double mXOrigin;
	double mYOrigin;

	Sprite() : mFullImage(NULL), mSurf(NULL),  mImageAlloc(false), mHasPlayedStateOnce(false),  mCurrentFrame(0), mCurrentState(0), mTimeToNextFrame(-1), mNumStates(0), mStateFrames(NULL), mFrameHeight(0), mFrameWidth(0), mXOrigin(0), mYOrigin(0), mFullHeight(0), mFullWidth(0){};
	Sprite(const Sprite& ref);
	operator = (const Sprite& ref);
	~Sprite();
	bool Initialize(CDisplay& displayref, char* BMPFile, int FullWidth, int FullHeight, int FrameWidth, int FrameHeight, int NumStates, int* StateFrames);
	bool InitializeSpriteCopy(const Sprite* ref);
	void SetDelay(int newdelay = -1);
	void SetFrame(int frame = 0);
	void SetState(int state = 0);
	bool HasPlayedOnce(){return mHasPlayedStateOnce;}
	int GetFrame();
	int GetState();
	int GetDelay();
	int GetWidth();
	int GetHeight();
	int GetFullHeight();
	int GetFullWidth();
	int GetNumFramesInCurrentState();
	void SetGlobalIndex(int index) {mGlobalIndex = index;}
	int GetGlobalIndex() {return mGlobalIndex;}
	void SetLoc(POINT loc);
	POINT GetLoc();
	virtual bool UpdateFrames(int timediff);
	virtual bool Draw(CDisplay& DisplayRef);
	LPDIRECTDRAWSURFACE7 GetSurface() {return mSurf;}
	
	inline RECT GetCurrentRectToDraw(CDisplay& displayref)
	{
		RECT Out;
		RECT Full;
		Out.left = 0;
		Out.right = mFrameWidth;
		Out.top = 0;
		Out.bottom = mFrameHeight;
		Full = displayref.GetWindowBounds();
		Full.right -= Full.left;
		Full.bottom -= Full.top;
		Full.left = 0;
		Full.top = 0;
		if(Out.left + (int)mXOrigin < Full.left)
			Out.left -= ((int)mXOrigin - 1);
		if(Out.right + (int)mXOrigin > Full.right)
			Out.right = (Full.right - (int)mXOrigin);
		if(Out.top + (int)mYOrigin < Full.top)
			Out.top -= ((int)mYOrigin -1);
		if(Out.bottom + (int)mYOrigin > Full.bottom)
			Out.bottom = (Full.bottom - (int)mYOrigin);

		Out.top += mFrameHeight * mCurrentFrame;
		Out.bottom += mFrameHeight * mCurrentFrame;
		Out.left += mFrameWidth * mCurrentState;
		Out.right += mFrameWidth * mCurrentState;
		return Out;
	}

	inline RECT ClipRectAgainstScreen(CDisplay& displayref, RECT SpriteRect)
	{
		RECT Out;
		RECT Full;
		Out.left = 0;
		Out.right = SpriteRect.right - SpriteRect.left;
		Out.top = 0;
		Out.bottom = SpriteRect.bottom - SpriteRect.top;
		Full = displayref.GetWindowBounds();
		Full.right -= Full.left;
		Full.bottom -= Full.top;
		Full.left = 0;
		Full.top = 0;
		if(Out.left + (int)mXOrigin < Full.left)
			Out.left -= (Full.right - ((int)mXOrigin + Out.left));
		if(Out.right + (int)mXOrigin > Full.right)
			Out.right += (Full.right - ((int)mXOrigin + Out.right));
		if(Out.top + (int)mYOrigin < Full.top)
			Out.top -= (Full.top - ((int)mYOrigin + Out.top));
		if(Out.bottom + (int)mYOrigin > Full.bottom)
			Out.bottom += (Full.bottom - ((int)mYOrigin + Out.bottom));

		Out.top += SpriteRect.top;
		Out.bottom += SpriteRect.top;
		Out.left += SpriteRect.left;
		Out.right += SpriteRect.left;
		return Out;
	}
};

Sprite* GetGlobalSpriteData(int Index);