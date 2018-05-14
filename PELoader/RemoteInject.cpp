#include "stdafx.h"
#include "RemoteInject.h"

//Dialog句柄
extern HWND MainDialogHwnd;

void remoteInject()
{
	wchar_t* processName = NULL;
	int processId=0;
	getSelectProcessName(&processName, &processId);
	if(processName==NULL)
	{
		MessageBoxA(MainDialogHwnd, "未选择进程项!", "信息提示!", MB_ICONWARNING);
		return;
	}
	DWORD mAddr=0;


	//获取dll路径
	TCHAR* buff = chooseFile(0);

 
	/*STARTUPINFOA startInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };*/
	//以挂起形式创建子进程
	//CreateProcessA(fileName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startInfo, &processInfo);
	
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE| PROCESS_VM_OPERATION, FALSE, processId);  //获取的是当前进程ID

	DWORD dllAddr = 0x8000000;
	//在子进程中申请虚拟地址空间
	LPVOID lpa = VirtualAllocEx(hProcess, (LPVOID)dllAddr, lstrlenW(buff) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	SIZE_T wNUm = 0;
	//向子进程虚拟空间写入DLL的名称
	WriteProcessMemory(hProcess, (LPVOID)dllAddr, buff, 100, &wNUm);
	//动态获取LoadLibraryA地址
	mAddr = (DWORD)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryW");
	//创建远程线程
	HANDLE handle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mAddr, (LPVOID)dllAddr, 0, NULL);

	WaitForSingleObject(handle, INFINITE);
	//释放子进程dll名称虚拟空间
	//	VirtualFree((LPVOID)dllAddr, strlen(buff) + 1, MEM_RELEASE);
	//恢复子进程中的主线程
	
}