// DiaB.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaB.h"
#include "afxdialogex.h"
#include <Psapi.h>


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
	InsertProcessList();
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
BOOL CDiaB::InsertProcessList()
{
	m_ctrlList.DeleteAllItems();
	int i = 0;//索引用
	CString temp;
	// L"进程名称",L"PID",L"父进程PID",L"线程数量",L"优先级",L"进程所在路径"
	for (MY_PROCESSINFO &stc : m_vecProcess) {
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