class TimerUnit
{
	Sprite LargeNumberSprite;
	Sprite SmallNumberSprite;
	Sprite CasingSprite;
	char* Caption;
	Font* CaptionFont;
	POINT AbsCasingOrigin;
	POINT RelCaptionOrigin;
	POINT RelTimeOrigin;
	int Status; //0 - OPEN AND READY, 1 - OPENING, 2 - CLOSING, 3 - CLOSED AND NOT READY
	int StoredDigitArray[6];
	int StoredMinutes;
	int StoredSeconds;
	int StoredSplit;
	int CurrentMinutes;
	int CurrentSeconds;
	double CurrentSplit;
	bool Running;
	bool TimeUp;
	void WrapAround();
	void UpdateDigits();
	void Draw(CDisplay& DisplayRef);
public:
	TimerUnit() : Caption(NULL), Running(false), TimeUp(false), StoredMinutes(0), StoredSeconds(0), StoredSplit(0), CurrentMinutes(0), CurrentSeconds(0), CurrentSplit(0) {}
	~TimerUnit() { if(Caption)delete [] Caption;}
	void Initialize(int LargeNumberSpriteID, int SmallNumberSpriteID, int CasingSpriteID, int CaptionFontID);
	void SetCaption(char* NewCaption);
	void SetTime(int Minutes, int Seconds, int Split);
	bool IsReadyToBeginCounting();
	void Reset();
	void Freeze();
	void Open();
	void Close();
	void Unfreeze();
	bool IsTimeUp();
	void Update(int TimeDiff, CDisplay& DisplayRef);
};