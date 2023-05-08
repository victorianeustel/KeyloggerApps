#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<iostream>
#include<string>
#include<string.h>
#include<Psapi.h>
#include <GDIPlus.h> 
#pragma comment (lib, "gdiplus.lib") 
using namespace Gdiplus;


namespace ScreenShot
{

    // using powershell
	int TakeScreenShot(const char *path) {
		HDC h = GetDC(NULL);
		int width = GetDeviceCaps(h, 118);
		int height = GetDeviceCaps(h, 117);
		char str[1024];
		//sprintf(str, "[Reflection.Assembly]::LoadWithPartialName('System.Drawing');function screenshot([Drawing.Rectangle]$bounds, $path) {$bmp = New-Object Drawing.Bitmap $bounds.width, $bounds.height;$graphics = [Drawing.Graphics]::FromImage($bmp);$graphics.CopyFromScreen($bounds.Location, [Drawing.Point]::Empty, $bounds.size);$bmp.Save($path);$graphics.Dispose();$bmp.Dispose();};$bounds = [Drawing.Rectangle]::FromLTRB(0, 0, %d, %d);screenshot $bounds '%s';", width, height, path);
		std::string param = "[Reflection.Assembly]::LoadWithPartialName('System.Drawing');function screenshot([Drawing.Rectangle]$bounds, $path) {$bmp = New-Object Drawing.Bitmap $bounds.width, $bounds.height;$graphics = [Drawing.Graphics]::FromImage($bmp);$graphics.CopyFromScreen($bounds.Location, [Drawing.Point]::Empty, $bounds.size);$bmp.Save($path);$graphics.Dispose();$bmp.Dispose();};$bounds = [Drawing.Rectangle]::FromLTRB(0, 0, " + Helper::ToString(width) + std::string(", ") + Helper::ToString(height) + ");screenshot $bounds '" + path + "';";
		SHELLEXECUTEINFO shellinfo = { 0 };
		shellinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellinfo.hwnd = NULL;
		LPCWSTR r = L"open";
		LPCWSTR r1 = L"powershell.exe";
		shellinfo.lpVerb = r;
		shellinfo.lpFile = r1;
		std::wstring temp = std::wstring(param.begin(), param.end());
		shellinfo.lpParameters = temp.c_str();
		shellinfo.lpDirectory = NULL;
		shellinfo.nShow = SW_SHOW;
		shellinfo.hInstApp = NULL;
		
		return (bool)ShellExecuteEx(&shellinfo);
	}

    BOOL getSize(HMONITOR Arg1,HDC Arg2,LPRECT Arg3,LPARAM Arg4){
        MONITORINFOEXA mif;
        mif.cbSize = sizeof(MONITORINFOEXA);
        DEVMODEA dm;
        dm.dmSize = sizeof(DEVMODEA);
        if (::GetMonitorInfoA(Arg1, &mif) != 0 && ::EnumDisplaySettingsA(mif.szDevice, ENUM_CURRENT_SETTINGS, &dm) != 0)
        {
            PDWORD p = PDWORD(Arg4);
            *p = dm.dmPelsWidth;
            *(p + 1) = dm.dmPelsHeight;
            return TRUE;
        }
        return FALSE;
    }
    // using c++
    BOOL make_screenshot(std::string file)
    {
        ULONG_PTR gdiplustoken;
        GdiplusStartupInput gdiplusstartupinput;
        GdiplusStartupOutput gdiplusstartupoutput;
        gdiplusstartupinput.SuppressBackgroundThread = true;
        GdiplusStartup(&gdiplustoken, &gdiplusstartupinput, &gdiplusstartupoutput);//start GDI+ 
        HDC dc = GetDC(GetDesktopWindow());
        HDC dc2 = CreateCompatibleDC(dc);
        RECT rcOkno;
        //GetWindowRect(GetDesktopWindow(), &rcOkno); // not working
        DWORD size2[2];
        BOOL b = ::EnumDisplayMonitors(
            nullptr,
            nullptr,
            (MONITORENUMPROC)getSize,
            (LPARAM)size2);
        int w = size2[0];
        int h = size2[1];
        HBITMAP hbitmap = CreateCompatibleBitmap(dc, w, h);
        HBITMAP holdbitmap = (HBITMAP)SelectObject(dc2, hbitmap);
        BitBlt(dc2, 0, 0, w, h, dc, 0, 0, SRCCOPY);
        Bitmap* bm = new Bitmap(hbitmap, NULL);
        UINT num;
        UINT size;
        ImageCodecInfo* imagecodecinfo;
        GetImageEncodersSize(&num, &size);
        imagecodecinfo = (ImageCodecInfo*)(malloc(size));
        GetImageEncoders(num, size, imagecodecinfo);
        CLSID clsidEncoder;
        for (int i = 0; i < num; i++)
        {
            if (wcscmp(imagecodecinfo[i].MimeType, L"image/jpeg") == 0)
                clsidEncoder = imagecodecinfo[i].Clsid;
        }
        free(imagecodecinfo);
        std::wstring ws;
        ws.assign(file.begin(), file.end());
        bm->Save(ws.c_str(), &clsidEncoder);
        SelectObject(dc2, holdbitmap);
        DeleteObject(dc2);
        DeleteObject(hbitmap);
        ReleaseDC(GetDesktopWindow(), dc);
        GdiplusShutdown(gdiplustoken);
        return b;
    }

    // get the current active window's name
    std::string ActiveWindow() {
        CHAR wnd_title[256];
        HWND hwnd = GetForegroundWindow(); // get handle of currently active window
        GetWindowTextA(hwnd, wnd_title, 256);

        DWORD dwPID;
        GetWindowThreadProcessId(hwnd, &dwPID);

        HANDLE Handle = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE,
            dwPID
        );
        if (Handle)
        {
            CHAR Buffer[MAX_PATH];
            if (GetModuleFileNameExA(Handle, 0, Buffer, MAX_PATH))
            {
                return Buffer;
            }
            CloseHandle(Handle);
        }
        return "";
    }
}