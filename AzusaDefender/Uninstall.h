#pragma once
#include <vector>
#include "afxcmn.h"
using std::vector;

// CUninstall 对话框

typedef struct _SOFTINFO
{
	WCHAR Name[50];//名称
	WCHAR UniPath[MAX_PATH];//卸载路径
}SOFTINFO,*PSOFTINFO;




class CUninstall : public CDialogEx
{
	DECLARE_DYNAMIC(CUninstall)

public:
	CUninstall(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUninstall();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG14 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	vector<SOFTINFO> m_vecSoftInfo;
	BOOL GetSoftInfo(HKEY& rootKey,LPCTSTR lpSubKey,HKEY& hkResult);
	BOOL GetRunInfo(HKEY& rootKey, LPCTSTR lpSubKey, HKEY& hkResult);
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlList;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

};
