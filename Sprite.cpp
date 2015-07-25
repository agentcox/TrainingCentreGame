#include "ddutil.h"
#include "dxutil.h"
#include "sprite.h"

extern CDisplay TheDisplay;

bool Sprite::Initialize(CDisplay& displayref, char* BMPFile, int FullWidth, int FullHeight, int FrameWidth, int FrameHeight, int NumStates, int* StateFrames)
{
	if(!BMPFile)
		return false;
	if(FAILED(displayref.CreateSurfaceFromBitmap(&mFullImage, BMPFile, FullWidth, FullHeight)))
		return false;
	mFullHeight = FullHeight;
	mFullWidth = FullWidth;
	mFullImage->SetColorKey(0xFF00FF);
	mSurf = mFullImage->GetDDrawSurface();
	mImageAlloc = true;
	mFrameWidth = FrameWidth;
	mFrameHeight = FrameHeight;
	mNumStates = NumStates;
	if(NumStates > 0 && StateFrames){
		mStateFrames = new int[NumStates];
		for(int i = 0; i < NumStates; i++){
			mStateFrames[i] = StateFrames[i];
		}
	}
	return true;
}

Sprite::~Sprite()
{
	if(mImageAlloc){
		mFullImage->Destroy();
		delete mFullImage;
	}
	if(mStateFrames)
		delete [] mStateFrames;
}

bool Sprite::Draw(CDisplay& displayref)
{
	if(!mSurf)
		return false;
	POINT pt;
	pt.x = (long)mXOrigin;
	pt.y = (long)mYOrigin;
	RECT copyfromrect = GetCurrentRectToDraw(displayref);
	if(pt.x < 0)
		pt.x = 0;
	if(pt.y < 0)
		pt.y = 0;
	displayref.ColorKeyBlt(pt.x, pt.y, mSurf, &copyfromrect);
	return true;
}

bool Sprite::UpdateFrames(int timediff)
{
	mTimer -= timediff;
	if(mTimer <= 0 && mTimeToNextFrame != -1){
		mTimer = mTimeToNextFrame;
		if(mStateFrames){
			mCurrentFrame++;
			if(mCurrentFrame >= mStateFrames[mCurrentState]){
				mCurrentFrame = 0;
				mHasPlayedStateOnce = true;
			}
		}
		else{
			mCurrentFrame++;
			if(mCurrentFrame >= (mFullHeight / mFrameHeight))
			{
				mCurrentFrame = 0;
				mHasPlayedStateOnce = true;
			}
		}
		return true;
	}
	return false;
}

int Sprite::GetNumFramesInCurrentState()
{
	if(mStateFrames)
	return mStateFrames[mCurrentState];
	else
	return (mFullHeight / mFrameHeight);
}

void Sprite::SetDelay(int newdelay)
{
	mTimeToNextFrame = newdelay;
}

void Sprite::SetFrame(int frame)
{
	if(mStateFrames)
	mCurrentFrame = frame % mStateFrames[mCurrentState];
	else
	mCurrentFrame = (frame) % (mFullHeight / mFrameHeight);

}
	
void Sprite::SetState(int state)
{
	mCurrentState = state % mNumStates;
	mCurrentFrame = 0;
	mHasPlayedStateOnce = false;
}

int Sprite::GetFrame()
{
	return mCurrentFrame;
}
	
int Sprite::GetState()
{
	return mCurrentState;
}

int Sprite::GetDelay()
{
	return mTimeToNextFrame;
}

int Sprite::GetHeight()
{
	return mFrameHeight;
}

int Sprite::GetWidth()
{
	return mFrameWidth;
}

void Sprite::SetLoc(POINT loc)
{
	mXOrigin = loc.x;
	mYOrigin = loc.y;
}

POINT Sprite::GetLoc()
{
	POINT ret;
	ret.x = (long)mXOrigin;
	ret.y = (long)mYOrigin;
	return ret;
}

bool Sprite::InitializeSpriteCopy(const Sprite* ref)
{
	if(!ref)
		return false;

	if(mImageAlloc)
		delete mFullImage;
	*this = *ref;
	TheDisplay.CreateSurfaceFromSurface(&mFullImage, ref->mFullImage);
	mFullImage->SetColorKey(0xFF00FF);
	mSurf = mFullImage->GetDDrawSurface();
	mImageAlloc = true;
	
	return true;
}

Sprite::Sprite(const Sprite& ref)
{
	if(mImageAlloc)
		delete mFullImage;
	*this = ref;
	TheDisplay.CreateSurfaceFromSurface(&mFullImage, ref.mFullImage);
	mFullImage->SetColorKey(0xFF00FF);
	mSurf = mFullImage->GetDDrawSurface();
	mImageAlloc = true;
}

Sprite::operator = (const Sprite& ref)
{
	mCurrentFrame = ref.mCurrentFrame;
	mCurrentState = ref.mCurrentState;
	mTimeToNextFrame = ref.mTimeToNextFrame;
	mTimer = ref.mTimer;
	mFrameWidth = ref.mFrameWidth;
	mFrameHeight = ref.mFrameHeight;
	mFullHeight = ref.mFullHeight;
	mFullWidth = ref.mFrameWidth;
	mNumStates = ref.mNumStates;
	mXOrigin = ref.mXOrigin;
	mYOrigin = ref.mYOrigin;
	
	if(mStateFrames)
			delete [] mStateFrames;

	if(ref.mNumStates > 1){
		mStateFrames = new int[mNumStates];
	for(int i = 0; i < mNumStates; i++)
		mStateFrames[i] = ref.mStateFrames[i];
	}
	else{
		mStateFrames = NULL;
	}
}
