# -*- coding:utf-8 -*-
import logging
import os
import time
import datetime
import ctypes
import json
import configparser
import io
import random
import string
import serial.tools.list_ports
import cv2
import numpy as np
import easyocr
from PIL import ImageGrab
import win32clipboard
import win32gui
# import tkinter
# import win32api
# import win32con
# import pywintypes
import autoit
# import uiautomation as uia

# uia.uiautomation.SetGlobalSearchTimeout(15)  # 设置全局搜索超时 15

org_screen_path = os.path.join(os.getenv("LOCALAPPDATA"), "tmpimage")
db_file = "database"

try:
    os.makedirs(org_screen_path)
except OSError:
    pass
try:
    os.mkdir("screenshot")
except:
    pass
try:
    os.mkdir("debug_img")
except:
    pass
try:
    os.makedirs("log")
except OSError:
    pass
try:
    os.makedirs("four_man_source")
except OSError:
    pass
if not os.path.exists(db_file):
    with open(db_file, 'w') as ffff:
        ffff.write("{}")

logger = logging.getLogger('log')
logger.setLevel(level=logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(module)s:%(lineno)d - %(message)s")
file_handler = logging.FileHandler("%s/%s.log" % ('log', datetime.datetime.now().strftime('%Y-%m-%d')))
file_handler.setFormatter(formatter)
logger.addHandler(file_handler)
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
logger.addHandler(stream_handler)

user32 = ctypes.windll.user32
screensize = user32.GetSystemMetrics(78), user32.GetSystemMetrics(79)

game_title = "[TITLE:梦幻西游 ONLINE; CLASS:MHXYMainFrame;]"
# login_game_title = "[TITLE:梦幻西游 ONLINE - ; CLASS:MHXYMainFrame;]"
# login_game_title = "[TITLE:梦幻西游 ONLINE - ;]"
cmd_titles = ["[CLASS:CASCADIA_HOSTING_WINDOW_CLASS;]", "[CLASS:ConsoleWindowClass;]"]

RESET_SYMBOL = b'reset\n'
# CLICK_SYMBOL = b'click:%d,%d\n'
RIGHT_CLICK_SYMBOL = b'rightclick:%d,%d\n'
CURRENT_MOUSE = b'currentM:%d,%d\n'
# CLICK_CURRENT_SYMBOL = b'clickcurrent\n'
# RIGHT_CLICK_CURRENT_SYMBOL = b'rightclickcurrent\n'
# MS_SCROLL_SYMBOL = b'scroll:%d,%d,%d\n'
MS_MOVE_SYMBOL = b'move:%d,%d\n'
MS_CLICK_LINEAR = b'clickln:%d,%d,%d,%d,%d,%d,%d\n'  # x,y,near_x,near_y,fake_x,fake_y,mode
MS_MOVE_LINEAR = b'moveln:%d,%d,%d,%d\n'  # cx,cy,x,y
MS_PRESS_LONG = b'mousepress:%d\n'
MS_DRAG_AND_DROP_SYMBOL = b'dnd:%d,%d,%d,%d\n'
KEY_STRING = b'kb:%s\n'
# KEY_PRESS = b"press:%s\n"

# 拟人
CLICK_SYMBOL = b'clickhm:%d,%d\n'
MS_MOVE_HUMAN_SYMBOL = b'movehm:%d,%d,%d,%d,%d\n'  # cx,cy,x,y,mode
KEY_PRESS = b"hkey:%s\n"
CLICK_CURRENT_SYMBOL = b'hkeyCC\n'
RIGHT_CLICK_CURRENT_SYMBOL = b'hkeyRCC\n'
MS_SCROLL_SYMBOL = b'scrollhm:%d,%d,%d\n'
# 战斗快捷键随机平A
# 战斗快捷键选择法术
# 战斗快捷键使用默认法术随机攻击
# 战斗快捷键使用默认法术选择攻击
# KEY_ALT_xxx = b'press:ALT_%s\n'
# KEY_ALT_fff = b'press:ALT+%s\n'
# KEY_CTRL_xxx = b'press:CTRL_%s\n'

KEY_ALT_xxx = b'hkey:ALT_%s\n'
KEY_ALT_fff = b'hkey:ALT+%s\n'
KEY_CTRL_xxx = b'hkey:CTRL_%s\n'
KEY_CTRL_fff = b'hkey:CTRL+%s\n'
KEY_HIDE = b'hide\n'
# 音乐播放
# PLAY_MP3 = b'mp3:PLAY_%s\n'
# STOP_MP3 = b'mp3:STOP\n'

PLAY_MP3 = b'mmp3:PLAY_%s\n'
STOP_MP3 = b'mmp3:STOP\n'
# 门派
MOWANGZHAI = "魔王寨"
LONGGONG = "龙宫"
PUTUOSHAN = "普陀山"
LINGBOCHENG = "凌波城"
HUASHENGSI = "化生寺"
DATANGGUANFU = "大唐官府"
WUDIDONG = "无底洞"

img_resoure = r"\object"
# 道具
# 长安饰品店老板
img_props_shuxin = img_resoure + r"\props\shuxin.png"
img_props_yinyaodai = img_resoure + r"\props\yinyaodai.png"
img_props_zhenzhulian = img_resoure + r"\props\zhenzhulian.png"
img_props_yaodai = img_resoure + r"\props\yaodai.png"
img_props_hushenfu = img_resoure + r"\props\hushenfu.png"
# 长安杂货店老板
img_props_dongmingcao = img_resoure + r"\props\dongmingcao.png"
img_props_taohua = img_resoure + r"\props\taohua.png"
img_props_sheyaoxiang = img_resoure + r"\props\sheyaoxiang.png"
img_props_lanhua = img_resoure + r"\props\lanhua.png"
img_props_gaoji_peg_food = img_resoure + r"\props\gaoji_peg_food.png"
img_props_peg_food = img_resoure + r"\props\peg_food.png"
# 长安服装店老板
img_props_buyi = img_resoure + r"\props\buyi.png"
img_props_fangjin = img_resoure + r"\props\fangjin.png"
img_props_maxue = img_resoure + r"\props\maxue.png"
img_props_linjia = img_resoure + r"\props\linjia.png"
img_props_wucaiqun = img_resoure + r"\props\wucaiqun.png"
img_props_meihuazanzi = img_resoure + r"\props\meihuazanzi.png"
img_props_buqun = img_resoure + r"\props\buqun.png"
img_props_buxie = img_resoure + r"\props\buxie.png"
img_props_zanzi = img_resoure + r"\props\zanzi.png"
img_props_mianju = img_resoure + r"\props\mianju.png"
# 长安福寿店老板
img_props_xiang = img_resoure + r"\props\xiang.png"
img_props_lazhu = img_resoure + r"\props\lazhu.png"
img_props_huangzhi = img_resoure + r"\props\huangzhi.png"
# 长安药店老板
img_props_xiangye = img_resoure + r"\props\xiangye.png"
img_props_baisehua = img_resoure + r"\props\baisehua.png"
img_props_caoguo = img_resoure + r"\props\caoguo.png"
img_props_guiqiecao = img_resoure + r"\props\guiqiecao.png"
img_props_foshou = img_resoure + r"\props\foshou.png"
img_props_shanyao = img_resoure + r"\props\shanyao.png"
img_props_yuejiancao = img_resoure + r"\props\yuejiancao.png"
img_props_jiuxiangchong = img_resoure + r"\props\jiuxiangchong.png"
img_props_qiyelian = img_resoure + r"\props\qiyelian.png"
# 长安武器店掌柜
img_props_zheshan = img_resoure + r"\props\zheshan.png"
img_props_hongyingqiang = img_resoure + r"\props\hongyingqiang.png"
img_props_duntiezhongjian = img_resoure + r"\props\duntiezhongjian.png"
img_props_niupibian = img_resoure + r"\props\niupibian.png"
img_props_quliuzhang = img_resoure + r"\props\quliuzhang.png"
img_props_tiezhua = img_resoure + r"\props\tiezhua.png"
img_props_songmuchui = img_resoure + r"\props\songmuchui.png"
img_props_liulizhu = img_resoure + r"\props\liulizhu.png"
img_props_shuangduanjian = img_resoure + r"\props\shuangduanjian.png"
img_props_qingtongduanjian = img_resoure + r"\props\qingtongduanjian.png"
img_props_liuyedao = img_resoure + r"\props\liuyedao.png"
img_props_suzhideng = img_resoure + r"\props\suzhideng.png"
img_props_qingtongfu = img_resoure + r"\props\qingtongfu.png"
img_props_wuseduandai = img_resoure + r"\props\wuseduandai.png"
img_props_huangtongquan = img_resoure + r"\props\huangtongquan.png"
img_props_yingmugong = img_resoure + r"\props\yingmugong.png"
img_props_ximubang = img_resoure + r"\props\ximubang.png"
img_props_youzhisan = img_resoure + r"\props\youzhisan.png"
# 长安武器店老板
img_props_liqinghuan = img_resoure + r"\props\liqinghuan.png"
img_props_zhenbaozhu = img_resoure + r"\props\zhenbaozhu.png"
img_props_longfengshuangjian = img_resoure + r"\props\longfengshuangjian.png"
img_props_baiduanzhang = img_resoure + r"\props\baiduanzhang.png"
img_props_zizhusan = img_resoure + r"\props\zizhusan.png"
img_props_zheshijujian = img_resoure + r"\props\zheshijujian.png"
img_props_yuruyi = img_resoure + r"\props\yuruyi.png"
img_props_youmingguizhua = img_resoure + r"\props\youmingguizhua.png"
img_props_wulongbian = img_resoure + r"\props\wulongbian.png"
img_props_juchimao = img_resoure + r"\props\juchimao.png"
img_props_shuangmianfu = img_resoure + r"\props\shuangmianfu.png"
img_props_jinggangshan = img_resoure + r"\props\jinggangshan.png"
img_props_yemowandao = img_resoure + r"\props\yemowandao.png"
img_props_balingjinzhua = img_resoure + r"\props\balingjinzhua.png"
img_props_hongdenglong = img_resoure + r"\props\hongdenglong.png"
img_props_wuyuejian = img_resoure + r"\props\wuyuejian.png"
img_props_zitangong = img_resoure + r"\props\zitangong.png"
img_props_jinsiduandai = img_resoure + r"\props\jinsiduandai.png"
img_props_baozi = img_resoure + r"\props\baozi.png"
img_props_cangku_empty_grid_640 = img_resoure + r"\props\cangku_empty_grid_640.png"
img_props_feixingfu = img_resoure + r"\props\feixingfu.png"
img_props_red_777 = img_resoure + r"\props\red_777.png"
img_props_green_777 = img_resoure + r"\props\green_777.png"
img_props_yellow_777 = img_resoure + r"\props\yellow_777.png"
img_props_white_777 = img_resoure + r"\props\white_777.png"
img_props_tianyantong = img_resoure + r"\props\tianyantong.png"
# 按钮
img_btn_huiqule = img_resoure + r"\btn\huiqule.png"
img_btn_talk_buy = img_resoure + r"\btn\talk_buy.png"
img_btn_give_props_comfirm = img_resoure + r"\btn\give_props_comfirm.png"
img_btn_give_props_renwu = img_resoure + r"\btn\give_props_renwu.png"
img_btn_give_props_prop = img_resoure + r"\btn\give_props_prop.png"
img_btn_npc_buy_comfirm = img_resoure + r"\btn\npc_buy_comfirm.png"
img_btn_talk_shifu_renwu = img_resoure + r"\btn\talk_shifu_renwu.png"
img_btn_exit_game_640 = img_resoure + r"\btn\exit_game_640.png"
img_btn_shimen_task_continue = img_resoure + r"\btn\shimen_task_continue.png"
img_btn_shimen_task_cancel = img_resoure + r"\btn\shimen_task_cancel.png"
img_btn_dubatian_diancuole = img_resoure + r"\btn\dubatian_diancuole.png"
img_btn_lishimin_wodiancuole = img_resoure + r"\btn\lishimin_wodiancuole.png"
img_btn_xiexieshifu_wozhejiuqu = img_resoure + r"\btn\xiexieshifu_wozhejiuqu.png"
img_btn_npc_talk_close = img_resoure + r"\btn\npc_talk_close.png"
img_btn_npc_talk_close_640 = img_resoure + r"\btn\npc_talk_close_640.png"
img_btn_cangku_caozuo_640 = img_resoure + r"\btn\cangku_caozuo_640.png"
img_btn_wupincangku_640 = img_resoure + r"\btn\wupincangku_640.png"
img_btn_cangku_prop_640 = img_resoure + r"\btn\cangku_prop_640.png"
img_btn_cangku_close_640 = img_resoure + r"\btn\cangku_close_640.png"
img_btn_current_channel_640 = img_resoure + r"\btn\current_channel_640.png"
img_btn_current_channel1_640 = img_resoure + r"\btn\current_channel1_640.png"
img_btn_world_channel_640 = img_resoure + r"\btn\world_channel_640.png"
img_btn_world_channel1_640 = img_resoure + r"\btn\world_channel1_640.png"
img_btn_chat_input_area_640 = img_resoure + r"\btn\chat_input_area_640.png"
img_btn_chat_input_area1_640 = img_resoure + r"\btn\chat_input_area1_640.png"
img_btn_xingnang_640 = img_resoure + r"\btn\xingnang_640.png"
img_btn_xingnang1_640 = img_resoure + r"\btn\xingnang1_640.png"
img_btn_package_prop_640 = img_resoure + r"\btn\package_prop_640.png"
img_btn_package_prop1_640 = img_resoure + r"\btn\package_prop1_640.png"
img_btn_beibao_640 = img_resoure + r"\btn\beibao_640.png"
img_btn_beibao = img_resoure + r"\btn\beibao.png"
img_btn_cangku_640 = img_resoure + r"\btn\cangku_640.png"
img_btn_cangku_full_selected_1_640 = img_resoure + r"\btn\cangku_full_selected_1_640.png"
img_btn_cangku_full_selected_2_640 = img_resoure + r"\btn\cangku_full_selected_2_640.png"
img_btn_cangku_full_selected_3_640 = img_resoure + r"\btn\cangku_full_selected_3_640.png"
img_btn_cangku_full_selected_4_640 = img_resoure + r"\btn\cangku_full_selected_4_640.png"
img_btn_cangku_full_selected_5_640 = img_resoure + r"\btn\cangku_full_selected_5_640.png"
img_btn_cangku_full_selected_6_640 = img_resoure + r"\btn\cangku_full_selected_6_640.png"
img_btn_cangku_full_selected_7_640 = img_resoure + r"\btn\cangku_full_selected_7_640.png"
img_btn_cangku_full_selected_8_640 = img_resoure + r"\btn\cangku_full_selected_8_640.png"
img_btn_cangku_full_selected_9_640 = img_resoure + r"\btn\cangku_full_selected_9_640.png"
img_btn_cangku_full_selected_10_640 = img_resoure + r"\btn\cangku_full_selected_10_640.png"
img_btn_cangku_full_selected_11_640 = img_resoure + r"\btn\cangku_full_selected_11_640.png"
img_btn_cangku_full_selected_12_640 = img_resoure + r"\btn\cangku_full_selected_12_640.png"
img_btn_cangku_full_selected_13_640 = img_resoure + r"\btn\cangku_full_selected_13_640.png"
img_btn_cangku_full_selected_14_640 = img_resoure + r"\btn\cangku_full_selected_14_640.png"
img_btn_cangku_full_selected_15_640 = img_resoure + r"\btn\cangku_full_selected_15_640.png"
img_btn_cangku_full_selected_16_640 = img_resoure + r"\btn\cangku_full_selected_16_640.png"
img_btn_cangku_full_selected_17_640 = img_resoure + r"\btn\cangku_full_selected_17_640.png"
img_btn_cangku_full_selected_18_640 = img_resoure + r"\btn\cangku_full_selected_18_640.png"
img_btn_cangku_full_selected_19_640 = img_resoure + r"\btn\cangku_full_selected_19_640.png"
img_btn_cangku_full_selected_20_640 = img_resoure + r"\btn\cangku_full_selected_20_640.png"
img_btn_cangku_full_selected_21_640 = img_resoure + r"\btn\cangku_full_selected_21_640.png"
img_btn_cangku_full_selected_22_640 = img_resoure + r"\btn\cangku_full_selected_22_640.png"
img_btn_cangku_full_selected_23_640 = img_resoure + r"\btn\cangku_full_selected_23_640.png"
# img_btn_cangku_full_unselected_1_640 = img_resoure + r"\btn\cangku_full_unselected_1_640.png"
img_btn_cangku_usable_selected_1_640 = img_resoure + r"\btn\cangku_usable_selected_1_640.png"
img_btn_cangku_usable_selected_2_640 = img_resoure + r"\btn\cangku_usable_selected_2_640.png"
img_btn_cangku_usable_selected_3_640 = img_resoure + r"\btn\cangku_usable_selected_3_640.png"
img_btn_cangku_usable_selected_4_640 = img_resoure + r"\btn\cangku_usable_selected_4_640.png"
img_btn_cangku_usable_selected_5_640 = img_resoure + r"\btn\cangku_usable_selected_5_640.png"
img_btn_cangku_usable_selected_6_640 = img_resoure + r"\btn\cangku_usable_selected_6_640.png"
img_btn_cangku_usable_selected_7_640 = img_resoure + r"\btn\cangku_usable_selected_7_640.png"
img_btn_cangku_usable_selected_8_640 = img_resoure + r"\btn\cangku_usable_selected_8_640.png"
img_btn_cangku_usable_selected_9_640 = img_resoure + r"\btn\cangku_usable_selected_9_640.png"
img_btn_cangku_usable_selected_10_640 = img_resoure + r"\btn\cangku_usable_selected_10_640.png"
img_btn_cangku_usable_selected_11_640 = img_resoure + r"\btn\cangku_usable_selected_11_640.png"
img_btn_cangku_usable_selected_12_640 = img_resoure + r"\btn\cangku_usable_selected_12_640.png"
img_btn_cangku_usable_selected_13_640 = img_resoure + r"\btn\cangku_usable_selected_13_640.png"
img_btn_cangku_usable_selected_14_640 = img_resoure + r"\btn\cangku_usable_selected_14_640.png"
img_btn_cangku_usable_selected_15_640 = img_resoure + r"\btn\cangku_usable_selected_15_640.png"
img_btn_cangku_usable_selected_16_640 = img_resoure + r"\btn\cangku_usable_selected_16_640.png"
img_btn_cangku_usable_selected_17_640 = img_resoure + r"\btn\cangku_usable_selected_17_640.png"
img_btn_cangku_usable_selected_18_640 = img_resoure + r"\btn\cangku_usable_selected_18_640.png"
img_btn_cangku_usable_selected_19_640 = img_resoure + r"\btn\cangku_usable_selected_19_640.png"
img_btn_cangku_usable_selected_20_640 = img_resoure + r"\btn\cangku_usable_selected_20_640.png"
img_btn_cangku_usable_selected_21_640 = img_resoure + r"\btn\cangku_usable_selected_21_640.png"
img_btn_cangku_usable_selected_22_640 = img_resoure + r"\btn\cangku_usable_selected_22_640.png"
img_btn_cangku_usable_selected_23_640 = img_resoure + r"\btn\cangku_usable_selected_23_640.png"
img_btn_cangku_usable_unselected_1_640 = img_resoure + r"\btn\cangku_usable_unselected_1_640.png"
img_btn_cangku_usable_unselected_2_640 = img_resoure + r"\btn\cangku_usable_unselected_2_640.png"
img_btn_cangku_usable_unselected_3_640 = img_resoure + r"\btn\cangku_usable_unselected_3_640.png"
img_btn_cangku_usable_unselected_4_640 = img_resoure + r"\btn\cangku_usable_unselected_4_640.png"
img_btn_cangku_usable_unselected_5_640 = img_resoure + r"\btn\cangku_usable_unselected_5_640.png"
img_btn_cangku_usable_unselected_6_640 = img_resoure + r"\btn\cangku_usable_unselected_6_640.png"
img_btn_cangku_usable_unselected_7_640 = img_resoure + r"\btn\cangku_usable_unselected_7_640.png"
img_btn_cangku_usable_unselected_8_640 = img_resoure + r"\btn\cangku_usable_unselected_8_640.png"
img_btn_cangku_usable_unselected_9_640 = img_resoure + r"\btn\cangku_usable_unselected_9_640.png"
img_btn_cangku_usable_unselected_10_640 = img_resoure + r"\btn\cangku_usable_unselected_10_640.png"
img_btn_cangku_usable_unselected_11_640 = img_resoure + r"\btn\cangku_usable_unselected_11_640.png"
img_btn_cangku_usable_unselected_12_640 = img_resoure + r"\btn\cangku_usable_unselected_12_640.png"
img_btn_cangku_usable_unselected_13_640 = img_resoure + r"\btn\cangku_usable_unselected_13_640.png"
img_btn_cangku_usable_unselected_14_640 = img_resoure + r"\btn\cangku_usable_unselected_14_640.png"
img_btn_cangku_usable_unselected_15_640 = img_resoure + r"\btn\cangku_usable_unselected_15_640.png"
img_btn_cangku_usable_unselected_16_640 = img_resoure + r"\btn\cangku_usable_unselected_16_640.png"
img_btn_cangku_usable_unselected_17_640 = img_resoure + r"\btn\cangku_usable_unselected_17_640.png"
img_btn_cangku_usable_unselected_18_640 = img_resoure + r"\btn\cangku_usable_unselected_18_640.png"
img_btn_cangku_usable_unselected_19_640 = img_resoure + r"\btn\cangku_usable_unselected_19_640.png"
img_btn_cangku_usable_unselected_20_640 = img_resoure + r"\btn\cangku_usable_unselected_20_640.png"
img_btn_cangku_usable_unselected_21_640 = img_resoure + r"\btn\cangku_usable_unselected_21_640.png"
img_btn_cangku_usable_unselected_22_640 = img_resoure + r"\btn\cangku_usable_unselected_22_640.png"
img_btn_cangku_usable_unselected_23_640 = img_resoure + r"\btn\cangku_usable_unselected_23_640.png"
img_btn_cannot_save_this_640 = img_resoure + r"\btn\cannot_save_this_640.png"
img_btn_flag_loc = img_resoure + r"\btn\flag_loc.png"
img_btn_shide_woyaoqu = img_resoure + r"\btn\shide_woyaoqu.png"
img_btn_haode_wobangni = img_resoure + r"\btn\haode_wobangni.png"
img_btn_laozimeishijianwan = img_resoure + r"\btn\laozimeishijianwan.png"  # 老子没时间陪你玩
img_btn_wozhengzaizhandouzhong = img_resoure + r"\btn\wozhengzaizhandouzhong.png"  # 我正在战斗中，请勿打扰。
img_btn_reset_auto_round = img_resoure + r"\btn\reset_auto_round.png"
img_btn_cancel_auto_round = img_resoure + r"\btn\cancel_auto_round.png"
img_btn_cancel_zhanli = img_resoure + r"\btn\cancel_zhanli.png"
img_btn_luguohenishuogehua = img_resoure + r"\btn\luguohenishuogehua.png"  # 路过和你说个话
img_btn_wohaixiangzaizhezhuanzhuan = img_resoure + r"\btn\wohaixiangzaizhezhuanzhuan.png"  # 我还想在这转转
img_btn_wojintianmeidaiqian_gaitianlaizhaoni = img_resoure + r"\btn\wojintianmeidaiqian_gaitianlaizhaoni.png"  # 我今天没带钱，改天来找你。
img_btn_wosuibiankankan = img_resoure + r"\btn\wosuibiankankan.png"  # 我随便看看
img_btn_womenhouhuiyouqi = img_resoure + r"\btn\womenhouhuiyouqi.png"  # 青山不转，绿水长流，我们后会有期
img_btn_luanchiyao_hairenming = img_resoure + r"\btn\luanchiyao_hairenming.png"  # 乱吃药，害人命
img_btn_menpaichuanggaunhudong = img_resoure + r"\btn\menpaichuanggaunhudong.png"  # 欢迎参加门派闯关活动
img_btn_woshenmedoubuzuo = img_resoure + r"\btn\woshenmedoubuzuo.png"  # 我什么都不做
img_btn_hefangshenshengbuyaodanglu = img_resoure + r"\btn\hefangshenshengbuyaodanglu.png"  # 你是何方神圣  不要挡路
# 地图
img_map_search = img_resoure + r"\map\map_search.png"
img_map_search1 = img_resoure + r"\map\map_search1.png"
img_map_wayfinding_search = img_resoure + r"\map\map_wayfinding_search.png"
img_map_niumowang = img_resoure + r"\map\niumowang.png"
img_map_niumowang1 = img_resoure + r"\map\niumowang1.png"
img_map_chenmama = img_resoure + r"\map\chenmama.png"
img_map_chenmama1 = img_resoure + r"\map\chenmama1.png"
img_map_xiaolianxiang = img_resoure + r"\map\xiaolianxiang.png"
img_map_xiaolianxiang1 = img_resoure + r"\map\xiaolianxiang1.png"
img_map_xiaoxiyu = img_resoure + r"\map\xiaoxiyu.png"
img_map_xiaoxiyu1 = img_resoure + r"\map\xiaoxiyu1.png"
img_map_dubatian = img_resoure + r"\map\dubatian.png"
img_map_dubatian1 = img_resoure + r"\map\dubatian1.png"
img_map_yanruyu = img_resoure + r"\map\yanruyu.png"
img_map_yanruyu1 = img_resoure + r"\map\yanruyu1.png"
img_map_paizhongxian = img_resoure + r"\map\paizhongxian.png"
img_map_paizhongxian1 = img_resoure + r"\map\paizhongxian1.png"
img_map_huiming = img_resoure + r"\map\huiming.png"
img_map_huiming1 = img_resoure + r"\map\huiming1.png"
img_map_wangbutou = img_resoure + r"\map\wangbutou.png"
img_map_wangbutou1 = img_resoure + r"\map\wangbutou1.png"
img_map_wujuren = img_resoure + r"\map\wujuren.png"
img_map_wujuren1 = img_resoure + r"\map\wujuren1.png"
img_map_wangwu = img_resoure + r"\map\wangwu.png"
img_map_wangwu1 = img_resoure + r"\map\wangwu1.png"
img_map_fuzhuangdian_laoban = img_resoure + r"\map\fuzhuangdian_laoban.png"
img_map_fuzhuangdian_laoban1 = img_resoure + r"\map\fuzhuangdian_laoban1.png"
img_map_wuqidian_laoban = img_resoure + r"\map\wuqidian_laoban.png"
img_map_wuqidian_laoban1 = img_resoure + r"\map\wuqidian_laoban1.png"
img_map_shipindian_laoban = img_resoure + r"\map\shipindian_laoban.png"
img_map_shipindian_laoban1 = img_resoure + r"\map\shipindian_laoban1.png"
img_map_jielaiheshang = img_resoure + r"\map\jielaiheshang.png"
img_map_jielaiheshang1 = img_resoure + r"\map\jielaiheshang1.png"
img_map_lishimin = img_resoure + r"\map\lishimin.png"
img_map_lishimin1 = img_resoure + r"\map\lishimin1.png"
img_map_huijing = img_resoure + r"\map\huijing.png"
img_map_huijing1 = img_resoure + r"\map\huijing1.png"
img_map_fushoudian_laoban = img_resoure + r"\map\fushoudian_laoban.png"
img_map_fushoudian_laoban1 = img_resoure + r"\map\fushoudian_laoban1.png"
img_map_huibei = img_resoure + r"\map\huibei.png"
img_map_huibei1 = img_resoure + r"\map\huibei1.png"
img_map_chenyuanwai = img_resoure + r"\map\chenyuanwai.png"
img_map_chenyuanwai1 = img_resoure + r"\map\chenyuanwai1.png"
img_map_changan_zhenpin_shangren = img_resoure + r"\map\changan_zhenpin_shangren.png"
img_map_changan_zhenpin_shangren1 = img_resoure + r"\map\changan_zhenpin_shangren1.png"
img_map_yaodian_laoban = img_resoure + r"\map\yaodian_laoban.png"
img_map_yaodian_laoban1 = img_resoure + r"\map\yaodian_laoban1.png"
img_map_zahuodian_laoban = img_resoure + r"\map\zahuodian_laoban.png"
img_map_zahuodian_laoban1 = img_resoure + r"\map\zahuodian_laoban1.png"
img_map_lanhu = img_resoure + r"\map\lanhu.png"
img_map_lanhu1 = img_resoure + r"\map\lanhu1.png"
img_map_jiangdaquan = img_resoure + r"\map\jiangdaquan.png"
img_map_jiangdaquan1 = img_resoure + r"\map\jiangdaquan1.png"
img_map_chengfuren = img_resoure + r"\map\chengfuren.png"
img_map_chengfuren1 = img_resoure + r"\map\chengfuren1.png"
img_map_youfangchefu = img_resoure + r"\map\youfangchefu.png"
img_map_youfangchefu1 = img_resoure + r"\map\youfangchefu1.png"
img_map_yinfuren = img_resoure + r"\map\yinfuren.png"
img_map_yinfuren1 = img_resoure + r"\map\yinfuren1.png"
img_map_weizheng = img_resoure + r"\map\weizheng.png"
img_map_weizheng1 = img_resoure + r"\map\weizheng1.png"
img_map_xiaotaohong = img_resoure + r"\map\xiaotaohong.png"
img_map_xiaotaohong1 = img_resoure + r"\map\xiaotaohong1.png"
img_map_huashengsi_jieyinseng = img_resoure + r"\map\huashengsi_jieyinseng.png"
img_map_huashengsi_jieyinseng1 = img_resoure + r"\map\huashengsi_jieyinseng1.png"
img_map_huashengsi_huihai = img_resoure + r"\map\huashengsi_huihai.png"
img_map_huashengsi_huihai1 = img_resoure + r"\map\huashengsi_huihai1.png"
img_map_qinfuren = img_resoure + r"\map\qinfuren.png"
img_map_qinfuren1 = img_resoure + r"\map\qinfuren1.png"
img_map_luodaoren = img_resoure + r"\map\luodaoren.png"
img_map_luodaoren1 = img_resoure + r"\map\luodaoren1.png"
img_map_donghailongwang = img_resoure + r"\map\donghailongwang.png"
img_map_donghailongwang1 = img_resoure + r"\map\donghailongwang1.png"
img_map_yunlai_jiudian_laoban = img_resoure + r"\map\yunlai_jiudian_laoban.png"
img_map_yunlai_jiudian_laoban1 = img_resoure + r"\map\yunlai_jiudian_laoban1.png"
img_map_fengtiejiang = img_resoure + r"\map\fengtiejiang.png"
img_map_fengtiejiang1 = img_resoure + r"\map\fengtiejiang1.png"
img_map_yinchengxiang = img_resoure + r"\map\yinchengxiang.png"
img_map_yinchengxiang1 = img_resoure + r"\map\yinchengxiang1.png"
img_map_datielu = img_resoure + r"\map\datielu.png"
img_map_datielu1 = img_resoure + r"\map\datielu1.png"
img_map_yuantiangang = img_resoure + r"\map\yuantiangang.png"
img_map_yuantiangang1 = img_resoure + r"\map\yuantiangang1.png"
img_map_guanyinjiejie = img_resoure + r"\map\guanyinjiejie.png"
img_map_guanyinjiejie1 = img_resoure + r"\map\guanyinjiejie1.png"
img_map_guodage = img_resoure + r"\map\guodage.png"
img_map_guodage1 = img_resoure + r"\map\guodage1.png"
img_map_liudashen = img_resoure + r"\map\liudashen.png"
img_map_liudashen1 = img_resoure + r"\map\liudashen1.png"
img_map_difu = img_resoure + r"\map\difu.png"
img_map_zhongkui = img_resoure + r"\map\zhongkui.png"
img_map_zhongkui1 = img_resoure + r"\map\zhongkui1.png"
# npc
img_npc_dream_wizard = img_resoure + r"\npc\dream_wizard.png"
img_npc_xiaolingxiang_wall = img_resoure + r"\npc\xiaolingxiang_wall.png"
img_npc_xiaoxiyu_wall = img_resoure + r"\npc\xiaoxiyu_wall.png"
img_npc_yanruyu_wall = img_resoure + r"\npc\yanruyu_wall.png"
img_npc_huiming_wall = img_resoure + r"\npc\huiming_wall.png"
img_npc_wangbutou_wall = img_resoure + r"\npc\wangbutou_wall.png"
img_npc_wujuren_wall = img_resoure + r"\npc\wujuren_wall.png"
img_npc_wangwu_wall = img_resoure + r"\npc\wangwu_wall.png"
img_npc_fuzhuangdian_laoban_wall = img_resoure + r"\npc\fuzhuangdian_laoban_wall.png"
img_npc_wuqidian_zhanggui_wall = img_resoure + r"\npc\wuqidian_zhanggui_wall.png"
img_npc_jielaiheshang_wall = img_resoure + r"\npc\jielaiheshang_wall.png"
img_npc_lishimin_wall = img_resoure + r"\npc\lishimin_wall.png"
img_npc_weizheng_wall = img_resoure + r"\npc\weizheng_wall.png"
img_npc_chenmama_wall = img_resoure + r"\npc\chenmama_wall.png"
img_npc_huijing_wall = img_resoure + r"\npc\huijing_wall.png"
img_npc_fushoudian_laoban_wall = img_resoure + r"\npc\fushoudian_laoban_wall.png"
img_npc_zahuodian_laoban_wall = img_resoure + r"\npc\zahuodian_laoban_wall.png"
img_npc_huibei_wall = img_resoure + r"\npc\huibei_wall.png"
img_npc_chenyuanwai_wall = img_resoure + r"\npc\chenyuanwai_wall.png"
img_npc_yaodian_laoban_wall = img_resoure + r"\npc\yaodian_laoban_wall.png"
img_npc_shipindian_laoban_wall = img_resoure + r"\npc\shipindian_laoban_wall.png"
img_npc_lanhu_wall = img_resoure + r"\npc\lanhu_wall.png"
img_npc_chengfuren_wall = img_resoure + r"\npc\chengfuren_wall.png"
img_npc_niumowang_wall = img_resoure + r"\npc\niumowang_wall.png"
img_npc_wuqidian_laoban_wall = img_resoure + r"\npc\wuqidian_laoban_wall.png"
img_npc_dubatian_wall = img_resoure + r"\npc\dubatian_wall.png"
img_npc_liufujiang_wall = img_resoure + r"\npc\liufujiang_wall.png"
img_npc_yinchengxiang_wall = img_resoure + r"\npc\yinchengxiang_wall.png"
img_npc_huihai_wall = img_resoure + r"\npc\huihai_wall.png"
img_npc_qinfuren_wall = img_resoure + r"\npc\qinfuren_wall.png"
img_npc_zhanglaocai_wall = img_resoure + r"\npc\zhanglaocai_wall.png"
img_npc_donghailongwang_wall = img_resoure + r"\npc\donghailongwang_wall.png"
img_npc_yunlai_jiudian_laoban_wall = img_resoure + r"\npc\yunlai_jiudian_laoban_wall.png"
img_npc_longgong_chuansongren_wall = img_resoure + r"\npc\longgong_chuansongren_wall.png"
img_npc_yinluxiaoyao_wall = img_resoure + r"\npc\yinluxiaoyao_wall.png"
img_npc_fengtiejiang_wall = img_resoure + r"\npc\fengtiejiang_wall.png"
img_npc_yuantiangang_wall = img_resoure + r"\npc\yuantiangang_wall.png"
img_npc_guanyinjiejie_wall = img_resoure + r"\npc\guanyinjiejie_wall.png"
img_npc_putuoshan_chuansongren_wall = img_resoure + r"\npc\putuoshan_chuansongren_wall.png"
img_npc_changan_cangku = img_resoure + r"\npc\changan_cangku_640.png"
img_npc_changan_cangku1 = img_resoure + r"\npc\changan_cangku1_640.png"
img_npc_changan_cangku2 = img_resoure + r"\npc\changan_cangku2_640.png"
img_npc_changan_cangku3 = img_resoure + r"\npc\changan_cangku3_640.png"
img_npc_changan_cangku4 = img_resoure + r"\npc\changan_cangku4_640.png"
img_npc_changan_yizhan_laoban_dir = os.path.join(os.getcwd(), "object", "npc", "yizhanlaoban")
# 场景标识
img_scene_changancheng = img_resoure + r"\scene\changancheng.png"
img_scene_changan_huichuntang = img_resoure + r"\scene\changan_huichuntang.png"
img_scene_changan_jinxiu_shipindian = img_resoure + r"\scene\changan_jinxiu_shipindian.png"
img_scene_changan_nanbei_zahuodian = img_resoure + r"\scene\changan_nanbei_zahuodian.png"
img_scene_changan_pingan_fushoudain = img_resoure + r"\scene\changan_pingan_fushoudain.png"
img_scene_changan_wansheng_wuqidian = img_resoure + r"\scene\changan_wansheng_wuqidian.png"
img_scene_changan_zhangji_buzhuang = img_resoure + r"\scene\changan_zhangji_buzhuang.png"
img_scene_changan_jinluandian = img_resoure + r"\scene\changan_jinluandian.png"
img_scene_changan_liuxiangge = img_resoure + r"\scene\changan_liuxiangge.png"
img_scene_changan_changlebo_xifang = img_resoure + r"\scene\changan_changlebo_xifang.png"
img_scene_changan_qinfuneishi = img_resoure + r"\scene\changan_qinfuneishi.png"
img_scene_changan_guozijian_shuku = img_resoure + r"\scene\guozijian_shuku.png"
img_scene_changan_shuxiangzhai = img_resoure + r"\scene\shuxiangzhai.png"
img_scene_changan_fengji_tiepu = img_resoure + r"\scene\fengji_tiepu.png"
img_scene_changan_chengxiangfu = img_resoure + r"\scene\chengxiangfu.png"
img_scene_changan_zhenyuan_wuguan = img_resoure + r"\scene\zhenyuan_wuguan.png"
img_scene_changan_lianxiangxuan = img_resoure + r"\scene\lianxiangxuan.png"
img_scene_changan_xiyuxuan = img_resoure + r"\scene\xiyuxuan.png"
img_scene_changan_yizhanlaoban = img_resoure + r"\scene\yizhanlaoban.png"
img_scene_datangguanfu = img_resoure + r"\scene\datangguanfu.png"
img_scene_huashengsi = img_resoure + r"\scene\huashengsi.png"
img_scene_huashengsi_guanghuadian = img_resoure + r"\scene\huashengsi_guanghuadian.png"
img_scene_mowangju = img_resoure + r"\scene\mowangju.png"  # 魔王居  是牛魔王内殿
img_scene_mowangzhai = img_resoure + r"\scene\mowangzhai.png"  # 魔王寨回城的地方
img_scene_shuijinggong = img_resoure + r"\scene\shuijinggong.png"
img_scene_longgong = img_resoure + r"\scene\longgong.png"
img_scene_chaoyindong = img_resoure + r"\scene\chaoyindong.png"
img_scene_putuoshan = img_resoure + r"\scene\putuoshan.png"
img_scene_datangguojing = img_resoure + r"\scene\datangguojing.png"
img_scene_datangjingwai = img_resoure + r"\scene\datangjingwai.png"
img_scene_difu_rukou = img_resoure + r"\scene\difu_rukou.png"
img_scene_difu = img_resoure + r"\scene\difu.png"
img_scene_zhongkui = img_resoure + r"\scene\zhongkui.png"
img_scene_zhuziguo = img_resoure + r"\scene\zhuziguo.png"
img_scene_changshoucun = img_resoure + r"\scene\changshoucun.png"
img_scene_aolaiguo = img_resoure + r"\scene\aolaiguo.png"
img_scene_xiliangnvguo = img_resoure + r"\scene\xiliangnvguo.png"
img_scene_baoxiangguo = img_resoure + r"\scene\baoxiangguo.png"
img_scene_jianyecheng = img_resoure + r"\scene\jianyecheng.png"
img_scene_jiangnanyewai = img_resoure + r"\scene\jiangnanyewai.png"
img_scene_zhuziguo_datangjingwai = img_resoure + r"\scene\zhuziguo_datangjingwai.png"
img_scene_putuojieyinxiannv = img_resoure + r"\scene\putuojieyinxiannv.png"
img_scene_datangguojing_datangjingwai_rukou = img_resoure + r"\scene\datangguojing_datangjingwai_rukou.png"
img_scene_wuzhuangguan = img_resoure + r"\scene\wuzhuangguan.png"
img_scene_datangjingwai_wuzhuangguan_rukou = img_resoure + r"\scene\datangjingwai_wuzhuangguan_rukou.png"
img_scene_nvercun = img_resoure + r"\scene\nvercun.png"
# 战斗
img_fight_do_hero_action = img_resoure + r"\fight\do_hero_action.png"
img_fight_do_peg_action = img_resoure + r"\fight\do_peg_action.png"
img_fighting = img_resoure + r"\fight\fighting.png"
img_fight_health_100 = img_resoure + r"\fight\health_100.png"
img_fight_health_95 = img_resoure + r"\fight\health_95.png"
img_fight_health_90 = img_resoure + r"\fight\health_90.png"
img_fight_health_85 = img_resoure + r"\fight\health_85.png"
img_fight_health_80 = img_resoure + r"\fight\health_80.png"
img_fight_health_75 = img_resoure + r"\fight\health_75.png"
img_fight_health_70 = img_resoure + r"\fight\health_70.png"
img_fight_health_65 = img_resoure + r"\fight\health_65.png"
img_fight_health_60 = img_resoure + r"\fight\health_60.png"
img_fight_health_55 = img_resoure + r"\fight\health_55.png"
img_fight_health_50 = img_resoure + r"\fight\health_50.png"
img_fight_mana_100 = img_resoure + r"\fight\mana_100.png"
img_fight_mana_95 = img_resoure + r"\fight\mana_95.png"
img_fight_mana_90 = img_resoure + r"\fight\mana_90.png"
img_fight_mana_85 = img_resoure + r"\fight\mana_85.png"
img_fight_mana_80 = img_resoure + r"\fight\mana_80.png"
img_fight_mana_75 = img_resoure + r"\fight\mana_75.png"
img_fight_mana_70 = img_resoure + r"\fight\mana_70.png"
img_fight_mana_65 = img_resoure + r"\fight\mana_65.png"
img_fight_mana_60 = img_resoure + r"\fight\mana_60.png"
img_fight_mana_55 = img_resoure + r"\fight\mana_55.png"
img_fight_mana_50 = img_resoure + r"\fight\mana_50.png"
img_fight_auto = img_resoure + r"\fight\auto.png"
img_fight_fourman_title_gray_107_255_0 = img_resoure + r"\fight\fourman_title_gray_107_255_0.png"
img_fight_fourman_tips = img_resoure + r"\fight\fourman_tips.png"
img_fight_auto_round30 = img_resoure + r"\fight\auto_round30.png"
img_fight_zanli_title = img_resoure + r"\fight\zanli_title.png"
# 光标技能
img_cursors_skill_huoxue = img_resoure + r"\cursors_skill_threshold\huoxue_127_255_0.png"  # 化生寺活血技能
img_cursors_skill_langyong = img_resoure + r"\cursors_skill_threshold\langyong_127_255_0.png"  # 凌波城浪涌
img_cursors_skill_lieshi = img_resoure + r"\cursors_skill_threshold\lieshi_127_255_0.png"  # 凌波城裂石
img_cursors_skill_tianbengdilie = img_resoure + r"\cursors_skill_threshold\tianbengdilie_127_255_0.png"  # 凌波城天崩地裂
# 交易
img_trade_trading_640 = img_resoure + r"\trade\trading_640.png"
img_trade_comfirm_640 = img_resoure + r"\trade\comfirm_640.png"
img_trade_empty_good_640 = img_resoure + r"\trade\empty_good_640.png"
img_trade_empty_package_640 = img_resoure + r"\trade\empty_package_640.png"
img_trade_my_pay_640 = img_resoure + r"\trade\my_pay_640.png"
img_trade_btn_my_comfirm_640 = img_resoure + r"\trade\btn_my_comfirm_640.png"
img_trade_btn_trade_gray_640 = img_resoure + r"\trade\btn_trade_gray_640.png"
img_trade_btn_trade_light_640 = img_resoure + r"\trade\btn_trade_light_640.png"
img_trade_btn_trade_cancel_640 = img_resoure + r"\trade\btn_trade_cancel_640.png"
img_trade_cangku_full_gray_107_255_0x640 = img_resoure + r"\trade\cangku_full_gray_107_255_0x640.png"


img_trade_goods_guangmangshi_640 = img_resoure + r"\trade\goods\guangmangshi_640.png"
img_trade_goods_taiyangshi_640 = img_resoure + r"\trade\goods\taiyangshi_640.png"
img_trade_goods_yueliangshi_640 = img_resoure + r"\trade\goods\yueliangshi_640.png"
img_trade_goods_heibaoshi_640 = img_resoure + r"\trade\goods\heibaoshi_640.png"
img_trade_goods_hongmanao_640 = img_resoure + r"\trade\goods\hongmanao_640.png"
img_trade_goods_shelizi_640 = img_resoure + r"\trade\goods\shelizi_640.png"
img_trade_goods_xinghuishi_640 = img_resoure + r"\trade\goods\xinghuishi_640.png"

img_goods_list = (
    img_trade_goods_guangmangshi_640,
    img_trade_goods_taiyangshi_640,
    img_trade_goods_yueliangshi_640,
    img_trade_goods_heibaoshi_640,
    img_trade_goods_hongmanao_640,
    img_trade_goods_shelizi_640,
    img_trade_goods_xinghuishi_640
)

img_trade_baoshi_lv1_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv1_gray_107_255_0x640.png"
img_trade_baoshi_lv2_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv2_gray_107_255_0x640.png"
img_trade_baoshi_lv3_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv3_gray_107_255_0x640.png"
img_trade_baoshi_lv4_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv4_gray_107_255_0x640.png"
img_trade_baoshi_lv5_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv5_gray_107_255_0x640.png"
img_trade_baoshi_lv6_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv6_gray_107_255_0x640.png"
img_trade_baoshi_lv7_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv7_gray_107_255_0x640.png"
img_trade_baoshi_lv8_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv8_gray_107_255_0x640.png"
img_trade_baoshi_lv9_gray_107_255_0x640 = img_resoure + r"\trade\baoshi_lv9_gray_107_255_0x640.png"

img_baoshi_lv_list = (
    img_trade_baoshi_lv1_gray_107_255_0x640,
    img_trade_baoshi_lv2_gray_107_255_0x640,
    img_trade_baoshi_lv3_gray_107_255_0x640,
    img_trade_baoshi_lv4_gray_107_255_0x640,
    img_trade_baoshi_lv5_gray_107_255_0x640,
    img_trade_baoshi_lv6_gray_107_255_0x640,
    img_trade_baoshi_lv7_gray_107_255_0x640,
    img_trade_baoshi_lv8_gray_107_255_0x640,
    img_trade_baoshi_lv9_gray_107_255_0x640
)

# 弹窗
img_popup_change_password_640 = img_resoure + r"\popup\change_password_640.png"
img_popup_tejia_jingduo_640 = img_resoure + r"\popup\tejia_jingduo_640.png"
img_popup_qianlichuanyin_640 = img_resoure + r"\popup\qianlichuanyin_640.png"

# 捉鬼
img_ghost_npc_datangjingwai = img_resoure + r"\ghost\npc_datangjingwai.png"
img_ghost_npc_putuoshan = img_resoure + r"\ghost\npc_putuoshan.png"
img_ghost_npc_jiangnanyewai = img_resoure + r"\ghost\npc_jiangnanyewai.png"
img_ghost_npc_baoxiangguo = img_resoure + r"\ghost\npc_baoxiangguo.png"
img_ghost_npc_jianyecheng = img_resoure + r"\ghost\npc_jianyecheng.png"
img_ghost_npc_wuzhuangguan = img_resoure + r"\ghost\npc_wuzhuangguan.png"
img_ghost_npc_zhuziguo = img_resoure + r"\ghost\npc_zhuziguo.png"
img_ghost_npc_aolaiguo = img_resoure + r"\ghost\npc_aolaiguo.png"
img_ghost_npc_xiliangnvguo = img_resoure + r"\ghost\npc_xiliangnvguo.png"
img_ghost_npc_changshoucun = img_resoure + r"\ghost\npc_changshoucun.png"
img_ghost_npc_nvercun = img_resoure + r"\ghost\npc_nvercun.png"
img_ghost_npc_overlap_page = img_resoure + r"\ghost\npc_overlap_page.png"
img_ghost_task_track_gray_107_255_0 = img_resoure + r"\ghost\task_track_gray_107_255_0.png"
img_ghost_task_zhuogui_gray_107_255_0 = img_resoure + r"\ghost\task_zhuogui_gray_107_255_0.png"
img_ghost_task_comma = img_resoure + r"\ghost\task_comma.png"
img_ghost_task_juhao_gray_107_255_0 = img_resoure + r"\ghost\task_juhao_gray_107_255_0.png"
img_ghost_task_fujin = img_resoure + r"\ghost\task_fujin.png"  # 没用天眼通显示 (x,y)附近
img_ghost_task_fujin_gray_107_255_0 = img_resoure + r"\ghost\task_fujin_gray_107_255_0.png"  # 没用天眼通显示 (x,y)附近
img_ghost_task_chu = img_resoure + r"\ghost\task_chu.png"  # 开天眼通后显示具体坐标 (x,y)处
img_ghost_task_chu_gray_107_255_0 = img_resoure + r"\ghost\task_chu_gray_107_255_0.png"  # 开天眼通后显示具体坐标 (x,y)处
img_ghost_task_zhua = img_resoure + r"\ghost\task_zhua.png"
img_ghost_task_changshoucun = img_resoure + r"\ghost\task_changshoucun.png"
img_ghost_task_aolaiguo = img_resoure + r"\ghost\task_aolaiguo.png"
img_ghost_task_jiangnanyewai = img_resoure + r"\ghost\task_jiangnanyewai.png"
img_ghost_task_zhuziguo = img_resoure + r"\ghost\task_zhuziguo.png"
img_ghost_task_nvercun = img_resoure + r"\ghost\task_nvercun.png"
img_ghost_task_baoxiangguo = img_resoure + r"\ghost\task_baoxiangguo.png"
img_ghost_task_jianyecheng = img_resoure + r"\ghost\task_jianyecheng.png"
img_ghost_task_datangjingwai = img_resoure + r"\ghost\task_datangjingwai.png"
img_ghost_task_wuzhuangguan = img_resoure + r"\ghost\task_wuzhuangguan.png"
img_ghost_task_xiliangnvguo = img_resoure + r"\ghost\task_xiliangnvguo.png"
img_ghost_task_putuoshan = img_resoure + r"\ghost\task_putuoshan.png"
img_ghost_task_60_gray_107_255_0 = img_resoure + r"\ghost\task_60_gray_107_255_0.png"
img_ghost_task_62_gray_107_255_0 = img_resoure + r"\ghost\task_62_gray_107_255_0.png"
img_ghost_task_10_gray_107_255_0 = img_resoure + r"\ghost\task_10_gray_107_255_0.png"
img_ghost_task_1_gray_107_255_0 = img_resoure + r"\ghost\task_1_gray_107_255_0.png"
img_ghost_task_6_gray_107_255_0 = img_resoure + r"\ghost\task_6_gray_107_255_0.png"
img_ghost_task_9_gray_107_255_0 = img_resoure + r"\ghost\task_9_gray_107_255_0.png"
img_ghost_loc_100_gray_107_255_0 = img_resoure + r"\ghost\loc_100_gray_107_255_0.png"
img_ghost_zhongkui_wayfinding_stop = img_resoure + r"\ghost\zhongkui_wayfinding_stop.png"
img_ghost_npc_chutao = img_resoure + r"\ghost\npc_chutao.png"
img_ghost_feixingfu_xiliangnvguo = img_resoure + r"\ghost\feixingfu_xiliangnvguo.png"
img_ghost_feixingfu_baoxiangguo = img_resoure + r"\ghost\feixingfu_baoxiangguo.png"
img_ghost_feixingfu_jianyecheng = img_resoure + r"\ghost\feixingfu_jianyecheng.png"
img_ghost_feixingfu_changshoucun = img_resoure + r"\ghost\feixingfu_changshoucun.png"
img_ghost_feixingfu_aolaiguo = img_resoure + r"\ghost\feixingfu_aolaiguo.png"
img_ghost_feixingfu_zhuziguo = img_resoure + r"\ghost\feixingfu_zhuziguo.png"
img_ghost_location_comma = img_resoure + r"\ghost\location_comma.png"
img_ghost_location_left = img_resoure + r"\ghost\location_left.png"
img_ghost_location_right = img_resoure + r"\ghost\location_right.png"
img_ghost_ctrl_zhongkui_gray_107_255_0 = img_resoure + r"\ghost\ctrl_zhongkui_gray_107_255_0.png"
# 鼠标漂移
# cursor_x = 12  # 鼠标截图坐标偏差
# cursor_y = 8  # 鼠标截图坐标偏差
# cursor_x = 8  # 鼠标截图坐标偏差
# cursor_y = 9  # 鼠标截图坐标偏差


class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int)]


astarDLL = ctypes.CDLL("./astaralgo.dll")
astarDLL.astar.restype = POINT


# class Node:
#     """A node class for A* Pathfinding"""
#
#     def __init__(self, x, y, g, parent=None):
#         self.parent = parent
#         self.x = x
#         self.y = y
#         self.g = g
#         self.s = 0
#
#     def update_score(self, endx, endy):
#         h = self.heuristic(endx, endy)
#         self.s = self.g + h
#
#     def heuristic(self, destx, desty):
#         # return pow(abs(destx - self.x), 2) + pow(abs((desty - self.y)), 2)
#         return abs(destx - self.x) + abs(desty - self.y)
#
#     def get_children(self, maze, maze_x_len, maze_y_len):
#         ret = list()
#         TILE_COST = 1
#         # if self.x > 0:
#         #     ret.append(Node(self.x - 1, self.y, self.g + TILE_COST, self))
#         # if self.y > 0:
#         #     ret.append(Node(self.x, self.y - 1, self.g + TILE_COST, self))
#         # if self.x < len(maze) - 1:
#         #     ret.append(Node(self.x + 1, self.y, self.g + TILE_COST, self))
#         # if self.y < len(maze[0]) - 1:
#         #     ret.append(Node(self.x, self.y + 1, self.g + TILE_COST, self))
#
#         # for new_position in [(0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (-1, 1), (1, -1), (1, 1)]:  # Adjacent squares
#         #     # Get node position
#         #     node_position = (self.x + new_position[0], self.y + new_position[1])
#         #     # Make sure within range
#         #     if node_position[0] > (len(maze) - 1) or node_position[0] < 0 or node_position[1] > (len(maze[len(maze)-1]) - 1) or node_position[1] < 0:
#         #         continue
#         #     ret.append(Node(node_position[0], node_position[1], self.g + TILE_COST, self))
#
#         for (x, y) in [(0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (-1, 1), (1, -1), (1, 1)]:  # Adjacent squares
#             # Get node position
#             node_x, node_y = self.x + x, self.y + y
#             # Make sure within range
#             if node_x > (maze_x_len - 1) or node_x < 0 or node_y > (maze_y_len - 1) or node_y < 0:
#                 continue
#             # Make sure walkable terrain
#             y_list = maze.get(node_x)
#             if y_list:
#                 for item in y_list:
#                     if item[0] <= node_y <= item[1]:
#                         ret.append(Node(node_x, node_y, self.g + TILE_COST, self))
#                         break
#         return ret
#
#     def __eq__(self, other):
#         return self.x == other.x and self.y == other.y
#
#
# def astar(startx, starty, endx, endy, maze, maze_x_len, maze_y_len):
#     """Returns a list of tuples as a path from the given start to the given end in the given maze"""
#
#     # Create start and end node
#     node = Node(startx, starty, 0)
#     node.update_score(endx, endy)
#     # Initialize both open and closed list
#     frontier = list()
#     open_list = [node]
#     logger.info("A*寻路")
#     ct = time.time()
#     while True:
#         if node.x == endx and node.y == endy:
#             # return True
#             path = []
#             while node is not None:
#                 path.append([node.x, node.y])
#                 node = node.parent
#             return path[::-1]  # Return reversed path
#         # if time.time() - ct > 2.0:
#         #     logger.info("寻路超时")
#             # return None
#         children = node.get_children(maze, maze_x_len, maze_y_len)
#         # Loop through children
#         for child in children:
#             found = -1
#             for i in range(len(open_list)):
#                 if open_list[i] == child:
#                     found = i
#                     # break
#             if found != -1:
#                 if open_list[found].s < child.s:
#                     open_list[found].g = child.g
#                     open_list[found].parent = child.parent
#                     open_list[found].update_score(endx, endy)
#             else:
#                 child.update_score(endx, endy)
#                 frontier.append(child)
#                 open_list.append(child)
#         if len(frontier) == 0:
#             return None
#         frontier.sort(key=lambda x: x.s, reverse=True)
#         node = frontier.pop()


# class MyLabel:
#     def __init__(self):
#         self.label = tkinter.Label(text="开始", font=('黑体', '14'), fg='red', bg='white', width=42)
#         self.label.master.overrideredirect(True)
#         self.label.master.geometry("+%s+495" % (screensize[0] - 430))
#         self.label.master.lift()
#         self.label.master.wm_attributes("-topmost", True)
#         self.label.master.wm_attributes("-disabled", True)
#         self.label.master.wm_attributes("-transparentcolor", "white")
#
#         hWindow = pywintypes.HANDLE(int(self.label.master.frame(), 16))
#         # http://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
#         # The WS_EX_TRANSPARENT flag makes events (like mouse clicks) fall through the window.
#         exStyle = win32con.WS_EX_COMPOSITED | win32con.WS_EX_LAYERED | win32con.WS_EX_NOACTIVATE | win32con.WS_EX_TOPMOST | win32con.WS_EX_TRANSPARENT
#         win32api.SetWindowLong(hWindow, win32con.GWL_EXSTYLE, exStyle)
#
#         self.label.pack()
#         self.label.update()
#
#         self.text_list = ["1", "2", "3"]
#
#     def update_text(self, content):
#         self.text_list.pop(0)
#         self.text_list.append(content)
#         text = '\n'.join(self.text_list)
#         self.label.config(text=text)
#         self.label.update()
#
#
# m_label = MyLabel()

ghost_list = (
    "戌时二刻醉死鬼"
)


def serial_write(data):
    # 向串口写数据，一般用byte类型数据
    global ser
    try:
        ser.write(data)
    except serial.serialutil.SerialException as e:
        logger.error(e)
        time.sleep(12)
        ser = serial_connect()
        # clear_serial_data()
        ser.write(data)
    # time.sleep(0.1)
    ser.readline()
    # ser.flush()  # it is buffering. required to get the data out *now*
    # data = ser.readline()
    # logger.info(data)
    # if data == b'reset\r\n':
    #     logger.info("设备重启，等待12秒")
    #     time.sleep(12)


def serial_read():
    global ser
    data = ""
    try:
        data = ser.readline()
    except serial.serialutil.SerialException as e:
        logger.error(e)
        # time.sleep(12)
        # ser = serial_connect()
        # clear_serial_data()
        # data = ser.readline()
    return data


def serial_click(x, y):
    serial_write(CLICK_SYMBOL % (x, y))


def serial_click_cur():
    serial_write(CLICK_CURRENT_SYMBOL)


def serial_right_click_cur():
    serial_write(RIGHT_CLICK_CURRENT_SYMBOL)


def serial_right_click(x, y):
    serial_write(RIGHT_CLICK_SYMBOL % (x, y))


def serial_move(x, y):
    serial_write(MS_MOVE_SYMBOL % (x, y))
    # time.sleep(0.1)


def serial_move_human(x, y, mode=1):
    cx, cy = autoit.mouse_get_pos()
    serial_write(MS_MOVE_HUMAN_SYMBOL % (cx, cy, x, y, mode))


def serial_scroll(n, x=0, y=-1):
    # 正常是负数向下，正数向上
    # 但mh是正数向下，负数向上, 而且滚动幅度每次只有1，不受x和y的值影响
    # for i in range(n):
    #     print(i)
    #     ser.write(MS_SCROLL_SYMBOL % (x, y, n))
    #     time.sleep(0.1)
    #     ser.readline()
    #     time.sleep(0.1)
    ser.write(MS_SCROLL_SYMBOL % (x, y, n))
    serial_read()


def serial_mouse_press(n):
    ser.write(MS_PRESS_LONG % n)
    time.sleep(n + 1)


def serial_drag_n_drop(x0, y0, x1, y1):
    serial_write(MS_DRAG_AND_DROP_SYMBOL % (x0, y0, x1, y1))


def serial_reset():
    global ser
    logger.info("重启设备，避免内存泄漏")
    ser.write(RESET_SYMBOL)
    # ser.readline()
    time.sleep(12)
    ser = serial_connect()
    # clear_serial_data()
    init_abs_mouse()


def clear_serial_data():
    for i in range(6):
        ser.write(b'\n')  # 可能是和别串口数据有冲突，刷掉脏数据
        # serial_write(b'\n')  # 可能是和别串口数据有冲突，刷掉脏数据
    time.sleep(0.1)


def input_enter():
    serial_write(KEY_PRESS % b"ENTER")


def input_tab():
    serial_write(KEY_PRESS % b"TAB")


def input_winkey():
    serial_write(KEY_PRESS % b"WIN")


def input_delete():
    serial_write(KEY_PRESS % b"DELETE")


def input_backspace(n):
    serial_write(KEY_PRESS % (b"BACKSPACE_%d" % n))


def input_page_up():
    serial_write(KEY_PRESS % b'PAGE_UP')


def input_page_down():
    serial_write(KEY_PRESS % b'PAGE_DOWN')


def input_up_arrow():
    serial_write(KEY_PRESS % b'UP_ARROW')


def input_down_arrow():
    serial_write(KEY_PRESS % b'DOWN_ARROW')


def input_alt_a():
    serial_write(KEY_ALT_xxx % b'a')


def input_alt_d():
    serial_write(KEY_ALT_xxx % b'd')


def input_alt_e():
    serial_write(KEY_ALT_xxx % b'e')


def input_alt_g():
    serial_write(KEY_ALT_xxx % b'g')


def input_alt_h():
    # 快捷键ALT+H来快速闭屏摆摊
    serial_write(KEY_ALT_xxx % b'h')


def input_alt_q():
    serial_write(KEY_ALT_xxx % b'q')


def input_alt_s():
    serial_write(KEY_ALT_xxx % b's')


def input_alt_v():
    serial_write(KEY_ALT_xxx % b'v')


def input_alt_greater():
    serial_write(KEY_ALT_xxx % b'>')


def input_alt_less():
    serial_write(KEY_ALT_xxx % b'<')


def input_alt_equal():
    serial_write(KEY_ALT_xxx % b'=')


def input_alt_dot():
    serial_write(KEY_ALT_xxx % b'.')


def input_alt_comma():
    serial_write(KEY_ALT_xxx % b',')


def input_f1():
    serial_write(KEY_PRESS % b"F1")


def input_f2():
    serial_write(KEY_PRESS % b"F2")


def input_f3():
    serial_write(KEY_PRESS % b"F3")


def input_f4():
    serial_write(KEY_PRESS % b"F4")


def input_f5():
    serial_write(KEY_PRESS % b"F5")


def input_f6():
    serial_write(KEY_PRESS % b"F6")


def input_f7():
    serial_write(KEY_PRESS % b"F7")


def input_f8():
    serial_write(KEY_PRESS % b"F8")


def input_f9():
    serial_write(KEY_PRESS % b"F9")


def input_alt_f4():
    serial_write(KEY_ALT_fff % b'F4')  # 快捷键ALT+H来快速闭屏摆摊


def input_ctrl_a():
    serial_write(KEY_CTRL_xxx % b'a')


def input_ctrl_v():
    serial_write(KEY_CTRL_xxx % b'v')


def input_ctrl_tab():
    serial_write(KEY_CTRL_fff % b'TAB')


def input_from_clipboard(name):
    logger.info("input_from_clipboard")
    set_clipboard(name)
    time.sleep(0.1)
    input_ctrl_v()
    time.sleep(0.1)
    clear_clipboard()


def read_config(file):
    config = configparser.RawConfigParser()
    config.read(file, encoding='utf-8')
    dd = dict()
    for i in config:
        d = dict(config[i])
        if d:
            dd[i] = d
    return dd


def read_json_config(file):
    with open(file, 'r', encoding='utf-8') as f:
        config = json.load(f)
    return config


def str_in_str(str_list, string):
    for s in str_list:
        if s in string:
            return True
    return False


def get_arduino_com_port():
    ports = serial.tools.list_ports.comports()
    for port, desc, hwid in ports:
        if "Arduino Leonardo" in desc:
            logger.info(port)
            return port


def init_abs_mouse():
    serial_write(b'absmouse_init:%d,%d\n' % screensize)


def wind_int(n):
    return random.randint(1, n) * random.choice((-1, 1))


def istrue():
    return random.choice((True, False))


def delay_time():
    return random.choice([0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.4, 0.4, 0.5, 0.6])


def random_str(length):
    return ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(length))


def get_win_bbox():
    return autoit.win_get_pos(game_title) if autoit.win_exists(game_title) else (0, 0, 10, 10)


def screen_bbox():
    win_bbox = get_win_bbox()
    return win_bbox[0], win_bbox[1], win_bbox[2], win_bbox[3]


def save_opencv_image(image, name=""):
    screencap_name = r'%s_Screenshot_%s_%s.png' % (name, datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    file = os.path.join(os.getcwd(), "screenshot", screencap_name)
    cv2.imwrite(file, cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB))


def screenshot_error(name=""):
    im = ImageGrab.grab(screen_bbox())
    screencap_name = r'\screenshot\%s_Screenshot_%s_%s.png' % (name, datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    im.save(os.getcwd() + screencap_name, format="PNG")


def screenshot_debug():
    im = ImageGrab.grab(screen_bbox())
    screencap_name = r'\debug_img\debug%s_%s.png' % (datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    im.save(os.getcwd() + screencap_name, format="PNG")


def screenshot_fourman():
    im = ImageGrab.grab(screen_bbox())
    screencap_name = r'\four_man_source\fourman_%s_%s.png' % (datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    im.save(os.getcwd() + screencap_name, format="PNG")


def screen_easyocr_grab(bbox=None):
    # bbox = (rect.left, rect.top, rect.right, rect.bottom)
    # 相当于两个坐标点：(rect.left, rect.top), (rect.right, rect.bottom)
    # jpg格式识别效果好一点，而且保存为本地图片的识别效果好一点
    im = ImageGrab.grab(bbox) if bbox else ImageGrab.grab()
    screencap_name = 'Screenshot_%s_%s.jpg' % (datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    screen_image = os.path.join(org_screen_path, screencap_name)
    im.save(screen_image, format="JPEG")
    img_byte_arr = io.BytesIO()
    im.save(img_byte_arr, format='PNG')
    return img_byte_arr.getvalue()


def screen_opencv_grab(bbox=None):
    # bbox = (rect.left, rect.top, rect.right, rect.bottom)
    # 相当于两个坐标点：(rect.left, rect.top), (rect.right, rect.bottom)
    # jpg格式识别效果好一点，而且保存为本地图片的识别效果好一点
    im = ImageGrab.grab(bbox) if bbox else ImageGrab.grab()
    # screencap_name = 'Screenshot_%s_%s.jpg' % (datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
    # screen_image = os.path.join(org_screen_path, screencap_name)
    # im.save(screen_image, format="JPEG")
    return im


def get_average_position(x0, y0, x1, y1):
    return int((x0 + x1) / 2), int((y0 + y1) / 2)


def get_easyocr_rect_center(easyocr_list, bbox):
    x0 = (easyocr_list[0][0] + easyocr_list[1][0]) / 2
    y0 = (easyocr_list[1][1] + easyocr_list[2][1]) / 2
    return int(x0) + bbox[0], int(y0) + bbox[1]


def get_rect_center(bbox):
    x0 = (bbox[0] + bbox[2]) / 2
    y0 = (bbox[1] + bbox[3]) / 2
    return int(x0), int(y0)


# def get_random_rect_center(easyocr_list, bbox):
#     x0 = (easyocr_list[0][0] + easyocr_list[1][0]) / 2
#     y0 = (easyocr_list[1][1] + easyocr_list[2][1]) / 2
#     return int(x0) + bbox[0] + offset_position(), int(y0) + bbox[1] + offset_position()


def handle_function_list(obj, func_list):
    for i in range(obj.call_index, len(func_list)):
        if not obj.race:
            func_list[i]()
            obj.called += 1
            obj.call_index += 1
        else:
            break
    if obj.called == len(func_list):
        logger.info("执行结束 %s" % func_list)
        obj.race = 0  # 执行结束
        return True


def cv_imread(file_path, flag=1):
    # 读取图像，解决imread不能读取中文路径的问题
    return cv2.imdecode(np.fromfile(file_path, dtype=np.uint8), flag)


def set_clipboard(text):
    win32clipboard.OpenClipboard()
    win32clipboard.EmptyClipboard()
    win32clipboard.SetClipboardData(win32clipboard.CF_UNICODETEXT, text)
    win32clipboard.CloseClipboard()


def clear_clipboard():
    # 清楚剪切板内容，以防检测
    win32clipboard.OpenClipboard()
    win32clipboard.EmptyClipboard()
    win32clipboard.CloseClipboard()


def get_all_game_rect():
    rect_ga_list = list()
    win_list = list()

    def callback(hwnd, extra):
        if win32gui.IsWindowVisible(hwnd):
            win_txt = win32gui.GetWindowText(hwnd)
            # logger.info(win_txt)
            if "梦幻西游 ONLINE -" in win_txt:
                if win_txt in win_list:
                    # 重复的替换掉
                    i = win_list.index(win_txt)
                    rect_ga_list.pop(i)
                    win_list.pop(i)
                win_list.append(win_txt)
                tmp = list()
                # tmp.append(win32gui.GetWindowRect(hwnd))
                tmp.append(autoit.win_get_pos_by_handle(hwnd))
                tmp.append(win_txt)
                rect_ga_list.append(tmp)
    win32gui.EnumWindows(callback, None)
    rect_ga_list = sorted(rect_ga_list)
    rect_list = list()
    ga_list = list()
    for rect, ga in rect_ga_list:
        rect_list.append(rect)
        ga_list.append(ga.split(" - (")[1].split(" - ")[0] + ".json")
    # logger.info(win_list)
    # logger.info(rect_ga_list)
    # logger.info(rect_list)
    logger.info("窗口总数: %s" % len(rect_list))
    return rect_list, ga_list


def is_foreground_window(rect):
    return autoit.win_get_pos_by_handle(win32gui.GetForegroundWindow()) == rect


def serial_connect():
    return serial.Serial(get_arduino_com_port(), 57616, timeout=1, write_timeout=10)


ser = serial_connect()
ser.flushInput()
ser.flushOutput()
clear_serial_data()
