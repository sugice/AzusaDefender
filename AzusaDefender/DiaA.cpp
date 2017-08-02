// DiaA.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaA.h"
#include "afxdialogex.h"
#include <Psapi.h>
#include "DiaVs.h"
#include "DiaPe.h"


// CDiaA 对话框

IMPLEMENT_DYNAMIC(CDiaA, CDialogEx)

CDiaA::CDiaA(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_boolCheck(FALSE)
	, m_boolCheck2(FALSE)
	, m_boolCheck3(FALSE)
	, m_isCheck(FALSE)
	, m_isCheck2(FALSE)
	, m_isCheck3(FALSE)
{

}

CDiaA::~CDiaA()
{
}

void CDiaA::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress2);
	DDX_Control(pDX, IDC_STATIC1, m_ctrlStatic);
	DDX_Control(pDX, IDC_STATIC2, m_ctrlStatic2);
	DDX_Check(pDX, IDC_CHECK1, m_boolCheck);
	DDX_Check(pDX, IDC_CHECK2, m_boolCheck2);
	DDX_Check(pDX, IDC_CHECK3, m_boolCheck3);
	DDX_Control(pDX, IDC_LIST1, m_ctrlListBox);
	DDX_Control(pDX, IDC_STATIC3, m_ctrlStatic3);
	DDX_Control(pDX, IDC_STATIC4, m_ctrlStatic4);
}


BEGIN_MESSAGE_MAP(CDiaA, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaA::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDiaA::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDiaA::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDiaA::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDiaA::OnBnClickedButton5)
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


//清理内存
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

//创建线程
void CDiaA::CreateMonitorThread()
{
	CreateThread(NULL, NULL, MonitorThread, (LPVOID)this, NULL, NULL);
}

//线程回调函数，用来实时监测CPU和内存状态
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

//弹出VS工程清理工具对话框
void CDiaA::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaVs* diaVs = new CDiaVs;
	diaVs->Create(IDD_DIALOG3, this);
	diaVs->ShowWindow(SW_SHOW);
}

//响应开始遍历垃圾按钮
void CDiaA::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_boolCheck && !m_isCheck)
	{
		m_isCheck = TRUE;
		SHQUERYRBINFO RecycleBininfo = {};
		RecycleBininfo.cbSize = sizeof(RecycleBininfo);
		SHQueryRecycleBin(NULL, &RecycleBininfo);
		CString cstrSize =ByteConversionGBMBKB(RecycleBininfo.i64Size);
		CString cstrNumItems;
		cstrNumItems.Format(_T("%d"), RecycleBininfo.i64NumItems);
		m_ctrlStatic3.SetWindowTextW(cstrSize);
		m_ctrlStatic4.SetWindowTextW(cstrNumItems);
		m_ctrlStatic3.ShowWindow(TRUE);
		m_ctrlStatic4.ShowWindow(TRUE);
	}
	if (m_boolCheck2 && !m_isCheck2)//系统垃圾路径
	{
		m_isCheck2 = TRUE;
		m_vecPath.push_back(_T("C:\\Windows\\Temp"));
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Temp"));
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Microsoft\\Windows\\WER\\ReportQueue"));
	}
	if (m_boolCheck3 && !m_isCheck3)//浏览器垃圾路径
	{
		m_isCheck3 = TRUE;
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache"));
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\GPUCache"));
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Google\\Chrome\\User Data\\ShaderCache\\GPUCache"));
		m_vecPath.push_back(_T("C:\\Users\\aimomo\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\JumpListIcons"));
	}
	//遍历之前先清空之前遍历的文件信息和列表框内容
	m_vecFile.clear();
	m_ctrlListBox.ResetContent();
	for (auto vecPathObj : m_vecPath)//遍历
	{
		TraverseFile(vecPathObj.GetBuffer(), m_vecFile);
	}
	int i = 0;
	for (auto vecFileObj : m_vecFile)//添加到listbox控件
	{
		m_ctrlListBox.AddString(vecFileObj);
		m_ctrlListBox.SelectString(i, vecFileObj);
		i++;
	}
}


//遍历要清除垃圾的文件夹
void CDiaA::TraverseFile(
	TCHAR  szPath[MAX_PATH], // 要遍历的文件路径
	vector<CString> &vecPath)// 保存遍历到的文件
{
	// 向文件夹末尾添加\\*，遍历所有文件
	CString strPath = szPath;
	strPath += L"\\*";
	WIN32_FIND_DATA fi = {};
	// 找到的文件信息放到fi里面
	HANDLE hFile = FindFirstFile(strPath, &fi);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		// 判断是否当前目录或上级目录
		if (!lstrcmp(fi.cFileName, L".") || !lstrcmp(fi.cFileName, L".."))
		{
			continue;
		}
		// 判断是否是文件夹，若是，递归遍历
		if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString strDIr = szPath;
			strDIr += L"\\";
			strDIr += fi.cFileName;
			TraverseFile(strDIr.GetBuffer(), vecPath);
			continue;
		}
		// 经过重重筛选，终于找到要删除的文件名了，和之前的路径构造成完整路径
		CString strDIr = szPath;
		strDIr += L"\\";
		strDIr += fi.cFileName;
		// 保存进路径向量里
		vecPath.push_back(strDIr);
	} while (FindNextFile(hFile, &fi));

}


//字节转KB、MB、GB
CString CDiaA::ByteConversionGBMBKB(__int64 KSize)
{
	const int GB = 1024 * 1024 * 1024;//定义GB的计算常量
	const int MB = 1024 * 1024;//定义MB的计算常量
	const int KB = 1024;//定义KB的计算常量
	CString strObj;
	if (KSize / GB >= 1)//如果当前Byte的值大于等于1GB
	{
		strObj.Format(_T("%0.1f"), round(KSize / (float)GB));
		return strObj + _T("GB");//将其转换成GB
	}
	else if (KSize / MB >= 1)//如果当前Byte的值大于等于1MB
	{
		strObj.Format(_T("%0.1f"), round(KSize / (float)MB));
		return strObj + _T("MB");//将其转换成MB
	}
	else if (KSize / KB >= 1)//如果当前Byte的值大于等于1KB
	{
		strObj.Format(_T("%0.1f"), round(KSize / (float)KB));
		return strObj + _T("KB");//将其转换成KB
	}
	else
	{
		strObj.Format(_T("%0.1d"), KSize);
		return strObj + _T("Byte");//显示Byte值
	}
}

//删除遍历到的文件
void CDiaA::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	for (DWORD i = 0; i < m_vecFile.size(); i++)
	{
		DeleteFile(m_vecFile[i].GetBuffer());
		m_ctrlListBox.DeleteString(0);
	}
	m_vecFile.swap(vector<CString>());
}

//弹出lordpe窗口
void CDiaA::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaPe* diaPe = new CDiaPe;
	diaPe->Create(IDD_DIALOG6, this);
	diaPe->ShowWindow(SW_SHOW);
}
