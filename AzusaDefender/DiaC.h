#pragma once
#include <vector>
#include "afxcmn.h"
using std::vector;

// CDiaC 对话框
typedef struct _SERVICEINFO
{
	CString cstrSerName;
	CString cstrSerType;
	CString cstrSerState;
	CString cstrStartType;
	CString cstrBinaryPathName;
}SERVICEINFO, *PSERVICEINFO;

class CDiaC : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaC)

public:
	CDiaC(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaC();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL enumService();
	BOOL InsertServiceList();
	vector<SERVICEINFO> m_vecServiceInfo;
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlList;
	afx_msg void OnBnClickedButton1();
};
