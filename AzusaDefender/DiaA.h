#pragma once


// CDiaA 对话框

class CDiaA : public CDialogEx
{
	DECLARE_DYNAMIC(CDiaA)

public:
	CDiaA(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiaA();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
