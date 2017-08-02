// DiaB.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaB.h"
#include "afxdialogex.h"
#include <Psapi.h>
#include<iostream>  
#include<fstream>
#include <string>
using namespace std;

// 宽字符转换为多字符(Unicode --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
    WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);


// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
    MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

// CDiaB 对话框

IMPLEMENT_DYNAMIC(CDiaB, CDialogEx)

CDiaB::CDiaB(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CDiaB::~CDiaB()
{
}

void CDiaB::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Control(pDX, IDC_LIST2, m_ctrlList2);
	DDX_Control(pDX, IDC_LIST3, m_ctrlList3);
	DDX_Control(pDX, IDC_LIST4, m_ctrlList4);
}


BEGIN_MESSAGE_MAP(CDiaB, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDiaB::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaB::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CDiaB::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化list信息
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[6] = { L"进程名称", L"PID", L"父进程PID", L"线程数量", L"优先级", L"进程所在路径" };
	for (int i = 0; i < 6; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	// 初始化list2信息
	m_ctrlList2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	TCHAR* szCol2[3] = { L"线程ID",L"进程ID",L"优先级" };
	for (int i = 0; i < 3; ++i)
	{
		m_ctrlList2.InsertColumn(i, szCol2[i], 0, 80);
	}
	// 初始化list3信息
	m_ctrlList3.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol3[4] = { L"模块名",L"基址",L"模块大小",L"路径" };
	for (int i = 0; i < 4; ++i)
	{
		m_ctrlList3.InsertColumn(i, szCol3[i], 0, 80);
	}
	// 初始化list4信息
	m_ctrlList4.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ctrlList4.InsertColumn(0, L"窗口名", 0, 80);
	// 遍历线程信息
	EnumProcess();
	InsertProcessList(m_vecProcess);
	//遍历窗口信息
	EnumWindow();
	InsertWindowInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



// CDiaB 消息处理程序

//************************************
// Method:    EnumProcess
// FullName:  CDiaB::EnumProcess
// Access:    protected 
// Returns:   BOOL
// Qualifier: 遍历进程及信息并保存到vector中
//************************************
BOOL CDiaB::EnumProcess()
{
	// 清空
	m_vecProcess.clear();
	// 遍历
	HANDLE hProcessSnap;//进程快照句柄
	HANDLE hProcess;//进程句柄
	PROCESSENTRY32 stcPe32 = { 0 };//进程快照信息
	stcPe32.dwSize = sizeof(PROCESSENTRY32);
	//1.创建一个进程相关的快照句柄
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) return FALSE;
	//2通过进程快照句柄获取第一个进程信息
	if (!Process32First(hProcessSnap, &stcPe32)) {
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	//3循环遍历进程信息
	do {
		MY_PROCESSINFO stcInfo = { 0 };
		//3.1获取进程名
		wcscpy_s(stcInfo.szProcess, MAX_PATH, stcPe32.szExeFile);
		//3.2获取完整路径及优先级信息
		//hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, NULL, stcPe32.th32ProcessID);
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, NULL, stcPe32.th32ProcessID);
		if (hProcess) {
			// 获取进程完整路径
			DWORD dwSize = MAX_PATH;
			QueryFullProcessImageName(hProcess, 0, stcInfo.szFullProcess, &dwSize);
			//GetProcessImageFileName(hProcess, stcInfo.szFullProcess, MAX_PATH);
			stcInfo.dwPriorityClass = GetPriorityClass(hProcess);//获取进程优先级
			CloseHandle(hProcess);
		}
		//3.3获取进程的其他相关信息
		stcInfo.dwPid = stcPe32.th32ProcessID;
		stcInfo.dwThreadCount = stcPe32.cntThreads;
		stcInfo.dwParentProcess = stcPe32.th32ParentProcessID;
		//3.4将获取到的相关信息保存到向量中
		m_vecProcess.push_back(stcInfo);
	} while (Process32Next(hProcessSnap, &stcPe32));
	CloseHandle(hProcessSnap);
	return TRUE;
}

//************************************
// Method:    InsertProcessList
// FullName:  CDiaB::InsertProcessList
// Access:    protected 
// Returns:   BOOL
// Qualifier: 将数据插入到列表控件中
//************************************
BOOL CDiaB::InsertProcessList(vector<MY_PROCESSINFO>& vecProcess)
{
	m_ctrlList.DeleteAllItems();
	int i = 0;//索引用
	CString temp;
	// L"进程名称",L"PID",L"父进程PID",L"线程数量",L"优先级",L"进程所在路径"
	for (MY_PROCESSINFO &stc : vecProcess) {
		// 第1列,进程名称
		m_ctrlList.InsertItem(i, stc.szProcess);
		// 第2列,PID
		temp.Format(L"%d", stc.dwPid);
		m_ctrlList.SetItemText(i, 1, temp);
		// 第3列,父进程PID
		temp.Format(L"%d", stc.dwParentProcess);
		m_ctrlList.SetItemText(i, 2, temp);
		// 第4列,线程数量
		temp.Format(L"%d", stc.dwThreadCount);
		m_ctrlList.SetItemText(i, 3, temp);
		// 第5列,优先级
		temp.Format(L"%d", stc.dwPriorityClass);
		m_ctrlList.SetItemText(i, 4, temp);
		// 第6列,进程所在路径
		m_ctrlList.SetItemText(i, 5, stc.szFullProcess);
		++i;
	}
	return TRUE;
}

// CDlgThread 消息处理程序
// 遍历线程
BOOL CDiaB::EnumThread(DWORD dwPid)
{
	m_vecThread.clear();
	//1.创建线程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnap == INVALID_HANDLE_VALUE) return 0;
	//2通过线程快照句柄获取第一个线程信息
	THREADENTRY32 stcThread = { sizeof(THREADENTRY32) };
	if (!Thread32First(hSnap, &stcThread)) {
		CloseHandle(hSnap);
		return 0;
	}
	//3循环遍历线程信息
	do {
		if (stcThread.th32OwnerProcessID != dwPid) {
			continue;
		}
		m_vecThread.push_back(stcThread);
	} while (Thread32Next(hSnap, &stcThread));
	CloseHandle(hSnap);
	return 0;
}

// 插入线程信息
BOOL CDiaB::InsertThreadInfo()
{
	//List控件处理
	CString str;
	int i = 0;
	for (THREADENTRY32 &thread : m_vecThread) {
		//线程ID
		str.Format(L"%d", thread.th32ThreadID);
		m_ctrlList2.InsertItem(i, str);
		//进程ID
		str.Format(L"%d", thread.th32OwnerProcessID);
		m_ctrlList2.SetItemText(i, 1, str);
		//优先级
		str.Format(L"%d", thread.tpBasePri);
		m_ctrlList2.SetItemText(i, 2, str);
	}
	return TRUE;
}


void CDiaB::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlList2.DeleteAllItems();
	m_ctrlList3.DeleteAllItems();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;  // 这就是双击的item的序号
	//int nSubItem = pNMListView->iSubItem; // 这是对应的子项号
	CString sText = m_ctrlList.GetItemText(nItem,1);
	DWORD dwPId = _ttoi(sText);
	EnumThread(dwPId);
	InsertThreadInfo();
	EnumModule(dwPId);
	InsertModuleInfo();
	*pResult = 0;
}

// 遍历模块信息
BOOL CDiaB::EnumModule(DWORD dwPid)
{
	m_vecModule.clear();
	//1.1打开进程
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (NULL == hProcess) {
		return FALSE;
	}
	//1.2确定保存模块句柄的数组大小
	HMODULE hModules[0x1] = { 0 };//模块数组
	DWORD cbNeed = 0;//实际获取的大小
	EnumProcessModulesEx(/*枚举进程模块*/
		hProcess,//进程句柄
		hModules,//模块句柄数组
		sizeof(hModules),//模块句柄数组的大小
		&cbNeed,//实际需要的数组大小
		LIST_MODULES_ALL);//枚举模块的类型
						  //1.3获取模块句柄表
	DWORD dwModCount = cbNeed / sizeof(HMODULE);//模块数量
	HMODULE* pModBuf = new HMODULE[dwModCount];//保存模块句柄的缓存
	EnumProcessModulesEx(/*枚举进程模块*/
		hProcess,//进程句柄
		pModBuf,//模块句柄数组
		dwModCount * sizeof(HMODULE),//模块句柄数组的大小
		&cbNeed,//实际需要的数组大小
		LIST_MODULES_ALL);//枚举模块的类型
						  //2.枚举进程模块信息
						  //char szModuleName[0x200] = {0};
	MODULEENTRY32 stcMod32 = { sizeof(MODULEENTRY32) };
	MODULEINFO stcModInfo;
	//3.分配vec空间
	for (UINT i = 0; i < dwModCount; i++) {
		GetModuleBaseName(hProcess, pModBuf[i], stcMod32.szModule, MAX_PATH);
		GetModuleFileNameEx(hProcess, pModBuf[i], stcMod32.szExePath, MAX_PATH);
		GetModuleInformation(hProcess, pModBuf[i], &stcModInfo, sizeof(MODULEINFO));
		stcMod32.hModule = pModBuf[i];
		stcMod32.modBaseAddr = (PBYTE)stcModInfo.lpBaseOfDll;
		stcMod32.modBaseSize = stcModInfo.SizeOfImage;
		stcMod32.th32ProcessID = dwPid;
		m_vecModule.push_back(stcMod32);
	}
	CloseHandle(hProcess);
	delete[] pModBuf;
	return TRUE;
}

// 插入模块信息
BOOL CDiaB::InsertModuleInfo()
{
	//List控件处理
	CString str;
	int i = 0;
	for (MODULEENTRY32 &module : m_vecModule) {
		//模块名
		m_ctrlList3.InsertItem(i, module.szModule);
		//基址
		str.Format(L"%08X", module.modBaseAddr);
		m_ctrlList3.SetItemText(i, 1, str);
		//模块大小
		str.Format(L"%d", module.modBaseSize);
		m_ctrlList3.SetItemText(i, 2, str);
		//所在路径
		m_ctrlList3.SetItemText(i, 3, module.szExePath);
		i++;
	}
	return TRUE;
}

BOOL CDiaB::EnumWindow()
{
	HWND hWnd = ::GetDesktopWindow();
	hWnd = ::GetWindow(hWnd, GW_CHILD);
	WCHAR szName[266] = { 0 };
	CString nameObj;
	while (hWnd!=NULL)
	{
		::GetWindowText(hWnd, szName, 266);
		nameObj = szName;
		m_vecStrName.push_back(nameObj);
		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return TRUE;
}

// 插入窗口信息
BOOL CDiaB::InsertWindowInfo()
{
	m_ctrlList4.DeleteAllItems();
	//List控件处理
	int i = 0;
	for (CString & strName : m_vecStrName) {
		//窗口名
		if (!strName.IsEmpty())
		{
			m_ctrlList4.InsertItem(i, strName);
		}
		i++;
	}
	return TRUE;
}

void CDiaB::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	GetEvilProcessInfo();
	InsertProcessList(m_vecEvilProcess);
}

//将黑白名单的信息读取到响应vector中
void CDiaB::ReadWBList()
{
	//读取保存着白名单的文件信息
	string buf;
	CStringA strObj;
	CString wstrObj;
	WCHAR wstrBuf[100] = { 0 };
	ifstream infile;
	infile.open(_T("whitelist.txt"));
	if (!infile)
	{
		return;
	}
	//逐行读取直到读取完毕
	while (getline(infile, buf))
	{
		strObj.Format("%s", buf.c_str());
		CHAR_TO_WCHAR(strObj.GetBuffer(), wstrBuf);
		wstrObj = wstrBuf;
		m_vecWhitelist.push_back(wstrObj);
	}
	infile.close();
	//读取保存着黑名单的文件信息
	string buf2;
	CStringA strObj2;
	CString wstrObj2;
	WCHAR wstrBuf2[100] = { 0 };
	ifstream infile2;
	infile2.open(_T("blacklist.txt"));
	if (!infile2)
	{
		return;
	}
	//逐行读取直到读取完毕
	while (getline(infile2, buf2))
	{
		strObj2.Format("%s", buf2.c_str());
		CHAR_TO_WCHAR(strObj2.GetBuffer(), wstrBuf2);
		wstrObj2 = wstrBuf2;
		m_vecBlacklist.push_back(wstrObj2);
	}
	infile2.close();
}

//寻找恶意进程
void CDiaB::GetEvilProcessInfo()
{
	//读黑白名单信息
	ReadWBList();
	//比对进程名
	CString strProName;
	for (auto vecProcess : m_vecProcess)//所有进程信息
	{
		MY_PROCESSINFO stcInfo = { 0 };
		strProName = vecProcess.szProcess;
		for (auto vecBlacklist : m_vecBlacklist)//黑名单进程名
		{
			if (vecBlacklist == strProName)
			{
				BOOL isNotInWL = TRUE;
				//看看在不在白名单中
				for (auto vecWhitelist : m_vecWhitelist)//白名单进程名
				{
					if (vecWhitelist == strProName)
					{
						isNotInWL = FALSE;
						break;
					}
				}
				//不在白名单中
				if (isNotInWL)
				{
					//将信息转存
					stcInfo.dwParentProcess = vecProcess.dwParentProcess;
					stcInfo.dwPid = vecProcess.dwPid;
					stcInfo.dwPriorityClass = vecProcess.dwPriorityClass;
					stcInfo.dwThreadCount = vecProcess.dwThreadCount;
					wcscpy_s(stcInfo.szFullProcess, MAX_PATH, vecProcess.szFullProcess);
					wcscpy_s(stcInfo.szProcess, MAX_PATH, vecProcess.szProcess);
					m_vecEvilProcess.push_back(stcInfo);
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
}

