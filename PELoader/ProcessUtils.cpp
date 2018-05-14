#include "stdafx.h"


//创建实例句柄
extern HINSTANCE hinstance;
//Dialog句柄
extern HWND MainDialogHwnd;
//创建进程模块processList句柄
HWND processHwnd = NULL;
////创建外部变量模块ModuleList窗体句柄
//extern HWND moduleHwnd;

CONST WCHAR* processColumnList[] = { L"PROCESS NAME ",L"ID",L"镜像基址",L"镜像基址" };
/**获取进程列表信息*/
void listProcess()
{
	int countProcess = 0;                                 //当前进程数量计数变量  
	PROCESSENTRY32 currentProcess;                      //存放快照进程信息的一个结构体  
	currentProcess.dwSize = sizeof(PROCESSENTRY32);     //在使用这个结构之前，先设置它的大小  
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//给系统内的所有进程拍一个快照
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		OutputDebugW(L"CreateToolhelp32Snapshot()调用失败!\n");
		return;
	}
	//定义一个行的结构体
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	//获取第一个进程信息  
	bool bMore = Process32First(hProcess, &currentProcess);
	//定义行的下标为0
	int rowIndex = 0;
	while (bMore)
	{
		lvItem.pszText = currentProcess.szExeFile;
		lvItem.iItem = rowIndex++;
		lvItem.iSubItem = 0;
		SendMessage(processHwnd, LVM_INSERTITEM, 0, (LONG)(&lvItem));
		TCHAR processID[10] = { 0 };
		//将DWORD的ID转化为字符串展示
		wsprintf(processID, TEXT("%d"), currentProcess.th32ProcessID);
		//第二列存放进程ID
		lvItem.pszText = processID;
		lvItem.iSubItem = 1;
		ListView_SetItem(processHwnd, &lvItem);

		OutputDebugW(L"PID=%d    PName= %s\n", currentProcess.th32ProcessID, currentProcess.szExeFile); //遍历进程快照，轮流显示每个进程信息  
		bMore = Process32Next(hProcess, &currentProcess);    //遍历下一个  
		countProcess++;
	}

	////清除hProcess句柄  
	CloseHandle(hProcess); 
	OutputDebugW(L"共有以上%d个进程在运行\n", countProcess);
}

/**初始化列标题*/
void initListControl()
{
	LV_COLUMN lvcolumn = { 0 };
	lvcolumn.mask = LVCF_TEXT | LVCF_WIDTH;

	//循环插入列表
	for (size_t i = 0; i < 4; i++)
	{
		//设置列的标题文本
		lvcolumn.pszText = (LPWSTR)processColumnList[i];
		//设置标题列的长度
		lvcolumn.cx = 235 + (i*-30);
		//发送窗口消息 添加列
		SendMessage(processHwnd, LVM_INSERTCOLUMN, i, (LONG_PTR)(&lvcolumn));
	}

	DWORD dwExStyle =
		LVS_EX_FULLROWSELECT |  //设置可选整行
		LVS_EX_GRIDLINES |  //listView的item的每个栏目之间添加分割线
		LVS_EX_HEADERDRAGDROP |//允许litview的目录通过拖放重排序
		LVS_EDITLABELS;
	//设置ListControl样式
	ListView_SetExtendedListViewStyle(processHwnd, dwExStyle);
	//设置整行选中 ,同上行代码有相同作用
	//SendMessage(processHwnd,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES | LVS_EX_GRIDLINES| LVS_REPORT| GWL_STYLE, LVS_EX_FULLROWSELECT);
	listProcess();
}
void getItemDetail(tagNMHDR* tagControl)
{
	//获取选中的行数量
	DWORD selectRowCount = ListView_GetSelectedCount(processHwnd);
	//if(selectRowCount==0)
	//{
	//	MessageBox(hwndMyDialog, L"请选择一个进程序号进行加载", L"消息提示!", MB_ICONWARNING);
	//}
	//获取选中行的下标 没有选中为-1
	long selectRowIndex = SendMessage(processHwnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (-1 == selectRowIndex)
	{
		//MessageBox(hwndMyDialog, L"请选择一个进程序号进行加载", L"消息提示!", MB_ICONWARNING);
		return;//没有选中直接返回
	}

	wchar_t itemText[10] = { 0 };
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = itemText;
	lvItem.iItem = selectRowIndex;
	lvItem.cchTextMax = 100;
	lvItem.iSubItem = 1;
	//根据lvItem描述信息以及选择的行下标 发送获取元素内容的消息
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	//传递进程id
	listModule(_wtoi(itemText));
	
}
void getSelectProcessName(wchar_t ** processName,int* processId)
{
	//获取选中的行数量
	DWORD selectRowCount = ListView_GetSelectedCount(processHwnd);
	//if(selectRowCount==0)
	//{
	//	MessageBox(hwndMyDialog, L"请选择一个进程序号进行加载", L"消息提示!", MB_ICONWARNING);
	//}
	//获取选中行的下标 没有选中为-1
	long selectRowIndex = SendMessage(processHwnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (-1 == selectRowIndex)
	{
		*processName = NULL;
		//MessageBox(hwndMyDialog, L"请选择一个进程序号进行加载", L"消息提示!", MB_ICONWARNING);
		return;//没有选中直接返回
	}

	wchar_t itemText[100] = { 0 };
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = itemText;
	lvItem.iItem = selectRowIndex;
	lvItem.cchTextMax = 100;
	lvItem.iSubItem = 0;
	//根据lvItem描述信息以及选择的行下标 发送获取元素内容的消息
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	//传递进程id
	*processName = itemText;
	//获取进程id
	lvItem.iSubItem = 1;
	wchar_t pid[100] = { 0 };
	lvItem.pszText = pid;
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	*processId = _wtoi(pid);
}

void refreshProcess()
{
	//清空模块中的原数据
	ListView_DeleteAllItems(processHwnd);
	listProcess();
}
