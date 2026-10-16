// OpCRC32.cpp: implementation of the COpCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpCRC32.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpCRC32::COpCRC32()
{
	DWORD	dwElement;
    ///unsigned long 	dwElement;
    LONG	lElementIndex, lPolyIndex;

    for (lElementIndex = 0; lElementIndex < 256; lElementIndex ++)
    {
        dwElement = lElementIndex;
        for (lPolyIndex = 8; lPolyIndex > 0; lPolyIndex --)
        {
            if (dwElement & 1)
                dwElement = (dwElement >> 1) ^ POLYNOMIAL;
            else
                dwElement >>= 1;
        }
        m_pdwCRC32Table[lElementIndex] = dwElement;
    }
}

COpCRC32::~COpCRC32()
{
}

DWORD COpCRC32::GetThisCRC(BYTE bThisByte)
{
	DWORD	dwOldCRC32;
	DWORD	dwTemp1, dwTemp2, dwTemp3, dwTemp4;

	dwOldCRC32 = m_dwCRC32;

//    m_dwCRC32 = ((dwOldCRC32 >> 8) & 0x00FFFFFF) ^ m_pdwCRC32Table[(dwOldCRC32 ^bThisByte) & 0x000000FF];
	dwTemp1 = (dwOldCRC32 >> 8) & 0x00FFFFFF;
	dwTemp2 = (dwOldCRC32 ^ bThisByte) & 0x000000FF;
	dwTemp3 = m_pdwCRC32Table[dwTemp2];
	dwTemp4 = dwTemp1 ^ dwTemp3;
			
	m_dwCRC32 = dwTemp4;

    return m_dwCRC32;

}

DWORD COpCRC32::GetCRC(PBYTE pDataStream, DWORD dwByteCount)
{
	DWORD	dwByteIndex;
	BYTE	bThisByte;

	//	Set m_dwCRC32 = 0xFFFFFFFF;
	InitCRC32();

	dwByteIndex = 0;
   	while (dwByteIndex < dwByteCount)
	{
		bThisByte = *pDataStream;
		GetThisCRC(bThisByte);

		pDataStream ++;
		dwByteIndex++;
	}
 
	return ~m_dwCRC32;
}

VOID COpCRC32::InitCRC32()
{
	m_dwCRC32 = 0xFFFFFFFF;
}
DWORD COpCRC32::GetCRCByDWORD(PBYTE pDataStream, DWORD dwByteCount)
{
	DWORD	dwByteIndex;
	DWORD   dwDWORDLength;
	DWORD   dwResverdLength;
	PBYTE   pTempPtr=NULL;
	dwByteIndex = 0;
	DWORD dwSum=0;
	dwByteIndex =0;
	pTempPtr = pDataStream;
	dwDWORDLength = dwByteCount/4; //按4字节获得DWORD字长度
	dwResverdLength = dwByteCount%4;//如果不能被4整除,取余数
	for(DWORD dwIndex=0;dwIndex < dwDWORDLength;dwIndex++ )
	{
		DWORD dwTemp=0;
		dwTemp = pTempPtr[0]<<24;
		dwTemp += pTempPtr[1]<<16;
		dwTemp += pTempPtr[2]<<8;
		dwTemp += pTempPtr[3];
		pTempPtr += 4;
		dwSum += dwTemp;
	}
    if(dwResverdLength !=0)
	{
		if(dwResverdLength==3)
		{
			DWORD dwTemp=0;
			dwTemp = pTempPtr[0]<<24;
			dwTemp += pTempPtr[1]<<16;
			dwTemp += pTempPtr[2]<<8;
			dwTemp += 0;
			dwSum += dwTemp;		
		}
		else if(dwResverdLength==2)
		{
			DWORD dwTemp=0;
			dwTemp = pTempPtr[0]<<24;
			dwTemp += pTempPtr[1]<<16;
			dwTemp += 0;
			dwTemp += 0;
			dwSum += dwTemp;
		}
		else if(dwResverdLength==1)
		{
			DWORD dwTemp=0;
			dwTemp = pTempPtr[0]<<24;
			dwTemp += 0;
			dwTemp += 0;
			dwTemp += 0;
			dwSum += dwTemp;	
		}	
	}

	return dwSum;
 	
}
