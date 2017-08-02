#pragma once
#include "afxcmn.h"


// CExportTable 对话框

class CExportTable : public CDialogEx
{
	DECLARE_DYNAMIC(CExportTable)

public:
	CExportTable(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExportTable();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG7 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strEdit;
	CString m_strEdit2;
	CString m_strEdit3;
	CString m_strEdit4;
	CString m_strEdit5;
	CString m_strEdit6;
	CString m_strEdit7;
	CListCtrl m_ctrlList;
	virtual BOOL OnInitDialog();
};
