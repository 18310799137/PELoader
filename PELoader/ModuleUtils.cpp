#include "stdafx.h"
//����ʵ�����
extern HINSTANCE hinstance;
//Dialog���
extern HWND MainDialogHwnd;
//����ģ��moduleHwnd������
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

	//���ģ���е�ԭ����
	ListView_DeleteAllItems(moduleHwnd);
	//����һ���еĽṹ��
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	
	//�����е��±�Ϊ0
	int rowIndex = 0;
	//�������� �洢ģ����
	HMODULE hModule[1024] = { 0 };
	DWORD dwRes=0;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);  //��ȡ���ǵ�ǰ����ID
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
			//���ò�������±�
			lvItem.iItem = rowIndex++;
			//��һ�д洢���
			lvItem.pszText = NO;
			lvItem.iSubItem = 0;
			SendMessage(moduleHwnd, LVM_INSERTITEM, 0, (LONG)(&lvItem));

			
			//�ڶ��д洢ģ������
			lvItem.pszText = wzName;
			lvItem.iSubItem = 1;
			ListView_SetItem(moduleHwnd, &lvItem);


			//�����д�ģ����ʼ��ַ
			TCHAR modleAddr[10] = { 0 };
			wsprintf(modleAddr, TEXT("%x"), hModule[i]);
			
			lvItem.pszText = modleAddr;
			lvItem.iSubItem = 2;
			ListView_SetItem(moduleHwnd, &lvItem);

		}
	}
	CloseHandle(hProcess);
}
/**��ʼ���б���*/
void initListModule()
{
	//��������е�����
	LV_COLUMN lvcolumn = { 0 };
	lvcolumn.mask = LVCF_TEXT | LVCF_WIDTH;
	int colCX = 0;
	//ѭ�������б�
	for (size_t i = 0; i < 3; i++)
	{
		//�����еı����ı�
		lvcolumn.pszText = (LPWSTR)moduleColumnList[i];
		//�����еĳ��� ,��һ��100,����350
		if (i == 0) {
			colCX = 130;
		}
		else if (i == 1) {
			colCX = 510;
		}
		else {
			colCX = 130;
		}
		//���ñ����еĳ���
		lvcolumn.cx = colCX;
		//���ʹ�����Ϣ �����
		SendMessage(moduleHwnd, LVM_INSERTCOLUMN, i, (LONG_PTR)(&lvcolumn));
	}

	DWORD dwExStyle =
		LVS_EX_FULLROWSELECT |  //���ÿ�ѡ����
		LVS_EX_GRIDLINES |  //listView��item��ÿ����Ŀ֮����ӷָ���
		LVS_EX_HEADERDRAGDROP |//����litview��Ŀ¼ͨ���Ϸ�������
		LVS_EDITLABELS;
	//����ListControl��ʽ
	ListView_SetExtendedListViewStyle(moduleHwnd, dwExStyle);
	//��������ѡ�� ,ͬ���д�������ͬ����
	//SendMessage(processHwnd,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES | LVS_EX_GRIDLINES| LVS_REPORT| GWL_STYLE, LVS_EX_FULLROWSELECT);
	
}