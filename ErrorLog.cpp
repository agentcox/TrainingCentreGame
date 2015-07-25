///CHARLES COX'S ERROR LOG AND REPORTING DYNAMIC (C2ELRD) (c)2001 Charles Cox

#include "ErrorLog.h"
#include <time.h>

///////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////
int ErrorLog::FindSuitableErrorLogFileNumber()
{
	FILE* FP;
	int Number = 0;
	char Name[500];
	strcpy(Name, "Err0.log");
	while((FP = ::fopen(Name, "r")) != NULL)
	{
		fclose(FP);
		FP = NULL;
		Number++;
		sprintf(Name, "Err%d.log", Number);
	}
	if(FP){
		::fclose(FP);
	}
	return Number;
}

bool ErrorLog::OpenErrorLog()
{
	FILE * fp;
	if((fp = fopen(mErrorFileName, "a+")) == NULL){ //CALL OVERLOADED FOPEN
		return false;
	}
	else{
		mFilePointer = fp;
		return true;
	}
}

void ErrorLog::CloseErrorLog()
{
	if(mFilePointer){
		fclose(mFilePointer); //CALL OVERLOADED FCLOSE
		mFilePointer = NULL;
	}
}

FILE* ErrorLog::fopen(const char* filename, const char* attribs)
{
	FILE* FP;
	if(mFileBeingAccessed){
		return NULL;
	}
	else{
		
	FP = ::fopen(filename, attribs); //CALL STD FOPEN
	if(FP){
	mFileBeingAccessed = true;
	}
	return FP;
	}
}

void ErrorLog::fclose(FILE* fp)
{
	if(fp){
		::fclose(fp); //CALL STD FCLOSE.
	}
	mFileBeingAccessed = false;
}

void ErrorLog::AddInitialErrorLogString()
{
	time_t OurTime = time(NULL);
	tm* timestruct = localtime(&OurTime); //LOCAL TIME IS REQUIRED.
	char InitialString[500];
	sprintf(InitialString, "Charles Cox's Error Log and Reporting Dynamic (C2ELRD) (c)2001 Charles Cox - Log #%d", mErrorLogID); //LET US NOT FORGET WHO WROTE THIS THING.
	if(!OpenErrorLog()){
		return;
	}
	fputs(InitialString, mFilePointer);
	fputc('\n', mFilePointer);
	sprintf(InitialString, "Logging Started %d/%d/%d - %2.2d:%2.2d", timestruct->tm_mon + 1, timestruct->tm_mday, 1900 + timestruct->tm_year, timestruct->tm_hour, timestruct->tm_min);
	fputs(InitialString, mFilePointer);
	fputc('\n', mFilePointer);
	sprintf(InitialString, "--------------------------------------------");
	fputs(InitialString, mFilePointer);
	fputc('\n', mFilePointer);
	CloseErrorLog();
}

void ErrorLog::AddFinalErrorLogString()
{
		time_t OurTime = time(NULL);
	tm* timestruct = localtime(&OurTime); //LOCAL TIME IS REQUIRED.
	char InitialString[500];
	
	if(!OpenErrorLog()){
		return;
	}
	
	sprintf(InitialString, "--------------------------------------------");
	fputs(InitialString, mFilePointer);
	fputc('\n', mFilePointer);
	sprintf(InitialString, "Logging Ended %d/%d/%d - %2.2d:%2.2d", timestruct->tm_mon + 1, timestruct->tm_mday, 1900 + timestruct->tm_year, timestruct->tm_hour, timestruct->tm_min);
	fputs(InitialString, mFilePointer);
	fputc('\n', mFilePointer);
	
	CloseErrorLog();
}


int ErrorLog::GetNumberIDDigits()
{
	if(!mErrorLogID && mNumberOfErrors < 1){
		return 1;
	}
	int CurrentMultiple = 10;
	int NumDigitsNeeded = 0;
	while(mNumberOfErrors / CurrentMultiple > 0){
		CurrentMultiple *= CurrentMultiple;
		NumDigitsNeeded++;
	}
	return NumDigitsNeeded + 1;
}

bool ErrorLog::WriteErrorString(char* ErrorString)
{
	if(!ErrorString){
		return false;
	}
	if(mFileBeingAccessed){
		return false;
	}
	if(!OpenErrorLog()){
		return false;
	}

	fputs(ErrorString, mFilePointer);
	fputc('\n', mFilePointer); //NEW LINE CHARACTER IS REQUIRED.
	CloseErrorLog();
	return true;
}

bool ErrorLog::GetInternalErrorInf(int ErrorNumber)
{
	//RIGHT. OKAY. THIS FILLS THE TEMPORARY ERROR INFORMATION STRUCTURE WITH STUFF.
	//WE CANNOT INITIALIZE AN INSTANCE OF AN ERRORINFO UNTIL WE HAVE THE STRING AND THE ID.
	long FirstAsterixPoint;
	long OurErrorFrontAsterixPoint;
	long OurErrorBackAsterixPoint;
	long CurrentPos = 0;
	long NumToMalloc;
	char * TempBuf;
	char * PureString;
	int StringPos;
	int PureStringStartPos;
	int PureStringEndPos;
	int PureStringLen;
	int StringLen;
	
	int CurrentError;
	//FIRST, ELIMINATE WRONG CASES.
	if(ErrorNumber < 0 || ErrorNumber >= mNumberOfErrors || !mErrorFileName || mErrorLogID < 0){
		return false;
	}

	//NOW, WE UTILIZE AN ALGORITHM TO SEARCH THROUGH FILE. START BY MOVING TO THE FIRST ASTERIX.
	if(!OpenErrorLog()){
		return false;
	}
	fseek(mFilePointer, 0, SEEK_SET);

	FirstAsterixPoint = SeekToNextAsterix();
	if(FirstAsterixPoint == -1){
		CloseErrorLog();
		return false;
	} //WE CAN NOW USE THIS FIRST ASTERIX POINT AS AN OFFSET.

	OurErrorFrontAsterixPoint = FirstAsterixPoint;

	for(CurrentError = -1; CurrentError < ErrorNumber - 1; CurrentError++){
		SeekToNextAsterix();
		OurErrorFrontAsterixPoint = SeekToNextAsterix();
	}

	//WE NOW SHOULD BE AT THE FIRST ASTERIX OF OUR DESIRED LINE.
	//RUN TO THE NEXT ASTERIX, RETRIEVE THE DIFFERENCE, AND THAT'S OUR MALLOC AMOUNT... ADD 3 FOR NEWLINE, ENDCHAR, AND LF WACKINESS.
	OurErrorBackAsterixPoint = SeekToNextAsterix();
	if(OurErrorFrontAsterixPoint == -1 || OurErrorBackAsterixPoint == -1){
		CloseErrorLog();
		return false;
	}
	NumToMalloc = OurErrorBackAsterixPoint - OurErrorFrontAsterixPoint;
	TempBuf = new char[NumToMalloc + 3];
	//SHOULD BE ABLE TO SET BACK TO THE LAST ASTERIX, THEN DO AN FGETS, STRIP NEW LINE.
	fseek(mFilePointer, OurErrorFrontAsterixPoint, SEEK_SET);
	fgets(TempBuf, NumToMalloc, mFilePointer);
	//THAT HAD OUGHT TO BE ALL WE NEED WITH THE FILE.
	CloseErrorLog();
	StripStringNewLine(TempBuf);
	StringLen = strlen(TempBuf); //WE HAD BETTER BE SURE.


	//TADA. NOW PARSE THE STRING.
	for(StringPos = 0; StringPos < StringLen;  StringPos++){
		if(TempBuf[StringPos] == '-' && TempBuf[StringPos + 1] == ' ' && TempBuf[StringPos - 1] == ' '){ //THIS LOOKS LIKE OUR START POINT.
			PureStringStartPos = StringPos + 2;
			break;
		}
	}
	if(PureStringStartPos == -1){
		delete [] TempBuf;
		return false;
	}
	PureStringEndPos = StringLen - 3; //ONE FOR '\0', ONE FOR *, AND ONE FOR ZERO-BASED COUNTING SYSTEM
	PureStringLen = PureStringEndPos - PureStringStartPos;
	PureString = new char[StringLen + 2];
	for(int i = 0, k = PureStringStartPos; i < StringLen; i++, k++){
		PureString[i] = TempBuf[k];
	}
	PureString[i] = '\0';
	//WE GOT IT.
	delete [] TempBuf;
	ErrorInformation EInfo(ErrorNumber, PureString);
	mTemporaryInfo = EInfo;
	delete [] PureString;
	return true;
}

int ErrorLog::SeekToNextAsterix()
{
	int BogusChar;
	if(!mFileBeingAccessed){
		return -1;
	}

	do
	{
		
		BogusChar = fgetc(mFilePointer);
		if(BogusChar == EOF){
			return -1;
		}
	}while(BogusChar != '*');
	return ftell(mFilePointer);

}

void ErrorLog::StripStringNewLine(char* String)
{
	if(!String || strlen(String) < 1){
		return;
	}
	int Len = strlen(String);
	for(int i = Len; i >= 0; i--){
		if(String[i] == '\n'){
			String[i] = '\0';
			return;
		}
	}
}


///////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////
bool ErrorLog::AddError(char* ErrorDescription)
{
	char* Default = "NO DESCRIPTION GIVEN";
	char* OurNewString;
	char* CharStringToCopyOver;
	
	//ALLOW FOR NULL STRINGS TO BE PASSED.
	if(!ErrorDescription){
		CharStringToCopyOver = Default;
	}
	else{
		CharStringToCopyOver = ErrorDescription;
	}

	OurNewString = new char [strlen(CharStringToCopyOver) + 8 + GetNumberIDDigits()]; //ALLOCATE EXTRA SPACE FOR CHARACTERS
	sprintf(OurNewString, "*[%d] - %s*", mNumberOfErrors, CharStringToCopyOver);

	if(!WriteErrorString(OurNewString)){
		return false;
	}
	mNumberOfErrors++;
	return true;
}

int ErrorLog::GetNumberOfErrors()
{
	if(!mErrorFileName || mErrorLogID < 0 || mNumberOfErrors < 0){
		return 0;
	}
	return mNumberOfErrors;
}

ErrorInformation ErrorLog::GetErrorInformation(int ErrorID)
{
	ErrorInformation EInf;
	if(GetInternalErrorInf(ErrorID)){
		EInf = mTemporaryInfo;
		
	}
	return EInf;
	
}

void ErrorLog::DumpErrors(int NumToDisplayAtOnce)
{
//	char* String;
//	int ID;
	int NumDisplayed;
	int TotalDisplayed;
	printf("Error Log Dump Beginning...\n");
	printf("-----------------------------------\n");
	if(mNumberOfErrors < 1){
		printf("Error Log Is Clear.\n");
	}
	
	
	
	for(TotalDisplayed = 0; TotalDisplayed < mNumberOfErrors;){
		for(NumDisplayed = 0; NumDisplayed < NumToDisplayAtOnce && TotalDisplayed < mNumberOfErrors; NumDisplayed++, TotalDisplayed++){
			GetInternalErrorInf(TotalDisplayed);
			printf("[%d] - %s\n", mTemporaryInfo.getErrorID(), mTemporaryInfo.getErrorString());
		}
		if(NumDisplayed >= NumToDisplayAtOnce && NumDisplayed < mNumberOfErrors){
		printf("Holding...Press ENTER To Resume.\n");
		getchar();
		printf("Resuming Error Dump...\n");
		}
	}
	printf("-----------------------------------\n");
	printf("End Of Error Log Dump.\n");
}

char * ErrorLog::operator [] (int ErrorNum)
{
		if(!GetInternalErrorInf(ErrorNum)){
			return "ILLEGAL ERROR ADDRESS";
		}
		return mTemporaryInfo.getErrorString();
}