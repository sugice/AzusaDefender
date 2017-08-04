// Uninstall.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "Uninstall.h"
#include "afxdialogex.h"


// CUninstall 对话框

IMPLEMENT_DYNAMIC(CUninstall, CDialogEx)

CUninstall::CUninstall(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG14, pParent)
{

}

CUninstall::~CUninstall()
{
}

void CUninstall::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}



BEGIN_MESSAGE_MAP(CUninstall, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CUninstall::OnNMDblclkList1)
END_MESSAGE_MAP()


BOOL CUninstall::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化listCtrl
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[2] = { L"软件名称", L"卸载路径"};
	for (int i = 0; i < 2; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	m_vecSoftInfo.clear();
	//遍历32位软件
	HKEY rootKey = HKEY_LOCAL_MACHINE;
	LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
	HKEY hkResult = 0;
	GetSoftInfo(rootKey, lpSubKey, hkResult);

	//遍历64位软件
	rootKey = HKEY_LOCAL_MACHINE;
	lpSubKey = L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
	hkResult = 0;
	GetSoftInfo(rootKey, lpSubKey, hkResult);

	DWORD i = 0;
	CString temp;
	m_ctrlList.DeleteAllItems();
	for (auto softInfo : m_vecSoftInfo)
	{
		temp = softInfo.Name;
		m_ctrlList.InsertItem(i, temp);
		temp = softInfo.UniPath;
		m_ctrlList.SetItemText(i, 1, temp);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CUninstall 消息处理程序


BOOL CUninstall::GetSoftInfo(HKEY& rootKey, LPCTSTR lpSubKey, HKEY& hkResult)
{
	//打开一个已经存在的注册表键
	LONG lReturn = RegOpenKeyEx(rootKey, lpSubKey, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkResult);
	//循环遍历Uninstall目录下的子键
	SOFTINFO softInfo = {0};
	LONG lEnumReturn = 0;
	DWORD dwIndex = 0;
	while (lEnumReturn != ERROR_NO_MORE_ITEMS)
	{
		DWORD dwKeyLen = 255;
		WCHAR szNewKeyName[MAX_PATH] = { 0 };
		lEnumReturn = RegEnumKeyEx(hkResult, dwIndex, szNewKeyName, &dwKeyLen, 0, NULL, NULL, NULL);
		if (lEnumReturn!= ERROR_SUCCESS)
		{
			return FALSE;
		}
		//通过得到的子键名称重新组合成新的子键路径
		WCHAR strMidReg[MAX_PATH] = {0};
		swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);
		//打开新的子健，获取其句柄
		HKEY hkValueKey = 0;//子健句柄
		LONG openKeyReturn = 0;
		openKeyReturn = RegOpenKeyEx(rootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);
		if (openKeyReturn != ERROR_SUCCESS)
		{
			return FALSE;
		}
		//获取键值
		DWORD dwNameLen = 255;//指向szBuffer的大小
		//获取软件名称
		DWORD dwType = 0;
		LONG queryValueReturn = 0;
		queryValueReturn = RegQueryValueEx(hkValueKey, L"DisplayName", 0, &dwType, (LPBYTE)softInfo.Name, &dwNameLen);
		if (queryValueReturn != ERROR_SUCCESS)
		{
			++dwIndex;
			continue;
		}
		dwNameLen = 255;//重新赋值，如果没有重新赋值，下一次将获取不到信息
		//拆卸路径
		LONG queryValueReturn2 = 0;
		queryValueReturn2 = RegQueryValueEx(hkValueKey, L"UninstallString", 0, &dwType, (LPBYTE)softInfo.UniPath, &dwNameLen);
		if (queryValueReturn != ERROR_SUCCESS)
		{
			++dwIndex;
			continue;
		}
		dwNameLen = 255;
		m_vecSoftInfo.push_back(softInfo);
		++dwIndex;
	}
	return TRUE;
}




void CUninstall::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;  // 这就是双击的item的序号
	//int nSubItem = pNMListView->iSubItem; // 这是对应的子项号
	CString sText = m_ctrlList.GetItemText(nItem, 1);
	LPCTSTR lpParameters = NULL;
	//执行卸载程序
	ShellExecute(NULL, NULL, sText, lpParameters, NULL, SW_SHOW);
	*pResult = 0;
}
