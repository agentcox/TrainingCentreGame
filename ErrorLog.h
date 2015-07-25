///CHARLES COX'S ERROR LOG AND REPORTING DYNAMIC (C2ELRD) (c)2001 Charles Cox
#include <stdio.h>
#include <string.h>

class ErrorInformation
{
private:
	int mErrorID;
	char* mErrorString;
	bool mAllocated;
public:
	//DEFAULT CONSTRUCTOR
	ErrorInformation()
	{
		mErrorID = -1;
		mErrorString = NULL;
		mAllocated = false;
	}

	ErrorInformation(ErrorInformation& E)
	{
			
		mErrorID = E.mErrorID;
		mAllocated = true;
		mErrorString = new char [strlen(E.mErrorString) + 1];
		strcpy(mErrorString, E.mErrorString);
	}

	ErrorInformation(int ID, char* ErrorString)
	{
		if(ErrorString && strlen(ErrorString) > 0 && ID >= 0){
			mAllocated = true;
			mErrorID = ID;
			mErrorString = new char[strlen(ErrorString) +1];
			strcpy(mErrorString, ErrorString);
		}
		else{
			mAllocated = false;
			mErrorID = -1;
			mErrorString = NULL;
		}
		
	}
	~ErrorInformation()
	{
		if(mAllocated){
			delete [] mErrorString;
			mAllocated = false;
			mErrorID = -1;
			mErrorString = NULL;
		}
	}
	char* getErrorString(){
		if(!mAllocated){
			return NULL;
		}
		return mErrorString;
	}
	int getErrorID(){
		return mErrorID;
	}
	operator = (ErrorInformation& E)
	{
		if(E.mAllocated && E.mErrorString){
		mErrorID = E.mErrorID;
		if(mAllocated){
			delete [] mErrorString;
		}
		mAllocated = true;
		mErrorString = new char [strlen(E.mErrorString) + 1];
		strcpy(mErrorString, E.mErrorString);
		}
	}
};

class ErrorLog
{
private:
	FILE * mFilePointer; //THE ERROR LOG'S FILE POINTER
	char* mErrorFileName; //THE ERROR LOG'S FILE NAME
	ErrorInformation mTemporaryInfo; //TO BE USED BY THE SUBSCRIPT OPERATOR
	int mErrorLogID; //THE ERROR LOG FILE NUMBER ID
	bool mFileBeingAccessed; //IS THE FILE CURRENTLY BEING ACCESSED?
	int mNumberOfErrors; //THE NUMBER OF ERRORS PRESENT IN THIS LOG
	int FindSuitableErrorLogFileNumber(); //PRIVATE FUNCTION TO GET THE NEXT AVAILABLE ERROR LOG ID TO STORE THE LOG IN.
	void AddInitialErrorLogString(); //ADDS A TIMESTAMP AND ID STRING TO THE ERROR LOG FILE. TO BE CALLED ONCE.
	void AddFinalErrorLogString(); //ADDS FINAL TIMESTAMP AND ID STRING TO THE ERROR LOG FILE. TO BE CALLED ONCE.
	FILE* fopen(const char* filename, const char* attribs); //OVERLOADED FOPEN THAT WATCHES FOR FILE ACCESS VIOLATIONS
	void fclose(FILE* fp); //OVERLOADED FCLOSE THAT WATCHES FOR FILE ACCESS VIOLATIONS
	bool OpenErrorLog(); //THE STANDARD ERROR LOG OPENING METHOD
	void CloseErrorLog(); //THE STANDARD ERROR LOG CLOSING METHOD
	int GetNumberIDDigits(); //RETRIEVES THE NUMBER OF ID DIGITS, USED TO DETERMINE STRING WIDTH FOR new OPERATOR
	bool WriteErrorString(char* ErrorString); //THIS IS HOW ERROR STRINGS SHOULD BE WRITTEN - CALLS OpenErrorLog and CloseErrorLog
	bool GetInternalErrorInf(int ErrorNumber);
	void StripStringNewLine(char* String);
	int SeekToNextAsterix();
public:
	//DEFAULT CONSTRUCTOR
	ErrorLog()
	{
		mFileBeingAccessed = false;
		int Number;
		char Name[500];
		//SO, LET'S TRY TO GET A SUITABLE FILENAME.
		Number = FindSuitableErrorLogFileNumber();
		sprintf(Name, "Err%d.log", Number);
		
		if(!(mFilePointer = ::fopen(Name, "w+"))){
			mNumberOfErrors = -1;
			mErrorLogID = -1;
			delete [] mErrorFileName;
		}
		else{
			mErrorFileName = new char[strlen(Name) + 1];
			strcpy(mErrorFileName, Name);
		::fclose(mFilePointer); //WE'RE GOOD TO GO.
		mErrorLogID = Number;
		mNumberOfErrors = 0;
		AddInitialErrorLogString();
		}
	}
	//DEFAULT DESTRUCTOR
	~ErrorLog()
	{
		AddFinalErrorLogString();
		if(mFilePointer){
			CloseErrorLog();
		}
		if(mErrorFileName){
			delete [] mErrorFileName;
		}
	}

	//PUBLIC FUNCTIONALITY
	char * operator [] (int ErrorNum);
	bool AddError(char* ErrorDescription);
	int GetNumberOfErrors();
	ErrorInformation GetErrorInformation(int ErrorID);
	void DumpErrors(int NumToDisplayAtOnce);
};