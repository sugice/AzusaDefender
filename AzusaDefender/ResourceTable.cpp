// ResourceTable.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "ResourceTable.h"
#include "afxdialogex.h"
#include "LordPe.h"
#include "DiaPe.h"


// CResourceTable 对话框

IMPLEMENT_DYNAMIC(CResourceTable, CDialogEx)

CResourceTable::CResourceTable(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG9, pParent)
{

}

CResourceTable::~CResourceTable()
{
}

void CResourceTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Control(pDX, IDC_LIST2, m_ctrlList2);
}


BEGIN_MESSAGE_MAP(CResourceTable, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CResourceTable::OnNMDblclkList1)
END_MESSAGE_MAP()


// CResourceTable 消息处理程序


BOOL CResourceTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化listCtrl
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ctrlList.InsertColumn(0, L"资源类型", 0, 100);
	//初始化listCtrl2
	m_ctrlList2.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol2[3] = { L"资源ID或名称", L"资源RVA", L"资源大小" };
	for (int i = 0; i < 3; ++i)
	{
		m_ctrlList2.InsertColumn(i, szCol2[i], 0, 100);
	}

	CLordPe lordPe(CDiaPe::m_strFilePath);
	lordPe.ResourceTable();
	m_ctrlList.DeleteAllItems();
	DWORD i = 0;
	for (auto& each : lordPe.m_vecResourceTpye)
	{
		m_ctrlList.InsertItem(i, each);
		++i;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CResourceTable::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlList2.DeleteAllItems();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;  // 这就是双击的item的序号
	//int nSubItem = pNMListView->iSubItem; // 这是对应的子项号

	CLordPe lordPe(CDiaPe::m_strFilePath);
	lordPe.ResourceTable();
	m_ctrlList2.DeleteAllItems();
	DWORD i = 0;
	CString temp;
	for (auto& each : lordPe.m_vvResourceInfo[nItem])
	{
		m_ctrlList2.InsertItem(i, each.NameOrID);
		temp.Format(L"%08X", each.ResourceRVA);
		m_ctrlList2.SetItemText(i, 1, temp);
		temp.Format(L"%08X", each.ResourceSize);
		m_ctrlList2.SetItemText(i, 2, temp);
		++i;
	}
	*pResult = 0;
}
