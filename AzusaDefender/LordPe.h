#pragma once
#include <vector>
using std::vector;


//基本信息
typedef struct _BASICINFO
{
	WORD NumberOfSections;//区段数量
	WORD SizeOfOptionalHeader;//扩展头大小
	WORD Characteristics;//文件属性

	WORD Magic;//文件类型标志
	DWORD SizeOfCode;//代码段总大小
	DWORD AddressOfEntryPoint;//入口点
	DWORD BaseOfCode;//代码基址
	DWORD BaseOfData;//数据基址
	DWORD ImageBase;//默认加载基址
	DWORD SectionAlignment;//块对齐
	DWORD FileAlignment;//文件块对齐
	DWORD SizeOfImage;//镜像大小
	DWORD SizeOfHeaders;//头部总大小
	WORD DllCharacteristics;//DLL特征
	DWORD NumberOfRvaAndSizes;//数据目录个数
}BASICINFO,*PBASICINFO;

//数据目录表
typedef struct _DataTableOfContents
{
	DWORD VirtualAddress;//相对虚拟地址
	DWORD Size;//大小
}DataTableOfContents,*PDataTableOfContents;

//区段表
typedef struct _SectionTable
{
	CString Name;//区段名
	DWORD VirtualSize;//实际大小
	DWORD SizeOfRawData;//文件中大小
	DWORD PointerToRawData;//文件中位置
	DWORD PointerToVirData;//内存中位置
	DWORD Characteristics;//区段属性
}SectionTable, *PSectionTable;

//导出表基本信息
typedef struct _MY_IM_EX_DI
{
	CString name;//dll名
	DWORD Base;//序号基数
	DWORD NumberOfFunctions;//函数数量
	DWORD NumberOfNames;//函数名称数量
	DWORD AddressOfFunctions;//地址表RVA
	DWORD AddressOfNames;//名称表RVA
	DWORD AddressOfNameOrdinals;//序号表RVA
}MY_IM_EX_DI,*PMY_IM_EX_DI;

//导出表函数信息
typedef struct _EXPORTFUNINFO
{
	DWORD ExportOrdinals;//导出序号
	DWORD FunctionRVA;//函数RVA
	DWORD FunctionOffset;//函数文件偏移
	CString FunctionName;//函数名
}EXPORTFUNINFO,*PEXPORTFUNINFO;

class CLordPe
{
public:
	CLordPe();
	~CLordPe();
	CLordPe(CString& filePath);
public:
	void GetBasicInfo();
	void ExportTable();
	BOOL GetDosHead(CString& filePath);
	DWORD RVAToOffset(IMAGE_DOS_HEADER* pDos, DWORD dwRva);
public:
	BYTE* m_pBuf;//用于释放申请的空间
	PIMAGE_DOS_HEADER m_pDosHdr;//DOS头地址
	BASICINFO m_basicInfo;//头文件中的基础信息
	vector<DataTableOfContents> m_vecDataTable;//数据目录表
	vector<SectionTable> m_vecSectionTable;//区段表
	vector<EXPORTFUNINFO> m_vecExportFunInfo;
	MY_IM_EX_DI m_my_im_ex_di;
};

