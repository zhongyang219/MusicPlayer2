================================================================================
    MICROSOFT 基础类库 : MusicPlayer2 项目概述
===============================================================================

应用程序向导已为您创建了此 MusicPlayer2 应用程序。此应用程序不仅演示 Microsoft 基础类的基本使用方法，还可作为您编写应用程序的起点。

本文件概要介绍组成 MusicPlayer2 应用程序的每个文件的内容。

MusicPlayer2.vcxproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件，其中包含生成该文件的 Visual C++ 的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

MusicPlayer2.vcxproj.filters
    这是使用“应用程序向导”生成的 VC++ 项目筛选器文件。它包含有关项目文件与筛选器之间的关联信息。在 IDE 中，通过这种关联，在特定节点下以分组形式显示具有相似扩展名的文件。例如，“.cpp”文件与“源文件”筛选器关联。

MusicPlayer2.h
    这是应用程序的主头文件。
    其中包括其他项目特定的标头（包括 Resource.h），并声明 CMusicPlayerApp 应用程序类。

MusicPlayer2.cpp
    这是包含应用程序类 CMusicPlayerApp 的主应用程序源文件。

MusicPlayer2.rc
    这是程序使用的所有 Microsoft Windows 资源的列表。它包括 RES 子目录中存储的图标、位图和光标。此文件可以直接在 Microsoft Visual C++ 中进行编辑。项目资源包含在 2052 中。

res\MusicPlayer2.ico
    这是用作应用程序图标的图标文件。此图标包括在主资源文件 MusicPlayer2.rc 中。

res\MusicPlayer2.rc2
    此文件包含不在 Microsoft Visual C++ 中进行编辑的资源。您应该将不可由资源编辑器编辑的所有资源放在此文件中。


/////////////////////////////////////////////////////////////////////////////

应用程序向导创建一个对话框类：

MusicPlayerDlg.h、MusicPlayerDlg.cpp - 对话框
    这些文件包含 CMusicPlayerDlg 类。此类定义应用程序的主对话框的行为。对话框模板包含在 MusicPlayer2.rc 中，该文件可以在 Microsoft Visual C++ 中编辑。

/////////////////////////////////////////////////////////////////////////////

其他功能：

ActiveX 控件
    该应用程序包含对使用 ActiveX 控件的支持。

/////////////////////////////////////////////////////////////////////////////

其他标准文件:

StdAfx.h, StdAfx.cpp
    这些文件用于生成名为 MusicPlayer2.pch 的预编译头 (PCH) 文件和名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是标准头文件，可用于定义新的资源 ID。Microsoft Visual C++ 将读取并更新此文件。

MusicPlayer2.manifest
	Windows XP 使用应用程序清单文件来描述特定版本的并行程序集的应用程序依赖项。加载程序使用这些信息来从程序集缓存中加载相应的程序集，并保护其不被应用程序访问。应用程序清单可能会包含在内，以作为与应用程序可执行文件安装在同一文件夹中的外部 .manifest 文件进行重新分发，它还可能以资源的形式包含在可执行文件中。
/////////////////////////////////////////////////////////////////////////////

其他注释:

应用程序向导使用“TODO:”来指示应添加或自定义的源代码部分。

如果应用程序使用共享 DLL 中的 MFC，您将需要重新分发 MFC DLL。如果应用程序所使用的语言与操作系统的区域设置不同，则还需要重新分发相应的本地化资源 mfc110XXX.DLL。
有关上述话题的更多信息，请参见 MSDN 文档中有关重新分发 Visual C++ 应用程序的部分。

/////////////////////////////////////////////////////////////////////////////

其他文件用途说明：

Player.h, Player.cpp
	包含CPlayer类。封装的播放器类，用于实现各种播放功能。

Lyric.h, Lyric.cpp
	包含CLyric类。封装的歌词类，用于实现解析歌词文件。

bass.h
	BASS音频库的头文件。

Time.h
	包含Time结构的定义，用于表示歌词和歌曲播放等的时间。


用于定义全局函数的类：
由于MFC不允许定义全局函数，所以把一些需要用于全局作用域的函数作为静态函数放到相应的类中。

Common.h, Common.cpp
	包含CCommon类。包含一些全局函数和结构体的定义。此类只包含静态函数。

AudioCommon.h, AudioCommon.cpp
	包含CAudioCommon类。包含一些处理音频相关的全局函数，以及一些相关的结构体的枚举类型的定义。此类只包含静态函数。

ColorConvert.h, ColorConvert.cpp
	包含CColorConvert类。包含一些颜色转换的全局函数，主要用于变换颜色的亮度。此类只包含静态函数。

DrawCommon.h, DrawCommon.cpp
	包含CDrawCommon类。此类用于绘制动态的文本。主要用于绘制滚动的文本和动态歌词。此类不是静态类。

LyricDownloadCommon.h, LyricDownloadCommon.cpp
	包含CLyricDownloadCommon类。包含一些网络相关的全局函数，主要用于在线下载歌词。此类只包含静态函数。


派生的控件类：

EditEx.h, EditEx.cpp
	包含CEditEx类。继承于CEdit类，用于支持文本编辑控件按Ctrl+A全选。

PlayListCtrl.h, PlayListCtrl.cpp
	包含CPlayListCtrl类。继承于CListCtrl类，用于实现列表中项目颜色设置，以及在鼠标提示中显示歌曲信息。

ProgressStatic.h, ProgressStatic.cpp
	包含CProgressStatic类。继承于CStatic类，作为一个进度条控件，用于实现指示歌曲进度，点击定位，以鼠标指向时显示时间。

StaticEx.h, StaticEx.cpp
	包含CStatic类。继承于CStatic类，用于实现在static控件上显示有颜色的文本。


对话框类：
	
OptionsDlg.h, OptionsDlg.cpp
	包含COptionsDlg类，选项设置对话框。

AppearanceSettingDlg.h, AppearanceSettingDlg.cpp
	包含CAppearanceSettingDlg类，选项设置对话框中的“外观设置”选项卡。

DataSettingDlg.h, DataSettingDlg.cpp
	包含CDataSettingDlg类，选项设置对话框中的“数据文件”选项卡。

PlaySettingsDlg.h, PlaySettingsDlg.cpp
	包含CPlaySettingsDlg类，选项设置对话框中的“播放选项”选项卡。

EqualizerDlg.h, EqualizerDlg.cpp
	包含CEqualizerDlg类，音效设定对话框中的“均衡器”选项卡。

FindDlg.h, FindDlg.cpp
	包含CFindDlg类，查找文件对话框。

HelpDlg.h , HelpDlg.cpp
	包含CHelpDlg类，帮助对话框。

LyricBatchDownloadDlg.h, LyricBatchDownloadDlg.cpp
	包含CLyricBatchDownloadDlg类，歌词批量下载对话框。

LyricDownloadDlg.h, LyricDownloadDlg.cpp
	包含CLyricDownloadDlg类，歌词下载对话框。

MiniModeDlg.h, MiniModeDlg.cpp
	包含CMiniModeDlg类，迷你模式对话框。

PropertyDlg.h, PropertyDlg.cpp
	包含CPropertyDlg类，属性对话框。

ReverbDlg.h, ReverbDlg.cpp
	包含CReverbDlg类，音效设定对话框中的“混响”选项卡。

SetPathDlg.h, SetPathDlg,cpp
	包含CSetPathDlg类，选择路径对话框。

SoundEffectDlg.h, SoundEffectDlg.cpp
	包含CSoundEffectDlg类，音效设定对话框。


已编译好的exe程序：https://pan.baidu.com/s/1i5QNwFF


【BASS音频库错误代码检索】
1 BASS_ERROR_MEM
内存不足。
2 BASS_ERROR_FILEOPEN
文件无法被打开。
3 BASS_ERROR_DRIVER
没有可用的设备驱动程序或者设备正在使用中。
4 BASS_ERROR_BUFLOST
缓冲区丢失，可能在初始化时使用了无效的句柄。
5 BASS_ERROR_HANDLE
音频句柄无效。
6 BASS_ERROR_FORMAT
设备不支持指定的格式。
7 BASS_ERROR_POSITION
请求的位置无效，例如试图定位到超出文件末尾的位置。
8 BASS_ERROR_INIT
请求的设备没有被正确初始化。
9 BASS_ERROR_START
输出被暂停或停止，BASS_Start来启动它。
10 BASS_ERROR_SSL
SSL/HTTPS支持不可用。
14 BASS_ERROR_ALREADY
设备已经被初始化。
18 BASS_ERROR_NOCHAN
采样没有可用的通道。已达到同时播放的最大数量，并且没有为该采样指定BASS_SAMPLE_OVER标志或onlynew=TRUE。
19 BASS_ERROR_ILLTYPE
attrib无效。
20 BASS_ERROR_ILLPARAM
指定了非法的参数。
21 BASS_ERROR_NO3D
该通道没有3D功能。
22 BASS_ERROR_NOEAX
该通道没有EAX支持。
23 BASS_ERROR_DEVICE
设备非法。
24 BASS_ERROR_NOPLAY
通道没有在播放。
25 BASS_ERROR_FREQ
无效的采样频率。
27 BASS_ERROR_NOTFILE
流不是一个文件流。
29 BASS_ERROR_NOHW
没有可用的硬件声音。
31 BASS_ERROR_EMPTY
32 BASS_ERROR_NONET
无法打开Internet连接。
33 BASS_ERROR_CREATE
34 BASS_ERROR_NOFX
指定的DX8效果不可用。
37 BASS_ERROR_NOTAVAIL
该属性不可用。
38 BASS_ERROR_DECODE
通道不可播放，这是一个“解码通道”。
39 BASS_ERROR_DX
DirectX未安装或版本过低。
40 BASS_ERROR_TIMEOUT
自上次创建通道以来，没有经过该采样最短的时间间隔（BASS_SAMPLE）。
41 BASS_ERROR_FILEFORM
文件不支持或无法识别。
42 BASS_ERROR_SPEAKER
指定的扬声器标志无效。设备/驱动程序不支持它们，它们试图将立体声流分配给单声道扬声器或启用3D功能。
43 BASS_ERROR_VERSION
插件需要不同的BASS版本。
44 BASS_ERROR_CODEC
该文件使用的编解码器不可用或不受支持。
45 BASS_ERROR_ENDED
通道已经达到了末尾。
46 BASS_ERROR_BUSY
设备正忙。
-1 BASS_ERROR_UNKNOWN
其他未知错误。
