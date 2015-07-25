#include <list>
using namespace std;

#include "stdafx.h"
#include "resource.h"
#include "ddutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "sprite.h"
#include "sounds.h"
#include "scene.h"
#include "Weapon.h"
#include "physicalobject.h"
#include "gameevents.h"
#include "GlobalData.h"
#include "DataFilePro.h"


#define GLOBAL_GRAPHIC_MUZZLEFLASH	800

extern CDisplay TheDisplay;
extern CSoundManager SoundManager;
Sprite** GlobalGraphics;
ASound** GlobalSounds;

int NumGlobalGraphics = 0;
int NumGlobalSounds = 0;

#define LINESPERSOUND	2
#define LINESPERSPRITE	8

bool InitGlobalData()
{
	if(!InitGlobalSounds())
		return false;
	
	if(!InitGlobalSprites())
		return false;

	return true;
}

bool InitGlobalSprites()
{
	int* stateframes = NULL;
	char* temp;
	int w;
	int h;
	int fw;
	int fh;
	int ns;
	DataFilePro* DFP = DFPOpenDataFile("sprites.dat", DFP_FILE_READONLY);
	if(!DFP)
		return false;
	int NumLines = DFP->GetNumLines(); //FIRST INTEGER IS NUMBER OF SPRITES HERE.
	if(NumLines % LINESPERSPRITE){
		DFPCloseDataFile(DFP);
		return false;
	}
	NumGlobalGraphics = NumLines / LINESPERSPRITE;
	GlobalGraphics = new Sprite*[NumGlobalGraphics];
	for(int i = 0; i < NumGlobalGraphics; i++)
	{
		GlobalGraphics[i] = new Sprite;
		(GlobalGraphics[i])->SetGlobalIndex(DFP->t_GetInteger()); //THE GLOBAL INDEX;
		DFP->t_GetUnboundedString(&temp); //WE HAVE THE NAME.
		w = DFP->t_GetInteger(); //WIDTH
		h = DFP->t_GetInteger(); //HEIGHT
		fw = DFP->t_GetInteger(); //FRAME WIDTH
		fh = DFP->t_GetInteger(); //FRAME HEIGHT
		ns = DFP->t_GetInteger(); //NUM STATES
			stateframes = new int[ns];
			DFP->ChangeDelimiter(',');
			for(int k = 0; k < ns; k++)
			{
				stateframes[k] = DFP->t_GetInteger(); //FRAMES PER STATE
				
			}
		(GlobalGraphics[i])->Initialize(TheDisplay, temp, w, h, fw, fh, ns, stateframes);
		DFP->ChangeDelimiter('\n');
		DFP->t_GetChar(); //TO POLISH OFF LINE.
	}
	DFPCloseDataFile(DFP);
	return true;
}

bool InitGlobalSounds()
{
	int id;
	char* temp;
	DataFilePro* DFP = DFPOpenDataFile("sounds.dat", DFP_FILE_READONLY);
	if(!DFP)
		return false;
	int NumLines = DFP->GetNumLines();
	if(NumLines % LINESPERSOUND){
		DFPCloseDataFile(DFP);
		return false;
	}
	NumGlobalSounds = NumLines / LINESPERSOUND;
	GlobalSounds = new ASound*[NumGlobalSounds];
	for(int i = 0; i < NumGlobalSounds; i++)
	{
		id = DFP->t_GetInteger();
		DFP->t_GetUnboundedString(&temp);
		GlobalSounds[i] = new ASound(temp, 0, id);
		delete [] temp;
	}
	DFPCloseDataFile(DFP);
	return true;
}

void DestroyGlobalData()
{
	for(int i = 0; i < NumGlobalGraphics; i++)
		delete GlobalGraphics[i];
	delete [] GlobalGraphics;

	for(i = 0; i < NumGlobalSounds; i++)
		delete GlobalSounds[i];
	delete [] GlobalSounds;
}

ASound* GetGlobalSoundData(int Index)
{
	for(int i = 0; i < NumGlobalSounds; i++)
	{
		if(GlobalSounds[i]->GetIndex() == Index){
		return GlobalSounds[i];
		}
	}
	return NULL;
}

Sprite* GetGlobalSpriteData(int Index)
{
	for(int i = 0; i < NumGlobalGraphics; i++)
	{
		if(GlobalGraphics[i]->GetGlobalIndex() == Index){
		return GlobalGraphics[i];
		}
	}
	return NULL;
}