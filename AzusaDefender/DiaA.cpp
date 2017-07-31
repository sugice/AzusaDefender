// DiaA.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaA.h"
#include "afxdialogex.h"
#include <Psapi.h>


// CDiaA 对话框

IMPLEMENT_DYNAMIC(CDiaA, CDialogEx)

CDiaA::CDiaA(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDiaA::~CDiaA()
{
}

void CDiaA::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_STATIC1, m_ctrlStatic);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress2);
	DDX_Control(pDX, IDC_STATIC2, m_ctrlStatic2);
}


BEGIN_MESSAGE_MAP(CDiaA, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaA::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDiaA 消息处理程序


BOOL CDiaA::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化CPU使用状态进度条内容
	INT cpuUserate = GetCpuUserate();
	m_ctrlProgress.SetRange(0, 100);
	m_ctrlProgress.SetPos(cpuUserate);
	CString strStatic;
	strStatic.Format(_T("%d"), cpuUserate);
	m_ctrlStatic.SetWindowTextW(strStatic);

	//初始化内存使用状态进度条内容
	DWORD memUserate = GetMemUserate();
	m_ctrlProgress2.SetRange(0, 100);
	m_ctrlProgress2.SetPos(memUserate);
	CString strStatic2;
	strStatic2.Format(_T("%d"), memUserate);
	m_ctrlStatic2.SetWindowTextW(strStatic2);
	//创建线程来实习检测CPU和内存的使用状态
	CreateMonitorThread();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//************************************
// Method:    GetCpuUserate
// FullName:  CDiaA::GetCpuUserate
// Access:    public 
// Returns:   BOOL
// Qualifier: 获取CPU占用率
//************************************
INT CDiaA::GetCpuUserate()
{
	//第一次获取处理器时间
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	//创建内核对象，并且等待1000毫秒
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	WaitForSingleObject(hEvent, 1000);
	//再次获取处理器时间
	FILETIME preidleTime, prekernelTime, preuserTime;
	GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
	//转换所有时间变量
	double dbIdleTime, dbKernelTime, dbUserTime, dbPreidleTime, dbPrekernelTime, dbPreuserTime;

	dbIdleTime = (double)(idleTime.dwHighDateTime*4.294967296E9) + (double)idleTime.dwLowDateTime;
	dbKernelTime = (double)(kernelTime.dwHighDateTime*4.294967296E9) + (double)kernelTime.dwLowDateTime;
	dbUserTime = (double)(userTime.dwHighDateTime*4.294967296E9) + (double)userTime.dwLowDateTime;

	dbPreidleTime = (double)(preidleTime.dwHighDateTime*4.294967296E9) + (double)preidleTime.dwLowDateTime;
	dbPrekernelTime = (double)(prekernelTime.dwHighDateTime*4.294967296E9) + (double)prekernelTime.dwLowDateTime;
	dbPreuserTime = (double)(preuserTime.dwHighDateTime*4.294967296E9) + (double)preuserTime.dwLowDateTime;
	
	INT cpuUserate = (INT)(100.0 - (dbPreidleTime - dbIdleTime) / (dbPrekernelTime - dbKernelTime + dbPreuserTime - dbUserTime)*100.0);
	return cpuUserate;
}

//************************************
// Method:    GetMemUserate
// FullName:  CDiaA::GetMemUserate
// Access:    public 
// Returns:   BOOL
// Qualifier: 获取内存使用率
//************************************
DWORD CDiaA::GetMemUserate()
{
	MEMORYSTATUSEX stcMemStatusEx = { 0 };
	stcMemStatusEx.dwLength = sizeof(stcMemStatusEx);
	GlobalMemoryStatusEx(&stcMemStatusEx);
	//DWORDLONG preUsedMem = stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;
	DWORD memUserate = stcMemStatusEx.dwMemoryLoad;
	return memUserate;
}

//************************************
// Method:    ClearUpMem
// FullName:  CDiaA::ClearUpMem
// Access:    public 
// Returns:   BOOL
// Qualifier: 清理内存
//************************************
BOOL CDiaA::ClearUpMem()
{
	DWORD dwPIDList[1000];
	DWORD bufSize = sizeof(dwPIDList);
	DWORD dwNeedSize = 0;
	EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
	for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);
		SetProcessWorkingSetSize(hProcess, -1, -1);
	}
	return TRUE;
}




void CDiaA::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	ClearUpMem();
	//重置内存使用状态进度条内容
	DWORD memUserate = GetMemUserate();
	m_ctrlProgress2.SetPos(memUserate);
	CString strStatic2;
	strStatic2.Format(_T("%d"), memUserate);
	m_ctrlStatic2.SetWindowTextW(strStatic2);
}

void CDiaA::CreateMonitorThread()
{
	CreateThread(NULL, NULL, MonitorThread, (LPVOID)this, NULL, NULL);
}

DWORD WINAPI CDiaA::MonitorThread(LPVOID lpParam)
{
	CDiaA *pThis = (CDiaA*)lpParam;
	CString strStatic,strStatic2;
	INT cpuUserate;
	DWORD memUserate;
	while (true)
	{
		//刷新CPU使用状态进度条
		cpuUserate = pThis->GetCpuUserate();
		pThis->m_ctrlProgress.SetPos(cpuUserate);
		strStatic.Format(_T("%d"), cpuUserate);
		pThis->m_ctrlStatic.SetWindowTextW(strStatic);

		//刷新内存使用状态进度条
		memUserate = pThis->GetMemUserate();
		pThis->m_ctrlProgress2.SetPos(memUserate);
		strStatic2.Format(_T("%d"), memUserate);
		pThis->m_ctrlStatic2.SetWindowTextW(strStatic2);
		Sleep(1000);
	}
}
