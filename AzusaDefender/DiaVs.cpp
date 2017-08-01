// DiaVs.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaVs.h"
#include "afxdialogex.h"


// CDiaVs 对话框

IMPLEMENT_DYNAMIC(CDiaVs, CDialogEx)

CDiaVs::CDiaVs(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, m_strFolder(_T(""))
	, m_strDelete(_T(".obj.tlog.lastbuildstate.idb.pdb.pch.res.ilk.exe.sdf.ipch.log"))
{

}

CDiaVs::~CDiaVs()
{
}

void CDiaVs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFolder);
	DDX_Text(pDX, IDC_EDIT2, m_strDelete);
	DDX_Control(pDX, IDC_LIST4, m_ctlListBoxFolder);
	DDX_Control(pDX, IDC_LIST5, m_ctrListBoxFile);
}


BEGIN_MESSAGE_MAP(CDiaVs, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaVs::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDiaVs::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDiaVs::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDiaVs::OnBnClickedButton4)
END_MESSAGE_MAP()


BOOL CDiaVs::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
// CDiaVs 消息处理程序


void CDiaVs::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szPath1[MAX_PATH] = {};
	BROWSEINFO bi = {};
	// 文件夹选择对话框所属窗口句柄
	bi.hwndOwner = m_hWnd;
	bi.pszDisplayName = szPath1;
	// 窗口说明信息
	bi.lpszTitle = L"打开要遍历的文件夹";
	// 对话框带编辑框
	bi.ulFlags = BIF_EDITBOX;
	LPITEMIDLIST pid;
	// 返回标识文件夹路径的“PIDL”，用它找到文件夹路径
	pid = SHBrowseForFolder(&bi);
	SHGetPathFromIDList(pid, szPath1);
	// 把路径更新到路径显示编辑框内
	m_strFolder = szPath1;
	// 把路径更新到路径显示ListBox内
	if (LB_ERR == m_ctlListBoxFolder.FindString(0, m_strFolder))
	{// 如果m_ListBox里面没有这个新的文件夹路径，再添加进m_ListBox
		m_ctlListBoxFolder.AddString(m_strFolder);
	}
	// 把变量内容更新到对话框
	UpdateData(FALSE);
	// 用完释放掉之前的“PID”
	CoTaskMemFree(pid);
}


void CDiaVs::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


void CDiaVs::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	// 遍历之前先清空之前遍历的文件信息和列表框内容
	m_vecFileDelete.clear();
	m_ctrListBoxFile.ResetContent();
	// 获取要遍历的文件夹数目
	int nCount = m_ctlListBoxFolder.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CString test;
		// 获取ListBox控件内的文件夹路径
		m_ctlListBoxFolder.GetText(i, test);
		// 遍历文件夹内的所有文件和子文件夹
		TraverseFile(test.GetBuffer(), m_strDelete, m_vecFileDelete);
	}
	// 将遍历到的要删除文件添加到ListBox内显示出来
	for (DWORD i = 0; i < m_vecFileDelete.size(); i++)
	{
		m_ctrListBoxFile.AddString(m_vecFileDelete[i]);
		m_ctrListBoxFile.SelectString(i, m_vecFileDelete[i]);

	}
}

void CDiaVs::TraverseFile(
	TCHAR  szPath[MAX_PATH], // 要遍历的文件路径
	const CString &strFilter,// 过滤的文件后缀
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
			TraverseFile(strDIr.GetBuffer(), strFilter, vecPath);
			continue;
		}
		// 获取文件扩展名，用于判断是否垃圾文件后缀
		TCHAR *p = PathFindExtension(fi.cFileName);
		// Find返回查找的子串p在m_Delete中的索引，若找不到返回-1
		if (-1 == strFilter.Find(p))
		{
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

void CDiaVs::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	for (DWORD i = 0; i < m_vecFileDelete.size(); i++)
	{
		DeleteFile(m_vecFileDelete[i].GetBuffer());
		m_ctrListBoxFile.DeleteString(0);
	}
	m_vecFileDelete.swap(vector<CString>());
}



