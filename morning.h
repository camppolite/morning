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

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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
#define changan_yizhan_laoban_valid_distence 10  // 与长安驿站老板对话时的最大有效距离
#define MATCHCENTER 1
#define MATCHLEFTTOP 2
#define MATCHEXIST 3

#define NPC_DIANXIAOER 1  // 店小二
#define NPC_CHANGAN_YIZHANLAOBAN 2  // 长安驿站老板

const char* PLAY_MP3 = "mmp3:PLAY_%d\n";
const char* STOP_MP3 = "mmp3:STOP\n";

const char* MS_MOVE_HUMAN_SYMBOL = "movehm:%d,%d,%d,%d,%d\n";  // cx, cy, x, y, mode
const char* CLICK_CURRENT_SYMBOL = "hkeyCC\n";
const char* RIGHT_CLICK_CURRENT_SYMBOL = "hkeyRCC\n";
const char* KEY_ALT_xxx = "hkey:ALT_%s\n";
const char* KEY_PRESS = "hkey:%s\n";

const char* img_btn_beibao = "object\\btn\\beibao.png";
const char* img_btn_package_prop = "object\\btn\\package_prop.png";
const char* img_btn_tingtingwufang = "object\\btn\\tingtingwufang.png";
const char* img_btn_npc_talk_close = "object\\btn\\npc_talk_close.png";
const char* img_btn_flag_loc = "object\\btn\\flag_loc.png";
const char* img_btn_shide_woyaoqu = "object\\btn\\shide_woyaoqu.png";
const char* img_btn_cancel_auto_round = "object\\btn\\cancel_auto_round.png";
const char* img_btn_cancel_zhanli = "object\\btn\\cancel_zhanli.png";
const char* img_btn_reset_auto_round = "object\\btn\\reset_auto_round.png";

const char* img_props_red_777 = "object\\props\\red_777.png";
const char* img_props_white_777 = "object\\props\\white_777.png";
const char* img_props_green_777 = "object\\props\\green_777.png";
const char* img_props_yellow_777 = "object\\props\\yellow_777.png";
const char* img_props_sheyaoxiang = "object\\props\\sheyaoxiang.png";
const char* img_npc_dianxiaoer = "object\\npc\\dianxiaoer.png";

const char* img_fight_fighting = "object\\fight\\fighting.png";
const char* img_fight_health_red = "object\\fight\\health_red.png";
const char* img_fight_health_blue = "object\\fight\\health_blue.png";
const char* img_fight_fourman_title_gray_107_255_0 = "object\\fight\\fourman_title_gray_107_255_0.png";
const char* img_fight_do_hero_action = "object\\fight\\do_hero_action.png";
const char* img_fight_do_peg_action = "object\\fight\\do_peg_action.png";
const char* img_fight_auto = "object\\fight\\auto.png";
const char* img_fight_auto_round30 = "object\\btn\\auto_round30.png";

const char* img_symbol_map = "object\\symbol\\map.png";
const char* img_symbol_feixingfu_xiliangnvguo = "object\\symbol\\feixingfu_xiliangnvguo.png";
const char* img_symbol_feixingfu_baoxiangguo = "object\\symbol\\feixingfu_baoxiangguo.png";
const char* img_symbol_feixingfu_jianyecheng = "object\\symbol\\feixingfu_jianyecheng.png";
const char* img_symbol_feixingfu_changshoucun = "object\\symbol\\feixingfu_changshoucun.png";
const char* img_symbol_feixingfu_aolaiguo = "object\\symbol\\feixingfu_aolaiguo.png";
const char* img_symbol_feixingfu_zhuziguo = "object\\symbol\\feixingfu_zhuziguo.png";

const char* img_cursors_cursor = "object\\cursors\\cursor.png";
const char* img_cursors_cursor_mask = "object\\cursors\\cursor_mask.png";

std::string to_changan_jiudian("to_changan_jiudian");
std::string to_dianxiaoer("to_dianxiaoer");
std::string talk_get_baoturenwu("talk_get_baoturenwu");
std::string parse_baotu_task("parse_baotu_task");
std::string goto_target_scene("goto_target_scene");
std::string attack_qiangdao("attack_qiangdao");
std::string handle_qiangdao("handle_qiangdao");
std::string baotu_end("baotu_end");
std::vector<std::string*> datu_step = {
	&to_changan_jiudian,
	&to_dianxiaoer, 
	&talk_get_baoturenwu,
	&parse_baotu_task,
	&goto_target_scene,
	&attack_qiangdao,
	&handle_qiangdao,
	&baotu_end
};

double gThreshold = 0.81;  // 默认值
int gMatchMethod = cv::TM_CCOEFF_NORMED;  // 默认值

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

class WindowInfo {
public:
	WindowInfo(HANDLE processID);
	void init();
	void hook_init();
	void datu();

	std::vector<uintptr_t> ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask);
	std::vector<uintptr_t> PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask);

	uintptr_t ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, std::vector<BYTE> pattern, const char* mask);
	uintptr_t PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern, const char* mask);
	uintptr_t getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
	uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, const char* mask, size_t offset);
	bool MatchingRectExist(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	POINT MatchingRectLoc(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
	POINT WaitMatchingRectLoc(cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
	bool WaitMatchingRectExist(cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	bool WaitMatchingRectDisapper(cv::Rect roi_rect, std::string templ_path, int timeout = 1000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	bool mouse_click_human(POINT pos, int xs = 0, int ys = 0, int mode = 1);
	POINT get_cursor_pos(POINT pos);
	bool ClickMatchImage(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int x_fix = 0, int y_fix = 0, int xs = 0, int ys = 0, int mode = 1, int timeout = 500);
	void scan_npc_pos_addr(int npc);
	void update_player_float_pos();
	void update_scene();
	void update_scene_id();
	void update_npc_pos(int npc);

	void move_cursor_center_top();
	void move_cursor_center_bottom();
	void move_cursor_right_top();

	void open_beibao();
	POINT open_map();
	void close_map();
	void close_beibao_smart(bool keep = false);
	void move_to_dianxiaoer();
	bool is_dianxiaoer_pos(float x, float y);
	bool is_changan_yizhanlaoban_pos(float x, float y);
	bool is_moving();
	bool wait_moving_stop(int timeout);
	bool is_near_dianxiaoer();
	bool is_near_changan_yizhanlaoban();
	bool wait_fighting();
	bool is_fighting();
	void handle_datu_fight();
	POINT compute_pos_pixel(POINT dst, unsigned int scene_id);
	int convert_to_map_pos_x(float x);
	int convert_to_map_pos_y(float y);
	bool talk_to_dianxiaoer();
	void parse_baotu_task_info();
	bool goto_scene(POINT dst, unsigned int scene_id);
	void move_to_position(POINT dst, long active_x = dianxiaoer_valid_distence, long active_y = dianxiaoer_valid_distence);
	void move_via_map(POINT dst);
	bool click_position(POINT dst, int xs = 0, int ys = 0, int mode = 1);
	void click_position_at_edge(POINT dst, int xs = 0, int ys = 0);
	bool attack_npc(POINT dst);
	void goto_changanjiudian();
	void from_changan_fly_to_datangguojing();
	void from_datangguojing_to_datangjingwai();
	void move_to_changanjidian_center();
	void fly_to_changanjiudian();
	void fly_to_scene(long x, long y, unsigned int scene_id);
	void UpdateWindowRect();
	void SplitTitleAsPlayerId();
	void use_beibao_prop(const char* image, bool turn = true, bool keep = false);
	void use_changan777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false);
	void use_zhuziguo777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false);
	void use_changshoucun777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false);
	void use_aolaiguo777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false);
	void use_feixingfu(unsigned int scene_id);
	void handle_sheyaoxiang_time();

	bool wait_scene_change(unsigned int scene_id, int timeout = 1700);
	void close_npc_talk();
	void close_npc_talk_fast();
	unsigned int get_scene_id_by_name(std::wstring name);
	bool low_health(cv::Rect roi, std::string templ_path, int deadline);
	bool low_health_hero(int deadline);
	bool low_health_peg(int deadline);
	bool low_mana_hero(int deadline);
	void supply_health_hero();
	void supply_health_peg();
	void supply_mana_hero();
	void handle_health();


	cv::Rect ROI_cursor(POINT pos);
	cv::Rect ROI_beibao();
	cv::Rect ROI_map();
	cv::Rect ROI_npc_talk();
	cv::Rect ROI_beibao_props();
	cv::Rect ROI_changan777_changanjiudian();
	cv::Rect ROI_changan777_yizhan_laoban();
	cv::Rect ROI_changan777_datangguojing();
	cv::Rect ROI_changan777_jiangnanyewai();
	cv::Rect ROI_changan777_huashengsi();
	cv::Rect ROI_changshoucun777_lucheng_n_qiangzhuan();
	cv::Rect ROI_changshoucun777_fangcunshan();
	cv::Rect ROI_changshoucun777_zhongshusheng();
	cv::Rect ROI_changshoucun777_dangpu();
	cv::Rect ROI_changshoucun777_taibaijinxing();
	cv::Rect ROI_changshoucun777_changshoujiaowai();
	cv::Rect ROI_aolaiguo777_nvercun();
	cv::Rect ROI_aolaiguo777_qianzhuang();
	cv::Rect ROI_aolaiguo777_penglaixiandao();
	cv::Rect ROI_aolaiguo777_yaodian();
	cv::Rect ROI_aolaiguo777_donghaiwan();
	cv::Rect ROI_aolaiguo777_dangpu();
	cv::Rect ROI_aolaiguo777_huaguoshan();
	cv::Rect ROI_zhuziguo777_datangjingwai();
	cv::Rect ROI_zhuziguo777_qilinshan();
	cv::Rect ROI_zhuziguo777_shenjidaozhang();
	cv::Rect ROI_zhuziguo777_jiudian();
	cv::Rect ROI_zhuziguo777_duanmuniangzi();
	cv::Rect ROI_zhuziguo777_yaodian();
	cv::Rect ROI_zhuziguo777_sichouzhilu();
	cv::Rect ROI_feixingfu_baoxiangguo();
	cv::Rect ROI_feixingfu_xiliangnvguo();
	cv::Rect ROI_feixingfu_jianyecheng();
	cv::Rect ROI_feixingfu_changshoucun();
	cv::Rect ROI_feixingfu_zhuziguo();
	cv::Rect ROI_feixingfu_aolaiguo();
	cv::Rect ROI_fighting();
	cv::Rect ROI_health_hero(int deadline);
	cv::Rect ROI_health_peg(int deadline);
	cv::Rect ROI_mana_hero(int deadline);
	cv::Rect ROI_four_man();
	cv::Rect ROI_fight_action();
	void test();

	float player_x = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	float player_y = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	POINT player_pos = { 0, 0 };

	uintptr_t player_pos_addr = 0;
	uintptr_t map_info_addr = 0;
	SIZE_T map_offset = 0x14;

	//这个结构包含所有NPC和玩家（包括自己）的坐标
	uintptr_t location_first_static_addr = 0;
	uintptr_t location_second_static_addr = 0;
	uintptr_t location_dynamic_addr_third_child_first_static_addr = 0;
	uintptr_t dianxiaoer_pos_addr = 0;
	uintptr_t changan_yizhanlaoban_pos_addr = 0;
	std::vector<POINT> dianxiaoer_pos_list;  // 店小二固定移动的几个坐标
	std::vector<POINT> changan_yizhanlaoban_pos_list;  // 长安驿站老板固定移动的几个坐标
	float dianxiaoer_pos_x = 0;
	float dianxiaoer_pos_y = 0;
	float changan_yizhanlaoban_pos_x = 0;
	float changan_yizhanlaoban_pos_y = 0;
	unsigned int baotu_target_scene_id = 0;
	POINT baotu_target_pos = { 0, 0 };
	//POINT baotu_astar_pos = { 0, 0 };  // 宝图A星寻路目的坐标
	unsigned int baotu_task_count = 0;  // 今日领取第几次任务

	uintptr_t scene_id_addr = 0;
	std::string scene;
	unsigned int m_scene_id = 0;
	std::string player_name;  // 梦幻西游 ONLINE - (四川1区[嘉陵江] - Ⅻ闵Ξ青[16705567])
	std::string player_id;
	bool moving = false;
	bool fail = false;
	bool mp3_playing = false;
	//bool four_man = false;
	int f_round = 0;
	uint64_t wait_hero_action_time = 0;

	HANDLE pid;
	HWND hwnd;
	RECT rect;
	long wWidth = 1024;  // 游戏窗口大小
	long wHeight = 768;  // 游戏窗口大小
	HANDLE hProcess = 0;
	HMODULE hNtdll = 0;
	HMODULE mhmainDllBase = 0;
	PFN_NtReadVirtualMemory pNtReadVirtualMemory;

	Step step = Step(datu_step);
};
class TimeProcessor {
public:
	TimeProcessor();
	bool timeout(uint64_t time);
	void update();
	bool time_wait(uint64_t time);

	uint64_t mTime_ms;
};
class GoodMorning {
public:
	GoodMorning();

	void init();
	void hook_data();
	void work();
	void test();
	void time_pawn_update();
	void update_db();

	const char* dbFile = "database";
	json db;
	bool waiting = false;
	std::vector<WindowInfo> winsInfo;
	HANDLE hSerial;
	TimeProcessor time_knight = TimeProcessor();
	TimeProcessor time_pawn = TimeProcessor();
	TimeProcessor task_pawn = TimeProcessor();
private:

};

std::vector<DWORD> FindPidsByName(const wchar_t* name);
HMODULE getProcessModulesAddress(HANDLE hProcess, const TCHAR* moduleName);
DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);
cv::Mat hwnd2mat(HWND hwnd);
void save_screenshot(cv::Mat& image);
bool MatchingExist(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, cv::Mat templ, std::string mask_path, double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path, double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
POINT MatchingRectLoc(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);

uint64_t getCurrentTimeMilliseconds();
std::wstring bytes_to_wstring(const unsigned char* buffer, size_t size);
std::vector<std::wstring> findContentBetweenTags(const std::wstring& source, const std::wstring& startTag, const std::wstring& endTag);
std::string AnsiToUtf8(const std::string& ansiStr);
POINT get_map_max_pixel(unsigned int scene_id);
cv::Mat CannyThreshold(cv::Mat src);
cv::Mat ThresholdinginRange(cv::Mat frame);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void init_log();
int randint(int min, int max);
cv::Rect ROI_NULL();


int Serial();
void SerialWrite(const char* data);
void SerialRead();
void serial_click_cur();
void serial_right_click_cur();
void serial_move_human(POINT pos, int mode=1);


void input_alt_xxx(const char* data);
void input_alt_a();
void input_alt_e();
void input_key_xxx(const char* data);
void input_tab();
void input_f1();
void stop_laba();
void play_mp3();



