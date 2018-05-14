#include "stdafx.h"
#include <stdio.h>
//����ʵ�����
extern HINSTANCE hinstance;

//Dialog���
extern HWND MainDialogHwnd;
IMAGE_DOS_HEADER* imageBase = NULL;
IMAGE_NT_HEADERS* ntHeader = NULL;
IMAGE_FILE_HEADER* fileHeader = NULL;
IMAGE_OPTIONAL_HEADER32*  opHeader = NULL;
IMAGE_SECTION_HEADER* sectionHeader = NULL;

//PE�Ի�����
HWND peHwnd=NULL;

/*�����ļ����ڴ�*/
void LoadFileToBuff(const wchar_t* filePath, char** buff, size_t* size)
{
	//��ʼ���ļ�ָ�룬��ȡexe·��
	FILE *fileRead=NULL;
	int readAble = _wfopen_s(&fileRead, filePath, L"rb");

	if (readAble) {
		OutputDebugStringF(" - file stream not open !");
		*buff = NULL;
		return;
	}
	wprintf(L"_file_name: - %s\n", filePath);
	//��ȡ�ļ���С
	fseek(fileRead, 0, SEEK_END); //��λ���ļ�ĩ
	long fileSize = ftell(fileRead);
	*size = fileSize;
	fseek(fileRead, 0, SEEK_SET);
	//�����ļ���С������
	char* _f_buff = new char[fileSize];
	*buff = _f_buff;
	fread(_f_buff, fileSize, 1, fileRead);
	//�ر� �����д����
	fclose(fileRead);
}
/*0��ȡ�ļ�,��0Ϊ�����ļ�*/
TCHAR* chooseFile(int operation)
{
loop:
	OPENFILENAME optionFile = { 0 };
	TCHAR filePathBuffer[MAX_PATH] = { 0 };//���ڽ����ļ���                                                            
	optionFile.lStructSize = sizeof(OPENFILENAME);//�ṹ���С                                                           
	optionFile.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������                                            
	optionFile.lpstrFilter = TEXT("ѡ���ļ�*.*\0*.*\0��ִ�г���*.exe\0*.exe\0��̬���ӿ��ļ�*.dll\0*.dll\0\0 ");//���ù���                                
	optionFile.nFilterIndex = 1;//����������                                                                             
	optionFile.lpstrFile = filePathBuffer;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL                                                    
	optionFile.nMaxFile = sizeof(filePathBuffer);//����������                                                               
	optionFile.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��                                                                     
	optionFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��                  

	if (operation) {
		optionFile.lpstrTitle = TEXT("����дһ���ļ�����");//ʹ��ϵͳĬ�ϱ������ռ��� 
		if (!GetSaveFileName(&optionFile))
		{
			return NULL;
		}
	}
	else {
		optionFile.lpstrTitle = TEXT("��ѡ��һ����ִ�г���");//ʹ��ϵͳĬ�ϱ������ռ���
		bool retn = GetOpenFileNameW(&optionFile);
		if (!retn)
		{
			return NULL;
		}
	}
	return filePathBuffer;
}



/*����PE�ṹ*/
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


/*���ڰ�ť ��Ϣ������*/
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
		//��ʼ��PE�ֶ�
		initPEFiled();
		break;
	case WM_CLOSE:
		EndDialog(loadPEHwnd, NULL);
		break;
		//��׼�����Ϣ֪ͨ
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
	//��������dataBuff��С
	size_t  size;
	//����data���ļ�buff
	char* dataBuff = NULL;
	//�����ļ��ڴ澵��
	LoadFileToBuff(filePath, &dataBuff, &size);
	//����PE�ṹ
	getPEAddress(dataBuff, &ntHeader, &fileHeader, &opHeader, &sectionHeader);
	//��ʾPE�ֶ� �Ի���
	DialogBoxW(hinstance, MAKEINTRESOURCE(PEParseDialog), MainDialogHwnd, LoadPEProc);
}