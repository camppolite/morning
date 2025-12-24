# -*- coding:utf-8 -*-
import os.path
import time

from step import *
from common import *


def hide_player():
    input_f9()
    logger.info("隐藏玩家")


def hide_stalls():
    # 隐藏摊位
    input_alt_h()
    logger.info("隐藏摆摊")


def hide_player_n_stalls():
    serial_write(KEY_HIDE)
    logger.info("隐藏玩家和摆摊")


def human_click_cur():
    for _ in range(random.randint(2, 3)):
        serial_click_cur()


def match_cursor_folder_threshold(cv_image, folder):
    normal_mouse = []
    for file_path, _, files in os.walk(os.path.join(os.getcwd(), "object", folder)):
        for file in files:
            normal_mouse.append([file_path, file])
    if folder == "cursors_threshold":
        cursor_x = 8  # 鼠标截图坐标偏差
        cursor_y = 8  # 鼠标截图坐标偏差
    elif folder == "cursors_attack_threshold":
        cursor_x = 1  # 鼠标截图坐标偏差
        cursor_y = 1  # 鼠标截图坐标偏差
    # elif folder == "cursors_miss_threshold":
    #     cursor_x = -8  # 鼠标截图坐标偏差
    #     cursor_y = -8  # 鼠标截图坐标偏差
    # elif folder == "cursors_hit_threshold":
    #     cursor_x = -8  # 鼠标截图坐标偏差
    #     cursor_y = -8  # 鼠标截图坐标偏差
    # elif folder == "huoxue_threshold":
    #     # 化生寺活血技能焦点定位
    #     cursor_x = 28  # 鼠标截图坐标偏差
    #     cursor_y = -7  # 鼠标截图坐标偏差
    else:
        cursor_x = 0
        cursor_y = 0
    # logger.info(folder)
    for obj in normal_mouse:
        obj_img = os.path.join(obj[0], obj[1])
        # logger.info(obj_img)
        pos = opencv_threshold_object_pos_ex(cv_image, obj_img)
        if pos[0] > 0:
            # logger.info(pos)
            # logger.info("找到光标 %s" % obj_img)
            return pos[0] - cursor_x, pos[1] - cursor_y  # 因为截图裁剪了，坐标需要修正
    return -1, -1


def match_cursor_threshold(cv_image):
    images = (
        img_cursors_skill_huoxue,
        img_cursors_skill_langyong,
        img_cursors_skill_lieshi,
        img_cursors_skill_tianbengdilie
    )
    cursor_x = 28  # 鼠标截图坐标偏差
    cursor_y = -7  # 鼠标截图坐标偏差
    for image in images:
        # if image == img_cursors_skill_huoxue:
        #     # 化生寺活血技能焦点定位
        #     cursor_x = 28  # 鼠标截图坐标偏差
        #     cursor_y = -7  # 鼠标截图坐标偏差
        # elif image == img_cursors_skill_langyong:
        #     cursor_x = 28  # 鼠标截图坐标偏差
        #     cursor_y = -7  # 鼠标截图坐标偏差
        # elif image == img_cursors_skill_lieshi:
        #     cursor_x = 28  # 鼠标截图坐标偏差
        #     cursor_y = -7  # 鼠标截图坐标偏差
        # elif image == img_cursors_skill_tianbengdilie:
        #     cursor_x = 28  # 鼠标截图坐标偏差
        #     cursor_y = -7  # 鼠标截图坐标偏差
        # else:
        #     cursor_x = 0
        #     cursor_y = 0
        obj_img = os.getcwd() + image
        # logger.info(obj_img)
        pos = opencv_threshold_object_pos_ex(cv_image, obj_img)
        if pos[0] > 0:
            # logger.info(pos)
            # logger.info("找到光标 %s" % obj_img)
            return pos[0] - cursor_x, pos[1] - cursor_y  # 因为截图裁剪了，坐标需要修正
    return -1, -1


def mouse_move_foreground_left_top(bbox):
    serial_move_human(bbox[0] + 150, bbox[1] + 150, mode=0)


def mouse_move_foreground_right_top(bbox):
    serial_move_human(bbox[0] + 820 + wind_int(15), bbox[1] + 180 + wind_int(3), mode=0)


def mouse_move_foreground_center_top(bbox):
    serial_move_human(bbox[0] + 515, bbox[1] + 150, mode=0)


def mouse_move_foreground_left_bottom(bbox):
    serial_move_human(bbox[0] + 250, bbox[3] - 100, mode=0)


def mouse_move_foreground_center_bottom(bbox):
    serial_move_human(bbox[0] + 515, bbox[3] - 100, mode=0)


def opencv_match(cv_image, image, threshold=0.90):
    cv_img_obj = cv_imread(os.getcwd() + image)
    try:
        res = cv2.matchTemplate(cv_image, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_gray_match(cv_image, image, threshold=0.90):
    cv_image = cv2.cvtColor(np.array(cv_image), cv2.COLOR_BGR2GRAY)
    thresh = cv2.threshold(cv_image, 107, 255, cv2.THRESH_TOZERO)[1]
    cv_img_obj = cv_imread(os.getcwd() + image, 0)
    try:
        res = cv2.matchTemplate(thresh, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_object_exist(bbox, image_object_path, threshold=0.95):
    # threshold = 0.95  # 匹配度
    cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(os.getcwd() + image_object_path)
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_object_exist2(image, image_object_path, threshold=0.95):
    # threshold = 0.95  # 匹配度
    cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(os.getcwd() + image_object_path)
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_gray_object_exist(bbox, gray_image_path, threshold=0.95, factor=1):
    image = screen_opencv_grab(bbox)
    cv_img1 = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)  # 处理灰度
    if factor != 1:
        cv_img1 = cv2.resize(cv_img1, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
    template = cv_imread(os.getcwd() + gray_image_path)
    template = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)
    thresh = cv2.threshold(template, 107, 255, cv2.THRESH_TOZERO)[1]
    if factor != 1:
        thresh = cv2.resize(thresh, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
    try:
        res = cv2.matchTemplate(cv_img1, thresh, cv2.TM_CCORR_NORMED)
        # res = cv2.matchTemplate(cv_img_obj, cv_img, cv2.TM_SQDIFF)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_gray_object_exist2(bbox, cv_image, threshold=0.95, factor=1):
    image = screen_opencv_grab(bbox)
    cv_img1 = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)  # 处理灰度
    if factor != 1:
        cv_img1 = cv2.resize(cv_img1, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
    cv_image = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    thresh = cv2.threshold(cv_image, 107, 255, cv2.THRESH_TOZERO)[1]
    if factor != 1:
        thresh = cv2.resize(thresh, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
    try:
        res = cv2.matchTemplate(cv_img1, thresh, cv2.TM_CCORR_NORMED)
        # res = cv2.matchTemplate(cv_img_obj, cv_img, cv2.TM_SQDIFF)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    return False


def opencv_gray_object_pos(bbox, cv_image, threshold=0.95):
    image = screen_opencv_grab(bbox)
    pos = (-1, -1)
    cv_img1 = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)  # 处理灰度
    cv_image = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    # thresh = cv2.threshold(cv_image, 107, 255, cv2.THRESH_TOZERO)[1]
    try:
        res = cv2.matchTemplate(cv_img1, cv_image, cv2.TM_CCORR_NORMED)
        # res = cv2.matchTemplate(cv_img_obj, cv_img, cv2.TM_SQDIFF)
    except cv2.error as e:
        logger.error(e)
        return pos
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)

    logger.info(max_val)
    if max_val >= threshold:
        img_shape = cv_image.shape  # height, width, channels = img.shape
        pos = (int(bbox[0] + max_loc[0] + img_shape[1] / 2), int(bbox[1] + max_loc[1] + img_shape[0] / 2))  # 中间的坐标
    return pos


def opencv_TM_SQDIFF_object_exist(bbox, image_object_path, threshold=90000.0):
    # TM_SQDIFF 算法是值越小，匹配度越高
    cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(os.getcwd() + image_object_path)
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_SQDIFF)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # 符合匹配标准才返回结果
    if min_val <= threshold:
        return True
    return False


def opencv_dynamic_object_pos(image, image_object_path, threshold=0.95):
    # 匹配指定图片对象的中心坐标
    center_pos = (-1, -1)
    cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(image_object_path)
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return center_pos
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        img_shape = cv_img_obj.shape  # (2340, 1080, 3)  第一个是w，第二个是h，第三个是颜色通道数
        center_pos = (int(max_loc[0] + img_shape[1] / 2), int(max_loc[1] + img_shape[0] / 2))  # 中间的坐标
    # else:
    #     logger.error("没找到%s" % image_object_path)
    return center_pos


def opencv_object_pos(bbox, image, threshold=0.95):
    # 匹配指定图片对象的中心坐标
    pos = (-1, -1)
    cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(os.getcwd() + image)
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return pos
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    if max_val >= threshold:
        img_shape = cv_img_obj.shape  # height, width, channels = img.shape
        pos = (int(max_loc[0] + img_shape[1] / 2), int(max_loc[1] + img_shape[0] / 2))  # 中间的坐标
    return pos


def opencv_object_pos2(bbox, cv_image, threshold=0.95):
    # 匹配指定图片对象的中心坐标
    pos = (-1, -1)
    cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)  # 处理灰度
    try:
        res = cv2.matchTemplate(cv_img, cv_image, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return pos
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    if max_val >= threshold:
        img_shape = cv_image.shape  # (2340, 1080, 3)  第一个是y坐标，第二个是x坐标，第三个是颜色通道数
        pos = (int(bbox[0] + max_loc[0] + img_shape[1] / 2), int(bbox[1] + max_loc[1] + img_shape[0] / 2))  # 中间的坐标
    return pos


def opencv_object_pos2_list(bbox, cv_image, threshold=0.87):
    # 匹配指定图片对象的中心坐标
    pos_list = list()
    cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)  # 处理灰度
    try:
        res = cv2.matchTemplate(cv_img, cv_image, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return pos_list
    h, w = cv_image.shape[:2]
    max_val = 1
    while max_val > threshold:
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        # logger.info(max_loc)
        # logger.info(max_val)
        if max_val >= threshold:
            img_shape = cv_image.shape  # (2340, 1080, 3)  第一个是y坐标，第二个是x坐标，第三个是颜色通道数
            res[max_loc[1] - h // 2:max_loc[1] + h // 2 + 1, max_loc[0] - w // 2:max_loc[0] + w // 2 + 1] = 0
            center_pos = (
                int(bbox[0] + max_loc[0] + img_shape[1] / 2), int(bbox[1] + max_loc[1] + img_shape[0] / 2))  # 中间的坐标
            pos_list.append(center_pos)
            if max_loc[0] <= 4:
                break
    return pos_list


def opencv_object_list(image, image_object_path, threshold=0.95):
    # threshold = 0.95  # 匹配度
    center_pos_list = list()

    cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)  # 处理灰度
    cv_img_obj = cv_imread(os.getcwd() + image_object_path)

    # cv_img = cv_imread(image)
    # cv_img_obj = cv_imread(image_object_path)

    h, w = cv_img_obj.shape[:2]
    try:
        res = cv2.matchTemplate(cv_img, cv_img_obj, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return center_pos_list
    max_val = 1
    while max_val > threshold:
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        # 符合匹配标准才返回结果
        if max_val >= threshold:
            img_shape = cv_img_obj.shape  # (2340, 1080, 3)  第一个是y坐标，第二个是x坐标，第三个是颜色通道数
            center_pos = (int(max_loc[0] + img_shape[1] / 2), int(max_loc[1] + img_shape[0] / 2))  # 中间的坐标
            res[max_loc[1] - h // 2:max_loc[1] + h // 2 + 1, max_loc[0] - w // 2:max_loc[0] + w // 2 + 1] = 0
            center_pos_list.append(center_pos)
    # if not center_pos_list:
    #     logger.error("没找到%s" % image_object_path)
    return center_pos_list


def opencv_threshold_object_pos_ex(image, image_object_path, threshold=0.80):
    # 匹配指定路径图片对象的左上角坐标
    # threshold = 0.79  # 匹配度
    center_pos = (-1, -1)

    cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)
    cv_img = sub_threshold(cv_img)
    cv_img_obj = cv_imread(image_object_path)
    try:
        res = cv2.matchTemplate(cv_img_obj, cv_img, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return center_pos
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        # img_shape = cv_img_obj.shape  # (2340, 1080, 3)  第一个是w，第二个是h，第三个是颜色通道数
        # logger.info(cv_img_obj.shape)
        # (tH, tW) = cv_img_obj.shape[:2]
        # top_left = max_loc
        # bottom_right = (top_left[0] + tW, top_left[1] + tH)
        # cv2.rectangle(cv_img, top_left, bottom_right, 255, 2)
        # screencap_name = 'Screenshot_%s_%s.jpg' % (datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S'), random_str(4))
        # org_screen_path = os.path.join(os.getenv("LOCALAPPDATA"), "tmpimage")
        # screen_image = os.path.join(org_screen_path, screencap_name)
        # cv2.imwrite(screen_image, cv_img)
        center_pos = (int(max_loc[0]), int(max_loc[1]))  # 左上角坐标
    # else:
    #     logger.error("没找到%s" % image_object_path)
    return center_pos


def opencv_threshold_object_exist(image, image_object_path, threshold=0.80):
    cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)
    cv_img = sub_threshold(cv_img)

    cv_img_obj = cv_imread(image_object_path)
    try:
        res = cv2.matchTemplate(cv_img_obj, cv_img, cv2.TM_CCOEFF_NORMED)
    except cv2.error as e:
        logger.error(e)
        return False
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # logger.info(max_val)
    # 符合匹配标准才返回结果
    if max_val >= threshold:
        return True
    # else:
    #     logger.error("没找到%s" % image_object_path)
    return False


def sub_threshold(image):
    threshold = 127  # Set your threshold value
    image[image >= threshold] = 255  # Above or equal threshold go to max value
    image[image < threshold] = 0  # Below threshold go to min value
    return image


class CatchGhosts:
    def __init__(self):
        self.game_rect = (0, 0, 10, 10)
        self.team = list()
        self.hs_index = 0  # 化生寺在队伍中的位置，加血的时候需要用到
        self.hwnd = None
        self.db = read_json_config(db_file)
        self.get_game_info()
        self.fight_cfg = read_json_config("战斗.json")

        self.gui_name_image = None
        self.reader_ch = None
        self.reader_en = None
        self.mp3_playing = False
        self.has_hs = False

        self.gui_step = Step(xiaogui_step)
        self.dazuo = dict()
        self.sects = self.parse_sects()
        # self.health_status = [0, 0, 0, 0, 0, 0, 0, 0]  # 需要加血状态
        self.health_icon = [None, None, None, None, None]  # 需要加血的头像,注意所有头像不能有相同的
        self.supply_action = [0, 0, 0, 0, 0]  # 需要加蓝的窗口
        self.win_tab_pos = (
            (self.game_rect[0] + 104, self.game_rect[1] + 41),
            (self.game_rect[0] + 274, self.game_rect[1] + 41),
            (self.game_rect[0] + 444, self.game_rect[1] + 41),
            (self.game_rect[0] + 614, self.game_rect[1] + 41),
            (self.game_rect[0] + 784, self.game_rect[1] + 41)
        )  # 长度 170
        self.gui_init()
        self.cursor_image = None

    def active_leader_win(self):
        for _ in range(5):
            if self.db["leader"] != self.get_player_name():
                leader_win_pos = self.win_tab_pos[0]
                serial_move_human(leader_win_pos[0] + wind_int(10), leader_win_pos[1] + wind_int(3))
                time.sleep(0.5)

    def active_hs_win(self):
        # 化生寺窗口
        if not self.is_sect(HUASHENGSI, player_name=self.get_player_name()):
            hs_win_pos = self.win_tab_pos[self.hs_index]
            serial_move_human(hs_win_pos[0] + wind_int(10), hs_win_pos[1] + wind_int(3))
            time.sleep(0.5)

    def active_index_win(self, index):
        pos = self.win_tab_pos[index]
        serial_move_human(pos[0] + wind_int(10), pos[1] + wind_int(3))
        time.sleep(0.5)
        # mouse_move_foreground_left_top(self.game_rect)

    def attack_xiaogui(self):
        logger.info("攻击小鬼")
        self.active_leader_win()
        # hide_player_n_stalls()

        x, y, n_x, n_y = self.get_relative_position(self.db["gui_loc"][0], self.db["gui_loc"][1], self.db["gui_scene"])
        # n_x, n_y = 30, 30
        # v_x, v_y = self.get_gui_near_offset()
        for i in range(2):
            # logger.info("n_x:%s, n_y:%s" % (n_x, n_y))
            x_0, y0 = x, y - 3
            # x_0, y0 = x, y - 37
            if i == 1:
                x_0, y0 = x, y - 40
                # x_0, y0 = x, y - 2
            # if v_x == -3:
            #     n_x = -30
            # if v_y == -3:
            #     n_y = -30
            if not self.mouse_click_human(x_0, y0, n_x=n_x, n_y=n_y, mode=6):
                logger.info("一次失败")
                if not self.mouse_click_human(x_0, y0, n_x=int(n_x * 3.5), n_y=n_y, mode=6):
                    logger.info("二次失败")
                    self.mouse_click_human(x_0, y0, n_x=n_x, n_y=int(n_y * 3.5), mode=6)
            if self.check_fighting():
                logger.info("进入战斗成功")
                mouse_move_foreground_left_top(self.game_rect)
                self.handle_wrong_attack()
                return True
            else:
                self.active_leader_win()
                serial_right_click_cur()
                mouse_move_foreground_left_top(self.game_rect)
                self.handle_wrong_attack()
                serial_move_human(x + n_x * 3, y + n_y * 3, mode=2)

        logger.info("检查是否重叠NPC")
        for _ in range(2):
            x, y, n_x, n_y = self.get_relative_position(
                self.db["gui_loc"][0], self.db["gui_loc"][1], self.db["gui_scene"])
            # n_x, n_y = 30, 30
            # v_x, v_y = self.get_gui_near_offset()
            # if v_x == -3:
            #     n_x = -30
            # if v_y == -3:
            #     n_y = -30
            if not self.mouse_click_human(x, y - 3, n_x=n_x, n_y=n_y, mode=5):  # 按住ctrl点击鼠标
                if not self.mouse_click_human(x, y - 3, n_x=int(n_x * 3.5), n_y=n_y, mode=5):
                    self.mouse_click_human(x, y - 3, n_x=n_x, n_y=int(n_y * 3.5), mode=5)

            mouse_move_foreground_left_bottom(self.game_rect)
            # if opencv_object_exist(self.cursor_bbox(x, y), img_ghost_npc_overlap_page):
            #     logger.info("和重叠NPC，进行重叠操作")

            pos = opencv_gray_object_pos(self.cursor_bbox(x, y, offset=145), self.gui_name_image, threshold=0.78)
            logger.info(pos)
            if pos[0] > 0:
                if not self.mouse_click_human(pos[0], pos[1], n_x=n_x, n_y=n_y, mode=6):
                    if not self.mouse_click_human(pos[0], pos[1], n_x=n_x * 3, n_y=n_y, mode=6):
                        self.mouse_click_human(pos[0], pos[1], n_x=n_x, n_y=n_y * 3, mode=6)
                if self.check_fighting():
                    logger.info("进入战斗成功")
                    mouse_move_foreground_left_top(self.game_rect)
                    self.handle_wrong_attack()
                    return True
                else:
                    self.active_leader_win()
                    logger.info("重叠NPC攻击失败")
                    serial_right_click_cur()
                    mouse_move_foreground_left_top(self.game_rect)
                    self.handle_wrong_attack()
                    serial_move_human(x + n_x * 3, y + n_y * 3, mode=2)
                # self.wait_walkfinding_finish(sheyaoxaing=self.need_sheyaoxiang(), timeout=3.0)
                # hide_player_n_stalls()
        return False

    # def astar_pathfinding(self, startx, starty, endx, endy, maze, maze_x_len, maze_y_len, x_o=13, y_o=10):
    #     path = astar(startx, starty, endx, endy, maze, maze_x_len, maze_y_len)
    #     logger.info(path)
    #     x_n = -1
    #     y_n = -1
    #     if path is not None:
    #         for i in range(len(path)):
    #             if abs(endx - path[i][0]) <= x_o and abs(endy - path[i][1]) <= y_o:
    #                 x_n = path[i][0]
    #                 y_n = path[i][1]
    #                 break
    #     x_v = -1 if endx < startx else 1
    #     y_v = -1 if endy < starty else 1
    #     if x_n == -1 or y_n == -1:
    #         # 匹配不上坐标
    #         logger.info("匹配不上坐标")
    #         side = 4
    #         x_n = endx - side * x_v  # 实际点
    #         y_n = endy - side * y_v
    #     logger.info("%s,%s" % (x_n, y_n))
    #     return x_n, y_n

    def close_npc_talk(self):
        closed = False
        for _ in range(3):
            if self.click_opencv_object(self.game_rect, img_btn_npc_talk_close, x_fix=wind_int(9), y_fix=wind_int(9)):
                logger.info("close_npc_talk")
                closed = True
                break
        if not closed:
            logger.info("随意点击关闭任务对话")
            serial_move_human(self.game_rect[0] + 670, self.game_rect[1] + 530)  # 随意点击关闭任务对话
        time.sleep(0.2)

    def check_fighting(self):
        return self.wait_opencv_object(self.fighting_bbox(), img_fighting, timeout=3.5, threshold=0.81)

    def gui_init(self):
        self.db["gui_scene"] = ""
        self.db["gui_loc"] = [-1, -1]
        # self.db["step"] = 0
        self.update_db()
        self.gui_name_image = None
        self.team = list()

    def do_work(self):
        self.gui_step.n = self.db.get("step") if self.db.get("step") else 0
        self.db["round"] = self.db.get("round") + 1 if self.db.get("round") else 0
        logger.info("队长: %s" % self.db["leader"])
        if opencv_object_exist(self.fighting_bbox(), img_fighting, threshold=0.85):
            logger.info("战斗中...")
            self.update_db()
            self.reader_en = easyocr.Reader(['en'], gpu=False)
            self.handle_xiaogui_fight()

        while True:
            self.set_win_foreground()
            self.active_leader_win()
            if self.gui_step.step() == to_datangguojing:
                logger.info("开始领取钟馗任务")
                if opencv_object_exist(self.scene_bbox(), img_scene_difu):
                    self.next_step(to_difu)
                elif opencv_object_exist(self.scene_bbox(), img_scene_datangguojing):
                    pass
                elif opencv_object_exist(self.scene_bbox(), img_scene_changancheng):
                    if not opencv_object_exist(self.changan_yizhan_laoban_bbox(), img_scene_changan_yizhanlaoban,
                                               threshold=0.78):
                        self.fly_to_changan_yizhan_laoban()
                    self.from_changan_fly_to_datangguojing()
                else:
                    self.fly_to_changan_yizhan_laoban()
                    self.from_changan_fly_to_datangguojing()
                self.next_step(to_datangguojing)
            elif self.gui_step.step() == to_difu:
                for _ in range(4):
                    self.from_datangguojing_to_difu()
                    if opencv_object_exist(self.scene_bbox(), img_scene_difu):
                        break
                self.next_step(to_difu)
            elif self.gui_step.step() == to_zhongkui:
                self.goto_zhongkui()
                self.next_step(to_zhongkui)
            elif self.gui_step.step() == talk_get_xiaogui:
                self.talk_to_zhongkui()
                self.next_step(talk_get_xiaogui)
            elif self.gui_step.step() == xiaogui_info:
                self.get_xiaogui_scene()
                self.close_npc_talk()
                # if self.db["gui_scene"]:
                #     logger.info("使用天眼通")
                #     self.use_beibao_prop(img_props_tianyantong, keep=self.kepp_zhuogui_beibao())
                #     self.wait_opencv_gray_object(self.task_bbox(), img_ghost_task_chu_gray_107_255_0)
                # self.get_xiaogui_name()
                # self.get_xiaogui_location()
                # if self.gui_loc_fail():
                #     serial_write(PLAY_MP3 % b'1')
                #     self.mp3_playing = True
                #     for _ in range(5):
                #         logger.info("***识别小鬼坐标失败，请手动进入战斗***")
                #     self.gui_step.n = xiaogui_step.index(fight_gui)
                #     continue
                if not self.use_tianyantong_delay():
                    self.use_tianyantong(self.kepp_zhuogui_beibao())
                    self.get_actual_gui_name_n_loc()
                    if self.gui_loc_fail():
                        x, y, n_x, n_y = self.get_relative_position(57, 50, "地府")
                        self.mouse_click_human(x, y, n_x=n_x, n_y=n_y)
                        self.wait_walkfinding_finish(sheyaoxaing=False, timeout=5)
                        self.get_actual_gui_name_n_loc()
                        if self.gui_loc_fail():
                            self.loc_fail_n_play()
                            continue
                else:
                    self.open_beibao()
                self.next_step(xiaogui_info)
            elif self.gui_step.step() == to_xiaogui_scene:
                self.goto_xiaogui_scene()
                self.next_step(to_xiaogui_scene)
            elif self.gui_step.step() == to_xiaogui:
                if self.gui_loc_fail():
                    self.gui_step.n = xiaogui_step.index(fight_gui)
                    self.mp3_playing = True
                    continue
                self.walk_to_xiaogui()
                self.next_step(to_xiaogui)
            elif self.gui_step.step() == attack_gui:
                if not self.attack_xiaogui():
                    self.mp3_playing = True
                self.next_step(attack_gui)
            elif self.gui_step.step() == fight_gui:
                self.wait_fighting()
                self.handle_xiaogui_fight()
                self.next_step(fight_gui)

            if self.gui_step.end:
                logger.info("完成一次抓鬼.")
                self.gui_step.init()
                self.gui_init()

    def easyocr_readtext(self, image, batch_size=128, min_size=20, detail=1, factor=1, gray=0, decoder='greedy'):
        if not isinstance(image, np.ndarray):
            image = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)
        if factor != 1:
            image = cv2.resize(image, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        if gray:
            image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 灰度
        if not self.reader_ch:
            logger.info("初始化easyocr Reader")
            self.reader_ch = easyocr.Reader(['ch_sim', 'en'], gpu=False)
        return self.reader_ch.readtext(image, batch_size=batch_size, min_size=min_size, detail=detail, decoder=decoder)

    def easyocr_readnumber(self, image, batch_size=128, min_size=1, detail=0, factor=2, gray=0, ts=0, decoder='greedy',
                           cat=False):
        cv_iamge = image
        if not isinstance(image, np.ndarray):
            cv_iamge = cv2.cvtColor(np.array(cv_iamge), cv2.COLOR_BGR2RGB)
        if gray:
            # logger.info("灰度处理")
            cv_iamge = cv2.cvtColor(cv_iamge, cv2.COLOR_BGR2GRAY)  # 灰度
        if ts:
            cv_iamge = cv2.threshold(cv_iamge, 107, 255, cv2.THRESH_TOZERO)[1]
        img_length = cv_iamge.shape[1]
        if cat:
            # logger.info("img_length:%s" % img_length)
            threshold = 0.95
            if img_length <= 10:
                # 一位数
                if opencv_gray_match(image, img_ghost_task_9_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_9_gray_107_255_0")
                    return 9
                elif opencv_gray_match(image, img_ghost_task_6_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_6_gray_107_255_0")
                    return 6
                elif opencv_gray_match(image, img_ghost_task_1_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_1_gray_107_255_0")
                    return 1
            elif 10 < img_length <= 17:
                # 二位数
                if opencv_gray_match(image, img_ghost_task_60_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_60_gray_107_255_0")
                    return 60
                elif opencv_gray_match(image, img_ghost_task_62_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_62_gray_107_255_0")
                    return 62
                elif opencv_gray_match(image, img_ghost_task_10_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_task_10_gray_107_255_0")
                    return 10
            elif 18 < img_length <= 24:
                # 三位数
                if opencv_gray_match(image, img_ghost_loc_100_gray_107_255_0, threshold=threshold):
                    logger.info("img_ghost_loc_100_gray_107_255_0")
                    return 100
        if factor != 1:
            cv_iamge = cv2.resize(cv_iamge, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        if not self.reader_en:
            logger.info("初始化easyocr Reader")
            self.reader_en = easyocr.Reader(['en'], gpu=False)
        result = self.reader_en.readtext(
            cv_iamge, batch_size=batch_size, min_size=min_size, detail=detail, decoder=decoder, allowlist='0123456789')
        if cat:
            for res in result:
                logger.info(res)
        if len(result) == 1:
            # if result[0] == "1l1":
            #     return 111
            # elif result[0] == "20?":
            #     return 207
            # elif result[0] == "20Z":
            #     return 202
            try:
                return int(result[0])
            except Exception as e:
                logger.error(e)
                logger.error("识别数字失败")
        else:
            logger.error("识别数字失败")
        return -1

    def fight_waiting_for_hero_action(self):
        return opencv_object_exist(self.fight_action_bbox(), img_fight_do_hero_action, threshold=0.79)

    def fight_waiting_for_peg_action(self):
        return opencv_object_exist(self.fight_action_bbox(), img_fight_do_peg_action, threshold=0.79)

    def fly_to_changan_yizhan_laoban(self):
        if opencv_object_exist(self.scene_bbox(), img_scene_changancheng):
            logger.info("长安场景开始飞")
            logger.info("使用长安合成旗")
            self.use_beibao_prop(img_props_red_777)
            mouse_move_foreground_center_bottom(self.game_rect)
            flag_loc = self.wait_opencv_object_pos(self.changan777_yizhan_laoban_bbox(), img_btn_flag_loc, threshold=0.85, timeout=5.0)
            for _ in range(4):
                self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
                time.sleep(0.1)
                ct = time.time()
                while True:
                    my_x, my_y = self.get_my_location()
                    if my_x != -1 and abs(my_x - 278) <= 30 and abs(my_y - 40) <= 30:
                        time.sleep(0.15)
                        return
                    if time.time() - ct >= 2.0:
                        break
        else:
            self.use_changan777(self.changan777_yizhan_laoban_bbox())

    def from_map_serach_to_npc(self, name, img_paths, wait_time, search=True, n=9):
        logger.info("从地图导航到:" + name)
        self.open_map()
        bbox = self.map_bbox()
        if not self.click_opencv_object(bbox, img_map_search, threshold=0.85):
            self.click_opencv_object(bbox, img_map_search1, threshold=0.85)
        if search:
            self.click_opencv_object(bbox, img_map_wayfinding_search, x_fix=50)
            input_from_clipboard(name)
        if isinstance(img_paths, list) or isinstance(img_paths, tuple):
            for img_path in img_paths:
                if self.click_opencv_object(bbox, img_path, mode=2):
                    serial_right_click_cur()
                    break
        else:
            self.click_opencv_object(bbox, img_paths, mode=2)
            serial_right_click_cur()
        if search:
            input_backspace(n)
        self.close_map()
        if wait_time > 0:
            ct = time.time()
            i = 0
            while time.time() - ct < wait_time:
                time.sleep(1)
                i += 1
                print(i)

    def from_zhuziguo777_to_datangjingwai(self, turn=True, keep=False):
        logger.info("从朱紫国到大唐境外")
        self.use_zhuziguo777(self.zhuziguo777_datangjingwai_bbox(), move=False, turn=turn, keep=keep)
        hide_player()
        time.sleep(0.1)
        # 朱紫国--大唐境外(3,2)
        x, y, n_x, n_y = self.get_relative_position(2, 4, "朱紫国")
        n_x = 35
        n_y = -35
        for _ in range(3):
            if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y):
                if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y * 2):
                    self.mouse_click_human(x, y, n_x=n_x * 2, n_y=n_y)
            if self.wait_opencv_object(self.scene_bbox(), img_scene_datangjingwai, timeout=4.0):
                break
        time.sleep(0.15)

    def from_changan777_to_datangguojing(self, turn=True, keep=False):
        logger.info("从长安到大唐国境")
        self.use_changan777(self.changan777_datangguojing_bbox(), turn=turn, keep=keep)
        hide_player()
        time.sleep(0.1)
        x = self.game_rect[0] + 90
        y = self.game_rect[1] + 740
        n_x = 30
        n_y = -30
        if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y):
            if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y * 2):
                self.mouse_click_human(x, y, n_x=n_x * 2, n_y=n_y)
        self.wait_opencv_object(self.scene_bbox(), img_scene_datangguojing, timeout=3.0)
        time.sleep(0.15)

    def from_changan777_to_jiangnanyewai(self, turn=True, keep=False):
        logger.info("从长安到江南野外")
        self.use_changan777(self.changan777_jiangnanyewai_bbox(), move=False, turn=turn, keep=keep)
        hide_stalls()
        time.sleep(0.1)
        x = self.game_rect[0] + 930
        y = self.game_rect[1] + 735
        near = -32
        if not self.mouse_click_human(x, y, n_x=near, n_y=near):
            if not self.mouse_click_human(x, y, n_x=near, n_y=near * 2):
                self.mouse_click_human(x, y, n_x=near * 2, n_y=near)
        self.wait_opencv_object(self.scene_bbox(), img_scene_jiangnanyewai, timeout=3.0)
        time.sleep(0.15)

    def from_changan_fly_to_datangguojing(self):
        logger.info("和长安驿站老板对话")
        self.active_leader_win()
        time.sleep(0.2)
        talk_ok = False
        pos = (-1, -1)
        for i in range(9):
            if not self.click_opencv_dynamic_object(
                    self.game_rect, img_npc_changan_yizhan_laoban_dir, threshold=0.79):
                hide_player()
                # time.sleep(0.1)
                if not self.click_opencv_dynamic_object(
                        self.game_rect, img_npc_changan_yizhan_laoban_dir, threshold=0.79):
                    hide_stalls()
                    self.click_opencv_dynamic_object(
                        self.game_rect, img_npc_changan_yizhan_laoban_dir, threshold=0.79)
            pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), img_btn_shide_woyaoqu, timeout=2.0)
            if pos[0] > 0:
                talk_ok = True
                break
            else:
                logger.info("点击驿站老板失败，重试")
                # mouse_move_foreground_center_top(self.game_rect)
                # self.wait_walkfinding_finish(sheyaoxaing=False, timeout=7)
                # map_pos = self.open_map()
                # self.mouse_click_human(map_pos[0] + 261, map_pos[1] + 295, mode=0)
                # human_click_cur()
                # time.sleep(0.1)
                # self.close_map()
                # self.wait_walkfinding_finish(sheyaoxaing=False, timeout=7)

                hide_player_n_stalls()

                x, y, n_x, n_y = self.get_relative_position(275, 41, "长安城")
                self.mouse_click_human(x, y)
                self.wait_walkfinding_finish(timeout=5, sheyaoxaing=False)
                mouse_move_foreground_left_bottom(self.game_rect)

        if talk_ok:
            logger.info("从长安驿站老板传送到大唐国境")
            for _ in range(7):
                self.mouse_click_human(pos[0] + wind_int(5), pos[1], n_x=wind_int(2), n_y=wind_int(30))
                if self.wait_opencv_object(self.scene_bbox(), img_scene_datangguojing, timeout=1.7):
                    time.sleep(0.15)
                    break
        else:
            logger.info("和驿站老板对话失败，需要手动操作")
        time.sleep(0.3)

    def from_datangguojing_to_difu(self):
        logger.info("从大唐国境到地府")
        self.active_leader_win()
        for i in range(3):
            self.handle_sheyaoxiang_time()
            map_pos = self.open_map()
            # 迷惑动作
            if istrue():
                self.mouse_click_human(map_pos[0] + 31 + wind_int(10), map_pos[1] + 123 + wind_int(5), mode=0)
                human_click_cur()
                time.sleep(0.1 * random.randint(1, 3))

            for _ in range(2):
                self.mouse_click_human(map_pos[0] + 45 + wind_int(2), map_pos[1] + 73, mode=0)
                human_click_cur()
                time.sleep(0.5)
            time.sleep(0.1)
            self.close_map()
            self.close_beibao_smart()

            if not self.reader_en:
                logger.info("初始化easyocr Reader")
                self.reader_en = easyocr.Reader(['en'], gpu=False)

            if i == 0:
                self.handle_team_status()

            self.wait_walkfinding_finish(timeout=20)
            my_x, my_y = self.get_my_location()
            space = 15
            if 47 - my_x >= space or 329 - my_y >= space:
                logger.info("地图导航位置不对，重新导航")
            else:
                break

        for _ in range(3):
            hide_player()
            time.sleep(0.2)
            # 地府--大唐国境(46,324)  47, 329
            x, y, n_x, n_y = self.get_relative_position(49, 329, "大唐国境")
            if not self.mouse_click_human(x, y - 35, n_x=n_x, n_y=30):
                self.mouse_click_human(x, y - 35, n_x=n_x, n_y=60)
            if self.wait_opencv_object(self.scene_bbox(), img_scene_difu, timeout=4.0):
                time.sleep(0.15)
                break
            self.wait_walkfinding_finish(timeout=2)

    def from_datangguojing_to_putuoshan(self):
        logger.info("从大唐国境到普陀山")
        self.active_leader_win()
        self.handle_sheyaoxiang_time()
        map_pos = self.open_map()
        # # 迷惑动作
        # if istrue():
        #     self.mouse_click_human(map_pos[0] + 254 + wind_int(10), map_pos[1] + 337 + wind_int(5), mode=0)
        #     human_click_cur()
        #     time.sleep(0.1 * random.randint(1, 3))

        self.mouse_click_human(map_pos[0] + 224, map_pos[1] + 350, mode=0)
        human_click_cur()
        time.sleep(0.1)
        self.close_map()
        self.close_beibao_smart()

        if self.use_tianyantong_delay():
            self.use_tianyantong(False)
            self.get_actual_gui_name_n_loc()
            self.loc_fail_n_play()
        # self.handle_dazuo_skill([1, 1, 1, 1, 1])

        self.wait_walkfinding_finish(timeout=22, scene="大唐国境")

        logger.info("和普陀接引仙女对话")
        talk_ok = False
        pos = (-1, -1)
        for i in range(7):
            hide_player()
            # 普陀接引仙女在大唐国境(222,62)   (221,60)
            x, y, n_x, n_y = self.get_relative_position(221, 61, "大唐国境")
            self.mouse_click_human(x, y - 5, n_x=n_x, n_y=n_y)
            pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), img_btn_shide_woyaoqu, timeout=2.5)
            if pos[0] > 0:
                talk_ok = True
                break
            else:
                mouse_move_foreground_center_top(self.game_rect)
                self.wait_walkfinding_finish(timeout=7)
                if i > 1:
                    map_pos = self.open_map()
                    # logger.info(map_pos)
                    self.mouse_click_human(map_pos[0] + 224, map_pos[1] + 350, mode=0)
                    human_click_cur()
                    time.sleep(0.1)
                    self.close_map()
                    self.wait_walkfinding_finish(timeout=15)

        if talk_ok:
            for _ in range(7):
                logger.info("点击按钮-是的，我要去")
                self.mouse_click_human(pos[0] + wind_int(5), pos[1], n_x=wind_int(2), n_y=wind_int(30))
                if self.wait_opencv_object(self.scene_bbox(), img_scene_putuoshan, timeout=2.0):
                    time.sleep(0.15)
                    break
        else:
            logger.info("和普陀接引仙女对话失败，需要手动操作")

            # input_tab()
            # map_pos = self.get_map_search_button_pos()
            # self.mouse_click_human(map_pos[0] + 234, map_pos[1] + 335, mode=0)
            # human_click_cur()
            # time.sleep(0.1)
            # input_tab()
            # self.wait_walkfinding_finish(sheyaoxaing=False, timeout=5)

        time.sleep(0.2)

    def from_datangguojing_to_datangjingwai(self):
        logger.info("从大唐国境到大唐境外")
        self.active_leader_win()
        self.handle_sheyaoxiang_time()
        map_pos = self.open_map()
        # 迷惑动作
        if istrue():
            self.mouse_click_human(map_pos[0] + 20 + wind_int(10), map_pos[1] + 210 + wind_int(10), mode=0)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))
        if istrue():
            self.mouse_click_human(map_pos[0] + 128 + wind_int(10), map_pos[1] + 387 + wind_int(10), mode=0)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))

        for _ in range(2):
            self.mouse_click_human(map_pos[0], map_pos[1] + 339, mode=0)
            human_click_cur()
            time.sleep(0.5)
        time.sleep(0.1)
        self.close_map()
        self.close_beibao_smart()

        # self.handle_dazuo_skill([1, 1, 1, 1, 1])

        self.wait_walkfinding_finish(timeout=300, scene="大唐国境")

        near = 30
        for _ in range(3):
            hide_player()
            time.sleep(0.2)
            # 大唐境外--大唐国境(4,79)
            x, y, n_x, n_y = self.get_relative_position(4, 78, "大唐国境")
            if not self.mouse_click_human(x - 15, y, n_x=near, n_y=near):
                self.mouse_click_human(x - 15, y, n_x=near, n_y=near * 2)
            if self.wait_opencv_object(self.scene_bbox(), img_scene_datangjingwai, timeout=3.0):
                time.sleep(0.15)
                break
            self.wait_walkfinding_finish(timeout=3)

        # x = self.game_rect[0] + 80
        # y = self.game_rect[1] + 370
        #
        # if not self.mouse_click_human(x, y, n_x=near, n_y=near):
        #     if not self.mouse_click_human(x, y, n_x=near, n_y=near * 2):
        #         self.mouse_click_human(x, y, n_x=near * 2, n_y=near)
        # self.wait_opencv_object(self.scene_bbox(), img_scene_datangjingwai, timeout=3.0)
        # time.sleep(0.15)

    def from_datangjingwai_to_wuzhuangguan(self):
        logger.info("从大唐境外到五庄观")
        self.active_leader_win()
        self.handle_sheyaoxiang_time()
        map_pos = self.open_map()
        # 迷惑动作
        if istrue():
            self.mouse_click_human(map_pos[0] + 465 + wind_int(10), map_pos[1] + 104 + wind_int(10), mode=0)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))

        for _ in range(2):
            self.mouse_click_human(map_pos[0] + 563, map_pos[1] + 104, mode=0)
            human_click_cur()
            time.sleep(0.5)
        time.sleep(0.1)
        self.close_map()
        self.close_beibao_smart()

        if self.use_tianyantong_delay():
            self.use_tianyantong(False)
            self.get_actual_gui_name_n_loc()
            self.loc_fail_n_play()
        # self.handle_dazuo_skill([1, 1, 1, 1, 1])

        self.wait_walkfinding_finish(timeout=45, scene="大唐境外")
        for _ in range(7):
            hide_player()
            # time.sleep(0.2)
            # 五庄观  大唐境外(634,74) 634, 81
            x, y, n_x, n_y = self.get_relative_position(634, 81, "大唐境外")
            if not self.mouse_click_human(x + 10, y, n_x=-45):
                self.mouse_click_human(x + 10, y, n_x=-90)
            if self.wait_opencv_object(self.scene_bbox(), img_scene_wuzhuangguan, timeout=5.0):
                time.sleep(0.15)
                break

            map_pos = self.open_map()
            self.mouse_click_human(map_pos[0] + 563, map_pos[1] + 104, mode=0)
            human_click_cur()
            time.sleep(0.1)
            self.close_map()
            self.wait_walkfinding_finish(timeout=45, scene="大唐境外")

    def from_jianyecheng_to_jiangnanyewai(self):
        logger.info("从建邺城到江南野外")
        self.use_feixingfu(self.feixingfu_jianyecheng_bbox(), img_ghost_feixingfu_jianyecheng, img_scene_jianyecheng)
        self.open_map()
        self.map_wayfinding_scene(19, 8, 556, 276, 287, 143, side=False)
        time.sleep(0.1)
        self.close_map()

        self.wait_walkfinding_finish(sheyaoxaing=False, timeout=11)

        talk_ok = False
        pos = (-1, -1)
        for i in range(7):
            hide_player_n_stalls()
            # 建邺守卫 (13,5)
            x, y, n_x, n_y = self.get_relative_position(13, 5, "建邺城")
            self.mouse_click_human(x, y - 10, n_x=n_x, n_y=n_y)
            pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), img_btn_shide_woyaoqu, timeout=2.5)
            if pos[0] > 0:
                talk_ok = True
                break
            else:
                mouse_move_foreground_center_top(self.game_rect)
                self.wait_walkfinding_finish(sheyaoxaing=False, timeout=5)
                if i > 1:
                    self.open_map()
                    self.map_wayfinding_scene(19, 8, 556, 276, 287, 143, side=False)
                    self.close_map()
                    self.wait_walkfinding_finish(sheyaoxaing=False, timeout=5)
        if talk_ok:
            for _ in range(8):
                self.mouse_click_human(pos[0] + wind_int(5), pos[1], n_x=wind_int(2), n_y=wind_int(30))
                if self.wait_opencv_object(self.scene_bbox(), img_scene_jiangnanyewai, timeout=2.0):
                    return
        else:
            logger.info("传送江南野外失败，需要手动操作")

    def get_xiaogui_scene(self):
        # 2.鬼的地点：长寿村、傲来国、朱紫国、西梁女国、宝象国、建邺城、大唐境外、江南野外、女儿村、普陀山、五庄观等。
        # 3.鬼的坐标：任务栏提示坐标±50范围内，需要进行寻找（使用天眼通符可以获得准确坐标）。
        logger.info("识别小鬼场景")
        scene = None
        threshold = 0.83
        cv_img = cv2.cvtColor(np.array(screen_opencv_grab(self.zhongkui_task_bbox())), cv2.COLOR_BGR2RGB)
        if opencv_match(cv_img, img_ghost_npc_changshoucun, threshold=threshold):
            scene = "长寿村"
        elif opencv_match(cv_img, img_ghost_npc_aolaiguo, threshold=threshold):
            scene = "傲来国"
        elif opencv_match(cv_img, img_ghost_npc_zhuziguo, threshold=threshold):
            scene = "朱紫国"
        elif opencv_match(cv_img, img_ghost_npc_xiliangnvguo, threshold=threshold):
            scene = "西凉女国"
        elif opencv_match(cv_img, img_ghost_npc_baoxiangguo, threshold=threshold):
            scene = "宝象国"
        elif opencv_match(cv_img, img_ghost_npc_jianyecheng, threshold=threshold):
            scene = "建邺城"
        elif opencv_match(cv_img, img_ghost_npc_datangjingwai, threshold=threshold):
            scene = "大唐境外"
        elif opencv_match(cv_img, img_ghost_npc_jiangnanyewai, threshold=threshold):
            scene = "江南野外"
        elif opencv_match(cv_img, img_ghost_npc_nvercun, threshold=threshold):
            scene = "女儿村"
        elif opencv_match(cv_img, img_ghost_npc_putuoshan, threshold=threshold):
            scene = "普陀山"
        elif opencv_match(cv_img, img_ghost_npc_wuzhuangguan, threshold=threshold):
            scene = "五庄观"

        if not scene:
            logger.error("识别小鬼场景失败")
        else:
            logger.info("小鬼场景:%s" % scene)
        self.db["gui_scene"] = scene
        self.update_db()

    def get_xiaogui_location(self):
        # logger.info("识别小鬼坐标")
        x_loc_bbox, y_loc_bbox = self.task_xiaogui_location_bbox()
        x = self.easyocr_readnumber(screen_opencv_grab(x_loc_bbox), factor=4, gray=1, ts=1, cat=True)
        if x == -1:
            logger.info("重试")
            x = self.easyocr_readnumber(screen_opencv_grab(x_loc_bbox), factor=4, gray=1, cat=True)
        if x == -1:
            logger.info("重试")
            x = self.easyocr_readnumber(screen_opencv_grab(x_loc_bbox), gray=1, cat=True)
        y = self.easyocr_readnumber(screen_opencv_grab(y_loc_bbox), factor=4, gray=1, ts=1, cat=True)
        if y == -1:
            logger.info("重试")
            y = self.easyocr_readnumber(screen_opencv_grab(y_loc_bbox), factor=4, gray=1, cat=True)
        if y == -1:
            logger.info("重试")
            y = self.easyocr_readnumber(screen_opencv_grab(y_loc_bbox), gray=1, cat=True)
        logger.info("小鬼坐标:%s,%s" % (x, y))
        self.db["gui_loc"] = x, y
        self.update_db()

    def get_my_location(self, log=True):
        # logger.info("识别自己坐标")
        x_image, y_image = self.my_location_image()
        # x_image = cv2.cvtColor(np.array(screen_opencv_grab(x_image)), cv2.COLOR_BGR2GRAY)
        # y_image = cv2.cvtColor(np.array(screen_opencv_grab(y_image)), cv2.COLOR_BGR2GRAY)
        # x_image.save("x_image.jpg", format="JPEG")
        # y_image.save("y_image.jpg", format="JPEG")
        factor = 4
        x = self.easyocr_readnumber(x_image, gray=1, ts=1)
        # if x == 0:
        #     logger.info("重试")
        #     x = self.easyocr_readnumber(x_image, gray=1, ts=1)
        if x == -1:
            logger.info("重试")
            x = self.easyocr_readnumber(x_image, factor=factor, gray=1, ts=1)
        if x == -1:
            logger.info("重试")
            x = self.easyocr_readnumber(x_image, factor=factor, gray=1)
        y = self.easyocr_readnumber(y_image, gray=1, ts=1)
        # if y == 0:
        #     logger.info("重试")
        #     y = self.easyocr_readnumber(y_image, gray=1, ts=1)
        if y == -1:
            logger.info("重试")
            y = self.easyocr_readnumber(y_image, factor=factor, gray=1, ts=1)
        if y == -1:
            logger.info("重试")
            y = self.easyocr_readnumber(y_image, factor=factor, gray=1)
        if log:
            logger.info("自己坐标:%s,%s" % (x, y))
        return x, y

    def get_relative_position(self, x, y, scene):
        x_rim = 6  # 窗口边框像素
        y_rim = 57  # 窗口边框像素
        c_x = 518  # 中点坐标 1024/2 + x_rim
        c_y = 441  # 中点坐标 768/2 + y_rim
        x_edge = 25  # 超过这个坐标，人物会在窗口中间
        y_edge = 19  # 超过这个坐标，人物会在窗口中间
        pixel = 20  # 20像素一个坐标点

        max_x, max_y = self.get_map_max_loc(scene)
        my_x, my_y = self.get_my_location()
        if my_x <= x_edge:
            x_pixel = x_rim + x * pixel
        elif (max_x - my_x) <= x_edge:
            x_pixel = x_rim + 1024 - (max_x - x) * pixel
        else:
            x_pixel = c_x - (my_x - x) * pixel
        if my_x > x:
            n_x = 30
        else:
            n_x = -30

        if my_y <= y_edge:
            y_pixel = y_rim + 768 - y * pixel
        elif (max_y - my_y) <= y_edge:
            y_pixel = y_rim + (max_y - y) * pixel
        else:
            y_pixel = c_y + (my_y - y) * pixel
        if my_y > y:
            n_y = -30
        else:
            n_y = 30
        # logger.info("相对像素:%s,%s" % (x_pixel, y_pixel))
        return self.game_rect[0] + x_pixel, self.game_rect[1] + y_pixel - 1, n_x, n_y

    # def get_gui_near_offset(self):
    #     edge = 10
    #     n_x = 1
    #     n_y = 1
    #     max_x, max_y = self.get_map_max_loc(self.db["gui_scene"])
    #     my_x, my_y = self.get_my_location()
    #     if abs(max_x - self.db["gui_loc"][0]) <= edge:
    #         n_x = -3
    #     elif self.db["gui_loc"][0] <= edge:
    #         n_x = 3
    #     elif my_x > self.db["gui_loc"][0]:
    #         n_x = 3
    #     else:
    #         n_x = -3
    #     if abs(max_y - self.db["gui_loc"][1]) <= edge:
    #         n_y = 3
    #     elif self.db["gui_loc"][1] <= edge:
    #         n_y = -3
    #     elif my_y > self.db["gui_loc"][1]:
    #         n_y = -3
    #     else:
    #         n_y = 3
    #     return n_x, n_y

    def get_player_name(self):
        win_txt = win32gui.GetWindowText(self.hwnd)
        return win_txt.split(" - (")[1].split(" - ")[1].split("[")[0]

    def use_tianyantong(self, keep):
        logger.info("使用天眼通")
        self.active_leader_win()
        self.use_beibao_prop(img_props_tianyantong, keep=keep)
        self.wait_opencv_gray_object(self.task_bbox(), img_ghost_task_chu_gray_107_255_0, timeout=3.0, threshold=0.79)
        if opencv_gray_object_exist(self.task_bbox(), img_ghost_task_fujin_gray_107_255_0, threshold=0.79):
            turn = False if keep else True
            self.use_beibao_prop(img_props_tianyantong, turn=turn, keep=keep)
            self.wait_opencv_gray_object(
                self.task_bbox(), img_ghost_task_chu_gray_107_255_0, timeout=3.0, threshold=0.79)

    def get_actual_gui_name_n_loc(self):
        self.get_xiaogui_name()
        for _ in range(3):
            self.get_xiaogui_location()
            if self.db["gui_loc"][0] != -1 and self.db["gui_loc"][1] != -1:
                break
            logger.info("识别小鬼坐标失败，重试")

    def loc_fail_n_play(self):
        if self.gui_loc_fail():
            screenshot_error("gui_loc_fail")
            serial_write(PLAY_MP3 % b'1')
            logger.info("播放喇叭")
            self.mp3_playing = True
            for _ in range(5):
                logger.info("***识别小鬼坐标失败，请手动进入战斗***")
            self.gui_step.n = xiaogui_step.index(fight_gui)

    def get_xiaogui_name(self):
        logger.info("记录小鬼名字")
        self.gui_name_image = cv2.cvtColor(
            np.array(screen_opencv_grab(self.task_xiaogui_name_bbox())), cv2.COLOR_BGR2RGB)
        # save_opencv_image(self.gui_name_image, "guiname")

    def get_dagui_scene(self):
        # 2.鬼的地点：天宫、花果山、麒麟山、北俱芦洲、女娲神迹、海底迷宫一层、地狱迷宫三层等。
        pass

    def get_dagui_task(self):
        pass

    def get_game_info(self):
        def callback(hwnd, extra):
            if win32gui.IsWindowVisible(hwnd):
                win_txt = win32gui.GetWindowText(hwnd)
                # logger.info(win_txt)
                if "梦幻西游 ONLINE -" in win_txt:
                    # self.accounts.append(win_txt)
                    self.hwnd = hwnd
                    self.game_rect = autoit.win_get_pos_by_handle(hwnd)

        win32gui.EnumWindows(callback, None)
        self.set_win_foreground()
        serial_move_human(self.game_rect[0] + 104 + wind_int(10), self.game_rect[1] + 41 + wind_int(3))
        time.sleep(0.2)
        leader = self.get_player_name()
        if self.db.get("leader") != leader:
            self.db["leader"] = leader
            self.db["摄妖香"] = 0
            self.update_db()

    def get_calibrate_pos(self, x, y):
        # logger.info("虚假试探坐标:%s,%s" % (x, y))
        serial_move_human(x, y, 0)  # 首先移动鼠标到指定坐标
        # time.sleep(0.2)
        ct = time.time()
        x1 = -1
        y1 = -1
        while True:
            if time.time() - ct >= 1.5:
                logger.error("没找到光标")
                return -1, -1
            # x1, y1 = self.get_game_cursor_threshold(x, y)
            # logger.info("%s, %s" % (x1, y1))
            time.sleep(0.12)
            x2, y2 = self.get_game_cursor_threshold(x, y)  # 鼠标的虚假位置，因为通过截图得到的都可能是虚假位置，这就是漂移
            # logger.info("%s, %s" % (x2, y2))
            if x2 == y2 == -1:
                continue
            if x1 == x2 and y1 == y2:
                return x1, y1
            x1 = x2
            y1 = y2
        # time.sleep(0.5 + delay_time())
        # x1, y1 = get_game_cursor(self.game_rect)  # 鼠标的虚假位置，因为通过截图得到的都可能是虚假位置，这就是漂移

        # x1, y1 = -1, -1
        # for _ in range(3):
        #     time.sleep(0.1)
        #     x1, y1 = self.get_game_cursor_threshold(x, y)  # 鼠标的虚假位置，因为通过截图得到的都可能是虚假位置，这就是漂移
        #     if x1 > 0:
        #         break
        # return x1, y1

    def get_game_cursor_threshold(self, x, y):
        # 匹配游戏窗口光标位置
        bbox = self.cursor_bbox(x, y)
        cv_image = screen_opencv_grab(bbox)
        self.cursor_image = cv_image
        folders = ("cursors_threshold", "cursors_attack_threshold")
        for folder in folders:
            pos = match_cursor_folder_threshold(cv_image, folder)
            if pos[0] > 0:
                return bbox[0] + pos[0], bbox[1] + pos[1]
        pos = match_cursor_threshold(cv_image)
        if pos[0] > 0:
            return bbox[0] + pos[0], bbox[1] + pos[1]
        # save_opencv_image(cv_image, "nocursor")
        # logger.error("没找到光标")
        return -1, -1

    def get_map_search_button_pos(self):
        img_list = (img_map_search, img_map_search1)
        return self.wait_opencv_object_pos(self.map_bbox(), img_list, timeout=0.5)

    def get_map_max_loc(self, scene):
        max_x, max_y = 0, 0
        if scene == "长寿村":
            max_x, max_y = 159, 209
        elif scene == "傲来国":
            max_x, max_y = 223, 150
        elif scene == "朱紫国":
            max_x, max_y = 191, 119
        elif scene == "西凉女国":
            max_x, max_y = 163, 123
        elif scene == "宝象国":
            max_x, max_y = 159, 119
        elif scene == "建邺城":
            max_x, max_y = 287, 143
        elif scene == "大唐国境":
            max_x, max_y = 351, 335
        elif scene == "大唐境外":
            max_x, max_y = 638, 118
        elif scene == "江南野外":
            max_x, max_y = 159, 119
        elif scene == "女儿村":
            max_x, max_y = 127, 143
        elif scene == "普陀山":
            max_x, max_y = 95, 71
        elif scene == "五庄观":
            max_x, max_y = 99, 74
        elif scene == "地府":
            max_x, max_y = 159, 119
        elif scene == "长安城":
            max_x, max_y = 548, 278
        else:
            logger.error("失败，地图最大坐标还没添加")
        return max_x, max_y

    def goto_zhongkui(self):
        logger.info("从地府走路到钟馗")
        self.active_leader_win()
        map_pos = self.open_map()
        # 迷惑动作
        if istrue():
            self.mouse_click_human(map_pos[0] + 239 + wind_int(10), map_pos[1] + 287 + wind_int(6), mode=0)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))
        if istrue():
            self.mouse_click_human(map_pos[0] + 159 + wind_int(5), map_pos[1] + 254 + wind_int(3), mode=0)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))

        for _ in range(2):
            self.mouse_click_human(map_pos[0] + 107, map_pos[1] + 220, mode=0)
            human_click_cur()
            time.sleep(0.2)
        time.sleep(0.1)
        self.close_map()

        self.supply_myself()

        self.wait_walkfinding_finish(sheyaoxaing=False, timeout=17)

    def goto_xiaogui_scene(self):
        logger.info("飞到小鬼场景")
        self.active_leader_win()
        scene = self.db["gui_scene"]
        x, y = self.db["gui_loc"]
        turn = False
        if scene == "长寿村":
            # 合成旗、飞行符
            if (x <= 86 and y <= 13) or (x <= 62 and y <= 84) or (x <= 76 and 14 <= y <= 18):
                self.use_changshoucun777(self.changshoucun777_taibaijinxing_bbox(), move=False, turn=turn)
            elif (87 <= x and y <= 32) or (79 <= x and 12 <= y <= 19) or (62 <= x and 20 <= y <= 26):
                self.use_changshoucun777(self.changshoucun777_changshoujiaowai_bbox(), move=False, turn=turn)
            elif (x <= 73 and 85 <= y <= 127) or (x <= 82 and 128 <= y <= 135):
                self.use_changshoucun777(self.changshoucun777_dangpu_bbox(), move=False, turn=turn)
            elif 117 <= x and 124 <= y <= 162:
                self.use_changshoucun777(self.changshoucun777_lucheng_bbox(), turn=turn)
            elif x <= 77 and 136 <= y:
                self.use_changshoucun777(self.changshoucun777_zhongshusheng_bbox(), move=False, turn=turn)
            elif 78 <= x and 163 <= y:
                self.use_changshoucun777(self.changshoucun777_fangcunshan_bbox(), move=False, turn=turn)
            elif (62 <= x and 33 <= y <= 76) or (113 <= x and 77 <= y <= 87):
                input_alt_e()
                self.use_feixingfu(
                    self.feixingfu_changshoucun_bbox(), img_ghost_feixingfu_changshoucun, img_scene_changshoucun)
            else:
                self.use_changshoucun777(self.changshoucun777_qianzhuang_bbox(), turn=turn)
        elif scene == "傲来国":
            # 合成旗、飞行符
            if (x <= 74 and y <= 38) or (x <= 65 and 39 <= y <= 60) or (x <= 93 and y <= 30):
                self.use_aolaiguo777(self.aolaiguo777_yaodian_bbox(), move=False, turn=turn)
            elif (121 <= x and y <= 34) or (188 <= x and 35 <= y <= 37):
                self.use_aolaiguo777(self.aolaiguo777_donghaiwan_bbox(), turn=turn)
            elif (143 <= x and 38 <= y <= 71) or (170 <= x and 72 <= y <= 89):
                self.use_aolaiguo777(self.aolaiguo777_dangpu_bbox(), turn=turn)
            elif 167 <= x and 94 <= y:
                self.use_aolaiguo777(self.aolaiguo777_huaguoshan_bbox(), turn=turn)
            elif 71 <= x <= 166 and 72 <= y:
                input_alt_e()
                self.use_feixingfu(self.feixingfu_aolaiguo_bbox(), img_ghost_feixingfu_aolaiguo, img_scene_aolaiguo)
            elif (x <= 62 and 61 <= y <= 112) or (63 <= x <= 70 and 75 <= y <= 112):
                self.use_aolaiguo777(self.aolaiguo777_penglaixiandao_bbox(), move=False, turn=turn)
            elif x <= 70 and 113 <= y:
                self.use_aolaiguo777(self.aolaiguo777_nvercun_bbox(), move=False, turn=turn)
            else:
                self.use_aolaiguo777(self.aolaiguo777_qianzhuang_bbox(), turn=turn)
        elif scene == "朱紫国":
            # 合成旗、飞行符
            if x <= 43 and y <= 22:
                self.use_zhuziguo777(self.zhuziguo777_datangjingwai_bbox(), move=False, turn=turn)
            elif 44 <= x <= 111 and y <= 24:
                self.use_zhuziguo777(self.zhuziguo777_duanmuniangzi_bbox(), turn=turn)
            elif (112 <= x and y <= 26) or (90 <= x <= 111 and 17 <= y <= 32):
                self.use_zhuziguo777(self.zhuziguo777_sichouzhilu_bbox(), turn=turn)
            elif (112 <= x and 27 <= y <= 65) or (104 <= x <= 111 and 33 <= y <= 65):
                self.use_zhuziguo777(self.zhuziguo777_yaodian_bbox(), turn=turn)
            elif (107 <= x and 66 <= y <= 107) or (114 <= x and 108 <= y):
                input_alt_e()
                self.use_feixingfu(self.feixingfu_zhuziguo_bbox(), img_ghost_feixingfu_zhuziguo, img_scene_zhuziguo)
            elif (39 <= x <= 106 and 73 <= y) or (107 <= x <= 113 and 107 <= y):
                self.use_zhuziguo777(self.zhuziguo777_shenjidaozhang_bbox(), move=False, turn=turn)
            elif x <= 38 and 71 <= y:
                self.use_zhuziguo777(self.zhuziguo777_qilinshan_bbox(), move=False, turn=turn)
            else:
                self.use_zhuziguo777(self.zhuziguo777_jiudian_bbox(), move=False, turn=turn)
        elif scene == "西凉女国":
            # 西凉女国：1.飞行符 2.合成旗-朱紫国驿站-西凉女国(这条路复杂而且慢)
            self.use_feixingfu(self.feixingfu_xiliangnvguo_bbox(), img_ghost_feixingfu_xiliangnvguo,
                               img_scene_xiliangnvguo)
        elif scene == "宝象国":
            # 飞行符
            self.use_feixingfu(self.feixingfu_baoxiangguo_bbox(), img_ghost_feixingfu_baoxiangguo,
                               img_scene_baoxiangguo)
        elif scene == "建邺城":
            # 建邺城：1.飞行符 2.合成旗-傲来东海湾驿站-东海湾-建邺城
            self.use_feixingfu(self.feixingfu_jianyecheng_bbox(), img_ghost_feixingfu_jianyecheng,
                               img_scene_jianyecheng)
        elif scene == "大唐境外":
            # 大唐境外：1.合成旗-朱紫国左下角-大唐境外 2.合成旗-罗道人旁的驿站老板-传送大唐国境-大唐境外
            zhuziguo = True
            if x <= 504:
                if 502 <= x and 25 <= y <= 41:
                    zhuziguo = False
                elif 500 <= x and 26 <= y <= 42:
                    zhuziguo = False
                elif 499 <= x and 27 <= y <= 43:
                    zhuziguo = False
                elif 498 <= x and 28 <= y <= 43:
                    zhuziguo = False
                elif 495 <= x and 29 <= y <= 43:
                    zhuziguo = False
                elif 494 <= x <= 502 and 42 <= y <= 56:
                    zhuziguo = False
                elif 493 <= x <= 502 and 42 <= y <= 57:
                    zhuziguo = False
                elif 492 <= x <= 502 and 42 <= y <= 59:
                    zhuziguo = False
                elif 491 <= x and 42 <= y <= 60:
                    zhuziguo = False
                elif 496 <= x and 64 <= y <= 68:
                    zhuziguo = False
                elif 503 <= x and 44 <= y <= 53:
                    zhuziguo = False
            else:
                zhuziguo = False

            if zhuziguo:
                self.from_zhuziguo777_to_datangjingwai(turn=turn)
            else:
                self.use_changan777(self.changan777_yizhan_laoban_bbox(), move=False, turn=turn)
                self.from_changan_fly_to_datangguojing()
                self.from_datangguojing_to_datangjingwai()
        elif scene == "江南野外":
            # 江南野外:1.合成旗-长安右下角-江南野外 2.飞行符-建邺城-江南野外
            # if 99 <= x and y <= 76:
            #     input_alt_e()
            #     self.from_jianyecheng_to_jiangnanyewai()
            # else:
            self.from_changan777_to_jiangnanyewai(turn=turn)
        elif scene == "女儿村":
            # 女儿村：合成旗-傲来国左上角-女儿村
            self.use_aolaiguo777(self.aolaiguo777_nvercun_bbox(), turn=turn)
            near = 30
            x, y, n_x, n_y = self.get_relative_position(5, 144, "傲来国")
            if not self.mouse_click_human(x, y, n_x=near, n_y=near):
                if not self.mouse_click_human(x, y, n_x=near * 2, n_y=near):
                    self.mouse_click_human(x, y, n_x=near, n_y=near * 2)
            self.wait_opencv_object(self.scene_bbox(), img_scene_nvercun, timeout=5.0)
            time.sleep(0.15)
        elif scene == "普陀山":
            # 普陀山：合成旗-长安左下角-大唐国境-普陀接引仙女
            self.from_changan777_to_datangguojing(turn=turn)
            self.from_datangguojing_to_putuoshan()
        elif scene == "五庄观":
            # 五庄观:合成旗-长安城驿站老板-大唐国境-大唐境外-五庄观
            self.use_changan777(self.changan777_yizhan_laoban_bbox(), turn=turn)
            self.from_changan_fly_to_datangguojing()
            self.from_datangguojing_to_datangjingwai()
            self.from_datangjingwai_to_wuzhuangguan()

    def gui_loc_fail(self):
        return self.db["gui_loc"][0] == -1 or self.db["gui_loc"][1] == -1

    def handle_xiaogui_fight(self):
        # clear_serial_data()
        self.mp3_playing = False
        f_round = 0
        wait_hero_action_time = 0
        four_man = False
        while True:
            if not opencv_object_exist(self.fighting_bbox(), img_fighting, threshold=0.80) and \
                    is_foreground_window(self.game_rect) and self.get_player_name() == self.db["leader"]:
                break
            hangup = False
            if opencv_gray_object_exist(self.four_man_bbox(), img_fight_fourman_title_gray_107_255_0, threshold=0.81):
                if not four_man:
                    screenshot_fourman()
                    four_man = True
            else:
                four_man = False
            if four_man:
                for _ in range(5):
                    logger.info("***四小人弹窗验证,请手动点击***")
                if not self.mp3_playing:
                    serial_write(PLAY_MP3 % b'1')
                    logger.info("播放喇叭")
                    self.mp3_playing = True
                # time.sleep(0.3)
            else:
                if self.db["leader"] != self.get_player_name():
                    if opencv_object_exist(self.fight_action_bbox(), img_fight_auto, threshold=0.75):
                        # time.sleep(0.2)
                        if not self.click_opencv_object(self.fight_action_bbox(), img_fight_auto, n_x=-40,
                                                        threshold=0.75):
                            self.click_opencv_object(self.fight_action_bbox(), img_fight_auto, n_x=-70, threshold=0.75)
                tip_pos = self.wait_opencv_object_pos(self.four_man_tips_bbox(), img_fight_fourman_tips, timeout=0)
                if tip_pos[0] > 0:
                    serial_move_human(tip_pos[0] - 50, tip_pos[1])
                    time.sleep(0.5)
                    mouse_move_foreground_left_top(self.game_rect)
                    if opencv_object_exist(self.game_rect, img_fight_zanli_title):
                        logger.info("窗口设置暂离，忽略")
                        for _ in range(3):
                            if self.db["leader"] != self.get_player_name():
                                leader_win_pos = self.win_tab_pos[0]
                                serial_move_human(leader_win_pos[0] + wind_int(10), leader_win_pos[1] + wind_int(3))
                    elif opencv_gray_object_exist(self.four_man_bbox(), img_fight_fourman_title_gray_107_255_0,
                                                  threshold=0.81):
                        screenshot_fourman()
                        four_man = True
                    else:
                        screenshot_fourman()
                        for _ in range(3):
                            if self.db["leader"] != self.get_player_name():
                                leader_win_pos = self.win_tab_pos[0]
                                serial_move_human(leader_win_pos[0] + wind_int(10), leader_win_pos[1] + wind_int(3))
                if self.mp3_playing:
                    # serial_write(STOP_MP3)
                    # logger.info("停止喇叭")
                    self.stop_laba()
                    self.mp3_playing = False
            # self.handle_fourman()
            if self.db["leader"] == self.get_player_name():
                threshold = 0.80
                cv_img = cv2.cvtColor(np.array(screen_opencv_grab(self.game_rect)), cv2.COLOR_BGR2RGB)
                if opencv_match(cv_img, img_btn_cancel_auto_round, threshold=threshold):
                    hangup = True
                elif opencv_match(cv_img, img_btn_cancel_zhanli, threshold=threshold):
                    hangup = True
                if self.fight_waiting_for_hero_action():
                    if time.time() - wait_hero_action_time > 2.0:
                        wait_hero_action_time = time.time()
                        f_round += 1
                        self.db["round"] += 1
                        logger.info("第%s回合" % f_round)
                        self.update_db()
                        if hangup:
                            logger.info("挂机自动战斗")
                    if not hangup:
                        self.xiaogui_hero_fight_action(f_round)
                elif self.fight_waiting_for_peg_action():
                    if hangup:
                        continue
                    logger.info("宠物平A")
                    input_alt_a()

                if self.db["round"] >= random.randint(15, 24) and f_round == 1:
                    logger.info("重置自动战斗挂机剩余回合")
                    for i in range(5):
                        mouse_move_foreground_center_top(self.game_rect)
                        btn_pos = self.wait_opencv_object_pos(self.game_rect, img_btn_reset_auto_round, timeout=0.5)
                        if btn_pos[0] > 0:
                            if i == 0:
                                for _ in range(2):
                                    self.mouse_click_human(btn_pos[0], btn_pos[1], n_y=40)
                            else:
                                for _ in range(3):
                                    self.mouse_click_human(btn_pos[0], btn_pos[1])
                                    if self.wait_opencv_object(self.game_rect, img_fight_auto_round30, timeout=0.3):
                                        break
                        input_ctrl_tab()
                        time.sleep(0.5)
                        if self.get_player_name() == self.db["leader"]:
                            logger.info("重置完毕.")
                            break
                    self.db["round"] = 0
                    self.update_db()
                    self.active_leader_win()
            time.sleep(0.3)
        logger.info("战斗结束")

    def handle_dagui_fight(self):
        pass

    def handle_team_status(self):
        if not self.team:
            self.team.append(self.db["leader"])
        # reset_round = self.db["round"] >= random.randint(15, 24)
        for i in range(5):
            player_name = self.get_player_name()
            if player_name not in self.team:
                self.team.append(player_name)

            # if reset_round and self.get_player_name() != self.db["leader"]:
            #     logger.info("重置自动战斗挂机剩余回合")
            #     mouse_move_foreground_left_top(self.game_rect)
            #     btn_pos = self.wait_opencv_object_pos(self.game_rect, img_btn_reset_auto_round, timeout=0.5)
            #     if btn_pos[0] > 0:
            #         for _ in range(3):
            #             self.mouse_click_human(btn_pos[0], btn_pos[1])
            #             if self.wait_opencv_object(self.game_rect, img_fight_auto_round30, timeout=0.3):
            #                 break
            # self.handle_dazuo_time(player_name)
            self.use_dazuo_skill(player_name)

            if self.is_sect(HUASHENGSI, player_name=player_name):
                self.has_hs = True
                if self.hs_index == 0:
                    self.hs_index = i
                self.supply_hero_mana()
                self.supply_peg_health()
                # for _ in range(3):
                if self.health_supply(self.hero_health_bbox(), 90):
                    self.perform_skill("推拿")
                    # time.sleep(0.2)
            else:
                if self.health_supply(self.hero_health_bbox(), 90):
                    cv_icon = cv2.cvtColor(np.array(screen_opencv_grab(self.hero_icon_bbox())), cv2.COLOR_BGR2RGB)
                    self.health_icon[i] = cv_icon
                    logger.info("需要回血窗口:%s" % i)
                    self.supply_action[i] = 3

                if self.mana_supply(self.hero_mana_bbox(), 50):
                    logger.info("需要回蓝窗口:%s" % i)
                    self.supply_action[i] = 1
                if self.health_supply(self.peg_health_bbox(), 55):
                    logger.info("需要宠物回血窗口:%s" % i)
                    self.supply_action[i] = 2
            input_ctrl_tab()
            time.sleep(0.5)
            if self.get_player_name() == self.db["leader"]:
                logger.info("操作完毕，退出.")
                break

        # logger.info(self.team)
        self.update_db()
        # if reset_round:
        #     self.db["round"] = 0
        #     self.update_db()
        self.supply_team_health()
        logger.info("换回队长操作带路")
        self.active_leader_win()

    def supply_team_health(self):
        if not self.hs_huoxue():
            return
        if not self.has_hs:
            return
        for i in range(len(self.supply_action)):
            if self.supply_action[i] == 3:
                self.supply_action[i] = 0
        self.active_hs_win()
        mouse_move_foreground_left_bottom(self.game_rect)
        for i in range(len(self.health_icon)):
            if self.health_icon[i] is not None:
                mouse_move_foreground_center_top(self.game_rect)
                pos_list = opencv_object_pos2_list(self.team_icons_bbox(), self.health_icon[i])
                # logger.info(pos_list)
                for x, y in pos_list:
                    for _ in range(1):
                        self.perform_skill("活血")  # 活血技能需要在头像外面先按键，然后再移动到头像上才能使用
                        hit = True
                        if not self.mouse_click_human(x, y, n_y=15, mode=0):
                            logger.info("鼠标出界，重试")
                            if not self.mouse_click_human(x, y, n_y=35, mode=0):
                                logger.info("活血技能使用失败")
                                hit = False
                        serial_click_cur()
                        if not hit:
                            serial_right_click_cur()
                        serial_move_human(self.game_rect[0] + 670 + wind_int(15), self.game_rect[1] + 170 + wind_int(3),
                                          mode=0)
                # time.sleep(0.5)
                self.health_icon[i] = None

    def supply_myself(self):
        # if self.db["round"] == 0:
        #     self.click_opencv_object(self.game_rect, img_btn_reset_auto_round, n_y=40, timeout=0)

        dazuo_whitelist = [0, 0, 0, 0, 0]
        for i in range(len(self.supply_action)):
            if self.supply_action[i] == 0:
                dazuo_whitelist[i] = 1
                continue
            if i != 0:
                self.active_index_win(i)
                self.use_dazuo_skill(self.get_player_name())
            else:
                self.use_dazuo_skill(self.get_player_name())

            self.supply_action[i] = 0
            self.supply_hero_health()
            self.supply_hero_mana()
            self.supply_peg_health()
        self.update_db()
        self.handle_dazuo_skill(dazuo_whitelist)
        self.active_leader_win()

    def supply_peg_health(self):
        for _ in range(3):
            if self.health_supply(self.peg_health_bbox(), 55):
                # if self.health_supply(self.peg_health_bbox(), 85):
                logger.info("宠物补血")
                if not self.mouse_click_human(self.game_rect[0] + 869, self.game_rect[1] + 66, n_y=20, mode=2):
                    logger.info("鼠标出界，重试")
                    self.mouse_click_human(self.game_rect[0] + 869, self.game_rect[1] + 66, n_y=45, mode=2)
                mouse_move_foreground_right_top(self.game_rect)
                time.sleep(0.3)
            else:
                break

    def supply_hero_health(self):
        n_x = -50
        n_y = 30
        do_supply = False
        for _ in range(3):
            if self.health_supply(self.hero_health_bbox(), 90):
                do_supply = True
                logger.info("人物补血")
                if not self.mouse_click_human(
                        self.game_rect[0] + 985, self.game_rect[1] + 66, n_x=n_x, n_y=n_y, mode=2):
                    logger.info("鼠标出界，重试")
                    if not self.mouse_click_human(
                            self.game_rect[0] + 985, self.game_rect[1] + 66, n_x=n_x * 2, n_y=n_y, mode=2):
                        logger.info("鼠标出界，重试")
                        self.mouse_click_human(
                            self.game_rect[0] + 985, self.game_rect[1] + 66, n_x=n_x, n_y=n_y * 2, mode=2)
                mouse_move_foreground_right_top(self.game_rect)
                time.sleep(0.3)
            else:
                break
        return do_supply

    def supply_hero_mana(self):
        n_x = -50
        n_y = 30
        for _ in range(3):
            if self.mana_supply(self.hero_mana_bbox(), 50):
                # if self.mana_supply(self.hero_mana_bbox(), 80):
                logger.info("人物补蓝")
                if not self.mouse_click_human(
                        self.game_rect[0] + 985, self.game_rect[1] + 78, n_x=n_x, n_y=n_y, mode=2):
                    logger.info("鼠标出界，重试")
                    if not self.mouse_click_human(
                            self.game_rect[0] + 985, self.game_rect[1] + 78, n_x=n_x * 2, n_y=n_y, mode=2):
                        logger.info("鼠标出界，重试")
                        self.mouse_click_human(
                            self.game_rect[0] + 985, self.game_rect[1] + 78, n_x=n_x, n_y=n_y * 2, mode=2)
                mouse_move_foreground_right_top(self.game_rect)
                time.sleep(0.3)
            else:
                break

    def handle_sheyaoxiang_time(self):
        logger.info("检查摄妖香时间")
        old_time = self.db.get("摄妖香") if self.db.get("摄妖香") else 0
        now_time = int(time.time())
        if now_time - old_time >= 1770:
            logger.info("摄妖香已过时，使用摄妖香")
            self.use_beibao_prop(img_props_sheyaoxiang)
            self.db["摄妖香"] = now_time
            self.update_db()

    def handle_wrong_attack(self):
        bbox = self.npc_talk_bbox()
        image = screen_opencv_grab(bbox)

        def close_npc_btn(image_path):
            threshold = 0.89
            pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), image_path, threshold=threshold, timeout=0)
            if pos[0] > 0:
                for _ in range(8):
                    self.mouse_click_human(pos[0] + wind_int(5), pos[1], n_x=wind_int(2), n_y=wind_int(30))
                    mouse_move_foreground_center_top(self.game_rect)
                    if self.wait_opencv_object_disapper(self.npc_talk_bbox(), image_path, threshold=threshold):
                        # time.sleep(0.1)
                        break

        if opencv_object_exist2(image, img_btn_laozimeishijianwan, threshold=0.89):
            self.close_npc_talk()
        elif opencv_object_exist2(image, img_btn_wozhengzaizhandouzhong, threshold=0.89):
            self.close_npc_talk()
        elif opencv_object_exist2(image, img_btn_luguohenishuogehua, threshold=0.89):
            close_npc_btn(img_btn_luguohenishuogehua)
        elif opencv_object_exist2(image, img_btn_wohaixiangzaizhezhuanzhuan, threshold=0.89):
            close_npc_btn(img_btn_wohaixiangzaizhezhuanzhuan)
        elif opencv_object_exist2(image, img_btn_wojintianmeidaiqian_gaitianlaizhaoni, threshold=0.89):
            close_npc_btn(img_btn_wojintianmeidaiqian_gaitianlaizhaoni)
        elif opencv_object_exist2(image, img_btn_wosuibiankankan, threshold=0.89):
            close_npc_btn(img_btn_wosuibiankankan)
        elif opencv_object_exist2(image, img_btn_womenhouhuiyouqi, threshold=0.89):
            close_npc_btn(img_btn_womenhouhuiyouqi)
        elif opencv_object_exist2(image, img_btn_luanchiyao_hairenming, threshold=0.89):
            close_npc_btn(img_btn_luanchiyao_hairenming)
        elif opencv_object_exist2(image, img_btn_menpaichuanggaunhudong, threshold=0.89):
            self.close_npc_talk()
        elif opencv_object_exist2(image, img_btn_woshenmedoubuzuo, threshold=0.89):
            close_npc_btn(img_btn_woshenmedoubuzuo)

    # def handle_dazuo_time(self, player_name):
    #     now_time = int(time.time())
    #     oldtime = self.db.get("打坐") if self.db.get("打坐") else 0
    #     if self.dazuo.get(player_name) and now_time - oldtime >= 180:
    #         logger.info("打坐")
    #         input_f6()

    def handle_dazuo_skill(self, dazuo_list):
        now_time = int(time.time())
        # logger.info(self.team)
        # logger.info(dazuo_list)
        logger.info("检查打坐时间")
        if not self.team:
            logger.info("no team")
            return
        dazuo = False
        for i in range(len(dazuo_list)):
            if dazuo_list[i] != 0:
                if self.dazuo.get(self.team[i]) and now_time - self.get_last_dazuo_time(self.team[i]) > 180:
                    if self.team[i] != self.get_player_name():
                        self.active_index_win(i)
                    logger.info("打坐")
                    input_f6()
                    dazuo = True
                    db_dazuo = self.db.get("打坐") if self.db.get("打坐") else dict()
                    db_dazuo[self.team[i]] = now_time
                    self.db["打坐"] = db_dazuo
        if dazuo:
            logger.info("更新打坐时间")
            self.update_db()
            self.active_leader_win()

    def use_dazuo_skill(self, player_name):
        now_time = int(time.time())
        if self.dazuo.get(player_name) and now_time - self.get_last_dazuo_time(player_name) >= 180:
            logger.info("打坐")
            input_f6()
            db_dazuo = self.db.get("打坐") if self.db.get("打坐") else dict()
            db_dazuo[player_name] = now_time
            self.db["打坐"] = db_dazuo

    def get_last_dazuo_time(self, player_name):
        if self.db.get("打坐"):
            return self.db.get("打坐").get(player_name) if self.db.get("打坐").get(player_name) else 0
        return 0

    def health_supply(self, bbox, deadline=80):
        threshold = 0.96
        percent = 0
        cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)
        if opencv_match(cv_img, img_fight_health_100, threshold=threshold):
            # logger.info("血量100%")
            percent = 100
        elif opencv_match(cv_img, img_fight_health_95, threshold=threshold):
            # logger.info("血量95%")
            percent = 95
        elif opencv_match(cv_img, img_fight_health_90, threshold=threshold):
            # logger.info("血量90%")
            percent = 90
        elif opencv_match(cv_img, img_fight_health_85, threshold=threshold):
            # logger.info("血量85%")
            percent = 85
        elif opencv_match(cv_img, img_fight_health_80, threshold=threshold):
            # logger.info("血量80%")
            percent = 80
        elif opencv_match(cv_img, img_fight_health_75, threshold=threshold):
            # logger.info("血量75%")
            percent = 75
        elif opencv_match(cv_img, img_fight_health_70, threshold=threshold):
            # logger.info("血量70%")
            percent = 70
        elif opencv_match(cv_img, img_fight_health_65, threshold=threshold):
            # logger.info("血量65%")
            percent = 65
        elif opencv_match(cv_img, img_fight_health_60, threshold=threshold):
            # logger.info("血量60%")
            percent = 60
        elif opencv_match(cv_img, img_fight_health_55, threshold=threshold):
            # logger.info("血量55%")
            percent = 55
        elif opencv_match(cv_img, img_fight_health_50, threshold=threshold):
            # logger.info("血量50%")
            percent = 50
        if percent <= deadline:
            logger.info("血量低于%s%%" % deadline)
            return True
        return False

    def mana_supply(self, bbox, deadline=80):
        threshold = 0.95
        percent = 0
        cv_img = cv2.cvtColor(np.array(screen_opencv_grab(bbox)), cv2.COLOR_BGR2RGB)
        if opencv_match(cv_img, img_fight_mana_100, threshold=threshold):
            # logger.info("蓝100%")
            percent = 100
        elif opencv_match(cv_img, img_fight_mana_95, threshold=threshold):
            # logger.info("蓝95%")
            percent = 95
        elif opencv_match(cv_img, img_fight_mana_90, threshold=threshold):
            # logger.info("蓝90%")
            percent = 90
        elif opencv_match(cv_img, img_fight_mana_85, threshold=threshold):
            # logger.info("蓝85%")
            percent = 85
        elif opencv_match(cv_img, img_fight_mana_80, threshold=threshold):
            # logger.info("蓝80%")
            percent = 80
        elif opencv_match(cv_img, img_fight_mana_75, threshold=threshold):
            # logger.info("蓝75%")
            percent = 75
        elif opencv_match(cv_img, img_fight_mana_70, threshold=threshold):
            # logger.info("蓝70%")
            percent = 70
        elif opencv_match(cv_img, img_fight_mana_65, threshold=threshold):
            # logger.info("蓝65%")
            percent = 65
        elif opencv_match(cv_img, img_fight_mana_60, threshold=threshold):
            # logger.info("蓝60%")
            percent = 60
        elif opencv_match(cv_img, img_fight_mana_55, threshold=threshold):
            # logger.info("蓝55%")
            percent = 55
        elif opencv_match(cv_img, img_fight_mana_50, threshold=threshold):
            # logger.info("蓝50%")
            percent = 50
        if percent <= deadline:
            logger.info("蓝低于%s%%" % deadline)
            return True
        return False

    def hs_huoxue(self):
        for icon in self.health_icon:
            if icon is not None:
                return True
        logger.info("人物不需要回血")
        return False

    def is_sect(self, sect_name, player_name=""):
        if not player_name:
            player_name = self.get_player_name()
        if self.fight_cfg.get(player_name):
            return self.fight_cfg.get(player_name).get("门派") == sect_name

    def kepp_zhuogui_beibao(self):
        if self.db["gui_scene"] and self.db["gui_scene"] in ("西凉女国", "宝象国", "建邺城"):
            return False
        return True

    def scene_max_position(self):
        max_x = 1000
        max_y = 1000
        if self.db["gui_scene"] == "长寿村":
            max_x = 159
            max_y = 209
        elif self.db["gui_scene"] == "五庄观":
            max_x = 99
            max_y = 74
        elif self.db["gui_scene"] == "建邺城":
            max_x = 287
            max_y = 143
        elif self.db["gui_scene"] == "西凉女国":
            max_x = 163
            max_y = 123
        elif self.db["gui_scene"] == "宝象国":
            max_x = 159
            max_y = 119
        elif self.db["gui_scene"] == "傲来国":
            max_x = 223
            max_y = 150
        elif self.db["gui_scene"] == "朱紫国":
            max_x = 191
            max_y = 119
        elif self.db["gui_scene"] == "大唐境外":
            max_x = 638
            max_y = 118
        elif self.db["gui_scene"] == "江南野外":
            max_x = 159
            max_y = 119
        elif self.db["gui_scene"] == "普陀山":
            max_x = 95
            max_y = 71
        elif self.db["gui_scene"] == "女儿村":
            max_x = 127
            max_y = 143
        return max_x, max_y

    def map_wayfinding_position(self, x, y, length, width, max_x, max_y, side=True):
        map_search_pos = self.get_map_search_button_pos()
        if map_search_pos[0] < 0:
            logger.info("识别地图按钮失败")
            return
        x_base = map_search_pos[0] - 10  # 偏移10
        y_base = map_search_pos[1] + 60 + width  # 偏移60
        # 边缘可能点不中，修复一下
        offset = 7
        if side:
            # side 是不和目的坐标重叠，方便抓鬼的时候发起攻击
            s = 4
            if x <= offset:
                x = offset + s
            elif x >= max_x - offset:
                x = max_x - offset - s
            else:
                x = x - wind_int(s)
            if y <= offset:
                y = offset + s
            elif y >= max_y - offset:
                y = max_y - offset - s
            else:
                y = y + wind_int(s)
        else:
            if x <= offset:
                x = offset
            elif x >= max_x - offset:
                x = max_x - offset
            else:
                x = x
            if y <= offset:
                y = offset
            elif y >= max_y - offset:
                y = max_y - offset
            else:
                y = y
        x_dst = x_base + round(x * length / max_x)
        y_dst = y_base - round(y * width / max_y)
        self.mouse_click_human(x_dst, y_dst, mode=0)
        human_click_cur()

    def map_wayfinding_scene(self, x, y, length, width, max_x, max_y, side=True, rd=True):
        # 迷惑动作
        if rd and random.choice((1, 2, 3, 4, 5, 6, 7, 8)) == 1:
            self.map_wayfinding_position(x + wind_int(10), y + wind_int(10), length, width, max_x, max_y, side=side)
            human_click_cur()
            time.sleep(0.1 * random.randint(1, 3))
        self.map_wayfinding_position(x, y, length, width, max_x, max_y, side=side)

    def map_wayfinding_changshoucun(self, x, y, my_x, my_y):
        # 长寿村地图像素长宽:267, 350  最大坐标 159, 209
        # x, y = self.smart_walk(x, y, my_x, my_y, changshoucun_path)
        # self.map_wayfinding_scene(x, y, 267, 350, 159, 209, side=False)

        constant = False
        if 130 <= x and 48 <= y <= 54:
            x = 137
            y = 61
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 267, 350, 159, 209, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 10)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 267, 350, 159, 209, side=False, rd=rd)

    def map_wayfinding_wuzhuangguan(self, x, y, my_x, my_y):
        # 五庄观地图像素长宽:369, 276  最大坐标 99, 74
        # side = True
        # if x <= 31:
        #     if y <= 35:
        #         # 入口左边矩形区域
        #         y = y - 12
        #     elif 36 <= y <= 38:
        #         # 入口左边区域修正
        #         x = 15
        #         y = 26
        #     elif 39 <= y <= 45:
        #         # 左中角矩形区域(人参树左下角)
        #         x = 18
        #         y = 33
        #     elif 46 <= y <= 50:
        #         # 左中角矩形区域(人参树根下)
        #         x = 23
        #         y = 38
        #     else:
        #         # 左上角矩形区域(人参树)
        #         x = x + 16
        #         y = y - 12
        #     # side = False
        # elif 32 <= x <= 46 and y <= 19:
        #     # 右边池子左下角修正
        #     x = x - 10
        # elif 47 <= x <= 58 and y <= 19:
        #     # 两个池子中间区域修正
        #     x = 32
        #     y = 23
        # elif 32 <= x <= 48 and 20 <= y <= 32:
        #     # 右边池子右下角修正
        #     x = 48
        #     y = 12
        # elif 28 <= x <= 60 and 17 <= y <= 41:
        #     # 两个池子与正门口区域修正
        #     x = 44
        #     y = 29
        # elif 32 <= x <= 79:
        #     if y >= 19:
        #         # 中间上方矩形区域
        #         x = x - 16
        #         if y <= 33:
        #             y = 19
        #         else:
        #             y = y - 12
        #     else:
        #         # 中间下方矩形区域
        #         x = x - 12
        #     # side = False
        # elif 80 <= x:
        #     if y <= 33:
        #         # 右下角矩形区域
        #         if x <= 88:
        #             x = 72
        #             y = 15
        #         else:
        #             x = 79
        #             y = 19
        #     else:
        #         # 右上角矩形区域
        #         y = y - 11
        #     # side = False
        # x_s, y_s = my_x, my_y
        # if 81 <= x and 40 <= y:
        #     x_s, y_s = 82, 33  # 右上角矩形区域
        # elif x <= 44 and 47 <= y:
        #     x_s, y_s = 44, 39  # 人参树院子区域
        # x, y = self.smart_walk(x, y, x_s, y_s, wuzhuangguan_path)
        # self.map_wayfinding_scene(x, y, 369, 276, 99, 74, side=False)

        constant = False
        if x <= 26 and 39 <= y <= 51:
            # 左中角矩形区域(人参树左下角)
            if y == 51:
                x = 23
            elif y == 50:
                x = 21
            elif y == 49:
                x = 19
            elif y == 48:
                x = 17
            else:
                x = 15
            y = y - 13
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 369, 276, 99, 74, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 9)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 369, 276, 99, 74, side=False, rd=rd)

    def map_wayfinding_jianyecheng(self, x, y, my_x, my_y):
        # 建邺城地图像素长宽:556, 276  最大坐标 287, 143
        # x_s, y_s = 65, 30  # 落地坐标
        # x_s, y_s = my_x, my_y
        # if x <= 61 and 117 <= y:
        #     x_s, y_s = 60, 111  # 左上角区域（宠物仙子旁边）
        # elif x <= 40 and 77 <= y <= 105:
        #     x_s, y_s = 46, 73  # 李善人房子的区域
        # elif 64 <= x <= 88 and 2 <= y <= 12:
        #     x_s, y_s = 39, 82  # 兵器铺区域
        # x, y = self.smart_walk(x, y, x_s, y_s, jianyecheng_path)
        # self.map_wayfinding_scene(x, y, 556, 276, 287, 143, side=False)

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        point = astarDLL.astar(my_x, my_y, x, y, 8)
        logger.info("%s, %s" % (point.x, point.y))
        if abs(point.x - x) == 4 and abs(point.y - y) == 4:
            logger.info("智能坐标失败")
            screenshot_error("astarfial")
        self.map_wayfinding_scene(point.x, point.y, 556, 276, 287, 143, side=False, rd=rd)

    def map_wayfinding_xiliangnvguo(self, x, y, my_x, my_y):
        # 西凉女国地图像素长宽:371, 281  最大坐标 163, 123
        # x_s, y_s = 101, 28  # 落地坐标101, 28
        # x_s, y_s = my_x, my_y
        # if 101 <= x <= 122 and 56 <= y <= 79:
        #     x_s, y_s = 122, 28  # 中间喷池右边的台阶
        # x, y = self.smart_walk(x, y, x_s, y_s, xiliangnvguo_path)
        # self.map_wayfinding_scene(x, y, 371, 281, 163, 123, side=False)

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        point = astarDLL.astar(my_x, my_y, x, y, 11)
        logger.info("%s, %s" % (point.x, point.y))
        if abs(point.x - x) == 4 and abs(point.y - y) == 4:
            logger.info("智能坐标失败")
            screenshot_error("astarfial")
        self.map_wayfinding_scene(point.x, point.y, 371, 281, 163, 123, side=False, rd=rd)

    def map_wayfinding_baoxiangguo(self, x, y, my_x, my_y):
        # 宝象国地图像素长宽:441, 331  最大坐标 159, 119
        # x_s, y_s = 115, 70  # 落地坐标115, 70
        # x_s, y_s = my_x, my_y
        # x, y = self.smart_walk(x, y, x_s, y_s, baoxiangguo_path)
        # self.map_wayfinding_scene(x, y, 441, 331, 159, 119, side=False)

        constant = False
        if 22 <= x <= 25 and 98 <= y <= 108:
            x = 36
            y = 102
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 441, 331, 159, 119, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 7)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 441, 331, 159, 119, side=False, rd=rd)

    def map_wayfinding_aolaiguo(self, x, y, my_x, my_y):
        # 傲来国地图像素长宽:410, 276  最大坐标 223, 150
        # x, y = self.smart_walk(x, y, my_x, my_y, aolaiguo_path)
        # self.map_wayfinding_scene(x, y, 410, 276, 223, 150, side=False)

        constant = False
        if (89 <= x <= 102 and 105 <= y <= 112) or (89 <= x <= 108 and 113 <= y <= 120):
            x = 103
            y = 108
            constant = True
        elif 2 <= x <= 24 and 40 <= y <= 51:
            x = 10
            # y = 38
            y = y - 13
            constant = True
        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 410, 276, 223, 150, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 6)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 410, 276, 223, 150, side=False, rd=rd)

    def map_wayfinding_zhuziguo(self, x, y, my_x, my_y):
        # 朱紫国地图像素长宽:439, 276  最大坐标 191, 119
        # x_n, y_n = x, y
        # x_s, y_s = my_x, my_y
        # x, y = self.smart_walk(x, y, x_s, y_s, zhuziguo_path)
        # if 138 <= x <= 150 and 25 <= y <= 41:
        #     # 药店的楼梯区域
        #     if not (138 <= x_n <= 150 and 25 <= y_n <= 41) and 41 < my_y:
        #         x_s = x_n
        #         x, y = self.smart_walk(x, y, x_s, y_s, zhuziguo_path)
        # self.map_wayfinding_scene(x, y, 439, 276, 191, 119, side=False)

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        point = astarDLL.astar(my_x, my_y, x, y, 5)
        logger.info("%s, %s" % (point.x, point.y))
        if abs(point.x - x) == 4 and abs(point.y - y) == 4:
            logger.info("智能坐标失败")
            screenshot_error("astarfial")
        self.map_wayfinding_scene(point.x, point.y, 439, 276, 191, 119, side=False, rd=rd)

    # def map_wayfinding_datangguojing(self, x, y):
    #     # 大唐国境地图像素长宽:377, 360  最大坐标 351, 335
    #     self.map_wayfinding_position(x, y, 377, 360, 351, 335)

    def map_wayfinding_datangjingwai(self, x, y, my_x, my_y):
        # 大唐境外地图像素长宽:583, 108  最大坐标 638, 118
        # side = True
        # if x <= 508:
        #     pass
        # else:
        #     # 入口坐标633, 101
        #     if 622 <= x and 70 <= y <= 78:
        #         # 五庄观入口区域
        #         x = 626
        #         y = 82
        #         side = False
        #     elif 618 <= x and 48 <= y <= 69:
        #         # 五庄观入口区域
        #         x = 617
        #         y = 60
        #         side = False
        #     elif 612 <= x and 31 <= y <= 47 or (596 <= x and 31 <= y <= 43):
        #         # 五庄观入口区域
        #         x = 612
        #         y = 43
        #         side = False
        # x_s, y_s = my_x, my_y
        # if 22 <= x <= 73 and y <= 38:
        #     x_s, y_s = 74, 51  # 建邺城左边区域
        # elif 86 <= x and y <= 75:
        #     x_s, y_s = 72, 43  # 建邺城右边区域
        # x, y = self.smart_walk(x, y, x_s, y_s, datangjingwai_path)
        # self.map_wayfinding_scene(x, y, 583, 108, 638, 118, side=side)

        constant = False
        if (54 <= x <= 79 and 33 <= y <= 44) or (76 <= x <= 81 and 45 <= y <= 46):
            # x = 67
            x = x - 14
            y = 46
            constant = True
        elif 76 <= x <= 89 and 47 <= y <= 55:
            x = 75
            y = 55
            constant = True
        elif 90 <= x <= 96 and 49 <= y <= 60:
            x = 82
            y = 62
            constant = True
        elif 97 <= x <= 105 and 50 <= y <= 60:
            x = 91
            y = 63
            constant = True
        elif 106 <= x <= 113 and 51 <= y <= 61:
            x = 99
            y = 64
            constant = True
        elif 114 <= x <= 127 and 51 <= y <= 61:
            x = 113
            y = 64
            constant = True
        elif (129 <= x <= 141 and 51 <= y <= 62) or (142 <= x <= 144 and 51 <= y <= 66):
            x = 130
            y = 66
            constant = True
        elif 141 <= x <= 154 and 55 <= y <= 67:
            x = 140
            y = 68
            constant = True
        elif 145 <= x <= 154 and y == 68:
            # x = 140
            x = x - 14
            constant = True
        elif 147 <= x <= 156 and y == 69:
            # x = 142
            x = x - 14
            constant = True
        elif 149 <= x <= 157 and y == 70:
            # x = 143
            x = x - 14
            constant = True
        elif 149 <= x <= 157 and 71 <= y <= 72:
            # x = 143
            x = x - 14
            constant = True
        elif 151 <= x <= 157 and 73 <= y <= 85:
            x = 143
            y = 72
            constant = True
        elif 220 <= x <= 229 and 27 <= y <= 36:
            x = 215
            y = 24
            constant = True
        elif 230 <= x <= 231 and 27 <= y <= 36:
            x = 217
            y = 23
            constant = True
        elif 232 <= x <= 233 and 27 <= y <= 35:
            x = 219
            y = 22
            constant = True
        elif x == 234 and 27 <= y <= 34:
            x = 220
            y = 21
            constant = True
        elif x == 235 and 27 <= y <= 33:
            x = 221
            y = 20
            constant = True
        elif 260 <= x <= 273 and 32 <= y <= 58:
            x = 274
            y = 45
            constant = True
        elif 241 <= x <= 253 and 53 <= y <= 68:
            x = 255
            y = 61
            constant = True
        elif 320 <= x <= 328 and 38 <= y <= 61:
            x = 314
            y = 48
            constant = True
        elif 319 <= x <= 324 and 62 <= y <= 63:
            x = 310
            y = 56
            constant = True
        elif 306 <= x <= 316 and 70 <= y <= 75:
            x = 302
            y = 62
            constant = True
        elif 306 <= x <= 309 and 76 <= y <= 80:
            x = 295
            y = 72
            constant = True
        elif 296 <= x <= 305 and 81 <= y <= 95:
            x = 291
            y = 82
            constant = True
        elif 330 <= x <= 337 and 89 <= y <= 109:
            x = 323
            y = 96
            constant = True
        elif 330 <= x <= 339 and 88 <= y <= 111:
            x = 326
            y = 101
            constant = True
        elif 377 <= x <= 387 and 68 <= y <= 74:
            x = 373
            y = 81
            constant = True
        elif 388 <= x <= 407 and 68 <= y <= 74:
            x = 394
            y = 81
            constant = True
        elif 392 <= x <= 407 and 61 <= y <= 67:
            x = 406
            y = 74
            constant = True
        elif 346 <= x <= 358 and 56 <= y <= 59:
            x = 344
            y = 69
            constant = True
        elif 344 <= x <= 353 and 51 <= y <= 59:
            x = 339
            y = 64
            constant = True
        elif (344 <= x <= 359 and 47 <= y <= 59) or (356 <= x <= 359 and 45 <= y <= 46):
            x = 345
            y = 46
            constant = True
        elif 360 <= x <= 364 and 43 <= y <= 56:
            x = 350
            y = 43
            constant = True
        elif 365 <= x <= 372 and 35 <= y <= 51:
            x = 358
            y = 38
            constant = True
        elif 323 <= x <= 324 and 11 <= y <= 21:
            x = 310
            y = 24
            constant = True
        elif 325 <= x <= 351 and 17 <= y <= 22:
            x = 337
            y = 30
            constant = True
        elif 334 <= x <= 362 and 13 <= y <= 16:
            x = 348
            y = 26
            constant = True
        # 右边，从国境进来的区域
        elif (581 <= x <= 592 and 86 <= y <= 100) or (581 <= x <= 587 and 101 <= y <= 107):
            # x = 595
            x = x + 14
            y = 99
            constant = True
        elif 573 <= x <= 580 and 97 <= y <= 107:
            x = 587
            y = 110
            constant = True
        elif 575 <= x <= 580 and 95 <= y <= 96:
            x = 589
            y = 108
            constant = True
        elif 577 <= x <= 580 and 91 <= y <= 94:
            x = 591
            y = 104
            constant = True
        elif 578 <= x <= 580 and y == 90:
            x = 592
            y = 103
            constant = True
        elif 579 <= x <= 580 and 88 <= y <= 89:
            x = 593
            y = 101
            constant = True
        elif 580 <= x <= 580 and y == 87:
            x = 594
            y = 100
            constant = True
        elif 582 <= x <= 595 and 85 <= y <= 86:
            x = 596
            y = 98
            constant = True
        elif 583 <= x <= 597 and 83 <= y <= 84:
            x = 597
            y = 96
            constant = True
        elif 584 <= x <= 598 and 81 <= y <= 82:
            x = 598
            y = 94
            constant = True
        elif 585 <= x <= 598 and y == 80:
            x = 599
            y = 92
            constant = True
        elif 586 <= x <= 598 and y == 79:
            x = 600
            y = 91
            constant = True
        elif 587 <= x <= 595 and 74 <= y <= 78:
            x = 601
            y = 87
            constant = True
        elif 628 <= x and 70 <= y <= 79:
            x = 631
            y = 83
            constant = True
        elif 624 <= x <= 632 and 61 <= y <= 69:
            x = 618
            y = 74
            constant = True
        elif 622 <= x <= 631 and 46 <= y <= 60:
            x = 617
            y = 59
            constant = True
        elif (619 <= x <= 625 and 37 <= y <= 45) or (597 <= x <= 625 and 29 <= y <= 36):
            x = 611
            y = 42
            constant = True
        elif 566 <= x <= 583 and 44 <= y <= 59:
            # x = 580
            x = x + 14
            y = 57
            constant = True
        elif 567 <= x <= 585 and 41 <= y <= 43:
            # x = 581
            x = x + 14
            y = 54
            constant = True
        elif 568 <= x <= 586 and y == 40:
            # x = 582
            x = x + 14
            y = 53
            constant = True
        elif 569 <= x <= 591 and 38 <= y <= 39:
            # x = 583
            x = x + 14
            y = 51
            constant = True
        elif 572 <= x <= 593 and y == 37:
            # x = 586
            x = x + 14
            y = 50
            constant = True
        elif 573 <= x <= 595 and y == 36:
            # x = 587
            x = x + 14
            y = 49
            constant = True
        elif 574 <= x <= 596 and y == 35:
            # x = 588
            x = x + 14
            y = 48
            constant = True
        elif 575 <= x <= 596 and y == 34:
            # x = 589
            x = x + 14
            y = 47
            constant = True
        elif 576 <= x <= 596 and y == 33:
            # x = 590
            x = x + 14
            y = 46
            constant = True
        elif 577 <= x <= 596 and y == 32:
            # x = 591
            x = x + 14
            y = 45
            constant = True
        elif 579 <= x <= 596 and y == 31:
            # x = 593
            x = x + 14
            y = 44
            constant = True
        elif 534 <= x <= 544 and 71 <= y <= 96:
            x = 548
            y = 84
            constant = True
        elif 547 <= x <= 556 and 58 <= y <= 77:
            x = 561
            y = 71
            constant = True
        elif (546 <= x <= 550 and 56 <= y <= 76) or (546 <= x <= 562 and 50 <= y <= 55):
            x = 560
            y = 63
            constant = True
        elif (545 <= x <= 550 and 56 <= y <= 76) or (545 <= x <= 562 and 47 <= y <= 55):
            x = 559
            y = 60
            constant = True
        elif 546 <= x <= 562 and y == 46:
            x = 560
            y = 59
            constant = True
        elif 547 <= x <= 562 and y == 45:
            x = 561
            y = 58
            constant = True
        elif 548 <= x <= 570 and y == 44:
            x = 562
            y = 57
            constant = True
        elif 549 <= x <= 570 and y == 43:
            x = 563
            y = 56
            constant = True
        elif 550 <= x <= 570 and y == 42:
            x = 564
            y = 55
            constant = True
        elif 551 <= x <= 575 and 36 <= y <= 41:
            x = 565
            y = 49
            constant = True
        elif 552 <= x <= 575 and y == 35:
            x = 566
            y = 48
            constant = True
        elif 554 <= x <= 579 and y == 34:
            x = 568
            y = 47
            constant = True
        elif 556 <= x <= 581 and y == 33:
            x = 570
            y = 46
            constant = True
        elif 558 <= x <= 583 and y == 32:
            x = 572
            y = 45
            constant = True
        elif 559 <= x <= 586 and y == 31:
            x = 573
            y = 44
            constant = True
        elif 561 <= x <= 589 and 29 <= y <= 30:
            x = 575
            y = 42
            constant = True
        elif 562 <= x <= 596 and y == 28:
            x = 576
            y = 41
            constant = True
        elif 563 <= x <= 596 and y == 27:
            x = 577
            y = 40
            constant = True
        elif 564 <= x <= 596 and y == 26:
            x = 578
            y = 39
            constant = True
        elif 565 <= x <= 596 and y == 25:
            x = 579
            y = 38
            constant = True
        elif 568 <= x <= 596 and y == 24:
            x = 582
            y = 37
            constant = True
        elif 569 <= x <= 596 and y == 23:
            x = 583
            y = 36
            constant = True
        elif 571 <= x <= 596 and y == 22:
            x = 585
            y = 35
            constant = True
        elif 573 <= x <= 596 and y == 21:
            x = 587
            y = 34
            constant = True
        elif 576 <= x <= 604 and 20 <= y <= 24:
            x = 590
            y = 33
            constant = True
        elif 577 <= x <= 605 and 19 <= y <= 24:
            x = 591
            y = 32
            constant = True
        elif 580 <= x <= 608 and 18 <= y <= 24:
            x = 594
            y = 31
            constant = True
        elif 585 <= x <= 613 and 17 <= y <= 24:
            x = 599
            y = 30
            constant = True
        elif 590 <= x <= 618 and 16 <= y <= 24:
            x = 604
            y = 29
            constant = True
        elif 595 <= x <= 623 and 16 <= y <= 27:
            x = 609
            y = 29
            constant = True
        elif (601 <= x <= 629 and 17 <= y <= 28) or (620 <= x <= 629 and 29 <= y <= 30):
            x = 615
            y = 30
            constant = True
        elif (604 <= x <= 632 and 18 <= y <= 28) or (623 <= x <= 629 and 29 <= y <= 31):
            x = 618
            y = 31
            constant = True
        elif (620 <= x <= 638 and 24 <= y <= 31) or (610 <= x <= 638 and 22 <= y <= 24):
            x = 624
            y = 35
            constant = True

        elif 505 <= x <= 512 and 24 <= y <= 37:
            x = 519
            y = 37
            constant = True
        elif 502 <= x <= 509 and 25 <= y <= 41:
            x = 516
            y = 38
            constant = True
        elif 500 <= x <= 508 and 26 <= y <= 42:
            x = 514
            y = 39
            constant = True
        elif 499 <= x <= 507 and 27 <= y <= 43:
            x = 513
            y = 40
            constant = True
        elif 498 <= x <= 507 and 28 <= y <= 43:
            x = 512
            y = 41
            constant = True
        elif 495 <= x <= 506 and 29 <= y <= 43:
            x = 509
            y = 42
            constant = True
        elif 494 <= x <= 502 and 42 <= y <= 56:
            x = 508
            y = 43
            constant = True
        elif 493 <= x <= 502 and 42 <= y <= 57:
            x = 507
            y = 44
            constant = True
        elif 492 <= x <= 502 and 42 <= y <= 59:
            x = 506
            y = 46
            constant = True
        elif 491 <= x <= 502 and 42 <= y <= 60:
            x = 505
            y = 48
            constant = True

        elif 496 <= x <= 501 and 64 <= y <= 68:
            x = 510
            y = 64
            constant = True
        elif 503 <= x <= 519 and 44 <= y <= 53:
            x = 517
            y = 57
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 583, 108, 638, 118, side=False, rd=rd)
        else:
            if 260 <= x <= 504:
                my_x = 240
                my_y = 14
            point = astarDLL.astar(my_x, my_y, x, y, 4)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 583, 108, 638, 118, side=False, rd=rd)

    def map_wayfinding_jiangnanyewai(self, x, y, my_x, my_y):
        # 江南野外地图像素长宽:369, 273  最大坐标 159, 119
        # side = True
        # if x <= 20 and 10 <= y:
        #     # 长安入口左上角区域
        #     x = 20
        #     y = y + 12
        #     # side = False
        # elif 21 <= x <= 37:
        #     # 地图中间区域
        #     x = 21
        #     y = y + 12
        #     # side = False
        # elif 77 <= x <= 90 and 68 <= y <= 88:
        #     # 中间油菜花小路
        #     x = 76
        #     y = 72
        # elif 91 <= x <= 103 and 68 <= y <= 88:
        #     # 中间油菜花小路
        #     x = 88
        #     y = 79
        # elif 38 <= x <= 76 and 40 <= y:
        #     # 中间区域，包含中间的桥
        #     x = x - 16
        #     y = y + 12
        #     # side = False
        # elif (60 <= x <= 92 and 28 <= y <= 40) or (76 <= x <= 92 and 40 <= y <= 52):
        #     # 中间桥的周围
        #     x = 76
        #     y = 40
        # elif 37 <= x <= 76 and y <= 39:
        #     # 中下角区域
        #     x = x + 16
        #     y = y + 8
        # elif 38 <= x and 89 <= y:
        #     # 地图右上角区域
        #     x = x - 16
        #     y = y
        # elif 117 <= x and 32 <= y <= 44:
        #     # 建邺城入口右下角的小山坡优化
        #     x = x - 16
        #     y = y + 2
        # elif 77 <= x and y <= 39:
        #     # 右下角区域
        #     x = x - 16
        #     y = y + 8
        # elif 77 <= x and 40 <= y <= 75:
        #     # 右中角区域
        #     x = x - 16
        #     y = y - 8
        # elif x <= 36 and y <= 12:
        #     # 左下角区域
        #     x = 20
        #     y = 16
        # x_s, y_s = my_x, my_y
        # if 22 <= x <= 73 and y <= 38:
        #     x_s, y_s = 74, 51  # 建邺城左边区域
        # elif 86 <= x and y <= 75:
        #     x_s, y_s = 72, 43  # 建邺城右边区域
        # x, y = self.smart_walk(x, y, x_s, y_s, jiangnanyewai_path)
        # self.map_wayfinding_scene(x, y, 369, 273, 159, 119, side=False)
        constant = False
        if 22 <= x <= 28 and y <= 9:
            # 左下角区域
            x = 14
            y = 13
            constant = True
        elif 29 <= x <= 35 and y <= 15:
            # 左下角区域
            x = 21
            y = 16
            constant = True
        elif (117 <= x <= 130 and 27 <= y <= 32) or (110 <= x <= 130 and 19 <= y <= 26):
            # 建邺城入口下面的小山区域
            x = 116
            y = 32
            constant = True
        elif 120 <= x <= 132 and 33 <= y <= 42:
            # 建邺城入口下面的小山区域
            # x = 120
            x = x - 14
            constant = True
        elif 131 <= x <= 134 and 29 <= y <= 42:
            # 建邺城入口下面的小山区域
            x = 120
            y = 42
            constant = True
        elif (135 <= x <= 149 and 32 <= y <= 44) or (141 <= x <= 149 and 45 <= y <= 47):
            # 建邺城入口下面的小山区域
            # x = 135
            x = x - 14
            y = 45
            constant = True
        elif 150 <= x and 38 <= y <= 46:
            # 建邺城入口下面的小山区域
            x = 146
            y = 51
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 369, 273, 159, 119, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 3)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 369, 273, 159, 119, side=False, rd=rd)

    def map_wayfinding_putuoshan(self, x, y, my_x, my_y):
        # 普陀山地图像素长宽:369, 276  最大坐标 95, 71
        # side = True
        # if 50 <= x and y <= 39:
        #     # 下方入口的圆盘区域
        #     if y <= 24:
        #         y = y - 12
        #     else:
        #         y = 24
        #     if x <= 69:
        #         x = x + 16
        #     else:
        #         x = 69
        # elif y <= 31:
        #     # 地图下方长方形区域
        #     x = x + 16
        #     y = 9
        #     # side = False
        # elif x <= 39 and 38 <= y <= 42:
        #     # 龙女宝宝区域
        #     x = 20
        #     y = 27
        # elif x >= 54 and 52 <= y:
        #     # 右上角区域
        #     x = 77
        #     y = 52
        # elif x <= 23 and 54 <= y:
        #     # 左上角区域
        #     if 7 <= x:
        #         x = x + 16
        #     else:
        #         x = 23
        #     if y <= 66:
        #         y = y - 12
        #     else:
        #         y = 54
        # else:
        #     # 剩余区域，如果上面的区域覆盖齐全的话，这个剩余区域应该就是地图上面的长方形区域
        #     x = x + 16
        #     # y = 44
        #     # side = False
        # x_n, y_n = x, y
        # x_s, y_s = my_x, my_y
        # x, y = self.smart_walk(x, y, x_s, y_s, putuoshan_path)
        # recount = False
        # if 55 <= x_n <= 87 and 25 <= y_n <= 41:
        #     # 中间小桥区域
        #     if 55 <= x <= 87 and 25 <= y <= 41:
        #         x_s = 41
        #         y_s = 21
        #         recount = True
        # elif 29 <= x_n <= 54 and 18 <= y_n <= 31:
        #     # 小桥左边区域
        #     if 29 <= x <= 54 and 18 <= y <= 31:
        #         x_s = 28
        #         y_s = 3
        #         recount = True
        # if recount:
        #     x, y = self.smart_walk(x_n, y_n, x_s, y_s, putuoshan_path)
        # self.map_wayfinding_scene(x, y, 369, 276, 95, 71, side=False)

        constant = False
        if 58 <= x <= 89 and 26 <= y <= 37:
            # 下方入口的圆盘区域
            x = 72
            y = 24
            constant = True
        elif 54 <= x <= 57 and 25 <= y <= 37:
            # 下方入口的圆盘区域
            x = 68
            y = 24
            constant = True
        elif x <= 29 and 38 <= y <= 40:
            # 龙女宝宝区域
            x = 19
            y = 27
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 369, 276, 95, 71, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 2)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 369, 276, 95, 71, side=False, rd=rd)

    def map_wayfinding_nvercun(self, x, y, my_x, my_y):
        # 女儿村地图像素长宽:320, 360  最大坐标 127, 143
        # 入口坐标119, 12
        # if 86 <= x and y <= 39:
        #     # 入口桥右边区域，即地图右下角区域
        #     x = x + 16
        #     y = y - 12
        # elif 47 <= x <= 85 and y <= 13:
        #     # 左下角的桥的右边区域
        #     x = x + 16
        #     y = 8
        # elif 26 <= x <= 46 and 14 <= y <= 27:
        #     # 左下角的桥+桥上面的房子门口周边
        #     x = 42
        #     y = 15
        # elif 18 <= x <= 25 and y <= 29:
        #     # 左下最边边的两间房子中间区域
        #     x = 34
        #     y = 17
        # elif x <= 17 and y <= 24:
        #     # 左下最边边的房子门口区域
        #     x = 18
        #     y = 17
        # elif x <= 32 and 25 <= y <= 45:
        #     # 左下房子上面的桥区域
        #     if 3 <= x:
        #         x = 19
        #     elif x <= 2:
        #         x = x + 16
        #     y = y - 12
        # elif x <= 28 and 46 <= y <= 57:
        #     # 柳飞絮区域(左边的亭子)
        #     x = 12
        #     y = 45
        # x_s, y_s = 119, 12  # 入口坐标119, 12
        # x_s, y_s = my_x, my_y
        # # if 72 <= x and 80 <= y:
        # #     x_s, y_s = 88, 61  # 右上角区域
        # # if x <= 113 and 63 <= y <= 73:
        # #     x_s, y_s = 97, 63  # 右边的单独小桥区域
        # if x <= 71 and 90 <= y or (72 <= x and 130 <= y):
        #     x_s, y_s = 39, 82  # 左上角区域
        # elif x <= 48 and 6 <= y <= 45:
        #     x_s, y_s = 68, 2  # 左边单独小岛区域
        # elif x <= 48 and 46 <= y <= 57:
        #     x_s, y_s = 57, 70  # 左边单独小岛连接中间陆地的小亭子区域
        # x, y = self.smart_walk(x, y, x_s, y_s, nvercun_path)
        # self.map_wayfinding_scene(x, y, 320, 360, 127, 143, side=False)
        # self.map_wayfinding_scene(x, y, 320, 360, 127, 143)

        constant = False
        if x <= 14 and 103 <= y <= 110:
            x = 12
            y = 100
            constant = True

        rd = abs(my_x - x) >= 30 or abs(my_y - y) >= 30
        if constant:
            logger.info("%s, %s" % (x, y))
            self.map_wayfinding_scene(x, y, 320, 360, 127, 143, side=False, rd=rd)
        else:
            point = astarDLL.astar(my_x, my_y, x, y, 1)
            logger.info("%s, %s" % (point.x, point.y))
            if abs(point.x - x) == 4 and abs(point.y - y) == 4:
                logger.info("智能坐标失败")
                screenshot_error("astarfial")
            self.map_wayfinding_scene(point.x, point.y, 320, 360, 127, 143, side=False, rd=rd)

    def mouse_click_human(self, x, y, n_x=0, n_y=0, mode=1):
        # mode:0不点击，1左键，2右键，5ctrl+左键, 6alt+a攻击
        # logger.info("mouse_click_human:%s, %s" % (x, y))
        near_x, near_y = x + n_x, y + n_y
        fake_x, fake_y = self.get_calibrate_pos(near_x, near_y)
        if fake_x < 0:
            save_opencv_image(self.cursor_image, "nocursor_near")
            return False
        offset_x, offset_y = (fake_x - near_x), (fake_y - near_y)  # 漂移量
        # logger.info("%s,%s" % (x, y))
        # logger.info("%s,%s" % (near_x, near_y))
        # logger.info("%s,%s" % (fake_x, fake_y))
        # logger.info("%s,%s" % (offset_x, offset_y))
        if abs(offset_x) > 33 or abs(offset_y) > 33:
            logger.info("漂移量过大，进行二段移动")
            real_x = x - offset_x
            real_y = y - offset_y
            # logger.info("%s,%s" % (real_x, real_y))
            fake_x, fake_y = self.get_calibrate_pos(real_x, real_y)
            # logger.info("%s,%s" % (fake_x, fake_y))
            offset_x, offset_y = (fake_x - real_x), (fake_y - real_y)  # 漂移量
            # logger.info("%s,%s" % (offset_x, offset_y))
            if fake_x < 0:
                save_opencv_image(self.cursor_image, "nocursor_real")
                return False
        if mode == 6:
            mode = 1
            input_alt_a()
            time.sleep(0.03)
            serial_move_human(near_x + 25, near_y - 15, 0)
            time.sleep(0.03)
        real_x = x - offset_x
        real_y = y - offset_y
        serial_move_human(real_x, real_y, mode)
        time.sleep(0.03)
        # time.sleep(0.1)
        return True

    def my_location_image(self):
        bbox = self.scene_bbox()
        image = screen_opencv_grab(bbox)
        x_image = image
        y_image = image
        cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)
        # thresh = cv2.threshold(cv_img, 107, 255, cv2.THRESH_TOZERO)[1]
        comma = cv_imread(os.getcwd() + img_ghost_location_comma, 0)
        try:
            res = cv2.matchTemplate(cv_img, comma, cv2.TM_CCORR_NORMED)
        except cv2.error as e:
            logger.error(e)
            return x_image, y_image
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        # logger.info(max_val)
        # logger.info(max_loc)

        # x_bbox = (0, 0, 10, 10)
        # y_bbox = (0, 0, 10, 10)
        if max_val >= 0.83:
            comma_x = max_loc[0]
            left = cv_imread(os.getcwd() + img_ghost_location_left, 0)
            try:
                res = cv2.matchTemplate(cv_img, left, cv2.TM_CCORR_NORMED)
            except cv2.error as e:
                logger.error(e)
                return x_image, y_image
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
            # logger.info(max_loc)
            if max_val >= 0.85:
                # logger.info((max_loc[0] + left.shape[1], max_loc[1], comma_x, max_loc[1] + comma.shape[0]))
                x_image = image.crop(
                    (max_loc[0] + left.shape[1], max_loc[1] + 1, comma_x, max_loc[1] + comma.shape[0] - 1))
                # x_bbox = (bbox[0] + max_loc[0] + left.shape[1], bbox[1] + max_loc[1] + 1, bbox[0] + comma_x, bbox[1] + max_loc[1] + comma.shape[0] - 1)
            right = cv_imread(os.getcwd() + img_ghost_location_right, 0)
            try:
                res = cv2.matchTemplate(cv_img, right, cv2.TM_CCORR_NORMED)
            except cv2.error as e:
                logger.error(e)
                return x_image, y_image
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
            # logger.info(max_loc)
            if max_val >= 0.85:
                # logger.info((comma_x + comma.shape[1], max_loc[1], max_loc[0], max_loc[1] + comma.shape[0]))
                y_image = image.crop(
                    (comma_x + comma.shape[1], max_loc[1] + 1, max_loc[0], max_loc[1] + comma.shape[0] - 1))
                # y_bbox = (bbox[0] + comma_x + comma.shape[1], bbox[1] + max_loc[1] + 1, bbox[0] + max_loc[0], bbox[1] + max_loc[1] + comma.shape[0] - 1)
        # logger.info(x_loc_bbox)
        # logger.info(y_loc_bbox)
        # return x_bbox, y_bbox
        return x_image, y_image

    def next_step(self, index):
        self.gui_step.next()
        # self.db["step"] = xiaogui_step.index(index) + 1
        self.update_db()

    def need_sheyaoxiang(self):
        if self.db["gui_scene"] and self.db["gui_scene"] in ("普陀山", "江南野外", "大唐境外"):
            return True
        return False

    def close_beibao_smart(self, keep=False):
        if not keep and opencv_object_exist(self.beibao_bbox(), img_btn_beibao):
            logger.info("close_beibao_smart")
            input_alt_e()

    def open_beibao(self):
        mouse_move_foreground_center_top(self.game_rect)
        time.sleep(0.1)
        input_alt_e()
        pos = (-1, -1)
        for _ in range(5):
            logger.info("open_beibao")
            pos = self.wait_opencv_object_pos(self.beibao_bbox(), img_btn_beibao, timeout=2.0)
            if pos[0] > 0:
                if opencv_object_exist(self.beibao_bbox(), img_btn_package_prop_640):
                    self.click_opencv_object(self.beibao_bbox(), img_btn_package_prop_640)
                break
                # time.sleep(0.3)
            input_alt_e()
        return pos

    def open_map(self):
        img_list = (img_map_search, img_map_search1)
        pos = (-1, -1)
        for _ in range(5):
            logger.info("open_map")
            input_tab()
            pos = self.wait_opencv_object_pos(self.map_bbox(), img_list, timeout=2.5)
            if pos[0] > 0:
                time.sleep(0.05)
                break
            time.sleep(0.2)
        logger.info(pos)
        return pos

    def close_map(self):
        img_list = (img_map_search, img_map_search1)
        input_tab()
        for _ in range(5):
            logger.info("close_map")
            if self.wait_opencv_object_disapper(self.map_bbox(), img_list):
                break
            input_tab()
            time.sleep(0.2)

    def parse_sects(self):
        sects = dict()
        for key, value in self.fight_cfg.items():
            sects[value.get("门派")] = key
            for k, v in value.items():
                if k == "门派":
                    continue
                if v == "打坐":
                    self.dazuo[key] = 1
                    continue

                sects[v] = k.upper()
        # logger.info(sects)
        return sects

    def perform_skill(self, skill):
        key = self.sects.get(skill)
        logger.info(skill)
        if key == "F1":
            input_f1()
        elif key == "F2":
            input_f2()
        elif key == "F3":
            input_f3()
        elif key == "F4":
            input_f4()
        elif key == "F5":
            input_f5()
        elif key == "F6":
            input_f6()
        elif key == "F7":
            input_f7()
        elif key == "F8":
            input_f8()
        else:
            logger.error("没有对应的技能配置")

    def set_win_foreground(self):
        if not is_foreground_window(self.game_rect):
            serial_move_human(self.game_rect[0] + 200, self.game_rect[1] + 13)
            time.sleep(0.5)

    def smart_walk(self, x, y, x_s, y_s, map_path, x_o=13, y_o=10):
        # x, y  # 目的点
        # x_s, y_s  # 出发点
        x_v = -1 if x < x_s else 1
        y_v = -1 if y < y_s else 1
        x_n = x - x_o * x_v  # 实际点
        y_n = y - y_o * y_v
        # if x_s > x_n and x_v > 0 or (x_s < x_n and x_v < 0):
        #     x_n = x_s
        # if y_s > y_n and y_v > 0 or (y_s < y_n and y_v < 0):
        #     y_n = y_s
        side = 4
        x_n0 = -1
        y_n0 = -1
        xy_sq = 0
        find = False
        # 先x递增，后y递减
        for _ in range(x_o * 2):
            if abs(x - x_n) < side:
                continue
            # print("x_n", x_n)
            y_list = map_path.get(x_n)
            if y_list:
                for item in y_list:
                    y_m = y_n
                    if y_v > 0:
                        if y_m + y_o * 2 * y_v < item[0] or y_m > item[1]:
                            continue
                    else:
                        if y_m < item[0] or y_m + y_o * 2 * y_v > item[1]:
                            continue
                    for __ in range(y_o * 2):
                        if item[0] <= y_m <= item[1] and side <= abs(y - y_m) <= y_o:
                            sq = pow(abs(x_n - x_s), 2) + pow(abs(y_m - y_s), 2)
                            # logger.info("%s,%s %s" % (x_n, y_m, sq))
                            if sq < xy_sq or xy_sq == 0:
                                xy_sq = sq
                                x_n0 = x_n
                                y_n0 = y_m
                        y_m += y_v
            x_n += x_v
        if x_n0 == -1 or y_n0 == -1:
            # 匹配不上坐标
            logger.info("匹配不上坐标")
            x_n0 = x - side * x_v  # 实际点
            y_n0 = y - side * y_v
        logger.info("%s,%s" % (x_n0, y_n0))
        return x_n0, y_n0

    def stop_laba(self):
        logger.info("停止喇叭")
        for _ in range(5):
            serial_write(STOP_MP3)

    def talk_to_zhongkui(self):
        logger.info("和钟馗对话")
        self.active_leader_win()
        talk_ok = False
        pos = (-1, -1)
        for i in range(7):
            hide_player()
            # 钟馗--地府(44,56)
            x, y, n_x, n_y = self.get_relative_position(44, 56, "地府")
            self.mouse_click_human(x, y - 10, n_x=n_x, n_y=n_y)
            pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), img_btn_haode_wobangni, timeout=2.5)
            if pos[0] > 0:
                talk_ok = True
                break
            else:
                if opencv_object_exist(self.npc_talk_bbox(), img_btn_hefangshenshengbuyaodanglu, threshold=0.89):
                    self.close_npc_talk()
                    self.mouse_click_human(x, y - 10, n_x=n_x, n_y=n_y, mode=5)
                    mouse_move_foreground_left_bottom(self.game_rect)
                    ctrl_pos = opencv_gray_object_pos(
                        self.cursor_bbox(x, y - 10, offset=100), cv_imread(os.getcwd() + img_ghost_ctrl_zhongkui_gray_107_255_0), threshold=0.91)
                    logger.info(ctrl_pos)
                    if ctrl_pos[0] > 0:
                        self.mouse_click_human(ctrl_pos[0], ctrl_pos[1], n_x=n_x, n_y=n_y)
                        pos = self.wait_opencv_object_pos(self.npc_talk_bbox(), img_btn_haode_wobangni, timeout=2.5)
                        if pos[0] > 0:
                            talk_ok = True
                            break
                        if opencv_object_exist(
                                self.npc_talk_bbox(), img_btn_hefangshenshengbuyaodanglu, threshold=0.89):
                            self.close_npc_talk()
                mouse_move_foreground_center_top(self.game_rect)
                map_pos = self.open_map()
                logger.info(map_pos)
                self.mouse_click_human(map_pos[0] + 104, map_pos[1] + 209, mode=0)
                human_click_cur()
                time.sleep(0.1)
                self.close_map()
                self.wait_walkfinding_finish(sheyaoxaing=False, timeout=7)

        if talk_ok:
            for _ in range(8):
                self.mouse_click_human(pos[0] + wind_int(5), pos[1], n_x=wind_int(2), n_y=wind_int(30))
                if self.wait_opencv_object(self.zhongkui_task_bbox(), img_ghost_npc_chutao, timeout=2.0):
                    return
        else:
            logger.info("和钟馗对话失败，需要手动操作")

            # input_tab()
            # map_pos = self.get_map_search_button_pos()
            # self.mouse_click_human(map_pos[0] + 106, map_pos[1] + 214, mode=0)
            # human_click_cur()
            # time.sleep(0.1)
            # input_tab()
            # self.wait_walkfinding_finish(sheyaoxaing=False, timeout=5)

    def task_xiaogui_left_template(self):
        gui_scene = self.db["gui_scene"]
        template = None
        if gui_scene == "长寿村":
            template = cv_imread(os.getcwd() + img_ghost_task_changshoucun, 0)
        elif gui_scene == "傲来国":
            template = cv_imread(os.getcwd() + img_ghost_task_aolaiguo, 0)
        elif gui_scene == "朱紫国":
            template = cv_imread(os.getcwd() + img_ghost_task_zhuziguo, 0)
        elif gui_scene == "西凉女国":
            template = cv_imread(os.getcwd() + img_ghost_task_xiliangnvguo, 0)
        elif gui_scene == "宝象国":
            template = cv_imread(os.getcwd() + img_ghost_task_baoxiangguo, 0)
        elif gui_scene == "建邺城":
            template = cv_imread(os.getcwd() + img_ghost_task_jianyecheng, 0)
        elif gui_scene == "大唐境外":
            template = cv_imread(os.getcwd() + img_ghost_task_datangjingwai, 0)
        elif gui_scene == "江南野外":
            template = cv_imread(os.getcwd() + img_ghost_task_jiangnanyewai, 0)
        elif gui_scene == "女儿村":
            template = cv_imread(os.getcwd() + img_ghost_task_nvercun, 0)
        elif gui_scene == "普陀山":
            template = cv_imread(os.getcwd() + img_ghost_task_putuoshan, 0)
        elif gui_scene == "五庄观":
            template = cv_imread(os.getcwd() + img_ghost_task_wuzhuangguan, 0)
        thresh = cv2.threshold(template, 107, 255, cv2.THRESH_TOZERO)[1]
        return thresh

    def update_db(self):
        data = json.dumps(self.db)
        with open(db_file, 'w') as f:
            f.write(data)

    def use_beibao_prop(self, image, turn=True, keep=False):
        if turn:
            self.open_beibao()
        # self.wait_opencv_object_pos(self.beibao_bbox(), img_btn_beibao, timeout=3.0)
        self.click_opencv_object(self.beibao_props_bbox(), image, mode=2, timeout=3.0)
        if not keep:
            time.sleep(0.15)
            input_alt_e()

    def use_changan777(self, bbox, move=True, turn=True, keep=False):
        logger.info("使用长安合成旗")
        self.use_beibao_prop(img_props_red_777, turn=turn, keep=keep)
        if move:
            mouse_move_foreground_center_bottom(self.game_rect)
        flag_loc = self.wait_opencv_object_pos(bbox, img_btn_flag_loc, threshold=0.85, timeout=5.0)
        # self.close_beibao_smart(keep)
        # if keep:
        #     input_alt_e()
        for _ in range(6):
            self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
            if self.wait_opencv_object(self.scene_bbox(), img_scene_changancheng, timeout=1.7):
                time.sleep(0.15)
                break

    def use_zhuziguo777(self, bbox, move=True, turn=True, keep=False):
        logger.info("使用朱紫国合成旗")
        self.use_beibao_prop(img_props_white_777, turn=turn, keep=keep)
        if move:
            mouse_move_foreground_center_bottom(self.game_rect)
        flag_loc = self.wait_opencv_object_pos(bbox, img_btn_flag_loc, threshold=0.85, timeout=4.0)
        # self.close_beibao_smart(keep)
        # if keep:
        #     input_alt_e()
        for _ in range(6):
            self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
            if self.wait_opencv_object(self.scene_bbox(), img_scene_zhuziguo, timeout=1.7):
                time.sleep(0.15)
                break

    def use_changshoucun777(self, bbox, move=True, turn=True, keep=False):
        logger.info("使用长寿村合成旗")
        self.use_beibao_prop(img_props_green_777, turn=turn, keep=keep)
        if move:
            mouse_move_foreground_center_bottom(self.game_rect)
        flag_loc = self.wait_opencv_object_pos(bbox, img_btn_flag_loc, threshold=0.85, timeout=4.0)
        if flag_loc[0] < 0:
            if bbox == self.changshoucun777_qianzhuang_bbox() or bbox == self.changshoucun777_lucheng_bbox():
                flag_loc = self.wait_opencv_object_pos(
                    self.changshoucun777_lucheng_n_qiangzhuan_bbox(), img_btn_flag_loc, threshold=0.85, timeout=0.0)
        # self.close_beibao_smart(keep)
        # if keep:
        #     input_alt_e()
        for _ in range(6):
            self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
            if self.wait_opencv_object(self.scene_bbox(), img_scene_changshoucun, timeout=1.7):
                time.sleep(0.15)
                break

    def use_aolaiguo777(self, bbox, move=True, turn=True, keep=False):
        logger.info("使用傲来国合成旗")
        self.use_beibao_prop(img_props_yellow_777, turn=turn, keep=keep)
        if move:
            mouse_move_foreground_center_bottom(self.game_rect)
        flag_loc = self.wait_opencv_object_pos(bbox, img_btn_flag_loc, threshold=0.85, timeout=4.0)
        if flag_loc[0] < 0:
            if bbox == self.aolaiguo777_qianzhuang_bbox():
                flag_loc = self.wait_opencv_object_pos(
                    self.aolaiguo777_yaodian_bbox(), img_btn_flag_loc, threshold=0.85, timeout=0.0)
        # self.close_beibao_smart(keep)
        # if keep:
        #     input_alt_e()
        for _ in range(6):
            self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
            if self.wait_opencv_object(self.scene_bbox(), img_scene_aolaiguo, timeout=1.7):
                time.sleep(0.15)
                break

    def use_feixingfu(self, bbox, flag_image, scene_image):
        logger.info("使用飞行符")
        mouse_move_foreground_center_bottom(self.game_rect)
        for _ in range(2):
            input_f1()
            # time.sleep(0.1)
        flag_loc = self.wait_opencv_object_pos(bbox, flag_image, threshold=0.85, timeout=3.0)
        for _ in range(3):
            if flag_loc[0] > 0:
                break
            input_f1()
            flag_loc = self.wait_opencv_object_pos(bbox, flag_image, threshold=0.85, timeout=1.0)

        for _ in range(4):
            self.mouse_click_human(*flag_loc, n_x=wind_int(30), n_y=wind_int(30))
            if self.wait_opencv_object(self.scene_bbox(), scene_image, timeout=1.7):
                time.sleep(0.15)
                break

    def use_tianyantong_delay(self):
        if self.db["gui_scene"] and self.db["gui_scene"] in ("五庄观", "普陀山"):
            return True
        return False

    def gui_visual_distance(self):
        x_edge = 23  # 超过这个坐标，人物会在窗口中间
        y_edge = 17  # 超过这个坐标，人物会在窗口中间
        x_vis = 16  # 鬼在这个坐标范围内才会刷新
        y_vis = 34  # 窗口显示的y坐标数
        gui_loc = self.db["gui_loc"]
        my_x, my_y = self.get_my_location()
        max_x, max_y = self.scene_max_position()
        visual = False
        if my_x < x_edge:
            if my_y < y_edge:
                if gui_loc[0] < x_vis and gui_loc[1] < y_vis:
                    logger.info("在可视范围内")
                    visual = True
            elif abs(max_y - my_y) < y_edge:
                if gui_loc[0] < x_vis and abs(max_y - gui_loc[1]) < y_vis:
                    logger.info("在可视范围内")
                    visual = True
            else:
                if gui_loc[0] < x_vis and abs(my_y - gui_loc[1]) < y_edge:
                    logger.info("在可视范围内")
                    visual = True
        elif abs(max_x - my_x) < x_edge:
            if my_y < y_edge:
                if abs(max_x - gui_loc[0]) < x_vis and gui_loc[1] < y_vis:
                    logger.info("在可视范围内")
                    visual = True
            elif abs(max_y - my_y) < y_edge:
                if abs(max_x - gui_loc[0]) < x_vis and abs(max_y - gui_loc[1]) < y_vis:
                    logger.info("在可视范围内")
                    visual = True
            else:
                if abs(max_x - gui_loc[0]) < x_vis and abs(my_y - gui_loc[1]) < y_edge:
                    logger.info("在可视范围内")
                    visual = True
        elif my_y < y_edge:
            if gui_loc[1] < y_vis and abs(gui_loc[0] - my_x) < x_edge:
                logger.info("在可视范围内")
                visual = True
        elif abs(max_y - my_y) < y_edge:
            if abs(max_y - gui_loc[1]) < y_vis and abs(gui_loc[0] - my_x) < x_edge:
                logger.info("在可视范围内")
                visual = True
        else:
            if abs(my_x - gui_loc[0]) < x_edge and abs(my_y - gui_loc[1]) < y_edge:
                logger.info("在可视范围内")
                visual = True
        if visual:
            logger.info("小鬼在可视范围内，不需要导航移动")
            # time.sleep(0.5)  # 等一会加载，隐藏玩家
        return visual

    def click_opencv_object(self, bbox, image_object_path, x_fix=0, y_fix=0, n_x=0, n_y=0, mode=1, threshold=0.95,
                            timeout=0.5):
        logger.info(image_object_path)
        pos = self.wait_opencv_object_pos(bbox, image_object_path, threshold=threshold, timeout=timeout)
        if pos[0] < 0:
            screenshot_error()
            return False
        res = self.mouse_click_human(pos[0] + x_fix, pos[1] + y_fix, n_x=n_x, n_y=n_y, mode=mode)
        # logger.info(res)
        return res

    def click_opencv_dynamic_object(self, bbox, dictionary, mode=1, threshold=0.95):
        img = screen_opencv_grab(bbox)
        for file_path, _, files in os.walk(dictionary):
            for file in files:
                pos = opencv_dynamic_object_pos(img, os.path.join(file_path, file), threshold=threshold)
                if pos[0] > 0:
                    self.mouse_click_human(bbox[0] + pos[0], bbox[1] + pos[1], n_x=wind_int(30), n_y=wind_int(30),
                                           mode=mode)
                    return True
        return False

    def wait_opencv_object_pos(self, bbox, image, timeout=2.0, threshold=0.95):
        pos = (-1, -1)
        ct = time.time()
        if isinstance(image, list) or isinstance(image, tuple):
            while True:
                for img in image:
                    pos = opencv_object_pos(bbox, img, threshold=threshold)
                    if pos[0] > 0:
                        return bbox[0] + pos[0], bbox[1] + pos[1]
                if timeout == 0:
                    break
                elif time.time() - ct >= timeout:
                    logger.info("超时: %s" % image[0])
                    break
        else:
            while True:
                pos = opencv_object_pos(bbox, image, threshold=threshold)
                if pos[0] > 0:
                    return bbox[0] + pos[0], bbox[1] + pos[1]
                if timeout == 0:
                    break
                elif time.time() - ct >= timeout:
                    logger.info("超时: %s" % image)
                    break
        return pos

    def wait_opencv_gray_object(self, bbox, image, timeout=1.0, threshold=0.97):
        find = False
        ct = time.time()
        while True:
            if opencv_gray_object_exist(bbox, image, threshold=threshold):
                find = True
                break
            if timeout == 0 or time.time() - ct >= timeout:
                break
        if not find:
            logger.error("等待超时:%s" % image)
        return find

    def wait_opencv_object(self, bbox, image, timeout=1.0, threshold=0.97):
        find = False
        ct = time.time()
        while True:
            if opencv_object_exist(bbox, image, threshold=threshold):
                find = True
                break
            if timeout == 0 or time.time() - ct >= timeout:
                break
        if not find:
            logger.error("等待超时:%s" % image)
        return find

    def wait_opencv_object_disapper(self, bbox, image, timeout=1.0, threshold=0.97):
        disapper = False
        ct = time.time()
        if isinstance(image, list) or isinstance(image, tuple):
            while True:
                for img in image:
                    if not opencv_object_exist(bbox, img, threshold=threshold):
                        return True
                if timeout == 0:
                    break
                elif time.time() - ct >= timeout:
                    logger.info("超时: %s" % image[0])
                    break
        else:
            while True:
                if not opencv_object_exist(bbox, image, threshold=threshold):
                    disapper = True
                if timeout == 0 or time.time() - ct >= timeout:
                    break
            if not disapper:
                logger.error("等待超时:%s" % image)
        return disapper

    def wait_opencv_object_from_disapper(self, bbox, image, timeout=1.0, threshold=0.97):
        # 在超时时间内，检查对象是否还没消失,消失则返回False并中断检查.规定时间内没消失则返回True
        ct = time.time()
        while True:
            if opencv_object_exist(bbox, image, threshold=threshold):
                find = False
            else:
                find = True
                break
            if timeout == 0 or time.time() - ct >= timeout:
                break
        return not find

    def wait_walkfinding_finish(self, sheyaoxaing=True, timeout=100.0, scene=""):
        logger.info("wait_walkfinding_finish")
        ct = time.time()
        ct_syx = 0
        ct_dazuo = 0
        dazuo = False
        if self.db["gui_scene"] in ("长寿村", "傲来国", "朱紫国", "西凉女国", "宝象国", "建邺城", "女儿村", "五庄观"):
            sheyaoxaing = False
        if scene in ("大唐国境", "大唐境外"):
            dazuo = True
            sheyaoxaing = True
        elif self.db["gui_loc"][0] != -1 and self.db["gui_loc"][1] != -1:
            if self.db["gui_scene"] == "大唐境外":
                if not (self.db["gui_loc"][0] <= 80 and 20 <= self.db["gui_loc"][1] <= 85) or \
                        not (577 <= self.db["gui_loc"][0] and 70 <= self.db["gui_loc"][1]):
                    dazuo = True
            elif self.db["gui_scene"] == "五庄观":
                if not (self.db["gui_loc"][0] <= 55 and self.db["gui_loc"][1] <= 35):
                    dazuo = True
            elif self.db["gui_scene"] == "普陀山":
                if not (self.db["gui_loc"][0] >= 40 and self.db["gui_loc"][1] <= 24):
                    dazuo = True
            elif self.db["gui_scene"] == "江南野外":
                if not (self.db["gui_loc"][0] <= 90 and self.db["gui_loc"][1] >= 60):
                    dazuo = True
            elif self.db["gui_scene"] == "女儿村":
                if not (self.db["gui_loc"][0] >= 65 and self.db["gui_loc"][1] <= 49):
                    dazuo = True
            elif self.db["gui_scene"] == "建邺城":
                if not (self.db["gui_loc"][0] <= 120 and self.db["gui_loc"][1] <= 80):
                    dazuo = True
            elif self.db["gui_scene"] == "宝象国":
                if not (self.db["gui_loc"][0] >= 55 and self.db["gui_loc"][1] >= 20):
                    dazuo = True
            elif self.db["gui_scene"] == "西凉女国":
                if not (self.db["gui_loc"][0] >= 40 and self.db["gui_loc"][1] <= 65):
                    dazuo = True
        while True:
            finish = False
            for _ in range(3):
                finish = False
                my_x1, my_y1 = self.get_my_location(log=False)
                time.sleep(0.06)
                my_x2, my_y2 = self.get_my_location(log=False)
                if my_x1 == my_x2 and my_y1 == my_y2:
                    finish = True
                else:
                    break
                # cv_img1 = cv2.cvtColor(np.array(screen_opencv_grab(self.scene_bbox())), cv2.COLOR_BGR2RGB)
                # time.sleep(0.2)
                # cv_img2 = cv2.cvtColor(np.array(screen_opencv_grab(self.scene_bbox())), cv2.COLOR_BGR2RGB)
                # res = cv2.matchTemplate(cv_img1, cv_img2, cv2.TM_CCOEFF_NORMED)
                # min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
                # logger.info(max_val)
                # if max_val >= 0.97:
                #     finish = True
                # else:
                #     break
            if finish:
                logger.info("导航走路结束")
                self.close_beibao_smart()
                # time.sleep(0.2)
                return True
            if sheyaoxaing and time.time() - ct_syx >= 10:
                # 10秒检查一次摄妖香
                ct_syx = time.time()
                self.handle_sheyaoxiang_time()
            if dazuo and time.time() - ct_dazuo >= 5:
                ct_dazuo = time.time()
                self.handle_dazuo_skill([1, 1, 1, 1, 1])
            if timeout == 0:
                break
            if time.time() - ct >= timeout:
                logger.info("导航走路超时")
                break
        time.sleep(0.4)
        self.close_beibao_smart()
        return False

    def wait_fighting(self):
        logger.info("等待进入战斗")
        if self.mp3_playing:
            serial_write(PLAY_MP3 % b'1')
            logger.info("播放喇叭")
        while True:
            if opencv_object_exist(self.fighting_bbox(), img_fighting, threshold=0.85):
                logger.info("进入战斗成功")
                if self.mp3_playing:
                    # serial_write(STOP_MP3)
                    # logger.info("停止喇叭")
                    self.stop_laba()
                    self.mp3_playing = False
                # time.sleep(0.3)
                break
            else:
                time.sleep(1)

    def walk_to_xiaogui(self, timeout=90.0):
        logger.info("走路到小鬼旁边")
        self.active_leader_win()
        gui_scene = self.db["gui_scene"]
        gui_loc = self.db["gui_loc"]
        space_x = 16  # 鬼在这个坐标范围内才会刷新
        space_y = 14
        my_x, my_y = self.get_my_location()
        if abs(my_x - gui_loc[0]) <= space_x and abs(my_y - gui_loc[1]) <= space_y:
            logger.info("小鬼在可视范围内")
            move = False
            side = 4
            for i in range(4):
                if gui_scene == "大唐境外" and 610 <= gui_loc[0] and 96 <= gui_loc[1]:
                    # 大唐国境入口附近的坐标不知道为什么像素计算不准确，移动一下试试
                    move = True
                    self.open_map()
                    self.map_wayfinding_position(gui_loc[0] - side, gui_loc[1] - side, 583, 108, 638, 118, side=False)
                    self.close_map()
                    self.wait_walkfinding_finish(timeout=8)
                    hide_player_n_stalls()
                    break
                if (my_x == gui_loc[0] and my_y == gui_loc[1]) or (abs(my_x - gui_loc[0]) <= 1 and -2 <= my_y - gui_loc[1] <= 0):
                    logger.info("和小鬼坐标重叠了，移动到一旁")
                    move = True
                    spec = False
                    mx = gui_loc[0]
                    my = gui_loc[1]

                    if gui_scene == "长寿村":
                        if 127 <= gui_loc[0] and gui_loc[1] <= 28:
                            # 长寿郊外
                            mx = gui_loc[0] - side
                            my = gui_loc[1] + side
                            spec = True
                        elif 105 <= gui_loc[0] <= 111 and 181 <= gui_loc[1]:
                            # 方寸山
                            my = gui_loc[1] - side
                            spec = True
                    elif gui_scene == "傲来国":
                        if gui_loc[0] <= 32 and 122 <= gui_loc[1]:
                            # 女儿村
                            mx = gui_loc[0] + side
                            my = gui_loc[1] - side
                            spec = True
                        elif 191 <= gui_loc[0] and 122 <= gui_loc[1]:
                            # 花果山
                            mx = gui_loc[0] - side
                            my = gui_loc[1] - side
                            spec = True
                    elif gui_scene == "朱紫国":
                        if gui_loc[0] <= 32 and 91 <= gui_loc[1]:
                            # 麒麟山
                            mx = gui_loc[0] + side
                            my = gui_loc[1] - side
                            spec = True
                        elif gui_loc[0] <= 32 and gui_loc[1] <= 28:
                            # 大唐境外
                            mx = gui_loc[0] + side
                            my = gui_loc[1] + side
                            spec = True
                        elif 120 <= gui_loc[0] <= 184 and gui_loc[1] <= 19:
                            # 丝绸之路
                            mx = gui_loc[1] - side
                            spec = True
                    elif gui_scene == "大唐境外":
                        if 606 <= gui_loc[0] and 90 <= gui_loc[1]:
                            # 大唐国境
                            mx = gui_loc[0] - side
                            my = gui_loc[1] - side
                            spec = True
                    elif gui_scene == "江南野外":
                        if gui_loc[0] <= 29 and 105 <= gui_loc[1]:
                            # 长安城
                            my = gui_loc[1] - side
                            spec = True
                        elif 144 <= gui_loc[0] and 49 <= gui_loc[1] <= 62:
                            # 建邺城
                            mx = gui_loc[0] - side
                            spec = True
                    elif gui_scene == "女儿村":
                        if 95 <= gui_loc[0] and gui_loc[1] <= 28:
                            # 傲来国
                            mx = gui_loc[0] - side
                            my = gui_loc[1] + side
                            spec = True
                    elif gui_scene == "五庄观":
                        if gui_loc[0] <= 32 and gui_loc[1] <= 28:
                            # 大唐境外
                            mx = gui_loc[0] + side
                            my = gui_loc[1] + side
                            spec = True

                    if not spec:
                        mx = gui_loc[0] + side
                        my = gui_loc[1] + side

                        # if i == 0:
                        #     serial_move_human(x + 40, y - 40)
                        # elif i == 1:
                        #     serial_move_human(x - 40, y + 40)
                        # elif i == 2:
                        #     serial_move_human(x - 40, y - 40)
                        # else:
                        #     serial_move_human(x + 40, y + 40)
                    self.open_map()
                    if gui_scene == "长寿村":
                        self.map_wayfinding_position(mx, my, 267, 350, 159, 209, side=False)
                    elif gui_scene == "傲来国":
                        self.map_wayfinding_position(mx, my, 410, 276, 223, 150, side=False)
                    elif gui_scene == "朱紫国":
                        self.map_wayfinding_position(mx, my, 439, 276, 191, 119, side=False)
                    elif gui_scene == "西凉女国":
                        self.map_wayfinding_position(mx, my, 371, 281, 163, 123, side=False)
                    elif gui_scene == "宝象国":
                        self.map_wayfinding_position(mx, my, 441, 331, 159, 119, side=False)
                    elif gui_scene == "建邺城":
                        self.map_wayfinding_position(mx, my, 556, 276, 287, 143, side=False)
                    elif gui_scene == "大唐境外":
                        self.map_wayfinding_position(mx, my, 583, 108, 638, 118, side=False)
                    elif gui_scene == "江南野外":
                        self.map_wayfinding_position(mx, my, 369, 273, 159, 119, side=False)
                    elif gui_scene == "女儿村":
                        self.map_wayfinding_position(mx, my, 320, 360, 127, 143, side=False)
                    elif gui_scene == "普陀山":
                        self.map_wayfinding_position(mx, my, 369, 276, 95, 71, side=False)
                    elif gui_scene == "五庄观":
                        self.map_wayfinding_position(mx, my, 369, 276, 99, 74, side=False)
                    self.close_map()
                    self.wait_walkfinding_finish(timeout=5)
                    # self.handle_wrong_attack()
                    hide_player_n_stalls()
                    my_x, my_y = self.get_my_location()
                else:
                    break
            if not move:
                if gui_scene in ("大唐境外", "江南野外", "女儿村", "五庄观"):
                    hide_player()
                else:
                    hide_stalls()
            return
        # if self.gui_visual_distance():
        #     hide_stalls()
        #     return
        sxy = False
        for _ in range(2):
            self.open_map()
            if gui_scene == "长寿村":
                self.map_wayfinding_changshoucun(*gui_loc, my_x, my_y)
            elif gui_scene == "傲来国":
                self.map_wayfinding_aolaiguo(*gui_loc, my_x, my_y)
            elif gui_scene == "朱紫国":
                self.map_wayfinding_zhuziguo(*gui_loc, my_x, my_y)
            elif gui_scene == "西凉女国":
                self.map_wayfinding_xiliangnvguo(*gui_loc, my_x, my_y)
            elif gui_scene == "宝象国":
                self.map_wayfinding_baoxiangguo(*gui_loc, my_x, my_y)
            elif gui_scene == "建邺城":
                self.map_wayfinding_jianyecheng(*gui_loc, my_x, my_y)
            elif gui_scene == "大唐境外":
                self.handle_sheyaoxiang_time()
                # time.sleep(0.1)
                self.map_wayfinding_datangjingwai(*gui_loc, my_x, my_y)
                timeout = 150.0
                sxy = True
            elif gui_scene == "江南野外":
                self.handle_sheyaoxiang_time()
                # time.sleep(0.1)
                self.map_wayfinding_jiangnanyewai(*gui_loc, my_x, my_y)
                sxy = True
            elif gui_scene == "女儿村":
                self.map_wayfinding_nvercun(*gui_loc, my_x, my_y)
            elif gui_scene == "普陀山":
                self.handle_sheyaoxiang_time()
                # time.sleep(0.1)
                self.map_wayfinding_putuoshan(*gui_loc, my_x, my_y)
                sxy = True
            elif gui_scene == "五庄观":
                self.map_wayfinding_wuzhuangguan(*gui_loc, my_x, my_y)

            time.sleep(0.1)
            self.close_map()

            self.wait_walkfinding_finish(sheyaoxaing=sxy, timeout=timeout)
            my_x, my_y = self.get_my_location()
            if abs(my_x - gui_loc[0]) <= space_x and abs(my_y - gui_loc[1]) <= space_y:
                break
            else:
                logger.info("小鬼不在可视范围内，重新导航")
        hide_player_n_stalls()

    def xiaogui_hero_fight_action(self, f_round):
        # boss_pos = (385, 335)  # 主怪的坐标，怪物战斗的坐标都是固定的
        boss_pos = (self.game_rect[0] + 384, self.game_rect[1] + 332)
        # time.sleep(0.1)
        if f_round == 1:
            # 点主怪放天崩地裂
            # logger.info("第一回合，点主怪放天崩地裂")
            # self.perform_skill("天崩地裂")
            # self.perform_skill("浪涌")
            logger.info("人物alt+q")
            input_alt_q()
            # self.mouse_click_human(boss_pos[0] + wind_int(10), boss_pos[1] + wind_int(5), mode=0)
            # serial_click_cur()
            time.sleep(0.1)
            logger.info("人物平A")
            input_alt_a()
            mouse_move_foreground_center_top(self.game_rect)
        elif f_round == 2:
            if self.is_sect(LINGBOCHENG):
                self.perform_skill("裂石")
                # input_alt_q()  # 将队长凌波城的默认技能设置为裂石，施放就简单了
                # self.perform_skill("天崩地裂")
                self.mouse_click_human(boss_pos[0] + wind_int(10), boss_pos[1] + wind_int(5), mode=0)
                serial_click_cur()

                time.sleep(0.1)
                input_alt_a()  # 假如没点中，则平A
            elif self.is_sect(DATANGGUANFU) or self.is_sect(WUDIDONG):
                logger.info("人物alt+q")
                input_alt_q()
            else:
                logger.info("人物alt+q")
                input_alt_q()
                time.sleep(0.1)
                logger.info("人物平A")
                input_alt_a()
            # time.sleep(0.1)
            # input_alt_a()  # 宠物攻击
            mouse_move_foreground_center_top(self.game_rect)
        elif f_round >= 3:
            if self.is_sect(LINGBOCHENG):
                logger.info("人物平A")
            elif self.is_sect(DATANGGUANFU) or self.is_sect(WUDIDONG):
                logger.info("人物alt+q")
                input_alt_q()
            else:
                logger.info("人物alt+q")
                input_alt_q()
                time.sleep(0.1)
                logger.info("人物平A")
                input_alt_a()
            # time.sleep(0.1)
            # input_alt_a()
        # time.sleep(0.3)
        # self.click_opencv_object(self.fight_action_bbox(), img_fight_auto)  # 点击自动

    def beibao_bbox(self):
        return self.game_rect[0] + 400, self.game_rect[1] + 180, self.game_rect[0] + 650, self.game_rect[1] + 380

    # def beibao_props_bbox(self, beibao_pos):
    #     return beibao_pos[0] - 13, beibao_pos[1] + 49, beibao_pos[0] + 239, beibao_pos[1] + 250

    def beibao_props_bbox(self):
        return self.game_rect[0] + 100, self.game_rect[1] + 100, self.game_rect[0] + 950, self.game_rect[1] + 750

    def cursor_bbox(self, x, y, offset=160):
        bbox = [x - offset, y - offset, x + offset, y + offset]
        if bbox[0] < self.game_rect[0]:
            bbox[0] = self.game_rect[0]
        if bbox[1] < self.game_rect[1]:
            bbox[1] = self.game_rect[1]
        if bbox[2] > self.game_rect[2]:
            bbox[2] = self.game_rect[2]
        if bbox[3] > self.game_rect[3]:
            bbox[3] = self.game_rect[3]
        if self.game_rect[0] >= bbox[2] or self.game_rect[1] >= bbox[3] or self.game_rect[2] <= bbox[0] or \
                self.game_rect[3] <= bbox[1]:
            bbox = [0, 0, 10, 10]
        return bbox

    def changan777_yizhan_laoban_bbox(self):
        logger.info("长安合成旗-驿站老板")
        return self.game_rect[0] + 495, self.game_rect[1] + 515, self.game_rect[0] + 540, self.game_rect[1] + 555

    def changan777_datangguojing_bbox(self):
        logger.info("长安合成旗-大唐国境")
        return self.game_rect[0] + 237, self.game_rect[1] + 555, self.game_rect[0] + 270, self.game_rect[1] + 587

    def changan777_jiangnanyewai_bbox(self):
        logger.info("长安合成旗-江南野外")
        return self.game_rect[0] + 750, self.game_rect[1] + 555, self.game_rect[0] + 798, self.game_rect[1] + 587

    def changshoucun777_qianzhuang_bbox(self):
        # 落地坐标:125,110
        logger.info("长寿村合成旗-钱庄")
        return self.game_rect[0] + 550, self.game_rect[1] + 340, self.game_rect[0] + 650, self.game_rect[1] + 405

    def changshoucun777_lucheng_bbox(self):
        # 落地坐标:144,141
        logger.info("长寿村合成旗-鲁成")
        return self.game_rect[0] + 550, self.game_rect[1] + 405, self.game_rect[0] + 650, self.game_rect[1] + 470

    def changshoucun777_lucheng_n_qiangzhuan_bbox(self):
        # 落地坐标:144,141
        logger.info("长寿村合成旗-鲁成+钱庄")
        return self.game_rect[0] + 550, self.game_rect[1] + 340, self.game_rect[0] + 650, self.game_rect[1] + 470

    def changshoucun777_fangcunshan_bbox(self):
        # 落地坐标:108,203
        logger.info("长寿村合成旗-方寸山")
        return self.game_rect[0] + 515, self.game_rect[1] + 250, self.game_rect[0] + 650, self.game_rect[1] + 345

    def changshoucun777_zhongshusheng_bbox(self):
        # 落地坐标:45,160
        logger.info("长寿村合成旗-钟书生")
        return self.game_rect[0] + 385, self.game_rect[1] + 265, self.game_rect[0] + 515, self.game_rect[1] + 385

    def changshoucun777_dangpu_bbox(self):
        # 落地坐标:25,112
        logger.info("长寿村合成旗-当铺")
        return self.game_rect[0] + 385, self.game_rect[1] + 385, self.game_rect[0] + 500, self.game_rect[1] + 510

    def changshoucun777_taibaijinxing_bbox(self):
        # 落地坐标:44,23
        logger.info("长寿村合成旗-太白金星")
        return self.game_rect[0] + 385, self.game_rect[1] + 510, self.game_rect[0] + 520, self.game_rect[1] + 625

    def changshoucun777_changshoujiaowai_bbox(self):
        # 落地坐标:144,6
        logger.info("长寿村合成旗-长寿郊外")
        return self.game_rect[0] + 520, self.game_rect[1] + 510, self.game_rect[0] + 650, self.game_rect[1] + 625

    def aolaiguo777_nvercun_bbox(self):
        # 落地坐标:8,141
        logger.info("傲来国合成旗-女儿村")
        return self.game_rect[0] + 305, self.game_rect[1] + 295, self.game_rect[0] + 360, self.game_rect[1] + 350

    def aolaiguo777_qianzhuang_bbox(self):
        # 落地坐标:105,55
        logger.info("傲来国合成旗-钱庄")
        return self.game_rect[0] + 440, self.game_rect[1] + 430, self.game_rect[0] + 575, self.game_rect[1] + 535

    def aolaiguo777_penglaixiandao_bbox(self):
        # 落地坐标:24,97
        logger.info("傲来国合成旗-蓬莱仙岛")
        return self.game_rect[0] + 305, self.game_rect[1] + 370, self.game_rect[0] + 450, self.game_rect[1] + 460

    def aolaiguo777_yaodian_bbox(self):
        # 落地坐标:48,28
        logger.info("傲来国合成旗-药店")
        return self.game_rect[0] + 305, self.game_rect[1] + 460, self.game_rect[0] + 450, self.game_rect[1] + 580

    def aolaiguo777_donghaiwan_bbox(self):
        # 落地坐标:165,14
        logger.info("傲来国合成旗-东海湾")
        return self.game_rect[0] + 550, self.game_rect[1] + 500, self.game_rect[0] + 730, self.game_rect[1] + 580

    def aolaiguo777_dangpu_bbox(self):
        # 落地坐标:185,62
        logger.info("傲来国合成旗-当铺")
        return self.game_rect[0] + 550, self.game_rect[1] + 405, self.game_rect[0] + 730, self.game_rect[1] + 500

    def aolaiguo777_huaguoshan_bbox(self):
        # 落地坐标:215,143
        logger.info("傲来国合成旗-花果山")
        return self.game_rect[0] + 630, self.game_rect[1] + 280, self.game_rect[0] + 730, self.game_rect[1] + 410

    def zhuziguo777_datangjingwai_bbox(self):
        # 落地坐标:6,3
        logger.info("朱紫国合成旗-大唐境外")
        return self.game_rect[0] + 290, self.game_rect[1] + 545, self.game_rect[0] + 350, self.game_rect[1] + 585

    def zhuziguo777_qilinshan_bbox(self):
        # 落地坐标:4,110
        logger.info("朱紫国合成旗-麒麟山")
        return self.game_rect[0] + 285, self.game_rect[1] + 300, self.game_rect[0] + 380, self.game_rect[1] + 400

    def zhuziguo777_shenjidaozhang_bbox(self):
        # 落地坐标:72,98
        logger.info("朱紫国合成旗-神机道人")
        return self.game_rect[0] + 380, self.game_rect[1] + 300, self.game_rect[0] + 615, self.game_rect[1] + 415

    def zhuziguo777_jiudian_bbox(self):
        # 落地坐标:53,42
        logger.info("朱紫国合成旗-酒店")
        return self.game_rect[0] + 300, self.game_rect[1] + 415, self.game_rect[0] + 540, self.game_rect[1] + 520

    def zhuziguo777_duanmuniangzi_bbox(self):
        # 落地坐标:84,9
        logger.info("朱紫国合成旗-端木娘子")
        return self.game_rect[0] + 430, self.game_rect[1] + 510, self.game_rect[0] + 550, self.game_rect[1] + 585

    def zhuziguo777_yaodian_bbox(self):
        # 落地坐标:147,43
        logger.info("朱紫国合成旗-药店")
        return self.game_rect[0] + 540, self.game_rect[1] + 410, self.game_rect[0] + 740, self.game_rect[1] + 520

    def zhuziguo777_sichouzhilu_bbox(self):
        # 落地坐标:151,12
        logger.info("朱紫国合成旗-丝绸之路")
        return self.game_rect[0] + 560, self.game_rect[1] + 510, self.game_rect[0] + 740, self.game_rect[1] + 585

    def feixingfu_baoxiangguo_bbox(self):
        # 落地坐标:0,0
        logger.info("飞行符-宝象国")
        return self.game_rect[0] + 385, self.game_rect[1] + 450, self.game_rect[0] + 430, self.game_rect[1] + 495

    def feixingfu_xiliangnvguo_bbox(self):
        # 落地坐标:0,0
        logger.info("飞行符-西凉女国")
        return self.game_rect[0] + 395, self.game_rect[1] + 370, self.game_rect[0] + 435, self.game_rect[1] + 410

    def feixingfu_jianyecheng_bbox(self):
        # 落地坐标:0,0
        logger.info("飞行符-建邺城")
        return self.game_rect[0] + 620, self.game_rect[1] + 470, self.game_rect[0] + 675, self.game_rect[1] + 520

    def feixingfu_changshoucun_bbox(self):
        # 落地坐标:111,61
        logger.info("飞行符-长寿村")
        return self.game_rect[0] + 400, self.game_rect[1] + 300, self.game_rect[0] + 440, self.game_rect[1] + 350

    def feixingfu_zhuziguo_bbox(self):
        # 落地坐标:140,93
        logger.info("飞行符-朱紫国")
        return self.game_rect[0] + 450, self.game_rect[1] + 510, self.game_rect[0] + 485, self.game_rect[1] + 555

    def feixingfu_aolaiguo_bbox(self):
        # 落地坐标:123,94
        logger.info("飞行符-傲来国")
        return self.game_rect[0] + 765, self.game_rect[1] + 525, self.game_rect[0] + 825, self.game_rect[1] + 580

    def fight_action_bbox(self):
        return self.game_rect[0] + 850, self.game_rect[1] + 150, self.game_rect[0] + 1000, self.game_rect[1] + 700

    def four_man_bbox(self):
        return self.game_rect[0] + 350, self.game_rect[1] + 250, self.game_rect[0] + 780, self.game_rect[1] + 500

    def four_man_tips_bbox(self):
        return self.game_rect[0], self.game_rect[1], self.game_rect[2], self.game_rect[1] + 57

    def changan_yizhan_laoban_bbox(self):
        return self.game_rect[0] + 300, self.game_rect[1] + 108, self.game_rect[0] + 880, self.game_rect[1] + 550

    def difu_rukou_bbox(self):
        return self.game_rect[0] + 490, self.game_rect[1] + 57, self.game_rect[0] + 650, self.game_rect[1] + 230

    def fighting_bbox(self):
        return self.game_rect[2] - 30, self.game_rect[1] + 150, self.game_rect[2], self.game_rect[1] + 470

    def hero_icon_bbox(self):
        return self.game_rect[0] + 914, self.game_rect[1] + 60, self.game_rect[0] + 958, self.game_rect[1] + 104

    def hero_health_bbox(self):
        return self.game_rect[0] + 962, self.game_rect[1] + 60, self.game_rect[0] + 1027, self.game_rect[1] + 71

    def hero_mana_bbox(self):
        return self.game_rect[0] + 962, self.game_rect[1] + 72, self.game_rect[0] + 1027, self.game_rect[1] + 83

    def peg_health_bbox(self):
        return self.game_rect[0] + 842, self.game_rect[1] + 60, self.game_rect[0] + 907, self.game_rect[1] + 71

    def map_bbox(self):
        return self.game_rect[0] + 120, self.game_rect[1] + 150, self.game_rect[0] + 940, self.game_rect[1] + 660

    def npc_talk_bbox(self):
        return self.game_rect[0] + 180, self.game_rect[1] + 330, self.game_rect[0] + 870, self.game_rect[1] + 660

    def team_icons_bbox(self):
        return self.game_rect[0] + 545, self.game_rect[1] + 57, self.game_rect[0] + 799, self.game_rect[1] + 107

    def scene_bbox(self):
        return self.game_rect[0] + 23, self.game_rect[1] + 80, self.game_rect[0] + 143, self.game_rect[1] + 97

    def task_bbox(self):
        return self.game_rect[0] + 869, self.game_rect[1] + 195, self.game_rect[0] + 1020, self.game_rect[1] + 366

    def task_title_bbox(self):
        return self.game_rect[0] + 869, self.game_rect[1] + 167, self.game_rect[0] + 1013, self.game_rect[1] + 191

    def task_xiaogui_x_bbox(self, image, bbox, comma_x):
        template = self.task_xiaogui_left_template()
        if template is not None:
            try:
                res = cv2.matchTemplate(image, template, cv2.TM_CCORR_NORMED)
            except cv2.error as e:
                logger.error(e)
                return 0, 0, 10, 10
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
            logger.info(max_val)
            if max_val >= 0.79:
                return bbox[0] + max_loc[0] + template.shape[1], bbox[1] + 1, bbox[0] + comma_x, bbox[3] - 1
        return 0, 0, 10, 10

    def task_xiaogui_y_bbox(self, image, bbox, comma_x, comma_w=7):
        # template = cv_imread(os.getcwd() + img_ghost_task_fujin_gray_107_255_0, 0)
        template = cv_imread(os.getcwd() + img_ghost_task_chu_gray_107_255_0, 0)
        try:
            res = cv2.matchTemplate(image, template, cv2.TM_CCORR_NORMED)
        except cv2.error as e:
            logger.error(e)
            return 0, 0, 10, 10
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        logger.info(max_val)
        # logger.info(max_loc)
        if max_val >= 0.79:
            return bbox[0] + comma_x - 1 + comma_w, bbox[1] + 1, bbox[0] + max_loc[0], bbox[3] - 1
        return bbox[0] + comma_x - 1 + comma_w, bbox[1] + 1, bbox[2], bbox[3] - 1

    def task_xiaogui_location_bbox(self):
        loc_bbox = self.task_bbox()
        image = screen_opencv_grab(loc_bbox)
        x_loc_bbox = (0, 0, 10, 10)
        y_loc_bbox = (0, 0, 10, 10)
        cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)
        thresh = cv2.threshold(cv_img, 107, 255, cv2.THRESH_TOZERO)[1]
        # thresh = cv2.resize(thresh, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        template = cv_imread(os.getcwd() + img_ghost_task_zhuogui_gray_107_255_0, 0)
        # template = cv2.resize(template, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        try:
            res = cv2.matchTemplate(thresh, template, cv2.TM_CCORR_NORMED)
        except cv2.error as e:
            logger.error(e)
            return x_loc_bbox, y_loc_bbox
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        logger.info(max_val)
        # 符合匹配标准才返回结果
        if max_val >= 0.79:
            bbox = loc_bbox[0] + max_loc[0], loc_bbox[1] + max_loc[1] + 29, loc_bbox[0] + max_loc[0] + 148, loc_bbox[
                1] + max_loc[1] + 43
            # logger.info(bbox)
            image = screen_opencv_grab(bbox)
            cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)
            thresh = cv2.threshold(cv_img, 107, 255, cv2.THRESH_TOZERO)[1]
            template = cv_imread(os.getcwd() + img_ghost_task_comma, 0)
            template = cv2.threshold(template, 107, 255, cv2.THRESH_TOZERO)[1]
            try:
                res = cv2.matchTemplate(thresh, template, cv2.TM_CCORR_NORMED)
            except cv2.error as e:
                logger.error(e)
                return x_loc_bbox, y_loc_bbox
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
            logger.info(max_val)
            # logger.info(max_loc)
            if max_val >= 0.87:
                x_loc_bbox = self.task_xiaogui_x_bbox(thresh, bbox, max_loc[0])
                y_loc_bbox = self.task_xiaogui_y_bbox(thresh, bbox, max_loc[0])
                # logger.info(x_loc_bbox)
                # logger.info(y_loc_bbox)
            # return loc_bbox[0] + max_loc[0], loc_bbox[1] + max_loc[1] + 29, loc_bbox[0] + max_loc[0] + 148, loc_bbox[1] + max_loc[1] + 43
        return x_loc_bbox, y_loc_bbox

    def task_xiaogui_name_bbox(self):
        loc_bbox = self.task_bbox()
        image = screen_opencv_grab(loc_bbox)
        name_bbox = [0, 0, 10, 10]
        cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)
        thresh = cv2.threshold(cv_img, 107, 255, cv2.THRESH_TOZERO)[1]
        # thresh = cv2.resize(thresh, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        template = cv_imread(os.getcwd() + img_ghost_task_zhuogui_gray_107_255_0, 0)
        # template = cv2.resize(template, None, fx=factor, fy=factor, interpolation=cv2.INTER_CUBIC)
        try:
            res = cv2.matchTemplate(thresh, template, cv2.TM_CCORR_NORMED)
        except cv2.error as e:
            logger.error(e)
            return name_bbox
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        # logger.info(max_val)
        # 符合匹配标准才返回结果
        if max_val >= 0.79:
            bbox = loc_bbox[0] + max_loc[0], loc_bbox[1] + max_loc[1] + 43, loc_bbox[0] + max_loc[0] + 148, loc_bbox[
                1] + max_loc[1] + 57
            # logger.info(bbox)
            image = screen_opencv_grab(bbox)
            name_bbox[0] = bbox[0]
            name_bbox[1] = bbox[1]
            cv_img = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2GRAY)
            thresh = cv2.threshold(cv_img, 107, 255, cv2.THRESH_TOZERO)[1]
            template = cv_imread(os.getcwd() + img_ghost_task_juhao_gray_107_255_0, 0)
            try:
                res = cv2.matchTemplate(thresh, template, cv2.TM_CCORR_NORMED)
            except cv2.error as e:
                logger.error(e)
                return name_bbox
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
            # logger.info(max_val)
            if max_val >= 0.79:
                name_bbox[2] = bbox[0] + max_loc[0] - 1
            else:
                name_bbox[2] = bbox[2]
            name_bbox[3] = bbox[3]
        return name_bbox

    def zhongkui_task_bbox(self):
        return self.game_rect[0] + 190, self.game_rect[1] + 430, self.game_rect[0] + 730, self.game_rect[1] + 455

    def test(self):
        self.set_win_foreground()
        # my_x, my_y = self.get_my_location()
        hide_player()
        time.sleep(0.1)
        # 朱紫国--大唐国境境外(3,2)
        x, y, n_x, n_y = self.get_relative_position(2, 4, "朱紫国")
        n_x = 35
        n_y = -35
        for _ in range(3):
            if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y):
                if not self.mouse_click_human(x, y, n_x=n_x, n_y=n_y * 2):
                    self.mouse_click_human(x, y, n_x=n_x * 2, n_y=n_y)
            if self.wait_opencv_object(self.scene_bbox(), img_scene_datangjingwai, timeout=4.0):
                break
        time.sleep(0.15)


if __name__ == '__main__':
    for title in cmd_titles:
        try:
            autoit.win_move(title, 1280, 510, 640, 530)
        except Exception as e:
            # logger.error(e)
            pass
        else:
            break
    serial_write(STOP_MP3)
    cg = CatchGhosts()
    # cg.test()
    try:
        cg.do_work()
    except Exception as e:
        for _ in range(5):
            logger.info("***程序出错，请重新运行***")
        logger.error(e)
        serial_write(PLAY_MP3 % b'1')
