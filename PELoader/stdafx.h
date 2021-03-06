// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>
#include <commdlg.h>
//遍历模块使用头文件
#include "psapi.h"

#include "resource.h"
// C 运行时头文件
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//进程快照函数头文件  
#include <CommCtrl.h>
#include <tlhelp32.h> 
#include "DbgPrint.h"
#pragma comment(lib,"comctl32.lib")
// TODO: 在此处引用程序需要的其他头文件
#include "ModuleUtils.h"
#include "ProcessUtils.h"
#include "PEParse.h"
#include "ShellProtected.h"
#include "RemoteInject.h"
