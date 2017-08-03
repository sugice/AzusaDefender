#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDiaPe 对话框

class CDiaPe : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaPe)

public:
	CDiaPe(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaPe();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	void UpdateDataToEdits();
public:
	CString m_strEdit;
	static CString m_strFilePath;
	//////////////////
	CString m_strEdit2;
	CString m_strEdit3;
	CString m_strEdit4;
	CString m_strEdit5;
	CString m_strEdit6;
	CString m_strEdit7;
	CString m_strEdit8;
	CString m_strEdit9;
	CString m_strEdit10;
	CString m_strEdit11;
	CString m_strEdit12;
	CString m_strEdit13;
	CString m_strEdit14;
	CString m_strEdit15;
	CString m_strEdit16;
	///////////////////
	CListCtrl m_ctrlList;
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlList2;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
