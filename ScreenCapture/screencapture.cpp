#pragma once
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <filesystem>
#include <cstdlib> // For rand() and srand()
//#include <map>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

namespace fs = std::filesystem;
//using namespace cv;

#define TARGET_APP_NAME L"mhmain.exe"

//std::map<HANDLE, MyWindowInfo> proccess;

class MyWindowInfo {
public:
	MyWindowInfo(HANDLE processID);

	HANDLE pid;
	HWND hwnd;
};

class ScreenShot {
public:
	ScreenShot();

	std::vector<MyWindowInfo> winsInfo;
};

ScreenShot::ScreenShot() {

}

MyWindowInfo::MyWindowInfo(HANDLE processID) {
	pid = processID;
}

ScreenShot sc;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWCH   Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _CLIENT_ID {
	PVOID              UniqueProcess;
	PVOID              UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG              Length;
	HANDLE             RootDirectory;
	PUNICODE_STRING    ObjectName;
	ULONG              Attributes;
	PVOID              SecurityDescriptor;
	PVOID              SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef NTSTATUS(NTAPI* PFN_NtOpenProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
	);

typedef NTSTATUS(NTAPI* PFN_NtReadVirtualMemory)(
	_In_ HANDLE ProcessHandle,
	_In_opt_ PVOID BaseAddress,
	_Out_writes_bytes_to_(NumberOfBytesToRead, *NumberOfBytesRead) PVOID Buffer,
	_In_ SIZE_T NumberOfBytesToRead,
	_Out_opt_ PSIZE_T NumberOfBytesRead
	);

inline cv::Mat hwnd2mat(HWND hwnd) {

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	cv::Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	// HALFTONE 与其他三种模式相比，HALFTONE 模式较慢，需要对源图像进行更多的处理;但会生成更高质量的图像。 另请注意，在设置 HALFTONE 模式后，必须调用 SetBrushOrgEx ，以避免画笔错位。
	//SetStretchBltMode(hwindowDC, HALFTONE);
	//SetBrushOrgEx(hwindowDC, 16, 16, nullptr);

	RECT rcClient;    // get the height and width of the screen
	GetClientRect(hwnd, &rcClient);
	//GetWindowRect(hwnd, &windowsize);
	srcheight = rcClient.bottom - rcClient.top;
	srcwidth = rcClient.right - rcClient.left;
	height = rcClient.bottom - rcClient.top;  //change this to whatever size you want to resize to
	width = rcClient.right - rcClient.left;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);
	//imshow("output", src);
	//cv::waitKey(0);
	auto current_path = fs::current_path();
	//fs::path filename = "data.txt";
	//fs::path full_path = current_path / filename;
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	char filename[35];
	filename[strftime(filename, sizeof(filename), "%Y-%m-%d %H-%M-%S-", lt)] = '\0';

	current_path /= filename + std::string("r") + std::to_string(rand()) + ".png";
	cv::imwrite(current_path.string(), src);
	return src;
}

inline std::vector<DWORD> FindPidsByName(const wchar_t* name)
{
	std::vector<DWORD> pids;
	HANDLE h;
	PROCESSENTRY32 singleProcess;
	h = CreateToolhelp32Snapshot( //takes a snapshot of specified processes
		TH32CS_SNAPPROCESS, //get all processes
		0); //ignored for SNAPPROCESS

	singleProcess.dwSize = sizeof(PROCESSENTRY32);

	do {
		//printf("szExeFile: %ws\n", singleProcess.szExeFile);
		if (_wcsicmp(singleProcess.szExeFile, name) == 0)
		{
			// 不区分大小写比较
			DWORD pid = singleProcess.th32ProcessID;
			pids.push_back(pid);
		}

	} while (Process32Next(h, &singleProcess));

	CloseHandle(h);

	return pids;
}

// Example callback function for EnumWindows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	DWORD currentProcessId = 0;
	GetWindowThreadProcessId(hwnd, &currentProcessId);

	DWORD targetProcessId = *reinterpret_cast<DWORD*>(lParam);

	if (currentProcessId == targetProcessId) {
		// Found a window belonging to the target process
		// Store or use the 'hwnd' as needed
		// For example, you could store it in a global variable or a vector
		// and return FALSE to stop enumeration if you only need one HWND.
		// If you need all HWNDs, return TRUE to continue.
		// 梦幻有好几个窗口，需要过滤一下
		// "梦幻西游 ONLINE - "
		int cTxtLen = 100;
		std::string gametitle = "梦幻西游 ONLINE";
		//std::string gametitle = "梦幻西游 ONLINE - ";
		// Allocate memory for the string and copy 
		// the string into the memory. 
		auto pszMem = (PSTR)VirtualAlloc((LPVOID)NULL,
			(DWORD)(cTxtLen), MEM_COMMIT,
			PAGE_READWRITE);
		GetWindowTextA(hwnd, pszMem, cTxtLen);
		if (strstr(pszMem, gametitle.c_str()) != 0) {
			// 如果是多标签模式,只有mhtab.exe有窗口
			VirtualFree(
				pszMem,       // Base address of block
				0,             // Bytes of committed pages
				MEM_RELEASE);  // Decommit the pages
			MyWindowInfo winfo((HANDLE)targetProcessId);
			winfo.hwnd = hwnd;
			sc.winsInfo.push_back(winfo);
			printf("窗口句柄回调成功：%s\n", gametitle.c_str());
			return FALSE;
		}
		VirtualFree(
			pszMem,       // Base address of block
			0,             // Bytes of committed pages
			MEM_RELEASE);  // Decommit the pages
	}
	return TRUE; // Continue enumeration
}


int main(int argc, const char** argv)
{
	// SEED the generator ONCE at the start of the program
	std::srand(static_cast<unsigned int>(time(nullptr)));

	for (auto processID : FindPidsByName(TARGET_APP_NAME)) {
		printf("窗口句柄开始\n");
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
	}
	Sleep(50);  // 等一下枚举窗口句柄回调完成再执行
	if (argc > 1) {
		while (true) {
			for (auto winfo : sc.winsInfo) {
				hwnd2mat(winfo.hwnd);
			}
		}
	}
	else {
		for (auto winfo : sc.winsInfo) {
			hwnd2mat(winfo.hwnd);
		}
	}

	return 0;
}
