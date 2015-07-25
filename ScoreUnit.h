class ScoreUnit
{
	Font* ScoreFont;
	Font* TitleFont;
	Font* CaptionFont;
	char* Title;
	char* Caption;
	int NumberRecords;
	int Score;
	int DigitsToDisplay;
	POINT Origin;
public:
	ScoreUnit() : NumberRecords(0), Title(NULL), Caption(NULL), Score(0), ScoreFont(NULL), TitleFont(NULL), CaptionFont(NULL), DigitsToDisplay(-1) {}
	~ScoreUnit()
	{
		if(Title)
			delete [] Title;
		if(Caption)
			delete [] Caption;
	}
	void Initialize(int ScoreFontID, int TitleFontID, int CaptionFontID)
	{
		ScoreFont = GetFontBySizeIndex(ScoreFontID);
		TitleFont = GetFontBySizeIndex(TitleFontID);
		CaptionFont = GetFontBySizeIndex(CaptionFontID);
	}
	void SetTitle(char* NewTitle) {if(Title)delete[]Title; Title = new char[strlen(NewTitle) + 1]; strcpy(Title, NewTitle);}
	void SetCaption(char* NewCaption) {if(Caption)delete[]Caption; Caption = new char[strlen(NewCaption) + 1]; strcpy(Caption, NewCaption);}
	void SetDigitsToDisplay(int NumDigits =  -1) { DigitsToDisplay = NumDigits;}
	void AddToScore(int ToAdd){Score += ToAdd; NumberRecords++;}
	void SubtractFromScore(int ToSub){Score -= ToSub; NumberRecords++;}
	void SetScore(int NewScore){Score = NewScore; NumberRecords++;}
	int GetScore(){return Score;}
	void ResetScore(){NumberRecords = 0; Score = 0;}
	int GetNumberRecords(){return NumberRecords;}
	void Draw(CDisplay& DisplayRef);
};