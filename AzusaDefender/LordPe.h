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
typedef struct _MY_IMPORT_DESCRIPTOR
{
	CString Name;//DLL名称
	DWORD OriginalFirstThunk;//INT(导入名称表RVA)
	DWORD OffsetOriginalFirstThunk;//INT(导入名称表偏移)
	DWORD FirstThunk;//IAT(导入地址表RVA)
	DWORD OffsetFirstThunk;//IAT(导入地址表偏移)

}MY_IMPORT_DESCRIPTOR,*PMY_IMPORT_DESCRIPTOR;

//导入函数信息
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

//资源信息
typedef struct _RESOURCEINFO
{
	CString NameOrID;//资源名称或ID
	DWORD ResourceRVA;//资源RVA
	DWORD ResourceSize;//资源大小
}RESOURCEINFO, *PRESOURCEINFO;


typedef struct _TYPEOFFECT {
	WORD Offset : 12;  // (1) 大小为12Bit的重定位偏移
	WORD Type : 4;    // (2) 大小为4Bit的重定位信息类型值
}TYPEOFFECT, *PTYPEOFFECT;	    // 这个结构体是A1Pass总结的

typedef struct _RELOINFO
{
	DWORD dwRelocRVA;//需要重定位的相对虚拟地址
	DWORD dwOffect;//根据相对虚拟地址算出的文件偏移
	DWORD dwType;//重定位方式（也可以叫属性）
	DWORD dwRelicValue;//从算出的文件偏移取出的数据，这个数据就是虚拟地址（VA）
}RELOCINFO, *PRELOINFO;

typedef struct _RELOCAREAINFO
{
	CString szSectionName;//区域所在的节名
	DWORD dwAreaRVA;//区域的基址（它是一个相对虚拟地址）
	DWORD dwNumberofReloc;//这个区域的重定位个数
	std::vector<RELOCINFO> vecRelocInfo;
}RELOCAREINFO, *PRELOCAREINFO;

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
	void ResourceTable();
	void ReLoTable();
	void parseResourcesTable(DWORD dwResRootDirAddr, IMAGE_RESOURCE_DIRECTORY* pResDir, int nDeep);
	void DelayLoadTable();
	BOOL GetDosHead(CString& filePath);
	DWORD RVAToOffset(IMAGE_DOS_HEADER* pDos, DWORD dwRva);
	void FindSectionName(IMAGE_DOS_HEADER* pDos, DWORD dwRva,CString& temp);
public:
	BYTE* m_pBuf;//用于释放申请的空间
	PIMAGE_DOS_HEADER m_pDosHdr;//DOS头地址
	//----------文件头、扩展头、数据目录表、区段表-----------//
	BASICINFO m_basicInfo;//头文件中的基础信息
	vector<DataTableOfContents> m_vecDataTable;//数据目录表
	vector<SectionTable> m_vecSectionTable;//区段表

	//----------------导出表---------------------//
	vector<EXPORTFUNINFO> m_vecExportFunInfo;
	MY_IM_EX_DI m_my_im_ex_di;

	//----------------导出表---------------------//
	vector<MY_IMPORT_DESCRIPTOR> m_vecImportDescriptor;
	vector<IMPORTFUNINFO> m_vecImportFunInfo;
	vector<vector<IMPORTFUNINFO>> m_vvImportFunInfo;

	//---------------资源表---------------------//
	vector<CString> m_vecResourceTpye;
	vector<RESOURCEINFO> m_vecResourceInfo;
	vector<vector<RESOURCEINFO>> m_vvResourceInfo;

	//---------------重定位表---------------------//
	vector<RELOCAREINFO> m_vecReloInfo;
};

