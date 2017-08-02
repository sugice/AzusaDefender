#pragma once


// CDiaPe 对话框

class CDiaPe : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaPe)

public:
	CDiaPe(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaPe();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CString m_strEdit;
	static CString strFilePath;
};
