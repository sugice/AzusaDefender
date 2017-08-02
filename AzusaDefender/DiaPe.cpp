// DiaPe.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaPe.h"
#include "afxdialogex.h"


// CDiaPe 对话框

IMPLEMENT_DYNAMIC(CDiaPe, CDialogEx)

CDiaPe::CDiaPe(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG6, pParent)
	, m_strEdit(_T(""))
{

}

CDiaPe::~CDiaPe()
{
}

void CDiaPe::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strEdit);
}


BEGIN_MESSAGE_MAP(CDiaPe, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaPe::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDiaPe 消息处理程序


void CDiaPe::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	OPENFILENAME ofn;      // 公共对话框结构。     
	TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。               
	// 初始化选择文件对话框。     
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All(*.*)\0*.*\0EXE(*.exe)\0*.exe\0DLL(*.dll)\0*.dll\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);    
	// 显示打开选择文件对话框。     

	if (GetOpenFileName(&ofn))
	{
		//显示选择的文件。     
		m_strEdit = szFile;
		strFilePath = szFile;
		UpdateData(FALSE);
	}
}

