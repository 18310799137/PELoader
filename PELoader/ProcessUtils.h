#pragma once
//��ʼ������ģ����Ԫ��
void initListControl(); 

//
void getItemDetail(tagNMHDR* tagControl);

/**��ȡ�����б���Ϣ*/
void listProcess();

void getSelectProcessName(wchar_t** processName,int* processId);

//ˢ�½����б�
void refreshProcess();