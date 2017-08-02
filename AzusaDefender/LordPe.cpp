#include "stdafx.h"
#include "LordPe.h"


CLordPe::CLordPe()
{
}


CLordPe::~CLordPe()
{
}

void CLordPe::GetBasicInfo(CString& filePath)
{
	// 1. 打开文件,将文件读取到内存.
	// CreateFile,ReadFile.
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,
						OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileSize = 0;
	dwFileSize = GetFileSize(hFile, NULL);

	// 2. 申请内存空间
	BYTE* pBuf = new BYTE[dwFileSize];

	// 3. 将文件内容读取到内存中
	DWORD dwRead = 0;
	ReadFile(hFile,pBuf,dwFileSize,&dwRead,NULL);

	// 将缓冲区当成DOS头结构体来解析
	IMAGE_DOS_HEADER* pDosHdr;// DOS头
	pDosHdr = (IMAGE_DOS_HEADER*)pBuf;
	IMAGE_NT_HEADERS* pNtHdr;

	// nt头,包含这文件头和扩展头
	pNtHdr = (IMAGE_NT_HEADERS*)(pDosHdr->e_lfanew + (DWORD)pBuf);

	// 判断是否是一个有效的pe文件
	if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE|| pNtHdr->Signature != IMAGE_NT_SIGNATURE)
	{
		AfxMessageBox(_T("不是有效的PE文件"));
		return;
	}

	IMAGE_FILE_HEADER* pFileHdr; // 文件头
	IMAGE_OPTIONAL_HEADER* pOptHdr;// 扩展头
	pFileHdr = &(pNtHdr->FileHeader);
	pOptHdr = &(pNtHdr->OptionalHeader);
	
	ZeroMemory(&m_basicInfo, sizeof(m_basicInfo));
	//文件头内的信息
	m_basicInfo.NumberOfSections = pFileHdr->NumberOfSections;
	m_basicInfo.SizeOfOptionalHeader = pFileHdr->SizeOfOptionalHeader;
	m_basicInfo.Characteristics = pFileHdr->Characteristics;
	//扩展头的信息
	m_basicInfo.Magic = pOptHdr->Magic;
	m_basicInfo.SizeOfCode = pOptHdr->SizeOfCode;
	m_basicInfo.AddressOfEntryPoint = pOptHdr->AddressOfEntryPoint;
	m_basicInfo.BaseOfCode = pOptHdr->BaseOfCode;
	m_basicInfo.BaseOfData = pOptHdr->BaseOfData;
	m_basicInfo.ImageBase = pOptHdr->ImageBase;
	m_basicInfo.SectionAlignment = pOptHdr->SectionAlignment;
	m_basicInfo.FileAlignment = pOptHdr->FileAlignment;
	m_basicInfo.SizeOfImage = pOptHdr->SizeOfImage;
	m_basicInfo.SizeOfHeaders = pOptHdr->SizeOfHeaders;
	m_basicInfo.DllCharacteristics = pOptHdr->DllCharacteristics;
	m_basicInfo.NumberOfRvaAndSizes = pOptHdr->NumberOfRvaAndSizes;

	PIMAGE_DATA_DIRECTORY pDataDirectory = pOptHdr->DataDirectory;
	DWORD i = 0;
	m_vecDataTable.clear();
	while (i < pOptHdr->NumberOfRvaAndSizes)
	{
		DataTableOfContents dataTableObj = { 0 };
		dataTableObj.VirtualAddress = pDataDirectory[i].VirtualAddress;
		dataTableObj.Size = pDataDirectory[i].Size;
		m_vecDataTable.push_back(dataTableObj);
		++i;
	}

	IMAGE_SECTION_HEADER* pScnHdr = NULL;
	pScnHdr = IMAGE_FIRST_SECTION(pNtHdr);
	m_vecSectionTable.clear();
	for (int i = 0; i < pFileHdr->NumberOfSections; ++i)
	{
		SectionTable sectionTable = { 0 };
		sectionTable.Name = pScnHdr[i].Name;
		sectionTable.VirtualSize = pScnHdr[i].Misc.VirtualSize;
		sectionTable.SizeOfRawData = pScnHdr[i].SizeOfRawData;
		sectionTable.PointerToRawData = pScnHdr[i].PointerToRawData;
		sectionTable.PointerToVirData = pScnHdr[i].VirtualAddress + pOptHdr->ImageBase;
		sectionTable.Characteristics = pScnHdr[i].Characteristics;
		m_vecSectionTable.push_back(sectionTable);
	}
}
