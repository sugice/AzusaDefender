#pragma once
#include "afxcmn.h"


// CDelayLoTable 对话框

class CDelayLoTable : public CDialogEx
{
	DECLARE_DYNAMIC(CDelayLoTable)

public:
	CDelayLoTable(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDelayLoTable();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG11 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlList;
	CListCtrl m_ctrlList2;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
