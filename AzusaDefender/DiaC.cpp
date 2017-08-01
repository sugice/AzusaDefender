// DiaC.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaC.h"
#include "afxdialogex.h"


// CDiaC 对话框

IMPLEMENT_DYNAMIC(CDiaC, CDialogEx)

CDiaC::CDiaC(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

CDiaC::~CDiaC()
{
}

void CDiaC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDiaC, CDialogEx)
END_MESSAGE_MAP()


// CDiaC 消息处理程序
