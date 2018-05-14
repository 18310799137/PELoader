#pragma once
void  __cdecl OutputDebugStringF(const char* format, ...);
void  __cdecl OutputDebugW(const wchar_t* format, ...);

#ifdef UNICODE
		#ifdef _DEBUG
		#define DbgPrintf OutputDebugStringF
		#else
		#define DbgPrintf
		#endif
	
#else
		#ifdef _DEBUG
		#define DbgPrintf OutputDebugW
		#else
		#define DbgPrintf
		#endif
#endif // unico
