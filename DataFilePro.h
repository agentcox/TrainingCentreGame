/*************************************************
Squadron Blackops - Team III - GAM 200-250, 2000-2001
Module			: Data File Pro
Author(s)		: Charles Cox
CPP Files		: datafilepro.cpp
Function		: Provides Data File I/O
Notes			:
Last Revision	: 03.26.2001
*************************************************/
#define DFP_FILE_READONLY	1
#define DFP_FILE_CONTINUE	2
#define DFP_FILE_BINARY		4


class _DataFilePro
{
private:
	FILE* mFilePointer;
	bool mBinary;
	char mDelimiter;
	long mBookmark;
	bool mWriteReadFlushNeeded;
	bool mLastWrote;
	void DFPpv_P_StripNewLine(char* buf);
	void DFPpv_P_InterveningFlush();
protected:
	_DataFilePro() : mFilePointer(NULL), mBinary(false), mBookmark(0){}
	bool DFPpvOpenFile(char* Name, int Attributes);
	int DFPpvGetNumLines();
	char DFPpvGetDelimiter();
	char DFPpvChangeDelimiter(char NewDelimiter);
	char DFPpv_t_GetNextRawCharacter();
	bool DFPpv_t_ReadStringToDelimiter(char** UnAllocatedString); //RETURNS TRUE IF DELIMITER HIT, FALSE IF PAST END OF FILE.
	bool DFPpv_b_ReadStructure(void* Structure, size_t SizeofOne, size_t NumRead);
	bool DFPpvWriteToFile(void* Contents, size_t SizeofOne, size_t NumWritten);
	bool DFPpvSetBookmark(long Fpos);
	bool DFPpvBackUpToBookmark();
	long DFPpvGetFilePos();
public:
	~_DataFilePro() { if(mFilePointer)fclose(mFilePointer); }
	bool DFPpvCloseFile();
};

class DataFilePro : public _DataFilePro
{
public:
	int GetNumLines();
	long GetFilePosition();
	bool SetFilePosition(long FilePos);
	char ChangeDelimiter(char newdel);
	bool b_FillStructureMultiple(void* Structure, size_t SizeofStructure, size_t NumberOfStructures);
	bool b_FillStructure(void* Structure, size_t SizeofStructure);
	bool b_WriteStructure(void* Structure, size_t SizeofStructure);
	bool t_WriteInteger(int Integer);
	bool t_WriteFloat(float Float);
	bool t_WriteDouble(double Double);
	bool t_WriteChar(char Char);
	bool t_WriteString(char* String);
	int t_GetInteger();
	float t_GetFloat();
	double t_GetDouble();
	char t_GetChar();
	bool t_GetBool();
	void t_GetBoundedString(char* Out, int MaxLen);
	void t_GetUnboundedString(char** UnallocatedString);
	static DataFilePro* Create(char* Filename, int Attributes, char Delimiter);
};

//CREATION SYSTEMS
DataFilePro* DFPOpenDataFile(char* Filename, int Attributes, char Delimiter = '\n');
void DFPCloseDataFile(DataFilePro* DFP);