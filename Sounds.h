#ifndef SOUNDS_H
#define SOUNDS_H
using namespace std;
class ASound
{
private:
	CSound* TheSound;
	int GlobalSoundIndex;
	int DelayToPlay;
	bool Playing;
public:
	friend class SoundQueue;
	ASound(char* soundfile, int timedelay, int globalsoundindex);
	ASound();
	ASound(const ASound& ref);
//	ASound& operator = (const ASound & ref);
	~ASound();
	void InitializeSoundCopy(ASound* ref);
	void Play();
	void DecrementTime(int timedown);
	int GetIndex() {return GlobalSoundIndex;}
	bool TimeUp();
};

class SoundQueue
{

	list<ASound> mList;
public:
	bool InsertSound(char* soundfile, int delaytoplay, int gsindex);
	bool InsertSound(ASound& ref);
	void ClearQueue();
	void Update(int timediff);
	~SoundQueue() {ClearQueue();}
};

ASound* GetGlobalSoundData(int Index);

#endif