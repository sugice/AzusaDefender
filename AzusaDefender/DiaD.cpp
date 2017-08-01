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
END_MESSAGE_MAP()


BOOL CDiaD::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化list信息
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[5] = { L"文件路径", L"创建时间", L"修改时间", L"文件大小", L"MD5值"};
	for (int i = 0; i < 5; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CDiaD 消息处理程序
void CDiaD::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	TraverseFile(L"D://Test", m_vecFile);
	CalculateMD5();
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
	//打开保存着MD5的文件
	//HANDLE hFile = CreateFile(L"MD5.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//CHAR ch[100] = {};
	//DWORD dwReads;
	//ReadFile(hFile, ch, 100, &dwReads, NULL);
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

void CDiaD::GetVirusInfo()
{
	WIN32_FIND_DATA stcData = { 0 };
	for (auto vecVirusFile : m_vecVirus)
	{
		HANDLE hFind = FindFirstFile(vecVirusFile, &stcData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}

	}
}


