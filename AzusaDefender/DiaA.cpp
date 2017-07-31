// DiaA.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaA.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CDiaA, CDialogEx)
END_MESSAGE_MAP()


// CDiaA 消息处理程序
