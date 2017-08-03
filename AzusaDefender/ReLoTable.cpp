// ReLoTable.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "ReLoTable.h"
#include "afxdialogex.h"
#include "LordPe.h"
#include "DiaPe.h"


// CReLoTable 对话框

IMPLEMENT_DYNAMIC(CReLoTable, CDialogEx)

CReLoTable::CReLoTable(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG10, pParent)
{

}

CReLoTable::~CReLoTable()
{
}

void CReLoTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Control(pDX, IDC_LIST2, m_ctrlList2);
}


BEGIN_MESSAGE_MAP(CReLoTable, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CReLoTable::OnNMDblclkList1)
END_MESSAGE_MAP()


// CReLoTable 消息处理程序


BOOL CReLoTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化listCtrl
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[3] = { L"区段名", L"重定位段RVA", L"重定位数量" };
	for (int i = 0; i < 3; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	//初始化listCtrl2
	m_ctrlList2.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol2[4] = { L"需重定位RVA", L"需重定位偏移", L"重定位类型", L"虚拟地址" };
	for (int i = 0; i < 3; ++i)
	{
		m_ctrlList2.InsertColumn(i, szCol2[i], 0, 100);
	}

	CLordPe lordPe(CDiaPe::m_strFilePath);
	lordPe.ReLoTable();
	m_ctrlList.DeleteAllItems();
	DWORD i = 0;
	CString temp;
	for (auto& each : lordPe.m_vecReloInfo)
	{
		m_ctrlList.InsertItem(i, each.szSectionName);
		temp.Format(L"%08X", each.dwAreaRVA);
		m_ctrlList.SetItemText(i, 1, temp);
		temp.Format(L"%08X", each.dwNumberofReloc);
		m_ctrlList.SetItemText(i, 2, temp);
		++i;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CReLoTable::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlList2.DeleteAllItems();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;  // 这就是双击的item的序号
									 //int nSubItem = pNMListView->iSubItem; // 这是对应的子项号

	CLordPe lordPe(CDiaPe::m_strFilePath);
	lordPe.ReLoTable();
	m_ctrlList2.DeleteAllItems();
	DWORD i = 0;
	CString temp;
	for (auto& each : lordPe.m_vecReloInfo[nItem].vecRelocInfo)
	{
		temp.Format(L"%08X", each.dwRelocRVA);
		m_ctrlList2.InsertItem(i, temp);
		temp.Format(L"%08X", each.dwOffect);
		m_ctrlList2.SetItemText(i, 1, temp);
		temp.Format(L"%08X", each.dwType);
		m_ctrlList2.SetItemText(i, 2, temp);
		temp.Format(L"%08X", each.dwRelicValue);
		m_ctrlList2.SetItemText(i, 2, temp);
		++i;
	}
	*pResult = 0;

}
