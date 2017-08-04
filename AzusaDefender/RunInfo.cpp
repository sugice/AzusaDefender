// RunInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "RunInfo.h"
#include "afxdialogex.h"


// CRunInfo 对话框

IMPLEMENT_DYNAMIC(CRunInfo, CDialogEx)

CRunInfo::CRunInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG15, pParent)
{

}

CRunInfo::~CRunInfo()
{
}

void CRunInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}


BEGIN_MESSAGE_MAP(CRunInfo, CDialogEx)
END_MESSAGE_MAP()


// CRunInfo 消息处理程序


BOOL CRunInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化listCtrl
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[2] = { L"主键名", L"子键" };
	for (int i = 0; i < 2; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	m_vecRunChildInfo.clear();
	//遍历32位软件
	HKEY hKey[2] = { HKEY_CURRENT_USER , HKEY_LOCAL_MACHINE };
	WCHAR* subKey[8] = { L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
						L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",
						L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
						L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce",
						L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
						L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx",
						L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run",
						L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
	};

	for (int i=0;i<2;++i)
	{
		for (int j=0;j<8;++j)
		{
			HKEY hkResult = 0;
			GetRunInfo(hKey[i], subKey[j], hkResult);
		}
	}

	DWORD i = 0;
	CString temp;
	m_ctrlList.DeleteAllItems();
	for (auto each : m_vecRunChildInfo)
	{
		temp = L"HKEY_LOCAL_MACHINE";
		m_ctrlList.InsertItem(i, temp);
		temp = each.strMidReg;
		m_ctrlList.SetItemText(i, 1, temp);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



BOOL CRunInfo::GetRunInfo(HKEY& rootKey, LPCTSTR lpSubKey, HKEY& hkResult)
{
	//打开一个已经存在的注册表键
	LONG lReturn = RegOpenKeyEx(rootKey, lpSubKey, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkResult);
	//循环遍历Uninstall目录下的子键
	LONG lEnumReturn = 0;
	DWORD dwIndex = 0;
	RUNCHILDKEYINFO runChildKeyInfo = { 0 };
	while (lEnumReturn != ERROR_NO_MORE_ITEMS)
	{
		DWORD dwKeyLen = 255;
		WCHAR szNewKeyName[MAX_PATH] = { 0 };
		lEnumReturn = RegEnumKeyEx(hkResult, dwIndex, szNewKeyName, &dwKeyLen, 0, NULL, NULL, NULL);
		if (lEnumReturn != ERROR_SUCCESS)
		{
			return FALSE;
		}
		//通过得到的子键名称重新组合成新的子键路径
		WCHAR strMidReg[MAX_PATH] = { 0 };
		swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);
		//打开新的子健，获取其句柄
		HKEY hkValueKey = 0;//子健句柄
		LONG openKeyReturn = 0;
		openKeyReturn = RegOpenKeyEx(rootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);
		if (openKeyReturn != ERROR_SUCCESS)
		{
			return FALSE;
		}
		runChildKeyInfo.RootKey = rootKey;
		runChildKeyInfo.strMidReg = strMidReg;
		//枚举子健内的所有值
		LONG enumValueReturn = 0;
		DWORD dwIndex2 = 0;
		RUNINFO runInfo = { 0 };
		while (enumValueReturn != ERROR_NO_MORE_ITEMS)
		{
			DWORD dwValueNameLen = 255;
			DWORD dwValueLen = 0;
			enumValueReturn = RegEnumValue(hkValueKey, dwIndex2, runInfo.Name, &dwValueNameLen, 0, NULL, (LPBYTE)runInfo.Value, &dwValueLen);
			if (enumValueReturn != ERROR_SUCCESS)
			{
				++dwIndex2;
				continue;
			}
			runChildKeyInfo.vecRunInfo.push_back(runInfo);
			++dwIndex2;
		}
		m_vecRunChildInfo.push_back(runChildKeyInfo);
		++dwIndex;
	}
	return TRUE;
}