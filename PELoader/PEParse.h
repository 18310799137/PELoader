#pragma once
#include "stdafx.h"
extern IMAGE_DOS_HEADER* imageBase;
extern IMAGE_NT_HEADERS* ntHeader;
extern IMAGE_FILE_HEADER* fileHeader;
extern IMAGE_OPTIONAL_HEADER32*  opHeader;
extern IMAGE_SECTION_HEADER* sectionHeader;
void showPEParseField();

/*0读取文件,非0为保存文件*/
TCHAR* chooseFile(int operation);

void LoadFileToBuff(const wchar_t* filePath, char** buff, size_t* size);
INT_PTR CALLBACK LoadPEProc(HWND aboutHwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);


/*解析PE结构*/
void getPEAddress(char* fileBuff, IMAGE_NT_HEADERS** ntHeader,
	IMAGE_FILE_HEADER** fileHeader, IMAGE_OPTIONAL_HEADER32** opHeader,
	IMAGE_SECTION_HEADER** sectionHeader);