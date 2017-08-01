// DiaC.cpp : 实现文件
//

#include "stdafx.h"
#include "AzusaDefender.h"
#include "DiaC.h"
#include "afxdialogex.h"
#include <winsvc.h>


// CDiaC 对话框

IMPLEMENT_DYNAMIC(CDiaC, CDialogEx)

CDiaC::CDiaC(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

CDiaC::~CDiaC()
{
}

void CDiaC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}



BEGIN_MESSAGE_MAP(CDiaC, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDiaC::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CDiaC::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CDiaC::OnBnClickedButton2)
END_MESSAGE_MAP()



BOOL CDiaC::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化list信息
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TCHAR *szCol[5] = { L"名称", L"状态", L"服务类型", L"启动类型", L"路径信息" };
	for (int i = 0; i < 5; ++i)
	{
		m_ctrlList.InsertColumn(i, szCol[i], 0, 100);
	}
	//枚举服务
	enumService();
	InsertServiceList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CDiaC 消息处理程序
//启动服务
void CDiaC::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwItem;
	for (int i = 0; i < m_ctrlList.GetItemCount(); i++)
	{
		if (m_ctrlList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			dwItem = i;
			break;
		}
	}
	CString strState;
	CString strName;
	strState = m_ctrlList.GetItemText(dwItem, 1);
	strName = m_ctrlList.GetItemText(dwItem, 0);
	if (strState==L"正在运行")
	{
		AfxMessageBox(L"服务正在运行，无需启动");
	}
	else
	{
		//打开计算机服务控制管理器
		SC_HANDLE hSvc = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		SC_HANDLE hSC = OpenService(hSvc, strName, SERVICE_QUERY_CONFIG| SERVICE_START);
		// 启动服务  
		if (::StartService(hSC, NULL, NULL) == FALSE)
		{
			AfxMessageBox(L"启动服务失败");
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
			return;
		}
		AfxMessageBox(L"启动服务成功");
		::CloseServiceHandle(hSvc);
		::CloseServiceHandle(hSC);
		//刷新服务列表
		m_vecServiceInfo.clear();//清空保存的服务信息
		enumService();
		InsertServiceList();
	}
}

//停止服务
void CDiaC::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwItem;
	for (int i = 0; i < m_ctrlList.GetItemCount(); i++)
	{
		if (m_ctrlList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			dwItem = i;
			break;
		}
	}
	CString strState;
	CString strName;
	strState = m_ctrlList.GetItemText(dwItem, 1);
	strName = m_ctrlList.GetItemText(dwItem, 0);
	if (strState == L"已停止")
	{
		AfxMessageBox(L"服务为停止状态，无需停止");
	}
	else
	{
		//打开计算机服务控制管理器
		SC_HANDLE hSvc = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		SC_HANDLE hSC = OpenService(hSvc, strName, SERVICE_QUERY_CONFIG | SERVICE_STOP);
		// 停止服务 
		SERVICE_STATUS status;
		if (::ControlService(hSC,
			SERVICE_CONTROL_STOP, &status) == FALSE)
		{
			AfxMessageBox(L"停止服务失败");
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
			return;
		}
		AfxMessageBox(L"停止服务成功");
		::CloseServiceHandle(hSvc);
		::CloseServiceHandle(hSC);
		//刷新服务列表
		m_vecServiceInfo.clear();//清空保存的服务信息
		enumService();
		InsertServiceList();
	}
}


// 逻辑实现部分

//枚举服务，获取信息
BOOL CDiaC::enumService()
{
	//打开计算机服务控制管理器
	SC_HANDLE hsCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	//第一次调用，获取需要的内存大小
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;
	EnumServicesStatusEx(hsCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,SERVICE_STATE_ALL, NULL, 0, &dwSize, &dwServiceNum, NULL, NULL);
	//申请内存
	LPENUM_SERVICE_STATUS_PROCESS pEnumService = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];
	//再次调用,枚举出服务
	BOOL bStatus = FALSE;
	bStatus = EnumServicesStatusEx(hsCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
									(PBYTE)pEnumService, dwSize, &dwSize, &dwServiceNum, NULL, NULL);
	if (!bStatus)//枚举失败则返回
	{
		::CloseServiceHandle(hsCM);
		return FALSE;
	}
	for (DWORD i = 0; i < dwServiceNum; i++)
	{
		//获取基础信息,保存到结构体中
		SERVICEINFO serviceInfo;
		//服务名
		serviceInfo.cstrSerName = pEnumService[i].lpServiceName;
		//服务状态
		switch (pEnumService[i].ServiceStatusProcess.dwCurrentState)
		{
		case SERVICE_CONTINUE_PENDING:
			serviceInfo.cstrSerState = _T("即将继续");
			break;
		case SERVICE_PAUSE_PENDING:
			serviceInfo.cstrSerState = _T("正在暂停");
			break;
		case SERVICE_PAUSED:
			serviceInfo.cstrSerState = _T("已暂停");
			break;
		case SERVICE_RUNNING:
			serviceInfo.cstrSerState = _T("正在运行");
			break;
		case SERVICE_START_PENDING:
			serviceInfo.cstrSerState = _T("正在启动");
			break;
		case SERVICE_STOP_PENDING:
			serviceInfo.cstrSerState = _T("正在停止");
			break;
		case SERVICE_STOPPED:
			serviceInfo.cstrSerState = _T("已停止");
			break;
		default:
			break;
		}
		//服务类型
		switch (pEnumService[i].ServiceStatusProcess.dwServiceType)
		{
		case SERVICE_FILE_SYSTEM_DRIVER:
			serviceInfo.cstrSerType = _T("文件系统驱动程序");
			break;
		case SERVICE_KERNEL_DRIVER:
			serviceInfo.cstrSerType = _T("设备驱动程序");
			break;
		case SERVICE_WIN32_OWN_PROCESS:
			serviceInfo.cstrSerType = _T("运行在自己的进程中");
			break;
		case SERVICE_WIN32_SHARE_PROCESS:
			serviceInfo.cstrSerType = _T("与其他服务共享进程");
			break;
		default:
			break;
		}
		//打开服务获取更多信息
		SC_HANDLE hService = OpenService(hsCM, pEnumService[i].lpServiceName, SERVICE_QUERY_CONFIG);
		//第一次调用获取需要的缓冲区大小
		QueryServiceConfig(hService, NULL, 0, &dwSize);
		//分配内存
		LPQUERY_SERVICE_CONFIG pServiceConfig = (LPQUERY_SERVICE_CONFIG)new char[dwSize];
		//再次调用，获取信息
		QueryServiceConfig(hService, pServiceConfig, dwSize, &dwSize);
		//获取信息填入结构体变量
		switch (pServiceConfig->dwStartType)
		{
		case SERVICE_AUTO_START:
			serviceInfo.cstrStartType = _T("自动");
			break;
		case SERVICE_BOOT_START:
			serviceInfo.cstrStartType = _T("由系统加载程序启动");
			break;
		case SERVICE_DEMAND_START:
			serviceInfo.cstrStartType = _T("手动");
			break;
		case SERVICE_DISABLED:
			serviceInfo.cstrStartType = _T("禁用");
			break;
		case SERVICE_SYSTEM_START:
			serviceInfo.cstrStartType = _T("由IoInitSystem功能启动");
			break;
		default:
			break;
		}
		//路径信息
		serviceInfo.cstrBinaryPathName = pServiceConfig->lpBinaryPathName;
		m_vecServiceInfo.push_back(serviceInfo);
	}
	return TRUE;
}

BOOL CDiaC::InsertServiceList()
{
	m_ctrlList.DeleteAllItems();
	int i = 0;//索引用
	CString temp;
	// L"名称", L"状态", L"服务类型", L"启动类型", L"路径信息"
	for (SERVICEINFO &serviceInfo : m_vecServiceInfo) {
		// 第1列,名称
		m_ctrlList.InsertItem(i, serviceInfo.cstrSerName);
		// 第2列,状态
		m_ctrlList.SetItemText(i, 1, serviceInfo.cstrSerState);
		// 第3列,服务类型
		m_ctrlList.SetItemText(i, 2,serviceInfo.cstrSerType);
		// 第4列,启动类型
		m_ctrlList.SetItemText(i, 3, serviceInfo.cstrStartType);
		// 第5列,路径信息
		m_ctrlList.SetItemText(i, 4, serviceInfo.cstrBinaryPathName);
		++i;
	}
	return TRUE;
}




void CDiaC::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	//枚举服务
	m_vecServiceInfo.clear();//清空保存的服务信息
	enumService();
	InsertServiceList();
}



