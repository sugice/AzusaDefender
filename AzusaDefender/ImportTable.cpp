// ImportTable.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "ImportTable.h"
#include "afxdialogex.h"


// CImportTable 对话框

IMPLEMENT_DYNAMIC(CImportTable, CDialogEx)

CImportTable::CImportTable(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG8, pParent)
{

}

CImportTable::~CImportTable()
{
}

void CImportTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImportTable, CDialogEx)
END_MESSAGE_MAP()


// CImportTable 消息处理程序
