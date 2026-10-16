// TestBaseItem.h: interface for the CTestBaseItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTBASEITEM_H__BCD45D4D_81CB_4228_B570_EBCD857CE58E__INCLUDED_)
#define AFX_TESTBASEITEM_H__BCD45D4D_81CB_4228_B570_EBCD857CE58E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestBaseItem  
{
public:
	CTestBaseItem();
	virtual ~CTestBaseItem();

public:
	CString			m_strLayerName;
	CString			m_strFromPortName;
	CString			m_strFromMPOConnectorIndex;
	CString			m_strFromMPOPosition;
	CString			m_strToPortName;
	CString			m_strToMPOConnectorIndex;
	CString			m_strToMPOPosition;
/////////////////////  测试ITEM 的序号 和 TP的测试序号对应////////////////
	int				m_nTestItemIndex;
//////////////////// 开关对应位置在归零序列中的位置
	int				m_nFromInReferenceItemIndex;
	int				m_nToInReferenceItemIndex;

	double			m_dbTestILValue;
	double			m_dbMaxValue; // 上门限
	double			m_dbMinValue; // 下门限

	BOOL			m_bTestFlag; // 是否测试，如未测试则不写入测试数据文件

};

#endif // !defined(AFX_TESTBASEITEM_H__BCD45D4D_81CB_4228_B570_EBCD857CE58E__INCLUDED_)
