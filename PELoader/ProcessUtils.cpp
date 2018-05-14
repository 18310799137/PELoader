#include "stdafx.h"


//����ʵ�����
extern HINSTANCE hinstance;
//Dialog���
extern HWND MainDialogHwnd;
//��������ģ��processList���
HWND processHwnd = NULL;
////�����ⲿ����ģ��ModuleList������
//extern HWND moduleHwnd;

CONST WCHAR* processColumnList[] = { L"PROCESS NAME ",L"ID",L"�����ַ",L"�����ַ" };
/**��ȡ�����б���Ϣ*/
void listProcess()
{
	int countProcess = 0;                                 //��ǰ����������������  
	PROCESSENTRY32 currentProcess;                      //��ſ��ս�����Ϣ��һ���ṹ��  
	currentProcess.dwSize = sizeof(PROCESSENTRY32);     //��ʹ������ṹ֮ǰ�����������Ĵ�С  
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//��ϵͳ�ڵ����н�����һ������
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		OutputDebugW(L"CreateToolhelp32Snapshot()����ʧ��!\n");
		return;
	}
	//����һ���еĽṹ��
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	//��ȡ��һ��������Ϣ  
	bool bMore = Process32First(hProcess, &currentProcess);
	//�����е��±�Ϊ0
	int rowIndex = 0;
	while (bMore)
	{
		lvItem.pszText = currentProcess.szExeFile;
		lvItem.iItem = rowIndex++;
		lvItem.iSubItem = 0;
		SendMessage(processHwnd, LVM_INSERTITEM, 0, (LONG)(&lvItem));
		TCHAR processID[10] = { 0 };
		//��DWORD��IDת��Ϊ�ַ���չʾ
		wsprintf(processID, TEXT("%d"), currentProcess.th32ProcessID);
		//�ڶ��д�Ž���ID
		lvItem.pszText = processID;
		lvItem.iSubItem = 1;
		ListView_SetItem(processHwnd, &lvItem);

		OutputDebugW(L"PID=%d    PName= %s\n", currentProcess.th32ProcessID, currentProcess.szExeFile); //�������̿��գ�������ʾÿ��������Ϣ  
		bMore = Process32Next(hProcess, &currentProcess);    //������һ��  
		countProcess++;
	}

	////���hProcess���  
	CloseHandle(hProcess); 
	OutputDebugW(L"��������%d������������\n", countProcess);
}

/**��ʼ���б���*/
void initListControl()
{
	LV_COLUMN lvcolumn = { 0 };
	lvcolumn.mask = LVCF_TEXT | LVCF_WIDTH;

	//ѭ�������б�
	for (size_t i = 0; i < 4; i++)
	{
		//�����еı����ı�
		lvcolumn.pszText = (LPWSTR)processColumnList[i];
		//���ñ����еĳ���
		lvcolumn.cx = 235 + (i*-30);
		//���ʹ�����Ϣ �����
		SendMessage(processHwnd, LVM_INSERTCOLUMN, i, (LONG_PTR)(&lvcolumn));
	}

	DWORD dwExStyle =
		LVS_EX_FULLROWSELECT |  //���ÿ�ѡ����
		LVS_EX_GRIDLINES |  //listView��item��ÿ����Ŀ֮����ӷָ���
		LVS_EX_HEADERDRAGDROP |//����litview��Ŀ¼ͨ���Ϸ�������
		LVS_EDITLABELS;
	//����ListControl��ʽ
	ListView_SetExtendedListViewStyle(processHwnd, dwExStyle);
	//��������ѡ�� ,ͬ���д�������ͬ����
	//SendMessage(processHwnd,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES | LVS_EX_GRIDLINES| LVS_REPORT| GWL_STYLE, LVS_EX_FULLROWSELECT);
	listProcess();
}
void getItemDetail(tagNMHDR* tagControl)
{
	//��ȡѡ�е�������
	DWORD selectRowCount = ListView_GetSelectedCount(processHwnd);
	//if(selectRowCount==0)
	//{
	//	MessageBox(hwndMyDialog, L"��ѡ��һ��������Ž��м���", L"��Ϣ��ʾ!", MB_ICONWARNING);
	//}
	//��ȡѡ���е��±� û��ѡ��Ϊ-1
	long selectRowIndex = SendMessage(processHwnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (-1 == selectRowIndex)
	{
		//MessageBox(hwndMyDialog, L"��ѡ��һ��������Ž��м���", L"��Ϣ��ʾ!", MB_ICONWARNING);
		return;//û��ѡ��ֱ�ӷ���
	}

	wchar_t itemText[10] = { 0 };
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = itemText;
	lvItem.iItem = selectRowIndex;
	lvItem.cchTextMax = 100;
	lvItem.iSubItem = 1;
	//����lvItem������Ϣ�Լ�ѡ������±� ���ͻ�ȡԪ�����ݵ���Ϣ
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	//���ݽ���id
	listModule(_wtoi(itemText));
	
}
void getSelectProcessName(wchar_t ** processName,int* processId)
{
	//��ȡѡ�е�������
	DWORD selectRowCount = ListView_GetSelectedCount(processHwnd);
	//if(selectRowCount==0)
	//{
	//	MessageBox(hwndMyDialog, L"��ѡ��һ��������Ž��м���", L"��Ϣ��ʾ!", MB_ICONWARNING);
	//}
	//��ȡѡ���е��±� û��ѡ��Ϊ-1
	long selectRowIndex = SendMessage(processHwnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (-1 == selectRowIndex)
	{
		*processName = NULL;
		//MessageBox(hwndMyDialog, L"��ѡ��һ��������Ž��м���", L"��Ϣ��ʾ!", MB_ICONWARNING);
		return;//û��ѡ��ֱ�ӷ���
	}

	wchar_t itemText[100] = { 0 };
	LV_ITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = itemText;
	lvItem.iItem = selectRowIndex;
	lvItem.cchTextMax = 100;
	lvItem.iSubItem = 0;
	//����lvItem������Ϣ�Լ�ѡ������±� ���ͻ�ȡԪ�����ݵ���Ϣ
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	//���ݽ���id
	*processName = itemText;
	//��ȡ����id
	lvItem.iSubItem = 1;
	wchar_t pid[100] = { 0 };
	lvItem.pszText = pid;
	SendMessage(processHwnd, LVM_GETITEMTEXT, selectRowIndex, (long)&lvItem);
	*processId = _wtoi(pid);
}

void refreshProcess()
{
	//���ģ���е�ԭ����
	ListView_DeleteAllItems(processHwnd);
	listProcess();
}
