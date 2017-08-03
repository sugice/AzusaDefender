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

//导入表基本信息
/*
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
union {
DWORD   Characteristics;            // 0 for terminating null import descriptor
DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
} DUMMYUNIONNAME;
DWORD   TimeDateStamp;                  // 0 if not bound,
// -1 if bound, and real date\time stamp
//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
// O.W. date/time stamp of DLL bound to (Old BIND)

DWORD   ForwarderChain;                 // -1 if no forwarders
DWORD   Name;
DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
*/
typedef struct _MY_IMPORT_DESCRIPTOR
{
	CString Name;//DLL名称
	DWORD OriginalFirstThunk;//INT(导入名称表RVA)
	DWORD FirstThunk;//IAT(导入地址表RVA)
	DWORD OffsetOriginalFirstThunk;//INT(导入名称表偏移)
	DWORD OffsetFirstThunk;//IAT(导入地址表偏移)

}MY_IMPORT_DESCRIPTOR,*PMY_IMPORT_DESCRIPTOR;

typedef struct _IMPORTFUNINFO
{
	DWORD Ordinal;
	CString Name;

}IMPORTFUNINFO ,*PIMPORTFUNINFO;

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
	void ImportTable();
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
	vector<MY_IMPORT_DESCRIPTOR> m_vecImportDescriptor;
	vector<IMPORTFUNINFO> m_vecImportFunInfo;
};

