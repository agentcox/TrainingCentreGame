#include <windows.h>
#include <list>
#include "dxutil.h"
#include "dsutil.h"
#include "sounds.h"
/*
class ASound
{
private:
	CSound* TheSound;
public:
	ASound(char* soundfile);
	ASound();
	~ASound();
	ASound(ASound& ref);
	ASound(char* soundfile);
};

class SoundQueue
{
private:
	list<ASound> mList;
public:
	InsertSound(char* soundfile, int delaytoplay);
	ClearQueue();
	~SoundQueue() {ClearQueue();}
};
*/
extern CSoundManager SoundManager;

ASound::ASound()
{
	TheSound = NULL;
	GlobalSoundIndex = 0;
	Playing = false;
}

ASound::~ASound()
{
	if(TheSound)
		delete TheSound;
}

ASound::ASound(const ASound& ref)
{
	*this = ref;
	CWaveFile* WV = GetGlobalSoundData(ref.GlobalSoundIndex)->TheSound->GetWaveFile();
	LPDIRECTSOUNDBUFFER dbuf;
	SoundManager.GetDirectSound()->DuplicateSoundBuffer(GetGlobalSoundData(ref.GlobalSoundIndex)->TheSound->GetBuffer(0), &dbuf);
	TheSound = new CSound(&dbuf, WV->GetSize(), 1, WV);
	TheSound->m_pWaveFile = new CWaveFile;
	TheSound->m_pWaveFile->OpenFromMemory(WV->m_pbData, WV->GetSize(), WV->m_pwfx, 0);
	GlobalSoundIndex = ref.GlobalSoundIndex;
	DelayToPlay = ref.DelayToPlay;
	Playing = false;
}
/*
ASound& ASound::operator = (const ASound& ref)
{
	CWaveFile* WV = ref.TheSound->GetWaveFile();
	SoundManager.CreateFromMemory(&TheSound, WV->m_pbData, WV->GetSize(), WV->m_pwfx, 0);
	SoundFile = new char[strlen(ref.SoundFile) +1];
	strcpy(SoundFile, ref.SoundFile);
	DelayToPlay = ref.DelayToPlay;
	Playing = false;
	return *this;
}
*/
ASound::ASound(char* soundfile, int timedelay, int globalsoundindex)
{
	SoundManager.Create(&TheSound, soundfile);
	GlobalSoundIndex = globalsoundindex;
	DelayToPlay = timedelay;
	Playing = false;
}

void ASound::InitializeSoundCopy(ASound* ref)
{
	*this = *ref;
	CWaveFile* WV = ref->TheSound->GetWaveFile();
	SoundManager.CreateFromMemory(&TheSound, WV->m_pbData, WV->GetSize(), WV->m_pwfx, 0);
	GlobalSoundIndex = ref->GlobalSoundIndex;
	DelayToPlay = ref->DelayToPlay;
	Playing = false;
}

void ASound::Play()
{
	if(TheSound){
		TheSound->Play(0,0);
		Playing = true;
	}
}

void ASound::DecrementTime(int timedown)
{
	DelayToPlay -= timedown;
}

bool ASound::TimeUp()
{
	return (DelayToPlay <= 0);
}

void SoundQueue::ClearQueue()
{
	mList.clear();
}

bool SoundQueue::InsertSound(char* soundfile, int delay, int gsindex)
{
	ASound newsnd(soundfile, delay, gsindex);
	mList.push_front(newsnd);
	return true;
}

bool SoundQueue::InsertSound(ASound& ref)
{
	ASound newsnd(ref);
	mList.push_front(newsnd);
	return true;
}

void SoundQueue::Update(int timediff)
{
	list<ASound>::iterator i = mList.begin();
	int NumTimes = 0;
	for(; i != mList.end(); i++, NumTimes++)
	{
		
		i->DecrementTime(timediff);
		
		if (i->TimeUp() && !i->Playing)
		{
			i->Play();
		}
		if(i->Playing && !i->TheSound->IsSoundPlaying())
		{
			i->TheSound->Stop();
			i = mList.erase(i);
		}
	}
	_asm nop;
}
