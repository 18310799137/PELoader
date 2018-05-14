#pragma once
//初始化进程模块列元素
void initListControl(); 

//
void getItemDetail(tagNMHDR* tagControl);

/**获取进程列表信息*/
void listProcess();

void getSelectProcessName(wchar_t** processName,int* processId);

//刷新进程列表
void refreshProcess();