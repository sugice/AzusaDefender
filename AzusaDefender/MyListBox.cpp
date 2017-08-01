// MyListBox.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "MyListBox.h"


// CMyListBox

IMPLEMENT_DYNAMIC(CMyListBox, CListBox)

CMyListBox::CMyListBox()
{

}

CMyListBox::~CMyListBox()
{
}


BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CMyListBox 消息处理程序


void CMyListBox::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szPath[MAX_PATH] = {};
	// 统计拖拽的文件夹的数目
	int nCount = DragQueryFile(hDropInfo, -1, szPath, MAX_PATH);
	// 分别获取每个文件夹的路径
	for (int i = 0; i < nCount; i++)
	{
		DragQueryFile(hDropInfo, i, szPath, MAX_PATH);
		if (LB_ERR == FindString(0, szPath))
		{// 如果m_ListBox里面没有这个新的文件夹路径，再添加进m_ListBox
			AddString(szPath);
		}
	}
	CListBox::OnDropFiles(hDropInfo);
}

void CMyListBox::Scroll()
{
	CString    str;
	CSize      sz;
	int        dx = 0;
	TEXTMETRIC tm;
	CDC*       pDC = GetDC();
	CFont*     pFont = GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < GetCount(); i++)
	{
		GetText(i, str);
		sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	SetHorizontalExtent(dx);
}

void CMyListBox::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	Scroll();				   // TODO: 在此处添加消息处理程序代码
	CListBox::OnPaint();				   // 不为绘图消息调用 CListBox::OnPaint()
}
