#include "stdafx.h"
#include "LordPe.h"

RESOURCEINFO resourceInfo = { 0 };

CLordPe::CLordPe(CString& filePath)
{
	GetDosHead(filePath);
}

CLordPe::CLordPe()
{

}

CLordPe::~CLordPe()
{
	delete[] m_pBuf;
}


BOOL CLordPe::GetDosHead(CString& filePath)
{
	// 1. 打开文件,将文件读取到内存.
	// CreateFile,ReadFile.
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD dwFileSize = 0;
	dwFileSize = GetFileSize(hFile, NULL);

	// 2. 申请内存空间
	BYTE* pBuf = new BYTE[dwFileSize];
	m_pBuf = pBuf;//保存起来，用于析构释放空间

	// 3. 将文件内容读取到内存中
	DWORD dwRead = 0;
	ReadFile(hFile, pBuf, dwFileSize, &dwRead, NULL);

	// 将缓冲区当成DOS头结构体来解析
	m_pDosHdr = (IMAGE_DOS_HEADER*)pBuf;//将DOS头指针保存起来

	// nt头,包含这文件头和扩展头
	IMAGE_NT_HEADERS* pNtHdr;
	pNtHdr = (IMAGE_NT_HEADERS*)(m_pDosHdr->e_lfanew + (DWORD)m_pDosHdr);

	// 判断是否是一个有效的pe文件
	if (m_pDosHdr->e_magic != IMAGE_DOS_SIGNATURE || pNtHdr->Signature != IMAGE_NT_SIGNATURE)
	{
		AfxMessageBox(_T("不是有效的PE文件"));
		return FALSE;
	}
	return TRUE;
}

void CLordPe::GetBasicInfo()
{
	// nt头,包含这文件头和扩展头
	IMAGE_NT_HEADERS* pNtHdr;
	pNtHdr = (IMAGE_NT_HEADERS*)(m_pDosHdr->e_lfanew + (DWORD)m_pDosHdr);

	IMAGE_FILE_HEADER* pFileHdr; // 文件头
	pFileHdr = &(pNtHdr->FileHeader);

	IMAGE_OPTIONAL_HEADER* pOptHdr;// 扩展头
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

//解析导出表
void CLordPe::ExportTable()
{
	// nt头,包含这文件头和扩展头
	IMAGE_NT_HEADERS* pNtHdr;
	pNtHdr = (IMAGE_NT_HEADERS*)(m_pDosHdr->e_lfanew + (DWORD)m_pDosHdr);

	IMAGE_OPTIONAL_HEADER* pOptHdr;// 扩展头
	pOptHdr = &(pNtHdr->OptionalHeader);

	//数据目录表
	PIMAGE_DATA_DIRECTORY pDataDirectory = pOptHdr->DataDirectory;

	// 5. 找到导出表
	DWORD dwExpRva = pDataDirectory[0].VirtualAddress;

	// 5.1 得到RVA的文件偏移
	DWORD dwExpOfs = RVAToOffset(m_pDosHdr, dwExpRva);
	IMAGE_EXPORT_DIRECTORY* pExpTab = NULL;
	pExpTab = (IMAGE_EXPORT_DIRECTORY*)(dwExpOfs + (DWORD)m_pDosHdr);

	// 解析DLL的导出表
	/*
	typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name; // dll名[字符串的RVA]
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;     // 地址表(DWORD数组)的rva
	DWORD   AddressOfNames;         // 名称表(DWORD数组)的rva
	DWORD   AddressOfNameOrdinals;  // 序号表(WORD数组)的rva
	} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
	*/
	// 1. 解析DLL的名
	DWORD dwNameOfs = RVAToOffset(m_pDosHdr, pExpTab->Name);
	char* pDllName =(char*)(dwNameOfs + (DWORD)m_pDosHdr);
	//导出表基本信息
	//ZeroMemory(&m_my_im_ex_di, sizeof(m_my_im_ex_di));//全字段置为零
	m_my_im_ex_di.name = pDllName;
	m_my_im_ex_di.Base = pExpTab->Base;
	m_my_im_ex_di.NumberOfFunctions = pExpTab->NumberOfFunctions;
	m_my_im_ex_di.NumberOfNames = pExpTab->NumberOfNames;
	m_my_im_ex_di.AddressOfFunctions = pExpTab->AddressOfFunctions;
	m_my_im_ex_di.AddressOfNames = pExpTab->AddressOfNames;
	m_my_im_ex_di.AddressOfNameOrdinals = pExpTab->AddressOfNameOrdinals;

	// 解析三张表
	DWORD dwExpAddrTabOfs = RVAToOffset(m_pDosHdr, pExpTab->AddressOfFunctions);
	DWORD dwExpNameTabOfs = RVAToOffset(m_pDosHdr, pExpTab->AddressOfNames);
	DWORD dwExpOrdTabOfs = RVAToOffset(m_pDosHdr, pExpTab->AddressOfNameOrdinals);

	// 地址表,名称表是一个DWORD类型数组
	DWORD* pExpAddr =(DWORD*)(dwExpAddrTabOfs + (DWORD)m_pDosHdr);

	DWORD* pExpName =(DWORD*)(dwExpNameTabOfs + (DWORD)m_pDosHdr);

	// 序号表是WORD类型的数组
	WORD* pExpOrd =(WORD*)(dwExpOrdTabOfs + (DWORD)m_pDosHdr);

	// 遍历所有的函数地址
	m_vecExportFunInfo.clear();
	for (int i = 0; i< pExpTab->NumberOfFunctions; ++i)
	{
		EXPORTFUNINFO exportFunInfo = {0};
		exportFunInfo.FunctionRVA = pExpAddr[i];//函数RVA
		exportFunInfo.FunctionOffset = RVAToOffset(m_pDosHdr, pExpAddr[i]);//函数相对文件偏移
		// 查找当前遍历到的地址有没有名称
		int j = 0;
		for (; j < pExpTab->NumberOfNames; ++j)
		{
			// 判断地址表的下标是否被被保存于函数名称序号表中.
			// 如果被保存了, 则说明这个下标所保存的地址,是一个有函数名称的地址
			if (i == pExpOrd[j])
				break;
		}
		// 判断循环是自然结束,还是通过break跳出
		if (j < pExpTab->NumberOfNames)
		{
			// 地址有名称,j就是对应的函数名表的下标.通过j可以找到一个函数名称的Rva
			DWORD dwNameRva = pExpName[j];
			DWORD dwNameOfs = RVAToOffset(m_pDosHdr, dwNameRva);
			char* pFunctionName = nullptr;
			pFunctionName = (char*)(dwNameOfs + (DWORD)m_pDosHdr);
			exportFunInfo.FunctionName = pFunctionName;//函数名
			exportFunInfo.ExportOrdinals = pExpTab->Base + i;//函数导出序号
		}
		else
		{
			// 说名地址没有以名称的方式导出.
			if (pExpAddr[i] != 0)
			{
				// 函数的导出序号:
				// 序号基数 + 虚序号(地址标的下标)
				exportFunInfo.FunctionName = L"";//函数名为空
				exportFunInfo.ExportOrdinals = pExpTab->Base + i;//函数导出序号
			}
		}
		m_vecExportFunInfo.push_back(exportFunInfo);
	}
}



void CLordPe::ImportTable()
{
	// nt头,包含这文件头和扩展头
	IMAGE_NT_HEADERS* pNtHdr;
	pNtHdr = (IMAGE_NT_HEADERS*)(m_pDosHdr->e_lfanew + (DWORD)m_pDosHdr);

	IMAGE_OPTIONAL_HEADER* pOptHdr;// 扩展头
	pOptHdr = &(pNtHdr->OptionalHeader);

	//数据目录表
	PIMAGE_DATA_DIRECTORY pDataDirectory = pOptHdr->DataDirectory;

	// 5. 找到导入表
	// 5. 得到导入表的RVA
	DWORD dwImpRva = pDataDirectory[1].VirtualAddress;

	IMAGE_IMPORT_DESCRIPTOR* pImpArray;

	pImpArray = (IMAGE_IMPORT_DESCRIPTOR*)(RVAToOffset(m_pDosHdr, dwImpRva) + (DWORD)m_pDosHdr);

	// 导入表数组的个数并没有其它字段记录.
	// 结束的标志是以一个全0的元素作为结尾
	m_vecImportDescriptor.clear();
	m_vvImportFunInfo.clear();
	while (pImpArray->Name != 0)
	{
		MY_IMPORT_DESCRIPTOR myImportDescriptor = { 0 };
		// 导入的Dll的名字(Rva)
		DWORD dwNameOfs = RVAToOffset(m_pDosHdr, pImpArray->Name);
		char* pDllName = (char*)(dwNameOfs + (DWORD)m_pDosHdr);
		myImportDescriptor.Name = pDllName;

		// 解析,在这个dll中,一共导入哪些函数
		myImportDescriptor.OriginalFirstThunk = pImpArray->OriginalFirstThunk;
		myImportDescriptor.FirstThunk = pImpArray->FirstThunk;

		// IAT(导入名表)记录着一个从一个dll中导入了哪些函数,这些函数要么是以名称导入,要么是以序号导入的
		// 记录在一个数组中. 这个数组是IMAGE_THUNK_DATA类型的结构体数组.
		DWORD INTOfs = RVAToOffset(m_pDosHdr, pImpArray->OriginalFirstThunk);
		myImportDescriptor.OffsetOriginalFirstThunk = INTOfs;

		DWORD IATOfs = RVAToOffset(m_pDosHdr, pImpArray->FirstThunk);
		myImportDescriptor.OffsetFirstThunk = IATOfs;

		m_vecImportDescriptor.push_back(myImportDescriptor);
		/*
		这是一个只有4个字节的结构体.里面联合体中的每一个字段保存的值都是一样.这些值, 就是导入函数的信息.
		导入函数的信息有以下部分:
		1. 导入函数的序号
		2. 导入函数的名称(可能有可能没有)
		可以根据结构体中的字段的最高位判断, 导入信息是以名称导入还是以序号导入
		typedef struct _IMAGE_THUNK_DATA32 {
		union {
		DWORD ForwarderString;      // PBYTE
		DWORD Function;             // PDWORD
		DWORD Ordinal;
		DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
		} u1;
		} IMAGE_THUNK_DATA32;
		*/
		IMAGE_THUNK_DATA* pInt = NULL;
		IMAGE_THUNK_DATA* pIat = NULL;
		pInt = (IMAGE_THUNK_DATA*)(INTOfs + (DWORD)m_pDosHdr);
		pIat = (IMAGE_THUNK_DATA*)(IATOfs + (DWORD)m_pDosHdr);

		//INT可以看做是IAT的备份，存在没有备份的情况，因此解析IAT
		m_vecImportFunInfo.clear();
		while (pIat->u1.Function != 0)
		{
			IMPORTFUNINFO importFunInfo = {0};
			// 判断是否是以序号导入
			if (IMAGE_SNAP_BY_ORDINAL32(pIat->u1.Function))
			{
				// 以序号方式导入结构体保存的值低16位就是一个导入的序号
				importFunInfo.Ordinal = pIat->u1.Ordinal & 0xFFFF;
				importFunInfo.Name = L"";
			}
			else
			{
				// 当函数是以名称导入的时候, pInt->u1.Function 保存的是一个rva , 
				//这个RVA指向一个保存函数名称信息的结构体
				DWORD dwImpNameOfs = RVAToOffset(m_pDosHdr, pIat->u1.Function);
				IMAGE_IMPORT_BY_NAME* pImpName;
				pImpName = (IMAGE_IMPORT_BY_NAME*)(dwImpNameOfs + (DWORD)m_pDosHdr);
				importFunInfo.Ordinal = pImpName->Hint;
				importFunInfo.Name = pImpName->Name;
				m_vecImportFunInfo.push_back(importFunInfo);
			}
			++pIat;
		}
		m_vvImportFunInfo.push_back(m_vecImportFunInfo);
		++pImpArray;
	}
}

//得到资源表起始地址，并调用解析函数
void CLordPe::ResourceTable()
{
	// nt头,包含这文件头和扩展头
	IMAGE_NT_HEADERS* pNtHdr;
	pNtHdr = (IMAGE_NT_HEADERS*)(m_pDosHdr->e_lfanew + (DWORD)m_pDosHdr);

	IMAGE_OPTIONAL_HEADER* pOptHdr;// 扩展头
	pOptHdr = &(pNtHdr->OptionalHeader);

	//数据目录表
	PIMAGE_DATA_DIRECTORY pDataDirectory = pOptHdr->DataDirectory;

	// 5. 找到资源表
	// 5. 得到资源表的RVA
	DWORD dwResRootRva = pDataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	PIMAGE_RESOURCE_DIRECTORY pResRoot;
	pResRoot = (IMAGE_RESOURCE_DIRECTORY*)(RVAToOffset(m_pDosHdr, dwResRootRva) + (DWORD)m_pDosHdr);
	parseResourcesTable((DWORD)pResRoot, pResRoot, 1);

}

//解析资源表的函数，递归解析
void CLordPe::parseResourcesTable(DWORD dwResRootDirAddr,//根目录的首地址
	IMAGE_RESOURCE_DIRECTORY* pResDir,//需要解析的资源目录
	int nDeep)//记录这是第几层目录
{
	// 获取目录入口的总个数
	DWORD dwEntryCount = pResDir->NumberOfIdEntries + pResDir->NumberOfNamedEntries;
	// 获取目录入口数组的首地址
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pResDirEntry;
	pResDirEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pResDir + 1);
	//pResDir + 1意思就是根据pResDir被定义的类型的长度，在pResDir原来的地址上再加上这个长度，使pResDir指向下一个地址。

	WCHAR buff[512];

	if (nDeep == 1) { //第一层目录入口,ID信息是资源类型的id,指向下一层的偏移指向的是第二层的资源目录.
		m_vecResourceTpye.clear();
		m_vvResourceInfo.clear();
		for (DWORD i = 0; i < dwEntryCount; ++i)
		{
			CString resourceType;
			// 解析资源的类型,判断资源类型是整形的还是字符串类型的
			if (pResDirEntry[i].NameIsString == 1) {
				// 资源类型是字符串
				IMAGE_RESOURCE_DIR_STRING_U* pTypeName;
				// NameOffset 保存的偏移,是把资源根目录的地址作为基地址的偏移
				pTypeName =(IMAGE_RESOURCE_DIR_STRING_U*)(pResDirEntry[i].NameOffset + dwResRootDirAddr);

				wcsncpy_s(buff, pTypeName->NameString, pTypeName->Length);//结构体名称最后的_U说明了结构体内存放的是宽字节字符
				resourceType = buff;
			}
			else {
				// 说明资源类型ID是一个整形的值
				char *szType[] =
				{
					("") ,				// 0
					("光标") ,				// 1 
					("位图") ,				// 2 RT_BITMAP
					("图标") ,			    // 3 RT_ICON
					("菜单") ,				// 4 RT_MENU
					("对话框") ,			// 5 RT_DIALOG      
					("字符串表") ,			// 6 RT_STRING      
					("字体目录") ,			// 7 RT_FONTDIR     
					("字体") ,				// 8 RT_FONT        
					("加速键") ,			// 9 RT_ACCELERATOR 
					("RC数据") ,		// 10 RT_RCDATA      
					("消息表") ,			// 11 RT_MESSAGETABLE
					("光标组") ,			// 12 
					("") ,				   // 13 
					("图标组") ,			// 14 
					"" ,					// 15
					("版本信息") ,			// 16
					("对话框包含目录") ,	// 17 #define RT_DLGINCLUDE   MAKEINTRESOURCE(17)
					"" ,								// 18 #define RT_PLUGPLAY     MAKEINTRESOURCE(19)
					"" ,								// 19 #define RT_VXD          MAKEINTRESOURCE(20)
					"" ,								// 20 #define RT_ANICURSOR    MAKEINTRESOURCE(21)
					"" ,								// 21 #define RT_ANIICON      MAKEINTRESOURCE(22)
					"" ,									// 22 
					("HTML") ,						    // 23 #define RT_HTML         MAKEINTRESOURCE(23)
					("清单文件")			// 24 RT_MANIFEST
				};

				if (pResDirEntry[i].Id >= 1 && pResDirEntry[i].Id <= 24) {
					resourceType = szType[pResDirEntry[i].Id];
				}
				else {
					resourceType.Format(L"%08X", pResDirEntry[i].Id);
				}
			}
			m_vecResourceTpye.push_back(resourceType);

			// 解析下一层目录
			IMAGE_RESOURCE_DIRECTORY* pNextDir =
				(IMAGE_RESOURCE_DIRECTORY*)(pResDirEntry[i].OffsetToDirectory + dwResRootDirAddr);

			parseResourcesTable(dwResRootDirAddr, pNextDir, nDeep + 1);
			m_vvResourceInfo.push_back(m_vecResourceInfo);
		}
	}
	else if (nDeep == 2) { /*第二层*/

		m_vecResourceInfo.clear();
		for (DWORD i = 0; i < dwEntryCount; ++i)
		{
			//ZeroMemory(&resourceInfo, sizeof(RESOURCEINFO));
			// 资源目录的第二层, 保存的是每种资源的资源id
			// 解析ID
			// 1. 整形ID
			// 2. 字符串ID
			if (pResDirEntry[i].NameIsString) {
				// 资源类型是字符串
				IMAGE_RESOURCE_DIR_STRING_U* pTypeName;
				// NameOffset 保存的偏移,是把资源根目录的地址作为基地址的偏移
				pTypeName =
					(IMAGE_RESOURCE_DIR_STRING_U*)
					(pResDirEntry[i].NameOffset + dwResRootDirAddr);

				wcsncpy_s(buff, pTypeName->NameString, pTypeName->Length);
				resourceInfo.NameOrID = buff;
			}
			else {
				CString temp;
				temp.Format(L"%08X", pResDirEntry[i].Id);
				resourceInfo.NameOrID = temp;
			}

			// 解析偏移
			// 解析下一层目录
			IMAGE_RESOURCE_DIRECTORY* pNextDir =(IMAGE_RESOURCE_DIRECTORY*)(pResDirEntry[i].OffsetToDirectory + dwResRootDirAddr);

			parseResourcesTable(dwResRootDirAddr, pNextDir, nDeep + 1);
			m_vecResourceInfo.push_back(resourceInfo);
		}
	}
	else if (nDeep == 3) {/*第三层*/

		// 第三层资源目录入口, 保存的是资源的数据入口
		// 解析偏移(偏移是一个指向数据入口的偏移)
		IMAGE_RESOURCE_DATA_ENTRY* pResDataEntry = 0;
		pResDataEntry =(IMAGE_RESOURCE_DATA_ENTRY*)(pResDirEntry->OffsetToData + dwResRootDirAddr);
		if (pResDirEntry->DataIsDirectory == 0) {
			resourceInfo.ResourceRVA = pResDataEntry->OffsetToData;
			resourceInfo.ResourceSize = pResDataEntry->Size;
		}
	}
}

//RVA转文件偏移
DWORD CLordPe::RVAToOffset(IMAGE_DOS_HEADER* pDos,
	DWORD dwRva)
{
	IMAGE_SECTION_HEADER* pScnHdr;

	IMAGE_NT_HEADERS* pNtHdr =
		(IMAGE_NT_HEADERS*)(pDos->e_lfanew + (DWORD)pDos);

	pScnHdr = IMAGE_FIRST_SECTION(pNtHdr);
	DWORD dwNumberOfScn = pNtHdr->FileHeader.NumberOfSections;

	// 1. 遍历所有区段找到所在区段
	for (int i = 0; i < dwNumberOfScn; ++i)
	{
		DWORD dwEndOfSection = pScnHdr[i].VirtualAddress + pScnHdr[i].SizeOfRawData;
		// 判断这个RVA是否在一个区段的范围内
		if (dwRva >= pScnHdr[i].VirtualAddress
			&& dwRva < dwEndOfSection)
		{
			// 2. 计算该RVA在区段内的偏移:rva 减去首地址
			DWORD dwOffset = dwRva - pScnHdr[i].VirtualAddress;
			// 3. 将区段内偏移加上区段的文件开始偏移
			return dwOffset + pScnHdr[i].PointerToRawData;
		}
	}
	return -1;
}