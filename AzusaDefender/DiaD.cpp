// DiaD.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaD.h"
#include "MD5.h"
#include "afxdialogex.h"
#include<iostream>  
#include<fstream>
#include <string>
#include <TlHelp32.h>
using namespace std;


// 宽字符转换为多字符(Unicode --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
    WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);


// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
    MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));
// CDiaD 对话框

IMPLEMENT_DYNAMIC(CDiaD, CDialogEx)

CDiaD::CDiaD(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG5, pParent)
{

}

CDiaD::~CDiaD()
{
}

void CDiaD::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}


BEGIN_MESSAGE_MAP(CDiaD, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaD::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDiaD::OnBnClickedButton2)
END_MESSAGE_MAP()


BOOL CDiaD::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化list信息
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	TCHAR *szCol[5] = { L"文件路径", L"创建时间", L"修改时间", L"文件大小", L"MD5值"};
	for (int i = 0; i < 5; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CDiaD 消息处理程序
//查毒
void CDiaD::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	TraverseFile(L"D://Test", m_vecFile);
	CalculateMD5();
	GetVirusInfo();
}

//杀毒
void CDiaD::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_ctrlList.GetItemCount(); i++)
	{
		if (m_ctrlList.GetCheck(i)) //这个是前面的checkbox被选中
		{
			CString deFile = m_ctrlList.GetItemText(i, 0);
			DeleteFile(deFile.GetBuffer());
			m_ctrlList.DeleteItem(i);
		}
	}
}

// 逻辑代码部分
//遍历测试文件夹
void CDiaD::TraverseFile(
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

//将遍历到的文件进行MD5比对
void CDiaD::CalculateMD5()
{
	CMD5 md5;
	CHAR strFilePath[MAX_PATH] = { 0 };
	CStringA cstrMD5;
	//读取保存着MD5值的文件信息
	string buf;
	CStringA strObj;
	ifstream infile;
	infile.open(_T("MD5.txt"));
	if (!infile)
	{
		return;
	}
	//逐行读取直到读取完毕
	while (getline(infile, buf))
	{
		strObj.Format("%s", buf.c_str());
		m_vecMD5Save.push_back(strObj);
	}
	infile.close();
	for (auto vecFile : m_vecFile)
	{
		WCHAR_TO_CHAR(vecFile.GetBuffer(), strFilePath);
		//计算文件MD5值
		cstrMD5 = md5.md5FileValue(strFilePath);
		//和保存的MD5值进行比对
		for (auto vecMD5Save : m_vecMD5Save)
		{
			if (vecMD5Save == cstrMD5)//MD5值相等，是病毒
			{
				m_vecVirus.push_back(vecFile);//将病毒文件保存起来
				m_vecVirusMD5.push_back(cstrMD5);//将对应的病毒MD5值保存起来
			}
		}
	}
}

//获取病毒信息
void CDiaD::GetVirusInfo()
{
	m_ctrlList.DeleteAllItems();
	DWORD i = 0;//索引
	for (auto vecVirusFile : m_vecVirus)
	{
		WIN32_FIND_DATA stcData = { 0 };
		HANDLE hFind = FindFirstFile(vecVirusFile, &stcData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}
		SYSTEMTIME syCreationTime = { 0 };
		SYSTEMTIME syWriteTime = { 0 };
		FileTimeToSystemTime(&(stcData.ftCreationTime), &syCreationTime);
		FileTimeToSystemTime(&(stcData.ftLastWriteTime), &syWriteTime);
		__int64 fileSizeByte = (stcData.nFileSizeHigh *(MAXDWORD + 1)) + stcData.nFileSizeLow;
		CString strCreationTime,strWriteTime;
		//文件创建时间
		strCreationTime.Format(L"%d/%d/%d %d:%d", syCreationTime.wYear, syCreationTime.wMonth, 
								syCreationTime.wDay, syCreationTime.wHour, syCreationTime.wMinute);
		//文件修改时间
		strWriteTime.Format(L"%d/%d/%d %d:%d", syWriteTime.wYear, syWriteTime.wMonth,
			syWriteTime.wDay, syWriteTime.wHour, syWriteTime.wMinute);
		//文件大小
		CString strFileSize = ByteConversionGBMBKB(fileSizeByte);
		//L"文件路径", L"创建时间", L"修改时间", L"文件大小", L"MD5值"
		m_ctrlList.InsertItem(i, vecVirusFile);
		m_ctrlList.SetItemText(i, 1, strCreationTime);
		m_ctrlList.SetItemText(i, 2, strWriteTime);
		m_ctrlList.SetItemText(i, 3, strFileSize);
		WCHAR virusMD5[33] = {0};
		CHAR_TO_WCHAR(m_vecVirusMD5[i].GetBuffer(),virusMD5);
		m_ctrlList.SetItemText(i, 4, virusMD5);

	}
}

//字节转KB、MB、GB
CString CDiaD::ByteConversionGBMBKB(__int64 KSize)
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


//BOOL CDiaD::GetEvilProcess()
//{
//	// 清空
//	m_vecProcess.clear();
//	// 遍历
//	HANDLE hProcessSnap;//进程快照句柄
//	HANDLE hProcess;//进程句柄
//	PROCESSENTRY32 stcPe32 = { 0 };//进程快照信息
//	stcPe32.dwSize = sizeof(PROCESSENTRY32);
//	//1.创建一个进程相关的快照句柄
//	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hProcessSnap == INVALID_HANDLE_VALUE) return FALSE;
//	//2通过进程快照句柄获取第一个进程信息
//	if (!Process32First(hProcessSnap, &stcPe32)) {
//		CloseHandle(hProcessSnap);
//		return FALSE;
//	}
//	//3循环遍历进程信息
//	do {
//		//3.1获取进程名
//		wcscpy_s(stcInfo.szProcess, MAX_PATH, stcPe32.szExeFile);
//
//	} while (Process32Next(hProcessSnap, &stcPe32));
//	CloseHandle(hProcessSnap);
//	return TRUE;
//}
//
//// [Added by thinkhy 09/12/20]  
//// Description: Kill process(es) by PID.  
//// Reference:   http://www.vckbase.com/document/viewdoc/?id=1882  
//// RETVALUE:    SUCCESS   TRUE  
////              FAILED    FALSE  
//BOOL CDiaD::KillProcess(DWORD dwPid)
//{
//	HANDLE hPrc;
//
//	if (0 == dwPid) return FALSE;
//
//	hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);  // Opens handle to the process.  
//
//	if (!TerminateProcess(hPrc, 0)) // Terminates a process.  
//	{
//		CloseHandle(hPrc);
//		return FALSE;
//	}
//	else
//		WaitForSingleObject(hPrc, DELAYTIME); // At most ,waite 2000  millisecond.  
//
//	CloseHandle(hPrc);
//	return TRUE;
//}
//
//
//// [Added by thinkhy 09/12/20]  
//// Description: Kill process(es) by Name.  
//// Reference:   http://bbs.51testing.com/thread-65884-1-1.html  
//// RETVALUE:    SUCCESS   TRUE  
////              FAILED    FALSE  
//BOOL CDiaD::KillProcessByName(const TCHAR *lpszProcessName) {
//	unsigned int   pid = -1;
//	BOOL    retval = TRUE;
//
//	if (lpszProcessName == NULL)
//		return -1;
//
//	DWORD dwRet = 0;
//	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	PROCESSENTRY32 processInfo;
//	processInfo.dwSize = sizeof(PROCESSENTRY32);
//	int flag = Process32First(hSnapshot, &processInfo);
//
//	// Find the process with name as same as lpszProcessName  
//	while (flag != 0)
//	{
//		if (_tcscmp(processInfo.szExeFile, lpszProcessName) == 0) {
//			// Terminate the process.  
//			pid = processInfo.th32ProcessID;
//			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
//
//			if (TerminateProcess(hProcess, 0) != TRUE) { // Failed to terminate it.  
//				retval = FALSE;
//				break;
//			}
//		}
//
//		flag = Process32Next(hSnapshot, &processInfo);
//	} // while (flag != 0)  
//
//	CloseHandle(hSnapshot);
//
//	if (pid == -1)
//		return FALSE;
//
//	return retval;
//}