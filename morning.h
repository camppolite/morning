#pragma once
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <codecvt>
#include <vector>
#include <Psapi.h>
#include <iostream>
#include <string>


#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>



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

#define START 0
#define LEN_OF_INT64 22  // 21 + 1



std::string to_changan_jiudian("to_changan_jiudian");
std::string to_dianxiaoer("to_dianxiaoer");
std::string talk_get_baoturenwu("talk_get_baoturenwu");
std::vector<std::string*> datu_step = { &to_changan_jiudian, &to_dianxiaoer, &talk_get_baoturenwu };

std::vector<DWORD> FindPidsByName(const wchar_t* name);
HMODULE getProcessModulesAddress(HANDLE hProcess, const TCHAR* moduleName);
DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);
cv::Mat hwnd2mat(HWND hwnd);
cv::Point MatchingRectPos(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
cv::Point MatchingRectPos(HWND hwnd, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
bool MatchingRect(HWND hwnd, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
cv::Mat MatchingMethod(cv::Mat image, cv::Mat templ, cv::Mat mask, double threshold, int match_method);
cv::Point getMatchLoc(cv::Mat result, double threshold, int match_method);



//uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void init_log();

cv::Rect ROI_NULL();

class Step {
public:
	Step();
	Step(std::vector<std::string*> step_list);
	void reset();
	void previous();
	void next();
	std::string* current();

	std::vector<std::string*> steps;
	int index = 0;
	bool end = false;
};

class MyWindowInfo {
public:
	MyWindowInfo(HANDLE processID);

	uintptr_t ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask);
	uintptr_t PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask);
	uintptr_t getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
	POINT MatchingRectPos(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);

	int pos_x = 0;
	int pos_y = 0;
	uintptr_t player_pos_addr = 0;
	uintptr_t map_info_addr = 0;
	uintptr_t dianxiaoer_pos_addr = 0;

	HANDLE pid;
	HWND hwnd;
	RECT rect;
	HANDLE rProcess = 0;
	HMODULE hNtdll = 0;
	PFN_NtReadVirtualMemory pNtReadVirtualMemory;

	Step step = Step(datu_step);
};

class GoodMorning {
public:
	GoodMorning();

	void hook_data();
	void work();
	void test();


	bool waiting = false;
	std::vector<MyWindowInfo> winsInfo;
	HANDLE hSerial;
private:

};



bool mouse_click_human(MyWindowInfo* winfo, POINT pos, int xs, int ys, int mode);
POINT get_cursor_pos(MyWindowInfo* winfo, POINT pos);



const char* STOP_MP3 = "mmp3:STOP\n";

const char* MS_MOVE_HUMAN_SYMBOL = "movehm:%d,%d,%d,%d,%d\n";  // cx, cy, x, y, mode
const char* CLICK_CURRENT_SYMBOL = "hkeyCC\n";
const char* KEY_ALT_xxx = "hkey:ALT_%s\n";


const char* image_cursors_cursor = "object\\cursors\\cursor.png";
const char* image_cursors_cursor_mask = "object\\cursors\\cursor_mask.png";



