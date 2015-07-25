#include <stdio.h>
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "globaldata.h"
#include "font.h"
#include "scoreunit.h"

void ScoreUnit::Draw(CDisplay& DisplayRef)
{
	//SO WE NEED TO GRAB ALL THE WIDTHS AND CHECK THE LONGEST ONE. PUSH THE WHOLE SHEBANG BACK BY THAT MUCH
	//SO THAT THE SCORE IS ALWAYS VISIBLE. TOP-RIGHT, REMEMBER.

	if(!CaptionFont || !TitleFont || !ScoreFont)
		return;
	int LongestWidth = 0;
	int TempWidth = 0;
	bool IsLongestWidthScore = false;
	char ScoreString[5000];
	
	if(DigitsToDisplay == -1)
	sprintf(ScoreString, "%.*d", DigitsToDisplay, Score);
	else
		sprintf(ScoreString, "%d", Score);
	
	LongestWidth = TitleFont->GetStringWidth(Title);
	TempWidth = ScoreFont->GetStringWidth(ScoreString) + 2 + CaptionFont->GetStringWidth(Caption);
	if(TempWidth > LongestWidth){
		LongestWidth = TempWidth;
		IsLongestWidthScore = true;
	}

	RECT Client;
	GetClientRect(DisplayRef.GetHWnd(), &Client);

	Origin.x = Client.right - LongestWidth - 20;
	Origin.y = 15; //START A LITTLE FROM THE TOP.

	//START AT TOP WITH THE TITLE.
	POINT FinalOrigin;
	if(IsLongestWidthScore)
	{
		FinalOrigin.x = Origin.x + (LongestWidth / 2);
		FinalOrigin.x -= TitleFont->GetStringWidth(Title) / 2;
		FinalOrigin.y = Origin.y;
	}
	else
	{
		FinalOrigin.x = Origin.x;
		FinalOrigin.y = Origin.y;
	}
	TitleFont->DrawString(Title, FinalOrigin);
	FinalOrigin.y += TitleFont->GetCharHeight() + 10;
	if(IsLongestWidthScore)
	{
		FinalOrigin.x = Origin.x;
	}
	else
	{
		FinalOrigin.x = Origin.x + (LongestWidth / 2);
		FinalOrigin.x -= (ScoreFont->GetStringWidth(ScoreString) + CaptionFont->GetStringWidth(Caption) + 2) / 2;
	}
	ScoreFont->DrawString(ScoreString, FinalOrigin);
	FinalOrigin.x += 2 + ScoreFont->GetStringWidth(ScoreString);
	CaptionFont->DrawString(Caption, FinalOrigin);
}