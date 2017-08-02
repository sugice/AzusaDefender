#pragma once
#include <vector>
#include "afxcmn.h"
using std::vector;


// CDiaD 对话框

class CDiaD : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaD)

public:
	CDiaD(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaD();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void TraverseFile(TCHAR szPath[MAX_PATH], vector<CString> &vecPath);
	void CalculateMD5();
	void GetVirusInfo();
	CString ByteConversionGBMBKB(__int64 KSize);
	vector<CString> m_vecFile;
	vector<CStringA> m_vecMD5Save;
	vector<CString> m_vecVirus;
	vector<CStringA> m_vecVirusMD5;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CListCtrl m_ctrlList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
};
