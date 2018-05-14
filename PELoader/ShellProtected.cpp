#pragma once
#include "stdafx.h"
extern HINSTANCE hinstance;
//Dialog���
extern HWND MainDialogHwnd;
WCHAR* filePath = NULL;
HWND sourceEdit = NULL;
HWND shellEdit = NULL;

WCHAR path[MAX_PATH] = { 0 };

HWND ShellHwnd = NULL;
void _mem_copy(char* source, char* target, int size) {
	char* temp_source = source;
	char* temp_target = target;

	int i = 0;
	while (i < size) {
		*temp_target = *temp_source;
		temp_target++;
		temp_source++;
		i++;
	}
}

/*��Ӳ��״̬ת��Ϊ�ڴ�״̬ - �ļ����ڴ�������״̬�����ļ�����   ��Ϊfilebuffer -> imagebuffer*/
int _read_fbuff_to_ibuff(char* _f_buff, char** _i_buff)
{
	//������
	WORD sectionNum = fileHeader->NumberOfSections;
	//�����ѡPEͷ��С
	WORD SizeOfOpeHead = fileHeader->SizeOfOptionalHeader;
	//�ڴ�����С
	DWORD _mem_Alignment = opHeader->SectionAlignment;
	//�����ڴ澵���ַ
	DWORD _image_base = opHeader->ImageBase;
	//�����ڴ������ƫ�Ƶ�ַ
	DWORD _image_buffer_oep = opHeader->AddressOfEntryPoint;
	//����_image_buffer�����ڴ��С
	DWORD _size_image = opHeader->SizeOfImage;


	//ȡ�����һ������������Ϣ
	IMAGE_SECTION_HEADER _last_section = sectionHeader[sectionNum - 1];
	/*�жϹ���Ҫ�����ڴ���볤��*/
	int _size_ = _last_section.SizeOfRawData / _mem_Alignment;
	_size_ = (_size_ + (_last_section.SizeOfRawData % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;


	// �����ڴ����쳤�� = �ڴ�ƫ��+�ļ����볤�� 
	SIZE_T fileBufferSize = _last_section.VirtualAddress + _size_;

	//ȡ����ѡPEͷ�е� �ڴ澵���С�������ֵ���ڼ�����ģ�����ƫ��+�ڴ���룩��ʹ�ô�ֵ��Ϊ imagebuffer�Ĵ�С
	fileBufferSize = _size_image > fileBufferSize ? _size_image : fileBufferSize;

	//�����ڴ澵�� ����ִ���ļ�
	*_i_buff = new char[fileBufferSize];

	char* _temp_ibuff = *_i_buff;
	//��ʼ��Ϊ0
	memset(_temp_ibuff, 0, fileBufferSize);

	//�޸��ļ�����Ϊ�ڴ����
	opHeader->FileAlignment = sectionHeader[0].VirtualAddress;
	opHeader->SizeOfHeaders = sectionHeader[0].VirtualAddress;

	for (size_t i = 0; i < sectionNum; i++)
	{
		//ȡ�������ڴ��е�ƫ��
		DWORD _vAddr = sectionHeader[i].VirtualAddress;
		DWORD _point_section = sectionHeader[i].PointerToRawData;
		DWORD _size_section = sectionHeader[i].SizeOfRawData;

		if (_size_section % _mem_Alignment != 0) {
			//���ڴ���� ��
			sectionHeader[i].SizeOfRawData = (_size_section / _mem_Alignment + (_size_section % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;
		}
		sectionHeader[i].PointerToRawData = _vAddr;
		//���������� �ļ�ƫ�ƺͶ����Ĵ�С
		_mem_copy(_f_buff + _point_section, _temp_ibuff + _vAddr, _size_section);
		//oep��ڵ�
		if (_image_buffer_oep>sectionHeader[i].VirtualAddress && _image_buffer_oep < (sectionHeader[i].VirtualAddress + sectionHeader[i].SizeOfRawData))
		{
			printf("��ڵ�OEP�ڵ�%d����,����Ϊ[%s],��ַΪ%0X", i + 1, sectionHeader[i].Name, _image_buffer_oep);
		}
	}
	/*����ͷ����Ϣ�� �������ַ�ʽ������
	1.ȡ����ѡPEͷ�е� header��С���� ntHeader->OptionalHeader.SizeOfHeaders
	2.�жϵ�һ���ڵ��ļ�ƫ��λ�� sectionHeader[0].PointerToRawData
	*/

	DWORD  _headers_offset = sectionHeader[0].PointerToRawData;
	_mem_copy(_f_buff, _temp_ibuff, _headers_offset);

	return fileBufferSize;
}



/*
������ע:
operationȡֵ��Χ:0����ѡ��һ����ִ�г���,1������дһ���ļ�������

*/
int chooseFile(wchar_t**retPath, int operation = 0, const wchar_t* title = L"�ļ�����")
{
loop:
	OPENFILENAME optionFile = { 0 };
	wchar_t  filePathBuffer[MAX_PATH] = { 0 };//���ڽ����ļ���

	optionFile.lStructSize = sizeof(OPENFILENAME);//�ṹ���С                                                           
	optionFile.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������                                            
	optionFile.lpstrFilter = TEXT("ѡ���ļ�*.*\0*.*\0��ִ�г���*.exe\0*.exe\0��̬���ӿ��ļ�*.dll\0*.dll\0\0 ");//���ù���                                
	optionFile.nFilterIndex = 1;//����������                                                                             
	optionFile.lpstrFile = filePathBuffer;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL                                                    
	optionFile.nMaxFile = sizeof(filePathBuffer);//����������                                                               
	optionFile.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��                                                                     
	optionFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��                  
	if (operation) {
		optionFile.lpstrTitle = title;//ʹ��ϵͳĬ�ϱ������ռ��� 
		if (!GetSaveFileName(&optionFile))
		{
			return -2;
		}
	}
	else {
		optionFile.lpstrTitle = title;//ʹ��ϵͳĬ�ϱ������ռ���
		if (!GetOpenFileName(&optionFile))
		{
			return -2;
		}
	}
	wchar_t * filePath = new wchar_t[MAX_PATH];
	*retPath = filePath;
	ZeroMemory(filePath, MAX_PATH);
	wchar_t * temp = filePathBuffer;
	while (*temp != '\0\0')
	{
		*filePath = *temp;
		filePath++;
		temp++;
	}
	return 0;
}




/*���������ڴ�״̬ д��Ӳ��*/
int  _write_restore_to_file(int _ibuff_size, char* _i_buff, wchar_t** retSavePath)
{
	wchar_t* savePath=NULL;
	int retnCode = chooseFile(&savePath, 1, L"��ѡ�񱣴�·��");
	if(retnCode)
	{
		return retnCode;
	}
	*retSavePath = savePath;
	wprintf(L"\nwill save file to: \n-> %s \n", savePath);

	FILE* fileWrite;
	int writeAble = _wfopen_s(&fileWrite, savePath, L"wb+");
	if (writeAble) {
		printf("\n- get write stream fail!");
		return -1;
	}
	int writeSize =	fwrite(_i_buff, sizeof(char), _ibuff_size, fileWrite);
	//ˢ�»�����
	fflush(fileWrite);

	fclose(fileWrite);
	printf("\n- save file over program will exit! ");
	Sleep(3000);
	return writeSize;
}




/*����һ����,����Ϊ  _file_buff-�ļ�buffer name-�ڵ����� virtualSize-�ļ��ж���ǰ�Ĵ�С          ����_add_section_file_size�����ں���ļ���С*/
char* add_section(char * _f_buff, int _file_buff_size, const char * name, int virtualSize, int* _add_section_file_size)
{

	_IMAGE_DOS_HEADER* _dos_header = (_IMAGE_DOS_HEADER*)_f_buff;
	//����PE���λ��ƫ��
	DWORD _pe_sign_offset = _dos_header->e_lfanew;

	_IMAGE_NT_HEADERS* _nt_header = (_IMAGE_NT_HEADERS *)(_f_buff + _pe_sign_offset);
	char* secStart = _f_buff + _dos_header->e_lfanew;
	DWORD secSize = sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + _nt_header->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_SECTION_HEADER)*(_nt_header->FileHeader.NumberOfSections);
	memcpy(_f_buff + sizeof(IMAGE_DOS_HEADER), secStart, secSize);

	_dos_header->e_lfanew = sizeof(_IMAGE_DOS_HEADER);
	_nt_header = (_IMAGE_NT_HEADERS *)(_f_buff + _dos_header->e_lfanew);


	//������
	WORD sectionNum = _nt_header->FileHeader.NumberOfSections;

	//�����ѡPEͷ��С
	WORD SizeOfOpeHead = _nt_header->FileHeader.SizeOfOptionalHeader;


	//�ڴ�����С
	DWORD	_mem_Alignment = _nt_header->OptionalHeader.SectionAlignment;
	//�����ڴ澵���ַ
	DWORD	_image_base = _nt_header->OptionalHeader.ImageBase;
	//�����ڴ������ƫ�Ƶ�ַ
	DWORD	_image_buffer_oep = _nt_header->OptionalHeader.AddressOfEntryPoint;
	//����_image_buffer�����ڴ��С
	DWORD	_size_image = _nt_header->OptionalHeader.SizeOfImage;
	//��ȡ����ͷ��������Ϣ��С
	DWORD	_headers = _nt_header->OptionalHeader.SizeOfHeaders;
	DWORD	_file_Alignment = _nt_header->OptionalHeader.FileAlignment;


	//�����׼PEͷ��С
	size_t fSize = sizeof(_nt_header->FileHeader);
	//�������׵�ַ = PE���+��׼PEͷ��С+��ѡPEͷ��С
	char* sHeaderAddr = ((char*)(_nt_header)) + fSize + SizeOfOpeHead + sizeof(_nt_header->Signature);

	//ת��Ϊָ�������� �ṹ��ָ��
	_IMAGE_SECTION_HEADER* _section_header = (_IMAGE_SECTION_HEADER*)sHeaderAddr;


	/*�жϹ���Ҫ�����ڴ���볤��*/
	int _add_section_need_size_ = virtualSize / _file_Alignment;
	_add_section_need_size_ = (_add_section_need_size_ + (virtualSize % _file_Alignment == 0 ? 0 : 1))*_file_Alignment;

	//����һ���µĿռ� ����Ĵ�С
	*_add_section_file_size = _file_buff_size + _add_section_need_size_;

	//����һ���µĿռ�
	char* _file_buff = new char[*_add_section_file_size];
	memset(_file_buff, 0, *_add_section_file_size);


	//�������������ڴ��������С
	int _add_section_need_virtual_size_ = virtualSize / _mem_Alignment;
	_add_section_need_virtual_size_ = (_add_section_need_virtual_size_ + (virtualSize % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;
	//�޸�sizeofimage Ϊԭ�ȴ�С+�½ڵĴ�С
	_nt_header->OptionalHeader.SizeOfImage = _size_image + _add_section_need_virtual_size_;



	IMAGE_SECTION_HEADER  _last_section = _section_header[sectionNum - 1];

	_section_header[sectionNum] = _section_header[0];
	_section_header[sectionNum].Characteristics = 0x80000020;
	_section_header[sectionNum].Misc.VirtualSize = virtualSize;
	_section_header[sectionNum].PointerToRawData = _file_buff_size;
	_section_header[sectionNum].SizeOfRawData = _add_section_need_size_;
	DWORD _previous_section_mem_size = (_last_section.SizeOfRawData / _mem_Alignment + (_last_section.SizeOfRawData % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;

	_section_header[sectionNum].VirtualAddress = _last_section.VirtualAddress + _previous_section_mem_size;

	_mem_copy((char*)name, (char*)(&_section_header[sectionNum].Name), IMAGE_SIZEOF_SHORT_NAME);
	_nt_header->FileHeader.NumberOfSections = (++sectionNum);
	_mem_copy(_f_buff, _file_buff, _file_buff_size);
	return _file_buff;
}
 



 

void AddProtectedShellFun()
{

	wchar_t dataPathBuff[MAX_PATH];
	GetWindowText(sourceEdit, dataPathBuff, MAX_PATH);

	//��������dataBuff��С
	size_t  size;
	//����data���ļ�buff
	char* dataBuff = NULL;
	//�����ļ����ڴ�
	LoadFileToBuff(dataPathBuff, &dataBuff, &size);

	//����PE�ṹ
	getPEAddress(dataBuff, &ntHeader, &fileHeader, &opHeader, &sectionHeader);

	//�ļ��ڴ�����
	char* dataImgBuff = NULL;
	int imageSize = _read_fbuff_to_ibuff(dataBuff, &dataImgBuff);
	char* encrypt = dataImgBuff;
	int i = 0;
	while (i<imageSize)
	{
		*encrypt = ~(*encrypt);
		encrypt++;
		i++;
	}
	wchar_t shellPathBuff[MAX_PATH];
	GetWindowText(shellEdit, shellPathBuff, MAX_PATH);

	//����shell����Ĵ�С
	size_t  shellSize = 0;
	//����data���ļ�buff
	char* shellBuff = NULL;
	//�����ļ����ڴ�
	LoadFileToBuff(shellPathBuff, &shellBuff, &shellSize);
	int addSecSize = 0;
	shellBuff = add_section(shellBuff, shellSize, ".myShell", imageSize, &addSecSize);

	memcpy(shellBuff + shellSize, dataImgBuff, imageSize);


	wchar_t* savePath=NULL;
	//д��Ӳ��
	int retSize = _write_restore_to_file(addSecSize, shellBuff, &savePath);
	if (retSize == addSecSize) 
	{
		wchar_t* filePath = (wchar_t*)malloc(MAX_PATH + 20);
		wsprintf(filePath, L"�ļ�·��:%s!\n", savePath);
		MessageBoxW(ShellHwnd, filePath, L"�ļ��������!", MB_OK);
	}
	else if(retSize==-1)
	{
		MessageBoxW(ShellHwnd, L"�ļ���ʧ��.д��δ���!", L"�ļ�д������!", MB_ICONERROR);
	}
	else if(retSize == -2)
	{
		//�û�ȡ����ѡ��
	}
	else {
		MessageBoxW(ShellHwnd, L"δ֪ԭ��.�ļ�д��ʧ��!", L"�ļ�д������!", MB_ICONERROR);
	}
	return ;
	
}
/*���ڰ�ť ��Ϣ������*/
INT_PTR CALLBACK ShellDialogProc(HWND ShellProtectedHwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		sourceEdit = GetDlgItem(ShellProtectedHwnd, ShowSourceFileEdit);
		shellEdit = GetDlgItem(ShellProtectedHwnd, ShowShellFileEdit);
		ShellHwnd = ShellProtectedHwnd;
		break;
	case WM_CLOSE:
		EndDialog(ShellProtectedHwnd, NULL);
		break;
		//��׼�����Ϣ֪ͨ
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case CancleShellProtectedButton:
			EndDialog(ShellProtectedHwnd, NULL);
			break;
		case SelectSourceFileButton:
				filePath = chooseFile(0);
				if(NULL!=filePath)
				{
					ZeroMemory(path, MAX_PATH);
					wcscpy(path, filePath);
					SetWindowText(sourceEdit, path);
				}
			break;
		case SelectShellFileButton:
				filePath = chooseFile(0);
				if (NULL != filePath)
				{
					ZeroMemory(path, MAX_PATH);
					wcscpy(path, filePath);
					SetWindowText(shellEdit, path);
				}
			break;
		case AddProtectedShellButton:
			ZeroMemory(path, MAX_PATH);
			GetWindowTextW(sourceEdit, path,MAX_PATH);
			if (!wcscmp(path,L""))
			{
				MessageBoxA(ShellProtectedHwnd,"��ѡ��Դ�ļ�","�ļ�ѡ������",MB_ICONWARNING);
				break;
			}
			ZeroMemory(path, MAX_PATH);
			GetWindowTextW(shellEdit, path, MAX_PATH);
			if (!wcscmp(path, L""))
			{
				MessageBoxA(ShellProtectedHwnd, "��ѡ����ļ�", "�ļ�ѡ������", MB_ICONWARNING);
				break;
			}
			AddProtectedShellFun();
			break;
		default:
			break;
		}
		break;
	default:
		return false;
	}
	return true;
}
void disposeShellProtectedMessage()
{
	DialogBox(hinstance, MAKEINTRESOURCE(ShellProtectedDialog), MainDialogHwnd, ShellDialogProc);
}