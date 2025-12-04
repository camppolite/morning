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

#define dianxiaoer_valid_distence 5  // 与店小二对话时的最大有效距离
//// 场景id
//#define 长安城 1001
//#define 长安酒店 1028
//#define 建邺城 1501
//#define 东海湾 1506


const char* STOP_MP3 = "mmp3:STOP\n";

const char* MS_MOVE_HUMAN_SYMBOL = "movehm:%d,%d,%d,%d,%d\n";  // cx, cy, x, y, mode
const char* CLICK_CURRENT_SYMBOL = "hkeyCC\n";
const char* KEY_ALT_xxx = "hkey:ALT_%s\n";
const char* KEY_PRESS = "hkey:%s\n";

const char* img_btn_beibao = "object\\btn\\beibao.png";
const char* img_btn_package_prop_640 = "object\\btn\\package_prop_640.png";
const char* img_btn_tingtingwufang = "object\\btn\\tingtingwufang.png";

const char* img_npc_dianxiaoer = "object\\npc\\dianxiaoer.png";

const char* img_cursors_cursor = "object\\cursors\\cursor.png";
const char* img_cursors_cursor_mask = "object\\cursors\\cursor_mask.png";

std::string to_changan_jiudian("to_changan_jiudian");
std::string wait_load_scene_changanjiudian("wait_load_scene_changanjiudian");
std::string to_dianxiaoer("to_dianxiaoer");
std::string talk_get_baoturenwu("talk_get_baoturenwu");
std::vector<std::string*> datu_step = {
	&to_changan_jiudian,
	&wait_load_scene_changanjiudian,
	&to_dianxiaoer, 
	&talk_get_baoturenwu
};

class Step {
public:
	Step();
	Step(std::vector<std::string*> step_list);
	void reset();
	void previous();
	void next();
	void set_current(std::string* step);

	std::vector<std::string*> steps;
	std::string* current;
	int index;
	bool end = false;
};

class MyWindowInfo {
public:
	MyWindowInfo(HANDLE processID);

	std::vector<uintptr_t> ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask);
	std::vector<uintptr_t> PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask);

	uintptr_t ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask);
	uintptr_t PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask);
	uintptr_t getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
	uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
	POINT MatchingRectPos(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
	void update_player_float_pos();
	void update_scene();
	void update_scene_id();
	void scan_dianxiaoer_addr_pos();
	void update_dianxiaoer_pos();

	void move_cursor_center_top();
	void open_beibao();
	void open_map();
	void move_to_dianxiaoer();
	bool is_dianxiaoer_pos(float x, float y);
	bool is_moving();
	bool is_near_dianxiaoer();
	bool is_dianxiaoer_stop();
	POINT compute_pos_pixel(POINT dst, unsigned int scene_id);
	int convert_to_map_pos_x(float x);
	int convert_to_map_pos_y(float y);
	bool talk_to_dianxiaoer();

	void from_changancheng_to_changanjiudian();
	void UpdateWindowRect();
	cv::Rect ROI_cursor(POINT pos);
	cv::Rect ROI_beibao();
	cv::Rect ROI_npc_talk();

	float player_x = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	float player_y = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	POINT player_pos = { 0, 0 };

	uintptr_t player_pos_addr = 0;
	uintptr_t map_info_addr = 0;
	SIZE_T map_offset = 0x14;

	uintptr_t dianxiaoer_pos_addr = 0;
	std::vector<POINT> dianxiaoer_pos_list;  // 店小二固定移动的几个坐标
	float dianxiaoer_pos_x = 0;
	float dianxiaoer_pos_y = 0;

	uintptr_t scene_id_addr = 0;
	std::string scene;
	unsigned int m_scene_id = 0;

	bool moveing = false;

	HANDLE pid;
	HWND hwnd;
	RECT rect;
	HANDLE hProcess = 0;
	HMODULE hNtdll = 0;
	HMODULE mhmainDllBase = 0;
	PFN_NtReadVirtualMemory pNtReadVirtualMemory;

	Step step = Step(datu_step);
};

class GoodMorning {
public:
	GoodMorning();

	void init();
	void hook_data();
	void work();
	void test();


	bool waiting = false;
	std::vector<MyWindowInfo> winsInfo;
	HANDLE hSerial;
private:

};

std::vector<DWORD> FindPidsByName(const wchar_t* name);
HMODULE getProcessModulesAddress(HANDLE hProcess, const TCHAR* moduleName);
DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);
cv::Mat hwnd2mat(HWND hwnd);
cv::Point MatchingRectPos(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
cv::Point MatchingRectPos(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
cv::Point MatchingRectLeftTop(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
bool MatchingRect(HWND hwnd, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
cv::Mat MatchingMethod(cv::Mat image, cv::Mat templ, cv::Mat mask, double threshold, int match_method);
cv::Point getMatchLoc(cv::Mat result, double threshold, int match_method, int width, int height);
cv::Point WaitMatchingRectPos(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
bool WaitMatchingRect(HWND hwnd, cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = 0.78, int match_method = cv::TM_CCORR_NORMED);
uint64_t getCurrentTimeMilliseconds();

//uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void init_log();

cv::Rect ROI_NULL();


int Serial();
void SerialWrite(const char* data);
void SerialRead();
void serial_click_cur();
void serial_move_human(POINT pos, int mode);
bool mouse_click_human(MyWindowInfo* winfo, POINT pos, int xs, int ys, int mode);
POINT get_cursor_pos(MyWindowInfo* winfo, POINT pos);
bool ClickMatchImage(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path="", double threshold=0.78, int match_method=cv::TM_CCORR_NORMED, int x_fix=0, int y_fix = 0, int xs = 0, int ys = 0, int mode=1, int timeout=500);
void input_alt_xxx(const char* data);
void input_alt_a();
void input_alt_e();
void input_key_xxx(const char* data);
void input_tab();



