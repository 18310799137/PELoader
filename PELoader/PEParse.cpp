#include "stdafx.h"
#include <stdio.h>
//创建实例句柄
extern HINSTANCE hinstance;

//Dialog句柄
extern HWND MainDialogHwnd;
IMAGE_DOS_HEADER* imageBase = NULL;
IMAGE_NT_HEADERS* ntHeader = NULL;
IMAGE_FILE_HEADER* fileHeader = NULL;
IMAGE_OPTIONAL_HEADER32*  opHeader = NULL;
IMAGE_SECTION_HEADER* sectionHeader = NULL;

//PE对话框句柄
HWND peHwnd=NULL;

/*加载文件到内存*/
void LoadFileToBuff(const wchar_t* filePath, char** buff, size_t* size)
{
	//初始化文件指针，读取exe路径
	FILE *fileRead=NULL;
	int readAble = _wfopen_s(&fileRead, filePath, L"rb");

	if (readAble) {
		OutputDebugStringF(" - file stream not open !");
		*buff = NULL;
		return;
	}
	wprintf(L"_file_name: - %s\n", filePath);
	//获取文件大小
	fseek(fileRead, 0, SEEK_END); //定位到文件末
	long fileSize = ftell(fileRead);
	*size = fileSize;
	fseek(fileRead, 0, SEEK_SET);
	//创建文件大小的数组
	char* _f_buff = new char[fileSize];
	*buff = _f_buff;
	fread(_f_buff, fileSize, 1, fileRead);
	//关闭 读入和写出流
	fclose(fileRead);
}
/*0读取文件,非0为保存文件*/
TCHAR* chooseFile(int operation)
{
loop:
	OPENFILENAME optionFile = { 0 };
	TCHAR filePathBuffer[MAX_PATH] = { 0 };//用于接收文件名                                                            
	optionFile.lStructSize = sizeof(OPENFILENAME);//结构体大小                                                           
	optionFile.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄                                            
	optionFile.lpstrFilter = TEXT("选择文件*.*\0*.*\0可执行程序*.exe\0*.exe\0动态链接库文件*.dll\0*.dll\0\0 ");//设置过滤                                
	optionFile.nFilterIndex = 1;//过滤器索引                                                                             
	optionFile.lpstrFile = filePathBuffer;//接收返回的文件名，注意第一个字符需要为NULL                                                    
	optionFile.nMaxFile = sizeof(filePathBuffer);//缓冲区长度                                                               
	optionFile.lpstrInitialDir = NULL;//初始目录为默认                                                                     
	optionFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项                  

	if (operation) {
		optionFile.lpstrTitle = TEXT("请填写一个文件名称");//使用系统默认标题留空即可 
		if (!GetSaveFileName(&optionFile))
		{
			return NULL;
		}
	}
	else {
		optionFile.lpstrTitle = TEXT("请选择一个可执行程序");//使用系统默认标题留空即可
		bool retn = GetOpenFileNameW(&optionFile);
		if (!retn)
		{
			return NULL;
		}
	}
	return filePathBuffer;
}



/*解析PE结构*/
void getPEAddress(char* fileBuff, IMAGE_NT_HEADERS** ntHeader,
	IMAGE_FILE_HEADER** fileHeader, IMAGE_OPTIONAL_HEADER32** opHeader,
	IMAGE_SECTION_HEADER** sectionHeader)
{
	imageBase = (IMAGE_DOS_HEADER*)fileBuff;
	*ntHeader = (IMAGE_NT_HEADERS*)(fileBuff + imageBase->e_lfanew);
	*fileHeader = &((*ntHeader)->FileHeader);
	*opHeader = &((*ntHeader)->OptionalHeader);
	char* sect = ((char*)(&(**fileHeader)) + (*fileHeader)->SizeOfOptionalHeader) + sizeof(IMAGE_FILE_HEADER);
	*sectionHeader = (IMAGE_SECTION_HEADER*)sect;
}
void initPEFiled()
{

	typedef struct _IMAGE_FILE_HEADER {
		WORD    Machine;
		WORD    NumberOfSections;
		DWORD   TimeDateStamp;
		DWORD   PointerToSymbolTable;
		DWORD   NumberOfSymbols;
		WORD    SizeOfOptionalHeader;
		WORD    Characteristics;
	} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

	char* field =(char*)malloc(20);
	sprintf(field, "%x", fileHeader->Machine);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->NumberOfSections);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->TimeDateStamp);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->PointerToSymbolTable);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->NumberOfSymbols);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->SizeOfOptionalHeader);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->Characteristics);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->SizeOfOptionalHeader);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);

	sprintf(field, "%x", fileHeader->SizeOfOptionalHeader);
	SetWindowTextA(GetDlgItem(peHwnd, IDC_Machine), field);


}


/*关于按钮 消息处理函数*/
INT_PTR CALLBACK LoadPEProc(HWND loadPEHwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		peHwnd = loadPEHwnd;
		//初始化PE字段
		initPEFiled();
		break;
	case WM_CLOSE:
		EndDialog(loadPEHwnd, NULL);
		break;
		//标准组件消息通知
	case WM_COMMAND:
		if (LOWORD(wParam) == SectionButton)
		{
			
			break;
		}
		if (LOWORD(wParam) == TableButton)
		{
			
			break;
		}
		if (LOWORD(wParam) == ClosePEButton)
		{
			EndDialog(loadPEHwnd, NULL);
			break;
		}
	default:
		return false;
	}
	return true;
}
void showPEParseField()
{
	WCHAR* filePath = (WCHAR*)malloc(MAX_PATH);
	if (NULL == filePath)
	{
		return;
	}
	wcscpy(filePath, chooseFile(0));
	//保存数据dataBuff大小
	size_t  size;
	//加载data到文件buff
	char* dataBuff = NULL;
	//加载文件内存镜像
	LoadFileToBuff(filePath, &dataBuff, &size);
	//解析PE结构
	getPEAddress(dataBuff, &ntHeader, &fileHeader, &opHeader, &sectionHeader);
	//显示PE字段 对话框
	DialogBoxW(hinstance, MAKEINTRESOURCE(PEParseDialog), MainDialogHwnd, LoadPEProc);
}