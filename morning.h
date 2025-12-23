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

#define dianxiaoer_valid_distence 4  // 与店小二对话时的最大有效距离
#define changan_yizhan_laoban_valid_distence 10  // 与长安驿站老板对话时的最大有效距离
#define NPC_TALK_VALID_DISTENCE 6  // 与NPC对话时的最大有效距离
#define MATCHCENTER 1
#define MATCHLEFTTOP 2
#define MATCHEXIST 3

#define THREAD_IDLE 0  // 线程空闲
#define 店小二 536871319 // 店小二
#define 长安驿站老板 536870914  // 长安驿站老板
#define 贼王 538155549  // 贼王
#define TASK_BAOTU 4  // 宝图任务
#define TASK_ZEIWANG 5  // 贼王任务

const char* PLAY_MP3 = "mmp3:PLAY_%d\n";
const char* STOP_MP3 = "mmp3:STOP\n";

//# 拟人
const char* MS_MOVE_SYMBOL = "movedf:%d,%d,%d\n";  // x,y,mode
//const char* MS_MOVE_HUMAN_SYMBOL = "movehm:%d,%d,%d,%d,%d\n";  // cx, cy, x, y, mode
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
const char* img_btn_woshilaishoushinide = "object\\btn\\woshilaishoushinide.png";
const char* img_btn_zeiwang_benshaoxiashilaititianxingdaode = "object\\btn\\zeiwang_benshaoxiashilaititianxingdaode.png";

const char* img_props_red_777 = "object\\props\\red_777.png";
const char* img_props_white_777 = "object\\props\\white_777.png";
const char* img_props_green_777 = "object\\props\\green_777.png";
const char* img_props_yellow_777 = "object\\props\\yellow_777.png";
const char* img_props_sheyaoxiang = "object\\props\\sheyaoxiang.png";
const char* img_npc_dianxiaoer = "object\\npc\\dianxiaoer.png";

const char* img_fight_fighting = "object\\fight\\fighting.png";
const char* img_fight_health_100 = "object\\fight\\health_100.png";
const char* img_fight_health_95 = "object\\fight\\health_95.png";
const char* img_fight_health_90 = "object\\fight\\health_90.png";
const char* img_fight_health_85 = "object\\fight\\health_85.png";
const char* img_fight_health_80 = "object\\fight\\health_80.png";
const char* img_fight_health_75 = "object\\fight\\health_75.png";
const char* img_fight_health_70 = "object\\fight\\health_70.png";
const char* img_fight_health_65 = "object\\fight\\health_65.png";
const char* img_fight_health_60 = "object\\fight\\health_60.png";
const char* img_fight_health_55 = "object\\fight\\health_55.png";
const char* img_fight_health_50 = "object\\fight\\health_50.png";
const char* img_fight_mana_100 = "object\\fight\\mana_100.png";
const char* img_fight_mana_95 = "object\\fight\\mana_95.png";
const char* img_fight_mana_90 = "object\\fight\\mana_90.png";
const char* img_fight_mana_85 = "object\\fight\\mana_85.png";
const char* img_fight_mana_80 = "object\\fight\\mana_80.png";
const char* img_fight_mana_75 = "object\\fight\\mana_75.png";
const char* img_fight_mana_70 = "object\\fight\\mana_70.png";
const char* img_fight_mana_65 = "object\\fight\\mana_65.png";
const char* img_fight_mana_60 = "object\\fight\\mana_60.png";
const char* img_fight_mana_55 = "object\\fight\\mana_55.png";
const char* img_fight_mana_50 = "object\\fight\\mana_50.png";
const char* img_fight_fourman_title_gray = "object\\fight\\fourman_title_gray.png";
const char* img_fight_do_hero_action = "object\\fight\\do_hero_action.png";
const char* img_fight_do_peg_action = "object\\fight\\do_peg_action.png";
const char* img_fight_auto = "object\\fight\\auto.png";
const char* img_fight_auto_round30 = "object\\fight\\auto_round30.png";

const char* img_symbol_map = "object\\symbol\\map.png";
const char* img_symbol_feixingfu_xiliangnvguo = "object\\symbol\\feixingfu_xiliangnvguo.png";
const char* img_symbol_feixingfu_baoxiangguo = "object\\symbol\\feixingfu_baoxiangguo.png";
const char* img_symbol_feixingfu_jianyecheng = "object\\symbol\\feixingfu_jianyecheng.png";
const char* img_symbol_feixingfu_changshoucun = "object\\symbol\\feixingfu_changshoucun.png";
const char* img_symbol_feixingfu_aolaiguo = "object\\symbol\\feixingfu_aolaiguo.png";
const char* img_symbol_feixingfu_zhuziguo = "object\\symbol\\feixingfu_zhuziguo.png";
const char* img_symbol_ciyushunxu_gray = "object\\symbol\\ciyushunxu_gray.png";
const char* img_symbol_yidongdezi_gray = "object\\symbol\\yidongdezi_gray.png";
const char* img_symbol_gaosunitadecangshenweizhi = "object\\symbol\\gaosunitadecangshenweizhi.png";
const char* img_symbol_wabao_title_gray = "object\\symbol\\wabao_title_gray.png";
const char* img_symbol_task_track_gray = "object\\symbol\\task_track_gray.png";
const char* img_symbol_zeiwang = "object\\symbol\\zeiwang.png";

const char* img_cursors_cursor = "object\\cursors\\cursor.png";
const char* img_cursors_cursor_mask = "object\\cursors\\cursor_mask.png";

std::string work_start("work_start");
std::string to_changan_jiudian("to_changan_jiudian");
std::string scan_dianxiaoer_pos("scan_dianxiaoer");
std::string to_dianxiaoer_get_task("to_dianxiaoer_get_task");
//std::string talk_get_baoturenwu("talk_get_baoturenwu");
//std::string scan_baotu_task("scan_baotu_task");
std::string goto_baotu_scene("goto_baotu_scene");
std::string attack_qiangdao("attack_qiangdao");
std::string scan_zeiwang_task("scan_zeiwang_task");
std::string goto_zeiwang_scene("goto_zeiwang_scene");
std::string fix_my_pos_zeiwang("fix_my_pos_zeiwang");
std::string scan_zeiwang_pos("scan_zeiwang_pos");
std::string try_zeiwang_pos("try_zeiwang_pos");
std::string attack_zeiwang("attack_zeiwang");
std::string baotu_end("baotu_end");
std::vector<std::string*> datu_step = {
	&work_start,
	&to_changan_jiudian,
	& scan_dianxiaoer_pos,
	&to_dianxiaoer_get_task, 
	//&talk_get_baoturenwu,
	//&scan_baotu_task,
	&goto_baotu_scene,
	&attack_qiangdao,
	&scan_zeiwang_task,
	& goto_zeiwang_scene,
	& fix_my_pos_zeiwang,
	& scan_zeiwang_pos,
	& try_zeiwang_pos,
	& attack_zeiwang,
	&baotu_end
};
//std::vector<std::thread>
double gThreshold = 0.81;  // 默认值
int gMatchMethod = cv::TM_CCOEFF_NORMED;  // 默认值

// 场景NPC固定坐标
std::vector<POINT> changan_zahuodian_npc_list = { {370,250} };  //长安杂货店
std::vector<POINT> changan_shipindian_npc_list = { {530,350} };   //长安饰品店
std::vector<POINT> changan_guozijian_npc_list = {  };// 长安国子监 todo
std::vector<POINT> jianyecheng_npc_list = {  };//建邺城 todo
//std::vector<POINT> jianyeyamen_npc_list = {  };//建邺衙门 todo
std::vector<POINT> jianyezahuodian_npc_list = { {690,510} };//建邺杂货店 todo
std::vector<POINT> aolaikezhanerlou_npc_list = {  };//傲来客栈二楼 todo
std::vector<POINT> aolaiguo_yaodian_npc_list = {  };//傲来药店 todo
std::vector<POINT> changshoucun_dangpu_npc_list = { {410,250} };//长寿村当铺 todo
//std::vector<POINT> changshoujiaowai_npc_list = {  };//长寿郊外 todo


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
	int index=0;
	bool end = false;
};
class TimeProcessor {
public:
	TimeProcessor();
	bool timeout(uint64_t time);
	void update();
	bool time_wait(uint64_t time);

	uint64_t mTime_ms;
};
class WindowInfo {
public:
	WindowInfo(HANDLE processID);
	void init();
	void hook_init();
	void datu();
	void test();

	std::vector<uintptr_t> ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, const BYTE* pattern, size_t pattern_size, const char* mask);
	std::vector<uintptr_t> PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern);

	uintptr_t ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, const BYTE* pattern, size_t pattern_size, const char* mask);
	uintptr_t PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern);
	uintptr_t getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, size_t offset);
	uintptr_t getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, size_t offset);
	bool MatchingRectExist(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	bool MatchingGrayRectExist(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = cv::TM_CCORR_NORMED);
	POINT MatchingRectLoc(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
	POINT WaitMatchingRectLoc(cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
	bool WaitMatchingRectExist(cv::Rect roi_rect, std::string templ_path, int timeout = 2000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	bool WaitMatchingRectDisapper(cv::Rect roi_rect, std::string templ_path, int timeout = 1000, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
	bool mouse_click_human(POINT pos, int xs = 0, int ys = 0, int mode = 1);
	POINT get_cursor_pos(POINT pos);
	bool ClickMatchImage(cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int x_fix = 0, int y_fix = 0, int xs = 0, int ys = 0, int mode = 1, int timeout = 500);
	void scan_npc_pos_in_thread();
	//void scan_npc_pos_addr(int npc);
	void scan_npc_pos_addr_by_id(unsigned int npc);
	void update_player_float_pos();
	void update_scene();
	void update_scene_id();
	void update_npc_pos(int npc);
	void move_cursor_center_top();
	void move_cursor_center_bottom();
	void move_cursor_right_top();
	void move_cursor_left_bottom();

	void open_beibao();
	POINT open_map();
	void close_map();
	void close_beibao_smart(bool keep = false);
	void move_to_dianxiaoer();
	bool is_given_pos(float x, float y, const std::vector<POINT>& pos_list);
	//bool is_dianxiaoer_pos(float x, float y);
	//bool is_changan_yizhanlaoban_pos(float x, float y);
	bool is_moving();
	bool wait_moving_stop(int timeout);
	bool is_near_dianxiaoer();
	bool is_near_changan_yizhanlaoban();
	bool is_near_loc(POINT dst, int near_x, int near_y);
	bool is_npc_visible(uintptr_t npc_loc_addr);
	bool wait_fighting();
	bool is_fighting();
	bool is_verifying();
	bool is_hangup(cv::Mat image);
	void handle_datu_fight();
	POINT compute_dianxiaoer_pos_lazy();
	POINT compute_pos_pixel(POINT dst, unsigned int scene_id, bool fix = false);
	int convert_to_map_pos_x(float x);
	int convert_to_map_pos_y(float y);
	bool talk_to_dianxiaoer();
	void parse_baotu_task_info();
	void parse_zeiwang_info();
	bool goto_scene(POINT dst, unsigned int scene_id);
	void move_to_position(POINT dst, long active_x = 0, long active_y = 0);
	void move_via_map(POINT dst);
	void move_to_other_scene(POINT door, unsigned int scene_id, int xs = 0, int ys = 0, bool close_beibao=false);
	void ship_to_other_scene(POINT door, unsigned int scene_id, int xs = 0, int ys = 0, bool close_beibao = false);
	bool click_position(POINT dst, int xs = 0, int ys = 0, int x_fix = 0, int y_fix = 0, int mode = 1);
	void click_position_at_edge(POINT dst, int xs = 0, int ys = 0, int x_fix = 0, int y_fix = 0, int mode = 1);
	bool talk_to_npc_fight(POINT dst, const char* templ);
	void goto_changanjiudian();
	void from_changan_fly_to_datangguojing();
	void from_datangguojing_to_datangjingwai();
	void from_changan_to_datangguojing();
	void move_to_changanjidian_center();
	void fly_to_changanjiudian();
	void fly_to_changan_yizhan_laoban();
	void fly_to_scene(long x, long y, unsigned int scene_id);
	void UpdateWindowRect();
	void SplitTitleAsPlayerId();
	void use_beibao_prop(const char* image, bool turn = true, bool keep = false);
	void use_changan777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false, bool wait_scene = true);
	void use_zhuziguo777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false, bool wait_scene = true);
	void use_changshoucun777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false, bool wait_scene = true);
	void use_aolaiguo777(cv::Rect roi, bool move = true, bool turn = true, bool keep = false, bool wait_scene = true);
	void use_feixingfu(unsigned int scene_id, bool wait_scene = true);
	void handle_sheyaoxiang_time();

	bool wait_scene_change(unsigned int scene_id, int timeout = 1700);
	void close_npc_talk();
	bool close_npc_talk_fast();
	bool low_health(cv::Rect roi, int deadline);
	bool low_mana(cv::Rect roi, int deadline);
	void supply_health_hero();
	void supply_health_peg();
	void supply_mana_hero();
	void handle_health();
	void time_pawn_update();
	bool out_of_rect(POINT pixel);

	cv::Rect ROI_cursor(POINT pos);
	cv::Rect ROI_beibao();
	cv::Rect ROI_map();
	cv::Rect ROI_npc_talk();
	cv::Rect ROI_beibao_props();
	cv::Rect ROI_task();
	cv::Rect ROI_changan777_changanjiudian();
	cv::Rect ROI_changan777_yizhan_laoban();
	cv::Rect ROI_changan777_datangguojing();
	cv::Rect ROI_changan777_jiangnanyewai();
	cv::Rect ROI_changan777_huashengsi();
	cv::Rect ROI_changan777_dangpu();
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
	cv::Rect ROI_health_hero();
	cv::Rect ROI_health_peg();
	cv::Rect ROI_mana_hero();
	cv::Rect ROI_four_man();
	cv::Rect ROI_ciyushunxu();
	cv::Rect ROI_yidongdezi();
	cv::Rect ROI_fight_action();

	float player_x = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	float player_y = 0;  // 这里的玩家坐标是float值，是内部地图坐标
	POINT player_pos = { 0, 0 };

	uintptr_t player_pos_addr = 0;
	uintptr_t map_info_addr = 0;
	SIZE_T map_offset = 0x14;

	uintptr_t npc_first_static_addr = 0;
	uintptr_t npc_loc_first_static_addr = 0;
	bool npc_found = false;
	//这个结构包含所有NPC和玩家（包括自己）的坐标
	//uintptr_t location_first_static_addr = 0;
	//uintptr_t location_second_static_addr = 0;
	//uintptr_t location_dynamic_addr_third_child_first_static_addr = 0;
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
	unsigned int baotu_task_count = 0;  // 今日领取第几次任务
	unsigned int zeiwang_scene_id = 0;
	std::vector<POINT> zeiwang_pos_list;
	POINT zeiwang_pos = {0,0};

	uintptr_t scene_id_addr = 0;
	std::string scene;
	unsigned int m_scene_id = 0;
	std::string player_name;  // 梦幻西游 ONLINE - (四川1区[嘉陵江] - Ⅻ闵Ξ青[16705567])
	std::string player_id;
	bool mp3_playing = false;
	bool moving = false;
	bool failure = false;
	bool popup_verify = false;
	int f_round = 0;
	uint64_t wait_hero_action_time = 0;
	int tScan_npc = THREAD_IDLE;  //线程后台扫描内存
	// Atomic variable shared across threads
	//std::atomic<int> tScan_npc{ NPC_IDLE }; //线程后台扫描内存

	HANDLE pid;
	HWND hwnd;
	RECT rect;
	long wWidth = 1024;  // 游戏窗口大小
	long wHeight = 768;  // 游戏窗口大小
	int mScreen_x = 21;  // wWidth / 2 / 20  屏幕可以范围坐标跨度
	int mScreen_y = 15;  // wHeight / 2 / 20 屏幕可以范围坐标跨度
	HANDLE hProcess = 0;
	HMODULE hNtdll = 0;
	HMODULE mhmainDllBase = 0;
	PFN_NtReadVirtualMemory pNtReadVirtualMemory;

	Step step = Step(datu_step);
	TimeProcessor time_pawn = TimeProcessor();
	//std::thread thread1 = std::thread(&WindowInfo::test, this);;
};
class GoodMorning {
public:
	GoodMorning();

	void init();
	void hook_data();
	void work();
	void test();
	//void time_pawn_update();
	void update_db();

	const char* dbFile = "database";
	json db;
	bool waiting = false;
	std::vector<std::unique_ptr<WindowInfo>> winsInfo;
	HANDLE hSerial;
	//TimeProcessor time_knight = TimeProcessor();
	//TimeProcessor time_pawn = TimeProcessor();
	//TimeProcessor task_pawn = TimeProcessor();
private:

};

void parseAobString(const std::string& aobStr, std::vector<unsigned char>& pattern, std::vector<char>& mask);
std::vector<DWORD> FindPidsByName(const wchar_t* name);
HMODULE getProcessModulesAddress(HANDLE hProcess, const TCHAR* moduleName);
DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);
cv::Mat hwnd2mat(HWND hwnd);
void save_screenshot(cv::Mat& image);
bool MatchingExist(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod);
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, cv::Mat templ, std::string mask_path="", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
POINT MatchingLoc(cv::Mat image, cv::Rect roi_rect, std::string templ_path, std::string mask_path="", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);
POINT MatchingRectLoc(cv::Rect roi_rect, std::string image_path, std::string templ_path, std::string mask_path = "", double threshold = gThreshold, int match_method = gMatchMethod, int loc = MATCHCENTER);

uint64_t getCurrentTimeMilliseconds();
std::wstring bytes_to_wstring(const unsigned char* buffer, size_t size);
std::vector<std::wstring> findContentBetweenTags(const std::wstring& source, const std::wstring& startTag, const std::wstring& endTag);
std::string AnsiToUtf8(const std::string& ansiStr);
POINT get_map_max_pixel(unsigned int scene_id);
std::vector<POINT> get_scene_npc_list(unsigned int scene_id);
cv::Mat CannyThreshold(cv::Mat src);
cv::Mat ThresholdinginRange(cv::Mat frame);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void init_log();
int randint(int min, int max);
cv::Rect ROI_NULL();
void test222(WindowInfo& winfo);

int Serial();
void SerialWrite(const char* data);
void SerialRead();
void serial_click_cur();
void serial_right_click_cur();
void serial_move(POINT pos, int mode=1);
//void serial_move_human(POINT pos, int mode = 1);

void input_alt_xxx(const char* data);
void input_alt_a();
void input_alt_e();
void input_key_xxx(const char* data);
void input_tab();
void input_f1();
void hide_player();
void stop_laba();
void play_mp3();



