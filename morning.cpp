#include "morning.h"
#include "log.h"


#include <fstream> 
#include <filesystem>
#include <cstdlib> // For rand() and srand()
#include <chrono>

#include <future>

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

//畅玩服
const cv::Mat img_btn_beibao = cv_imread("object\\btn\\beibao.png");
const cv::Mat img_btn_package_prop = cv_imread("object\\btn\\package_prop.png");
const cv::Mat img_btn_tingtingwufang = cv_imread("object\\btn\\tingtingwufang.png");
const cv::Mat img_btn_npc_talk_close = cv_imread("object\\btn\\npc_talk_close.png");
const cv::Mat img_btn_npc_talk_close2 = cv_imread("object\\btn\\npc_talk_close2.png");
const cv::Mat img_btn_flag_loc = cv_imread("object\\btn\\flag_loc.png");
const cv::Mat img_btn_shide_woyaoqu = cv_imread("object\\btn\\shide_woyaoqu.png");
const cv::Mat img_btn_cancel_auto_round = cv_imread("object\\btn\\cancel_auto_round.png");
const cv::Mat img_btn_cancel_zhanli = cv_imread("object\\btn\\cancel_zhanli.png");
const cv::Mat img_btn_reset_auto_round = cv_imread("object\\btn\\reset_auto_round.png");
const cv::Mat img_btn_woshilaishoushinide = cv_imread("object\\btn\\woshilaishoushinide.png");
const cv::Mat img_btn_zeiwang_benshaoxiashilaititianxingdaode = cv_imread("object\\btn\\zeiwang_benshaoxiashilaititianxingdaode.png");
//const cv::Mat img_btn_laozimeishijianwan = cv_imread("object\\btn\\laozimeishijianwan.png");
//const cv::Mat img_btn_wozhengzaizhandouzhong = cv_imread("object\\btn\\wozhengzaizhandouzhong.png");
//const cv::Mat img_btn_menpaichuanggaunhudong = cv_imread("object\\btn\\menpaichuanggaunhudong.png");
//const cv::Mat img_btn_luguohenishuogehua = cv_imread("object\\btn\\luguohenishuogehua.png");
//const cv::Mat img_btn_wohaixiangzaizhezhuanzhuan = cv_imread("object\\btn\\wohaixiangzaizhezhuanzhuan.png");
//const cv::Mat img_btn_wojintianmeidaiqian_gaitianlaizhaoni = cv_imread("object\\btn\\wojintianmeidaiqian_gaitianlaizhaoni.png");
//const cv::Mat img_btn_wosuibiankankan = cv_imread("object\\btn\\wosuibiankankan.png");
//const cv::Mat img_btn_womenhouhuiyouqi = cv_imread("object\\btn\\womenhouhuiyouqi.png");
//const cv::Mat img_btn_luanchiyao_hairenming = cv_imread("object\\btn\\luanchiyao_hairenming.png");
//const cv::Mat img_btn_woshenmedoubuzuo = cv_imread("object\\btn\\woshenmedoubuzuo.png");


const cv::Mat img_props_red_777 = cv_imread("object\\props\\red_777.png");
const cv::Mat img_props_white_777 = cv_imread("object\\props\\white_777.png");
const cv::Mat img_props_green_777 = cv_imread("object\\props\\green_777.png");
const cv::Mat img_props_yellow_777 = cv_imread("object\\props\\yellow_777.png");
const cv::Mat img_props_sheyaoxiang = cv_imread("object\\props\\sheyaoxiang.png");
const cv::Mat img_npc_dianxiaoer = cv_imread("object\\npc\\dianxiaoer.png");


const cv::Mat img_fight_health_100 = cv_imread("object\\fight\\health_100.png");
const cv::Mat img_fight_health_95 = cv_imread("object\\fight\\health_95.png");
const cv::Mat img_fight_health_90 = cv_imread("object\\fight\\health_90.png");
const cv::Mat img_fight_health_85 = cv_imread("object\\fight\\health_85.png");
const cv::Mat img_fight_health_80 = cv_imread("object\\fight\\health_80.png");
const cv::Mat img_fight_health_75 = cv_imread("object\\fight\\health_75.png");
const cv::Mat img_fight_health_70 = cv_imread("object\\fight\\health_70.png");
const cv::Mat img_fight_health_65 = cv_imread("object\\fight\\health_65.png");
const cv::Mat img_fight_health_60 = cv_imread("object\\fight\\health_60.png");
const cv::Mat img_fight_health_55 = cv_imread("object\\fight\\health_55.png");
const cv::Mat img_fight_health_50 = cv_imread("object\\fight\\health_50.png");
const cv::Mat img_fight_mana_100 = cv_imread("object\\fight\\mana_100.png");
const cv::Mat img_fight_mana_95 = cv_imread("object\\fight\\mana_95.png");
const cv::Mat img_fight_mana_90 = cv_imread("object\\fight\\mana_90.png");
const cv::Mat img_fight_mana_85 = cv_imread("object\\fight\\mana_85.png");
const cv::Mat img_fight_mana_80 = cv_imread("object\\fight\\mana_80.png");
const cv::Mat img_fight_mana_75 = cv_imread("object\\fight\\mana_75.png");
const cv::Mat img_fight_mana_70 = cv_imread("object\\fight\\mana_70.png");
const cv::Mat img_fight_mana_65 = cv_imread("object\\fight\\mana_65.png");
const cv::Mat img_fight_mana_60 = cv_imread("object\\fight\\mana_60.png");
const cv::Mat img_fight_mana_55 = cv_imread("object\\fight\\mana_55.png");
const cv::Mat img_fight_mana_50 = cv_imread("object\\fight\\mana_50.png");
const cv::Mat img_fight_fourman_title_gray = cv_imread("object\\fight\\fourman_title_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_fight_fourman_title2_gray = cv_imread("object\\fight\\fourman_title2_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_fight_do_hero_action = cv_imread("object\\fight\\do_hero_action.png");
const cv::Mat img_fight_do_peg_action = cv_imread("object\\fight\\do_peg_action.png");
const cv::Mat img_fight_auto = cv_imread("object\\fight\\auto.png");
const cv::Mat img_fight_auto_round30 = cv_imread("object\\fight\\auto_round30.png");

const cv::Mat img_symbol_map = cv_imread("object\\symbol\\map.png");
const cv::Mat img_symbol_feixingfu_xiliangnvguo = cv_imread("object\\symbol\\feixingfu_xiliangnvguo.png");
const cv::Mat img_symbol_feixingfu_baoxiangguo = cv_imread("object\\symbol\\feixingfu_baoxiangguo.png");
const cv::Mat img_symbol_feixingfu_jianyecheng = cv_imread("object\\symbol\\feixingfu_jianyecheng.png");
const cv::Mat img_symbol_feixingfu_changshoucun = cv_imread("object\\symbol\\feixingfu_changshoucun.png");
const cv::Mat img_symbol_feixingfu_aolaiguo = cv_imread("object\\symbol\\feixingfu_aolaiguo.png");
const cv::Mat img_symbol_feixingfu_zhuziguo = cv_imread("object\\symbol\\feixingfu_zhuziguo.png");
//const cv::Mat img_symbol_ciyushunxu_gray = cv_imread("object\\symbol\\ciyushunxu_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_yidongdezi_gray = cv_imread("object\\symbol\\yidongdezi_gray.png", cv::IMREAD_GRAYSCALE);
//const cv::Mat img_symbol_gaosunitadecangshenweizhi = cv_imread("object\\symbol\\gaosunitadecangshenweizhi.png");
const cv::Mat img_symbol_wozhidaowomenlaodadexingzong = cv_imread("object\\symbol\\wozhidaowomenlaodadexingzong.png");
const cv::Mat img_symbol_wabao_title_gray = cv_imread("object\\symbol\\wabao_title_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_task_track_gray = cv_imread("object\\symbol\\task_track_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_zeiwang = cv_imread("object\\symbol\\zeiwang.png");
const cv::Mat img_symbol_paixu_verify_reset = cv_imread("object\\symbol\\paixu_verify_reset.png");
const cv::Mat img_symbol_jibaile50geqiangdao = cv_imread("object\\symbol\\jibaile50geqiangdao.png");

const cv::Mat img_cursors_cursor = cv_imread("object\\cursors\\cursor.png");

//时间服
const cv::Mat img_btn_beibao_card = cv_imread("object_card\\btn\\beibao.png");
const cv::Mat img_btn_package_prop_card = cv_imread("object_card\\btn\\package_prop.png");
const cv::Mat img_btn_tingtingwufang_card = cv_imread("object_card\\btn\\tingtingwufang.png");
const cv::Mat img_btn_npc_talk_close_card = cv_imread("object_card\\btn\\npc_talk_close.png");
const cv::Mat img_btn_npc_talk_close2_card = cv_imread("object_card\\btn\\npc_talk_close2.png");
const cv::Mat img_btn_flag_loc_card = cv_imread("object_card\\btn\\flag_loc.png");
const cv::Mat img_btn_shide_woyaoqu_card = cv_imread("object_card\\btn\\shide_woyaoqu.png");
const cv::Mat img_btn_cancel_auto_round_card = cv_imread("object_card\\btn\\cancel_auto_round.png");
const cv::Mat img_btn_cancel_zhanli_card = cv_imread("object_card\\btn\\cancel_zhanli.png");
const cv::Mat img_btn_reset_auto_round_card = cv_imread("object_card\\btn\\reset_auto_round.png");
const cv::Mat img_btn_woshilaishoushinide_card = cv_imread("object_card\\btn\\woshilaishoushinide.png");
const cv::Mat img_btn_zeiwang_benshaoxiashilaititianxingdaode_card = cv_imread("object_card\\btn\\zeiwang_benshaoxiashilaititianxingdaode.png");
//const cv::Mat img_btn_laozimeishijianwan_card = cv_imread("object_card\\btn\\laozimeishijianwan.png");
//const cv::Mat img_btn_wozhengzaizhandouzhong_card = cv_imread("object_card\\btn\\wozhengzaizhandouzhong.png");
//const cv::Mat img_btn_menpaichuanggaunhudong_card = cv_imread("object_card\\btn\\menpaichuanggaunhudong.png");
//const cv::Mat img_btn_luguohenishuogehua_card = cv_imread("object_card\\btn\\luguohenishuogehua.png");
//const cv::Mat img_btn_wohaixiangzaizhezhuanzhuan_card = cv_imread("object_card\\btn\\wohaixiangzaizhezhuanzhuan.png");
//const cv::Mat img_btn_wojintianmeidaiqian_gaitianlaizhaoni_card = cv_imread("object_card\\btn\\wojintianmeidaiqian_gaitianlaizhaoni.png");
//const cv::Mat img_btn_wosuibiankankan_card = cv_imread("object_card\\btn\\wosuibiankankan.png");
//const cv::Mat img_btn_womenhouhuiyouqi_card = cv_imread("object_card\\btn\\womenhouhuiyouqi.png");
//const cv::Mat img_btn_luanchiyao_hairenming_card = cv_imread("object_card\\btn\\luanchiyao_hairenming.png");
//const cv::Mat img_btn_woshenmedoubuzuo_card = cv_imread("object_card\\btn\\woshenmedoubuzuo.png");


const cv::Mat img_props_red_777_card = cv_imread("object_card\\props\\red_777.png");
const cv::Mat img_props_white_777_card = cv_imread("object_card\\props\\white_777.png");
const cv::Mat img_props_green_777_card = cv_imread("object_card\\props\\green_777.png");
const cv::Mat img_props_yellow_777_card = cv_imread("object_card\\props\\yellow_777.png");
const cv::Mat img_props_sheyaoxiang_card = cv_imread("object_card\\props\\sheyaoxiang.png");
const cv::Mat img_npc_dianxiaoer_card = cv_imread("object_card\\npc\\dianxiaoer.png");


const cv::Mat img_fight_health_100_card = cv_imread("object_card\\fight\\health_100.png");
const cv::Mat img_fight_health_95_card = cv_imread("object_card\\fight\\health_95.png");
const cv::Mat img_fight_health_90_card = cv_imread("object_card\\fight\\health_90.png");
const cv::Mat img_fight_health_85_card = cv_imread("object_card\\fight\\health_85.png");
const cv::Mat img_fight_health_80_card = cv_imread("object_card\\fight\\health_80.png");
const cv::Mat img_fight_health_75_card = cv_imread("object_card\\fight\\health_75.png");
const cv::Mat img_fight_health_70_card = cv_imread("object_card\\fight\\health_70.png");
const cv::Mat img_fight_health_65_card = cv_imread("object_card\\fight\\health_65.png");
const cv::Mat img_fight_health_60_card = cv_imread("object_card\\fight\\health_60.png");
const cv::Mat img_fight_health_55_card = cv_imread("object_card\\fight\\health_55.png");
const cv::Mat img_fight_health_50_card = cv_imread("object_card\\fight\\health_50.png");
const cv::Mat img_fight_mana_100_card = cv_imread("object_card\\fight\\mana_100.png");
const cv::Mat img_fight_mana_95_card = cv_imread("object_card\\fight\\mana_95.png");
const cv::Mat img_fight_mana_90_card = cv_imread("object_card\\fight\\mana_90.png");
const cv::Mat img_fight_mana_85_card = cv_imread("object_card\\fight\\mana_85.png");
const cv::Mat img_fight_mana_80_card = cv_imread("object_card\\fight\\mana_80.png");
const cv::Mat img_fight_mana_75_card = cv_imread("object_card\\fight\\mana_75.png");
const cv::Mat img_fight_mana_70_card = cv_imread("object_card\\fight\\mana_70.png");
const cv::Mat img_fight_mana_65_card = cv_imread("object_card\\fight\\mana_65.png");
const cv::Mat img_fight_mana_60_card = cv_imread("object_card\\fight\\mana_60.png");
const cv::Mat img_fight_mana_55_card = cv_imread("object_card\\fight\\mana_55.png");
const cv::Mat img_fight_mana_50_card = cv_imread("object_card\\fight\\mana_50.png");
const cv::Mat img_fight_fourman_title_gray_card = cv_imread("object_card\\fight\\fourman_title_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_fight_fourman_title2_gray_card = cv_imread("object_card\\fight\\fourman_title2_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_fight_do_hero_action_card = cv_imread("object_card\\fight\\do_hero_action.png");
const cv::Mat img_fight_do_peg_action_card = cv_imread("object_card\\fight\\do_peg_action.png");
const cv::Mat img_fight_auto_card = cv_imread("object_card\\fight\\auto.png");
const cv::Mat img_fight_auto_round30_card = cv_imread("object_card\\fight\\auto_round30.png");

const cv::Mat img_symbol_map_card = cv_imread("object_card\\symbol\\map.png");
const cv::Mat img_symbol_feixingfu_xiliangnvguo_card = cv_imread("object_card\\symbol\\feixingfu_xiliangnvguo.png");
const cv::Mat img_symbol_feixingfu_baoxiangguo_card = cv_imread("object_card\\symbol\\feixingfu_baoxiangguo.png");
const cv::Mat img_symbol_feixingfu_jianyecheng_card = cv_imread("object_card\\symbol\\feixingfu_jianyecheng.png");
const cv::Mat img_symbol_feixingfu_changshoucun_card = cv_imread("object_card\\symbol\\feixingfu_changshoucun.png");
const cv::Mat img_symbol_feixingfu_aolaiguo_card = cv_imread("object_card\\symbol\\feixingfu_aolaiguo.png");
const cv::Mat img_symbol_feixingfu_zhuziguo_card = cv_imread("object_card\\symbol\\feixingfu_zhuziguo.png");
//const cv::Mat img_symbol_ciyushunxu_gray = cv_imread("object_card\\symbol\\ciyushunxu_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_yidongdezi_gray_card = cv_imread("object_card\\symbol\\yidongdezi_gray.png", cv::IMREAD_GRAYSCALE);
//const cv::Mat img_symbol_gaosunitadecangshenweizhi_card = cv_imread("object_card\\symbol\\gaosunitadecangshenweizhi.png");
const cv::Mat img_symbol_wozhidaowomenlaodadexingzong_card = cv_imread("object_card\\symbol\\wozhidaowomenlaodadexingzong.png");
const cv::Mat img_symbol_wabao_title_gray_card = cv_imread("object_card\\symbol\\wabao_title_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_task_track_gray_card = cv_imread("object_card\\symbol\\task_track_gray.png", cv::IMREAD_GRAYSCALE);
const cv::Mat img_symbol_zeiwang_card = cv_imread("object_card\\symbol\\zeiwang.png");
const cv::Mat img_symbol_paixu_verify_reset_card = cv_imread("object_card\\symbol\\paixu_verify_reset.png");
const cv::Mat img_symbol_jibaile50geqiangdao_card = cv_imread("object_card\\symbol\\jibaile50geqiangdao.png");

const cv::Mat img_cursors_cursor_card = cv_imread("object_card\\cursors\\cursor.png");


WindowInfo::WindowInfo(HANDLE processID) {
	pid = processID;

	//接任务站位(18,13)
	dianxiaoer_pos_list.push_back(POINT{ 230, 750 }); // (11,12)
	dianxiaoer_pos_list.push_back(POINT{ 230, 630 }); // (11,18)
	dianxiaoer_pos_list.push_back(POINT{ 470, 650 }); // (23,17)
	//接任务站位(18,11)
	dianxiaoer_pos_list.push_back(POINT{ 410, 850 }); // (20,7)
	//接任务站位(26,8)
	dianxiaoer_pos_list.push_back(POINT{ 450, 850 }); // (22,7)
	//接任务站位(26,9)
	dianxiaoer_pos_list.push_back(POINT{ 650, 790 }); // (32,10)
	//接任务站位(39,14)
	dianxiaoer_pos_list.push_back(POINT{ 750, 610 }); // (37,19)
	dianxiaoer_pos_list.push_back(POINT{ 890, 690 }); // (44,15)
	//接任务站位(34,16)
	dianxiaoer_pos_list.push_back(POINT{ 710, 710 }); // (35,14)
	//接任务站位(30,22)
	dianxiaoer_pos_list.push_back(POINT{ 570, 450 }); // (28,27)

	changan_yizhanlaoban_pos_list.push_back(POINT{ 5570, 4710 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5570, 4750 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5610, 4730 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5650, 4710 });
	changan_yizhanlaoban_pos_list.push_back(POINT{ 5650, 4750 });

}
void WindowInfo::init() {
	if (RegionMonthly) {
		m_img_btn_beibao= &img_btn_beibao;
		m_img_btn_package_prop= &img_btn_package_prop;
		m_img_btn_tingtingwufang= &img_btn_tingtingwufang;
		m_img_btn_npc_talk_close= &img_btn_npc_talk_close;
		m_img_btn_npc_talk_close2 = &img_btn_npc_talk_close2;
		m_img_btn_flag_loc= &img_btn_flag_loc;
		m_img_btn_shide_woyaoqu= &img_btn_shide_woyaoqu;
		m_img_btn_cancel_auto_round= &img_btn_cancel_auto_round;
		m_img_btn_cancel_zhanli= &img_btn_cancel_zhanli;
		m_img_btn_reset_auto_round = &img_btn_reset_auto_round;
		m_img_btn_woshilaishoushinide= &img_btn_woshilaishoushinide;
		m_img_btn_zeiwang_benshaoxiashilaititianxingdaode= &img_btn_zeiwang_benshaoxiashilaititianxingdaode;
		//m_img_btn_laozimeishijianwan = &img_btn_laozimeishijianwan;
		//m_img_btn_wozhengzaizhandouzhong = &img_btn_wozhengzaizhandouzhong;
		//m_img_btn_menpaichuanggaunhudong = &img_btn_menpaichuanggaunhudong;
		//m_img_btn_luguohenishuogehua = &img_btn_luguohenishuogehua;
		//m_img_btn_wohaixiangzaizhezhuanzhuan = &img_btn_wohaixiangzaizhezhuanzhuan;
		//m_img_btn_wojintianmeidaiqian_gaitianlaizhaoni = &img_btn_wojintianmeidaiqian_gaitianlaizhaoni;
		//m_img_btn_wosuibiankankan = &img_btn_wosuibiankankan;
		//m_img_btn_womenhouhuiyouqi = &img_btn_womenhouhuiyouqi;
		//m_img_btn_luanchiyao_hairenming = &img_btn_luanchiyao_hairenming;
		//m_img_btn_woshenmedoubuzuo = &img_btn_woshenmedoubuzuo;

		m_img_props_red_777= &img_props_red_777;
		m_img_props_white_777= &img_props_white_777;
		m_img_props_green_777= &img_props_green_777;
		m_img_props_yellow_777= &img_props_yellow_777;
		m_img_props_sheyaoxiang= &img_props_sheyaoxiang;
		m_img_npc_dianxiaoer= &img_npc_dianxiaoer;


		m_img_fight_health_100= &img_fight_health_100;
		m_img_fight_health_95= &img_fight_health_95;
		m_img_fight_health_90= &img_fight_health_90;
		m_img_fight_health_85= &img_fight_health_85;
		m_img_fight_health_80= &img_fight_health_80;
		m_img_fight_health_75= &img_fight_health_75;
		m_img_fight_health_70= &img_fight_health_70;
		m_img_fight_health_65= &img_fight_health_65;
		m_img_fight_health_60= &img_fight_health_60;
		m_img_fight_health_55= &img_fight_health_55;
		m_img_fight_health_50= &img_fight_health_50;
		m_img_fight_mana_100= &img_fight_mana_100;
		m_img_fight_mana_95= &img_fight_mana_95;
		m_img_fight_mana_90= &img_fight_mana_90;
		m_img_fight_mana_85= &img_fight_mana_85;
		m_img_fight_mana_80= &img_fight_mana_80;
		m_img_fight_mana_75=&img_fight_mana_75;
		m_img_fight_mana_70=&img_fight_mana_70;
		m_img_fight_mana_65=&img_fight_mana_65;
		m_img_fight_mana_60=&img_fight_mana_60;
		m_img_fight_mana_55=&img_fight_mana_55;
		m_img_fight_mana_50=&img_fight_mana_50;
		m_img_fight_fourman_title_gray=&img_fight_fourman_title_gray;
		m_img_fight_fourman_title2_gray = &img_fight_fourman_title2_gray;
		m_img_fight_do_hero_action=&img_fight_do_hero_action;
		m_img_fight_do_peg_action=&img_fight_do_peg_action;
		m_img_fight_auto=&img_fight_auto;
		m_img_fight_auto_round30 = &img_fight_auto_round30;

		m_img_symbol_map=&img_symbol_map;
		m_img_symbol_feixingfu_xiliangnvguo=&img_symbol_feixingfu_xiliangnvguo;
		m_img_symbol_feixingfu_baoxiangguo=&img_symbol_feixingfu_baoxiangguo;
		m_img_symbol_feixingfu_jianyecheng=&img_symbol_feixingfu_jianyecheng;
		m_img_symbol_feixingfu_changshoucun=&img_symbol_feixingfu_changshoucun;
		m_img_symbol_feixingfu_aolaiguo=&img_symbol_feixingfu_aolaiguo;
		m_img_symbol_feixingfu_zhuziguo=&img_symbol_feixingfu_zhuziguo;
		m_img_symbol_yidongdezi_gray=&img_symbol_yidongdezi_gray;
		//m_img_symbol_gaosunitadecangshenweizhi=&img_symbol_gaosunitadecangshenweizhi;
		m_img_symbol_wozhidaowomenlaodadexingzong=&img_symbol_wozhidaowomenlaodadexingzong;
		m_img_symbol_wabao_title_gray=&img_symbol_wabao_title_gray;
		m_img_symbol_task_track_gray=&img_symbol_task_track_gray;
		m_img_symbol_zeiwang=&img_symbol_zeiwang;
		m_img_symbol_paixu_verify_reset=&img_symbol_paixu_verify_reset;
		m_img_symbol_jibaile50geqiangdao = &img_symbol_jibaile50geqiangdao;

		m_img_cursors_cursor=&img_cursors_cursor;


		mScreen_x = 16;  // wWidth / 2 / 25  屏幕可以范围坐标跨度
		mScreen_y = 11;  // wHeight / 2 / 25 屏幕可以范围坐标跨度
	}
	else {
		//_card for 点卡服
		m_img_btn_beibao = &img_btn_beibao_card;
		m_img_btn_package_prop = &img_btn_package_prop_card;
		m_img_btn_tingtingwufang = &img_btn_tingtingwufang_card;
		m_img_btn_npc_talk_close = &img_btn_npc_talk_close_card;
		m_img_btn_npc_talk_close2 = &img_btn_npc_talk_close2_card;
		m_img_btn_flag_loc = &img_btn_flag_loc_card;
		m_img_btn_shide_woyaoqu = &img_btn_shide_woyaoqu_card;
		m_img_btn_cancel_auto_round = &img_btn_cancel_auto_round_card;
		m_img_btn_cancel_zhanli = &img_btn_cancel_zhanli_card;
		m_img_btn_reset_auto_round = &img_btn_reset_auto_round_card;
		m_img_btn_woshilaishoushinide = &img_btn_woshilaishoushinide_card;
		m_img_btn_zeiwang_benshaoxiashilaititianxingdaode = &img_btn_zeiwang_benshaoxiashilaititianxingdaode_card;
		//m_img_btn_laozimeishijianwan = &img_btn_laozimeishijianwan_card;
		//m_img_btn_wozhengzaizhandouzhong = &img_btn_wozhengzaizhandouzhong_card;
		//m_img_btn_menpaichuanggaunhudong = &img_btn_menpaichuanggaunhudong_card;
		//m_img_btn_luguohenishuogehua = &img_btn_luguohenishuogehua_card;
		//m_img_btn_wohaixiangzaizhezhuanzhuan = &img_btn_wohaixiangzaizhezhuanzhuan_card;
		//m_img_btn_wojintianmeidaiqian_gaitianlaizhaoni = &img_btn_wojintianmeidaiqian_gaitianlaizhaoni_card;
		//m_img_btn_wosuibiankankan = &img_btn_wosuibiankankan_card;
		//m_img_btn_womenhouhuiyouqi = &img_btn_womenhouhuiyouqi_card;
		//m_img_btn_luanchiyao_hairenming = &img_btn_luanchiyao_hairenming_card;
		//m_img_btn_woshenmedoubuzuo = &img_btn_woshenmedoubuzuo_card;

		m_img_props_red_777 = &img_props_red_777_card;
		m_img_props_white_777 = &img_props_white_777_card;
		m_img_props_green_777 = &img_props_green_777_card;
		m_img_props_yellow_777 = &img_props_yellow_777_card;
		m_img_props_sheyaoxiang = &img_props_sheyaoxiang_card;
		m_img_npc_dianxiaoer = &img_npc_dianxiaoer_card;


		m_img_fight_health_100 = &img_fight_health_100_card;
		m_img_fight_health_95 = &img_fight_health_95_card;
		m_img_fight_health_90 = &img_fight_health_90_card;
		m_img_fight_health_85 = &img_fight_health_85_card;
		m_img_fight_health_80 = &img_fight_health_80_card;
		m_img_fight_health_75 = &img_fight_health_75_card;
		m_img_fight_health_70 = &img_fight_health_70_card;
		m_img_fight_health_65 = &img_fight_health_65_card;
		m_img_fight_health_60 = &img_fight_health_60_card;
		m_img_fight_health_55 = &img_fight_health_55_card;
		m_img_fight_health_50 = &img_fight_health_50_card;
		m_img_fight_mana_100 = &img_fight_mana_100_card;
		m_img_fight_mana_95 = &img_fight_mana_95_card;
		m_img_fight_mana_90 = &img_fight_mana_90_card;
		m_img_fight_mana_85 = &img_fight_mana_85_card;
		m_img_fight_mana_80 = &img_fight_mana_80_card;
		m_img_fight_mana_75 = &img_fight_mana_75_card;
		m_img_fight_mana_70 = &img_fight_mana_70_card;
		m_img_fight_mana_65 = &img_fight_mana_65_card;
		m_img_fight_mana_60 = &img_fight_mana_60_card;
		m_img_fight_mana_55 = &img_fight_mana_55_card;
		m_img_fight_mana_50 = &img_fight_mana_50_card;
		m_img_fight_fourman_title_gray = &img_fight_fourman_title_gray_card;
		m_img_fight_fourman_title2_gray = &img_fight_fourman_title2_gray_card;
		m_img_fight_do_hero_action = &img_fight_do_hero_action_card;
		m_img_fight_do_peg_action = &img_fight_do_peg_action_card;
		m_img_fight_auto = &img_fight_auto_card;
		m_img_fight_auto_round30 = &img_fight_auto_round30_card;

		m_img_symbol_map = &img_symbol_map_card;
		m_img_symbol_feixingfu_xiliangnvguo = &img_symbol_feixingfu_xiliangnvguo_card;
		m_img_symbol_feixingfu_baoxiangguo = &img_symbol_feixingfu_baoxiangguo_card;
		m_img_symbol_feixingfu_jianyecheng = &img_symbol_feixingfu_jianyecheng_card;
		m_img_symbol_feixingfu_changshoucun = &img_symbol_feixingfu_changshoucun_card;
		m_img_symbol_feixingfu_aolaiguo = &img_symbol_feixingfu_aolaiguo_card;
		m_img_symbol_feixingfu_zhuziguo = &img_symbol_feixingfu_zhuziguo_card;
		m_img_symbol_yidongdezi_gray = &img_symbol_yidongdezi_gray_card;
		//m_img_symbol_gaosunitadecangshenweizhi = &img_symbol_gaosunitadecangshenweizhi_card;
		m_img_symbol_wozhidaowomenlaodadexingzong = &img_symbol_wozhidaowomenlaodadexingzong_card;
		m_img_symbol_wabao_title_gray = &img_symbol_wabao_title_gray_card;
		m_img_symbol_task_track_gray = &img_symbol_task_track_gray_card;
		m_img_symbol_zeiwang = &img_symbol_zeiwang_card;
		m_img_symbol_paixu_verify_reset = &img_symbol_paixu_verify_reset_card;
		m_img_symbol_jibaile50geqiangdao = &img_symbol_jibaile50geqiangdao_card;

		m_img_cursors_cursor = &img_cursors_cursor_card;
	}
	health_list = {
		*m_img_fight_health_100,
		*m_img_fight_health_95,
		*m_img_fight_health_90,
		*m_img_fight_health_85,
		*m_img_fight_health_80,
		*m_img_fight_health_75,
		*m_img_fight_health_70,
		*m_img_fight_health_65,
		*m_img_fight_health_60,
		*m_img_fight_health_55,
		*m_img_fight_health_50
	};
	mana_list = {
		*m_img_fight_mana_100,
		*m_img_fight_mana_95,
		*m_img_fight_mana_90,
		*m_img_fight_mana_85,
		*m_img_fight_mana_80,
		*m_img_fight_mana_75,
		*m_img_fight_mana_70,
		*m_img_fight_mana_65,
		*m_img_fight_mana_60,
		*m_img_fight_mana_55,
		*m_img_fight_mana_50
	};
}
void WindowInfo::data_reset() {
	dianxiaoer_pos_addr = 0;
	dianxiaoer_pos_x = 0;
	dianxiaoer_pos_y = 0;
	changan_yizhanlaoban_pos_addr = 0;
	changan_yizhanlaoban_pos_x = 0;
	changan_yizhanlaoban_pos_y = 0;
	baotu_target_scene_id = 0;
	baotu_target_pos = { 0, 0 };
	zeiwang_pos_addr = 0;
	zeiwang_scene_id = 0;
	zeiwang_id = 0;
	zeiwang_name.clear();
	zeiwang_pos = { 0,0 };
	zeiwang_fake_pos = { 0,0 };
	f_round = 0;
	npc_found = false;
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
		27);
	if (scene_id_addr == 0) log_error("查找场景id地址失败");

	// 店小二结构体静态地址
	//location_first_static_addr = getRelativeStaticAddressByAoB(
	//	hProcess,
	//	mhmainDllBase,
	//	"48 8D 0D ? ? ? ? 48 89 08 48 8B 53 08 48 89 50 08 48 8B 53 10 48 89 50 10",
	//	"xxx????xxxxxxxxxxxxxxxxxxx",
	//	3);
	//location_second_static_addr = getRelativeCallAddressByAoB(
	//	hProcess,
	//	mhmainDllBase,
	//	"48 8B 08 48 8D 56 40 48 8D 05 ? ? ? ? 48 89 45 E7",
	//	"xxxxxxxxxx????xxxx",
	//	10);
	//location_dynamic_addr_third_child_first_static_addr = getRelativeStaticAddressByAoB(
	//	hProcess,
	//	mhmainDllBase,
	//	"48 8D 05 ? ? ? ? 48 89 03 48 8D 4B 30 E8 ? ? ? ?",
	//	"xxx????xxxxxxxx????",
	//	3);

	// NPC结构静态指针地址
	npc_first_static_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"4C 8D 05 ? ? ? ? 4C 8B CB 4C 89 01 48 8B D1 44 8B C7",  // 得到4个结果，第1个就是想要的
		3);
	npc_loc_first_static_addr = getRelativeStaticAddressByAoB(
		hProcess,
		mhmainDllBase,
		"90 48 8D 05 ? ? ? ? 48 89 03 48 8D 4B 30",
		4);
}
void WindowInfo::datu() {
	if (popup_verify){
		SetForegroundWindow(hwnd);
		time_pawn_update();
		while (is_verifying()) {
			Sleep(100);
		}
		if (mp3_playing) {
			stop_laba();
			//std::thread(stop_laba).detach();
			//Sleep(800);
		}
		mp3_playing = false;
		popup_verify = false;
		auto image = hwnd2mat(hwnd);
		if (!is_hangup(image)) {
			// 自动战斗按钮出现，点击一下
			auto fight_rc = ROI_fight_action();
			auto auto_btn_pos = MatchingLoc(image, fight_rc, *m_img_fight_auto);
			if (auto_btn_pos.x > 0) {
				log_info("点击自动战斗:%d,%d", auto_btn_pos.x, auto_btn_pos.y);
				click_position_at_edge({ rect.left + fight_rc.x + auto_btn_pos.x, rect.top + fight_rc.y + auto_btn_pos.y }, -60);
			}
		}
		return;
	}
	if (tScan_npc != THREAD_IDLE) return;
	if (baotu_task_count >= 50)return;//一天只能打50次
	//必须要经常置顶窗口，否则梦幻后台不更新界面，截图是固定画面
	if (IsIconic(hwnd)) {
		// Window is minimized
		log_info(player_name.c_str());
		for (int i = 0; i < 5; i++) { log_info("窗口最小化，无法运行，请先激活窗口"); }
		failure = true;
		return;
	}
	if (is_fighting()) {
		//if (time_pawn.timeout(5000)) {
		//	SetForegroundWindow(hwnd);
		//	time_pawn_update();
		//}
		if (f_round < 1) {
			SetForegroundWindow(hwnd);
			//只处理第一回合的战斗，其他回合挂机
			handle_datu_fight();
		}
		if (step.current == &work_start) { step.set_current(&to_changan_jiudian); }//战斗中，打完直接去接任务
		return;
	}
	if (moving) {
		if (is_moving()) {
			if (time_pawn.timeout(35000)) {
				SetForegroundWindow(hwnd);
				handle_sheyaoxiang_time();
				time_pawn_update();
			}
			return;
		}
	}
	//SetForegroundWindow(hwnd);
	time_pawn_update();
	if (step.current == &work_start) {
		// 重新运行程序，任务步骤预检查
		log_info("work_start,%s",player_id);
		//Sleep(150); // 有时候战斗退出后会显示任务界面，这里等待一会再检查
		step.next();
		cv::Mat image = hwnd2mat(hwnd);
		cv::Mat image_gray;
		cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
		if (MatchingLoc(image_gray,ROI_task(), *m_img_symbol_task_track_gray).x>-1) {
			if (MatchingLoc(image_gray,ROI_task(), *m_img_symbol_wabao_title_gray, "", 0.91).x>-1) {
				if (MatchingLoc(image,ROI_task(), *m_img_symbol_zeiwang, "", 0.91).x>-1) {
					log_info("贼王没打，开始打贼王,%s", player_id);
					tScan_npc = TASK_ZEIWANG;
					step.set_current(&goto_zeiwang_scene);
					auto pixel = MatchingLoc(image,ROI_npc_talk(), *m_img_btn_npc_talk_close);
					if (pixel.x > -1) {
						SetForegroundWindow(hwnd);
						close_npc_talk();
					}
				}
				else {
					log_info("已接打图任务，继续任务,%s", player_id);
					tScan_npc = TASK_BAOTU;
					step.set_current(&close_dianxiaoer);
				}
			}
		}
	}
	else if (step.current == &to_changan_jiudian) {
		log_info("to_changan_jiudian,%s", player_id);
		update_scene_id();
		if (m_scene_id != 长安酒店) {
			SetForegroundWindow(hwnd);
			goto_changanjiudian();
		}
		if (m_scene_id == 长安酒店) {
			tScan_npc = 店小二;
			npc_found = false;
			step.set_current(&to_dianxiaoer_get_task);
		}
	}
	else if (step.current == &scan_dianxiaoer_pos) {
		log_info("scan_dianxiaoer,%s", player_id);
		update_scene_id();
		if (m_scene_id != 长安酒店) step.set_current(&to_changan_jiudian);
		else {
			tScan_npc = 店小二;
			npc_found = false;
			step.next();
		}
	}
	else if (step.current == &to_dianxiaoer_get_task) {
		if (npc_found) {
			//线程扫描结束，开始移动
			move_to_dianxiaoer();
			if (is_near_dianxiaoer()) {
				SetForegroundWindow(hwnd);
				log_info("talk_get_baoturenwu,%s", player_id);
				if (talk_to_dianxiaoer()) {
					if (WaitMatchingGrayRectExist(ROI_task(), *m_img_symbol_wabao_title_gray, 600)) {
						// 接任务后关闭对话窗
						//if (close_npc_talk_fast()) {
						//	tScan_npc = TASK_BAOTU;
						//	step.next();
						//}
						tScan_npc = TASK_BAOTU;
						step.next();
					}
					else {
						if (MatchingRectExist(ROI_npc_talk(), *m_img_symbol_jibaile50geqiangdao)) {
							baotu_task_count = 50;
							data_reset();
							return;
						}
						if (is_verifying()) {
							play_mp3_once();
							popup_verify = true;
							for (int i = 0; i < 5; i++) { log_info("***宝图任务弹窗验证,请手动点击***"); }
							return;
						}
						step.set_current(&scan_dianxiaoer_pos);
					}
				}
			}
			else {
				if (!is_npc_visible(dianxiaoer_pos_addr)) {
					step.set_current(&scan_dianxiaoer_pos);
					move_to_changanjidian_center();
				}
			}
		}
		else {
			step.set_current(&scan_dianxiaoer_pos);
			move_to_changanjidian_center();
		}
	}
	else if (step.current == &close_dianxiaoer) {
		if (baotu_target_scene_id <= 0 || baotu_target_pos.x <= 0) {
			log_info("解析宝图任务失败，需要手动处理");
			failure = true;
		}
		else {
			SetForegroundWindow(hwnd);
			close_npc_talk_fast();
			goto_scene(baotu_target_pos, baotu_target_scene_id);
			step.next();
		}
	}
	else if (step.current == &goto_baotu_scene) {
		if (baotu_target_scene_id <= 0 || baotu_target_pos.x <= 0) {
			log_info("解析宝图任务失败，需要手动处理");
			failure = true;
		}
		else {
			SetForegroundWindow(hwnd);
			if (goto_scene(baotu_target_pos, baotu_target_scene_id)) {
				step.next();
			}
		}
	}
	else if (step.current == &attack_qiangdao) {
		log_info("attack_qiangdao,%s", player_id);
		if (is_near_loc(baotu_target_pos, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE)) {
			SetForegroundWindow(hwnd);
			talk_to_npc_fight(baotu_target_pos, *m_img_btn_woshilaishoushinide);
			step.next();
		}
		else { step.set_current(&goto_baotu_scene); };
	}
	else if (step.current == &scan_zeiwang_task) {
		log_info("scan_zeiwang_task,%s", player_id);
		SetForegroundWindow(hwnd);
		move_cursor_center_bottom();
		if (WaitMatchingRectExist(ROI_npc_talk(), *m_img_symbol_wozhidaowomenlaodadexingzong, 100)) {
			//关闭贼王提示对话窗
			close_npc_talk_fast();
			tScan_npc = TASK_ZEIWANG;
			step.next();
		}
		else { 
			data_reset(); 	
		}
	}
	else if (step.current == &goto_zeiwang_scene) {
		if (zeiwang_scene_id <= 0) {
			log_info("解析贼王失败，需要手动处理");
			failure = true;
		}
		else {
			SetForegroundWindow(hwnd);
			log_info("goto_zeiwang_scene");
			if (goto_scene(zeiwang_fake_pos, zeiwang_scene_id)) {
				if (zeiwang_fake_pos.x > 0) {
					//对于有坐标的贼王，直接寻路到坐标点就行
					step.set_current(&scan_zeiwang_pos);
				}
				else {
					step.next();
				}
			}
		}
	}
	else if (step.current == &fix_my_pos_zeiwang) {
		// 对于大地图，要走到能看到全图的特定位置再开始扫描
		log_info("fix_my_pos_zeiwang");
		switch (zeiwang_scene_id)
		{
		case 长安杂货店:
		{
			break;
		}
		case 长安饰品店:
		{
			break;
		}
		case 长安国子监:
		{
			break;
		}
		//case 建邺杂货店:
		//{
		//	break;
		//}
		case 傲来客栈:
		{
			break;
		}
		case 傲来客栈二楼:
		{
			break;
		}
		case 傲来国药店:
		{
			break;
		}
		//case 长寿村当铺:
		//{
		//	break;
		//}
		//case 长寿郊外:
		//{
		//	break;
		//}
		default:
			break;
		}
		step.next();
	}
	else if (step.current == &scan_zeiwang_pos) {
		log_info("scan_zeiwang_pos");
		//if (zeiwang_fake_pos.x > 0) {
		//	SetForegroundWindow(hwnd);
		//	move_to_position(zeiwang_fake_pos, MAP_MOVE_DISTENCE, MAP_MOVE_DISTENCE);
		//}
		tScan_npc = 贼王;
		step.next();
	}
	else if (step.current == &attack_zeiwang) {
		if (zeiwang_pos.x <= 0) {
			log_info("处理贼王坐标失败，需要手动处理");
			failure = true;
			return;
		}
		SetForegroundWindow(hwnd);
		log_info("贼王坐标:%d,%d", zeiwang_pos.x, zeiwang_pos.y);
		if (!is_near_loc(zeiwang_pos, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE)) {
			log_info("goto_zeiwang");
			move_to_position(zeiwang_pos, MAP_MOVE_DISTENCE, MAP_MOVE_DISTENCE);
		}
		else {
			log_info("attack_zeiwang");
			if (talk_to_npc_fight(zeiwang_pos, *m_img_btn_zeiwang_benshaoxiashilaititianxingdaode)) {
				//step.set_current(&baotu_end);
				data_reset();
			}
			else {
				step.set_current(&fix_my_pos_zeiwang);
			}
		}
	}
	else if (step.current == &baotu_end) {
		data_reset();
	}
}
std::vector<uintptr_t> WindowInfo::ScanMemoryRegionEx(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, const BYTE* pattern, size_t pattern_size, const char* mask)
{
	std::vector<uintptr_t> res;
	BYTE* buffer = new BYTE[regionSize];
	// 分配并初始化坏字符表
	//unsigned char badChar[ALPHABET_SIZE];
	//preProcessBadChar(pattern, mask, pattern_size, badChar);
	SIZE_T bytesRead;
	if (pNtReadVirtualMemory(hProcess, (PVOID)startAddress, buffer, regionSize, &bytesRead) == 0)
		//if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
	{
		//log_info("startAddress:0x%llX, regionSize:0x%llX", startAddress, regionSize);
		for (SIZE_T i = 0; i <= bytesRead - pattern_size; i++)  // Updated loop condition
		{
			bool found = true;
			for (SIZE_T j = 0; j < pattern_size; j++)
			{
				if (!mask[j] && buffer[i + j] != pattern[j])
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
				i += pattern_size; // 跳过已对比过的字段
			}
		}
	}

	delete[] buffer;
	return res;
}
std::vector<uintptr_t> WindowInfo::PerformAoBScanEx(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern)
{
	// ModuleBase 为NULL则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
	std::vector<unsigned char> pattern_bytes;
	std::vector<char> pattern_mask;

	parseAobString(pattern, pattern_bytes, pattern_mask);

	// Get the raw pointer using the address-of operator on the first element
	BYTE* aob_Array = &pattern_bytes[0];
	char* mask_Array = &pattern_mask[0];
	std::vector<uintptr_t> res;
	DWORD ModuleSize = GetModuleSize(hProcess, ModuleBase);

	// Enumerate memory regions in the target process
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	LPVOID minimumApplicationAddress = systemInfo.lpMinimumApplicationAddress;
	LPVOID maximumApplicationAddress = systemInfo.lpMaximumApplicationAddress;
	if (ModuleBase) {
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
			if (ModuleBase) scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE);
			else scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && memoryInfo.Type == MEM_PRIVATE && memoryInfo.Protect == PAGE_READWRITE;
			// Check if the memory region is accessible and not reserved
			if (scan_condition)
				//if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE))
			{
				// Scan the memory region
				auto matchAddress = ScanMemoryRegionEx(hProcess, memoryInfo.BaseAddress, memoryInfo.RegionSize, aob_Array, pattern_bytes.size(), mask_Array);
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

uintptr_t WindowInfo::ScanMemoryRegion(HANDLE hProcess, LPCVOID startAddress, SIZE_T regionSize, const BYTE* pattern, size_t pattern_size, const char* mask)
{
	uintptr_t matchAddress = 0;
	BYTE* buffer = new BYTE[regionSize];

	SIZE_T bytesRead;
	if (pNtReadVirtualMemory(hProcess, (PVOID)startAddress, buffer, regionSize, &bytesRead) == 0)
		//if (ReadProcessMemory(hProcess, startAddress, buffer, regionSize, &bytesRead))
	{
		//log_info("startAddress:0x%llX, regionSize:0x%llX", startAddress, regionSize);
		for (SIZE_T i = 0; i <= bytesRead - pattern_size; i++)  // Updated loop condition
		{
			bool found = true;
			for (SIZE_T j = 0; j < pattern_size; j++)
			{
				if (!mask[j] && buffer[i + j] != pattern[j])
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

uintptr_t WindowInfo::PerformAoBScan(HANDLE hProcess, HMODULE ModuleBase, const std::string pattern)
{
	// ModuleBase 为NULL则扫描PRV内存，
	// all 为true，则扫描全部匹配结果,为false扫描返回第一个
	std::vector<unsigned char> pattern_bytes;
	std::vector<char> pattern_mask;

	parseAobString(pattern, pattern_bytes, pattern_mask);
	// Get the raw pointer using the address-of operator on the first element
	BYTE* aob_Array = &pattern_bytes[0];
	char* mask_Array = &pattern_mask[0];

	uintptr_t matchAddress = 0;
	DWORD ModuleSize = GetModuleSize(hProcess, ModuleBase);

	// Enumerate memory regions in the target process
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	LPVOID minimumApplicationAddress = systemInfo.lpMinimumApplicationAddress;
	LPVOID maximumApplicationAddress = systemInfo.lpMaximumApplicationAddress;
	if (ModuleBase) {
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
			if (ModuleBase) scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE);
			else scan_condition = memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && memoryInfo.Type == MEM_PRIVATE && memoryInfo.Protect == PAGE_READWRITE;
			// Check if the memory region is accessible and not reserved
			if (scan_condition)
				//if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && !(memoryInfo.Type & MEM_PRIVATE))
			{
				// Scan the memory region
				matchAddress = ScanMemoryRegion(hProcess, memoryInfo.BaseAddress, memoryInfo.RegionSize, aob_Array, pattern_bytes.size(), mask_Array);
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

uintptr_t WindowInfo::getRelativeStaticAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, size_t offset) {
	// adr_offset = rav_offset - 3
	// opcode_adr = AoB_adr + adr_offset
	// StaticAddress - opcode_adr - 7 = rav
	//auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB, mask);
	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB);
	if (AoB_adr <= 0) return AoB_adr;

	SIZE_T regionSize = 0x10;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)(AoB_adr + offset), buffer, regionSize, &bytesRead);
	auto rav = *reinterpret_cast<long*>(buffer);
	delete[] buffer;
	return rav + 7 + AoB_adr + offset - 3;
}

uintptr_t WindowInfo::getRelativeCallAddressByAoB(HANDLE hProcess, HMODULE ModuleBase, std::string AoB, size_t offset) {
	// adr_offset = rav_offset - 1
	// opcode_adr = AoB_adr + adr_offset
	// StaticAddress - opcode_adr - 5 = rav
	auto AoB_adr = PerformAoBScan(hProcess, ModuleBase, AoB);
	if (AoB_adr <= 0) return AoB_adr;

	SIZE_T regionSize = 0x10;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)(AoB_adr + offset), buffer, regionSize, &bytesRead);
	auto rav = *reinterpret_cast<long*>(buffer);
	delete[] buffer;
	return rav + 5 + AoB_adr + offset - 1;
}

bool WindowInfo::MatchingRectExist(cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method)
{
	return MatchingRectLoc(roi_rect, templ, mask_path, threshold, match_method, MATCHEXIST).x > -1;
}
bool WindowInfo::MatchingGrayRectExist(cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method)
{
	auto image = hwnd2mat(hwnd);
	cv::Mat img_gray;
	cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);
	return MatchingLoc(img_gray, roi_rect, templ, mask_path, threshold, match_method).x>-1;
}
POINT WindowInfo::MatchingRectLoc(cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method, int loc) {
	auto image = hwnd2mat(hwnd);
	return MatchingLoc(image, roi_rect, templ, mask_path, threshold, match_method, loc);
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
POINT WindowInfo::WaitMatchingRectLoc(cv::Rect roi_rect, const cv::Mat& templ, int timeout, std::string mask_path, double threshold, int match_method, int loc) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto pos = MatchingRectLoc(roi_rect, templ, mask_path, threshold, match_method, loc);
		if (pos.x > 0) return {rect.left + pos.x, rect.top + pos.y};
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时");
			break;
		}
	}
	return { -1,-1 };
}
bool WindowInfo::WaitMatchingGrayRectExist(cv::Rect roi_rect, const cv::Mat& templ, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto match = MatchingGrayRectExist(roi_rect, templ, mask_path, threshold, match_method);
		if (match) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时");
			break;
		}
	}
	return false;
}
bool WindowInfo::WaitMatchingRectExist(cv::Rect roi_rect, const cv::Mat& templ, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		auto match = MatchingRectExist(roi_rect, templ, mask_path, threshold, match_method);
		if (match) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时");
			break;
		}
	}
	return false;
}

bool WindowInfo::WaitMatchingRectDisapper(cv::Rect roi_rect, const cv::Mat& templ, int timeout, std::string mask_path, double threshold, int match_method) {
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		if (!MatchingRectExist(roi_rect, templ, mask_path, threshold, match_method)) return true;
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("超时");
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
	//log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
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

void WindowInfo::scan_npc_pos_in_thread() {
	while (1) {
		//log_info("scan_npc_pos_in_thread");
		switch (tScan_npc)
		{
			case 店小二:
			case 长安驿站老板:
			{
				scan_npc_pos_addr_by_id(tScan_npc);
				tScan_npc = THREAD_IDLE;
				break;
			}
			case 贼王:
			{
				//Sleep(3000);// 等待内存刷新，再开始扫描
				scan_zeiwang_id();
				if (zeiwang_id > 0) { scan_npc_pos_addr_by_id(贼王); }
				tScan_npc = THREAD_IDLE;
				break;
			}
			case TASK_BAOTU:
			{
				if (RegionMonthly) {
					//Sleep(1500);// 刚接任务，内容没生成，等待一下再开始扫描
					parse_baotu_task_info();
				}
				else {
					parse_baotu_task_info_card();
				}
				tScan_npc = THREAD_IDLE;
				break;
			}
			case TASK_ZEIWANG:
			{
				parse_zeiwang_info();
				tScan_npc = THREAD_IDLE;
				break;
			}
		}
		Sleep(100);
	}

}
//void WindowInfo::scan_npc_pos_addr(int npc) {
//	// 这个结构不是每次都会出现的，如果找不到，可以先出去再进来，重复试几次一般都会产生这个内存结构
//	// 先找到 #c80c0ff挖宝图任务 这个地址
//	//auto wabaoturenwu_AoB_adr = PerformAoBScan(
//	//	hProcess,
//	//	0,
//	//	"23 63 38 30 63 30 66 66 CD DA B1 A6 CD BC C8 CE CE F1 00",
//	//	"xxxxxxxxxxxxxxxxxxx");
//	//// 然后结构体上方有个地址指针，指向一个结构，这个结构包含店小二的动态坐标
//	//// 注意：店小二如果这个离开了玩家视野，这个地址需要重新查找，也就是说这个地址要店小二出现在玩家视野中才会出现
//	//SIZE_T regionSize = 0x8;
//	//BYTE* buffer = new BYTE[regionSize];
//	//SIZE_T bytesRead;
//	//pNtReadVirtualMemory(hProcess, (PVOID)(wabaoturenwu_AoB_adr - 0x40), buffer, regionSize, &bytesRead);
//	//auto ptr = *reinterpret_cast<QWORD*>(buffer);
//	//delete[] buffer;
//
//	//if (bytesRead > 0) {
//	//	bytesRead = 0;
//	//	buffer = new BYTE[regionSize];
//	//	pNtReadVirtualMemory(hProcess, (PVOID)(ptr), buffer, regionSize, &bytesRead);
//	//	if (bytesRead > 0) {
//	//		dianxiaoer_pos_addr = ptr + 0x4C;
//	//		log_info("店小二坐标地址:0x%llX", dianxiaoer_pos_addr);
//	//	}
//	//	delete[] buffer;
//	//}
//	uintptr_t pos_addr=-1;
//	if (npc == 店小二) {
//		log_info("查找店小二坐标开始:0x%p", hProcess);
//		pos_addr = dianxiaoer_pos_addr;
//		dianxiaoer_pos_x = 0;
//		dianxiaoer_pos_y = 0;
//	}
//	else if (npc == 长安驿站老板) {
//		log_info("查找驿站老板坐标开始:0x%p", hProcess);
//		pos_addr = changan_yizhanlaoban_pos_addr;
//		changan_yizhanlaoban_pos_x = 0;
//		changan_yizhanlaoban_pos_y = 0;
//	}
//	else if (npc == NPC_ZEIWANG) {
//		pos_addr = 0;
//	}
//	if (pos_addr == 0) {
//		// 结构特点：2个静态地址+一个动态地址，动态地址开头包含6个指针，第1个指针为静态地址，第5和6的指针为空
//		std::string struct_AoB;
//		auto ptr1 = reinterpret_cast<char*>(&location_first_static_addr);
//		for (int i = 0; i < 8; i++) {
//			auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
//			char hexStr[3];
//			sprintf(hexStr, "%2X ", c);
//			struct_AoB += hexStr;
//		}
//		auto ptr2 = reinterpret_cast<char*>(&location_second_static_addr);
//		for (int i = 0; i < 8; i++) {
//			auto c = *reinterpret_cast<const unsigned char*>(ptr2 + i);
//			char hexStr[3];
//			sprintf(hexStr, "%2X ", c);
//			struct_AoB += hexStr;
//		}
//		struct_AoB += "? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 00 00 80 3F 00 00 80 3F";
//		//std::string struct_AoB = "38 E0 D1 30 9D FB 7F 00 00 98 0D 9B 9B FB 7F 00 00 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 00 00 80 3F 00 00 80 3F";
//		//struct_AoB = "9D FB 7F 00 00";
//		auto scan_ret = PerformAoBScanEx(
//			hProcess,
//			0,
//			struct_AoB,
//			"xxxxxxxxxxxxxxxx????????????????xxxxxxxx");
//		//auto scan_ret = PerformAoBScanEx(
//		//	hProcess,
//		//	0,
//		//	struct_AoB,
//		//	"xxxxxxxxxxxxxxxx????????????????xxxxxxxx");
//
//		auto npc_list = get_scene_npc_list(zeiwang_scene_id);
//
//		for (const auto& item : scan_ret) {
//			SIZE_T regionSize = 0x38;
//			BYTE* buffer = new BYTE[regionSize];
//			BYTE* buffer1 = new BYTE[regionSize];
//			SIZE_T bytesRead;
//			pNtReadVirtualMemory(hProcess, (PVOID)(item + 0x10), buffer, regionSize, &bytesRead);  // 动态地址
//			if (bytesRead > 0) {
//				auto heap_add = *reinterpret_cast<QWORD*>(buffer);
//				bytesRead = 0;
//				pNtReadVirtualMemory(hProcess, (PVOID)heap_add, buffer1, regionSize, &bytesRead);
//				if (bytesRead > 0) {
//					auto m_static_child_addr1 = *reinterpret_cast<QWORD*>(buffer1);
//					if (m_static_child_addr1 == location_dynamic_addr_third_child_first_static_addr) {
//						auto heap_child_addr5 = *reinterpret_cast<QWORD*>(buffer1 + 0x20);
//						auto heap_child_addr6 = *reinterpret_cast<QWORD*>(buffer1 + 0x28);
//						if (heap_child_addr5 == 0 && heap_child_addr6 == 0) {
//							auto x = *reinterpret_cast<float*>(buffer1 + 0x30);
//							auto y = *reinterpret_cast<float*>(buffer1 + 0x34);
//							//log_info("npc坐标:%f,%f", x, y);
//							// 长安酒店内有自己坐标，酒楼老板坐标，店小二坐标 酒楼老板坐标(910, 570)
//							update_player_float_pos();
//							if (x != player_x && y != player_y) {
//								// 这个结构包含所有NPC和玩家（包括自己）的坐标，所以要做过滤
//								if (npc == 店小二) {
//									if (is_given_pos(x, y, dianxiaoer_pos_list)) {
//										dianxiaoer_pos_x = x;
//										dianxiaoer_pos_y = y;
//										dianxiaoer_pos_addr = item;
//										log_info("店小二坐标地址:0x%llX", item);
//										break;
//									}
//								}
//								else if (npc == 长安驿站老板) {
//									if (is_given_pos(x, y, changan_yizhanlaoban_pos_list)) {
//										changan_yizhanlaoban_pos_x = x;
//										changan_yizhanlaoban_pos_y = y;
//										changan_yizhanlaoban_pos_addr = item;
//										log_info("驿站老版坐标地址:0x%llX", item);
//										break;
//									}
//								}
//								else if (npc == NPC_ZEIWANG) {
//									if (x > 0 && y > 0) log_info("npc坐标:%f,%f", x, y);
//									if (!is_given_pos(x,y,npc_list)) {
//										zeiwang_pos_list.push_back({ (long)x,(long)y });
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//
//			delete[] buffer;
//			delete[] buffer1;
//		}
//		log_info("查找坐标结束:0x%p", hProcess);
//	}
//}

void WindowInfo::scan_npc_pos_addr_by_id(unsigned int npc) {
	// 根据NPC id得到坐标,适用玩家+自己坐标
	// 扫描包含NPC id的结构体，具体为：开头静态地址+一个动态地址+NPC_ID(4byte)
	//坐标地址: [[[[addr + 0x28]+ 0xA0] + 0x10] ] + 0x4C
	uintptr_t search_addr = 0;
	unsigned int npc_id = npc;
	if (!RegionMonthly) {
		if (npc == 店小二)npc_id = 店小二_card;
		else if (npc == 长安驿站老板)npc_id = 长安驿站老板_card;
	}
	if (npc == 贼王)npc_id = zeiwang_id;
	log_info("查找%d坐标开始:%s", npc_id, player_id);

	// 结构特点：2个静态地址+一个动态地址，动态地址开头包含6个指针，第1个指针为静态地址，第5和6的指针为空
	std::string struct_AoB;
	auto ptr1 = reinterpret_cast<char*>(&npc_first_static_addr);
	for (int i = 0; i < 8; i++) {
		auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
		char hexStr[3];
		sprintf(hexStr, "%2X ", c);
		struct_AoB += hexStr;
	}
	struct_AoB += "? ? ? ? ? ? ? ?";
	//unsigned int var = 店小二;
	//unsigned int var = 16710472;
	auto ptr2 = reinterpret_cast<unsigned char*>(&npc_id);
	for (int i = 0; i < 4; i++) {
		auto c = *reinterpret_cast<const unsigned char*>(ptr2 + i);
		char hexStr[3];
		sprintf(hexStr, " %2X", c);
		struct_AoB += hexStr;
	}
	auto npc_id_addr = PerformAoBScan(
		hProcess,
		0,
		struct_AoB);
	if (npc_id_addr > 0) {
		log_info("找到NPC id结构地址");
		SIZE_T regionSize = 0xA8;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)npc_id_addr, buffer, regionSize, &bytesRead);  // 静态地址
		if (bytesRead > 0) {
			auto heap_add = *reinterpret_cast<QWORD*>(buffer + 0x28);
			bytesRead = 0;
			memset(buffer, 0, regionSize);
			pNtReadVirtualMemory(hProcess, (PVOID)heap_add, buffer, regionSize, &bytesRead);
			if (bytesRead > 0) {
				auto heap_child1_addr = *reinterpret_cast<QWORD*>(buffer + 0xA0);
				bytesRead = 0;
				memset(buffer, 0, regionSize);
				pNtReadVirtualMemory(hProcess, (PVOID)heap_child1_addr, buffer, regionSize, &bytesRead);
				if (bytesRead > 0) {
					int offset = RegionMonthly ? 0x20 : 0x10;// 畅玩服和点卡服这个结构有点不一样
					auto p_npc_loc_addr = *reinterpret_cast<QWORD*>(buffer + offset);
					bytesRead = 0;
					memset(buffer, 0, regionSize);
					pNtReadVirtualMemory(hProcess, (PVOID)p_npc_loc_addr, buffer, regionSize, &bytesRead);
					if (bytesRead > 0) {
						auto npc_loc_addr = *reinterpret_cast<QWORD*>(buffer);
						if (is_npc_visible(npc_loc_addr)) {
							npc_found = true;
							search_addr = npc_loc_addr;
							log_info("找到坐标地址:0x%p", search_addr);
						}
					}
				}
			}
		}
		delete[] buffer;
	}
	
	if (npc == 店小二) {
		dianxiaoer_pos_addr = search_addr;
	}
	else if (npc == 长安驿站老板) {
		changan_yizhanlaoban_pos_addr = search_addr;
	}
	else if (npc == 贼王) {
		zeiwang_pos_addr = search_addr;
		update_npc_pos(贼王);
	}
	log_info("查找坐标结束:%s", player_id);
}
void WindowInfo::scan_zeiwang_id() {
	if (!zeiwang_name.empty()) {
		log_info("查找贼王id开始:%s", player_id);
		auto ptr1 = reinterpret_cast<char*>(&npc_first_static_addr);
		for (int i = 0; i < 8; i++) {
			auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
			char hexStr[3];
			sprintf(hexStr, "%2X ", c);
		}
		auto npc_waixing_list = PerformAoBScanEx(
			hProcess,
			NULL,
			"A0 52 7D 8F 18 4F 48 51 A7 7E 3D 00 38 00 20 00 16 59 62 5F 3D 00 32 00 30 00 34 00 35 00 20 00 F6 65 C5 88 35 00 3D 00 30 00 20 00"  //加载优先级=8 外形=2045 时装5=0 特效=0 
		);
		int symbol_len = 44;
		for (const auto& wai_xing : npc_waixing_list) {
			SIZE_T regionSize = 0x200;
			BYTE* buffer = new BYTE[regionSize];
			SIZE_T bytesRead;
			pNtReadVirtualMemory(hProcess, (PVOID)(wai_xing - regionSize + symbol_len), buffer, regionSize, &bytesRead);  // 静态地址
			if (bytesRead > 0) {
				std::wstring content;
				for (int i = 0; i < bytesRead - 8; i++) {
					if (buffer[i] == 0x00 && buffer[i + 1] == 0x00 && buffer[i + 2] == 0xB9 && buffer[i + 3] == 0x65 && buffer[i + 4] == 0x11 && buffer[i + 5] == 0x54 && buffer[i + 6] == 0x3D && buffer[i + 7] == 0x00) {	// 方向=:B9 65 11 54 3D 00
						content = bytes_to_wstring(&buffer[i+2], bytesRead - i+2 - symbol_len);
						break;
					}
				}
				if (!content.empty()) {
					auto tags_wstr = findContentBetweenTags(content, L"id=", L" X=");
					if (!tags_wstr.empty()) {
						zeiwang_id = std::stoi(tags_wstr.at(0));
						log_info("找到贼王id:%d", zeiwang_id);
					}
				}
			}
		}
		log_info("查找贼王id结束:%s", player_id);
	}
}
void WindowInfo::scan_current_scene_npc_id() {
	std::string struct_AoB;
	auto ptr1 = reinterpret_cast<char*>(&npc_first_static_addr);
	for (int i = 0; i < 8; i++) {
		auto c = *reinterpret_cast<const unsigned char*>(ptr1 + i);
		char hexStr[3];
		sprintf(hexStr, "%2X ", c);
		struct_AoB += hexStr;
	}
	struct_AoB.substr(0, struct_AoB.size() - 1);
	auto npc_id_addrs = PerformAoBScanEx(
		hProcess,
		NULL,
		struct_AoB);
	for (const auto &npc_id_addr : npc_id_addrs) {
		SIZE_T regionSize = 0xA8;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)npc_id_addr, buffer, regionSize, &bytesRead);  // 静态地址
		if (bytesRead > 0) {
			auto npc_id = *reinterpret_cast<QWORD*>(buffer + 0x10);
			auto heap_add = *reinterpret_cast<QWORD*>(buffer + 0x28);
			bytesRead = 0;
			memset(buffer, 0, regionSize);
			pNtReadVirtualMemory(hProcess, (PVOID)heap_add, buffer, regionSize, &bytesRead);
			if (bytesRead > 0) {
				auto heap_child1_addr = *reinterpret_cast<QWORD*>(buffer + 0xA0);
				bytesRead = 0;
				memset(buffer, 0, regionSize);
				pNtReadVirtualMemory(hProcess, (PVOID)heap_child1_addr, buffer, regionSize, &bytesRead);
				if (bytesRead > 0) {
					int offset = RegionMonthly?0x20:0x10;// 畅玩服和点卡服这个结构有点不一样
					auto p_npc_loc_addr = *reinterpret_cast<QWORD*>(buffer + offset);
					bytesRead = 0;
					memset(buffer, 0, regionSize);
					pNtReadVirtualMemory(hProcess, (PVOID)p_npc_loc_addr, buffer, regionSize, &bytesRead);
					if (bytesRead > 0) {
						auto npc_loc_addr = *reinterpret_cast<QWORD*>(buffer);
						if (is_npc_visible(npc_loc_addr)) {
							bytesRead = 0;
							memset(buffer, 0, regionSize);
							pNtReadVirtualMemory(hProcess, (PVOID)npc_loc_addr, buffer, regionSize, &bytesRead);
							if (bytesRead > 0) {
								auto float_x = *reinterpret_cast<float*>(buffer + 0x4C);
								auto float_y = *reinterpret_cast<float*>(buffer + 0x50);
								auto x = convert_to_map_pos_x(float_x);
								auto y = convert_to_map_pos_y(float_y);
								log_info("找到id:%d, (%d,%d)", npc_id, x, y);
							}
						}
					}
				}
			}
		}
		delete[] buffer;
	}
}
//void WindowInfo::update_npc_pos(int npc) {
//	// 读取更新坐标
//	uintptr_t pos_addr = -1;
//	if (npc == 店小二) {
//		pos_addr = dianxiaoer_pos_addr;
//		dianxiaoer_pos_x = 0;
//		dianxiaoer_pos_y = 0;
//	}
//	else if (npc == 长安驿站老板) {
//		pos_addr = changan_yizhanlaoban_pos_addr;
//		changan_yizhanlaoban_pos_x = 0;
//		changan_yizhanlaoban_pos_y = 0;
//	}
//	if (pos_addr != -1) {
//		SIZE_T regionSize = 0x24;
//		BYTE* buffer = new BYTE[regionSize];
//		SIZE_T bytesRead;
//		pNtReadVirtualMemory(hProcess, (PVOID)pos_addr, buffer, regionSize, &bytesRead);
//		if (bytesRead > 0) {
//			auto first_static_addr = *reinterpret_cast<uintptr_t*>(buffer);
//			if (location_first_static_addr == first_static_addr) {
//				auto x = *reinterpret_cast<float*>(buffer + 0x18);
//				auto y = *reinterpret_cast<float*>(buffer + 0x1C);
//				if (npc == 店小二) {
//					if (is_given_pos(x, y, dianxiaoer_pos_list)) {
//						dianxiaoer_pos_x = x;
//						dianxiaoer_pos_y = y;
//						log_info("店小二坐标地址:%f,%f", x,y);
//						auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
//						auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
//						log_info("店小二坐标:%d, %d", dxe_x, dxe_y);
//					}
//				}
//				else if (npc == 长安驿站老板) {
//					if (is_given_pos(x, y, changan_yizhanlaoban_pos_list)) {
//						changan_yizhanlaoban_pos_x = x;
//						changan_yizhanlaoban_pos_y = y;
//					}
//				}
//			}
//			else {
//				// 店小二消失，内存释放后，结构体变了
//				if (npc == 店小二) {
//					dianxiaoer_pos_addr = 0;
//				}
//				else if (npc == 长安驿站老板) {
//					changan_yizhanlaoban_pos_addr = 0;
//				}
//			}
//		}
//		delete[] buffer;
//	}
//
//}

void WindowInfo::update_npc_pos(int npc) {
	// 读取更新坐标
	uintptr_t pos_addr = -1;
	if (npc == 店小二) {
		pos_addr = dianxiaoer_pos_addr;
		dianxiaoer_pos_x = 0;
		dianxiaoer_pos_y = 0;
	}
	else if (npc == 长安驿站老板) {
		pos_addr = changan_yizhanlaoban_pos_addr;
		changan_yizhanlaoban_pos_x = 0;
		changan_yizhanlaoban_pos_y = 0;
	}
	else if (npc == 贼王) {
		pos_addr = zeiwang_pos_addr;
		zeiwang_pos.x = 0;
		zeiwang_pos.y = 0;
	}
	if (pos_addr != -1) {
		SIZE_T regionSize = 0x54;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)pos_addr, buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			auto npc_loc_addr = *reinterpret_cast<uintptr_t*>(buffer);
			if (npc_loc_first_static_addr == npc_loc_addr) {
				auto x = *reinterpret_cast<float*>(buffer + 0x4C);
				auto y = *reinterpret_cast<float*>(buffer + 0x50);
				if (npc == 店小二) {
					if (is_given_pos(x, y, dianxiaoer_pos_list)) {
						dianxiaoer_pos_x = x;
						dianxiaoer_pos_y = y;
						log_info("店小二坐标地址:%f,%f", x, y);
						auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
						auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
						log_info("店小二坐标:%d, %d", dxe_x, dxe_y);
					}
				}
				else if (npc == 长安驿站老板) {
					if (is_given_pos(x, y, changan_yizhanlaoban_pos_list)) {
						changan_yizhanlaoban_pos_x = x;
						changan_yizhanlaoban_pos_y = y;
					}
				}
				else if (npc == 贼王) {
					zeiwang_pos.x = convert_to_map_pos_x(x);
					zeiwang_pos.y = convert_to_map_pos_y(y);
				}
			}
			else {
				// 店小二消失，内存释放后，结构体变了
				npc_found = false;
				if (npc == 店小二) {
					dianxiaoer_pos_addr = 0;
				}
				else if (npc == 长安驿站老板) {
					changan_yizhanlaoban_pos_addr = 0;
				}
				else if (npc == 贼王) {
					zeiwang_pos_addr = 0;
				}
			}
		}
		delete[] buffer;
	}
}

void WindowInfo::move_cursor_center_top() {
	serial_move({ rect.left + 515, rect.top + 95 }, 0);
}

void WindowInfo::move_cursor_center_bottom() {
	serial_move({ rect.left + 515, rect.bottom - 150 }, 0);
}
void WindowInfo::move_cursor_right_top() {
	serial_move({ rect.left + 820, rect.top + 95 }, 0);
}
void WindowInfo::move_cursor_left_bottom() {
	serial_move({ rect.left + 250, rect.bottom - 150 }, 0);
}
void WindowInfo::open_beibao() {
	move_cursor_center_top();
	double threshold = RegionMonthly ? 0.83 : 0.95;
	for (int i = 0; i < 5; i++) {
		input_alt_e();
		if (WaitMatchingRectExist(ROI_beibao(), *m_img_btn_beibao, 1500)) {
			ClickMatchImage(ROI_beibao(), *m_img_btn_package_prop, "", threshold, gMatchMethod, 0, 0, 0, 0, 1, 0);
			break;
		}
	}
}

POINT WindowInfo::open_map() {
	POINT pos = { -1, -1 };
	for (int i = 0; i < 5; i++) {
		input_tab();
		pos = WaitMatchingRectLoc(ROI_map(), *m_img_symbol_map, 2500);
		if (pos.x > 0) {
			break;
		}
	}
	return pos;
}

void WindowInfo::close_map() {
	for (int i = 0; i < 5; i++) {
		input_tab();
		if (WaitMatchingRectDisapper(ROI_map(), *m_img_symbol_map, 2500)) break;
	}
}
void WindowInfo::close_beibao_smart(bool keep) {
	if (!keep && MatchingRectExist(ROI_beibao(), *m_img_btn_beibao)) input_alt_e();
}
POINT WindowInfo::compute_dianxiaoer_pos_lazy() {
	const POINT* vector_arrary = &dianxiaoer_pos_list[0];
	POINT dxe={ dianxiaoer_pos_x, dianxiaoer_pos_y };

	for (int i = 0;i < dianxiaoer_pos_list.size();i++) {
		if (vector_arrary[i].x == dianxiaoer_pos_x && vector_arrary[i].y == dianxiaoer_pos_y) {
			switch (i)
			{
			case 0:
			case 1:
				return {14,14};
			case 2:
				return { 22,14 };
			case 3:
				return { 18,11 };
			case 4:
				return { 26,8 };
			case 5:
				return { 27,9 };
			case 6:
			case 7:
				return { 40,15 };
			case 8:
				return { 34,16 };
			case 9:
				return { 30,24 };
			}
			break;
		}
	}
	return{ -1,-1 };
}
POINT WindowInfo::compute_pos_pixel(POINT dst, unsigned int scene_id,bool fix) {
	// 根据坐标计算相对自己在屏幕上的像素
	POINT px = { 0, 0 };
	//int x_pixel = 0;
	//int y_pixel = 0;
	int center_x = wWidth / 2;  // 中点坐标 1024 / 2 + x_rim
	int center_y = wHeight / 2;  // 中点坐标 768 / 2 + y_rim
	int x_edge = 20;  // 超过这个坐标，人物会在窗口中间
	int y_edge = 15;  // 超过这个坐标，人物会在窗口中间
	int pixel = 25;	 // 25像素一个坐标点
	if (!RegionMonthly) {
		x_edge = 25;  // 超过这个坐标，人物会在窗口中间
		y_edge = 19;  // 超过这个坐标，人物会在窗口中间
		pixel = 20;	 // 20像素一个坐标点
	}
	auto max_loc = get_map_max_loc(scene_id);

	if (player_pos.x <= x_edge) px.x = dst.x * pixel;
	else if (max_loc.x - player_pos.x <= x_edge) px.x = wWidth - (max_loc.x - dst.x) * pixel;
	else px.x = center_x - (player_pos.x - dst.x) * pixel;

	if (player_pos.y <= y_edge) px.y = wHeight - dst.y * pixel;
	else if (max_loc.y - player_pos.y <= y_edge) px.y = (max_loc.y - dst.y) * pixel;
	else px.y = center_y + (player_pos.y - dst.y) * pixel;

	if (fix) {
		// 如果目的像素靠近窗口边缘，鼠标漂移有时候会不显示游戏光标，需要做修正
		int px_fix = 30;
		if (px.x <= px_fix) px.x = px_fix;
		else if (wWidth - px.x <= px_fix) px.x = wWidth - px_fix;
		if (px.y <= px_fix) px.y = px_fix;
		else if (wHeight - px.y <= px_fix) px.y = wHeight - px_fix;
	}
	return { rect.left + px.x, rect.top + px.y };
}

void WindowInfo::move_to_dianxiaoer() {
	//scan_npc_pos_addr(NPC_DIANXIAOER);

	if (!is_near_dianxiaoer() && dianxiaoer_pos_x > 0) {
		//auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		//auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		auto dst = compute_dianxiaoer_pos_lazy();
		// A星寻路
		//auto astar_pos = astar(player_pos.x, player_pos.y, dst.x, dst.y, m_scene_id, dianxiaoer_valid_distence - 1, dianxiaoer_valid_distence - 1);
		//log_info("店小二坐标:%d, %d", dst.x, dst.y);
		//log_info("A星寻路结果:%d, %d", astar_pos.x, astar_pos.y);
		SetForegroundWindow(hwnd);
		if (RegionMonthly) {
			move_to_position_flat(dst);
		}
		else {
			click_position(dst);
		}
		wait_moving_stop(5000);
	}
}
void WindowInfo::goto_changanjiudian() {
	update_player_float_pos();
	if (m_scene_id == 长安城) {
		POINT jiudian = { 468, 171 };	// 长安城到长安酒店入口(464,168)
		if (!(abs(player_pos.x - jiudian.x) <= NPC_TALK_VALID_DISTENCE && abs(player_pos.y - jiudian.y) <= NPC_TALK_VALID_DISTENCE)) {
			fly_to_changanjiudian();
		}
		move_to_other_scene(jiudian, 长安酒店);
		wait_moving_stop(3000);
	}
	else if (m_scene_id == 长安酒店二楼) {
		POINT jiudian = { 34, 31 };	// 长安酒店二楼到长安酒店入口
		if (!(abs(player_pos.x - jiudian.x) <= mScreen_x && abs(player_pos.y - jiudian.y) <= mScreen_y)) {
			fly_to_changanjiudian();
		}
		move_to_other_scene(jiudian, 长安酒店);
		wait_moving_stop(3000);
	}
	else {
		// 不在酒店门口，
		log_info("不在酒店门口，使用飞行棋");
		fly_to_changanjiudian();
	}
	//if (!(abs(player_pos.x - jiudian.x) <= NPC_TALK_VALID_DISTENCE && abs(player_pos.y - jiudian.y) <= NPC_TALK_VALID_DISTENCE)) {
	//	fly_to_changanjiudian();
	//}
	//move_to_other_scene(jiudian, 长安酒店);
	//wait_moving_stop(3000);
	moving = true;
}
void WindowInfo::move_to_changanjidian_center() {
	// 有时候离店小二太远，店小二会消失看不见，移动到酒店中间，就不存在这个问题
	if (dianxiaoer_pos_addr == 0) {
		SetForegroundWindow(hwnd);
		click_position({ 23,13 });
		moving = true;
	}
}
void WindowInfo::from_changan_fly_to_datangguojing() {
	log_info("从长安驿站老板飞到大唐国境");
	if (!npc_found) {
		tScan_npc = 长安驿站老板;
		return;
	}
	//scan_npc_pos_addr(NPC_CHANGAN_YIZHANLAOBAN);

	if (!is_near_changan_yizhanlaoban() && changan_yizhanlaoban_pos_x > 0) {
		hide_player_n_stalls();
		move_to_position({247,43}, 长安驿站老板, 长安驿站老板);  // 这个固定坐标可以和驿站老板对话
		wait_moving_stop(5000);
		update_npc_pos(长安驿站老板);
		update_player_float_pos();
	}
	if (changan_yizhanlaoban_pos_x > 0) {
		auto dst_x = convert_to_map_pos_x(changan_yizhanlaoban_pos_x);
		auto dst_y = convert_to_map_pos_y(changan_yizhanlaoban_pos_y);
		hide_player();
		ship_to_other_scene({dst_x,dst_y}, 大唐国境);
	}
}
void WindowInfo::from_datangguojing_to_datangjingwai() {
	log_info("从大唐国境到大唐境外");
	move_to_other_scene({ 3,77 }, 大唐境外, 30, 0);
}
void WindowInfo::from_changan_to_datangguojing() {
	if (m_scene_id == 长安城) {
		move_to_other_scene({ 6,5 }, 大唐国境, 30, -30);
	}
	else {
		log_info("从长安到大唐国境");
		use_changan777(ROI_changan777_datangguojing(), false);
		move_to_other_scene({ 6,5 }, 大唐国境, 30, -30);
	}
}
void WindowInfo::fly_to_changanjiudian() {
	use_changan777(ROI_changan777_changanjiudian(), true);
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
			if ((x <= 86 && y <= 13) || (x <= 62 && y <= 84) || (x <= 76 && 14 <= y && y <= 18)) use_changshoucun777(ROI_changshoucun777_taibaijinxing(), false, turn, false, false);
			else if ((87 <= x && y <= 32) || (79 <= x && 12 <= y  && y <= 19) || (62 <= x && 20 <= y && y <= 26)) use_changshoucun777(ROI_changshoucun777_changshoujiaowai(), false, turn, false, false);
			else if ((x <= 73 && 85 <= y && y <= 127) || (x <= 82 && 128 <= y && y <= 135)) use_changshoucun777(ROI_changshoucun777_dangpu(), false, turn, false, false);
			else if (117 <= x && 124 <= y && y <= 162) use_changshoucun777(ROI_changshoucun777_lucheng_n_qiangzhuan(), true, turn, false, false);
			else if (x <= 77 && 136 <= y) use_changshoucun777(ROI_changshoucun777_zhongshusheng(), false, turn, false, false);
			else if (78 <= x && 163 <= y) use_changshoucun777(ROI_changshoucun777_fangcunshan(), false, turn, false, false);
			else if ((62 <= x && 33 <= y && y <= 76) || (113 <= x && 77 <= y && y <= 87)) {
				use_feixingfu(scene_id,false);
			}
			else use_changshoucun777(ROI_changshoucun777_lucheng_n_qiangzhuan(), true, turn,false,false);
			break;
		}
		case 傲来国:
		{
			if ((x <= 74 && y <= 38) || (x <= 65 && 39 <= y && y <= 60) || (x <= 93 && y <= 30)) use_aolaiguo777(ROI_aolaiguo777_yaodian(), false, turn, false, false);
			else if ((121 <= x && y <= 34) || (188 <= x && 35 <= y && y <= 37)) use_aolaiguo777(ROI_aolaiguo777_donghaiwan(), true, turn, false, false);
			else if ((143 <= x && 38 <= y && y <= 71) || (170 <= x && 72 <= y && y <= 89)) use_aolaiguo777(ROI_aolaiguo777_dangpu(), true, turn, false, false);
			else if (167 <= x && 94 <= y) use_aolaiguo777(ROI_aolaiguo777_huaguoshan(), true, turn, false, false);
			else if (71 <= x && x <= 166 && 72 <= y) {
				use_feixingfu(scene_id, false);
			}
			else if ((x <= 62 && 61 <= y && y <= 112) || (63 <= x && x <= 70 && 75 <= y && y <= 112)) use_aolaiguo777(ROI_aolaiguo777_penglaixiandao(), false, turn, false, false);
			else if (x <= 70 && 113 <= y) use_aolaiguo777(ROI_aolaiguo777_nvercun(), false, turn, false, false);
			else use_aolaiguo777(ROI_aolaiguo777_qianzhuang(), true, turn, false, false);
			break;
		}
		case 朱紫国:
		{
			if (x <= 43  && y <= 22) use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false, turn, false, false);
			else if (44 <= x && x <= 111 && y <= 24) use_zhuziguo777(ROI_zhuziguo777_duanmuniangzi(), true, turn, false, false);
			else if ((112 <= x && y <= 26) || (90 <= x && x <= 111 && 17 <= y && y <= 32)) use_zhuziguo777(ROI_zhuziguo777_sichouzhilu(), true, turn, false, false);
			else if ((112 <= x && 27 <= y && y <= 65) || (104 <= x && x <= 111 && 33 <= y && y <= 65)) use_zhuziguo777(ROI_zhuziguo777_yaodian(), true, turn, false, false);
			else if ((107 <= x && 66 <= y && y <= 107) || (114 <= x && 108 <= y)) {
				use_feixingfu(scene_id, false);
			}
			else if ((39 <= x && x <= 106 && 73 <= y) || (107 <= x && x <= 113 && 107 <= y)) use_zhuziguo777(ROI_zhuziguo777_shenjidaozhang(), false, turn, false, false);
			else if (x <= 38 && 71 <= y) use_zhuziguo777(ROI_zhuziguo777_qilinshan(), false, turn, false, false);
			else use_zhuziguo777(ROI_zhuziguo777_jiudian(), false, turn, false, false);
			break;
		}
		case 西梁女国:
		{
			// 西凉女国：1.飞行符 2.合成旗-朱紫国驿站-西凉女国(这条路复杂而且慢)
			use_feixingfu(scene_id, false);
			break;
		}
		case 宝象国:
		{
			use_feixingfu(scene_id, false);
			break;
		}
		case 建邺城:
		{
			// 建邺城：1.飞行符 2.合成旗-傲来东海湾驿站-东海湾-建邺城
			use_feixingfu(scene_id, false);
			break;
		}
		case 大唐境外:
		{
			// 大唐境外：1.合成旗-朱紫国左下角-大唐境外 2.合成旗-罗道人旁的驿站老板-传送大唐国境-大唐境外
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
				if (m_scene_id == 朱紫国) {
					move_to_other_scene({ 2,4 }, 大唐境外, 30, -30);
				}
				else {
					use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false);
					move_to_other_scene({ 2,4 }, 大唐境外, 30, -30);
				}
			}
			else {
				if (m_scene_id == 大唐国境) {
					from_datangguojing_to_datangjingwai();
				}
				else if (m_scene_id != 长安城) {
					log_info("大唐国境到大唐境外");
					use_changan777(ROI_changan777_yizhan_laoban(), false);
					from_changan_fly_to_datangguojing();
				}
				else{
					from_changan_fly_to_datangguojing();
				}
			}
			break;
		}
		case 江南野外:
		{
			// 江南野外:1.合成旗-长安右下角-江南野外 2.飞行符-建邺城-江南野外
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 542, 6 }, 江南野外, -30, -30);
			}
			else {
				log_info("从长安到江南野外");
				use_changan777(ROI_changan777_jiangnanyewai(), false);
				move_to_other_scene({ 542, 6 }, 江南野外, -30, -30);
			}
			break;
		}
		case 女儿村:
		{
			// 女儿村：合成旗-傲来国左上角-女儿村
			if (m_scene_id == 傲来国) {
				move_to_other_scene({ 5, 139 }, 女儿村, 30, 30);
			}
			else {
				log_info("从傲来国到女儿村");
				use_aolaiguo777(ROI_aolaiguo777_nvercun(), false);
				move_to_other_scene({ 5, 139 }, 女儿村, 30, 30);
			}
			break;
		}
		case 普陀山:
		{
			// 普陀山：合成旗-长安左下角-大唐国境-普陀接引仙女
			if (m_scene_id == 大唐国境) {
				POINT dst = { 221,60 };
				if (!is_near_loc(dst, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE)) {
					log_info("从大唐国境到普陀接引仙女");
					move_to_position(dst, MAP_MOVE_DISTENCE, MAP_MOVE_DISTENCE);
					close_beibao_smart();
					moving = true;
				}
				else if (player_pos.x == dst.x && player_pos.y == dst.y) {
					click_position({ dst.x + 2,dst.y });
				}
				else {
					log_info("传送普陀山");
					hide_player();
					ship_to_other_scene(dst, 普陀山);
				}
			}
			else {
				from_changan_to_datangguojing();
			}
			break;
		}
		case 五庄观:
		{
			// 五庄观:合成旗-长安城驿站老板-大唐国境-大唐境外-五庄观
			if (m_scene_id == 大唐国境) {
				from_datangguojing_to_datangjingwai();
				move_to_other_scene({ 636,76 }, 五庄观, -30, 0, true);
			}
			else if (m_scene_id == 大唐境外) {
				log_info("从大唐境外到五庄观");
				move_to_other_scene({ 636,76 }, 五庄观,-30,0,true);
			}
			else if (m_scene_id != 长安城) {
				use_changan777(ROI_changan777_yizhan_laoban(), false);
				from_changan_fly_to_datangguojing();
			}
			else{
				from_changan_fly_to_datangguojing();
			}
			break;
		}
		case 化生寺:
		{
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 511, 274 }, 化生寺, 0, 30);
			}
			else {
				log_info("从长安到化生寺");
				use_changan777(ROI_changan777_huashengsi(), false);
				move_to_other_scene({ 511, 274 }, 化生寺, 0, 30);
			}
			break;
		}
		case 长安杂货店:
		{
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 534,136 }, 长安杂货店);
			}
			else {
				use_changan777(ROI_changan777_changanjiudian());
				move_to_other_scene({ 534,136 }, 长安杂货店);
			}
			break;
		}
		case 长安饰品店:
		{
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 387,15 }, 长安饰品店);
			}
			else {
				use_changan777(ROI_changan777_dangpu());
				move_to_other_scene({ 387,15 }, 长安饰品店);
			}
			break;
		}
		case 长安国子监:
		{
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 275,204 }, 长安国子监);
			}
			else {
				use_feixingfu(长安城);
				move_to_other_scene({ 275,204 }, 长安国子监);
			}
			break;
		}
		case 长安酒店二楼:
		{
			if (m_scene_id == 长安城) {
				move_to_other_scene({ 470, 170 }, 长安城);
			}
			else if (m_scene_id == 长安酒店) {
				move_to_other_scene({ 36,34 }, 长安酒店二楼);
			}
			else {
				POINT jiudian = { 468, 171 };	// 长安酒店入口(464,168)
				fly_to_changanjiudian();
				move_to_other_scene(jiudian, 长安城);
			}
			break;
		}
		case 长寿村当铺:
		{
			if (m_scene_id == 长寿村) {
				move_to_other_scene({ 15,128 }, 长寿村当铺);
			}
			else {
				use_changshoucun777(ROI_changshoucun777_dangpu());
				move_to_other_scene({ 15,128 }, 长寿村当铺);
			}
			break;
		}
		case 长寿郊外:
		{
			if (m_scene_id != 长寿村) {
				use_changshoucun777(ROI_changshoucun777_changshoujiaowai(), false);
				move_to_other_scene({ 147, 6 }, 长寿郊外, 0, 30);
			}
			else {
				move_to_other_scene({ 147, 6 }, 长寿郊外, 0, 30);
			}
			break;
		}
		case 长寿村酒店:
		{
			if (m_scene_id == 长寿村) {
				move_to_other_scene({ 109,148 }, 长寿村酒店);
			}
			else {
				use_changshoucun777(ROI_changshoucun777_dangpu());
				move_to_other_scene({ 109,148 }, 长寿村酒店);
			}
			break;
		}
		case 傲来客栈:
		{
			if (m_scene_id == 傲来国) {
				move_to_other_scene({ 180,30 }, 傲来客栈);
			}
			else {
				use_aolaiguo777(ROI_aolaiguo777_donghaiwan());
				move_to_other_scene({ 180,30 }, 傲来客栈);
			}
			break;
		}
		case 傲来客栈二楼:
		{
			if (m_scene_id == 傲来国) {
				move_to_other_scene({ 180,30 }, 傲来客栈);
			}
			else if (m_scene_id == 傲来客栈) {
				move_to_other_scene({ 32,34 }, 傲来客栈二楼);
			}
			else {
				use_aolaiguo777(ROI_aolaiguo777_donghaiwan());
				move_to_other_scene({ 180,30 }, 傲来客栈);
			}
			break;
		}
		case 傲来国药店:
		{
			if (m_scene_id == 傲来国) {
				move_to_other_scene({ 53,40 }, 傲来国药店);
			}
			else {
				use_aolaiguo777(ROI_aolaiguo777_yaodian());
				move_to_other_scene({ 53,40 }, 傲来国药店);
			}
			break;
		}
		case 大唐国境:
		{
			from_changan_to_datangguojing();
			break;
		}
		case 地府:
		{
			log_info("寻路到地府");
			if (m_scene_id == 大唐国境) {
				move_to_other_scene({ 48,330 }, 地府, 0, 30);
			}
			else if (m_scene_id == 长安城) {
				from_changan_fly_to_datangguojing();
			}
			else {
				fly_to_changan_yizhan_laoban();
				tScan_npc = 长安驿站老板;
			}
			break;
		}
		case 狮驼岭:
		{
			if (m_scene_id == 大唐境外) {
				move_to_other_scene({ 5, 48 }, 狮驼岭, 0, -60);
			}
			else if (m_scene_id != 朱紫国) {
				log_info("从朱紫国到狮驼岭");
				use_zhuziguo777(ROI_zhuziguo777_datangjingwai(), false);
				move_to_other_scene({ 3, 4 }, 大唐境外, 30, -30);
			}
			else {
				move_to_other_scene({ 3, 4 }, 大唐境外, 30, -30);
			}
			break;
		}
		case 建邺衙门:
		{
			if (m_scene_id == 建邺城) {
				move_to_other_scene({ 146,83 }, 建邺衙门);
			}
			else {
				use_feixingfu(建邺城);
			}
			break;
		}
		case 建邺杂货店:
		{
			if (m_scene_id == 建邺城) {
				move_to_other_scene({ 115,130 }, 建邺杂货店);
			}
			else {
				use_feixingfu(建邺城);
				move_to_other_scene({ 115,130 }, 建邺杂货店);
			}
			break;
		}
		case 东海湾:
		{
			if (m_scene_id == 傲来国) {
				ship_to_other_scene({ 168, 15 }, 东海湾);
			}
			else {
				log_info("从傲来国到东海湾");
				use_aolaiguo777(ROI_aolaiguo777_donghaiwan(), false);
				ship_to_other_scene({ 168, 15 }, 东海湾);
			}
			break;
		}
		case 花果山:
		{
			if (m_scene_id != 傲来国) {
				use_aolaiguo777(ROI_aolaiguo777_huaguoshan(), false);
				move_to_other_scene({ 216, 147 }, 花果山, -30, 30);
			}
			else {
				move_to_other_scene({ 216, 147 }, 花果山, -30, 30);
			}
			break;
		}
	}
}
bool WindowInfo::goto_scene(POINT dst, unsigned int scene_id) {
	update_scene_id();
	update_player_float_pos();
	if (m_scene_id == scene_id) {
		if (dst.x == 0 && dst.y == 0)return true;  // 坐标为0，代表进入场景就可以了。
		// 已处在目的场景，走路过去
		//if (is_near_loc(dst, MAP_MOVE_DISTENCE, MAP_MOVE_DISTENCE))	return true;
		move_to_position(dst, MAP_MOVE_DISTENCE, MAP_MOVE_DISTENCE);
		close_beibao_smart();
		return true;
	}
	// 跨地图，需要用飞行棋或走路
	fly_to_scene(dst.x, dst.y, scene_id);
	return false;
}
void WindowInfo::use_beibao_prop(const cv::Mat& image, bool turn, bool keep) {
	if (turn) open_beibao(); // 这里的动作是：用完道具后是否关闭背包。打开背包使用飞行旗的时候，背包自动关闭了，不需要再关闭背包
	ClickMatchImage(ROI_beibao_props(), image, "", gThreshold, gMatchMethod, 0, 0, 0, 0, 2, 2500);
	if (!keep) {
		//Sleep(150);
		input_alt_e();
	}
}

void WindowInfo::use_changan777(cv::Rect roi, bool move, bool turn, bool keep, bool wait_scene) {
	log_info("使用长安合成旗");
	use_beibao_prop(*m_img_props_red_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, *m_img_btn_flag_loc, 3000, "", 0.85);
	mouse_click_human(flag_loc);
	if (wait_scene)wait_scene_change(长安城);
}

void WindowInfo::use_zhuziguo777(cv::Rect roi, bool move, bool turn, bool keep, bool wait_scene) {
	log_info("使用朱紫国合成旗");
	use_beibao_prop(img_props_white_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, *m_img_btn_flag_loc, 3000, "", 0.85);
	mouse_click_human(flag_loc);
	if (wait_scene)wait_scene_change(朱紫国);
}

void WindowInfo::use_changshoucun777(cv::Rect roi, bool move, bool turn, bool keep, bool wait_scene) {
	log_info("使用长寿村合成旗");
	use_beibao_prop(img_props_green_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, *m_img_btn_flag_loc, 3000, "", 0.85);
	mouse_click_human(flag_loc);
	if (wait_scene)wait_scene_change(长寿村);
}

void WindowInfo::use_aolaiguo777(cv::Rect roi, bool move, bool turn, bool keep, bool wait_scene) {
	log_info("使用傲来国合成旗");
	use_beibao_prop(img_props_yellow_777, turn, keep);
	if (move) move_cursor_center_bottom();
	auto flag_loc = WaitMatchingRectLoc(roi, *m_img_btn_flag_loc, 3000, "", 0.85);
	if (flag_loc.x < 0) {
		if (roi == ROI_aolaiguo777_qianzhuang()) flag_loc = WaitMatchingRectLoc(ROI_aolaiguo777_yaodian(), *m_img_btn_flag_loc, 0, "", 0.85);
	}
	mouse_click_human(flag_loc);
	if(wait_scene) wait_scene_change(傲来国);
}

void WindowInfo::use_feixingfu(unsigned int scene_id, bool wait_scene) {
	log_info("使用飞行符");
	move_cursor_center_bottom();
	input_f1();
	cv::Rect roi;
	cv::Mat flag_image;
	switch (scene_id)
	{
	case 建邺城:
	{
		flag_image = *m_img_symbol_feixingfu_jianyecheng;
		roi = ROI_feixingfu_jianyecheng();
		break;
	}
	case 西梁女国:
	{
		flag_image = *m_img_symbol_feixingfu_xiliangnvguo;
		roi = ROI_feixingfu_xiliangnvguo();
		break;
	}
	case 宝象国:
	{
		flag_image = *m_img_symbol_feixingfu_baoxiangguo;
		roi = ROI_feixingfu_baoxiangguo();
		break;
	}
	case 长寿村:
	{
		flag_image = *m_img_symbol_feixingfu_changshoucun;
		roi = ROI_feixingfu_changshoucun();
		break;
	}
	case 傲来国:
	{
		flag_image = *m_img_symbol_feixingfu_aolaiguo;
		roi = ROI_feixingfu_aolaiguo();
		break;
	}
	case 朱紫国:
	{
		flag_image = *m_img_symbol_feixingfu_zhuziguo;
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
	if (wait_scene)wait_scene_change(scene_id);
}
void WindowInfo::handle_sheyaoxiang_time() {
	bool is_check = false;
	const auto* vector_arrary = &monster_scene_list[0];
	for (int i = 0; i < monster_scene_list.size(); i++) {
		if (vector_arrary[i] == m_scene_id)
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
		use_beibao_prop(*m_img_props_sheyaoxiang);
		gm.db[player_id][utf_8_sheyaoxiang] = now_time;
		gm.update_db();
	}
}
void WindowInfo::handle_wrong_attack() {
	move_cursor_center_bottom();
	ClickMatchImage(ROI_npc_talk(), *m_img_btn_npc_talk_close2);
}
bool WindowInfo::wait_scene_change(unsigned int scene_id, int timeout) {
	auto t_ms = getCurrentTimeMilliseconds();
	bool ret = false;
	while (true) {
		update_scene_id();
		if (m_scene_id == scene_id) {
			ret = true;
			break;
		}
		if (timeout == 0) break;
		else if (getCurrentTimeMilliseconds() - t_ms > timeout) {
			log_info("wait_scene_change超时");
			break;
		}
		Sleep(5);
	}
	update_player_float_pos();
	return ret;
}

void WindowInfo::close_npc_talk() {
	serial_move({ rect.left + 670, rect.top + 475 });  // npc对话框很大，不用怕鼠标漂移点不准
}
bool WindowInfo::close_npc_talk_fast() {
	auto pixel = MatchingRectLoc(ROI_npc_talk(), *m_img_btn_npc_talk_close);
	if (pixel.x < 0) return false;
	close_npc_talk();
	return true;
}
bool WindowInfo::mouse_click_human(POINT pixel, int xs, int ys, int mode) {
	// mode:0不点击，1左键，2右键，5ctrl+左键, 6alt+a攻击
	if (pixel.x < 0 && pixel.y < 0)return false;
	POINT target_pos = pixel;
	POINT mouse_move_pos = { pixel.x + xs, pixel.y + ys };
	POINT cursor_pos;
	auto t_ms = getCurrentTimeMilliseconds();
	do {
		// 因为有鼠标漂移，所以需要多次移动
		if (getCurrentTimeMilliseconds() - t_ms > 1600) {
			log_warn("鼠标点击超时");
			return false;
		}
		serial_move(mouse_move_pos, 0);
		cursor_pos = get_cursor_pos(mouse_move_pos);
		//if (cursor_pos.x < 0) continue;
		if (out_of_rect(cursor_pos)) continue;
		mouse_move_pos = { mouse_move_pos.x + target_pos.x - cursor_pos.x, mouse_move_pos.y + target_pos.y - cursor_pos.y };
		t_ms = getCurrentTimeMilliseconds();
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
	case 5:
	{
		serial_ctrl_click_cur();
		break;
	}
	case 6: 
	{
		input_alt_a();
		serial_click_cur();
	}
	default:
		break;
	}
	return true;
}

POINT WindowInfo::get_cursor_pos(POINT pos) {
	// 获取鼠标漂移量
	POINT tmp_pos = { -99, -99 };
	auto t_ms = getCurrentTimeMilliseconds();
	auto rc = ROI_NULL();
	while (true) {
		// 循环等待鼠标移动停止
		if (getCurrentTimeMilliseconds() - t_ms > 900) return {-1,-1};
		//Sleep(5);
		auto image = hwnd2mat(hwnd);
		cv::Rect roi_rect = ROI_cursor(pos) & cv::Rect(0, 0, image.cols, image.rows);
		auto image_roi = image(roi_rect);
		if (image_roi.empty()) continue;
		cv::Mat image_inRange = ThresholdinginRange(image_roi);
		auto cursor_pos = MatchingLoc(image_inRange, rc, *m_img_cursors_cursor, "", 0.854, cv::TM_CCORR_NORMED, MATCHLEFTTOP);  // 游戏自身的鼠标，鼠标用cv::TM_CCORR_NORMED方法匹配准确率最高
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

bool WindowInfo::ClickMatchImage(cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method, int x_fix, int y_fix, int xs, int ys, int mode, int timeout) {
	auto cv_pos = WaitMatchingRectLoc(roi_rect, templ, timeout, mask_path, threshold, match_method);
	if (cv_pos.x < 0) return false;
	return mouse_click_human({cv_pos.x + x_fix, cv_pos.y + y_fix }, xs, ys, mode);
}

bool WindowInfo::talk_to_dianxiaoer() {
	// 对话店小二
	auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
	auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
	log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
	//log_info("店小二坐标:%f,%f", dianxiaoer_pos_x, dianxiaoer_pos_y);
	log_info("店小二坐标:%d,%d", dxe_x, dxe_y);
	hide_player();
	click_position({ dxe_x, dxe_y });  // 点击与店小二对话
	auto pos = WaitMatchingRectLoc(ROI_npc_talk(), *m_img_btn_tingtingwufang);
	if (pos.x > 0) {
		mouse_click_human(pos, 0, 0, 1);			// 弹出对话框，接任务
		return true;
	}
	return false;
}
bool WindowInfo::is_given_pos(float x, float y, const std::vector<POINT>& pos_list) {
	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
	const POINT* vector_arrary = &pos_list[0];
	for (int i = 0;i < pos_list.size();i++) {
		if (x > 0 && y > 0 && vector_arrary[i].x == x && vector_arrary[i].y == y) return true;
	}
	//for (auto& pos : dianxiaoer_pos_list) {
	//	if (x > 0 && y > 0 && pos.x == x && pos.y == y) return true;
	//}
	return false;
}
//bool WindowInfo::is_dianxiaoer_pos(float x, float y) {
//	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
//	POINT* vector_arrary = &dianxiaoer_pos_list[0];
//	for (int i = 0;i < dianxiaoer_pos_list.size();i++) {
//		if (x > 0 && y > 0 && vector_arrary[i].x == x && vector_arrary[i].y == y) return true;
//	}
//	//for (auto& pos : dianxiaoer_pos_list) {
//	//	if (x > 0 && y > 0 && pos.x == x && pos.y == y) return true;
//	//}
//	return false;
//}
//bool WindowInfo::is_changan_yizhanlaoban_pos(float x, float y) {
//	// 店小二是按照顺时针固定几个坐标的规律移动的,根据坐标判断是否是店小二
//	POINT* vector_arrary = &dianxiaoer_pos_list[0];
//	for (auto& pos : changan_yizhanlaoban_pos_list) {
//		if (x > 0 && y > 0 && pos.x == x && pos.y == y) return true;
//	}
//	return false;
//}
bool WindowInfo::is_moving() {
	// 判断自己是否在移动
	float x0 = 0;
	float y0 = 0;
	for (int i = 0; i < 2; i++) {
		update_player_float_pos();
		if (player_x == 0 && player_y == 0) return true;
		if ((int)player_x > 0 && (int)player_y > 0) {
			if (x0 == player_x && y0 == player_y) {
				moving = false;
				return false;
			}
			x0 = player_x;
			y0 = player_y;
		}
		Sleep(80);
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
	update_npc_pos(店小二);
	update_player_float_pos();
	if (dianxiaoer_pos_x > 0) {
		auto dxe_x = convert_to_map_pos_x(dianxiaoer_pos_x);
		auto dxe_y = convert_to_map_pos_y(dianxiaoer_pos_y);
		if (abs(dxe_x - player_pos.x) <= (dianxiaoer_valid_distence + 2) && abs(dxe_y - player_pos.y) <= (dianxiaoer_valid_distence + 2)) {
			//  +1是防止鼠标漂移误差
			return true;
		}
	}
	return false;
}
bool WindowInfo::is_near_changan_yizhanlaoban() {
	update_npc_pos(长安驿站老板);
	update_player_float_pos();
	if (changan_yizhanlaoban_pos_addr > 0) {
		auto dxe_x = convert_to_map_pos_x(changan_yizhanlaoban_pos_x);
		auto dxe_y = convert_to_map_pos_y(changan_yizhanlaoban_pos_y);
		if (abs(dxe_x - player_pos.x) <= NPC_TALK_VALID_DISTENCE && abs(dxe_y - player_pos.y) <= NPC_TALK_VALID_DISTENCE) {
			//  +1是防止鼠标漂移误差
			return true;
		}
	}
	return false;
}
bool WindowInfo::is_near_loc(POINT dst, int near_x, int near_y) {
	if (abs(dst.x - player_pos.x) <= near_x && abs(dst.y - player_pos.y) <= near_y) {
		return true;
	}
	return false;
}

bool WindowInfo::is_npc_visible(uintptr_t npc_loc_addr) {
	// NPC消失，内存释放后，结构体变了
	bool res = false;
	SIZE_T regionSize = 0x8;
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	pNtReadVirtualMemory(hProcess, (PVOID)npc_loc_addr, buffer, regionSize, &bytesRead);
	if (bytesRead > 0) {
		auto npc_loc_addr = *reinterpret_cast<uintptr_t*>(buffer);
		res = npc_loc_first_static_addr == npc_loc_addr;
	}
	delete[] buffer;
	return res;
}

bool WindowInfo::wait_fighting() {
	//return WaitMatchingRectExist(ROI_fighting(), img_fight_fighting, 3500,"",0.85);
	auto t_ms = getCurrentTimeMilliseconds();
	while (true) {
		update_player_float_pos();
		if (player_x == 0 && player_y == 0) return true;
		else if (getCurrentTimeMilliseconds() - t_ms > 1000) {
			return false;
		}
		//Sleep(5);
	}
	return false;
}
bool WindowInfo::is_fighting() {
	//战斗的时候玩家坐标变成0,0
	update_player_float_pos();
	return player_x == 0 && player_y == 0;
	//return MatchingRectExist(ROI_fighting(), img_fight_fighting, "", 0.85);
}
bool WindowInfo::is_verifying() {
	auto image = hwnd2mat(hwnd);
	cv::Mat img_gray;
	cv::cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);
	if (MatchingExist(image, ROI_paixu_verify(), *m_img_symbol_paixu_verify_reset, "", 0.81))return true;
	if (MatchingLoc(img_gray, ROI_yidongdezi(), *m_img_symbol_yidongdezi_gray, "", 0.81).x > -1)return true;
	if (MatchingExist(img_gray, ROI_four_man(), *m_img_fight_fourman_title_gray, "", 0.83))return true;
	if (MatchingExist(img_gray, ROI_four_man(), *m_img_fight_fourman_title2_gray, "", 0.83))return true;
	return false;
}
bool WindowInfo::is_four_man() {
	auto image = hwnd2mat(hwnd);
	cv::Mat img_gray;
	cv::cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);
	if (MatchingExist(img_gray, ROI_four_man(), *m_img_fight_fourman_title_gray, "", 0.83) ||
		MatchingExist(img_gray, ROI_four_man(), *m_img_fight_fourman_title2_gray, "", 0.83)) {
		play_mp3_once();
		popup_verify = true;
		for (int i = 0; i < 5; i++) { log_info("***四小人弹窗验证,请手动点击***"); }
		return true;
	}
	return false;
}
bool WindowInfo::is_hangup(cv::Mat& image) {
	// 是否已挂自动战斗
	bool hangup = false;
	if (MatchingExist(image, ROI_NULL(), *m_img_btn_cancel_auto_round))hangup = true;
	else if (MatchingExist(image, ROI_NULL(), *m_img_btn_cancel_zhanli))hangup = true;
	return hangup;
}
bool WindowInfo::is_in_mini_scene() {
	//小场景没有小地图，只能通过点击屏幕移动
	const auto* vector_arrary = &mini_scene_list[0];
	for (int i = 0;i < mini_scene_list.size();i++) {
		if (vector_arrary[i] == m_scene_id) return true;
	}
	return false;
}
void WindowInfo::handle_datu_fight() {
	// 自动战斗挂机处理
	auto image = hwnd2mat(hwnd);
	auto hangup = is_hangup(image);
	if (hangup) {
		//检查重置自动战斗挂机剩余回合
		if (!RegionMonthly) {
			if (MatchingExist(image, ROI_fight_action(), *m_img_fight_do_hero_action)) {
				if (getCurrentTimeMilliseconds() - wait_hero_action_time > 2.0) {
					log_info("战斗回合+1");
					wait_hero_action_time = getCurrentTimeMilliseconds();
					f_round += 1;
					auto round = gm.db[player_id]["round"];
					gm.db[player_id]["round"] = round + 1;
					gm.update_db();
				}
			}
			if (gm.db[player_id]["round"] >= randint(19, 24) && f_round == 0) {
				log_info("重置自动战斗挂机剩余回合");
				//SetForegroundWindow(hwnd);
				move_cursor_center_top();
				auto btn_pos = WaitMatchingRectLoc(ROI_NULL(), *m_img_btn_reset_auto_round, 500, "", 0.95);
				if (btn_pos.x > 0) {
					for (int i = 0; i < 3; i++) {
						if (mouse_click_human(btn_pos, 0, 30))break;
					}
					if (WaitMatchingRectExist(ROI_NULL(), *m_img_fight_auto_round30, 300, "", 0.95)) {
						gm.db[player_id]["round"] = 0;
						gm.update_db();
						log_info("重置完毕.");
					}
				}
			}
		}
	}
	else {
		// 四小人验证处理
		if (is_four_man()) {
			return;
		}
		if (MatchingExist(image, ROI_fight_action(), *m_img_fight_do_hero_action)) {
			wait_hero_action_time = getCurrentTimeMilliseconds();
			f_round += 1;
			auto round = gm.db[player_id]["round"];
			gm.db[player_id]["round"] = round+1;
			gm.update_db();
			ClickMatchImage(ROI_fight_action(), *m_img_fight_auto,"",gThreshold,gMatchMethod,0,0,-60,0);
		}
		else if (MatchingExist(image, ROI_fight_action(), *m_img_fight_do_peg_action)) {
			log_info("宠物平A");
			input_alt_a();
		}
	}
}
int WindowInfo::convert_to_map_pos_x(float x) {
	// (x - 1) * 20 + 30 = player_x  其中x是地图上显示的坐标
	if (x == 0) return x;
	return (x - 30) / 20 + 1;
}

int WindowInfo::convert_to_map_pos_y(float y) {
	// (max_y - y - 1) * 20 + 30 = player_y  其中y是地图上显示的坐标,max_y是地图y的最大值。例如建邺城y最大值是143
	if (y == 0) return y;
	update_scene_id();
	auto max = get_map_max_loc(m_scene_id);
	return max.y - 1 - (y - 30) / 20;
}

void WindowInfo::parse_baotu_task_info() {
	log_info("开始解析宝图任务内容:0x%p", hProcess);
	//◆正在#K狮驼岭#B拦路抢劫的#K<EvalFunc>{"str":"强盗凌豹羽","evalID":2755,}</EvalFunc>#B喜欢把他的宝贝藏在#R56，76#B附近，先到先得啊。(今天已领取#R1#B次)#r
	// 宝图内容格式：◆xxxx(今天已领取xxx次)#r
	// ◆:C6 25
	// "08 FF CA 4E 29 59 F2 5D 86 98 D6 53 23 00 52 00 ? ? 23 00 6E 00 2F 00 35 00 30 00 21 6B 09 FF",  // （今天已领取#R8#n/50次）
	auto baotu_task_symbol_list = PerformAoBScanEx(
		hProcess,
		NULL,
		"23 00 6E 00 2F 00 35 00 30 00 21 6B 09 FF"  // （今天已领取#R8#n/50次）
	);
	int temp_count = 0;
	SIZE_T regionSize = 0x260; // 宝图任务的内容长度，这个长度应该够用了
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead;
	SIZE_T symbol_len = 14;
	int pre_len = 22;
	// 先根据和店小二的对话，找到今日领取次数
	for (size_t i = 0; i < baotu_task_symbol_list.size(); i++) {
		pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol_list[i] - pre_len), buffer, symbol_len + pre_len, &bytesRead);
		if (bytesRead > 0) {
			auto today_num = bytes_to_wstring(buffer, bytesRead);
			auto tag_number = findContentBetweenTags(today_num, L"#R", L"#n");
			memset(buffer, bytesRead, 0);
			bytesRead = 0;
			int num = 0;
			try {
				num = std::stoi(tag_number.at(0));
			}
			catch (std::invalid_argument& e) {
				continue;
			}
			catch (std::out_of_range& e) {
				continue;
			}
			if (baotu_task_count > 0) {
				if (num <= baotu_task_count) { continue; }  // 领取次数小于等于上一次的记录，说明是以前的内存没有被释放，忽略掉
				else {
					temp_count = num;
					break;
				}
			}
			else {
				if (num > temp_count) {
					temp_count = num;
				}
			}
		}
	}
	if (temp_count > 0) {
		baotu_task_count = temp_count;
		log_info("今天已领取:%d次", baotu_task_count);
		std::vector<uintptr_t>baotu_task_content_list;
		//找到强盗名字
		for (int m = 0; m < baotu_task_symbol_list.size(); m++) {
			memset(buffer, bytesRead, 0);
			bytesRead = 0;
			pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol_list[m] - regionSize + symbol_len), buffer, regionSize, &bytesRead);
			//pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol_list[m] - pre_len), buffer, symbol_len + pre_len, &bytesRead);
			std::wstring content;
			if (bytesRead > 0) {
				//auto sss22 = bytes_to_wstring(buffer, bytesRead);
				auto today_num = bytes_to_wstring(&buffer[regionSize - symbol_len - pre_len], symbol_len + pre_len);
				auto tag_number = findContentBetweenTags(today_num, L"#R", L"#n");
				try {
					auto num = std::stoi(tag_number.at(0));
					if (num != baotu_task_count)continue;
				}
				catch (std::invalid_argument& e) {
					continue;
				}
				catch (std::out_of_range& e) {
					continue;
				}
				//auto sss = bytes_to_wstring(buffer, bytesRead);
				for (int i = 0; i < bytesRead - 6; i++) {
					if (buffer[i] == 0x00 && buffer[i + 1] == 0x00 && buffer[i + 2] == 0x23 && buffer[i + 3] == 0x00 && buffer[i + 4] == 0x57 && buffer[i + 5] == 0x00) {	// #W:23 00 57 00
						content = bytes_to_wstring(&buffer[i + 2], bytesRead - i + 2);
						break;
					}
				}
			}
			if (!content.empty()) {
				auto tags_wstr = findContentBetweenTags(content, L",\"estr\":\"", L"\",\"evalID\"");
				if (!tags_wstr.empty()) {
					std::string struct_AoB = "7B 00 22 00 73 00 74 00 72 00 22 00 3A 00 22 00 ";
					std::wstring name = tags_wstr[0].c_str();
					setlocale(LC_ALL, ""); // 设置本地化
					log_info("%ls", name.c_str());
					auto ptr1 = reinterpret_cast<char*>(&name);
					for (int j = 0; j < name.size() * 2; j++) {
						auto c = *reinterpret_cast<const unsigned char*>(ptr1 + j);
						char hexStr[3];
						sprintf(hexStr, "%2X ", c);
						struct_AoB += hexStr;
					}
					struct_AoB += "22 00 2C 00 22 00 65 00 76 00 61 00 6C 00 49 00 44 00 22 00 3A 00";
					baotu_task_content_list = PerformAoBScanEx(
						hProcess,
						NULL,
						struct_AoB.c_str()
					);
					break;
				}
			}
		}
		//根据强盗名字找到强盗坐标和场景
		for (size_t j = 0; j < baotu_task_content_list.size(); ++j) {
			int end_len = 0xA0;
			std::wstring qiangdao_content;
			memset(buffer, bytesRead, 0);
			bytesRead = 0;
			pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_content_list[j] - regionSize + end_len * 2), buffer, regionSize, &bytesRead);
			if (bytesRead > 0) {
				for (int k = 0; k < bytesRead - 4; k++) {
					if (buffer[k] == 0x00 && buffer[k + 1] == 0x00 && buffer[k + 2] == 0xC6 && buffer[k + 3] == 0x25) {	// ◆:C6 25
						qiangdao_content = bytes_to_wstring(&buffer[k + 2], bytesRead - k + 2 + end_len * 2);
						break;
					}
				}
			}
			if (!qiangdao_content.empty()) {
				std::vector<std::wstring> all_tags_wstr;
				auto tag_wstr1 = findContentBetweenTags(qiangdao_content, L"#K", L"#B");
				auto tag_wstr2 = findContentBetweenTags(qiangdao_content, L"#R", L"#B");
				all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr1.begin(), tag_wstr1.end());
				all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr2.begin(), tag_wstr2.end());
				for (auto& wstr : all_tags_wstr)
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
				if (baotu_target_scene_id > 0 && baotu_target_pos.x > 0) {
					log_info("解析宝图任务成功:%d,(%d,%d)", baotu_target_scene_id, baotu_target_pos.x, baotu_target_pos.y);
					break;
				}
			}
		}
		if (baotu_target_scene_id <= 0) log_info("未支持的场景，等待添加");
	}
	log_info("结束解析宝图任务内容:0x%p", hProcess);
	delete[]buffer;
}
void WindowInfo::parse_baotu_task_info_card() {
	log_info("开始解析宝图任务内容:0x%p", hProcess);
	//◆正在#K狮驼岭#B拦路抢劫的#K<EvalFunc>{"str":"强盗凌豹羽","evalID":2755,}</EvalFunc>#B喜欢把他的宝贝藏在#R56，76#B附近，先到先得啊。(今天已领取#R1#B次)#r
	// 宝图内容格式：◆xxxx(今天已领取xxx次)#r
	// ◆:C6 25
	// "28 00 CA 4E 29 59 F2 5D 86 98 D6 53 23 00 52 00 ? ? 23 00 42 00 21 6B 29 00 23 00 72 00 00 00",  // (今天已领取#R1#B次)#r
	auto baotu_task_symbol_list = PerformAoBScanEx(
		hProcess,
		NULL,
		"? ? 23 00 42 00 21 6B 29 00 23 00 72 00 00 00"  // (今天已领取#R1#B次)#r
	);
	int symbol_len = 16;
	int pre_symbol_len = 20;
	int temp_count = 0;
	int index = 0;
	SIZE_T regionSize = 0x260; // 宝图任务的内容长度，这个长度应该够用了
	for (size_t i = 0; i < baotu_task_symbol_list.size(); ++i) {
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol_list[i] - regionSize + symbol_len), buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			auto today_num = bytes_to_wstring(&buffer[regionSize - symbol_len - pre_symbol_len], symbol_len + pre_symbol_len);
			auto tag_number = findContentBetweenTags(today_num, L"(今天已领取#R", L"#B次)");
			int num = 0;
			try {
				num = std::stoi(tag_number.at(0));
			}
			catch (std::invalid_argument& e) {
				continue;
			}
			catch (std::out_of_range& e) {
				continue;
			}
			if (baotu_task_count > 0) {
				if (num <= baotu_task_count) { continue; }  // 领取次数小于等于上一次的记录，说明是以前的内存没有被释放，忽略掉
				else {
					temp_count = num;
					index = i;
					break;
				}
			}
			else {
				if (num > temp_count) {
					temp_count = num;
					index = i;
				}
			}
		}
		delete[]buffer;
	}
	if (temp_count > 0) {
		std::wstring content;
		BYTE* buffer = new BYTE[regionSize];
		SIZE_T bytesRead;
		pNtReadVirtualMemory(hProcess, (PVOID)(baotu_task_symbol_list[index] - regionSize + symbol_len), buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			for (int i = 0; i < bytesRead - 4; i++) {
				if (buffer[i] == 0x00 && buffer[i + 1] == 0x00 && buffer[i + 2] == 0xC6 && buffer[i + 3] == 0x25) {	// ◆:C6 25
					content = bytes_to_wstring(&buffer[i + 2], bytesRead - i + 2 - symbol_len - pre_symbol_len + 4);
					break;
				}
			}
		}
		if (!content.empty()) {
			baotu_task_count = temp_count;
			log_info("今天已领取:%d次", baotu_task_count);
			std::vector<std::wstring> all_tags_wstr;
			auto tag_wstr1 = findContentBetweenTags(content, L"#K", L"#B");
			auto tag_wstr2 = findContentBetweenTags(content, L"#R", L"#B");
			all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr1.begin(), tag_wstr1.end());
			all_tags_wstr.insert(all_tags_wstr.end(), tag_wstr2.begin(), tag_wstr2.end());
			for (auto& wstr : all_tags_wstr)
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
			if (baotu_target_scene_id <= 0) { log_info("未支持的场景，等待添加"); }
			else{ log_info("解析宝图任务成功:%d,(%d,%d)", baotu_target_scene_id, baotu_target_pos.x, baotu_target_pos.y); }
		}
		delete[]buffer;
	}
	log_info("结束解析宝图任务内容:0x%p", hProcess);
}
void WindowInfo::parse_zeiwang_info() {
	log_info("开始解析贼王内容:0x%p", hProcess);
	// ◆缉拿潜伏在#R傲来客栈二楼#B内待机作案的#R<EvalFunc>{"str":"贼王司马龚","estr":"贼王司马龚","evalID":2755,}</EvalFunc>。当前剩余时间30分钟。#r
	// 接下一次宝图任务后，贼王内存结构就会释放，所以基本只有存在一条匹配记录
	//贼王位置有时候会刷新变动，但只是界面显示变了，实际坐标没变
	auto zeiwang_symbol_list = PerformAoBScanEx(
		hProcess,
		NULL,
		"22 00 2C 00 22 00 65 00 73 00 74 00 72 00 22 00 3A 00 22 00 3C 8D 8B 73"  // ","estr":"贼王
	);
	int symbol_len = 24;
	int tail_symbol_len = 0xC0;
	SIZE_T regionSize = 0x300; // 宝图任务的内容长度，这个长度应该够用了
	BYTE* buffer = new BYTE[regionSize];
	SIZE_T bytesRead = 0;
	for(int s=0;s < zeiwang_symbol_list.size();s++){
		//log_info("找到贼王任务内容，开始解析");
		std::wstring content;
		pNtReadVirtualMemory(hProcess, (PVOID)(zeiwang_symbol_list[s] - regionSize + symbol_len + tail_symbol_len), buffer, regionSize, &bytesRead);
		if (bytesRead > 0) {
			for (int i = 0; i < bytesRead - 4; i++) {
				if (buffer[i] == 0x00 && buffer[i + 1] == 0x00 && buffer[i+2] == 0xC6 && buffer[i + 3] == 0x25) {	// ◆:C6 25
					content = bytes_to_wstring(&buffer[i+2], bytesRead - i+2);
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
			for (auto& wstr : all_tags_wstr)
			{
				if (zeiwang_scene_id <= 0) zeiwang_scene_id = get_scene_id_by_name(wstr);
				if (zeiwang_fake_pos.x <= 0) {
					size_t pos = wstr.find(L"，", 0);
					if (pos != std::wstring::npos) {
						zeiwang_fake_pos.x = std::stoi(wstr.substr(0, pos));
						zeiwang_fake_pos.y = std::stoi(wstr.substr(pos + 1, wstr.length()));
					}
				}
			}
			if (zeiwang_scene_id > 0) {
				bytesRead = 0;
				memset(buffer, 0, regionSize);
				pNtReadVirtualMemory(hProcess, (PVOID)zeiwang_symbol_list[s], buffer, 0x18, &bytesRead);
				if (bytesRead > 0) {
					auto name_content = bytes_to_wstring(buffer, bytesRead);
					auto tags_str = findContentBetweenTags(content, L"\"estr\":\"", L"\",\"");
					if (!tags_str.empty()) zeiwang_name = tags_str[0];
				}
				setlocale(LC_ALL, ""); // 设置本地化
				log_info("%d,%ls", zeiwang_scene_id, zeiwang_name.c_str());
				break;
			}
		}
		bytesRead = 0;
		memset(buffer, 0, regionSize);
	}
	delete[]buffer;
	if (zeiwang_scene_id <= 0) log_info("未支持的场景，等待添加");
	log_info("结束解析贼王内容:0x%p", hProcess);
}

void WindowInfo::move_to_position(POINT dst, long active_x, long active_y) {
	// active_x, active_y:NPC在这个坐标范围内对话才有效
	if (abs(player_pos.x - dst.x) <= active_x && abs(player_pos.y - dst.y) <= active_y) {
		log_info("有效对话范围内，不用移动");
		return;  // 有效对话范围内，不用移动
	}
	handle_sheyaoxiang_time();
	POINT astar_pos = dst;
	if (active_x != 0 && active_y != 0) {
		// A星寻路
		astar_pos = astar(player_pos.x, player_pos.y, dst.x, dst.y, m_scene_id, active_x, active_y);
		if (astar_pos.x < 0) {
			if (player_pos.x < dst.x) astar_pos.x = dst.x - active_x;
			else player_pos.x = dst.x + active_x;
			if (player_pos.y < dst.y) astar_pos.y = dst.y - active_y;
			else player_pos.y = dst.y + active_y;
		}
		log_info("寻路坐标:%d,%d", astar_pos.x, astar_pos.y);
	}
	if (is_in_mini_scene()) {
		log_info("屏幕视线内移动");
		click_position_at_edge(astar_pos);
	}
	else {
		move_via_map(astar_pos);
	}
	//if (abs(player_pos.x - astar_pos.x) <= mScreen_x && abs(player_pos.y - astar_pos.y) <= mScreen_y) {
	//	log_info("屏幕视线内移动");
	//	click_position(astar_pos);
	//}
	//else {
	//log_info("地图移动");
	//move_via_map(astar_pos);
	handle_health();
	//}
	moving = true;
}
void WindowInfo::move_to_position_flat(POINT dst, long active_x, long active_y) {
	// active_x, active_y:NPC在这个坐标范围内对话才有效
	// 针对没有障碍物的地图，随意点击任何地方都能移动
	// 有些图有大障碍物，点击了是不会移动的
	if (abs(player_pos.x - dst.x) <= mScreen_x && abs(player_pos.y - dst.y) <= mScreen_y) {
		log_info("屏幕视线内移动");
		click_position_at_edge(dst);
	}
	else {
		log_info("地图移动");
		move_via_map(dst);
	}
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
	long x_base = map_pos.x - 7; // 地图左上角实际的像素位置
	long y_base = map_pos.y + 58 + max_px.y; // 地图左上角实际的像素位置
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
	if (is_near_loc(door, mScreen_x, mScreen_x)) {
		handle_sheyaoxiang_time();
		click_position_at_edge(door, xs, ys);
		//wait_scene_change(scene_id);
	}
	else {
		move_to_position(door);
		if(close_beibao)close_beibao_smart();
		//moving = true;
	}
	moving = true;
}
void WindowInfo::ship_to_other_scene(POINT door, unsigned int scene_id, int xs, int ys, bool close_beibao) {
	// 通过驿站老板或NPC传送到另一个场景
	//door：另一个场景入口
	//scene_id:另一个场景id
	click_position(door,0,-5);
	auto pos = WaitMatchingRectLoc(ROI_npc_talk(), *m_img_btn_shide_woyaoqu);
	if (pos.x > 0) {
		mouse_click_human(pos, 0, 0, 1);
		wait_scene_change(scene_id);
	}
}
bool WindowInfo::click_position(POINT dst, int xs, int ys, int x_fix, int y_fix, int mode) {
	// 通过鼠标点击目的坐标
	// 如果NPC在视野范围内，可以和NPC对话
	// 如果没有NPC，则移动到目的坐标
	update_player_float_pos();
	auto px = compute_pos_pixel({ dst.x, dst.y}, m_scene_id, xs == 0 && ys == 0);
	return mouse_click_human({px.x + x_fix, px.y + y_fix }, xs, xs, mode);
}
void WindowInfo::click_position_at_edge(POINT dst, int xs, int ys, int x_fix, int y_fix, int mode) {
	for (int i = 0; i < 3; i++) {
		if (click_position(dst, xs, ys, x_fix, y_fix, mode))break;
		move_cursor_center_bottom();
	}
}
bool WindowInfo::talk_to_npc_fight(POINT dst, const cv::Mat& templ) {
	POINT temp_pos = {-1,-1};
	for (int i = 0;i < 2;i++) {
		update_player_float_pos();
		if (temp_pos.x != player_pos.x && temp_pos.y != player_pos.y) {
			//人物坐标发生改变，要重新隐藏一下玩家
			hide_player_n_stalls();
		}
		temp_pos = player_pos;
		log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
		log_info("点击的坐标:%d,%d", dst.x, dst.y);
		int ys = -5;
		if (i > 0)ys = -40;
		click_position_at_edge(dst,0,0,0,ys);
		if (ClickMatchImage(ROI_npc_talk(), templ, "", gThreshold, gMatchMethod, 0, 0, 0, 0, 1, 1200) && wait_fighting()) {
			log_info("发起战斗成功");
			is_four_man();
			return true;
		}
		handle_wrong_attack();
	}
	log_info("检查是否重叠");
	for (int j = 0;j <= 3;j++) {
		update_player_float_pos();
		if (temp_pos.x != player_pos.x && temp_pos.y != player_pos.y) {
			//人物坐标发生改变，要重新隐藏一下玩家
			hide_player_n_stalls();
		}
		temp_pos = player_pos;
		log_info("玩家坐标:%d,%d", player_pos.x, player_pos.y);
		log_info("点击的坐标:%d,%d", dst.x, dst.y);
		click_position_at_edge({ dst.x,dst.y}, 0, 0, 0, -3,5);
		// 一页显示5个NPC，只尝试前3个
		click_position_at_edge({ dst.x,dst.y}, 0, 0, 15, 30+j*20);
		if (ClickMatchImage(ROI_npc_talk(), templ, "", gThreshold, gMatchMethod, 0, 0, 0, 0, 1, 1200) && wait_fighting()) {
			log_info("发起战斗成功");
			is_four_man();
			return true;
		}
		handle_wrong_attack();
	}

	failure = true;
	return false;
}

bool WindowInfo::low_health(cv::Rect roi, int deadline) {
	int percent = 105;
	auto image = hwnd2mat(hwnd);
	const auto* vector_arrary = &health_list[0];
	for (int i = 0;i < health_list.size();i++) {
		if (!MatchingExist(image, roi, vector_arrary[i], "", 0.96)) percent -= 5;
		if (percent <= deadline)return true;
	}
	return false;
}
bool WindowInfo::low_mana(cv::Rect roi, int deadline) {
	int percent = 105;
	auto image = hwnd2mat(hwnd);
	const auto* vector_arrary = &mana_list[0];
	for (int i = 0;i < mana_list.size();i++) {
		if (!MatchingExist(image, roi, vector_arrary[i], "", 0.96)) percent -= 5;
		if (percent <= deadline)return true;
	}
	return false;
}
void WindowInfo::supply_health_hero() {
	if (low_health(ROI_health_hero(),80)) {
		for (int i = 0; i < 3; i++) {
			if (mouse_click_human({ rect.left + 980,rect.top + 9 }, -100,  30, 2))break;
		}
		move_cursor_right_top();
	}
}
void WindowInfo::supply_health_peg() {
	if (low_health(ROI_health_peg(), 55)) {
		for (int i = 0; i < 3; i++) {
			if (mouse_click_human({ rect.left + 869,rect.top + 9 }, -30, 30, 2))break;
		}
		move_cursor_right_top();
	}
}
void WindowInfo::supply_mana_hero() {
	if (low_mana(ROI_mana_hero(), 50)) {
		for (int i = 0; i < 3; i++) {
			if (mouse_click_human({ rect.left + 980,rect.top + 21 }, -100, 30, 2))break;
		}
		move_cursor_right_top();
	}
}
void WindowInfo::handle_health() {
	log_info("检查血蓝");
	supply_health_hero();
	supply_health_peg();
	supply_mana_hero();
}
void WindowInfo::time_pawn_update() {
	time_pawn.update();
	//task_pawn.update();
}
bool WindowInfo::out_of_rect(POINT pixel) {
	return pixel.x <rect.left || pixel.x>rect.right || pixel.y<rect.top || pixel.y>rect.bottom;
}
void WindowInfo::play_mp3_once() {
	if (!mp3_playing) { 
		play_mp3(); 
		mp3_playing = true;
	}
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
cv::Rect WindowInfo::ROI_cursor(POINT pos, int width) {
	cv::Rect roi(pos.x - rect.left - width, pos.y - rect.top - width, width *2, width *2);
	return roi;
}

cv::Rect WindowInfo::ROI_beibao() {
	return cv::Rect(400, 125, 250, 200);
}

cv::Rect WindowInfo::ROI_map() {
	return cv::Rect(120, 100, 820, 560);
}

cv::Rect WindowInfo::ROI_npc_talk() {
	return cv::Rect(160, 330, 700, 330);
}

cv::Rect WindowInfo::ROI_beibao_props() {
	return cv::Rect(100, 100, 850, 650);
}
cv::Rect WindowInfo::ROI_task() {
	return cv::Rect(820, 100, 204, 310);
}
cv::Rect WindowInfo::ROI_changan777_changanjiudian() {
	log_info("长安合成旗-长安酒店");
	npc_found = false;
	return cv::Rect(680, 350, 40, 40);
}

cv::Rect WindowInfo::ROI_changan777_yizhan_laoban() {
	log_info("长安合成旗-驿站老板");
	npc_found = false;
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
cv::Rect WindowInfo::ROI_changan777_dangpu() {
	log_info("长安合成旗-当铺");
	return cv::Rect(530, 490, 60, 50);
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
	return cv::Rect(375, 390, 55, 60);
}
cv::Rect WindowInfo::ROI_feixingfu_xiliangnvguo() {
	log_info("飞行符-西凉女国");
	return cv::Rect(385, 310, 50, 60);
}
cv::Rect WindowInfo::ROI_feixingfu_jianyecheng() {
	log_info("飞行符-建邺城");
	return cv::Rect(610, 410, 65, 70);
}
cv::Rect WindowInfo::ROI_feixingfu_changshoucun() {
	log_info("飞行符-长寿村");
	return cv::Rect(390, 240, 50, 70);
}
cv::Rect WindowInfo::ROI_feixingfu_zhuziguo() {
	log_info("飞行符-朱紫国");
	return cv::Rect(440, 450, 45, 60);
}
cv::Rect WindowInfo::ROI_feixingfu_aolaiguo() {
	log_info("飞行符-傲来国");
	return cv::Rect(755, 465, 70, 70);
}
//cv::Rect WindowInfo::ROI_fighting() {
//	return cv::Rect(1004, 115, 20, 65);
//}
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
	return cv::Rect(290, 170, 490, 280);
}
cv::Rect WindowInfo::ROI_paixu_verify() {
	return cv::Rect(300, 180, 450, 320);
}
cv::Rect WindowInfo::ROI_yidongdezi() {
	return cv::Rect(150, 90, 280, 170);
}
cv::Rect WindowInfo::ROI_fight_action() {
	return cv::Rect(850, 100, 150, 550);
}
void WindowInfo::test() {
	// 玩家坐标地址
	//is_verifying();
	//scan_npc_pos_addr(NPC_ZEIWANG);
	//parse_baotu_task_info();
	//parse_zeiwang_info();
	SetForegroundWindow(hwnd);
	//mouse_click_human({ rect.left + 996, rect.top + 8 },0,0,0);
	//mouse_click_human({ rect.left + 130, rect.top + 645 }, 30, -30, 0);
	//update_scene_id();
	//scan_npc_pos_addr_by_id(店小二);
	//scan_npc_pos_addr(店小二);
	//scan_npc_pos_addr_by_id(长安驿站老板);
	//scan_npc_pos_addr_by_id(贼王);
	//scan_npc_pos_addr_by_id(26946341);
	//update_player_float_pos();
	//scan_current_scene_npc_id();
	//update_npc_pos(店小二);
	//move_to_position({ 460,140 }, NPC_TALK_VALID_DISTENCE, NPC_TALK_VALID_DISTENCE);
	//hwnd2mat(hwnd);
	while (1) {
		//Sleep(2000);
		//update_npc_pos(店小二);
		update_scene_id();
		update_player_float_pos();
		update_player_float_pos();
		//log_info("测试日志22222");
	}
}

TimeProcessor::TimeProcessor() {
	mTime_ms = getCurrentTimeMilliseconds();
}
bool TimeProcessor::timeout(uint64_t time) {
	bool out = getCurrentTimeMilliseconds() - mTime_ms >= time;
	if (out) {
		update();
		//log_info("超时");
	}
	return out;
}
void TimeProcessor::update() {
	mTime_ms = getCurrentTimeMilliseconds();
}
bool TimeProcessor::time_wait(uint64_t time) {
	return getCurrentTimeMilliseconds() - mTime_ms >= time;
}

GoodMorning::GoodMorning() {
	for (auto processID : FindPidsByName(TARGET_APP_NAME)) {
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
	}
}

void GoodMorning::init() {
	struct stat st = { 0 };
	if (stat("screenshot", &st) == -1) {
		_mkdir("screenshot");
	}
	for (auto& winfo : this->winsInfo) {
		winfo->UpdateWindowRect();
		winfo->SplitTitleAsPlayerId();
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
	for (auto& winfo : this->winsInfo) {
		if (!winfo->player_id.empty()) {
			bool new_content = false;
			auto player = db[winfo->player_id];
			bool no_key = false;
			if (player.empty()) {
				player["title"] = AnsiToUtf8(winfo->player_name);
				player["round"] = 0;
				new_content = true;
			}
			else if (!player.contains("title")) {
				player["title"] = AnsiToUtf8(winfo->player_name);
				new_content = true;
			}
			if (new_content) {
				db[winfo->player_id] = player;
				update = true;
			}
		}
	}
	if (update) update_db();
}

void GoodMorning::hook_data() {
	for (auto& winfo : this->winsInfo) {
		winfo->init();
		winfo->hook_init();
		winfo->update_player_float_pos();
	}
}

void GoodMorning::work() {
	// 创建线程池
	std::vector<std::thread> threads;
	for (int i = 0;i < this->winsInfo.size();i++) {
		auto t1= std::thread(&WindowInfo::scan_npc_pos_in_thread, this->winsInfo[i].get());
		t1.detach();
		threads.emplace_back(std::move(t1));
	}

	while (true) {
		//std::thread t1;
		for (const auto& winfo : this->winsInfo) {
			//log_info("测试日志");
			//winfo->tScan_npc = 1;
			//Sleep(3000);
			//continue;

			winfo->datu();
			if (winfo->popup_verify) {
				winfo->datu();
			}
			if (winfo->time_pawn.timeout(300000)) {
				winfo->play_mp3_once();
				SetForegroundWindow(winfo->hwnd);
				for (int i = 0;i < 5;i++) { log_info("运行超时，退出"); }
				return;
			}
			if (winfo->failure) {
				winfo->play_mp3_once();
				SetForegroundWindow(winfo->hwnd);
				for (int i = 0;i < 5;i++) { log_info("运行失败，请重新运行程序退出"); }
				return;
			}

		}
		//Sleep(10);
	}
}
//void GoodMorning::time_pawn_update() {
//	time_pawn.update();
//	task_pawn.update();
//}
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
	for (const auto& winfo : this->winsInfo) {
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
		//SetForegroundWindow(winfo.hwnd);
		//winfo.update_scene_id();
		//winfo.update_player_float_pos();
		//winfo.parse_baotu_task_info();
		//winfo.parse_zeiwang_info();
		while (true) {
			//winfo.update_player_float_pos();
			winfo->test();
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
	current = steps[index];
}

void Step::reset() {
	index = 1;
	current = steps[index];
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

// --- 辅助函数：将十六进制字符串转换为字节向量和掩码向量 ---
void parseAobString(const std::string& aobStr, std::vector<unsigned char>& pattern, std::vector<char>& mask) {
	int start, end;
	start = end = 0;
	char dl = ' ';
	while ((start = aobStr.find_first_not_of(dl, end)) != std::string::npos) {
		// str.find(dl, start) will return the index of dl
		// from start index
		end = aobStr.find(dl, start);
		// substr function return the substring of the
		// original string from the given starting index
		// to the given end index
		auto sub_s = aobStr.substr(start, end - start);
		if (sub_s == "?") {
			pattern.push_back(0x00); // 占位符
			mask.push_back(1);
		}
		else {
			pattern.push_back(static_cast<unsigned char>(std::stoul(sub_s, nullptr, 16)));
			mask.push_back(0);
		}
	}
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
cv::Mat cv_imread(const char* filename, int flags) {
	return cv::imread((current_path / filename).string(), flags);
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
	if (src.empty())return src;

	cv::Mat image_bgr;
	// 2. Use cvtColor with the COLOR_BGRA2BGR conversion code
	cv::cvtColor(src, image_bgr, cv::COLOR_BGRA2BGR);

	//save_screenshot(image_bgr);

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
			auto winfo = std::make_unique<WindowInfo>((HANDLE)targetProcessId);
			//WindowInfo winfo((HANDLE)targetProcessId);
			winfo->hwnd = hwnd;
			winfo->player_name = pszMem;
			auto parent_region=findContentBetweenTags(pszMem,"ONLINE - (", "[");
			if (!parent_region.empty()) {
				winfo->RegionMonthly=parent_region[0] == "畅玩服";//畅玩服与时间服区分
			}
			gm.winsInfo.push_back(std::move(winfo));
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
void test222(void* uintptr_t) {
	while (1) {
		log_info("测试日志22222");
		Sleep(2000);
	}
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

	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
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

	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	blur(src_gray, detected_edges, cv::Size(3, 3));

	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio1, kernel_size);

	dst = cv::Scalar::all(0);

	src.copyTo(dst, detected_edges);

	cv::imwrite("222.png", dst);
}
static void Image2Gray() {
	auto src = cv::imread("111.png", cv::IMREAD_GRAYSCALE); // Load an image
	cv::imwrite("222.png", src);
}
bool MatchingExist(cv::Mat& image, cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method)
{
	return MatchingLoc(image, roi_rect, templ, mask_path, threshold, match_method, MATCHEXIST).x > -1;
}
POINT MatchingLoc(cv::Mat& image, cv::Rect roi_rect, const cv::Mat& templ, std::string mask_path, double threshold, int match_method, int loc) {
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
	if (image_roi.empty())return { -1, -1 };
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
		//if (match_method == cv::TM_CCOEFF_NORMED) log_info("TM_CCOEFF_NORMED:%f matchLoc:%d,%d", maxVal, matchLoc.x, matchLoc.y);
		// --- 6. Draw a rectangle around the best match area ---
		// The top-left corner is matchLoc. The bottom-right is calculated by adding the template dimensions.
		//if (match_method == cv::TM_CCORR_NORMED) {
		//	log_info("TM_CCORR_NORMED:%f matchLoc:%d,%d", maxVal, matchLoc.x, matchLoc.y);
		//	rectangle(image_roi, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
		//	save_screenshot(image_roi);
		//}
	}
	return { matchLoc.x, matchLoc.y };
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
		//std::cerr << "Error writing to serial port." << std::endl;
	}
	else {
		//std::cout << "Sent: " << data << std::endl;
	}
}

void SerialRead() {
	// Example: Reading data (simplified, typically done in a loop/thread)
	char buffer[256];
	DWORD bytes_read;
	if (!ReadFile(gm.hSerial, buffer, sizeof(buffer) - 1, &bytes_read, NULL)) {
		//std::cerr << "Error reading from serial port." << std::endl;
	}
	else {
		buffer[bytes_read] = '\0';
		//std::cout << "Received: " << buffer << std::endl;
	}
}

void serial_move(POINT pos, int mode) {
	int64_t snp_len = strlen(MS_MOVE_SYMBOL) + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + 1;
	// 在堆上分配内存
	char* data_buf = new char[snp_len];
	snprintf(data_buf, snp_len, MS_MOVE_SYMBOL, pos.x, pos.y, mode);
	SerialWrite(data_buf);
	// 使用完毕后，必须手动释放内存，防止内存泄漏
	delete[] data_buf;
	SerialRead();
	//Sleep(50);
}
//void serial_move_human(POINT pos, int mode) {
//	POINT mouse_pos;
//	GetCursorPos(&mouse_pos);
//	int64_t snp_len = strlen(MS_MOVE_HUMAN_SYMBOL) + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + LEN_OF_INT64 + 1;
//	// 在堆上分配内存
//	char* data_buf = new char[snp_len];
//	snprintf(data_buf, snp_len, MS_MOVE_HUMAN_SYMBOL, mouse_pos.x, mouse_pos.y, pos.x, pos.y, mode);
//	SerialWrite(data_buf);
//	// 使用完毕后，必须手动释放内存，防止内存泄漏
//	delete[] data_buf;
//	SerialRead();
//	//Sleep(50);
//}
void serial_click_cur() {
	SerialWrite(CLICK_CURRENT_SYMBOL);
	SerialRead();
}
void serial_right_click_cur() {
	SerialWrite(RIGHT_CLICK_CURRENT_SYMBOL);
	SerialRead();
}
void serial_ctrl_click_cur() {
	SerialWrite(CTRL_CLICK_CURRENT_SYMBOL);
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
void hide_player() {
	input_key_xxx("F9");
	log_info("隐藏玩家");
}
void hide_stalls() {
	input_alt_xxx("h");
	log_info("隐藏摆摊");
}
void hide_player_n_stalls() {
	SerialWrite(KEY_HIDE);
	log_info("隐藏玩家和摆摊");
	SerialRead();
}
void stop_laba() {
	log_info("开始停止喇叭");
	for (int i = 0;i < 1;i++) { 
		SerialWrite(STOP_MP3); 
		SerialRead();
	}
	log_info("结束停止喇叭");
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
		else {
			break;
		}
	}
	return res;
}
std::vector<std::string> findContentBetweenTags(
	const std::string& source,
	const std::string& startTag,
	const std::string& endTag) {
	std::vector<std::string> res;
	size_t pos = 0; // Start search from the beginning

	// Loop until find() returns std::wstring::npos (not found)
	while ((pos = source.find(startTag, pos)) != std::string::npos) {
		// 计算实际内容的起始索引：起始标记的位置 + 起始标记的长度
		size_t contentStartIdx = pos + startTag.length();
		size_t endPos = source.find(endTag, contentStartIdx);
		// 检查是否找到了结束标记
		if (endPos != std::string::npos) {
			// substr() 的第二个参数是需要提取的长度，而不是结束索引。
			size_t length = endPos - contentStartIdx;
			res.push_back(source.substr(contentStartIdx, length));
			pos = endPos + endTag.length();
		}
		else {
			break;
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
		case 大唐国境:
			return { 377, 360 };
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
			return { 384, 285 };
		case 长安酒店二楼:
			return { 384, 285 };
		case 长安城:
			return { 545, 276 };
		case 东海湾:
			return { 276, 276 };
		case 化生寺:
			return { 379, 276 };
		case 长安国子监:
			return { 384, 284 };
		case 长安杂货店:
			return { 31, 38 };  //乱写，目前不用到
		case 地府:
			return { 369, 276 };
		case 狮驼岭:
			return { 369, 276 };
		case 建邺杂货店:
			return { 119, 119 }; //乱写，目前不用到
		case 花果山:
			return { 369, 276 };
		case 长寿郊外:
			return { 316, 276 };
		case 傲来客栈:
			return { 119, 119 }; //乱写，目前不用到
		case 傲来客栈二楼:
			return { 384, 284 };
		case 傲来国药店:
			return { 119, 119 }; //乱写，目前不用到
	}
	return { -1, -1 };
}
//std::vector<POINT> get_scene_npc_list(unsigned int scene_id) {
//	switch (scene_id)
//	{
//		case 长安杂货店:
//			return changan_zahuodian_npc_list;
//		case 长安饰品店:
//			return changan_shipindian_npc_list;
//		case 长安国子监:
//			return changan_guozijian_npc_list;
//		case 建邺城:
//			return jianyecheng_npc_list;
//		//case 建邺衙门:
//		//	return jianyeyamen_npc_list;
//		case 建邺杂货店:
//			return jianyezahuodian_npc_list;
//		case 傲来客栈二楼:
//			return aolaikezhanerlou_npc_list;
//		case 傲来国药店:
//			return aolaiguo_yaodian_npc_list;
//		case 长寿村当铺:
//			return changshoucun_dangpu_npc_list;
//		//case 长寿郊外:
//		//	return changshoujiaowai_npc_list;
//	}
//	return std::vector<POINT>{};
//}
int main(int argc, const char** argv)
{
	//log_info("日志输出测试");

	//CannyThreshold(0, 0);
	//Image2Gray();
	//ThresholdinginRange();

	//auto image = cv::imread("111.png", cv::IMREAD_COLOR);
	//cv::Mat image_inRange = ThresholdinginRange(image);
	//MatchingLoc(image_inRange,ROI_NULL(), img_cursors_cursor,"",0.7,cv::TM_CCORR_NORMED);
	//MatchingRectLoc(ROI_NULL(), "111.png","666.png");
	//std:;vector<POINT> ssss = { {10,10},{20,30} };
	//auto ss = ssss.front();
	//ssss.erase(ssss.begin());

	//test();
	//SerialWrite(STOP_MP3);
	//serial_move_human(67, 84, 1);

	//const char* send_data = "hkey:WIN\n";
	//SerialWrite(send_data);
	//SerialRead();
	//Sleep(10000);
	//for (auto processID : FindPidsByName(TARGET_APP_NAME)) {
	//	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processID));
	//}
	//Sleep(10);  // 等一下枚举窗口句柄回调完成再执行

	HWND hwnd = GetConsoleWindow();
	// 2. Set new position (x=100, y=100)
	// SWP_NOSIZE: Keeps the current width and height
	// SWP_NOZORDER: Keeps the window's current place in the stack (front/back)
	SetWindowPos(hwnd, NULL, 1280, 510, 640, 530, SWP_NOZORDER);

	init_log();
	log_info("开始，检测到窗口数量:%d个.", gm.winsInfo.size());

	// SEED the generator ONCE at the start of the program
	std::srand(static_cast<unsigned int>(time(nullptr)));
	Serial();
	//stop_laba();

	gm.init();
	gm.hook_data();
	//gm.test();
	gm.work();
	return 0;
}


