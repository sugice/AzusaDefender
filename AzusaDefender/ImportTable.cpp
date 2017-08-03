// ImportTable.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "ImportTable.h"
#include "LordPe.h"
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
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Control(pDX, IDC_LIST2, m_ctrlList2);
}


BEGIN_MESSAGE_MAP(CImportTable, CDialogEx)
END_MESSAGE_MAP()


// CImportTable 消息处理程序


BOOL CImportTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化listCtrl
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[5] = { L"DLL名称", L"INT RVA",L"INT偏移", L"IAT RVA", L"IAT偏移" };
	for (int i = 0; i < 5; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	CLordPe lordPe;
	lordPe.ImportTable();
	m_ctrlList.DeleteAllItems();
	DWORD i = 0;
	CString temp;
	for (MY_IMPORT_DESCRIPTOR &importDescriptor : lordPe.m_vecImportDescriptor)
	{
		m_ctrlList.InsertItem(i, importDescriptor.Name);
		temp.Format(L"", importDescriptor.OriginalFirstThunk);
		m_ctrlList.SetItemText(i, 1, temp);
		temp.Format(L"", importDescriptor.OffsetOriginalFirstThunk);
		m_ctrlList.SetItemText(i, 2, temp);
		temp.Format(L"", importDescriptor.FirstThunk);
		m_ctrlList.SetItemText(i, 3, temp);
		temp.Format(L"", importDescriptor.OffsetFirstThunk);
		m_ctrlList.SetItemText(i, 4, temp);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
