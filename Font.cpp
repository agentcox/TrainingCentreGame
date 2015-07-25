#include <list>
using namespace std;
#include <time.h>
#include <stdlib.h>
#include "stdafx.h"
#include "resource.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "sounds.h"
#include "scene.h"
#include "weapon.h"
#include "physicalobject.h"
#include "gameevents.h"
#include "globaldata.h"
#include "datafilepro.h"
#include "font.h"

#define NUMBEROFLETTERS	36
extern CDisplay TheDisplay;

#define FONTDATAFILENUMLINES	5
Font* GameFonts;
int NumFonts;

void Font::LoadFont(char* bmp, int* LetterWidths, int SpaceWidth, int LetterHeight, int SizeIndex, int FullWidth, int FullHeight)
{
	int m, i;
	int NewWidths[NUMBEROFLETTERS];
	for(m = 0; m < NUMBEROFLETTERS; m++)
	{
		NewWidths[m] = 0;
		for(i = 0; i < m; i++)
	{
	NewWidths[m] += LetterWidths[i];
	}
	}
	mLetterSprite = new Sprite;
	mLetterSprite->Initialize(TheDisplay, bmp, FullWidth, FullHeight, FullWidth, FullHeight, 1, NULL);
	mFullWidth = FullWidth;
	mFullHeight = FullHeight;
	mLetterWidths = new int[NUMBEROFLETTERS];
	mLetterOffsets = new int[NUMBEROFLETTERS];
	for(i = 0; i < NUMBEROFLETTERS; i++)
	{
		mLetterOffsets[i] = NewWidths[i];
		mLetterWidths[i] = LetterWidths[i];
	}
	mLetterHeight = LetterHeight;
	mSpaceWidth = SpaceWidth;
}

int Font::DrawLetter(char Letter, POINT DrawTo)
{
	if(Letter == ' ')
		return mSpaceWidth;
	Letter = toupper(Letter);
	Letter -= 48; //BRINGS IT TO ZERO REF.
	if(Letter < 0)
		return 0;
	if(Letter > 9)
	{
		Letter -= 7;
		if(Letter < 0)
			return 0;
	}
	if(Letter > NUMBEROFLETTERS)
		return 0;
	RECT GetRt;
	GetRt.left = mLetterOffsets[Letter];
	GetRt.right = GetRt.left + mLetterWidths[Letter];
	if(GetRt.right > mFullWidth)
		GetRt.right = mFullWidth;
	GetRt.top = 0;
	GetRt.bottom = mLetterHeight;
	mLetterSprite->SetLoc(DrawTo);
	GetRt = mLetterSprite->ClipRectAgainstScreen(TheDisplay, GetRt);
	TheDisplay.ColorKeyBlt(DrawTo.x, DrawTo.y, mLetterSprite->mFullImage->GetDDrawSurface(), &GetRt);
	return mLetterWidths[Letter];
}

void Font::DrawString(char* String, POINT DrawTo)
{
	int w;
	int len = strlen(String);
	for(int i = 0; i < len; i++)
	{
		if(isspace(String[i]))
			DrawTo.x += mSpaceWidth;
		else
		{
			w = DrawLetter(String[i], DrawTo);
			DrawTo.x += w;
		}
	}
}

int Font::GetCharWidth(char Letter)
{
	Letter = toupper(Letter);
	Letter -= 48; //BRINGS IT TO ZERO REF.
	if(Letter < 0)
		return mSpaceWidth;
	if(Letter > 9)
	{
		Letter -= 7;
		if(Letter < 0)
			return 0;
	}
	if(Letter > NUMBEROFLETTERS)
		return mSpaceWidth;
	return mLetterWidths[Letter];
}

int Font::GetStringWidth(char* String)
{
	int w = 0;
	int len = strlen(String);
	for(int i = 0; i < len; i++)
	{
		if(isspace(String[i]))
			w += mSpaceWidth;
		else
		{
			w += GetCharWidth(toupper(String[i]));
		}
	}
	return w;
}

bool LoadGameFonts()
{
	char* temp;
	int* lw = new int[NUMBEROFLETTERS];
	int fh;
	int fw;
	int sw;
	DataFilePro* DFP = DFPOpenDataFile("fonts.dat", DFP_FILE_READONLY);
	if(!DFP)
		return false;
	int NumLines = DFP->GetNumLines();
	if(NumLines % FONTDATAFILENUMLINES)
	{
	DFPCloseDataFile(DFP);
	return false;
	}
	NumFonts = NumLines / FONTDATAFILENUMLINES;
	GameFonts = new Font[NumFonts];
	for(int i = 0; i < NumFonts; i++)
	{
		GameFonts[i].SetIndex(i);
		DFP->t_GetUnboundedString(&temp);
		sw = DFP->t_GetInteger();
		fw = DFP->t_GetInteger();
		fh = DFP->t_GetInteger();
		DFP->ChangeDelimiter(',');
		for(int k = 0; k < NUMBEROFLETTERS; k++)
		{
			lw[k] = DFP->t_GetInteger();
		}
		DFP->ChangeDelimiter('\n');
		DFP->t_GetChar();
		GameFonts[i].LoadFont(temp, lw, sw, fh, i, fw, fh);
		delete [] temp;
		
	}
	delete [] lw;
	DFPCloseDataFile(DFP);
	return true;
}

void DestroyGameFonts()
{
	delete [] GameFonts;
}

Font* GetFontBySizeIndex(int index)
{
	for(int i = 0; i < NumFonts; i++)
	{
		if(GameFonts[i].GetIndex() == index)
			return &(GameFonts[i]);
	}
	return NULL;
}