class Font
{
	Sprite* mLetterSprite;
	int* mLetterWidths;
	int* mLetterOffsets;
	int mSpaceWidth;
	int mLetterHeight;
	int mSizeIndex;
	int mFullWidth;
	int mFullHeight;
public:
	Font() : mLetterSprite(NULL), mLetterWidths(NULL), mLetterOffsets(NULL){}
	~Font()
	{
		if(mLetterSprite){
			delete mLetterSprite;
		}
		if(mLetterWidths)
		{
			delete [] mLetterWidths;
		}
		if(mLetterOffsets)
			delete mLetterOffsets;
	}
	int GetIndex(){return mSizeIndex;}
	void SetIndex(int index){mSizeIndex = index;}
	void LoadFont(char* bmp, int* LetterWidths, int SpaceWidth, int LetterHeight, int SizeIndex, int FullWidth, int FullHeight);
	int DrawLetter(char Letter, POINT DrawTo);
	void DrawString(char* String, POINT DrawTo);
	int GetCharWidth(char Character);
	int GetStringWidth(char* String);
	int GetCharHeight() {return mLetterHeight;}
};

bool LoadGameFonts();
void DestroyGameFonts();
Font* GetFontBySizeIndex(int index);