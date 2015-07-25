
#if !defined(AFX_COVERTOPS_H__968C4EDA_3F0D_11D5_BF15_0080C8FE513D__INCLUDED_)
#define AFX_COVERTOPS_H__968C4EDA_3F0D_11D5_BF15_0080C8FE513D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#define TRAININGCENTREVERSION 1.01
#define TRAININGCENTRELASTUPDATE	06302001

struct ResoStruct
{
	int width;
	int height;
	int bpp;
};

unsigned long inline HRExtractDXErr(HRESULT codein)
{
	codein ^= (1 << 31);
	codein ^= (0x876 << 16);
	return codein;
}


void InitObjects(CDisplay& Displayref);
void AddError(char* ErrorMessage);

#endif // !defined(AFX_COVERTOPS_H__968C4EDA_3F0D_11D5_BF15_0080C8FE513D__INCLUDED_)
