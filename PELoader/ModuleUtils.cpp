#include "stdafx.h"
//创建实例句柄
extern HINSTANCE hinstance;
//Dialog句柄
extern HWND MainDialogHwnd;
//创建模块moduleHwnd窗体句柄
HWND moduleHwnd = NULL;

CONST WCHAR* moduleColumnList[] = { L"NO.", L"Module NAME ",L"Module Address" };
BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}
VOID listModule(int  processId)
{

	//清空模块中的原数据
	ListView_DeleteAllItems(moduleHwnd);
	//定义一个行的结构体
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	
	//定义行的下标为0
	int rowIndex = 0;
	//定义数组 存储模块句柄
	HMODULE hModule[1024] = { 0 };
	DWORD dwRes=0;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);  //获取的是当前进程ID
	if (hProcess == NULL) {
		EnableDebugPrivilege();
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
	}
		/*LIST_MODULES_32BIT    List the 32 - bit modules.
		LIST_MODULES_64BIT   List the 64 - bit modules.
		LIST_MODULES_ALL  List all modules.
		LIST_MODULES_DEFAULT  Use the de
		*/
	EnumProcessModulesEx(hProcess, hModule, sizeof(hModule), &dwRes, LIST_MODULES_64BIT);
	//EnumProcessModules(hProcess, hModule, sizeof(hModule), &dwRes);
	for (int i = 0; i < (dwRes / sizeof(HMODULE)); i++)
	{
		WCHAR wzName[MAX_PATH] = { 0 };
		if (GetModuleFileNameEx(hProcess, hModule[i], wzName, sizeof(wzName) / sizeof(TCHAR)))
		{
			 wchar_t NO[5] = { 0 };
			 wsprintf(NO, _T("%d"), i);
			//设置插入的行下标
			lvItem.iItem = rowIndex++;
			//第一列存储序号
			lvItem.pszText = NO;
			lvItem.iSubItem = 0;
			SendMessage(moduleHwnd, LVM_INSERTITEM, 0, (LONG)(&lvItem));

			
			//第二列存储模块名称
			lvItem.pszText = wzName;
			lvItem.iSubItem = 1;
			ListView_SetItem(moduleHwnd, &lvItem);


			//第三列存模块起始地址
			TCHAR modleAddr[10] = { 0 };
			wsprintf(modleAddr, TEXT("%x"), hModule[i]);
			
			lvItem.pszText = modleAddr;
			lvItem.iSubItem = 2;
			ListView_SetItem(moduleHwnd, &lvItem);

		}
	}
	CloseHandle(hProcess);
}
/**初始化列标题*/
void initListModule()
{
	//定义插入列的属性
	LV_COLUMN lvcolumn = { 0 };
	lvcolumn.mask = LVCF_TEXT | LVCF_WIDTH;
	int colCX = 0;
	//循环插入列表
	for (size_t i = 0; i < 3; i++)
	{
		//设置列的标题文本
		lvcolumn.pszText = (LPWSTR)moduleColumnList[i];
		//定义列的长度 ,第一列100,其余350
		if (i == 0) {
			colCX = 130;
		}
		else if (i == 1) {
			colCX = 510;
		}
		else {
			colCX = 130;
		}
		//设置标题列的长度
		lvcolumn.cx = colCX;
		//发送窗口消息 添加列
		SendMessage(moduleHwnd, LVM_INSERTCOLUMN, i, (LONG_PTR)(&lvcolumn));
	}

	DWORD dwExStyle =
		LVS_EX_FULLROWSELECT |  //设置可选整行
		LVS_EX_GRIDLINES |  //listView的item的每个栏目之间添加分割线
		LVS_EX_HEADERDRAGDROP |//允许litview的目录通过拖放重排序
		LVS_EDITLABELS;
	//设置ListControl样式
	ListView_SetExtendedListViewStyle(moduleHwnd, dwExStyle);
	//设置整行选中 ,同上行代码有相同作用
	//SendMessage(processHwnd,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES | LVS_EX_GRIDLINES| LVS_REPORT| GWL_STYLE, LVS_EX_FULLROWSELECT);
	
}