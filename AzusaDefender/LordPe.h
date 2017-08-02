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

class CLordPe
{
public:
	CLordPe();
	~CLordPe();
public:
	void GetBasicInfo(CString& filePath);
	BASICINFO m_basicInfo;
	vector<DataTableOfContents> m_vecDataTable;
	vector<SectionTable> m_vecSectionTable;
};

