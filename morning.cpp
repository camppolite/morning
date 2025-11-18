#pragma once 
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <codecvt>
#include <vector>
#include <Psapi.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ntdll")

typedef unsigned _int64 QWORD;
#define TARGET_APP_NAME "mhmain.exe"
#define MHMAIN_DLL L"mhmain.dll"

int pos_x = 0;
int pos_y = 0;
uintptr_t player_pos_addr = 0;
uintptr_t map_info_addr = 0;
uintptr_t dianxiaoer_pos_addr = 0;

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


HANDLE rProcess = 0;
HMODULE hNtdll = 0;
PFN_NtReadVirtualMemory pNtReadVirtualMemory;


std::wstring utf8_to_ws(std::string const& utf8)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
		return cnv.from_bytes(utf8);
	}
	catch (std::range_error& e)
	{
		size_t length = utf8.length();
		std::wstring result;
		result.reserve(length);
		for (size_t i = 0; i < length; i++)
		{
			result.push_back(utf8[i] & 0xFF);
		}
		return result;
	}
}


DWORD FindPidByName(const char* name)
{
	HANDLE h;
	PROCESSENTRY32 singleProcess;
	h = CreateToolhelp32Snapshot( //takes a snapshot of specified processes
		TH32CS_SNAPPROCESS, //get all processes
		0); //ignored for SNAPPROCESS

	singleProcess.dwSize = sizeof(PROCESSENTRY32);

	do {
		//printf("szExeFile: %ws\n", singleProcess.szExeFile);
		if (_wcsicmp(singleProcess.szExeFile, utf8_to_ws(name).c_str()) == 0)
		{
			// 不区分大小写比较
			DWORD pid = singleProcess.th32ProcessID;
			CloseHandle(h);
			return pid;
		}

	} while (Process32Next(h, &singleProcess));

	CloseHandle(h);

	return 0;
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

uintptr_t ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask)
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

uintptr_t PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask)
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

QWORD getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
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


QWORD getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset) {
	// adr_offset = rav_offset - 1
	// opcode_adr = AoB_adr + adr_offset
	// StaticAddress - opcode_adr - 5 = rav
	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB, mask);
	auto rav = *reinterpret_cast<long*>(AoB_adr + offset);
	return rav + 5 + AoB_adr + offset - 1;
}

void init_data() {
	DWORD processID = FindPidByName(TARGET_APP_NAME);

	hNtdll = GetModuleHandleA("ntdll.dll");
	PFN_NtOpenProcess pNtOpenProcess = (PFN_NtOpenProcess)GetProcAddress(hNtdll, "NtOpenProcess");
	pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(hNtdll, "NtReadVirtualMemory");

	OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
	CLIENT_ID         CID = { (HANDLE)processID, NULL };
	NTSTATUS status = pNtOpenProcess(&rProcess, PROCESS_ALL_ACCESS, &OA, &CID);

	auto mhmainDllBase = getProcessModulesAddress(rProcess, MHMAIN_DLL);

	// 读取玩家坐标
	player_pos_addr = getRelativeStaticAddressByAoB(
		rProcess,
		mhmainDllBase,
		"83 3D 00 00 00 00 FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D 00 00 00 00 E8 00 00 00 00 48 8D 0D 00 00 00 00 E8 00 00 00 00",
		"xx????xxxxxxxxxxxx????x????xxx????x????",
		18);
	if (player_pos_addr > 0) {
		SIZE_T regionSize = 0x8;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(rProcess, (PVOID)player_pos_addr, buffer, regionSize, &bytesRead);
		pos_x = *reinterpret_cast<float*>(buffer);
		pos_y = *reinterpret_cast<float*>(buffer + 4);
		delete[] buffer;
	}


	// 场景[100,10] (111,111)
	// B4 F3 CC C6 B9 FA BE B3 5B 31 39 38 2C 32 33 32 5D 00 B6 FE 28 31 31 31 2C 31 31 31 29 00
	auto map_info_AoB_adr = PerformAoBScan(rProcess, 0, "28 31 31 31 2C 31 31 31 29 00", "xxxxxxxxxx");

	if (map_info_AoB_adr > 0) {
		SIZE_T map_offset = 0x14;
		map_info_addr = map_info_AoB_adr - map_offset;
		BYTE* buffer = new BYTE[map_offset];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(rProcess, (PVOID)map_info_addr, buffer, map_offset, &bytesRead);
		delete[] buffer;
	}

	// 店小二坐标
	// 0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA ? ? ? ? ? ? ? ?
	auto dianxiaoer_AoB_adr = PerformAoBScan(
		rProcess,
		0,
		"0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if (dianxiaoer_AoB_adr > 0) {
		dianxiaoer_pos_addr = dianxiaoer_AoB_adr + 0x58;
	}
}

int main(int argc, const char** argv)
{

	init_data();
	return 0;
}


