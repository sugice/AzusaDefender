#pragma once
#include "MyListBox.h"
#include <vector>
using std::vector;


// CDiaVs 对话框

class CDiaVs : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaVs)

public:
	CDiaVs(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaVs();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolder;
	vector<CString> m_vecFileDelete;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	void TraverseFile(TCHAR szPath[MAX_PATH], const CString &strFilter, vector<CString> &vecPath);
	afx_msg void OnBnClickedButton4();
	virtual BOOL OnInitDialog();
	CString m_strDelete;
	CMyListBox m_ctlListBoxFolder;
	CMyListBox m_ctrListBoxFile;
};
