#pragma once
#include "stdafx.h"
extern HINSTANCE hinstance;
//Dialog句柄
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

/*将硬盘状态转换为内存状态 - 文件在内存中拉伸状态，将文件对齐   称为filebuffer -> imagebuffer*/
int _read_fbuff_to_ibuff(char* _f_buff, char** _i_buff)
{
	//节数量
	WORD sectionNum = fileHeader->NumberOfSections;
	//计算可选PE头大小
	WORD SizeOfOpeHead = fileHeader->SizeOfOptionalHeader;
	//内存对齐大小
	DWORD _mem_Alignment = opHeader->SectionAlignment;
	//程序内存镜像基址
	DWORD _image_base = opHeader->ImageBase;
	//程序内存中入口偏移地址
	DWORD _image_buffer_oep = opHeader->AddressOfEntryPoint;
	//计算_image_buffer所需内存大小
	DWORD _size_image = opHeader->SizeOfImage;


	//取出最后一个节区描述信息
	IMAGE_SECTION_HEADER _last_section = sectionHeader[sectionNum - 1];
	/*判断共需要几个内存对齐长度*/
	int _size_ = _last_section.SizeOfRawData / _mem_Alignment;
	_size_ = (_size_ + (_last_section.SizeOfRawData % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;


	// 所需内存拉伸长度 = 内存偏移+文件对齐长度 
	SIZE_T fileBufferSize = _last_section.VirtualAddress + _size_;

	//取出可选PE头中的 内存镜像大小，如果此值大于计算出的（最后节偏移+内存对齐）则使用此值做为 imagebuffer的大小
	fileBufferSize = _size_image > fileBufferSize ? _size_image : fileBufferSize;

	//创建内存镜像 填充可执行文件
	*_i_buff = new char[fileBufferSize];

	char* _temp_ibuff = *_i_buff;
	//初始化为0
	memset(_temp_ibuff, 0, fileBufferSize);

	//修改文件对齐为内存对齐
	opHeader->FileAlignment = sectionHeader[0].VirtualAddress;
	opHeader->SizeOfHeaders = sectionHeader[0].VirtualAddress;

	for (size_t i = 0; i < sectionNum; i++)
	{
		//取出节在内存中的偏移
		DWORD _vAddr = sectionHeader[i].VirtualAddress;
		DWORD _point_section = sectionHeader[i].PointerToRawData;
		DWORD _size_section = sectionHeader[i].SizeOfRawData;

		if (_size_section % _mem_Alignment != 0) {
			//按内存对齐 ，
			sectionHeader[i].SizeOfRawData = (_size_section / _mem_Alignment + (_size_section % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;
		}
		sectionHeader[i].PointerToRawData = _vAddr;
		//拷贝节区中 文件偏移和对齐后的大小
		_mem_copy(_f_buff + _point_section, _temp_ibuff + _vAddr, _size_section);
		//oep入口点
		if (_image_buffer_oep>sectionHeader[i].VirtualAddress && _image_buffer_oep < (sectionHeader[i].VirtualAddress + sectionHeader[i].SizeOfRawData))
		{
			printf("入口点OEP在第%d节中,名称为[%s],地址为%0X", i + 1, sectionHeader[i].Name, _image_buffer_oep);
		}
	}
	/*拷贝头部信息， 后面两种方式都可以
	1.取出可选PE头中的 header大小描述 ntHeader->OptionalHeader.SizeOfHeaders
	2.判断第一个节的文件偏移位置 sectionHeader[0].PointerToRawData
	*/

	DWORD  _headers_offset = sectionHeader[0].PointerToRawData;
	_mem_copy(_f_buff, _temp_ibuff, _headers_offset);

	return fileBufferSize;
}



/*
参数备注:
operation取值范围:0代表选择一个可执行程序,1代表填写一个文件名保存

*/
int chooseFile(wchar_t**retPath, int operation = 0, const wchar_t* title = L"文件名称")
{
loop:
	OPENFILENAME optionFile = { 0 };
	wchar_t  filePathBuffer[MAX_PATH] = { 0 };//用于接收文件名

	optionFile.lStructSize = sizeof(OPENFILENAME);//结构体大小                                                           
	optionFile.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄                                            
	optionFile.lpstrFilter = TEXT("选择文件*.*\0*.*\0可执行程序*.exe\0*.exe\0动态链接库文件*.dll\0*.dll\0\0 ");//设置过滤                                
	optionFile.nFilterIndex = 1;//过滤器索引                                                                             
	optionFile.lpstrFile = filePathBuffer;//接收返回的文件名，注意第一个字符需要为NULL                                                    
	optionFile.nMaxFile = sizeof(filePathBuffer);//缓冲区长度                                                               
	optionFile.lpstrInitialDir = NULL;//初始目录为默认                                                                     
	optionFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项                  
	if (operation) {
		optionFile.lpstrTitle = title;//使用系统默认标题留空即可 
		if (!GetSaveFileName(&optionFile))
		{
			return -2;
		}
	}
	else {
		optionFile.lpstrTitle = title;//使用系统默认标题留空即可
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




/*将拉伸后的内存状态 写入硬盘*/
int  _write_restore_to_file(int _ibuff_size, char* _i_buff, wchar_t** retSavePath)
{
	wchar_t* savePath=NULL;
	int retnCode = chooseFile(&savePath, 1, L"请选择保存路径");
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
	//刷新缓冲区
	fflush(fileWrite);

	fclose(fileWrite);
	printf("\n- save file over program will exit! ");
	Sleep(3000);
	return writeSize;
}




/*新增一个节,参数为  _file_buff-文件buffer name-节的名称 virtualSize-文件中对齐前的大小          参数_add_section_file_size新增节后的文件大小*/
char* add_section(char * _f_buff, int _file_buff_size, const char * name, int virtualSize, int* _add_section_file_size)
{

	_IMAGE_DOS_HEADER* _dos_header = (_IMAGE_DOS_HEADER*)_f_buff;
	//读出PE存放位置偏移
	DWORD _pe_sign_offset = _dos_header->e_lfanew;

	_IMAGE_NT_HEADERS* _nt_header = (_IMAGE_NT_HEADERS *)(_f_buff + _pe_sign_offset);
	char* secStart = _f_buff + _dos_header->e_lfanew;
	DWORD secSize = sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + _nt_header->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_SECTION_HEADER)*(_nt_header->FileHeader.NumberOfSections);
	memcpy(_f_buff + sizeof(IMAGE_DOS_HEADER), secStart, secSize);

	_dos_header->e_lfanew = sizeof(_IMAGE_DOS_HEADER);
	_nt_header = (_IMAGE_NT_HEADERS *)(_f_buff + _dos_header->e_lfanew);


	//节数量
	WORD sectionNum = _nt_header->FileHeader.NumberOfSections;

	//计算可选PE头大小
	WORD SizeOfOpeHead = _nt_header->FileHeader.SizeOfOptionalHeader;


	//内存对齐大小
	DWORD	_mem_Alignment = _nt_header->OptionalHeader.SectionAlignment;
	//程序内存镜像基址
	DWORD	_image_base = _nt_header->OptionalHeader.ImageBase;
	//程序内存中入口偏移地址
	DWORD	_image_buffer_oep = _nt_header->OptionalHeader.AddressOfEntryPoint;
	//计算_image_buffer所需内存大小
	DWORD	_size_image = _nt_header->OptionalHeader.SizeOfImage;
	//获取所有头部描述信息大小
	DWORD	_headers = _nt_header->OptionalHeader.SizeOfHeaders;
	DWORD	_file_Alignment = _nt_header->OptionalHeader.FileAlignment;


	//计算标准PE头大小
	size_t fSize = sizeof(_nt_header->FileHeader);
	//节数组首地址 = PE标记+标准PE头大小+可选PE头大小
	char* sHeaderAddr = ((char*)(_nt_header)) + fSize + SizeOfOpeHead + sizeof(_nt_header->Signature);

	//转换为指向节数组的 结构体指针
	_IMAGE_SECTION_HEADER* _section_header = (_IMAGE_SECTION_HEADER*)sHeaderAddr;


	/*判断共需要几个内存对齐长度*/
	int _add_section_need_size_ = virtualSize / _file_Alignment;
	_add_section_need_size_ = (_add_section_need_size_ + (virtualSize % _file_Alignment == 0 ? 0 : 1))*_file_Alignment;

	//开辟一个新的空间 所需的大小
	*_add_section_file_size = _file_buff_size + _add_section_need_size_;

	//开辟一个新的空间
	char* _file_buff = new char[*_add_section_file_size];
	memset(_file_buff, 0, *_add_section_file_size);


	//计算新增节在内存中所需大小
	int _add_section_need_virtual_size_ = virtualSize / _mem_Alignment;
	_add_section_need_virtual_size_ = (_add_section_need_virtual_size_ + (virtualSize % _mem_Alignment == 0 ? 0 : 1))*_mem_Alignment;
	//修改sizeofimage 为原先大小+新节的大小
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

	//保存数据dataBuff大小
	size_t  size;
	//加载data到文件buff
	char* dataBuff = NULL;
	//加载文件到内存
	LoadFileToBuff(dataPathBuff, &dataBuff, &size);

	//解析PE结构
	getPEAddress(dataBuff, &ntHeader, &fileHeader, &opHeader, &sectionHeader);

	//文件内存拉伸
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

	//返回shell程序的大小
	size_t  shellSize = 0;
	//加载data到文件buff
	char* shellBuff = NULL;
	//加载文件到内存
	LoadFileToBuff(shellPathBuff, &shellBuff, &shellSize);
	int addSecSize = 0;
	shellBuff = add_section(shellBuff, shellSize, ".myShell", imageSize, &addSecSize);

	memcpy(shellBuff + shellSize, dataImgBuff, imageSize);


	wchar_t* savePath=NULL;
	//写入硬盘
	int retSize = _write_restore_to_file(addSecSize, shellBuff, &savePath);
	if (retSize == addSecSize) 
	{
		wchar_t* filePath = (wchar_t*)malloc(MAX_PATH + 20);
		wsprintf(filePath, L"文件路径:%s!\n", savePath);
		MessageBoxW(ShellHwnd, filePath, L"文件保存完成!", MB_OK);
	}
	else if(retSize==-1)
	{
		MessageBoxW(ShellHwnd, L"文件打开失败.写入未完成!", L"文件写入提醒!", MB_ICONERROR);
	}
	else if(retSize == -2)
	{
		//用户取消了选择
	}
	else {
		MessageBoxW(ShellHwnd, L"未知原因.文件写入失败!", L"文件写入提醒!", MB_ICONERROR);
	}
	return ;
	
}
/*关于按钮 消息处理函数*/
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
		//标准组件消息通知
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
				MessageBoxA(ShellProtectedHwnd,"请选择源文件","文件选择提醒",MB_ICONWARNING);
				break;
			}
			ZeroMemory(path, MAX_PATH);
			GetWindowTextW(shellEdit, path, MAX_PATH);
			if (!wcscmp(path, L""))
			{
				MessageBoxA(ShellProtectedHwnd, "请选择壳文件", "文件选择提醒", MB_ICONWARNING);
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