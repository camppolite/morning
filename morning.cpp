#include "morning.h"
#include "log.h"
#include "astar.h"

#include <filesystem>
#include <cstdlib> // For rand() and srand()
#include <chrono>

#include <setupapi.h>
#include <devguid.h> // For GUID_DEVINTERFACE_COMPORT

//using namespace cv;

cv::Mat src, src_gray;
cv::Mat dst, detected_edges;

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";

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
auto current_path = fs::current_path();


MyWindowInfo::MyWindowInfo(HANDLE processID) {
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
}

std::vector<uintptr_t> MyWindowInfo::ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
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
				std::cout << i << std::endl;
				std::cout << "Pattern match found at address: 0x" << std::hex << matchAddress << std::endl;
				res.push_back(matchAddress);
				i += pattern.size(); // 跳过已对比过的字段
			}
		}
	}

	delete[] buffer;
	return res;
}

std::vector<uintptr_t> MyWindowInfo::PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
{
	// ModuleBase 为0则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
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

uintptr_t MyWindowInfo::ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
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
				std::cout << i << std::endl;
				std::cout << "Pattern match found at address: 0x" << std::hex << matchAddress << std::endl;
				// Additional actions can be performed here
				//break;

				SIZE_T regionSize = 0x20;
				BYTE* buffer = new BYTE[regionSize];
				SIZE_T bytesRead;
				pNtReadVirtualMemory(hProcess, (PVOID)(matchAddress), buffer, regionSize, &bytesRead);
				auto value = *reinterpret_cast<QWORD*>(buffer + 0x10);
				delete[] buffer;

				buffer = new BYTE[regionSize];
				pNtReadVirtualMemory(hProcess, (PVOID)(value), buffer, regionSize, &bytesRead);
				if (bytesRead > 0) {
					auto addr = *reinterpret_cast<QWORD*>(buffer);
					delete[] buffer;

					//buffer = new BYTE[regionSize];
					//pNtReadVirtualMemory(hProcess, (PVOID)(addr), buffer, regionSize, &bytesRead);
					//auto addr2 = *reinterpret_cast<QWORD*>(buffer);
					//delete[] buffer;

					log_info("Pattern match found at address:0x%llX", matchAddress);
					log_info("ScanMemoryRegion:0x%llX", addr);
				}
			}
		}
	}

	delete[] buffer;
	return matchAddress;
}

uintptr_t MyWindowInfo::PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
{
	// ModuleBase 为0则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
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

uintptr_t MyWindowInfo::getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
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

POINT MyWindowInfo::MatchingRectPos(cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method) {
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
	POINT pos = {-1, -1};

	auto image = hwnd2mat(hwnd);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return pos;
	}
	cv::Mat mask;
	if (!mask_path.empty())
	{
		mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
		if (mask.empty())
		{
			log_error("Can't read mask image\n");
			return pos;
		}
	}

	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		cv::Mat image_roi = image(roi_rect);
	}

	auto result = MatchingMethod(image_roi, templ, mask, threshold, match_method);
	auto cv_pos = getMatchLoc(result, threshold, match_method, rect, templ.cols, templ.rows);
	if (cv_pos.x > -1) {
		pos.x += cv_pos.x;
		pos.y += cv_pos.y;
	}
	return pos;
}

void MyWindowInfo::update_player_float_pos() {
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
}

void MyWindowInfo::update_scene() {
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

void MyWindowInfo::update_scene_id() {
	// 读取更新场景id
	SIZE_T regionSize = 0x8;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)scene_id_addr, buffer, regionSize, &bytesRead);
	m_scene_id = *reinterpret_cast<unsigned int*>(buffer);
	delete[] buffer;
}

void MyWindowInfo::scan_dianxiaoer_addr_pos() {
	if (dianxiaoer_pos_addr == 0) {
		log_info("查找店小二坐标开始:0x%X", hProcess);
		dianxiaoer_pos_x = 0;
		dianxiaoer_pos_y = 0;
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

		// 结构特点：2个静态地址+一个动态地址，动态地址开头包含6个指针，第1个指针为静态地址，第5和6的指针为空
		auto static_addr1 = getRelativeStaticAddressByAoB(
			hProcess,
			mhmainDllBase,
			"48 8D 0D ? ? ? ? 48 89 08 48 8B 53 08 48 89 50 08 48 8B 53 10 48 89 50 10",
			"xxx????xxxxxxxxxxxxxxxxxxx",
			3);
		auto static_addr2 = getRelativeCallAddressByAoB(
			hProcess,
			mhmainDllBase,
			"48 8B 08 48 8D 56 40 48 8D 05 ? ? ? ? 48 89 45 E7",
			"xxxxxxxxxx????xxxx",
			10);

		auto static_child_addr1 = getRelativeStaticAddressByAoB(
			hProcess,
			mhmainDllBase,
			"48 8D 05 ? ? ? ? 48 89 03 48 8D 4B 30 E8 ? ? ? ?",
			"xxx????xxxxxxxx????",
			3);

		std::string struct_AoB;
		auto ptr1 = reinterpret_cast<char*>(&static_addr1);
		for (int i = 0; i < 8; i++) {
			auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
			char hexStr[3];
			sprintf(hexStr, "%2X ", c);
			struct_AoB += hexStr;
		}
		auto ptr2 = reinterpret_cast<char*>(&static_addr2);
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
					if (m_static_child_addr1 == static_child_addr1) {
						auto heap_child_addr5 = *reinterpret_cast<QWORD*>(buffer1 + 0x20);
						auto heap_child_addr6 = *reinterpret_cast<QWORD*>(buffer1 + 0x28);
						if (heap_child_addr5 == 0 && heap_child_addr6 == 0) {
							auto x = *reinterpret_cast<float*>(buffer1 + 0x30);
							auto y = *reinterpret_cast<float*>(buffer1 + 0x34);
							// 长安酒店内有自己坐标，酒店老板坐标，店小二坐标 酒店老板坐标(910, 570)
							update_player_float_pos();
							if (x != player_x && y != player_y) {
								// 这个结构包含所有NPC和玩家（包括自己）的坐标，所以要做过滤
								if (is_dianxiaoer_pos(x, y)) {
									dianxiaoer_pos_x = x;
									dianxiaoer_pos_y = y;
									dianxiaoer_pos_addr = item;
									log_info("店小二坐标地址:0x%llX", dianxiaoer_pos_addr);
									break;
								}
							}
						}
					}
				}
			}

			delete[] buffer;
			delete[] buffer1;
		}
		log_info("查找店小二坐标结束:0x%X", hProcess);
	}
}

void MyWindowInfo::update_dianxiaoer_pos() {
	// 读取更新小二坐标
	dianxiaoer_pos_x = 0;
	dianxiaoer_pos_y = 0;
	SIZE_T regionSize = 0x24;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)dianxiaoer_pos_addr, buffer, regionSize, &bytesRead);
	if (bytesRead > 0) {
		dianxiaoer_pos_x = *reinterpret_cast<float*>(buffer + 0x18);
		dianxiaoer_pos_y = *reinterpret_cast<float*>(buffer + 0x1C);
	}
	delete[] buffer;
}

void MyWindowInfo::move_cursor_center_top() {
	POINT pos = { rect.left + 515, rect.top + 150 };
	serial_move_human(pos, 0);
}

void MyWindowInfo::open_beibao() {
	move_cursor_center_top();
	for (int i = 0; i < 5; i++) {
		input_alt_e();
		if (WaitMatchingRect(hwnd, ROI_beibao(), img_btn_beibao, 2000)) {
			if (MatchingRect(hwnd, ROI_beibao(), img_btn_package_prop_640)) {
				ClickMatchImage(this, ROI_beibao(), img_btn_package_prop_640);
			}
			break;
		}
	}
}

void MyWindowInfo::open_map() {
	for (int i = 0; i < 5; i++) {
		input_tab();
		if (WaitMatchingRect(hwnd, ROI_beibao(), img_btn_beibao, 2000)) {
			if (MatchingRect(hwnd, ROI_beibao(), img_btn_package_prop_640)) {
				ClickMatchImage(this, ROI_beibao(), img_btn_package_prop_640);
			}
			break;
		}
	}
}

POINT MyWindowInfo::compute_pos_pixel(POINT dst, unsigned int scene_id) {
	// 根据坐标计算相对自己在屏幕上的像素
	POINT px = { 0, 0 };
	//int x_pixel = 0;
	//int y_pixel = 0;
	int center_x = 512;  // 中点坐标 1024 / 2 + x_rim
	int center_y = 384;  // 中点坐标 768 / 2 + y_rim
	int x_edge = 25;  // 超过这个坐标，人物会在窗口中间
	int y_edge = 19;  // 超过这个坐标，人物会在窗口中间
	int pixel = 20;	 // 20像素一个坐标点

	auto max_loc = get_map_max_loc(scene_id);

	if (player_pos.x <= x_edge) px.x = dst.x * pixel;
	else if (max_loc.x - player_pos.x <= x_edge) px.x = 1024 - (max_loc.x - dst.x) * pixel;
	else px.x = center_x - (player_pos.x - dst.x) * pixel;

	if (player_pos.y <= y_edge) px.y = 768 - dst.y * pixel;
	else if (max_loc.y - player_pos.y <= y_edge) (max_loc.y - dst.y) * pixel;
	else px.y = center_y - (player_pos.y - dst.y) * pixel;

	return px;
}

void MyWindowInfo::move_to_dianxiaoer() {
	scan_dianxiaoer_addr_pos();
	update_dianxiaoer_pos();
	update_player_float_pos();

	auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
	auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);

	// A星寻路
	auto astar_pos = astar(player_pos.x, player_pos.y, dxe_x, dxe_y, m_scene_id, 5, 5);
	mouse_click_human(this, POINT{ astar_pos.x, astar_pos.y }, 0, 0, 1);
}

bool MyWindowInfo::talk_to_dianxiaoer() {
	if (!is_moving() && is_near_dianxiaoer() && is_dianxiaoer_pos(dianxiaoer_pos_x, dianxiaoer_pos_y)) {
		// 对话店小二
		auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		auto px = compute_pos_pixel(POINT{ dxe_x, dxe_y }, m_scene_id);
		log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
		log_info("店小二坐标:%f,%f", dianxiaoer_pos_x, dianxiaoer_pos_y);
		log_info("店小二坐标:%d,%d", dxe_x, dxe_y);
		log_info("相对像素:%d,%d", px.x, px.y);
		log_info("相对坐标:%d,%d", rect.left + px.x, rect.top + px.y);
		hwnd2mat(hwnd);
		mouse_click_human(this, POINT{ rect.left + px.x, rect.top + px.y }, 0, 0, 1);
		return true;
	}
	return false;
}

bool MyWindowInfo::is_dianxiaoer_pos(float x, float y) {
	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
	for (auto& pos : dianxiaoer_pos_list) {
		if (pos.x == x && pos.y == y) return true;
	}
	return false;
}

bool MyWindowInfo::is_moving() {
	// 判断自己是否在移动
	float x0 = 0;
	float y0 = 0;
	for (int i = 0; i < 2; i++) {
		update_player_float_pos();
		if ((int)player_x % 10 == 0 && (int)player_y % 10 == 0) {
			// 坐标值都是10的倍数
			if (x0 == player_x && y0 == player_y) {
				return false;
			}
			x0 = player_x;
			y0 = player_y;
		}
		Sleep(5);
	}
	return true;
}

bool MyWindowInfo::is_near_dianxiaoer() {
	update_dianxiaoer_pos();
	auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
	auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
	if (abs(dxe_x - player_pos.x) <= 5 && abs(dxe_x - player_pos.x) <= 5) {
		return true;
	}
	return false;
}

POINT MyWindowInfo::get_map_max_loc(unsigned int scene_id) {
	POINT pos = {-1, -1};
	switch (scene_id)
	{
	case 长安酒店:
	{
		pos = { 66, 49 };
		break;
	}
	default:
		break;
	}
	return pos;
}

int MyWindowInfo::convert_to_map_pos_x(float x) {
	// (x - 1) * 20 + 30 = player_x  其中x是地图上显示的坐标
	return (x - 30) / 20 + 1;
}

int MyWindowInfo::convert_to_map_pos_y(float y) {
	// (max_y - y - 1) * 20 + 30 = player_y  其中y是地图上显示的坐标,max_y是地图y的最大值。例如建邺城y最大值是143
	update_scene_id();
	auto pos = get_map_max_loc(m_scene_id);
	return pos.y - 1 - (y - 30) / 20;
}

void MyWindowInfo::UpdateWindowRect() {
	GetWindowRect(hwnd, &rect);
	// 后台截图，窗口右偏7像素，窗口标题31像素
	rect.left += 7;
	rect.bottom += 31;
}

cv::Rect MyWindowInfo::ROI_cursor(POINT pos) {
	int len = 160;
	cv::Rect roi(pos.x - len, pos.y - len, len * 2, len * 2);
	if (roi.x < rect.left) roi.x = rect.left;
	if (roi.y < rect.top) roi.y = rect.top;
	if (roi.x + roi.width > rect.right) roi.width = rect.right - rect.left;
	if (roi.y + roi.height > rect.bottom) roi.height = rect.bottom - rect.top;
	if (rect.left >= roi.x + roi.width || rect.top >= roi.x + roi.height || rect.right <= roi.x || rect.bottom <= roi.y)
	{
		cv::Rect roi_error(0, 0, 10, 10);
		return roi_error;
	}
	return roi;
}

cv::Rect MyWindowInfo::ROI_beibao() {
	return cv::Rect(rect.left + 400, rect.top + 180, 250, 200);
}


GoodMorning::GoodMorning() {

}

void GoodMorning::init() {
	for (MyWindowInfo& winfo : this->winsInfo) {
		GetWindowRect(winfo.hwnd, &winfo.rect);
	}
}

void GoodMorning::hook_data() {
	for (MyWindowInfo& winfo : this->winsInfo) {

		winfo.hNtdll = GetModuleHandleA("ntdll.dll");
		PFN_NtOpenProcess pNtOpenProcess = (PFN_NtOpenProcess)GetProcAddress(winfo.hNtdll, "NtOpenProcess");
		winfo.pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(winfo.hNtdll, "NtReadVirtualMemory");

		OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
		CLIENT_ID         CID = { (HANDLE)winfo.pid, NULL };
		NTSTATUS status = pNtOpenProcess(&winfo.hProcess, PROCESS_ALL_ACCESS, &OA, &CID);

		winfo.mhmainDllBase = getProcessModulesAddress(winfo.hProcess, MHMAIN_DLL);

		// 玩家坐标地址
		winfo.player_pos_addr = winfo.getRelativeStaticAddressByAoB(
			winfo.hProcess,
			winfo.mhmainDllBase,
			"83 3D ? ? ? ? FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ?",
			"xx????xxxxxxxxxxxx????x????xxx????x????",
			18);
		if (winfo.player_pos_addr == 0) log_error("查找玩家坐标地址失败");

		//log_info("查找场景地址开始:0x%X", winfo.hProcess);
		//// 场景[100,10] (111,111)
		//// B4 F3 CC C6 B9 FA BE B3 5B 31 39 38 2C 32 33 32 5D 00 B6 FE 28 31 31 31 2C 31 31 31 29 00
		//auto map_info_AoB_adr = winfo.PerformAoBScan(winfo.hProcess, 0, "28 31 31 31 2C 31 31 31 29 00", "xxxxxxxxxx");
		//if (map_info_AoB_adr == 0) log_error("查找场景地址失败");
		//else winfo.map_info_addr = map_info_AoB_adr - winfo.map_offset;
		//log_info("查找场景地址结束:0x%X", winfo.hProcess);

		// 场景id
		//48 89 00 48 89 40 08 48 89 40 10 66 C7 40 18 01 01 48 89 05 ? ? ? ? 44 89 3D ? ? ? ?
		winfo.scene_id_addr = winfo.getRelativeStaticAddressByAoB(
			winfo.hProcess,
			winfo.mhmainDllBase,
			"48 89 00 48 89 40 08 48 89 40 10 66 C7 40 18 01 01 48 89 05 ? ? ? ? 44 89 3D ? ? ? ?",
			"xxxxxxxxxxxxxxxxxxxx????xxx????",
			27);
		if (winfo.scene_id_addr == 0) log_error("查找场景id地址失败");
	}
}

void GoodMorning::work() {
	while (true) {
		for (MyWindowInfo& winfo : this->winsInfo) {
			SetForegroundWindow(winfo.hwnd);
			if (winfo.step.current() == &to_changan_jiudian) {
				log_info("111111");
				winfo.step.next();
			}
			else if (winfo.step.current() == &to_dianxiaoer) {
				if (!winfo.is_near_dianxiaoer()) {
					winfo.move_to_dianxiaoer();
					winfo.step.next();
				}
			}
			else if (winfo.step.current() == &talk_get_baoturenwu) {
				winfo.move_to_dianxiaoer();
				winfo.step.next();
			}
			//switch (winfo.step)
			//{
			//case START:
			//{
			//	SetForegroundWindow(winfo.hwnd);
			//	break;
			//}

			//default:
			//	break;
			//}
		}
	}
}

void GoodMorning::test() {
	HWND sc = GetDesktopWindow();

	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	log_info("Mouse position: %d, %d", cursor_pos.x, cursor_pos.y);
	RECT rect;
	for (MyWindowInfo& winfo : this->winsInfo) {
		//auto wabaoturenwu_AoB_adr = winfo.PerformAoBScan(
		//	winfo.hProcess,
		//	0,
		//	"20 51 B2 9C FB 7F 00 00 01 00 00 00 01 00 00 00",
		//	"xxxxxxxxxxxxxxxx");
		winfo.scan_dianxiaoer_addr_pos();
		while (true) {
			winfo.update_player_float_pos();
			winfo.update_scene_id();

			//winfo.update_scene();
			//winfo.update_dianxiaoer_pos();
			winfo.move_to_dianxiaoer();
			Sleep(3000);
			printf("\n");
		}


		GetWindowRect(winfo.hwnd, &rect);

		SetForegroundWindow(winfo.hwnd);

		//MatchingRect(winfo.hwnd, ROI_NULL(), "object\\cursors\\cursor.png", "object\\cursors\\cursor_mask.png");
		//cv::Rect roi_test(290, 200, 50, 50);
		//MatchingRectPos(ROI_NULL(), "2025-11-26 16-28-51-r8483.png", "object\\cursors\\cursor.png", "object\\cursors\\cursor_mask.png");
		//MatchingRectPos(ROI_NULL(), "2025-11-26 16-28-51-r15605.png", "object\\cursors\\cursor.png", "object\\cursors\\cursor_mask.png");
		//hwnd2mat(winfo.hwnd);

		bool res = ClickMatchImage(&winfo, ROI_NULL(), "object\\test.png", "", 0.78, cv::TM_CCORR_NORMED, 0, 0, 0, 0, 1);
	}
	//hwnd2mat(sc);
	printf("\n");

}

Step::Step() {}

Step::Step(std::vector<std::string*> step_list) {
	steps = step_list;
}

void Step::reset() {
	index = 0;
	end = false;
}

void Step::previous() {
	if (steps.size() == 1) return;
	if (index > 0) index -= 1;
}

void Step::next() {
	if (steps.size() == 1) end = true;
	else if (index < steps.size() - 1) index += 1;
	else end = true;
}

std::string* Step::current() {
	return steps[index];
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

	cv::Mat image_bgr;
	// 2. Use cvtColor with the COLOR_BGRA2BGR conversion code
	cv::cvtColor(src, image_bgr, cv::COLOR_BGRA2BGR);

	auto current_path = fs::current_path();
	//fs::path filename = "data.txt";
	//fs::path full_path = current_path / filename;
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	char filename[35];
	filename[strftime(filename, sizeof(filename), "%Y-%m-%d %H-%M-%S-", lt)] = '\0';
	//log_info("rand:%d", rand());
	current_path /= filename + std::string("r") + std::to_string(rand()) + ".png";
	cv::imwrite(current_path.string().c_str(), image_bgr);

	return image_bgr;
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

static void CannyThreshold(int, void*)
{
	blur(src_gray, detected_edges, cv::Size(3, 3));
	const int ratio = 3;
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);

	dst = cv::Scalar::all(0);

	src.copyTo(dst, detected_edges);
	cv::imwrite("222.png", dst);
	imshow(window_name, dst);
}

cv::Point MatchingRectPos(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path, double threshold, int match_method) {
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
	cv::Point pos(-1, -1);

	auto image = cv::imread((current_path / image_path).string(), cv::IMREAD_COLOR);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return pos;
	}
	cv::Mat mask;
	if (!mask_path.empty())
	{
		mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
		if (mask.empty())
		{
			log_error("Can't read mask image\n");
			return pos;
		}
	}

	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		cv::Mat image_roi = image(roi_rect);
	}

	auto result = MatchingMethod(image_roi, templ, mask, threshold, match_method);
	RECT rect{ 0, 0, 0, 0 };
	return getMatchLoc(result, threshold, match_method, rect, templ.cols, templ.rows);
}

cv::Point WaitMatchingRectPos(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto pos = MatchingRectPos(winfo, roi_rect, templ_path, mask_path, threshold, match_method);
		if (pos.x > 0) return pos;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时: %s", templ_path.c_str());
			return cv::Point(-1, -1);
		}
		Sleep(5);
	}
}

bool WaitMatchingRect(HWND hwnd, cv::Rect roi_rect, std::string templ_path, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto match = MatchingRect(hwnd, roi_rect, templ_path, mask_path, threshold, match_method);
		if (match) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时: %s", templ_path.c_str());
			return false;
		}
		Sleep(5);
	}
}

cv::Point MatchingRectPos(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method) {
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
	cv::Point pos(-1, -1);

	auto image = hwnd2mat(winfo->hwnd);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return pos;
	}
	cv::Mat mask;
	if (!mask_path.empty())
	{
		mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
		if (mask.empty())
		{
			log_error("Can't read mask image\n");
			return pos;
		}
	}

	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		cv::Mat image_roi = image(roi_rect);
	}

	auto result = MatchingMethod(image_roi, templ, mask, threshold, match_method);
	return getMatchLoc(result, threshold, match_method, winfo->rect, templ.cols, templ.rows);
}

cv::Point MatchingRectLeftTop(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method) {
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
	cv::Point pos(-1, -1);

	auto image = hwnd2mat(winfo->hwnd);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return pos;
	}
	cv::Mat mask;
	if (!mask_path.empty())
	{
		mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
		if (mask.empty())
		{
			log_error("Can't read mask image\n");
			return pos;
		}
	}

	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		cv::Mat image_roi = image(roi_rect);
	}

	auto result = MatchingMethod(image_roi, templ, mask, threshold, match_method);
	return getMatchLoc(result, threshold, match_method, winfo->rect, 0, 0);
}

bool MatchingRect(HWND hwnd, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method)
{
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
	auto image = hwnd2mat(hwnd);
	auto templ = cv::imread((current_path / templ_path).string(), cv::IMREAD_COLOR);
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return false;
	}
	cv::Mat mask;
	if (!mask_path.empty())
	{
		mask = cv::imread((current_path / mask_path).string(), cv::IMREAD_COLOR);
		if (mask.empty())
		{
			log_error("Can't read mask image\n");
			return false;
		}
	}

	cv::Mat image_roi = image;
	if (!roi_rect.empty()) {
		// Ensure the ROI is within the image boundaries
		roi_rect = roi_rect & cv::Rect(0, 0, image.cols, image.rows);

		// 2. Access the ROI using the Mat operator()
		// 'image_roi' is a new Mat header pointing to the data in 'image'
		cv::Mat image_roi = image(roi_rect);
	}

	auto result = MatchingMethod(image_roi, templ, mask, threshold, match_method);
	RECT rect{ 0, 0, 0, 0 };
	auto matchLoc = getMatchLoc(result, threshold, match_method, rect, 0, 0);
	return matchLoc.x > -1;
}

cv::Mat MatchingMethod(cv::Mat image, cv::Mat templ, cv::Mat mask, double threshold, int match_method)
{
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
	cv::Mat result;
	if (image.empty() || templ.empty())
	{
		log_error("Can't read one of the images\n");
		return result;
	}

	int result_cols = image.cols - templ.cols + 1;
	int result_rows = image.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
	try {
		if (!mask.empty() && method_accepts_mask)
		{ matchTemplate(image, templ, result, match_method, mask); }
		else
		{ matchTemplate(image, templ, result, match_method); }
	}
	catch (cv::Exception& e) {
		log_error(e.what());
		return result;
	}
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	return result;
}

cv::Point getMatchLoc(cv::Mat result, double threshold, int match_method, RECT win_rect, int width, int height) {
	//int height = image.rows;
	//int width = image.cols;
	cv::Point matchLoc(-1, -1);
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{ matchLoc = minLoc; }
	else
	{
		if (maxVal >= threshold)
		{ matchLoc = maxLoc; }
	}
	log_info("matchLoc:%d, %d", matchLoc.x, matchLoc.y);
	//int height = image.rows;
	//int width = image.cols;
	matchLoc.x += win_rect.left + width / 2;
	matchLoc.y += win_rect.top + height / 2;
	log_info("MatchingRectPos:%d, %d\n", matchLoc.x, matchLoc.y);
	log_info("win rect:%d, %d\n", win_rect.left, win_rect.top);
	return matchLoc;
}

void ThresholdinginRange()
{
	auto frame = cv::imread("cursor.png");
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
	auto current_path = fs::current_path();
	current_path /= "mask.png";
	cv::imwrite(current_path.string(), frame_threshold);
	cv::imshow("output", frame_threshold);
	cv::waitKey(0);
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
}

void serial_click_cur() {
	SerialWrite(CLICK_CURRENT_SYMBOL);
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

bool mouse_click_human(MyWindowInfo* winfo, POINT pos, int xs, int ys, int mode) {
	// mode:0不点击，1左键，2右键，5ctrl+左键, 6alt+a攻击
	POINT target_pos = pos;
	POINT mouse_pos = { pos.x + xs, pos.y + ys };
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
		if (getCurrentTimeMilliseconds() - t_ms > 3500) {
			log_warn("鼠标点击超时");
			return false;
		}
		serial_move_human(mouse_pos, 0);
		cursor_pos = get_cursor_pos(winfo, mouse_pos);
		if (cursor_pos.x < 0) return false;
		mouse_pos = { mouse_pos.x + target_pos.x - cursor_pos.x, mouse_pos.y + target_pos.y - cursor_pos.y };
		Sleep(5);
	} while (abs(target_pos.x - cursor_pos.x) > 5 || abs(target_pos.y - cursor_pos.y) > 5);
	switch (mode)
	{
	case 1:
	{
		serial_click_cur();
		break;
	}
	default:
		break;
	}
	return true;
}

POINT get_cursor_pos(MyWindowInfo* winfo, POINT pos) {
	// 获取鼠标漂移量
	POINT tmp_pos = {-1, -1};
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		// 循环等待鼠标移动停止
		if (getCurrentTimeMilliseconds() - t_ms > 1500) return tmp_pos;
		Sleep(5);
		auto cursor_pos = MatchingRectLeftTop(winfo, winfo->ROI_cursor(pos), img_cursors_cursor, img_cursors_cursor_mask);  // 游戏自身的鼠标
		if (cursor_pos.x == -1 && cursor_pos.y == -1) continue;
		if (tmp_pos.x == cursor_pos.x && tmp_pos.y == cursor_pos.y) return tmp_pos;
		tmp_pos.x = cursor_pos.x;
		tmp_pos.y = cursor_pos.y;
	}
}

bool ClickMatchImage(MyWindowInfo* winfo, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold, int match_method, int x_fix, int y_fix, int xs, int ys, int mode, int timeout) {
	auto cv_pos = WaitMatchingRectPos(winfo, roi_rect, templ_path, timeout, mask_path, threshold, match_method);
	if (cv_pos.x < 0) return false;
	POINT pos = { cv_pos.x + x_fix, cv_pos.y + y_fix };
	return mouse_click_human(winfo, pos, xs, ys, mode);
}


int main(int argc, const char** argv)
{
	init_log();
	//log_info("日志输出测试");

	// SEED the generator ONCE at the start of the program
	std::srand(static_cast<unsigned int>(time(nullptr)));

	//src = imread("111.png", IMREAD_COLOR); // Load an image
	//if (src.empty())
	//{
	//	std::cout << "Could not open or find the image!\n" << std::endl;
	//	std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
	//	return -1;
	//}

	//dst.create(src.size(), src.type());

	//cvtColor(src, src_gray, COLOR_BGR2GRAY);

	//namedWindow(window_name, WINDOW_AUTOSIZE);

	//createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	//CannyThreshold(0, 0);

	//waitKey(0);


	// 1. Define the ROI using cv::Rect(x, y, width, height)
	// Example: A 100x100 pixel region starting at (50, 50) from the top-left corner
	cv::Rect roi_rect(50, 50, 100, 100);

	//ThresholdinginRange();
	//MatchingMethod();

	
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
	//gm.work();
	gm.test();
	return 0;
}


