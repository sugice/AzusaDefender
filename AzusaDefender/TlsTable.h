#pragma once


// CTlsTable 对话框

class CTlsTable : public CDialogEx
{
	DECLARE_DYNAMIC(CTlsTable)

public:
	CTlsTable(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTlsTable();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG12 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strEdit;
	CString m_strEdit2;
	CString m_strEdit3;
	CString m_strEdit4;
	CString m_strEdit5;
	CString m_strEdit6;
};
