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

店小二坐标aob
0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 00 00 00 00 00 00 00 00 01 F0 AD BA 00 CA 9A 3B FF C9 9A 3B 00 F0 AD BA 00 00 00 00 0D F0 AD BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 AD BA ? ? ? ? ? ? ? ?

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

0X1E6E32
0X3A8C0F
0X3A8D9A
0X39F259
0X3A767A
0X1AD8C4
0X1AD085
0X17E7E8
0X1ABA6F
0X1AAECB
0X1B9E75
0X2896E3
0X1A64AC
0X3608E3
0X2E3A5D
0X2E33A0
0X9CE25A
0X16FC79
0X192AC5
0X192726
0X1925BB
0X157904
0X158D44
0X1D95D8
0X2ECFFB
0X2ECF07
0X2ECDB0
0X2EC6A9
0X2EC544
0X1DB553
0x7FF6022AA621
0x7FF6022ABEE3
0x7FF6022AD846
0X5BC37374
0X5BFCCC91







