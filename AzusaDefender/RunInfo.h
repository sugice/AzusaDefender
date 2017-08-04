#pragma once
#include "afxcmn.h"
#include <vector>
using std::vector;

//启动项子键键值
typedef struct _RUNINFO
{
	WCHAR Name[50];//名称
	WCHAR Value[100];//值
}RUNINFO, *PRUNINFO;

//启动项子健信息
typedef struct _RUNCHILDKEYINFO
{
	HKEY RootKey;
	CString strMidReg;
	vector<RUNINFO> vecRunInfo;
}RUNCHILDKEYINFO, *PRUNCHILDKEYINFO;

// CRunInfo 对话框

class CRunInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CRunInfo)

public:
	CRunInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRunInfo();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG15 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL GetRunInfo(HKEY& rootKey, LPCTSTR lpSubKey, HKEY& hkResult);
	CListCtrl m_ctrlList;
	vector<RUNCHILDKEYINFO> m_vecRunChildInfo;
};
