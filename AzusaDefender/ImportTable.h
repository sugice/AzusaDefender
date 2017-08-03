#pragma once


// CImportTable 对话框

class CImportTable : public CDialogEx
{
	DECLARE_DYNAMIC(CImportTable)

public:
	CImportTable(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CImportTable();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG8 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
