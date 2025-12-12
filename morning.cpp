#include "morning.h"
#include "log.h"
#include "astar.h"

#include <fstream> 
#include <filesystem>
#include <cstdlib> // For rand() and srand()
#include <chrono>

#include <setupapi.h>
#include <devguid.h> // For GUID_DEVINTERFACE_COMPORT

#include <io.h>   // Required for Windows _setmode
#include <fcntl.h> // Required for Windows _O_U8TEXT

//using namespace cv;

#include <direct.h>

#pragma comment(lib, "ntdll")
#pragma comment(lib, "setupapi.lib")
// GUID for COM ports
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
// Structure to hold port information
struct ComPortInfo {
	std::wstring portName;
	std::wstring description;
};

namespace fs = std::filesystem;

typedef unsigned long long QWORD;
#define TARGET_APP_NAME L"mhmain.exe"
//#define TARGET_APP_NAME "mhtab.exe"
#define MHMAIN_DLL L"mhmain.dll"
#define MAX_WIN = 10

GoodMorning gm;
auto current_path = fs::current_path();


WindowInfo::WindowInfo(HANDLE processID) {
	pid = processID;
	dianxiaoer_pos_list.push_back(POINT{ 450, 850 });
	dianxiaoer_pos_list.push_back(POINT{ 710, 710 });
	dianxiaoer_pos_list.push_back(POINT{ 570, 450 });
	dianxiaoer_pos_list.push_back(POINT{ 890, 690 });
	dianxiaoer_pos_list.push_back(POINT{ 410, 850 });
	dianxiaoer_pos_list.push_back(POINT{ 230, 750 });
	dianxiaoer_pos_list.push_back(POINT{ 230, 630 });
	dianxiaoer_pos_list.push_back(POINT{ 470, 650 });
	dianxiaoer_pos_list.push_back(POINT{ 750, 610 });
	dianxiaoer_pos_list.push_back(POINT{ 650, 790 });

	changan_yizhanlaoban_pos_list.push_back(POINT{ 5570, 4710 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5570, 4750 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5610, 4730 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5650, 4710 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5650, 4750 });
	init();
}

void WindowInfo::init() {
	dianxiaoer_pos_addr = 0;
	dianxiaoer_pos_x = 0;
	dianxiaoer_pos_y = 0;
	changan_yizhanlaoban_pos_addr = 0;
	changan_yizhanlaoban_pos_x = 0;
	changan_yizhanlaoban_pos_y = 0;
	baotu_target_scene_id = 0;
	baotu_target_pos = { 0, 0 };
	mp3_playing = false;
	f_round = 0;
	step.reset();
}
void WindowInfo::hook_init() {
	hNtdll = GetModuleHandleA("ntdll.dll");
	PFN_NtOpenProcess pNtOpenProcess = (PFN_NtOpenProcess)GetProcAddress(hNtdll, "NtOpenProcess");
	pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(hNtdll, "NtReadVirtualMemory");

	OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
	CLIENT_ID         CID = { (HANDLE)pid, NULL };
	NTSTATUS status = pNtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &OA, &CID);

	mhmainDllBase = getProcessModulesAddress(hProcess, MHMAIN_DLL);

	// 玩家坐标地址
	player_pos_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"83 3D ? ? ? ? FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ?",
		"xx????xxxxxxxxxxxx????x????xxx????x????",
		18);
	if (player_pos_addr == 0) log_error("查找玩家坐标地址失败");

	//log_info("查找场景地址开始:0x%X", winfo.hProcess);
	//// 场景[100,10] (111,111)
	//// B4 F3 CC C6 B9 FA BE B3 5B 31 39 38 2C 32 33 32 5D 00 B6 FE 28 31 31 31 2C 31 31 31 29 00
	//auto map_info_AoB_adr = winfo.PerformAoBScan(winfo.hProcess, 0, "28 31 31 31 2C 31 31 31 29 00", "xxxxxxxxxx");
	//if (map_info_AoB_adr == 0) log_error("查找场景地址失败");
	//else winfo.map_info_addr = map_info_AoB_adr - winfo.map_offset;
	//log_info("查找场景地址结束:0x%X", winfo.hProcess);

	// 场景id
	//48 89 00 48 89 40 08 48 89 40 10 66 C7 40 18 01 01 48 89 05 ? ? ? ? 44 89 3D ? ? ? ?
	scene_id_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"48 89 00 48 89 40 08 48 89 40 10 66 C7 40 18 01 01 48 89 05 ? ? ? ? 44 89 3D ? ? ? ?",
		"xxxxxxxxxxxxxxxxxxxx????xxx????",
		27);
	if (scene_id_addr == 0) log_error("查找场景id地址失败");

	// 店小二结构体静态地址
	location_first_static_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"48 8D 0D ? ? ? ? 48 89 08 48 8B 53 08 48 89 50 08 48 8B 53 10 48 89 50 10",
		"xxx????xxxxxxxxxxxxxxxxxxx",
		3);
	location_second_static_addr = getRelativeCallAddressByAoB(
		hProcess,
		mhmainDllBase,
		"48 8B 08 48 8D 56 40 48 8D 05 ? ? ? ? 48 89 45 E7",
		"xxxxxxxxxx????xxxx",
		10);

	location_dynamic_addr_third_child_first_static_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"48 8D 05 ? ? ? ? 48 89 03 48 8D 4B 30 E8 ? ? ? ?",
		"xxx????xxxxxxxx????",
		3);
}
void WindowInfo::datu() {
	if (fail) return;
	if (moving) {
		if (is_moving()) return;
	}
	if (is_fighting()) {
		handle_datu_fight();
		return;
	}
	SetForegroundWindow(hwnd);
	if (step.current == &to_changan_jiudian) {
		log_info("to_changan_jiudian");
		goto_changanjiudian();
		step.next();
	}
	else if (step.current == &to_dianxiaoer) {
		log_info("to_dianxiaoer");
		update_scene_id();
		if (m_scene_id != 长安酒店) step.set_current(&to_changan_jiudian);
		else {
			move_to_dianxiaoer();
			step.next();
		}
	}
	else if (step.current == &talk_get_baoturenwu) {
		log_info("talk_get_baoturenwu");
		if (talk_to_dianxiaoer()) {
			step.next();
		}
		else {
			move_to_changanjidian_center();
			step.set_current(&to_dianxiaoer);
		}
	}
	else if (step.current == &parse_baotu_task) {
		log_info("parse_baotu_task");
		//close_npc_talk();
		if (!MatchingRectExist(ROI_npc_talk(), img_btn_tingtingwufang) && WaitMatchingRectExist(ROI_npc_talk(), img_npc_dianxiaoer, 1300)){
			// 接任务后关闭对话窗
			close_npc_talk_fast();
			parse_baotu_task_info();
			if (baotu_target_scene_id <= 0 || baotu_target_pos.x <= 0) {
				log_info("解析宝图任务失败，需要手动处理");
				mp3_playing = true;
				fail = true;
			}
			else step.next();
		}
		else{ step.set_current(&to_dianxiaoer); }

	}
	else if (step.current == &goto_target_scene) {
		if (goto_scene(baotu_target_pos, baotu_target_scene_id)) {
			step.next();
		}
		if (moving)handle_health();
	}
	else if (step.current == &attack_qiangdao) {
		attack_npc(baotu_target_pos);
		step.next();
	}
	else if (step.current == &handle_qiangdao) {
		wait_fighting();
		step.next();
	}
	else if (step.current == &baotu_end) {
		init();
	}
}
std::vector<uintptr_t> WindowInfo::ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
{
	std::vector<uintptr_t> res;
	BYTE* buffer = new BYTE[regionSize];

	SIZE_T bytesRead;
	if (pNtReadVirtualMemory(hProcess, (PVOID)startAddress, buffer, regionSize, &bytesRead) == 0)
		//if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
	{
		//log_info("startAddress:0x%llX, regionSize:0x%llX", startAddress, regionSize);
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
				auto matchAddress = reinterpret_cast<uintptr_t>(startAddress) + i;
				//std::cout << i << std::endl;
				//std::cout << "Pattern match found at address: 0x" << std::hex << matchAddress << std::endl;
				res.push_back(matchAddress);
				i += pattern.size(); // 跳过已对比过的字段
			}
		}
	}

	delete[] buffer;
	return res;
}

std::vector<uintptr_t> WindowInfo::PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
{
	// ModuleBase 为0则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
	std::vector<BYTE> aob_byte;
	int start, end;
	start = end = 0;
	char dl = ' ';
	while ((start = pattern.find_first_not_of(dl, end)) != std::string::npos) {
		// str.find(dl, start) will return the index of dl
		// from start index
		end = pattern.find(dl, start);
		// substr function return the substring of the
		// original string from the given starting index
		// to the given end index
		auto sub_s = pattern.substr(start, end - start);
		if (sub_s == "?") sub_s = "00";
		aob_byte.push_back(static_cast<BYTE>(stoi(sub_s, nullptr, 16)));
	}

	std::vector<uintptr_t> res;
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
			bool scan_condition = false;
			if (ModuleBase > 0) scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE);
			else scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && (memoryInfo.Type == MEM_PRIVATE);
			// Check if the memory region is accessible and not reserved
			if (scan_condition)
				//if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE))
			{
				// Scan the memory region
				auto matchAddress = ScanMemoryRegionEx(hProcess, memoryInfo.BaseAddress, memoryInfo.RegionSize, aob_byte, mask);
				if (!matchAddress.empty()) {
					// Insert all elements from vec2 at the end of vec1
					// vec1.end() is the insertion point
					// vec2.begin() and vec2.end() define the range to insert
					res.insert(res.end(), matchAddress.begin(), matchAddress.end());
				}
			}
			address = reinterpret_cast<LPVOID>(reinterpret_cast<char*>(address) + memoryInfo.RegionSize);
		}
		else
		{
			std::cout << "Failed to query memory information. Error code: " << GetLastError() << std::endl;
			break;
		}
	}
	return res;
}

uintptr_t WindowInfo::ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
{
	uintptr_t matchAddress = 0;
	BYTE* buffer = new BYTE[regionSize];

	SIZE_T bytesRead;
	if (pNtReadVirtualMemory(hProcess, (PVOID)startAddress, buffer, regionSize, &bytesRead) == 0)
		//if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
	{
		//log_info("startAddress:0x%llX, regionSize:0x%llX", startAddress, regionSize);
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
				//std::cout << i << std::endl;
				//std::cout << "Pattern match found at address: 0x" << std::hex << matchAddress << std::endl;
				// Additional actions can be performed here
				break;
			}
		}
	}

	delete[] buffer;
	return matchAddress;
}

uintptr_t WindowInfo::PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
{
	// ModuleBase 为0则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
	std::vector<BYTE> aob_byte;
	int start, end;
	start = end = 0;
	char dl = ' ';
	while ((start = pattern.find_first_not_of(dl, end)) != std::string::npos) {
		// str.find(dl, start) will return the index of dl
		// from start index
		end = pattern.find(dl, start);
		// substr function return the substring of the
		// original string from the given starting index
		// to the given end index
		auto sub_s = pattern.substr(start, end - start);
		if (sub_s == "?") sub_s = "00";
		aob_byte.push_back(static_cast<BYTE>(stoi(sub_s, nullptr, 16)));
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
			bool scan_condition = false;
			if (ModuleBase > 0) scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE);
			else scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && (memoryInfo.Type == MEM_PRIVATE);
			// Check if the memory region is accessible and not reserved
			if (scan_condition)
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

uintptr_t WindowInfo::getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
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

uintptr_t WindowInfo::getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
	// adr_offset = rav_offset - 1
	// opcode_adr = AoB_adr + adr_offset
	// StaticAddress - opcode_adr - 5 = rav
	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB, mask);
	if (AoB_adr <= 0) return AoB_adr;

	SIZE_T regionSize = 0x10;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)(AoB_adr + offset), buffer, regionSize, &bytesRead);
	auto rav = *reinterpret_cast<long*>(buffer);
	delete[] buffer;
	return rav + 5 + AoB_adr + offset - 1;
}

bool WindowInfo::MatchingRectExist(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method)
{
	return MatchingRectLoc(roi_rect, templ_path, mask_path, threshold, match_method, MATCHEXIST).x > -1;
}
bool WindowInfo::MatchingGrayRectExist(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method)
{
	auto image = hwnd2mat(hwnd);
	cv::Mat img_gray;
	cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_GRAYSCALE);
	return MatchingLoc(img_gray, roi_rect, templ, mask_path, threshold, match_method).x>-1;
}
POINT WindowInfo::MatchingRectLoc(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method, int loc) {
	auto image = hwnd2mat(hwnd);
	return MatchingLoc(image, roi_rect, templ_path, mask_path, threshold, match_method, loc);
}

//POINT WindowInfo::MatchingRectLoc(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method, int loc) {
//	// Mask image(M) : The mask, a grayscale image that masks the template
//	// Only two matching methods currently accept a mask: TM_SQDIFF and TM_CCORR_NORMED (see below for explanation of all the matching methods available in opencv).
//	// The mask must have the same dimensions as the template
//	// The mask should have a CV_8U or CV_32F depth and the same number of channels as the template image. In CV_8U case, the mask values are treated as binary, i.e. zero and non-zero.
//	// In CV_32F case, the values should fall into [0..1] range and the template pixels will be multiplied by the corresponding mask pixel values.
//	// Since the input images in the sample have the CV_8UC3 type, the mask is also read as color image.
//
//	//In OpenCV, a mask image is a binary image (pixels are typically 0 or 255) used to define a Region of Interest (ROI). 
//	// You can create a mask using several methods, with the two most common approaches being: 
//	//Drawing shapes on a black canvas
//	//Thresholding an existing image
//
//	// cv2.TM_CCORR_NORMED  # 这个对颜色敏感度高，如果目标存在，很容易配到。但是如果目标不存在也很容易误匹配且返回的匹配结果也很高。所以这个方法只适用匹配100%存在的目标
//	// cv::TM_CCOEFF_NORMED 这个通用性好
//	// loc:1匹配中心坐标，2匹配左上角坐标，即原始匹配,3不计算坐标，只匹配是否存在
//	auto image = hwnd2mat(hwnd);
//	cv::Mat image_roi = image;
//	if (!roi_rect.empty()) {
//		// Ensure the ROI is within the image boundaries
//		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);
//
//		// 2. Access the ROI using the Mat operator()
//		// 'image_roi' is a new Mat header pointing to the data in 'image'
//		image_roi = image(roi_rect);
//	}
//	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
//	cv::Mat mask;
//	if (!mask_path.empty())mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
//
//	cv::Mat result;
//	int result_cols = image_roi.cols - templ.cols + 1;
//	int result_rows = image_roi.rows - templ.rows + 1;
//
//	result.create(result_rows, result_cols, CV_32FC1);
//
//	bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
//	try {
//		if (!mask.empty() && method_accepts_mask)
//		{
//			matchTemplate(image_roi, templ, result, match_method, mask);
//		}
//		else
//		{
//			matchTemplate(image_roi, templ, result, match_method);
//		}
//	}
//	catch (cv::Exception& e) {
//		log_error(e.what());
//	}
//	cv::Point matchLoc(-1, -1);
//	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
//
//	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
//	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
//	{
//		matchLoc = minLoc;
//	}
//	else
//	{
//		if (maxVal >= threshold)
//		{
//			matchLoc = maxLoc;
//			if (loc != MATCHEXIST) {
//				if (loc == MATCHCENTER) {
//					//int width = templ.cols;
//					//int height = templ.rows;
//					matchLoc.x += templ.cols / 2;
//					matchLoc.y += templ.rows / 2;
//				}
//				if (!roi_rect.empty()) {
//					matchLoc.x += roi_rect.x;
//					matchLoc.y += roi_rect.y;
//				}
//				//log_info("matchLoc:%d, %d", matchLoc.x, matchLoc.y);
//			}
//		}
//		if (templ_path == img_cursors_cursor) {
//			log_info("maxVal:%f matchLoc:%d,%d", maxVal, matchLoc.x + templ.cols, matchLoc.y + templ.rows);
//			// --- 6. Draw a rectangle around the best match area ---
//			// The top-left corner is matchLoc. The bottom-right is calculated by adding the template dimensions.
//			rectangle(image, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
//			save_screenshot(image);
//		}
//	}
//	return { matchLoc.x, matchLoc.y };
//}
POINT WindowInfo::WaitMatchingRectLoc(cv::Rect roi_rect, std::string templ_path, int timeout, std::string mask_path, double threshold, int match_method, int loc) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto pos = MatchingRectLoc(roi_rect, templ_path, mask_path, threshold, match_method, loc);
		if (pos.x > 0) return {rect.left + pos.x, rect.top + pos.y};
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时: %s", templ_path.c_str());
			return {-1,-1};
		}
		Sleep(5);
	}
}

bool WindowInfo::WaitMatchingRectExist(cv::Rect roi_rect, std::string templ_path, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto match = MatchingRectExist(roi_rect, templ_path, mask_path, threshold, match_method);
		if (match) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时: %s", templ_path.c_str());
			return false;
		}
		Sleep(5);
	}
}

bool WindowInfo::WaitMatchingRectDisapper(cv::Rect roi_rect, std::string templ_path, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		if (!MatchingRectExist(roi_rect, templ_path, mask_path, threshold, match_method)) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时: %s", templ_path.c_str());
			break;
		}
	}
	return false;
}

void WindowInfo::update_player_float_pos() {
	// 读取更新玩家坐标
	SIZE_T regionSize = 0x8;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)player_pos_addr, buffer, regionSize, &bytesRead);
	player_x = *reinterpret_cast<float*>(buffer);
	player_y = *reinterpret_cast<float*>(buffer + 4);
	delete[] buffer;
	player_pos.x = convert_to_map_pos_x(player_x);
	player_pos.y = convert_to_map_pos_y(player_y);
	log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
}

void WindowInfo::update_scene() {
	// 得到的string内容为"建邺城[25,88]"，可以分解为场景名+玩家坐标
	BYTE* buffer = new BYTE[map_offset];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)map_info_addr, buffer, map_offset, &bytesRead);
	// Use the std::string constructor that accepts a const char*
	 // The cast is necessary to convert unsigned char* to the expected const char*
	std::string str_result(reinterpret_cast<const char*>(buffer));
	size_t start = 0;
	size_t end = 0;
	end = str_result.find("[", start);
	if (end != std::string::npos) {
		scene = str_result.substr(start, end - start);
		// Move the starting point past the delimiter
		start = end + 1;
		end = str_result.find(",", start);
		if (end != std::string::npos) {
			player_pos.x = std::stoi(str_result.substr(start, end - start));
			// Move the starting point past the delimiter
			start = end + 1;
			end = str_result.find("]", start);
			if (end != std::string::npos) {
				player_pos.y = std::stoi(str_result.substr(start, end - start));
				// Move the starting point past the delimiter
				start = end + 1;
			}
		}
	}
	delete[] buffer;
}

void WindowInfo::update_scene_id() {
	// 读取更新场景id
	SIZE_T regionSize = 0x8;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)scene_id_addr, buffer, regionSize, &bytesRead);
	m_scene_id = *reinterpret_cast<unsigned int*>(buffer);
	delete[] buffer;
}

void WindowInfo::scan_npc_pos_addr(int npc) {
	// 这个结构不是每次都会出现的，如果找不到，可以先出去再进来，重复试几次一般都会产生这个内存结构
	// 先找到 #c80c0ff挖宝图任务 这个地址
	//auto wabaoturenwu_AoB_adr = PerformAoBScan(
	//	hProcess,
	//	0,
	//	"23 63 38 30 63 30 66 66 CD DA B1 A6 CD BC C8 CE CE F1 00",
	//	"xxxxxxxxxxxxxxxxxxx");
	//// 然后结构体上方有个地址指针，指向一个结构，这个结构包含店小二的动态坐标
	//// 注意：店小二如果这个离开了玩家视野，这个地址需要重新查找，也就是说这个地址要店小二出现在玩家视野中才会出现
	//SIZE_T regionSize = 0x8;
	//BYTE* buffer = new BYTE[regionSize];
	//SIZE_T bytesRead;
	//pNtReadVirtualMemory(hProcess, (PVOID)(wabaoturenwu_AoB_adr - 0x40), buffer, regionSize, &bytesRead);
	//auto ptr = *reinterpret_cast<QWORD*>(buffer);
	//delete[] buffer;

	//if (bytesRead > 0) {
	//	bytesRead = 0;
	//	buffer = new BYTE[regionSize];
	//	pNtReadVirtualMemory(hProcess, (PVOID)(ptr), buffer, regionSize, &bytesRead);
	//	if (bytesRead > 0) {
	//		dianxiaoer_pos_addr = ptr + 0x4C;
	//		log_info("店小二坐标地址:0x%llX", dianxiaoer_pos_addr);
	//	}
	//	delete[] buffer;
	//}
	uintptr_t pos_addr=-1;
	if (npc == NPC_DIANXIAOER) {
		log_info("查找店小二坐标开始:0x%X", hProcess);
		pos_addr = dianxiaoer_pos_addr;
		dianxiaoer_pos_x = 0;
		dianxiaoer_pos_y = 0;
	}
	else if (npc == NPC_CHANGAN_YIZHANLAOBAN) {
		log_info("查找驿站老板坐标开始:0x%X", hProcess);
		pos_addr = changan_yizhanlaoban_pos_addr;
		changan_yizhanlaoban_pos_x = 0;
		changan_yizhanlaoban_pos_y = 0;
	}
	if (pos_addr == 0) {
		// 结构特点：2个静态地址+一个动态地址，动态地址开头包含6个指针，第1个指针为静态地址，第5和6的指针为空
		std::string struct_AoB;
		auto ptr1 = reinterpret_cast<char*>(&location_first_static_addr);
		for (int i = 0; i < 8; i++) {
			auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
			char hexStr[3];
			sprintf(hexStr, "%2X ", c);
			struct_AoB += hexStr;
		}
		auto ptr2 = reinterpret_cast<char*>(&location_second_static_addr);
		for (int i = 0; i < 8; i++) {
			auto c = *reinterpret_cast<const unsigned char*>(ptr2 + i);
			char hexStr[3];
			sprintf(hexStr, "%2X ", c);
			struct_AoB += hexStr;
		}
		struct_AoB += "? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 00 00 80 3F 00 00 80 3F";
		//std::string struct_AoB = "38 E0 D1 30 9D FB 7F 00 00 98 0D 9B 9B FB 7F 00 00 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 00 00 80 3F 00 00 80 3F";
		//struct_AoB = "9D FB 7F 00 00";
		auto scan_ret = PerformAoBScanEx(
			hProcess,
			0,
			struct_AoB,
			"xxxxxxxxxxxxxxxx????????????????xxxxxxxx");
		for (const auto& item : scan_ret) {
			SIZE_T regionSize = 0x38;
			BYTE* buffer = new BYTE[regionSize];
			BYTE* buffer1 = new BYTE[regionSize];
			SIZE_T bytesRead;
			pNtReadVirtualMemory(hProcess, (PVOID)(item + 0x10), buffer, regionSize, &bytesRead);  // 动态地址
			if (bytesRead > 0) {
				auto heap_add = *reinterpret_cast<QWORD*>(buffer);
				bytesRead = 0;
				pNtReadVirtualMemory(hProcess, (PVOID)heap_add, buffer1, regionSize, &bytesRead);
				if (bytesRead > 0) {
					auto m_static_child_addr1 = *reinterpret_cast<QWORD*>(buffer1);
					if (m_static_child_addr1 == location_dynamic_addr_third_child_first_static_addr) {
						auto heap_child_addr5 = *reinterpret_cast<QWORD*>(buffer1 + 0x20);
						auto heap_child_addr6 = *reinterpret_cast<QWORD*>(buffer1 + 0x28);
						if (heap_child_addr5 == 0 && heap_child_addr6 == 0) {
							auto x = *reinterpret_cast<float*>(buffer1 + 0x30);
							auto y = *reinterpret_cast<float*>(buffer1 + 0x34);
							// 长安酒店内有自己坐标，酒店老板坐标，店小二坐标 酒店老板坐标(910, 570)
							update_player_float_pos();
							if (x != player_x && y != player_y) {
								// 这个结构包含所有NPC和玩家（包括自己）的坐标，所以要做过滤
								if (npc == NPC_DIANXIAOER) {
									if (is_dianxiaoer_pos(x, y)) {
										dianxiaoer_pos_x = x;
										dianxiaoer_pos_y = y;
										dianxiaoer_pos_addr = item;
										log_info("店小二坐标地址:0x%llX", item);
										break;
									}
								}
								else if (npc == NPC_CHANGAN_YIZHANLAOBAN) {
									if (is_changan_yizhanlaoban_pos(x, y)) {
										changan_yizhanlaoban_pos_x = x;
										changan_yizhanlaoban_pos_y = y;
										changan_yizhanlaoban_pos_addr = item;
										log_info("驿站老版坐标地址:0x%llX", item);
										break;
									}
								}
	
							}
						}
					}
				}
			}

			delete[] buffer;
			delete[] buffer1;
		}
		log_info("查找坐标结束:0x%X", hProcess);
	}
}

void WindowInfo::update_npc_pos(int npc) {
	// 读取更新坐标
	uintptr_t pos_addr = -1;
	if (npc == NPC_DIANXIAOER) {
		pos_addr = dianxiaoer_pos_addr;
		dianxiaoer_pos_x = 0;
		dianxiaoer_pos_y = 0;
	}
	else if (npc == NPC_CHANGAN_YIZHANLAOBAN) {
		pos_addr = changan_yizhanlaoban_pos_addr;
		changan_yizhanlaoban_pos_x = 0;
		changan_yizhanlaoban_pos_y = 0;
	}
	if (pos_addr != -1) {
		SIZE_T regionSize = 0x24;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)pos_addr, buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			auto first_static_addr = *reinterpret_cast<uintptr_t*>(buffer);
			if (location_first_static_addr == first_static_addr) {
				auto x = *reinterpret_cast<float*>(buffer + 0x18);
				auto y = *reinterpret_cast<float*>(buffer + 0x1C);
				if (npc == NPC_DIANXIAOER) {
					if (is_dianxiaoer_pos(x, y)) {
						dianxiaoer_pos_x = x;
						dianxiaoer_pos_y = y;
					}
				}
				else if (npc == NPC_CHANGAN_YIZHANLAOBAN) {
					if (is_changan_yizhanlaoban_pos(x, y)) {
						changan_yizhanlaoban_pos_x = x;
						changan_yizhanlaoban_pos_y = y;
					}
				}
			}
			else {
				// 店小二消失，内存释放后，结构体变了
				if (npc == NPC_DIANXIAOER) {
					dianxiaoer_pos_addr = 0;
				}
				else if (npc == NPC_CHANGAN_YIZHANLAOBAN) {
					changan_yizhanlaoban_pos_addr = 0;
				}
			}
		}
		delete[] buffer;
	}

}

void WindowInfo::move_cursor_center_top() {
	serial_move_human({ rect.left + 515, rect.top + 95 }, 0);
}

void WindowInfo::move_cursor_center_bottom() {
	serial_move_human({ rect.left + 515, rect.bottom - 100 }, 0);
}
void WindowInfo::move_cursor_right_top() {
	serial_move_human({ rect.left + 820, rect.top + 125 }, 0);
}
void WindowInfo::open_beibao() {
	move_cursor_center_top();
	for (int i = 0; i < 5; i++) {
		input_alt_e();
		if (WaitMatchingRectExist(ROI_beibao(), img_btn_beibao, 2000)) {
			if (MatchingRectExist(ROI_beibao(), img_btn_package_prop,"",0.95)) {
				ClickMatchImage(ROI_beibao(), img_btn_package_prop);
			}
			break;
		}
	}
}

POINT WindowInfo::open_map() {
	POINT pos = { -1, -1 };
	for (int i = 0; i < 5; i++) {
		input_tab();
		pos = WaitMatchingRectLoc(ROI_map(), img_symbol_map, 2500);
		if (pos.x > 0) {
			break;
		}
	}
	return pos;
}

void WindowInfo::close_map() {
	for (int i = 0; i < 5; i++) {
		input_tab();
		if (WaitMatchingRectDisapper(ROI_map(), img_symbol_map, 2500)) break;
	}
}
void WindowInfo::close_beibao_smart(bool keep) {
	if (!keep && MatchingRectExist(ROI_beibao(), img_btn_beibao)) input_alt_e();
}
POINT WindowInfo::compute_pos_pixel(POINT dst, unsigned int scene_id) {
	// 根据坐标计算相对自己在屏幕上的像素
	POINT px = { 0, 0 };
	//int x_pixel = 0;
	//int y_pixel = 0;
	int center_x = wWidth / 2;  // 中点坐标 1024 / 2 + x_rim
	int center_y = wHeight / 2;  // 中点坐标 768 / 2 + y_rim
	int x_edge = 25;  // 超过这个坐标，人物会在窗口中间
	int y_edge = 19;  // 超过这个坐标，人物会在窗口中间
	int pixel = 20;	 // 20像素一个坐标点

	auto max_loc = get_map_max_loc(scene_id);

	if (player_pos.x <= x_edge) px.x = dst.x * pixel;
	else if (max_loc.x - player_pos.x <= x_edge) px.x = wWidth - (max_loc.x - dst.x) * pixel;
	else px.x = center_x - (player_pos.x - dst.x) * pixel;

	if (player_pos.y <= y_edge) px.y = wHeight - dst.y * pixel;
	else if (max_loc.y - player_pos.y <= y_edge) px.y = (max_loc.y - dst.y) * pixel;
	else px.y = center_y + (player_pos.y - dst.y) * pixel;

	// 如果目的像素靠近窗口边缘，鼠标漂移有时候会不显示游戏光标，需要做修正
	int px_fix = 30;
	if (px.x <= px_fix) px.x = px_fix;
	else if (wWidth - px.x <= px_fix) px.x = wWidth - px_fix;
	if (px.y <= px_fix) px.y = px_fix;
	else if (wHeight - px.y <= px_fix) px.y = wHeight - px_fix;
	return { rect.left + px.x, rect.top + px.y };
}

void WindowInfo::move_to_dianxiaoer() {
	scan_npc_pos_addr(NPC_DIANXIAOER);

	if (!is_near_dianxiaoer() && dianxiaoer_pos_x > 0) {
		auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		// A星寻路
		auto astar_pos = astar(player_pos.x, player_pos.y, dxe_x, dxe_y, m_scene_id, dianxiaoer_valid_distence - 1, dianxiaoer_valid_distence - 1);
		log_info("店小二坐标:%d, %d", dxe_x, dxe_y);
		log_info("A星寻路结果:%d, %d", astar_pos.x, astar_pos.y);
		click_position(astar_pos);
		wait_moving_stop(5000);
	}
}
void WindowInfo::goto_changanjiudian() {
	update_scene_id();
	if (m_scene_id == 长安酒店)return;
	update_player_float_pos();
	POINT jiudian = { 469, 170 };	// 长安酒店入口(464,168)
	if (m_scene_id != 长安城 || !(abs(player_pos.x - jiudian.x) <= 22 && abs(player_pos.y - jiudian.y) <= 18)) {
		// 不在酒店门口，
		log_info("不在酒店门口，使用飞行棋");
		fly_to_changanjiudian();
		update_player_float_pos();
	}
	auto px = compute_pos_pixel(jiudian, m_scene_id);
	mouse_click_human(px, 0, 0, 1);
	moving = true;
}
void WindowInfo::move_to_changanjidian_center() {
	// 有时候离店小二太远，店小二会消失看不见，移动到酒店中间，就不存在这个问题
	if (dianxiaoer_pos_addr == 0) {
		click_position({ 23,13 });
		moving = true;
	}
}
void WindowInfo::from_changan_fly_to_datangguojing() {
	log_info("从长安长安驿站老板飞到大唐国境");
	scan_npc_pos_addr(NPC_CHANGAN_YIZHANLAOBAN);

	if (!is_near_changan_yizhanlaoban() && changan_yizhanlaoban_pos_x > 0) {
		click_position({247,43});  // 这个固定坐标可以和驿站老板对话
		wait_moving_stop(5000);
		update_npc_pos(NPC_CHANGAN_YIZHANLAOBAN);
		update_player_float_pos();
	}
	if (changan_yizhanlaoban_pos_x > 0) {
		auto dst_x = convert_to_map_pos_x(changan_yizhanlaoban_pos_x);
		auto dst_y = convert_to_map_pos_y(changan_yizhanlaoban_pos_y);
		click_position({dst_x, dst_y});
		ClickMatchImage(ROI_npc_talk(), img_btn_shide_woyaoqu,"", gThreshold, gMatchMethod,0,0,0,0,1,2000);
		wait_scene_change(大唐国境);
	}
}
void WindowInfo::from_datangguojing_to_datangjingwai() {
	log_info("从大唐国境到大唐境外");
	move_to_other_scene({ 3,77 }, 大唐境外, 30, 0);
}
void WindowInfo::fly_to_changanjiudian() {
	use_changan777(ROI_changan777_changanjiudian(), false);
}
void WindowInfo::fly_to_changan_yizhan_laoban() {
	use_changan777(ROI_changan777_yizhan_laoban(), false);
}
void WindowInfo::fly_to_scene(long x, long y, unsigned int scene_id) {
	log_info("飞到宝图场景");
	bool turn = true;
	switch (scene_id)
	{
	case 长寿村:
	{
		if ((x <= 86 && y <= 13) || (x <= 62 && y <= 84) || (x <= 76 && 14 <= y && y <= 18)) use_changshoucun777(ROI_changshoucun777_taibaijinxing(), false, turn);
		else if ((87 <= x && y <= 32) || (79 <= x && 12 <= y  && y <= 19) || (62 <= x && 20 <= y && y <= 26)) use_changshoucun777(ROI_changshoucun777_changshoujiaowai(), false, turn);
		else if ((x <= 73 && 85 <= y && y <= 127) || (x <= 82 && 128 <= y && y <= 135)) use_changshoucun777(ROI_changshoucun777_dangpu(), false, turn);
		else if (117 <= x && 124 <= y && y <= 162) use_changshoucun777(ROI_changshoucun777_lucheng_n_qiangzhuan(), true, turn);
		else if (x <= 77 && 136 <= y) use_changshoucun777(ROI_changshoucun777_zhongshusheng(), false, turn);
		else if (78 <= x && 163 <= y) use_changshoucun777(ROI_changshoucun777_fangcunshan(), false, turn);
		else if ((62 <= x && 33 <= y && y <= 76) || (113 <= x && 77 <= y && y <= 87)) {
			use_feixingfu(scene_id);
		}
		else use_changshoucun777(ROI_changshoucun777_lucheng_n_qiangzhuan(), true, turn);
		break;
	}
	case 傲来国:
	{
		if ((x <= 74 && y <= 38) || (x <= 65 && 39 <= y && y <= 60) || (x <= 93 && y <= 30)) use_aolaiguo777(ROI_aolaiguo777_yaodian(), false, turn);
		else if ((121 <= x && y <= 34) || (188 <= x && 35 <= y && y <= 37)) use_aolaiguo777(ROI_aolaiguo777_donghaiwan(), true, turn);
		else if ((143 <= x && 38 <= y && y <= 71) || (170 <= x && 72 <= y && y <= 89)) use_aolaiguo777(ROI_aolaiguo777_dangpu(), true, turn);
		else if (167 <= x && 94 <= y) use_aolaiguo777(ROI_aolaiguo777_huaguoshan(), true, turn);
		else if (71 <= x && x <= 166 && 72 <= y) {
			use_feixingfu(scene_id);
		}
		else if ((x <= 62 && 61 <= y && y <= 112) || (63 <= x && x <= 70 && 75 <= y && y <= 112)) use_aolaiguo777(ROI_aolaiguo777_penglaixiandao(), false, turn);
		else if (x <= 70 && 113 <= y) use_aolaiguo777(ROI_aolaiguo777_nvercun(), false, turn);
		else use_aolaiguo777(ROI_aolaiguo777_qianzhuang(), true, turn);
		break;
	}
	case 朱紫国:
	{
		if (x <= 43  && y <= 22) use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false, turn);
		else if (44 <= x && x <= 111 && y <= 24) use_zhuziguo777(ROI_zhuziguo777_duanmuniangzi(), true, turn);
		else if ((112 <= x && y <= 26) || (90 <= x && x <= 111 && 17 <= y && y <= 32)) use_zhuziguo777(ROI_zhuziguo777_sichouzhilu(), true, turn);
		else if ((112 <= x && 27 <= y && y <= 65) || (104 <= x && x <= 111 && 33 <= y && y <= 65)) use_zhuziguo777(ROI_zhuziguo777_yaodian(), true, turn);
		else if ((107 <= x && 66 <= y && y <= 107) || (114 <= x && 108 <= y)) {
			use_feixingfu(scene_id);
		}
		else if ((39 <= x && x <= 106 && 73 <= y) || (107 <= x && x <= 113 && 107 <= y)) use_zhuziguo777(ROI_zhuziguo777_shenjidaozhang(), false, turn);
		else if (x <= 38 && 71 <= y) use_zhuziguo777(ROI_zhuziguo777_qilinshan(), false, turn);
		else use_zhuziguo777(ROI_zhuziguo777_jiudian(), false, turn);
		break;
	}
	case 西梁女国:
	{
		// 西凉女国：1.飞行符 2.合成旗-朱紫国驿站-西凉女国(这条路复杂而且慢)
		use_feixingfu(scene_id);
		break;
	}
	case 宝象国:
	{
		use_feixingfu(scene_id);
		break;
	}
	case 建邺城:
	{
		// 建邺城：1.飞行符 2.合成旗-傲来东海湾驿站-东海湾-建邺城
		use_feixingfu(scene_id);
		break;
	}
	case 大唐境外:
	{
		// 大唐境外：1.合成旗-朱紫国左下角-大唐境外 2.合成旗-罗道人旁的驿站老板-传送大唐国境-大唐境外
		if (m_scene_id == 朱紫国) {
			goto_scene({ 2,4 }, 朱紫国);
			wait_scene_change(大唐境外);
			update_player_float_pos();
		}
		else if (m_scene_id != 大唐国境) {
			from_datangguojing_to_datangjingwai();
		}
		else {
			bool via_zhuziguo = true;
			if (x <= 504) {
				if(502 <= x and 25 <= y <= 41)via_zhuziguo = false;
				else if(500 <= x and 26 <= y <= 42)via_zhuziguo = false;
				else if (499 <= x and 27 <= y <= 43)via_zhuziguo = false;
				else if (498 <= x and 28 <= y <= 43)via_zhuziguo = false;
				else if (495 <= x and 29 <= y <= 43)via_zhuziguo = false;
				else if (494 <= x <= 502 and 42 <= y <= 56)via_zhuziguo = false;
				else if (493 <= x <= 502 and 42 <= y <= 57)via_zhuziguo = false;
				else if (492 <= x <= 502 and 42 <= y <= 59)via_zhuziguo = false;
				else if (491 <= x and 42 <= y <= 60)via_zhuziguo = false;
				else if (496 <= x and 64 <= y <= 68)via_zhuziguo = false;
				else if (503 <= x and 44 <= y <= 53)via_zhuziguo = false;
			}
			else via_zhuziguo = false;
			if (via_zhuziguo) {
				log_info("从朱紫国到大唐境外");
				use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false);
				click_position_at_edge({ 2, 4 }, 35, -35);
				wait_scene_change(大唐境外);
				update_player_float_pos();
			}
			else {
				log_info("大唐国境到大唐境外");
				use_changan777(ROI_changan777_yizhan_laoban(), false);
				from_changan_fly_to_datangguojing();
			}
		}
		break;
	}
	case 江南野外:
	{
		// 江南野外:1.合成旗-长安右下角-江南野外 2.飞行符-建邺城-江南野外
		log_info("从长安到江南野外");
		use_changan777(ROI_changan777_jiangnanyewai(), false);
		click_position_at_edge({ 542, 4 }, -30, -30);
		wait_scene_change(江南野外);
		update_player_float_pos();
		break;
	}
	case 女儿村:
	{
		// 女儿村：合成旗-傲来国左上角-女儿村
		log_info("从傲来国到女儿村");
		use_aolaiguo777(ROI_aolaiguo777_nvercun(), true);
		click_position_at_edge({ 5, 144 }, 30, 30);
		wait_scene_change(女儿村);
		update_player_float_pos();
		break;
	}
	case 普陀山:
	{
		// 普陀山：合成旗-长安左下角-大唐国境-普陀接引仙女
		if (m_scene_id == 大唐国境) {
			POINT dst = { 221,60 };
			if (!is_near_loc(dst, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE)) {
				log_info("从大唐国境到普陀接引仙女");
				handle_sheyaoxiang_time();
				move_to_position(dst, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE);
				close_beibao_smart();
				moving = true;
			}
			else {
				log_info("传送普陀山");
				click_position(dst);
				auto pos = WaitMatchingRectLoc(ROI_npc_talk(), img_btn_shide_woyaoqu);
				if (pos.x > 0) {
					wait_scene_change(普陀山);
					update_player_float_pos();
				}
			}
		}
		else {
			log_info("从长安到大唐国境");
			use_changan777(ROI_changan777_datangguojing(), false);
			click_position_at_edge({ 8,5 }, 30, -30);
			wait_scene_change(大唐国境);
			update_player_float_pos();
		}
		break;
	}
	case 五庄观:
	{
		// 五庄观:合成旗-长安城驿站老板-大唐国境-大唐境外-五庄观
		if (m_scene_id == 大唐国境) {
			from_datangguojing_to_datangjingwai();
		}
		else if (m_scene_id == 大唐境外) {
			log_info("从大唐境外到五庄观");
			move_to_other_scene({ 636,76 }, 五庄观,-30,0,true);
		}
		else {
			use_changan777(ROI_changan777_yizhan_laoban(), false);
			from_changan_fly_to_datangguojing();
		}
		break;
	}
	case 化生寺:
	{
		log_info("从长安到化生寺");
		use_changan777(ROI_changan777_huashengsi(), false);
		click_position_at_edge({ 511, 274 },0,30);
		wait_scene_change(化生寺);
		update_player_float_pos();
		break;
	}
	case 东海湾:
	{
		log_info("从傲来国到东海湾");
		use_aolaiguo777(ROI_aolaiguo777_donghaiwan(), false);
		click_position({ 168, 15 });
		auto pos = WaitMatchingRectLoc(ROI_npc_talk(), img_btn_shide_woyaoqu);
		if (pos.x > 0) {
			wait_scene_change(东海湾);
			update_player_float_pos();
		}
		break;
	}
	case 狮驼岭:
	{
		log_info("从朱紫国到狮驼岭");
		use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false);
		click_position_at_edge({ 3, 4 },30,0);
		wait_scene_change(大唐境外);
		click_position_at_edge({ 5, 49 }, 30, 0);
		wait_scene_change(狮驼岭);
		update_player_float_pos();
		break;
	}
	case 长安国子监:
	{
		if (m_scene_id == 长安城) {
			move_to_other_scene({ 225,65 }, 长安国子监);//todo
		}
		else {
			fly_to_changanjiudian();
		}
		break;
	}
	case 地府:
	{
		log_info("寻路到地府");
		if (m_scene_id == 大唐国境) {
			move_to_other_scene({ 48,328 }, 地府, 0, 30);
		}
		else {
			fly_to_changan_yizhan_laoban();
			from_changan_fly_to_datangguojing();
		}
		break;
	}
	case 建邺杂货店:
	{
		break;
	}
	case 花果山:
	{
		break;
	}
	default:
		break;
	}
}
bool WindowInfo::goto_scene(POINT dst, unsigned int scene_id) {
	update_scene_id();
	if (m_scene_id == scene_id) {
		// 已处在目的场景，走路过去
		move_to_position(dst,NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE);
		close_beibao_smart();
		return true;
	}
	// 跨地图，需要用飞行棋或走路
	fly_to_scene(dst.x, dst.y, scene_id);
	return false;
}
void WindowInfo::use_beibao_prop(const char* image, bool turn, bool keep) {
	if (turn) open_beibao(); // 这里的动作是：用完道具后是否关闭背包。打开背包使用飞行旗的时候，背包自动关闭了，不需要再关闭背包
	ClickMatchImage(ROI_beibao_props(), image, "", gThreshold, gMatchMethod, 0, 0, 0, 0, 2, 3000);
	if (!keep) {
		//Sleep(150);
		input_alt_e();
	}
}

void WindowInfo::use_changan777(cv::Rect roi, bool move, bool turn, bool keep) {
	log_info("使用长安合成旗");
	use_beibao_prop(img_props_red_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, img_btn_flag_loc, 5000, "", 0.85);
	mouse_click_human(flag_loc);
	wait_scene_change(长安城);
	update_player_float_pos();
}

void WindowInfo::use_zhuziguo777(cv::Rect roi, bool move, bool turn, bool keep) {
	log_info("使用朱紫国合成旗");
	use_beibao_prop(img_props_white_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, img_btn_flag_loc, 4000, "", 0.85);
	mouse_click_human(flag_loc);
	wait_scene_change(朱紫国);
	update_player_float_pos();
}

void WindowInfo::use_changshoucun777(cv::Rect roi, bool move, bool turn, bool keep) {
	log_info("使用长寿村合成旗");
	use_beibao_prop(img_props_green_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, img_btn_flag_loc, 4000, "", 0.85);
	mouse_click_human(flag_loc);
	wait_scene_change(长寿村);
	update_player_float_pos();
}

void WindowInfo::use_aolaiguo777(cv::Rect roi, bool move, bool turn, bool keep) {
	log_info("使用傲来国合成旗");
	use_beibao_prop(img_props_yellow_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, img_btn_flag_loc, 4000, "", 0.85);
	if (flag_loc.x < 0) {
		if (roi == ROI_aolaiguo777_qianzhuang()) flag_loc = WaitMatchingRectLoc(ROI_aolaiguo777_yaodian(), img_btn_flag_loc, 0, "", 0.85);
	}
	mouse_click_human(flag_loc);
	wait_scene_change(傲来国);
	update_player_float_pos();
}

void WindowInfo::use_feixingfu(unsigned int scene_id) {
	log_info("使用飞行符");
	move_cursor_center_bottom();
	input_f1();
	cv::Rect roi;
	const char* flag_image = nullptr;
	switch (scene_id)
	{
	case 建邺城:
	{
		flag_image = img_symbol_feixingfu_jianyecheng;
		roi = ROI_feixingfu_jianyecheng();
		break;
	}
	case 西梁女国:
	{
		flag_image = img_symbol_feixingfu_xiliangnvguo;
		roi = ROI_feixingfu_xiliangnvguo();
		break;
	}
	case 宝象国:
	{
		flag_image = img_symbol_feixingfu_baoxiangguo;
		roi = ROI_feixingfu_baoxiangguo();
		break;
	}
	case 长寿村:
	{
		flag_image = img_symbol_feixingfu_changshoucun;
		roi = ROI_feixingfu_changshoucun();
		break;
	}
	case 傲来国:
	{
		flag_image = img_symbol_feixingfu_aolaiguo;
		roi = ROI_feixingfu_aolaiguo();
		break;
	}
	case 朱紫国:
	{
		flag_image = img_symbol_feixingfu_zhuziguo;
		roi = ROI_feixingfu_zhuziguo();
		break;
	}
	default:
		return;
	}
	auto flag_loc = WaitMatchingRectLoc(roi, flag_image, 3000, "", 0.85);
	if (flag_loc.x < 0) {
		for (int i = 0;i < 3;i++) {
			input_f1();
			flag_loc = WaitMatchingRectLoc(roi, flag_image, 1000, "", 0.85);
			if (flag_loc.x > 0) break;
		}
	}
	mouse_click_human({ flag_loc.x, flag_loc.y });
	wait_scene_change(scene_id);
	update_player_float_pos();
}
void WindowInfo::handle_sheyaoxiang_time() {
	vector<unsigned int>monster_scene_list = { 大唐国境,狮驼岭,普陀山,大唐境外,江南野外,东海湾 };//野外遇怪场景
	bool is_check = false;
	for (auto scene_id : monster_scene_list) {
		if (m_scene_id == scene_id)
		{
			is_check = true;
			break;
		}
	}
	if (!is_check)return;
	log_info("检查摄妖香时间");
	time_t old_time = 0;
	auto utf_8_sheyaoxiang = AnsiToUtf8("摄妖香");
	if (gm.db[player_id].contains(utf_8_sheyaoxiang)) old_time = gm.db[player_id][utf_8_sheyaoxiang];
	time_t now_time = time(NULL);
	if (now_time - old_time >= 1770) {
		log_info("摄妖香已过时，使用摄妖香");
		use_beibao_prop(img_props_sheyaoxiang);
		gm.db[player_id][utf_8_sheyaoxiang] = now_time;
		gm.update_db();
	}
}
bool WindowInfo::wait_scene_change(unsigned int scene_id, int timeout) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		update_scene_id();
		if (m_scene_id == scene_id) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("wait_scene_change超时");
			return false;
		}
		Sleep(5);
	}
}

void WindowInfo::close_npc_talk() {
	if (!ClickMatchImage(ROI_npc_talk(), img_btn_npc_talk_close)) close_npc_talk_fast();
}
void WindowInfo::close_npc_talk_fast() {
	serial_move_human({ rect.left + 670, rect.top + 475 });  // npc对话框很大，不用怕鼠标漂移点不准
}
bool WindowInfo::mouse_click_human(POINT pos, int xs, int ys, int mode) {
	// mode:0不点击，1左键，2右键，5ctrl+左键, 6alt+a攻击
	POINT target_pos = pos;
	POINT mouse_move_pos = { pos.x + xs, pos.y + ys };
	POINT cursor_pos;

	if (mode == 6) {
		mode = 1;
		input_alt_a();
		target_pos.x += 25;
		target_pos.y -= 15;
	}

	auto t_ms = getCurrentTimeMilliseconds();
	do {
		// 因为有鼠标漂移，所以需要多次移动
		if (getCurrentTimeMilliseconds() - t_ms > 5000) {
			log_warn("鼠标点击超时");
			return false;
		}
		serial_move_human(mouse_move_pos, 0);
		cursor_pos = get_cursor_pos(mouse_move_pos);
		if (cursor_pos.x < 0) continue;
		mouse_move_pos = { mouse_move_pos.x + target_pos.x - cursor_pos.x, mouse_move_pos.y + target_pos.y - cursor_pos.y };
		//Sleep(5);
	} while (abs(target_pos.x - cursor_pos.x) > 6 || abs(target_pos.y - cursor_pos.y) > 6);
	switch (mode)
	{
	case 1:
	{
		serial_click_cur();
		break;
	}
	case 2:
	{
		serial_right_click_cur();
		break;
	}
	default:
		break;
	}
	return true;
}

POINT WindowInfo::get_cursor_pos(POINT pos) {
	// 获取鼠标漂移量
	POINT tmp_pos = { -1, -1 };
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		// 循环等待鼠标移动停止
		if (getCurrentTimeMilliseconds() - t_ms > 1000) return tmp_pos;
		//Sleep(5);
		auto image = hwnd2mat(hwnd);
		cv::Rect roi_rect = ROI_cursor(pos) & cv::Rect(0, 0, image.cols, image.rows);
		auto image_roi = image(roi_rect);
		if (image_roi.empty()) continue;
		cv::Mat image_inRange = ThresholdinginRange(image_roi);
		auto cursor_pos = MatchingLoc(image_inRange, ROI_NULL(), img_cursors_cursor, "", 0.69, cv::TM_CCOEFF_NORMED, MATCHLEFTTOP);  // 游戏自身的鼠标，鼠标用cv::TM_CCORR_NORMED方法匹配准确率最高
		if (cursor_pos.x == -1 && cursor_pos.y == -1) continue;
		if (tmp_pos.x == cursor_pos.x && tmp_pos.y == cursor_pos.y)
		{
			tmp_pos.x += rect.left + roi_rect.x;
			tmp_pos.y += rect.top + roi_rect.y;
			return tmp_pos;
		}
		tmp_pos.x = cursor_pos.x;
		tmp_pos.y = cursor_pos.y;
	}

}

bool WindowInfo::ClickMatchImage(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method, int x_fix, int y_fix, int xs, int ys, int mode, int timeout) {
	auto cv_pos = WaitMatchingRectLoc(roi_rect, templ_path, timeout, mask_path, threshold, match_method);
	if (cv_pos.x < 0) return false;
	return mouse_click_human({cv_pos.x + x_fix, cv_pos.y + y_fix }, xs, ys, mode);
}

bool WindowInfo::talk_to_dianxiaoer() {
	if (is_near_dianxiaoer()) {
		// 对话店小二
		auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
		//log_info("店小二坐标:%f,%f", dianxiaoer_pos_x, dianxiaoer_pos_y);
		log_info("店小二坐标:%d,%d", dxe_x, dxe_y);
		click_position({ dxe_x, dxe_y });  // 点击与店小二对话
		auto pos = WaitMatchingRectLoc(ROI_npc_talk(), img_btn_tingtingwufang);
		if (pos.x > 0) {
			mouse_click_human(pos, 0, 0, 1);			// 弹出对话框，接任务
			return true;
		}
	}
	return false;
}

bool WindowInfo::is_dianxiaoer_pos(float x, float y) {
	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
	for (auto& pos : dianxiaoer_pos_list) {
		if (x > 0 && y > 0 && pos.x == x && pos.y == y) return true;
	}
	return false;
}
bool WindowInfo::is_changan_yizhanlaoban_pos(float x, float y) {
	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
	for (auto& pos : changan_yizhanlaoban_pos_list) {
		if (x > 0 && y > 0 && pos.x == x && pos.y == y) return true;
	}
	return false;
}
bool WindowInfo::is_moving() {
	// 判断自己是否在移动
	float x0 = 0;
	float y0 = 0;
	for (int i = 0; i < 2; i++) {
		update_player_float_pos();
		if (player_x != 0 && player_y != 0 && (int)player_x % 10 == 0 && (int)player_y % 10 == 0) {
			// 坐标值都是10的倍数
			if (x0 == player_x && y0 == player_y) {
				moving = false;
				return false;
			}
			x0 = player_x;
			y0 = player_y;
		}
		Sleep(250);
	}
	return true;
}
bool WindowInfo::wait_moving_stop(int timeout) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		if (!is_moving()) return true;
		if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("等待移动超时");
			break;
		}
	}
	return false;
}
bool WindowInfo::is_near_dianxiaoer() {
	update_npc_pos(NPC_DIANXIAOER);
	update_player_float_pos();
	if (dianxiaoer_pos_x > 0) {
		auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		if (abs(dxe_x - player_pos.x) <= (dianxiaoer_valid_distence + 1) && abs(dxe_y - player_pos.y) <= (dianxiaoer_valid_distence + 1)) {
			//  +1是防止鼠标漂移误差
			return true;
		}
	}
	return false;
}
bool WindowInfo::is_near_changan_yizhanlaoban() {
	update_npc_pos(NPC_CHANGAN_YIZHANLAOBAN);
	update_player_float_pos();
	if (changan_yizhanlaoban_pos_addr > 0) {
		auto dxe_x = convert_to_map_pos_x(changan_yizhanlaoban_pos_x);
		auto dxe_y = convert_to_map_pos_y(changan_yizhanlaoban_pos_y);
		if (abs(dxe_x - player_pos.x) <= changan_yizhan_laoban_valid_distence && abs(dxe_x - player_pos.x) <= changan_yizhan_laoban_valid_distence) {
			//  +1是防止鼠标漂移误差
			return true;
		}
	}
	return false;
}
bool WindowInfo::is_near_loc(POINT dst, int near_x, int near_y) {
	if (abs(dst.x - player_pos.x) <= near_x && abs(dst.y - player_pos.x) <= near_y) {
		return true;
	}
	return false;
}
bool WindowInfo::wait_fighting() {
	return WaitMatchingRectExist(ROI_fighting(), img_fight_fighting, 3500,"",0.85);
}
bool WindowInfo::is_fighting() {
	return MatchingRectExist(ROI_fighting(), img_fight_fighting, "", 0.85);
}
void WindowInfo::handle_datu_fight() {
	// 四小人验证处理
	bool four_man = false;
	if (MatchingGrayRectExist(ROI_four_man(), img_fight_fourman_title_gray, "", 0.81)) {if(!four_man)four_man = true;}
	else { four_man = true; }
	if (four_man) {
		for (int i = 0;i < 5;i++) { log_info("***四小人弹窗验证,请手动点击***"); }
		if (!mp3_playing) {
			play_mp3();
			mp3_playing = true;
		}
	}
	else {
		if (mp3_playing) {
			stop_laba();
			mp3_playing = false;
		}
		auto auto_btn_pos = MatchingRectLoc(ROI_fight_action(), img_fight_auto, "", 0.75);
		if (auto_btn_pos.x>0) {click_position_at_edge(auto_btn_pos,-10);}
	}
	// 自动战斗挂机处理
	bool hangup = false;
	auto image = hwnd2mat(hwnd);
	if (MatchingExist(image, ROI_NULL(), img_btn_cancel_auto_round))hangup = true;
	else if (MatchingExist(image, ROI_NULL(), img_btn_cancel_zhanli))hangup = true;
	if (MatchingExist(image, ROI_fight_action(), img_fight_do_hero_action)) {
		if (getCurrentTimeMilliseconds() - wait_hero_action_time > 2.0) {
			wait_hero_action_time = getCurrentTimeMilliseconds();
			f_round += 1;
			gm.db[player_id]["round"] += 1;
			gm.update_db();
		}
	}
	else if (MatchingExist(image, ROI_fight_action(), img_fight_do_peg_action)) {
		if (!hangup) {
			log_info("宠物平A");
			input_alt_a();
		}
	}
	//重置自动战斗挂机剩余回合
	if (gm.db[player_id]["round"] >= randint(19,24) && f_round == 1) {
		log_info("重置自动战斗挂机剩余回合");
		move_cursor_center_top();
		auto btn_pos = WaitMatchingRectLoc(ROI_NULL(), img_btn_reset_auto_round, 500,"",0.95);
		if (btn_pos.x > 0) {
			click_position_at_edge(btn_pos, 0, 10);
			if (WaitMatchingRectExist(ROI_NULL(), img_fight_auto_round30, 300, "", 0.95)) {
				gm.db[player_id]["round"] = 0;
				gm.update_db();
				log_info("重置完毕.");
			}
		}
	}
}
int WindowInfo::convert_to_map_pos_x(float x) {
	// (x - 1) * 20 + 30 = player_x  其中x是地图上显示的坐标
	return (x - 30) / 20 + 1;
}

int WindowInfo::convert_to_map_pos_y(float y) {
	// (max_y - y - 1) * 20 + 30 = player_y  其中y是地图上显示的坐标,max_y是地图y的最大值。例如建邺城y最大值是143
	update_scene_id();
	auto max = get_map_max_loc(m_scene_id);
	return max.y - 1 - (y - 30) / 20;
}

void WindowInfo::parse_baotu_task_info() {
	log_info("开始解析宝图任务内容:0x%X", hProcess);
	// 宝图内容格式：◆xxxx(今天已领取x次)
	// ◆:C6 25
	auto baotu_task_symbol = PerformAoBScan(
		hProcess,
		0,
		"28 00 CA 4E 29 59 F2 5D 86 98 D6 53 23 00 52 ? ? 00 23 00 42 00 21 6B 29 00",  // (今天已领取#R1#B次)
		"xxxxxxxxxxxxxxxx??xxxxxxxxx");
	int symbol_len = 26;
	std::wstring content;
	std::wstring today_times;
	if (baotu_task_symbol > 0) {
		SIZE_T regionSize = 0x140; // 宝图任务的内容长度，这个长度应该够用了
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol - regionSize + symbol_len), buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			today_times = bytes_to_wstring(&buffer[regionSize - symbol_len], symbol_len);
			for (int i = 0; i < bytesRead - 1; i++) {
				if (buffer[i] == 0xC6 && buffer[i + 1] == 0x25) {
					content = bytes_to_wstring(&buffer[i], bytesRead - i - symbol_len);
					break;
				}
			}
		}
		if (!content.empty()) {
			std::vector<std::wstring> all_tags_wstr;
			auto tag_wstr1 = findContentBetweenTags(content, L"#K", L"#B");
			auto tag_wstr2 = findContentBetweenTags(content, L"#R", L"#B");
			all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr1.begin(), tag_wstr1.end());
			all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr2.begin(), tag_wstr2.end());
			for(auto& wstr : all_tags_wstr)
			{
				if (baotu_target_scene_id <= 0) baotu_target_scene_id = get_scene_id_by_name(wstr);
				if (baotu_target_pos.x <= 0) {
					size_t pos = wstr.find(L"，", 0);
					if (pos != std::wstring::npos) {
						baotu_target_pos.x = std::stoi(wstr.substr(0, pos));
						baotu_target_pos.y = std::stoi(wstr.substr(pos + 1, wstr.length()));
					}
				}
			}
			if (baotu_target_scene_id <= 0) log_info("未支持的场景，等待添加");
			auto tag_wstr3 = findContentBetweenTags(today_times, L"(今天已领取#R", L"#B次)");
			baotu_task_count = std::stoi(tag_wstr3.at(0));
			log_info("今天已领取:%d次", baotu_task_count);
		}
	}
	log_info("结束解析宝图任务内容:0x%X", hProcess);
}

void WindowInfo::move_to_position(POINT dst, long active_x, long active_y) {
	// active_x, active_y:NPC在这个坐标范围内对话才有效
	if (abs(player_pos.x - dst.x) <= active_x && (player_pos.y - dst.y) <= active_y) return;  // 有效对话范围内，不用移动
	// A星寻路
	auto astar_pos = astar(player_pos.x, player_pos.y, dst.x, dst.y, m_scene_id, active_x, active_y);
	if (astar_pos.x < 0) {
		if (astar_pos.x < dst.x) astar_pos.x = dst.x - active_x;
		else astar_pos.x = dst.x + active_x;
		if (astar_pos.y < dst.y) astar_pos.y = dst.y - active_y;
		else astar_pos.y = dst.y + active_y;
	}
	if (abs(player_pos.x - astar_pos.x) <= mScreen_x && (player_pos.y - astar_pos.y) <= mScreen_y) click_position(astar_pos);
	else move_via_map(astar_pos);
	moving = true;
}
void WindowInfo::move_via_map(POINT dst) {
	// 目标不在视野范围内，通过地图进行移动
	// dst:目的坐标
	auto map_pos = open_map();
	if (map_pos.x < 0) {
		log_info("打开地图失败");
		return;
	}
	auto max_px = get_map_max_pixel(m_scene_id);  // 地图的像素高和宽
	auto max_loc = get_map_max_loc(m_scene_id);  // 地图的xy最大坐标值
	long x_base = map_pos.x - 10; // 地图左上角实际的像素位置
	long y_base = map_pos.y + 60 + max_px.y; // 地图左上角实际的像素位置
	// 边缘可能点不中，修复一下
	int edge = 7;
	long x = dst.x;
	long y = dst.y;
	if (x < edge) x = edge;
	else if (x >= max_loc.x - edge)x = max_loc.x - edge;
	if (y <= edge)y = edge;
	else if (y >= max_loc.y - edge)y = max_loc.y - edge;
	long x_dst = x_base + round(x * max_px.x / max_loc.x);
	long y_dst = y_base - round(y * max_px.y / max_loc.y);
	mouse_click_human({ x_dst, y_dst });
	close_map();
}
void WindowInfo::move_to_other_scene(POINT door, unsigned int scene_id, int xs, int ys, bool close_beibao) {
	//door：另一个场景入口
	//scene_id:另一个场景id
	handle_sheyaoxiang_time();
	if (is_near_loc(door, mScreen_x, mScreen_x)) {
		click_position_at_edge(door, xs, ys);
		wait_scene_change(scene_id);
	}
	else {
		move_to_position(door);
		if(close_beibao)close_beibao_smart();
		moving = true;
	}
}
bool WindowInfo::click_position(POINT dst, int xs, int ys, int mode) {
	// 通过鼠标点击目的坐标
	// 如果NPC在视野范围内，可以和NPC对话
	// 如果没有NPC，则移动到目的坐标
	update_player_float_pos();
	auto px = compute_pos_pixel({ dst.x, dst.y }, m_scene_id);
	return mouse_click_human(px, xs, xs, mode);
}
void WindowInfo::click_position_at_edge(POINT dst, int xs, int ys) {
	if (!click_position(dst, 0, 0))
		if (!click_position(dst, 0, ys))
			click_position(dst, xs, 0);
}
bool WindowInfo::attack_npc(POINT dst) {
	click_position_at_edge(dst);
	ClickMatchImage(ROI_npc_talk(), img_btn_woshilaishoushinide, "", gThreshold, gMatchMethod, 0, 0, 0, 0, 1, 2000);
	if (wait_fighting()) return true;
	mp3_playing = true;
	return false;
}
unsigned int WindowInfo::get_scene_id_by_name(std::wstring name) {
	unsigned int scene_id = 0;
	if (name == L"狮驼岭") {scene_id = 狮驼岭;}
	else if (name == L"建邺城") {scene_id = 建邺城;}
	else if (name == L"建邺衙门") {scene_id = 建邺衙门;}
	else if (name == L"建邺杂货店") {scene_id = 建邺杂货店;}
	else if (name == L"女儿村") {scene_id = 女儿村;}
	else if (name == L"化生寺") {scene_id = 化生寺;}
	else if (name == L"长寿村") {scene_id = 长寿村;}
	else if (name == L"西梁女国") {scene_id = 西梁女国;}
	else if (name == L"傲来国") {scene_id = 傲来国;}
	else if (name == L"大唐国境") {scene_id = 大唐国境;}
	else if (name == L"地府") {scene_id = 地府;}
	else if (name == L"普陀山") {scene_id = 普陀山;}
	else if (name == L"五庄观") {scene_id = 五庄观;}
	else if (name == L"大唐境外") {scene_id = 大唐境外;}
	else if (name == L"江南野外") {scene_id = 江南野外;}
	else if (name == L"朱紫国") {scene_id = 朱紫国;}
	else if (name == L"宝象国") {scene_id = 宝象国;}
	else if (name == L"花果山") {scene_id = 花果山;}
	else if (name == L"东海湾") {scene_id = 东海湾;}
	return scene_id;
}
bool WindowInfo::low_health(cv::Rect roi, int deadline) {
	vector<const char*> templ_list = {
		img_fight_health_100,
		img_fight_health_95,
		img_fight_health_90,
		img_fight_health_85,
		img_fight_health_80,
		img_fight_health_75,
		img_fight_health_70,
		img_fight_health_65,
		img_fight_health_60,
		img_fight_health_55,
		img_fight_health_50,
	};
	int percent = 105;
	auto image = hwnd2mat(hwnd);
	for (auto temp : templ_list) {
		if (!MatchingExist(image, roi, temp, "", 0.96)) percent -= 5;
		if (percent <= deadline)return true;
	}
	return false;
}
bool WindowInfo::low_mana(cv::Rect roi, int deadline) {
	vector<const char*> templ_list = {
		img_fight_mana_100,
		img_fight_mana_95,
		img_fight_mana_90,
		img_fight_mana_85,
		img_fight_mana_80,
		img_fight_mana_75,
		img_fight_mana_70,
		img_fight_mana_65,
		img_fight_mana_60,
		img_fight_mana_55,
		img_fight_mana_50,
	};
	int percent = 105;
	auto image = hwnd2mat(hwnd);
	for (auto temp : templ_list) {
		if (!MatchingExist(image, roi, temp, "", 0.96)) percent -= 5;
		if (percent <= deadline)return true;
	}
	return false;
}
void WindowInfo::supply_health_hero() {
	if (low_health(ROI_health_hero(),80)) {
		mouse_click_human({ rect.left + 985,rect.top + 9 }, -50, 30, 2);
		move_cursor_right_top();
	}
}
void WindowInfo::supply_health_peg() {
	if (low_health(ROI_health_peg(), 55)) {
		mouse_click_human({ rect.left + 869,rect.top + 9 }, -50, 30, 2);
		move_cursor_right_top();
	}
}
void WindowInfo::supply_mana_hero() {
	if (low_mana(ROI_mana_hero(), 50)) {
		mouse_click_human({ rect.left + 985,rect.top + 21 }, -50, 30, 2);
		move_cursor_right_top();
	}
}
void WindowInfo::handle_health() {
	supply_health_hero();
	supply_health_peg();
	supply_mana_hero();
}

void WindowInfo::UpdateWindowRect() {
	// 实际截图与窗口在屏幕上的坐标有偏差，修正
	// 后台截图，窗口右偏8像素，窗口标题31像素
	int x_fix = 8;
	int y_fix = 31;
	GetWindowRect(hwnd, &rect);
	rect.left += x_fix;
	rect.top += y_fix;
	rect.right = rect.left + x_fix + wWidth;
	rect.bottom = rect.top + y_fix + wHeight;
}
void WindowInfo::SplitTitleAsPlayerId() {
	// 根据窗口title，解析玩家id
	int start, end;
	start = end = 0;
	start = player_name.find_last_of('[');
	if (start != std::string::npos) {
		end = player_name.find_last_of(']');
		player_id = player_name.substr(start + 1, end - start - 1);
	}
}
cv::Rect WindowInfo::ROI_cursor(POINT pos) {
	int len = 160;
	long left = pos.x - rect.left - len;
	long top = pos.y - rect.top - len;
	long width = len * 2;
	long height = len * 2;

	if (pos.x - rect.left < len)
	{
		left = 0;
		//width = len + pos.x - rect.left;
	}
	else if (rect.right - pos.x < len) {
		left = pos.x - len;
		//width = rect.right - left;
	}
	if (pos.y - rect.top < len) {
		top = 0;
		//height = len + pos.y - rect.top;
	}
	else if (rect.bottom - pos.y < len) {
		top = pos.y - len;
		//height = rect.bottom - top;
	}

	cv::Rect roi(left, top, width, height);
	return roi;
}

// 多标签窗口边框像素x:6,y:57
//现在的截图边框像素x:8
// 也就是说，需要x+2，y-57
cv::Rect WindowInfo::ROI_beibao() {
	return cv::Rect(400, 125, 250, 200);
}

cv::Rect WindowInfo::ROI_map() {
	return cv::Rect(120, 150, 820, 510);
}

cv::Rect WindowInfo::ROI_npc_talk() {
	return cv::Rect(180, 330, 680, 330);
}

cv::Rect WindowInfo::ROI_beibao_props() {
	return cv::Rect(100, 100, 850, 650);
}

cv::Rect WindowInfo::ROI_changan777_changanjiudian() {
	log_info("长安合成旗-长安酒店");
	dianxiaoer_pos_addr = 0;
	return cv::Rect(680, 350, 40, 40);
}

cv::Rect WindowInfo::ROI_changan777_yizhan_laoban() {
	log_info("长安合成旗-驿站老板");
	changan_yizhanlaoban_pos_addr = 0;
	return cv::Rect(495, 470, 45, 40);
}

cv::Rect WindowInfo::ROI_changan777_datangguojing() {
	log_info("长安合成旗-大唐国境");
	return cv::Rect(235, 498, 35, 45);
}
cv::Rect WindowInfo::ROI_changan777_jiangnanyewai() {
	log_info("长安合成旗-江南野外");
	return cv::Rect(750, 498, 50, 45);
}
cv::Rect WindowInfo::ROI_changan777_huashengsi() {
	log_info("长安合成旗-化生寺");
	return cv::Rect(710, 240, 80, 60);
}
cv::Rect WindowInfo::ROI_changshoucun777_lucheng_n_qiangzhuan() {
	// 落地坐标:125,110 钱庄
	// 落地坐标:144,141 鲁成
	log_info("长寿村合成旗-鲁成+钱庄");
	return cv::Rect(550, 285, 100, 130);
}
cv::Rect WindowInfo::ROI_changshoucun777_fangcunshan() {
	// 落地坐标:108,203
	log_info("长寿村合成旗-方寸山");
	return cv::Rect(515, 195, 135, 95);
}
cv::Rect WindowInfo::ROI_changshoucun777_zhongshusheng() {
	// 落地坐标:45,160
	log_info("长寿村合成旗-钟书生");
	return cv::Rect(385, 210, 130, 120);
}
cv::Rect WindowInfo::ROI_changshoucun777_dangpu() {
	// 落地坐标:25,112
	log_info("长寿村合成旗-当铺");
	return cv::Rect(385, 330, 115, 125);
}
cv::Rect WindowInfo::ROI_changshoucun777_taibaijinxing() {
	// 落地坐标:44,23
	log_info("长寿村合成旗-太白金星");
	return cv::Rect(385, 455, 135, 120);
}
cv::Rect WindowInfo::ROI_changshoucun777_changshoujiaowai() {
	// 落地坐标:144,6
	log_info("长寿村合成旗-长寿郊外");
	return cv::Rect(520, 455, 130, 120);
}
cv::Rect WindowInfo::ROI_aolaiguo777_nvercun() {
	// 落地坐标:8,141
	log_info("傲来国合成旗-女儿村");
	return cv::Rect(305, 240, 55, 55);
}
cv::Rect WindowInfo::ROI_aolaiguo777_qianzhuang() {
	// 落地坐标:105,55
	log_info("傲来国合成旗-钱庄");
	return cv::Rect(440, 375, 135, 105);
}
cv::Rect WindowInfo::ROI_aolaiguo777_penglaixiandao() {
	// 落地坐标:24,97
	log_info("傲来国合成旗-蓬莱仙岛");
	return cv::Rect(305, 315, 145, 90);
}
cv::Rect WindowInfo::ROI_aolaiguo777_yaodian() {
	// 落地坐标:48,28
	log_info("傲来国合成旗-药店");
	return cv::Rect(305, 405, 145, 120);
}
cv::Rect WindowInfo::ROI_aolaiguo777_donghaiwan() {
	// 落地坐标:165,14
	log_info("傲来国合成旗-东海湾");
	return cv::Rect(550, 445, 180, 80);
}
cv::Rect WindowInfo::ROI_aolaiguo777_dangpu() {
	// 落地坐标:185,62
	log_info("傲来国合成旗-当铺");
	return cv::Rect(550, 350, 180, 95);
}
cv::Rect WindowInfo::ROI_aolaiguo777_huaguoshan() {
	// 落地坐标:215,143
	log_info("傲来国合成旗-花果山");
	return cv::Rect(630, 225, 100, 130);
}
cv::Rect WindowInfo::ROI_zhuziguo777_datangjingwai() {
	// 落地坐标:6,3
	log_info("朱紫国合成旗-大唐境外");
	return cv::Rect(290, 490, 60, 50);
}
cv::Rect WindowInfo::ROI_zhuziguo777_qilinshan() {
	// 落地坐标:4,110
	log_info("朱紫国合成旗-麒麟山");
	return cv::Rect(285, 245, 95, 100);
}
cv::Rect WindowInfo::ROI_zhuziguo777_shenjidaozhang() {
	// 落地坐标:72,98
	log_info("朱紫国合成旗-神机道人");
	return cv::Rect(380, 245, 235, 115);
}
cv::Rect WindowInfo::ROI_zhuziguo777_jiudian() {
	// 落地坐标:53,42
	log_info("朱紫国合成旗-酒店");
	return cv::Rect(300, 360, 240, 105);
}
cv::Rect WindowInfo::ROI_zhuziguo777_duanmuniangzi() {
	// 落地坐标:84,9
	log_info("朱紫国合成旗-端木娘子");
	return cv::Rect(430, 455, 120, 75);
}
cv::Rect WindowInfo::ROI_zhuziguo777_yaodian() {
	// 落地坐标:147,43
	log_info("朱紫国合成旗-药店");
	return cv::Rect(540, 355, 200, 110);
}
cv::Rect WindowInfo::ROI_zhuziguo777_sichouzhilu() {
	// 落地坐标:151,12
	log_info("朱紫国合成旗-丝绸之路");
	return cv::Rect(560, 455, 180, 75);
}
cv::Rect WindowInfo::ROI_feixingfu_baoxiangguo() {
	log_info("飞行符-宝象国");
	return cv::Rect(375, 390, 55, 50);
}
cv::Rect WindowInfo::ROI_feixingfu_xiliangnvguo() {
	log_info("飞行符-西凉女国");
	return cv::Rect(385, 310, 50, 45);
}
cv::Rect WindowInfo::ROI_feixingfu_jianyecheng() {
	log_info("飞行符-建邺城");
	return cv::Rect(610, 415, 65, 50);
}
cv::Rect WindowInfo::ROI_feixingfu_changshoucun() {
	log_info("飞行符-长寿村");
	return cv::Rect(390, 240, 50, 55);
}
cv::Rect WindowInfo::ROI_feixingfu_zhuziguo() {
	log_info("飞行符-朱紫国");
	return cv::Rect(440, 450, 45, 50);
}
cv::Rect WindowInfo::ROI_feixingfu_aolaiguo() {
	log_info("飞行符-傲来国");
	return cv::Rect(755, 465, 70, 60);
}
cv::Rect WindowInfo::ROI_fighting() {
	return cv::Rect(1004, 115, 20, 65);
}
cv::Rect WindowInfo::ROI_health_hero() {
	return cv::Rect(955, 3, 65, 11);
}
cv::Rect WindowInfo::ROI_health_peg() {
	return cv::Rect(835, 3, 65, 11);
}
cv::Rect WindowInfo::ROI_mana_hero() {
	return cv::Rect(955, 15, 65, 11);
}
cv::Rect WindowInfo::ROI_four_man() {
	return cv::Rect(350, 200, 430, 250);
}
cv::Rect WindowInfo::ROI_fight_action() {
	return cv::Rect(850, 100, 150, 550);
}
void WindowInfo::test() {
	update_scene_id();
	update_player_float_pos();
	goto_scene(baotu_target_pos, baotu_target_scene_id);
	printf("\n");
}
TimeProcessor::TimeProcessor() {
	mTime_ms = getCurrentTimeMilliseconds();
}
bool TimeProcessor::timeout(uint64_t time) {
	bool out = getCurrentTimeMilliseconds() - mTime_ms >= time;
	if (out) {
		update();
		log_info("超时");
	}
	return out;
}
void TimeProcessor::update() {
	mTime_ms = getCurrentTimeMilliseconds();
}
bool TimeProcessor::time_wait(uint64_t time) {
	return getCurrentTimeMilliseconds() - mTime_ms >= time;
}

GoodMorning::GoodMorning() {}

void GoodMorning::init() {
	struct stat st = { 0 };
	if (stat("screenshot", &st) == -1) {
		_mkdir("screenshot");
	}
	for (WindowInfo& winfo : this->winsInfo) {
		winfo.UpdateWindowRect();
		winfo.SplitTitleAsPlayerId();
	}
	if (!fs::exists(dbFile)) {
		std::ofstream outFile(dbFile);
		outFile << "{}";
		outFile << std::endl;
		outFile.close();
	}

	std::ifstream configFile(dbFile);
	try
	{
		db = json::parse(configFile);
	}
	catch (json::parse_error& e) {
		db = json();
	}
	bool update = false;
	for (WindowInfo& winfo : this->winsInfo) {
		if (!winfo.player_id.empty()) {
			bool new_content = false;
			auto player = db[winfo.player_id];
			bool no_key = false;
			if (player.empty()) {
				player["title"] = AnsiToUtf8(winfo.player_name);
				player["round"] = 0;
				new_content = true;
			}
			else if (!player.contains("title")) {
				player["title"] = AnsiToUtf8(winfo.player_name);
				new_content = true;
			}
			if (new_content) {
				db[winfo.player_id] = player;
				update = true;
			}
		}
	}
	if (update) update_db();
}

void GoodMorning::hook_data() {
	for (WindowInfo& winfo : this->winsInfo) {
		winfo.hook_init();
	}
}

void GoodMorning::work() {
	while (true) {
		for (WindowInfo& winfo : this->winsInfo) {
			winfo.datu();
		}
		Sleep(10);
	}
}
void GoodMorning::time_pawn_update() {
	time_pawn.update();
	task_pawn.update();
}
void GoodMorning::update_db() {
	// 将 JSON 对象序列化并写入文件，参数 4 表示使用 4 个空格进行美观格式化输出
	std::ofstream o(dbFile);
	o << std::setw(4) << db << std::endl;
}
void GoodMorning::test() {
	//HWND sc = GetDesktopWindow();

	//POINT cursor_pos;
	//GetCursorPos(&cursor_pos);
	//log_info("Mouse position: %d, %d", cursor_pos.x, cursor_pos.y);
	//RECT rect;
	//cv::Rect roi_test;
	//MatchingRectPos(roi_test, "screenshot\\2025-12-09 01-11-22-r30319.png", "object\\btn\\tingtingwufang.png","",0.78);
	for (WindowInfo& winfo : this->winsInfo) {
		//cv::Rect roi_test(450, 338, 300, 300);
		//cv::Rect roi_test;
		//MatchingRectPos(roi_test, "screenshot\\2025-12-09 01-11-22-r30319.png", "object\\btn\\tingtingwufang.png");
		//auto cursor_pos = MatchingRectLeftTop(winfo.hwnd, ROI_NULL(), img_btn_tingtingwufang, "", 0.78, cv::TM_CCOEFF_NORMED);  // 游戏自身的鼠标
		//MatchingRectLoc(cv::Rect(200, 200, 3000, 3000), "screenshot\\2025-12-09 14-19-30-r20911.png", "object\\cursors\\cursor_15.png", "object\\cursors\\cursor_mask.png", 0.78, cv::TM_CCOEFF_NORMED, MATCHLEFTTOP);
		//MatchingRectLoc(roi_test, "screenshot\\2025-12-09 14-19-30-r20911.png", "object\\cursors\\cursor.png", "", 0.78, cv::TM_CCOEFF_NORMED, MATCHCENTER);
		//MatchingRectLoc(cv::Rect(10, 10, 3000, 3000), "screenshot\\2025-12-09 14-19-30-r20911.png", "object\\cursors\\cursor.png", "", 0.78, cv::TM_CCOEFF_NORMED, MATCHLEFTTOP);
		//MatchingRectLoc(cv::Rect(10, 10, 3000, 3000), "screenshot\\2025-12-09 14-19-30-r20911.png", "object\\cursors\\cursor.png", "", 0.78, cv::TM_CCOEFF_NORMED, MATCHEXIST);
		//auto wabaoturenwu_AoB_adr = winfo.PerformAoBScan(
		//	winfo.hProcess,
		//	0,
		//	"20 51 B2 9C FB 7F 00 00 01 00 00 00 01 00 00 00",
		//	"xxxxxxxxxxxxxxxx");
		//hwnd2mat(winfo.hwnd);
		//winfo.scan_dianxiaoer_addr_pos();
		//winfo.update_player_float_pos();
		//winfo.update_scene_id();
		//winfo.click_position({ 189, 121 });
		SetForegroundWindow(winfo.hwnd);
		winfo.update_scene_id();
		winfo.update_player_float_pos();
		winfo.parse_baotu_task_info();
		while (true) {
			//winfo.update_player_float_pos();
			winfo.test();
			//winfo.update_scene_id();
			//winfo.from_changan_fly_to_datangguojing();
			//winfo.click_position({189, 130});
			//winfo.update_scene();
			//winfo.update_dianxiaoer_pos();
			//winfo.move_to_dianxiaoer();
			//winfo.parse_baotu_task_info();
			//winfo.scan_npc_pos_addr();
			//gm.update_db();
			//Sleep(3000);
			//printf("\n");
		}


		//GetWindowRect(winfo.hwnd, &rect);

		//SetForegroundWindow(winfo.hwnd);

		//MatchingRect(winfo.hwnd, ROI_NULL(), "object\\cursors\\cursor.png", "object\\cursors\\cursor_mask.png");

		//MatchingRectPos(ROI_NULL(), "2025-11-26 16-28-51-r15605.png", "object\\cursors\\cursor.png", "object\\cursors\\cursor_mask.png");
		//hwnd2mat(winfo.hwnd);

		//bool res = winfo.ClickMatchImage(ROI_NULL(), "object\\test.png", "", 0.78, cv::TM_CCORR_NORMED, 0, 0, 0, 0, 1);
	}
	//hwnd2mat(sc);
	printf("\n");

}

Step::Step() {}

Step::Step(std::vector<std::string*> step_list) {
	steps = step_list;
	current = steps[0];
}

void Step::reset() {
	current = steps[0];
	end = false;
}

void Step::previous() {
	if (steps.size() == 1) return;
	if (index > 0) {
		index -= 1;
		current = steps[index];
	}
}

void Step::next() {
	if (steps.size() == 1) end = true;
	else if (index < steps.size() - 1) {
		index += 1;
		current = steps[index];
	}
	else end = true;
}

void Step::set_current(std::string* step) {
	// Use std::find to get an iterator to the element
	std::vector<std::string*>::iterator it = std::find(steps.begin(), steps.end(), step);

	// Check if the element was found
	if (it != steps.end()) {
		// Calculate the index using std::distance
		index = std::distance(steps.begin(), it);
		current = step;
	}
}

cv::Rect ROI_NULL() {
	cv::Rect roi_empty;
	return roi_empty;
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

	cv::Mat image_bgr;
	// 2. Use cvtColor with the COLOR_BGRA2BGR conversion code
	cv::cvtColor(src, image_bgr, cv::COLOR_BGRA2BGR);

	save_screenshot(image_bgr);

	return image_bgr;
}
void save_screenshot(cv::Mat& image) {
	auto save_path = fs::current_path() / "screenshot";
	//fs::path filename = "data.txt";
	//fs::path full_path = current_path / filename;
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	char filename[35];
	filename[strftime(filename, sizeof(filename), "%Y-%m-%d %H-%M-%S-", lt)] = '\0';
	//log_info("rand:%d", rand());
	save_path /= filename + std::string("r") + std::to_string(rand()) + ".png";
	cv::imwrite(save_path.string().c_str(), image);
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

			WindowInfo winfo((HANDLE)targetProcessId);
			winfo.hwnd = hwnd;
			winfo.player_name = pszMem;
			gm.winsInfo.push_back(winfo);
			printf("窗口句柄回调成功：%s\n", pszMem);
			VirtualFree(
				pszMem,       // Base address of block
				0,             // Bytes of committed pages
				MEM_RELEASE);  // Decommit the pages
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

uint64_t getCurrentTimeMilliseconds() {
	// 1. Get the current time point
	auto now = std::chrono::system_clock::now();

	// 2. Cast the duration since the epoch to milliseconds
	auto duration_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()
	);

	// 3. Return the count of milliseconds as an integer
	return duration_since_epoch.count();
}
int randint(int min, int max) {
	return (std::rand() % (max - min + 1)) + min;
}
void test() {
	HANDLE hSerial;
	DCB dcbSerialParams = { 0 };
	COMMTIMEOUTS timeouts = { 0 };

	// Open the serial port
	hSerial = CreateFileW(L"\\\\.\\COM1",
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

std::vector<ComPortInfo> EnumerateSerialPorts() {
	std::vector<ComPortInfo> ports;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVINFO_DATA deviceInfoData;
	DWORD i = 0;



	// Get a device information set for the COM port device interface class
	hDevInfo = SetupDiGetClassDevs(
		&GUID_DEVINTERFACE_COMPORT,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
	);

	if (hDevInfo == INVALID_HANDLE_VALUE) {
		std::wcerr << L"SetupDiGetClassDevs failed: " << GetLastError() << std::endl;
		return ports;
	}

	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// Enumerate through all devices in the set
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceInfoData); i++) {
		TCHAR portName[MAX_PATH];
		TCHAR friendlyName[MAX_PATH];
		HKEY hDeviceRegistryKey = NULL;

		// 1. Get the port name (e.g., "COM1") from the registry
		hDeviceRegistryKey = SetupDiOpenDevRegKey(
			hDevInfo,
			&deviceInfoData,
			DICS_FLAG_GLOBAL,
			0,
			DIREG_DEV,
			KEY_READ
		);

		if (hDeviceRegistryKey != INVALID_HANDLE_VALUE) {
			DWORD dwType = 0;
			DWORD dwSize = sizeof(portName);
			if (RegQueryValueEx(hDeviceRegistryKey, TEXT("PortName"), NULL, &dwType, (LPBYTE)portName, &dwSize) == ERROR_SUCCESS) {
				// 2. Get the friendly name (e.g., "Arduino Uno (COM1)")
				if (SetupDiGetDeviceRegistryProperty(
					hDevInfo,
					&deviceInfoData,
					SPDRP_FRIENDLYNAME,
					NULL,
					(PBYTE)friendlyName,
					sizeof(friendlyName),
					NULL
				)) {
					ports.push_back({ portName, friendlyName });
				}
				else if (SetupDiGetDeviceRegistryProperty( // Fallback to SPDRP_DEVICEDESC if friendly name is unavailable
					hDevInfo,
					&deviceInfoData,
					SPDRP_DEVICEDESC,
					NULL,
					(PBYTE)friendlyName,
					sizeof(friendlyName),
					NULL
				)) {
					ports.push_back({ portName, friendlyName });
				}
			}
			RegCloseKey(hDeviceRegistryKey);
		}
	}

	// Clean up
	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (GetLastError() != ERROR_NO_MORE_ITEMS) {
		// Handle potential error during enumeration
		std::wcerr << L"Error during enumeration: " << GetLastError() << std::endl;
	}

	return ports;
}

std::wstring getArduinoLeonardoComPort() {
	std::vector<ComPortInfo> comPorts = EnumerateSerialPorts();

	if (comPorts.empty()) {
		std::wcout << L"No serial ports found." << std::endl;
	}
	else {
		std::wcout << L"Available Serial Ports:" << std::endl;
		for (const auto& port : comPorts) {
			if (std::wcsstr(port.description.c_str(), L"Arduino Leonardo") != nullptr) {
				std::wcout << L"* Port: " << port.portName << L" -> Description: " << port.description << std::endl;
				return port.portName;
			}

		}
	}
	return std::wstring();
}
cv::Mat CannyThreshold(cv::Mat src)
{
	cv::Mat src_gray;
	cv::Mat dst, detected_edges;

	int lowThreshold = 0;
	const int max_lowThreshold = 100;
	const int ratio1 = 3;
	const int kernel_size = 3;

	dst.create(src.size(), src.type());

	cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	blur(src_gray, detected_edges, cv::Size(3, 3));

	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio1, kernel_size);

	dst = cv::Scalar::all(0);

	src.copyTo(dst, detected_edges);

	return dst;
}

static void CannyThreshold(int, void*)
{
	cv::Mat src, src_gray;
	cv::Mat dst, detected_edges;

	int lowThreshold = 0;
	const int max_lowThreshold = 100;
	const int ratio1 = 3;
	const int kernel_size = 3;
	const char* window_name = "Edge Map";

	src = cv::imread("111.png", cv::IMREAD_COLOR); // Load an image
	dst.create(src.size(), src.type());

	cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	blur(src_gray, detected_edges, cv::Size(3, 3));

	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio1, kernel_size);

	dst = cv::Scalar::all(0);

	src.copyTo(dst, detected_edges);

	cv::imwrite("222.png", dst);
}
bool MatchingExist(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method)
{
	return MatchingLoc(image, roi_rect, templ_path, mask_path, threshold, match_method, MATCHEXIST).x > -1;
}
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, cv::Mat templ, std::string mask_path, double threshold, int match_method, int loc) {
	// Mask image(M) : The mask, a grayscale image that masks the template
	// Only two matching methods currently accept a mask: TM_SQDIFF and TM_CCORR_NORMED (see below for explanation of all the matching methods available in opencv).
	// The mask must have the same dimensions as the template
	// The mask should have a CV_8U or CV_32F depth and the same number of channels as the template image. In CV_8U case, the mask values are treated as binary, i.e. zero and non-zero.
	// In CV_32F case, the values should fall into [0..1] range and the template pixels will be multiplied by the corresponding mask pixel values.
	// Since the input images in the sample have the CV_8UC3 type, the mask is also read as color image.

	//In OpenCV, a mask image is a binary image (pixels are typically 0 or 255) used to define a Region of Interest (ROI). 
	// You can create a mask using several methods, with the two most common approaches being: 
	//Drawing shapes on a black canvas
	//Thresholding an existing image

	// cv2.TM_CCORR_NORMED  # 这个对颜色敏感度高，如果目标存在，很容易配到。但是如果目标不存在也很容易误匹配且返回的匹配结果也很高。所以这个方法只适用匹配100%存在的目标
	// cv::TM_CCOEFF_NORMED 这个通用性好
	// loc:1匹配中心坐标，2匹配左上角坐标，即原始匹配,3不计算坐标，只匹配是否存在
	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		image_roi = image(roi_rect);
	}
	cv::Mat mask;
	if (!mask_path.empty())mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);

	cv::Mat result;
	int result_cols = image_roi.cols - templ.cols + 1;
	int result_rows = image_roi.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
	try {
		if (!mask.empty() && method_accepts_mask){matchTemplate(image_roi, templ, result, match_method, mask);}
		else{matchTemplate(image_roi, templ, result, match_method);}
	}
	catch (cv::Exception& e) {
		log_error(e.what());
	}
	cv::Point matchLoc(-1, -1);
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		if (maxVal >= threshold)
		{
			matchLoc = maxLoc;
			if (loc != MATCHEXIST) {
				if (loc == MATCHCENTER) {
					//int width = templ.cols;
					//int height = templ.rows;
					matchLoc.x += templ.cols / 2;
					matchLoc.y += templ.rows / 2;
				}
				if (!roi_rect.empty()) {
					matchLoc.x += roi_rect.x;
					matchLoc.y += roi_rect.y;
				}
				//log_info("matchLoc:%d, %d", matchLoc.x, matchLoc.y);
			}
		}
		//log_info("maxVal:%f matchLoc:%d,%d", maxVal, matchLoc.x, matchLoc.y);
		//if (templ_path == img_cursors_cursor) {
		//	if (match_method == cv::TM_CCOEFF_NORMED) log_info("TM_CCOEFF_NORMED:%f matchLoc:%d,%d", maxVal, matchLoc.x, matchLoc.y);
		//	// --- 6. Draw a rectangle around the best match area ---
		//	// The top-left corner is matchLoc. The bottom-right is calculated by adding the template dimensions.
		//	if (match_method == cv::TM_CCORR_NORMED) log_info("TM_CCORR_NORMED:%f matchLoc:%d,%d", maxVal, matchLoc.x, matchLoc.y);
		//	if (maxVal >= threshold) rectangle(image, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
		//	save_screenshot(image);
		//}
	}
	return { matchLoc.x, matchLoc.y };
}
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method, int loc) {
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	return MatchingLoc(image, roi_rect, templ, mask_path, threshold, match_method, loc);
}
POINT MatchingRectLoc(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path, double threshold, int match_method, int loc) {
	// Mask image(M) : The mask, a grayscale image that masks the template
	// Only two matching methods currently accept a mask: TM_SQDIFF and TM_CCORR_NORMED (see below for explanation of all the matching methods available in opencv).
	// The mask must have the same dimensions as the template
	// The mask should have a CV_8U or CV_32F depth and the same number of channels as the template image. In CV_8U case, the mask values are treated as binary, i.e. zero and non-zero.
	// In CV_32F case, the values should fall into [0..1] range and the template pixels will be multiplied by the corresponding mask pixel values.
	// Since the input images in the sample have the CV_8UC3 type, the mask is also read as color image.

	//In OpenCV, a mask image is a binary image (pixels are typically 0 or 255) used to define a Region of Interest (ROI). 
	// You can create a mask using several methods, with the two most common approaches being: 
	//Drawing shapes on a black canvas
	//Thresholding an existing image

	// cv2.TM_CCORR_NORMED  # 这个对颜色敏感度高
	POINT pos{ -1, -1 };

	auto image = cv::imread((current_path / image_path).string(), cv::IMREAD_COLOR);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	cv::Mat mask;
	if (!mask_path.empty())mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		image_roi = image(roi_rect);
	}
	cv::Mat result;
	int result_cols = image.cols - templ.cols + 1;
	int result_rows = image.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
	try {
		if (!mask.empty() && method_accepts_mask)
		{
			matchTemplate(image_roi, templ, result, match_method, mask);
		}
		else
		{
			matchTemplate(image_roi, templ, result, match_method);
		}
	}
	catch (cv::Exception& e) {
		log_error(e.what());
	}
	cv::Point matchLoc(-1, -1);
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		//log_info("maxVal:%f", maxVal);
		if (maxVal >= threshold)
		{
			matchLoc = maxLoc;
			if (loc != MATCHEXIST) {
				if (loc == MATCHCENTER) {
					//int width = templ.cols;
					//int height = templ.rows;
					matchLoc.x += templ.cols / 2;
					matchLoc.y += templ.rows / 2;
				}
				//if (!roi_rect.empty()) {
				//	matchLoc.x += roi_rect.x;
				//	matchLoc.y += roi_rect.y;
				//}
				//log_info("matchLoc:%d, %d", matchLoc.x, matchLoc.y);
			}
		}
	}
	return { matchLoc.x, matchLoc.y };
}
void ThresholdinginRange()
{
	auto frame = cv::imread("111.png");
	//auto frame = imread("111.png");
	cv::Mat frame_HSV, frame_threshold;
	// Convert from BGR to HSV colorspace
	cv::cvtColor(frame, frame_HSV, cv::COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	// 色调H（Hue）：用角度度量，取值范围为0°~360°，从红色开始按逆时针方向计算，红色为0°，绿色为120°,蓝色为240°。
	// 饱和度S（Saturation）：取值范围为0.0~1.0，值越大，颜色越饱和。用距V轴的距离来度量 
	// 明度V（Value）：取值范围为0(黑色)~1(白色)。轴V=0端为黑色，轴V=1端为白色。
	//The mask will have 255 (white) for pixels within the range, and 0 (black)otherwise.
	cv::inRange(frame_HSV, cv::Scalar(30, 100, 100), cv::Scalar(110, 255, 255), frame_threshold);  // 鼠标
	//cv::inRange(frame_HSV, cv::Scalar(98, 97, 97), cv::Scalar(188, 255, 255), frame_threshold);  // 调试
	//cv::inRange(frame_HSV, cv::Scalar(100, 100, 100), cv::Scalar(140, 255, 255), frame_threshold);  // demo
	// 3. (Optional) Use bitwise AND to show *only* the blue pixels on the original image
	cv::Mat blue_only_result;
	bitwise_and(frame, frame, blue_only_result, frame_threshold);
	auto current_path = fs::current_path();
	current_path /= "mask.png";
	cv::imwrite(current_path.string(), frame_threshold); // A black and white mask

	cv::imwrite("222.png", blue_only_result);// Original colors where blue was found
	//cv::imshow("output", frame_threshold);
	//cv::waitKey(0);
}
cv::Mat ThresholdinginRange(cv::Mat frame)
{
	cv::Mat frame_HSV, frame_threshold;
	// Convert from BGR to HSV colorspace
	cv::cvtColor(frame, frame_HSV, cv::COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	// 色调H（Hue）：用角度度量，取值范围为0°~360°，从红色开始按逆时针方向计算，红色为0°，绿色为120°,蓝色为240°。
	// 饱和度S（Saturation）：取值范围为0.0~1.0，值越大，颜色越饱和。用距V轴的距离来度量 
	// 明度V（Value）：取值范围为0(黑色)~1(白色)。轴V=0端为黑色，轴V=1端为白色。
	//The mask will have 255 (white) for pixels within the range, and 0 (black)otherwise.
	cv::inRange(frame_HSV, cv::Scalar(30, 100, 100), cv::Scalar(110, 255, 255), frame_threshold);  // 鼠标
	//cv::inRange(frame_HSV, cv::Scalar(98, 97, 97), cv::Scalar(188, 255, 255), frame_threshold);  // 调试
	//cv::inRange(frame_HSV, cv::Scalar(100, 100, 100), cv::Scalar(140, 255, 255), frame_threshold);  // demo
	// 3. (Optional) Use bitwise AND to show *only* the blue pixels on the original image
	cv::Mat blue_only_result;
	bitwise_and(frame, frame, blue_only_result, frame_threshold);
	//auto current_path = fs::current_path();
	//current_path /= "mask.png";
	//cv::imwrite(current_path.string(), frame_threshold); // A black and white mask

	//cv::imwrite("222.png", blue_only_result);// Original colors where blue was found
	//cv::imshow("output", frame_threshold);
	//cv::waitKey(0);
	return blue_only_result;
}
int Serial() {
	auto comPortName = getArduinoLeonardoComPort();
	if (comPortName.empty())
	{
		log_error("没找到com");
		return 1;
	}
	DCB dcbSerialParams = { 0 };
	COMMTIMEOUTS timeouts = { 0 };

	// Open the serial port
	// Use "\\\\.\\COM3" for COM ports >= 10, or "COM3" for COM ports < 10
	gm.hSerial = CreateFile((L"\\\\.\\" + comPortName).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,                          // No sharing
		NULL,                       // No security attributes
		OPEN_EXISTING,              // Open existing port
		FILE_ATTRIBUTE_NORMAL,      // Normal file attributes
		NULL);                      // No template file

	if (gm.hSerial == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening serial port." << GetLastError() << std::endl;
		return 1;
	}

	// Get current serial port parameters
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(gm.hSerial, &dcbSerialParams)) {
		std::cerr << "Error getting comm state." << std::endl;
		CloseHandle(gm.hSerial);
		return 1;
	}

	// Set serial port parameters (e.g., 9600 baud, 8 data bits, no parity, 1 stop bit)
	dcbSerialParams.BaudRate = 57616;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.StopBits = ONESTOPBIT;

	if (!SetCommState(gm.hSerial, &dcbSerialParams)) {
		std::cerr << "Error setting comm state." << std::endl;
		CloseHandle(gm.hSerial);
		return 1;
	}

	// Set communication timeouts
	timeouts.ReadIntervalTimeout = 1000;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1000;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(gm.hSerial, &timeouts)) {
		std::cerr << "Error setting timeouts." << std::endl;
		CloseHandle(gm.hSerial);
		return 1;
	}

	// Close the serial port
	//CloseHandle(hSerial);
	return 0;
}

void SerialWrite(const char* data) {
	DWORD bytes_written;
	if (!WriteFile(gm.hSerial, data, strlen(data), &bytes_written, NULL)) {
		std::cerr << "Error writing to serial port." << std::endl;
	}
	else {
		std::cout << "Sent: " << data << std::endl;
	}
}

void SerialRead() {
	// Example: Reading data (simplified, typically done in a loop/thread)
	char buffer[256];
	DWORD bytes_read;
	if (!ReadFile(gm.hSerial, buffer, sizeof(buffer) - 1, &bytes_read, NULL)) {
		std::cerr << "Error reading from serial port." << std::endl;
	}
	else {
		buffer[bytes_read] = '\0';
		std::cout << "Received: " << buffer << std::endl;
	}
}

void serial_move_human(POINT pos, int mode) {
	POINT mouse_pos;
	GetCursorPos(&mouse_pos);
	int64_t snp_len = strlen(MS_MOVE_HUMAN_SYMBOL) + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + 1;
	// 在堆上分配内存
	char* data_buf = new char[snp_len];
	snprintf(data_buf, snp_len, MS_MOVE_HUMAN_SYMBOL, mouse_pos.x, mouse_pos.y, pos.x, pos.y, mode);
	SerialWrite(data_buf);
	// 使用完毕后，必须手动释放内存，防止内存泄漏
	delete[] data_buf;
	SerialRead();
	//Sleep(50);
}

void serial_click_cur() {
	SerialWrite(CLICK_CURRENT_SYMBOL);
	SerialRead();
}
void serial_right_click_cur() {
	SerialWrite(RIGHT_CLICK_CURRENT_SYMBOL);
	SerialRead();
}
void input_alt_xxx(const char* data) {
	int64_t snp_len = strlen(KEY_ALT_xxx) + strlen(data) + 1;
	// 在堆上分配内存
	char* data_buf = new char[snp_len];
	snprintf(data_buf, snp_len, KEY_ALT_xxx, data);
	SerialWrite(data_buf);
	// 使用完毕后，必须手动释放内存，防止内存泄漏
	delete[] data_buf;
	SerialRead();
}

void input_alt_a() {
	input_alt_xxx("a");
}

void input_alt_e() {
	input_alt_xxx("e");
}

void input_key_xxx(const char* data) {
	int64_t snp_len = strlen(KEY_PRESS) + strlen(data) + 1;
	// 在堆上分配内存
	char* data_buf = new char[snp_len];
	snprintf(data_buf, snp_len, KEY_PRESS, data);
	SerialWrite(data_buf);
	// 使用完毕后，必须手动释放内存，防止内存泄漏
	delete[] data_buf;
	SerialRead();
}

void input_tab() {
	input_key_xxx("TAB");
}

void input_f1() {
	input_key_xxx("F1");
}

void stop_laba() {
	log_info("停止喇叭");
	for (int i = 0;i < 5;i++) { 
		SerialWrite(STOP_MP3); 
		SerialRead();
	}
}
void play_mp3() {
	log_info("播放喇叭");
	int64_t snp_len = strlen(PLAY_MP3) + LEN_OF_INT64 + 1;
	// 在堆上分配内存
	char* data_buf = new char[snp_len];
	snprintf(data_buf, snp_len, PLAY_MP3, 1);
	SerialWrite(data_buf);
	// 使用完毕后，必须手动释放内存，防止内存泄漏
	delete[] data_buf;
	SerialRead();
}
std::wstring bytes_to_wstring(const unsigned char* buffer, size_t size) {
	std::vector<unsigned char> raw_bytes(size);
	memcpy(raw_bytes.data(), buffer, size);

	// 1. Reinterpret the unsigned char* data as a const wchar_t*
	const wchar_t* reconstructed_data_ptr = reinterpret_cast<const wchar_t*>(raw_bytes.data());

	// 2. Use the std::wstring constructor that takes a pointer and a character count
	// NOTE: The constructor expects the length in *characters* (wchar_t units), not bytes.
	size_t character_length = size / sizeof(wchar_t);
	std::wstring wstr(reconstructed_data_ptr);
	// Resize to fit exact content end with null terminate
	wstr.resize(character_length);
	return wstr;
}

std::vector<std::wstring> findContentBetweenTags(
	const std::wstring& source,
	const std::wstring& startTag,
	const std::wstring& endTag) {
	std::vector<std::wstring> res;
	size_t pos = 0; // Start search from the beginning

	// Loop until find() returns std::wstring::npos (not found)
	while ((pos = source.find(startTag, pos)) != std::wstring::npos) {
		// 计算实际内容的起始索引：起始标记的位置 + 起始标记的长度
		size_t contentStartIdx = pos + startTag.length();
		size_t endPos = source.find(endTag, contentStartIdx);
		// 检查是否找到了结束标记
		if (endPos != std::wstring::npos) {
			// substr() 的第二个参数是需要提取的长度，而不是结束索引。
			size_t length = endPos - contentStartIdx;
			res.push_back(source.substr(contentStartIdx, length));
			pos = endPos + endTag.length();
		}
	}
	return res;
}
// Function to convert a Windows-1252/ANSI string to UTF-8
std::string AnsiToUtf8(const std::string& ansiStr) {
	int requiredSize = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, nullptr, 0);
	if (requiredSize == 0) return "";

	std::vector<wchar_t> wideStr(requiredSize);
	MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, wideStr.data(), requiredSize);

	requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.data(), -1, nullptr, 0, nullptr, nullptr);
	if (requiredSize == 0) return "";

	std::vector<char> utf8Str(requiredSize);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.data(), -1, utf8Str.data(), requiredSize, nullptr, nullptr);

	return std::string(utf8Str.data());
}
POINT get_map_max_pixel(unsigned int scene_id) {
	switch (scene_id)
	{
		case 女儿村:
			return { 320, 360 };
		case 普陀山:
			return { 369, 276 };
		case 江南野外:
			return { 369, 273 };
		case 大唐境外:
			return { 583, 108 };
		case 朱紫国:
			return { 439, 276 };
		case 傲来国:
			return { 410, 276 };
		case 宝象国:
			return { 441, 331 };
		case 建邺城:
			return { 556, 276 };
		case 五庄观:
			return { 369, 276 };
		case 长寿村:
			return { 267, 350 };
		case 西梁女国:
			return { 371, 281 };
		case 长安酒店:
			break;
		case 长安城:
			break;
		case 东海湾:
			return { 276, 276 };
		case 化生寺:
			return { 379, 276 };
		case 长安国子监:
			return { 119, 119 }; //todo
		case 地府:
			return { 369, 276 };
		case 狮驼岭:
			return { 369, 276 };
		case 建邺杂货店:
			return { 119, 119 }; //todo
		case 花果山:
			return { 119, 119 }; //todo
		default:
			break;
	}
	return { -1, -1 };
}

int main(int argc, const char** argv)
{
	init_log();
	//log_info("日志输出测试");

	// SEED the generator ONCE at the start of the program
	std::srand(static_cast<unsigned int>(time(nullptr)));

	//CannyThreshold(0, 0);

	//ThresholdinginRange();

	Serial();
	//test();
	//SerialWrite(STOP_MP3);
	//serial_move_human(67, 84, 1);

	//const char* send_data = "hkey:WIN\n";
	//SerialWrite(send_data);
	//SerialRead();
	//Sleep(10000);
	for (auto processID : FindPidsByName(TARGET_APP_NAME)) {
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
	}
	Sleep(50);  // 等一下枚举窗口句柄回调完成再执行

	gm.init();
	gm.hook_data();
	//gm.test();
	gm.work();
	return 0;
}


