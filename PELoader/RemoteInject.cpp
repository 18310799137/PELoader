#include "stdafx.h"
#include "RemoteInject.h"

//Dialog���
extern HWND MainDialogHwnd;

void remoteInject()
{
	wchar_t* processName = NULL;
	int processId=0;
	getSelectProcessName(&processName, &processId);
	if(processName==NULL)
	{
		MessageBoxA(MainDialogHwnd, "δѡ�������!", "��Ϣ��ʾ!", MB_ICONWARNING);
		return;
	}
	DWORD mAddr=0;


	//��ȡdll·��
	TCHAR* buff = chooseFile(0);

 
	/*STARTUPINFOA startInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };*/
	//�Թ�����ʽ�����ӽ���
	//CreateProcessA(fileName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startInfo, &processInfo);
	
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE| PROCESS_VM_OPERATION, FALSE, processId);  //��ȡ���ǵ�ǰ����ID

	DWORD dllAddr = 0x8000000;
	//���ӽ��������������ַ�ռ�
	LPVOID lpa = VirtualAllocEx(hProcess, (LPVOID)dllAddr, lstrlenW(buff) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	SIZE_T wNUm = 0;
	//���ӽ�������ռ�д��DLL������
	WriteProcessMemory(hProcess, (LPVOID)dllAddr, buff, 100, &wNUm);
	//��̬��ȡLoadLibraryA��ַ
	mAddr = (DWORD)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryW");
	//����Զ���߳�
	HANDLE handle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mAddr, (LPVOID)dllAddr, 0, NULL);

	WaitForSingleObject(handle, INFINITE);
	//�ͷ��ӽ���dll��������ռ�
	//	VirtualFree((LPVOID)dllAddr, strlen(buff) + 1, MEM_RELEASE);
	//�ָ��ӽ����е����߳�
	
}