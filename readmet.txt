主程序
mhmain.exe ___only__for_xyq__restart__game___ 0 ___tabcontrol__game___0 6 57 13092 -2080577166 00000000
mhmain.dll 反调试
bcdedit /debug off  否则游戏启动不了

xygame是游戏入口函数

TitanHide使用
保证: bcdedit /set testsigning on
1.启动服务
管理员身份运行
Run the command sc start TitanHide to start the TitanHide service.
Run the command sc query TitanHide to check if TitanHide is running.
2.运行TitanHideGUI.exe程序
3.输入pid，进行设置即可


制作shellcode
1.开始菜单管理员身份打开
	x64 Native Tools Command Prompt for VS 2019
2.编译cpp
	"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe" /c /FA /GS- c-shellcode.cpp
3.修改c-shellcode.asm内容
	Remove dependencies from external libraries
	Align stack
	Remove or comment out PDATA and XDATA segments
	Fix a simple syntax issue
4.链接
	"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\ml64.exe" c-shellcode.asm /link /entry:AlignRSP
5.复制shellcode
用的HxD复制

winmmcore.dll
fmodex64.dll
mhmain.dll:$

qword_1001EB0AD0



rcx==7FFC05150CBC

玩家坐标aob
83 3D ? ? ? ? FF 75 DF 0F 57 D2 0F 57 C9 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ?

长安 药店老板(330,390) 00 00 80 3F 00 00 80 3F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A5 43 00 00 C3 43 00 00 80 3F 00 00 80 3F
长安酒店内有自己坐标，酒店老板坐标，店小二坐标
酒店老板坐标(910,570)
店小二坐标aob
静态地址1：48 8D 0D ? ? ? ? 48 89 08 48 8B 53 08 48 89 50 08 48 8B 53 10 48 89 50 10
静态地址2：48 8B 08 48 8D 56 40 48 8D 05 ? ? ? ? 48 89 45 E7
38 E0 D1 30 9D FB 7F 00 00 98 0D 9B 9B FB 7F 00 00 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 00 00 80 3F 00 00 80 3F


根据NPC id得到坐标
4C 8D 05 ? ? ? ? 4C 8B CB 4C 89 01 48 8B D1 44 8B C7
 48 8B 5C 24 30 48 83 C4 20 5F E9 ? ? ? ? 40 53

F0 AF E7 A2 FD 7F 00 00 ? ? ? ? ? ? ? ? 97 01 00 20
坐标地址
[[[[addr+0x28]+0xA0]+0x10]]+0x4C

90 48 8D 05 ? ? ? ? 48 89 03 48 8D 4B 30

NPC ID
店小二id=536871319
酒楼老板
长安驿站老板id=536870914

utf-16 搜索字符串
id:
player id
 name:
 id=
店小二id=1073742231
长安驿站老板id=1073741826
长安药店老板 1073742214
酒楼老板 1073742232
1073741834
1073741917
1073741861
1073741846
1075271418
1073741921

1073741830
1073741842

贸易车队总管1073742799

贼王id
538155549
538333290
夏大叔 536872212
谭村长id=536872206
id=536872000
id=537921460
16710472
12479176
26946341
id=1073742636
536872000
537921460
553089541
553089551
536871741
id=536871741
13742055


名字='
地图层次
外形=2044

一些关键字符串：
on_left_click
process_hotkey
focus
find_path
trace_path
地图动画
结束释放
attack
magic_finished
召唤兽
damage
move_begin_time
move_speed
move_dis
速度
步数
die
speed
Direction
设置方向
walk
on_message
相对父位置
偏移位置X
坐标
position
鼠标
mouse
Cursor
delta
seek
sourcePositionVariancex
Adjust scrolling amount to make given position visible.
Generally GetCursorStartPos() + offset to compute a valid position.
ImVec2

mhmain.dll+2400CF8
fmodex64.dll FMOD::Channel::getPosition(__int64, _DWORD *, unsigned int)
函数堆栈可以看到场景id scene/id
建邺城 scene/1501
东海湾 scene/1506

场景id aob
48 89 00 48 89 40 08 48 89 40 10 66 C7 40 18 01 01 48 89 05 ? ? ? ? 44 89 3D ? ? ? ?

streq(ansi([r9]), "screen_to_logic_pos")
streq(ansi([[rsi+10]]), "screen_to_logic_pos")
stristr(ansi(rdx), "x")

PyEval_CallObjectWithKeywords

call    qword ptr cs:__guard_dispatch_icall_fptr

ReadQword(ReadQword(R15+0x10)+8)==mhmain.dll:$0x24074C
ReadDword(ReadQword(ReadQword(rbp+7+0x38)+0x18)+0x8)==0x03EB
ReadDword(ReadQword(rdi+0x18)+0x8)==0x03EB
ReadDword(ReadQword(ReadQword(rdx+0x70)+0x18)+0x8)==0x03EB
ReadDword(ReadQword(ReadQword(rdi+0x70)+0x18)+0x8)==0x03EB
ReadDword(ReadQword(ReadQword(ReadQword(rbp-0x49)+0x70)+0x18)+0x8)==0x03EB

recv
首字节（1byte）xor后得到内容的长度
内容解密一次，然后每4字节读取，不知道后面用来做什么

角色移动不在recv范围

mapid:
桃源村：03EB
建邺城：05DD
建邺衙门: 601
东海湾：5E2


寻路：
x:4byte
y:4byte(方向倒过来)
在xy目的值打断点，查看调用堆栈，逐个函数查看参数有用的值
kernelbase.GetQueuedCompletionStatus

任务追踪 0x013A
师门任务id=314
巡逻 0x4D
送信 
化生寺 0x3E
慧明
慧静

长安城 0x3F
程夫人
殷丞相
刘副将
毒霸天
张老财


买东西
武器 0x4F
细木棒 
松木锤
摄妖香
青铜斧
腰带

杂货 0x4E
桃花 


送信完成 0x26

在师门附近
帮师父送信给
任务完成，找
买到xx送给

强盗
普陀山 0x62
花果山 0x61
地府 0x82
化生寺 0x79
长寿郊外 0x61 0x62
长寿村
东海湾 0x61
狮驼岭 0x62
西凉女国 0x61
建邺城 0x6B

贼王
普陀山 0x7E 0x75
长寿村 0x77