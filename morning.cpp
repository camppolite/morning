#pragma once 
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <codecvt>
#include <vector>
#include <Psapi.h>
#include <iostream>
#include <string>

typedef unsigned _int64 QWORD;
#define TARGET_APP_NAME "mhmain.exe"
#define MHMAIN_DLL L"mhmain.dll"

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
	if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
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
	MEMORY_BASIC_INFORMATION memoryInfo;
	for (LPVOID address = ModuleBase; address < ModuleBase + ModuleSize;)
	{
		if (VirtualQueryEx(hProcess, address, &memoryInfo, sizeof(memoryInfo)) == sizeof(memoryInfo))
		{
			// Check if the memory region is accessible and not reserved
			if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE))
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
	auto rav = *reinterpret_cast<long*>(AoB_adr + offset);
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

int main(int argc, const char** argv)
{

	DWORD processID = FindPidByName(TARGET_APP_NAME);
	HANDLE rProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

	HMODULE mhmainDllBase = getProcessModulesAddress(rProcess, MHMAIN_DLL);
	std::string player_pos_AoB = "83 3D 00 00 00 00 FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D 00 00 00 00 E8 00 00 00 00 48 8D 0D 00 00 00 00 E8 00 00 00 00";
	const char* player_pos_mask = "xx????xxxxxxxxxxxx????x????xxx????x????";
	uintptr_t player_pos_aob_adr = getRelativeStaticAddressByAoB(rProcess, mhmainDllBase, player_pos_AoB, player_pos_mask, 18);
	return 0;
}