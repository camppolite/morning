#include "morning.h"
#include "log.h"

#include <filesystem>
#include <cstdlib> // For rand() and srand()

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

cv::Mat src, src_gray;
cv::Mat dst, detected_edges;

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";

#include <direct.h>

#pragma comment(lib, "ntdll")

namespace fs = std::filesystem;


typedef unsigned long long QWORD;
#define TARGET_APP_NAME L"mhmain.exe"
//#define TARGET_APP_NAME "mhtab.exe"
#define MHMAIN_DLL L"mhmain.dll"
#define MAX_WIN = 10

//int pos_x = 0;
//int pos_y = 0;
//uintptr_t player_pos_addr = 0;
//uintptr_t map_info_addr = 0;
//uintptr_t dianxiaoer_pos_addr = 0;
//HWND win_hwnd;
//HANDLE rProcess = 0;
//HMODULE hNtdll = 0;
//PFN_NtReadVirtualMemory pNtReadVirtualMemory;

GoodMorning gm;


MyWindowInfo::MyWindowInfo(HANDLE processID) {
	pid = processID;
}

uintptr_t MyWindowInfo::ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
{
	uintptr_t matchAddress = 0;
	BYTE* buffer = new BYTE[regionSize];

	SIZE_T bytesRead;
	if (pNtReadVirtualMemory(hProcess, (PVOID)startAddress, buffer, regionSize, &bytesRead) == 0)
		//if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
	{
		for (SIZE_T i = 0; i <= bytesRead - pattern.size(); i++)  // Updated loop condition
		{
			bool found = true;
			for (SIZE_T j = 0; j < pattern.size(); j++)
			{
				if (mask[j] == 'x' && buffer[i + j] != pattern.at(j))
				{
					found = false;
					break;
				}
			}

			// Pattern match found
			if (found)
			{
				matchAddress = reinterpret_cast<uintptr_t>(startAddress) + i;
				std::cout << i << std::endl;
				std::cout << "Pattern match found at address: 0x" << std::hex << matchAddress << std::endl;
				// Additional actions can be performed here
				break;
			}
		}
	}

	delete[] buffer;
	return matchAddress;
}

uintptr_t MyWindowInfo::PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
{
	// ModuleBase 为0则全局内存扫描，
	std::vector<BYTE> aob_byte;
	int start, end;
	start = end = 0;
	char dl = ' ';
	while ((start = pattern.find_first_not_of(dl, end))
		!= std::string::npos) {
		// str.find(dl, start) will return the index of dl
		// from start index
		end = pattern.find(dl, start);
		// substr function return the substring of the
		// original string from the given starting index
		// to the given end index
		aob_byte.push_back(static_cast<BYTE>(stoi(pattern.substr(start, end - start), nullptr, 16)));
	}

	uintptr_t matchAddress = 0;
	DWORD ModuleSize = GetModuleSize(hProcess, ModuleBase);

	// Enumerate memory regions in the target process
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	LPVOID minimumApplicationAddress = systemInfo.lpMinimumApplicationAddress;
	LPVOID maximumApplicationAddress = systemInfo.lpMaximumApplicationAddress;
	if (ModuleBase > 0) {
		// 全程序内存扫描
		DWORD ModuleSize = GetModuleSize(hProcess, ModuleBase);
		minimumApplicationAddress = ModuleBase;
		maximumApplicationAddress = ModuleBase + ModuleSize;
	}

	MEMORY_BASIC_INFORMATION memoryInfo;
	//for (LPVOID address = ModuleBase; address < ModuleBase + ModuleSize;)
	for (LPVOID address = minimumApplicationAddress; address < maximumApplicationAddress;)
	{
		if (VirtualQueryEx(hProcess, address, &memoryInfo, sizeof(memoryInfo)) == sizeof(memoryInfo))
		{
			// Check if the memory region is accessible and not reserved
			if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS)
				//if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE))
			{
				// Scan the memory region
				matchAddress = ScanMemoryRegion(hProcess, memoryInfo.BaseAddress, memoryInfo.RegionSize, aob_byte, mask);
				if (matchAddress > 0) break;
			}
			address = reinterpret_cast<LPVOID>(reinterpret_cast<char*>(address) + memoryInfo.RegionSize);
		}
		else
		{
			std::cout << "Failed to query memory information. Error code: " << GetLastError() << std::endl;
			break;
		}
	}
	return matchAddress;
}

uintptr_t MyWindowInfo::getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
	// adr_offset = rav_offset - 3
	// opcode_adr = AoB_adr + adr_offset
	// StaticAddress - opcode_adr - 7 = rav
	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB, mask);
	if (AoB_adr <= 0) return AoB_adr;

	SIZE_T regionSize = 0x10;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)(AoB_adr + offset), buffer, regionSize, &bytesRead);
	auto rav = *reinterpret_cast<long*>(buffer);
	delete[] buffer;
	return rav + 7 + AoB_adr + offset - 3;
}


GoodMorning::GoodMorning() {

}

void GoodMorning::hook_data() {
	for (auto winfo : gm.winsInfo) {

		winfo.hNtdll = GetModuleHandleA("ntdll.dll");
		PFN_NtOpenProcess pNtOpenProcess = (PFN_NtOpenProcess)GetProcAddress(winfo.hNtdll, "NtOpenProcess");
		winfo.pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(winfo.hNtdll, "NtReadVirtualMemory");

		OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
		CLIENT_ID         CID = { (HANDLE)winfo.pid, NULL };
		NTSTATUS status = pNtOpenProcess(&winfo.rProcess, PROCESS_ALL_ACCESS, &OA, &CID);

		auto mhmainDllBase = getProcessModulesAddress(winfo.rProcess, MHMAIN_DLL);

		//// 读取玩家坐标
		//player_pos_addr = getRelativeStaticAddressByAoB(
		//	rProcess,
		//	mhmainDllBase,
		//	"83 3D 00 00 00 00 FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D 00 00 00 00 E8 00 00 00 00 48 8D 0D 00 00 00 00 E8 00 00 00 00",
		//	"xx????xxxxxxxxxxxx????x????xxx????x????",
		//	18);
		//if (player_pos_addr > 0) {
		//	SIZE_T regionSize = 0x8;
		//	BYTE* buffer = new BYTE[regionSize];
		//	SIZE_T bytesRead;
		//	pNtReadVirtualMemory(rProcess, (PVOID)player_pos_addr, buffer, regionSize, &bytesRead);
		//	pos_x = *reinterpret_cast<float*>(buffer);
		//	pos_y = *reinterpret_cast<float*>(buffer + 4);
		//	delete[] buffer;
		//}

		//// 场景[100,10] (111,111)
		//// B4 F3 CC C6 B9 FA BE B3 5B 31 39 38 2C 32 33 32 5D 00 B6 FE 28 31 31 31 2C 31 31 31 29 00
		//auto map_info_AoB_adr = PerformAoBScan(rProcess, 0, "28 31 31 31 2C 31 31 31 29 00", "xxxxxxxxxx");

		//if (map_info_AoB_adr > 0) {
		//	SIZE_T map_offset = 0x14;
		//	map_info_addr = map_info_AoB_adr - map_offset;
		//	BYTE* buffer = new BYTE[map_offset];
		//	SIZE_T bytesRead;
		//	pNtReadVirtualMemory(rProcess, (PVOID)map_info_addr, buffer, map_offset, &bytesRead);
		//	delete[] buffer;
		//}

		//// 店小二坐标
		//// 0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA ? ? ? ? ? ? ? ?
		//auto dianxiaoer_AoB_adr = PerformAoBScan(
		//	rProcess,
		//	0,
		//	"0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA",
		//	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		//if (dianxiaoer_AoB_adr > 0) {
		//	dianxiaoer_pos_addr = dianxiaoer_AoB_adr + 0x58;
		//}
	}
}

void GoodMorning::work() {
	Step step = Step::START;
	while (true) {
		switch (step)
		{
		case START:
			break;
		case DO_TASK:
			break;
		case DONE_TASK:
			break;
		default:
			break;
		}
	}
}

void GoodMorning::test() {
	RECT rect;
	for (auto winfo : this->winsInfo) {

		GetWindowRect(winfo.hwnd, &rect);
	}
	printf("\n");
}

std::vector<DWORD> FindPidsByName(const wchar_t* name)
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

HMODULE getProcessModulesAddress(HANDLE hProcess, const TCHAR* moduleName)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;
	TCHAR lpBaseName[MAX_PATH] = TEXT("<unknown>");

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			GetModuleBaseName(hProcess, hMods[i], lpBaseName, sizeof(lpBaseName) / sizeof(TCHAR));

			if (_wcsicmp(lpBaseName, moduleName) == 0) {
				// 不区分大小写比较
				return *(HMODULE*)&hMods[i];  // 64位需要转地址才正确
			}
		}
	}
	return 0;
}

DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule)
{
	if (!hModule)
		return 0;

	MODULEINFO remoteProcessModuleInfo;
	bool success = GetModuleInformation(hProcess, hModule, &remoteProcessModuleInfo, sizeof(MODULEINFO));

	return remoteProcessModuleInfo.SizeOfImage;
}

//uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
//	// adr_offset = rav_offset - 1
//	// opcode_adr = AoB_adr + adr_offset
//	// StaticAddress - opcode_adr - 5 = rav
//	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB, mask);
//	auto rav = *reinterpret_cast<long*>(AoB_adr + offset);
//	return rav + 5 + AoB_adr + offset - 1;
//}

cv::Mat hwnd2mat(HWND hwnd) {

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
	// SEED the generator ONCE at the start of the program
	std::srand(static_cast<unsigned int>(t));
	current_path /= filename + std::string("r") + std::to_string(rand()) + ".png";
	cv::imwrite(current_path.string().c_str(), src);
	return src;
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
			//printf("111111111\n");
			//hwnd2mat(hwnd);
			//win_hwnd = hwnd;
			// 如果是多标签模式,只有mhtab.exe有窗口
			//SetForegroundWindow(hwnd);
			VirtualFree(
				pszMem,       // Base address of block
				0,             // Bytes of committed pages
				MEM_RELEASE);  // Decommit the pages
			MyWindowInfo winfo((HANDLE)targetProcessId);
			winfo.hwnd = hwnd;
			gm.winsInfo.push_back(winfo);
			return FALSE;
		}
		VirtualFree(
			pszMem,       // Base address of block
			0,             // Bytes of committed pages
			MEM_RELEASE);  // Decommit the pages
	}
	return TRUE; // Continue enumeration
}

void init_log() {
	log_close();
	/* Get current time */
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	struct stat st = { 0 };
	if (stat("log", &st) == -1) {
		_mkdir("log");
	}
	char log_file[22];
	log_file[strftime(log_file, sizeof(log_file), "log/%Y-%m-%d.log", lt)] = '\0';
	FILE* fp = fopen(log_file, "a");
	log_set_fp(fp);
}

//void init_data() {
//	DWORD processID = FindPidByName(TARGET_APP_NAME);
//
//	hNtdll = GetModuleHandleA("ntdll.dll");
//	PFN_NtOpenProcess pNtOpenProcess = (PFN_NtOpenProcess)GetProcAddress(hNtdll, "NtOpenProcess");
//	pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(hNtdll, "NtReadVirtualMemory");
//
//	OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
//	CLIENT_ID         CID = { (HANDLE)processID, NULL };
//	NTSTATUS status = pNtOpenProcess(&rProcess, PROCESS_ALL_ACCESS, &OA, &CID);
//
//	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
//
//	auto mhmainDllBase = getProcessModulesAddress(rProcess, MHMAIN_DLL);
//
//	// 读取玩家坐标
//	player_pos_addr = getRelativeStaticAddressByAoB(
//		rProcess,
//		mhmainDllBase,
//		"83 3D 00 00 00 00 FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D 00 00 00 00 E8 00 00 00 00 48 8D 0D 00 00 00 00 E8 00 00 00 00",
//		"xx????xxxxxxxxxxxx????x????xxx????x????",
//		18);
//	if (player_pos_addr > 0) {
//		SIZE_T regionSize = 0x8;
//		BYTE* buffer = new BYTE[regionSize];
//		SIZE_T bytesRead;
//		pNtReadVirtualMemory(rProcess, (PVOID)player_pos_addr, buffer, regionSize, &bytesRead);
//		pos_x = *reinterpret_cast<float*>(buffer);
//		pos_y = *reinterpret_cast<float*>(buffer + 4);
//		delete[] buffer;
//	}
//
//
//	// 场景[100,10] (111,111)
//	// B4 F3 CC C6 B9 FA BE B3 5B 31 39 38 2C 32 33 32 5D 00 B6 FE 28 31 31 31 2C 31 31 31 29 00
//	auto map_info_AoB_adr = PerformAoBScan(rProcess, 0, "28 31 31 31 2C 31 31 31 29 00", "xxxxxxxxxx");
//
//	if (map_info_AoB_adr > 0) {
//		SIZE_T map_offset = 0x14;
//		map_info_addr = map_info_AoB_adr - map_offset;
//		BYTE* buffer = new BYTE[map_offset];
//		SIZE_T bytesRead;
//		pNtReadVirtualMemory(rProcess, (PVOID)map_info_addr, buffer, map_offset, &bytesRead);
//		delete[] buffer;
//	}
//
//	// 店小二坐标
//	// 0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA ? ? ? ? ? ? ? ?
//	auto dianxiaoer_AoB_adr = PerformAoBScan(
//		rProcess,
//		0,
//		"0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA",
//		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	if (dianxiaoer_AoB_adr > 0) {
//		dianxiaoer_pos_addr = dianxiaoer_AoB_adr + 0x58;
//	}
//}

void test() {
	HANDLE hSerial;
	DCB dcbSerialParams = { 0 };
	COMMTIMEOUTS timeouts = { 0 };

	// Open the serial port
	hSerial = CreateFileW(L"COM1",
		GENERIC_READ | GENERIC_WRITE,
		0,                          // No sharing
		NULL,                       // No security attributes
		OPEN_EXISTING,              // Open existing port
		FILE_ATTRIBUTE_NORMAL,      // Normal file attributes
		NULL);                      // No template file

	if (hSerial == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening serial port." << std::endl;
		//return 1;
	}

	// Get current serial port parameters
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		std::cerr << "Error getting comm state." << std::endl;
		CloseHandle(hSerial);
		//return 1;
	}

	// Set serial port parameters (e.g., 9600 baud, 8 data bits, no parity, 1 stop bit)
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.StopBits = ONESTOPBIT;

	if (!SetCommState(hSerial, &dcbSerialParams)) {
		std::cerr << "Error setting comm state." << std::endl;
		CloseHandle(hSerial);
		//return 1;
	}

	// Set communication timeouts
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(hSerial, &timeouts)) {
		std::cerr << "Error setting timeouts." << std::endl;
		CloseHandle(hSerial);
		//return 1;
	}

	// Example: Writing data
	char data_to_send[] = "Hello Serial!";
	DWORD bytes_written;
	if (!WriteFile(hSerial, data_to_send, sizeof(data_to_send) - 1, &bytes_written, NULL)) {
		std::cerr << "Error writing to serial port." << std::endl;
	}
	else {
		std::cout << "Sent: " << data_to_send << std::endl;
	}

	// Example: Reading data (simplified, typically done in a loop/thread)
	char buffer[256];
	DWORD bytes_read;
	if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytes_read, NULL)) {
		std::cerr << "Error reading from serial port." << std::endl;
	}
	else {
		buffer[bytes_read] = '\0';
		std::cout << "Received: " << buffer << std::endl;
	}

	// Close the serial port
	CloseHandle(hSerial);
}

void getMyComPortName() {
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD i = 0;
		DWORD nameSize, dataSize;
		TCHAR name[256], data[256];

		while (true) {
			nameSize = sizeof(name);
			dataSize = sizeof(data);

			// Enumerate the values in the key
			if (RegEnumValueW(hKey, i++, name, &nameSize, NULL, NULL, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
				// 'data' contains the COM port name, e.g., "COM1"
				printf("1111111111\n");
			}
			else {
				break; // Break the loop if enumeration fails (end of list)
			}
		}
		RegCloseKey(hKey);
	}
}

static void CannyThreshold(int, void*)
{
	blur(src_gray, detected_edges, Size(3, 3));

	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);

	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	cv::imwrite("222.png", dst);
	imshow(window_name, dst);
}

int main(int argc, const char** argv)
{
	src = imread("111.png", IMREAD_COLOR); // Load an image
	if (src.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
		return -1;
	}

	dst.create(src.size(), src.type());

	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	namedWindow(window_name, WINDOW_AUTOSIZE);

	createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	CannyThreshold(0, 0);

	waitKey(0);




	getMyComPortName();

	init_log();
	log_info("日志输出测试");

	for (auto processID : FindPidsByName(TARGET_APP_NAME)) {
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
	}
	Sleep(50);  // 等一下枚举窗口句柄回调完成再执行

	//gm.hook_data();
	//gm.work();
	gm.test();
	return 0;
}


