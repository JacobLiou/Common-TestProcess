// TestBaseItem.cpp: implementation of the CTestBaseItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestBaseItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestBaseItem::CTestBaseItem()
{
	m_nTestItemIndex = 0;
	m_nFromInReferenceItemIndex = 0;
	m_nToInReferenceItemIndex = 0;
	m_dbTestILValue = 0;
	m_dbMaxValue = 0;
	m_dbMinValue = 0;
	m_bTestFlag = FALSE;
}

CTestBaseItem::~CTestBaseItem()
{

}
