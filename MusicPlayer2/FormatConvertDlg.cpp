// FormatConvertDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FormatConvertDlg.h"
#include "afxdialogex.h"
#include "BassCore.h"
#include "COSUPlayerHelper.h"
#include "WIC.h"
#include "TagLibHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "SongDataManager.h"
#include "PropertyDlgHelper.h"

#define CONVERTING_TEMP_FILE_NAME L"converting_5k2019u6271iyt8j"

#define MAX_ALBUM_COVER_SIZE (128 * 1024)                           //编码器支持的最大专辑封面大小
#define CONVERT_TEMP_ALBUM_COVER_NAME L"cover_R1hdyFy6CoEK7Gu8"     //临时的专辑封面文件名
#define COMPRESSED_ALBUM_COVER_PIXEL 512                            //要将专辑封面图片压缩的尺寸

CBASSEncodeLibrary CFormatConvertDlg::m_bass_encode_lib;
CBASSWmaLibrary CFormatConvertDlg::m_bass_wma_lib;
CBassMixLibrary CFormatConvertDlg::m_bass_mix_lib;



static wstring GetCueDisplayFileName(const wstring& title, const wstring& artist)
{
    wstring str;
    if (!artist.empty() && !title.empty())
        str = artist + L" - " + title;
    else
        str = artist + title;
    return str;
}

//////////////////////////////////////////////////////////////////////////

// CFormatConvertDlg 对话框

IMPLEMENT_DYNAMIC(CFormatConvertDlg, CBaseDialog)

CFormatConvertDlg::CFormatConvertDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_FORMAT_CONVERT_DIALOG, pParent)
{
}

CFormatConvertDlg::CFormatConvertDlg(const vector<SongInfo>& items, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_FORMAT_CONVERT_DIALOG, pParent)
{
    //获取文件列表
    for (auto item : items)
    {
        CAudioCommon::ClearDefaultTagStr(item);
        if (CAudioCommon::GetAudioTypeByFileName(item.file_path) == AU_MIDI)
        {
            //如果文件是 MIDI 音乐，则把SF2音色库信息添加到注释信息
            CString comment;
            comment.Format(_T("Converted from MIDI by MusicPlayer2. SF2: %s"), CPlayer::GetInstance().GetSoundFontName().c_str());
            item.comment = comment;
        }
        if (!CCommon::IsItemInVector(m_file_list, [&](const SongInfo& song)
            {
                if (!song.is_cue)
                    return item.file_path == song.file_path;
                else
                    return (item.file_path == song.file_path && item.track == song.track);
            }))
            m_file_list.push_back(item);
    }

    m_freq_map[L"8 kHz"] = 8000;
    m_freq_map[L"16 kHz"] = 16000;
    m_freq_map[L"22 kHz"] = 22050;
    m_freq_map[L"24 kHz"] = 24000;
    m_freq_map[L"32 kHz"] = 32000;
    m_freq_map[L"44.1 kHz"] = 44100;
    m_freq_map[L"48 kHz"] = 48000;
}

CFormatConvertDlg::~CFormatConvertDlg()
{
    m_bass_encode_lib.UnInit();
    m_bass_wma_lib.UnInit();
    m_bass_mix_lib.UnInit();
}

CString CFormatConvertDlg::GetDialogName() const
{
    return _T("FormatConvertDlg");

}

void CFormatConvertDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SONG_LIST1, m_file_list_ctrl);
    DDX_Control(pDX, IDC_OUT_FORMAT_COMBO, m_encode_format_combo);
    DDX_Control(pDX, IDC_PROGRESS_BAR, m_progress_bar);
    DDX_Control(pDX, IDC_FREQ_COMBO, m_freq_comb);
    DDX_Control(pDX, IDC_OUT_DIR_EDIT, m_out_dir_edit);
    DDX_Control(pDX, IDC_OUT_NAME_EDIT, m_out_name_edit);
}

void CFormatConvertDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_encode_format = static_cast<EncodeFormat>(ini.GetInt(L"format_convert", L"encode_format", 1));
    m_write_tag = ini.GetBool(L"format_convert", L"write_tag", true);
    m_write_album_cover = ini.GetBool(L"format_convert", L"write_album_cover", true);
    m_file_exist_action = ini.GetInt(L"format_convert", L"file_exist_action", 0);
    m_add_file_serial_num = ini.GetBool(L"format_convert", L"add_file_serial_num", false);
    m_out_dir = ini.GetString(L"format_convert", L"out_dir", L"");
    if (!CCommon::FolderExist(m_out_dir))		//如果读取到的目录不存在，则设置为“我的文档”目录
    {
        m_out_dir = CCommon::GetSpecialDir(CSIDL_MYDOCUMENTS);
    }
    m_out_name = ini.GetString(L"format_convert", L"out_name_formular", FORMULAR_ORIGINAL);
    m_convert_freq = ini.GetBool(L"format_convert", L"convert_freq", false);
    m_freq_sel = ini.GetString(L"format_convert", L"freq_sel", L"");
    m_open_output_dir = ini.GetBool(L"format_convert", L"open_output_dir", false);
}

void CFormatConvertDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteInt(L"format_convert", L"encode_format", static_cast<int>(m_encode_format));
    ini.WriteBool(L"format_convert", L"write_tag", m_write_tag);
    ini.WriteBool(L"format_convert", L"write_album_cover", m_write_album_cover);
    ini.WriteInt(L"format_convert", L"file_exist_action", m_file_exist_action);
    ini.WriteBool(L"format_convert", L"add_file_serial_num", m_add_file_serial_num);
    ini.WriteString(L"format_convert", L"out_dir", m_out_dir);
    ini.WriteString(L"format_convert", L"out_name_formular", m_out_name);
    ini.WriteBool(L"format_convert", L"convert_freq", m_convert_freq);
    ini.WriteString(L"format_convert", L"freq_sel", m_freq_sel);
    ini.WriteBool(L"format_convert", L"open_output_dir", m_open_output_dir);
    ini.Save();
}

void CFormatConvertDlg::LoadEncoderConfig()
{
    CIniHelper ini(theApp.m_local_dir + L"Encoder\\encoder.ini");

    m_mp3_encode_para.encode_type = ini.GetInt(L"mp3_encoder", L"encode_type", 0);
    m_mp3_encode_para.cbr_bitrate = ini.GetString(L"mp3_encoder", L"cbr_bitrate", L"128");
    m_mp3_encode_para.abr_bitrate = ini.GetString(L"mp3_encoder", L"abr_bitrate", L"128");
    m_mp3_encode_para.vbr_quality = ini.GetInt(L"mp3_encoder", L"vbr_quality", 4);
    m_mp3_encode_para.cmd_para = ini.GetString(L"mp3_encoder", L"cmd_para", L"");
    m_mp3_encode_para.joint_stereo = ini.GetBool(L"mp3_encoder", L"joint_stereo", true);
    m_wma_encode_para.cbr = ini.GetBool(L"wma_encoder", L"cbr", true);
    m_wma_encode_para.cbr_bitrate = ini.GetInt(L"wma_encoder", L"cbr_bitrate", 64);
    m_wma_encode_para.vbr_quality = ini.GetInt(L"wma_encoder", L"vbr_quality", 75);
    m_ogg_encode_quality = ini.GetInt(L"ogg_encoder", L"quality", 4);
}

void CFormatConvertDlg::SaveEncoderConfig() const
{
    CIniHelper ini(theApp.m_local_dir + L"Encoder\\encoder.ini");

    ini.WriteInt(L"mp3_encoder", L"encode_type", m_mp3_encode_para.encode_type);
    ini.WriteString(L"mp3_encoder", L"cbr_bitrate", m_mp3_encode_para.cbr_bitrate);
    ini.WriteString(L"mp3_encoder", L"abr_bitrate", m_mp3_encode_para.abr_bitrate);
    ini.WriteInt(L"mp3_encoder", L"vbr_quality", m_mp3_encode_para.vbr_quality);
    ini.WriteString(L"mp3_encoder", L"cmd_para", m_mp3_encode_para.cmd_para);
    ini.WriteBool(L"mp3_encoder", L"joint_stereo", m_mp3_encode_para.joint_stereo);
    ini.WriteBool(L"wma_encoder", L"cbr", m_wma_encode_para.cbr);
    ini.WriteInt(L"wma_encoder", L"cbr_bitrate", m_wma_encode_para.cbr_bitrate);
    ini.WriteInt(L"wma_encoder", L"vbr_quality", m_wma_encode_para.vbr_quality);
    ini.WriteInt(L"ogg_encoder", L"quality", m_ogg_encode_quality);

    ini.Save();
}


BEGIN_MESSAGE_MAP(CFormatConvertDlg, CBaseDialog)
    ON_CBN_SELCHANGE(IDC_OUT_FORMAT_COMBO, &CFormatConvertDlg::OnCbnSelchangeOutFormatCombo)
    ON_BN_CLICKED(IDC_START_CONVERT_BUTTON, &CFormatConvertDlg::OnBnClickedStartConvertButton)
    //ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CFormatConvertDlg::OnBnClickedBrowseButton)
    ON_MESSAGE(WM_CONVERT_PROGRESS, &CFormatConvertDlg::OnConvertProgress)
    ON_MESSAGE(WM_CONVERT_COMPLETE, &CFormatConvertDlg::OnConvertComplete)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_ENCODER_CONFIG_BUTTON, &CFormatConvertDlg::OnBnClickedEncoderConfigButton)
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_COPY_TAG_CHECK, &CFormatConvertDlg::OnBnClickedCopyTagCheck)
    ON_BN_CLICKED(IDC_COPY_ALBUM_COVER_CHECK, &CFormatConvertDlg::OnBnClickedCopyAlbumCoverCheck)
    ON_CBN_SELCHANGE(IDC_TARGET_FILE_EXIST_COMBO, &CFormatConvertDlg::OnCbnSelchangeTargetFileExistCombo)
    ON_BN_CLICKED(IDC_ADD_NUMBER_CHECK, &CFormatConvertDlg::OnBnClickedAddNumberCheck)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST1, &CFormatConvertDlg::OnNMRClickSongList1)
    ON_COMMAND(ID_ADD_FILE, &CFormatConvertDlg::OnAddFile)
    ON_COMMAND(ID_DELETE_SELECT, &CFormatConvertDlg::OnDeleteSelect)
    ON_COMMAND(ID_CLEAR_LIST, &CFormatConvertDlg::OnClearList)
    ON_COMMAND(ID_MOVE_UP, &CFormatConvertDlg::OnMoveUp)
    ON_COMMAND(ID_MOVE_DOWN, &CFormatConvertDlg::OnMoveDown)
    ON_COMMAND(ID_EDIT_TAG_INFO, &CFormatConvertDlg::OnEditTagInfo)
    ON_WM_INITMENU()
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST1, &CFormatConvertDlg::OnNMDblclkSongList1)
    ON_BN_CLICKED(IDC_CHANGE_FREQ_CHECK, &CFormatConvertDlg::OnBnClickedChangeFreqCheck)
    ON_CBN_SELCHANGE(IDC_FREQ_COMBO, &CFormatConvertDlg::OnCbnSelchangeFreqCombo)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CFormatConvertDlg::OnEditBrowseChanged)
    ON_BN_CLICKED(IDC_OPEN_TARGET_DIR_CHECK, &CFormatConvertDlg::OnBnClickedOpenTargetDirCheck)
END_MESSAGE_MAP()


// CFormatConvertDlg 消息处理程序


BOOL CFormatConvertDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CenterWindow();

    SetIcon(theApp.m_icon_set.convert, FALSE);
    SetIcon(AfxGetApp()->LoadIcon(IDI_CONVERT_D), true);
    SetButtonIcon(IDC_ENCODER_CONFIG_BUTTON, theApp.m_icon_set.setting.GetIcon(true));
    SetButtonIcon(IDC_START_CONVERT_BUTTON, theApp.m_icon_set.convert);

    LoadConfig();
    LoadEncoderConfig();

    m_encoder_succeed = InitEncoder();
    if (!m_bass_mix_lib.IsSucceed())
        m_convert_freq = false;

    //初始化菜单
    m_list_popup_menu.LoadMenu(IDR_FORMAT_CONVERT_POPUP_MENU);
    CMenuIcon::AddIconToMenuItem(m_list_popup_menu.GetSafeHmenu(), ID_ADD_FILE, FALSE, theApp.m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_list_popup_menu.GetSafeHmenu(), ID_EDIT_TAG_INFO, FALSE, theApp.m_icon_set.edit.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_list_popup_menu.GetSafeHmenu(), ID_DELETE_SELECT, FALSE, theApp.m_icon_set.close.GetIcon(true));


    //初始化文件列表
    CRect rect;
    m_file_list_ctrl.GetWindowRect(rect);
    int width0{ theApp.DPI(40) }, width1, width2{ theApp.DPI(65) };
    width1 = rect.Width() - width0 - width2 - theApp.DPI(20) - 1;
    //插入列
    m_file_list_ctrl.SetExtendedStyle(m_file_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_file_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width0);		//插入第1列
    m_file_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width1);		//插入第2列
    m_file_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_CONVERTING_STATE), LVCFMT_LEFT, width2);		//插入第3列
    //插入项目
    ShowFileList();
    //设置主题颜色
    //m_file_list_ctrl.SetColor(theApp.m_app_setting_data.theme_color);

    //初始化转换格式的下拉列表
    m_encode_format_combo.AddString(_T("WAV"));
    m_encode_format_combo.AddString(CCommon::LoadText(IDS_MP3_LAME_ENCODER));
    m_encode_format_combo.AddString(_T("WMA"));
    m_encode_format_combo.AddString(_T("OGG"));
    m_encode_format_combo.SetCurSel(static_cast<int>(m_encode_format));

    //初始化选项控件的状态
    ((CButton*)GetDlgItem(IDC_COPY_TAG_CHECK))->SetCheck(m_write_tag);
    ((CButton*)GetDlgItem(IDC_COPY_ALBUM_COVER_CHECK))->SetCheck(m_write_album_cover);
    ((CButton*)GetDlgItem(IDC_ADD_NUMBER_CHECK))->SetCheck(m_add_file_serial_num);
    CComboBox* file_exist_combo = (CComboBox*)GetDlgItem(IDC_TARGET_FILE_EXIST_COMBO);
    file_exist_combo->AddString(CCommon::LoadText(IDS_AUTO_RENAME));
    file_exist_combo->AddString(CCommon::LoadText(IDS_IGNORE));
    file_exist_combo->AddString(CCommon::LoadText(IDS_OVERWRITE));
    file_exist_combo->SetCurSel(m_file_exist_action);
    ((CButton*)GetDlgItem(IDC_OPEN_TARGET_DIR_CHECK))->SetCheck(m_open_output_dir);


    for (const auto& item : m_freq_map)
        m_freq_comb.AddString(item.first.c_str());
    if (m_freq_sel.empty())
        m_freq_comb.SelectString(0, _T("44.1 kHz"));
    else
        m_freq_comb.SelectString(0, m_freq_sel.c_str());
    m_freq_comb.EnableWindow(m_convert_freq);
    ((CButton*)GetDlgItem(IDC_CHANGE_FREQ_CHECK))->SetCheck(m_convert_freq);

    m_out_name_edit.SetWindowText(m_out_name.c_str());
    m_out_name_edit.SetEditBrowseMode(CBrowseEdit::EditBrowseMode::RENAME);

    if (!m_out_dir.empty() && m_out_dir.back() != L'\\')
        m_out_dir.push_back(L'\\');
    m_out_dir_edit.SetWindowText(m_out_dir.c_str());
    m_out_dir_edit.EnableFolderBrowseButton(CCommon::LoadText(IDS_SELECT_OUTPUT_FOLDER));
    SetDlgItemText(IDC_PROGRESS_TEXT, _T(""));
    m_progress_bar.SetBackgroundColor(GetSysColor(COLOR_BTNFACE));
    m_progress_bar.ShowWindow(SW_HIDE);

    SetEncodeConfigBtnState();

    //#ifndef COMPILE_IN_WIN_XP
    //	if (CWinVersionHelper::IsWindows7OrLater())
    //		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbar));	//创建ITaskbarList3的实例
    //#endif

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFormatConvertDlg::EnableControls(bool enable)
{
    GetDlgItem(IDC_OUT_FORMAT_COMBO)->EnableWindow(enable);
    //GetDlgItem(IDC_BROWSE_BUTTON)->EnableWindow(enable);
    GetDlgItem(IDC_START_CONVERT_BUTTON)->EnableWindow(enable);
    GetDlgItem(IDC_ENCODER_CONFIG_BUTTON)->EnableWindow(enable);
    //GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(enable);
    //GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(enable);
    //GetDlgItem(IDC_CLEAR_BUTTON)->EnableWindow(enable);
    //GetDlgItem(IDC_MOVE_UP_BUTTON)->EnableWindow(enable);
    //GetDlgItem(IDC_MOVE_DOWN_BUTTON)->EnableWindow(enable);
    GetDlgItem(IDC_COPY_TAG_CHECK)->EnableWindow(enable);
    GetDlgItem(IDC_TARGET_FILE_EXIST_COMBO)->EnableWindow(enable);
    GetDlgItem(IDC_COPY_ALBUM_COVER_CHECK)->EnableWindow(enable);
    GetDlgItem(IDC_ADD_NUMBER_CHECK)->EnableWindow(enable);
    GetDlgItem(IDC_CHANGE_FREQ_CHECK)->EnableWindow(enable);
    m_freq_comb.EnableWindow(enable && m_convert_freq);
    GetDlgItem(IDC_OUT_DIR_EDIT)->EnableWindow(enable);
    GetDlgItem(IDC_OUT_NAME_EDIT)->EnableWindow(enable);
}

void CFormatConvertDlg::SetEncodeConfigBtnState()
{
    GetDlgItem(IDC_ENCODER_CONFIG_BUTTON)->EnableWindow(m_encode_format != EncodeFormat::WAV);
}

void CFormatConvertDlg::ShowFileList()
{
    m_file_list_ctrl.DeleteAllItems();
    for (size_t i{}; i < m_file_list.size(); i++)
    {
        CString tmp;
        tmp.Format(_T("%d"), i + 1);
        m_file_list_ctrl.InsertItem(i, tmp);
        if (!m_file_list[i].is_cue)
        {
            CFilePathHelper file_path(m_file_list[i].file_path);
            m_file_list_ctrl.SetItemText(i, 1, file_path.GetFileName().c_str());
        }
        else
        {
            m_file_list_ctrl.SetItemText(i, 1, GetCueDisplayFileName(m_file_list[i].title, m_file_list[i].artist).c_str());
        }
    }
}

bool CFormatConvertDlg::InitEncoder()
{
    //初始化bass encode库
    wstring encode_dll_path;
    wstring wma_dll_path;
    wstring plugin_dir;
    m_encode_dir = theApp.m_local_dir + L"Encoder\\";
    plugin_dir = theApp.m_local_dir + L"Plugins\\";
    encode_dll_path = m_encode_dir + L"bassenc.dll";
    m_bass_encode_lib.Init(encode_dll_path);

    wma_dll_path = plugin_dir + L"basswma.dll";
    m_bass_wma_lib.Init(wma_dll_path);

    m_bass_mix_lib.Init(m_encode_dir + L"bassmix.dll");

    return m_bass_encode_lib.IsSucceed();
}

bool CFormatConvertDlg::EncodeSingleFile(CFormatConvertDlg* pthis, int file_index)
{
    //设置输出文件路径
    SongInfo& song_info{ pthis->m_file_list[file_index] };
    const wstring& file_path{ pthis->m_file_list[file_index].file_path };

    // 输出文件路径
    wstring out_file_path{ pthis->m_out_dir };

    // 为目标文件添加序号
    if (pthis->m_add_file_serial_num)
    {
        CString index_str;
        if (pthis->m_file_list.size() < 10)
            index_str.Format(_T("%d."), file_index + 1);
        else if (pthis->m_file_list.size() < 10)
            index_str.Format(_T("%2d."), file_index + 1);
        else
            index_str.Format(_T("%3d."), file_index + 1);
        out_file_path += index_str;
    }

    // 按照格式字符串生成输出文件名
    out_file_path += CPropertyDlgHelper::FileNameFromTag(pthis->m_out_name, song_info);

    // 按照输出格式添加后缀
    switch (pthis->m_encode_format)
    {
    case EncodeFormat::WAV:
        out_file_path += L".wav";
        break;
    case EncodeFormat::MP3:
        out_file_path += L".mp3";
        break;
    case EncodeFormat::WMA:
        out_file_path += L".wma";
        break;
    case EncodeFormat::OGG:
        out_file_path += L".ogg";
        break;
    }

    //判断目标文件是否存在
    if (pthis->m_file_exist_action == 0)		//如果设置了“目标文件存在时自动重命名”，自动在文件名后面添加形如“ (数字)”的编号
    {
        CCommon::FileAutoRename(out_file_path);
    }
    else if (pthis->m_file_exist_action == 1)		//如果设置了“目标文件存在时忽略”
    {
        if (CCommon::FileExist(out_file_path))
        {
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, 102);
            return false;
        }
    }

    wstring out_file_path_temp = CCommon::GetTemplatePath() + CONVERTING_TEMP_FILE_NAME;     //转换时的临时文件名

    //创建解码通道
    const int BUFF_SIZE{ 20000 };
    // char buff[BUFF_SIZE];
    std::unique_ptr<char[]> buff(new char[BUFF_SIZE]);
    HSTREAM hStream = BASS_StreamCreateFile(FALSE, file_path.c_str(), 0, 0, BASS_STREAM_DECODE/* | BASS_SAMPLE_FLOAT*/);
    if (hStream == 0)
    {
        ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, CONVERT_ERROR_FILE_CONNOT_OPEN);
        return false;
    }

    //获取通道信息
    BASS_CHANNELINFO channel_info;
    BASS_ChannelGetInfo(hStream, &channel_info);
    bool is_midi = (CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype) == AU_MIDI);
    if (is_midi)
    {
        bool sf2_loaded = (CBassCore::m_bass_midi_lib.IsSucceed() && CBassCore::m_sfont.font != 0);
        if (sf2_loaded)
        {
            CBassCore::m_bass_midi_lib.BASS_MIDI_StreamSetFonts(hStream, &CBassCore::m_sfont, 1);
        }
        else
        {
            BASS_StreamFree(hStream);
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, CONVERT_ERROR_MIDI_NO_SF2);
            return false;
        }
    }

    //转换采样频率
    int freq = pthis->GetFreq();
    HSTREAM hStreamOld = 0;
    if (m_bass_mix_lib.IsSucceed() && pthis->m_convert_freq && freq > 0 && freq != channel_info.freq)
    {
        hStreamOld = hStream;
        hStream = m_bass_mix_lib.BASS_Mixer_StreamCreate(freq, channel_info.chans, BASS_MIXER_END | BASS_STREAM_DECODE);
        if (hStream != 0)
        {
            m_bass_mix_lib.BASS_Mixer_StreamAddChannel(hStream, hStreamOld, 0);
        }
        else
        {
            hStream = hStreamOld;
            hStreamOld = 0;
        }
    }

    //获取流的长度
    QWORD length = BASS_ChannelGetLength(hStreamOld != 0 ? hStreamOld : hStream, 0);
    if (hStreamOld != 0 && channel_info.freq != 0)     //如果开启了转换采样频率
        length = length * freq / channel_info.freq;
    if (length == 0) length = 1;	//防止length作为除数时为0

    //如果转换的音频是cue音轨，则先定位到曲目开始处
    if (song_info.is_cue)
    {
        CBassCore::SetCurrentPosition(hStreamOld != 0 ? hStreamOld : hStream, song_info.start_pos.toInt());
    }

    HENCODE hEncode;
    if (pthis->m_encode_format != EncodeFormat::WMA)
    {
        //设置解码器命令行参数
        wstring cmdline;
        switch (pthis->m_encode_format)
        {
        case EncodeFormat::MP3:
        {
            //设置lame命令行参数
            cmdline = pthis->m_encode_dir;
            cmdline += L"lame.exe ";
            cmdline += pthis->m_mp3_encode_para.cmd_para;
            CString str;
            //if (pthis->m_write_tag)
            //{
            //	str.Format(_T(" --tt \"%s\" --ta \"%s\" --tl \"%s\" --ty \"%s\" --tc \"%s\" --tn \"%s\" --tg \"%s\""),
            //		song_info.title.c_str(), song_info.artist.c_str(), song_info.album.c_str(), song_info.get_year().c_str(), song_info.comment.c_str(),
            //		CAudioCommon::TrackToString(song_info.track).GetString(), song_info.genre.c_str());
            //	cmdline += str;
            //}
            //if(pthis->m_write_tag)
            //	cmdline += L" --add-id3v2";
            cmdline += L" - \"";
            cmdline += out_file_path_temp;
            cmdline.push_back(L'\"');
        }
        break;
        case EncodeFormat::OGG:
        {
            cmdline = pthis->m_encode_dir;
            CString str;
            str.Format(_T("oggenc -q %d"), pthis->m_ogg_encode_quality);
            cmdline += str;
            if (pthis->m_write_tag)
            {
                str.Format(_T(" -t \"%s\" -a \"%s\" -l \"%s\" -N \"%s\""),
                    song_info.title.c_str(), song_info.artist.c_str(), song_info.album.c_str(), CAudioCommon::TrackToString(song_info.track).GetString());
                cmdline += str;
            }
            cmdline += L" -o \"";
            cmdline += out_file_path;
            cmdline += L"\" -";
        }
        break;
        default:
            cmdline = out_file_path;
        }
        //开始解码
        hEncode = m_bass_encode_lib.BASS_Encode_StartW(hStream, cmdline.c_str(), BASS_ENCODE_AUTOFREE | (pthis->m_encode_format == EncodeFormat::WAV ? BASS_ENCODE_PCM : 0), NULL, 0);
        if (hEncode == 0)
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, CONVERT_ERROR_ENCODE_CHANNEL_FAILED);
            return false;
        }
    }
    else		//输出为WMA时使用专用的编码器
    {
        if (!m_bass_wma_lib.IsSucceed())
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        //开始解码
        int wma_bitrate;
        if (pthis->m_wma_encode_para.cbr)
            wma_bitrate = pthis->m_wma_encode_para.cbr_bitrate * 1000;
        else
            wma_bitrate = pthis->m_wma_encode_para.vbr_quality;
        hEncode = m_bass_wma_lib.BASS_WMA_EncodeOpenFileW(hStream, NULL, BASS_WMA_ENCODE_STANDARD | BASS_WMA_ENCODE_SOURCE, wma_bitrate, out_file_path.c_str());
        int error = BASS_ErrorGetCode();
        int error_code{ CONVERT_ERROR_ENCODE_CHANNEL_FAILED };
        //如果出现了错误，则写入错误日志
        if (error != 0)
        {
            CString log_str;
            log_str = CCommon::LoadTextFormat(IDS_CONVERT_WMA_ERROR, { file_path });
            switch (error)
            {
            case BASS_ERROR_WMA:
                log_str += CCommon::LoadText(IDS_NO_WMP9_OR_LATER);
                error_code = CONVERT_ERROR_WMA_NO_WMP9_OR_LATER;
                break;
            case BASS_ERROR_NOTAVAIL:
                log_str += CCommon::LoadText(IDS_NO_SUPPORTED_ENCODER_WARNING);
                error_code = CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER;
                break;
            default:
                log_str += CCommon::LoadText(IDS_UNKNOW_ERROR);
                break;
            }
            theApp.WriteLog(wstring(log_str));
        }
        if (hEncode == 0)
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, error_code);
            return false;
        }
        //写入WMA标签
        if (pthis->m_write_tag)
        {
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"Title", song_info.title.c_str());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"Author", song_info.artist.c_str());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"WM/AlbumTitle", song_info.album.c_str());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"WM/Year", song_info.get_year().c_str());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"WM/TrackNumber", CAudioCommon::TrackToString(song_info.track).GetString());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"WM/Genre", song_info.genre.c_str());
            m_bass_wma_lib.BASS_WMA_EncodeSetTagW(hEncode, L"Description", song_info.comment.c_str());
        }
    }

    while (BASS_ChannelIsActive(hStream) != BASS_ACTIVE_STOPPED)
    {
        if (theApp.m_format_convert_dialog_exit)
        {
            m_bass_encode_lib.BASS_Encode_Stop(hEncode);
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        // BASS_ChannelGetData(hStream, buff, BUFF_SIZE);
        BASS_ChannelGetData(hStream, buff.get(), BUFF_SIZE);
        //if (m_bass_encode_lib.BASS_Encode_IsActive(hStream) == 0)
        //{
        //	m_bass_encode_lib.BASS_Encode_Stop(hEncode);
        //	BASS_StreamFree(hStream);
        //}

        //获取转换百分比
        int percent;
        static int last_percent{ -1 };
        if (!song_info.is_cue)
        {
            QWORD position = BASS_ChannelGetPosition(hStream, 0);
            percent = static_cast<int>(position * 100 / length);
        }
        else
        {
            int cue_position = CBassCore::GetBASSCurrentPosition(hStreamOld != 0 ? hStreamOld : hStream) - song_info.start_pos.toInt();
            int cue_length = song_info.lengh.toInt();
            percent = static_cast<int>(cue_position * 100 / cue_length);
            if (percent == 100)
                break;
        }
        if (percent < 0 || percent>100)
        {
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, CONVERT_ERROR_ENCODE_PARA_ERROR);
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        if (last_percent != percent)
            ::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, percent);
        last_percent = percent;
    }

    BASS_StreamFree(hStream);
    if (hStreamOld != 0)
        BASS_StreamFree(hStreamOld);

    //转换完成后
    if (pthis->m_encode_format == EncodeFormat::MP3)
    {
        CFilePathHelper out_file_path_helper{ out_file_path };
        CCommon::MoveAFile(AfxGetMainWnd()->GetSafeHwnd(), out_file_path_temp, out_file_path_helper.GetDir());
        if (CCommon::FileExist(out_file_path_helper.GetFilePath()))
            CCommon::DeleteAFile(AfxGetMainWnd()->GetSafeHwnd(), out_file_path_helper.GetFilePath());
        CCommon::FileRename(out_file_path_helper.GetDir() + CONVERTING_TEMP_FILE_NAME, out_file_path_helper.GetFileName());

        //向mp3文件添加标签信息
        if (pthis->m_write_tag)
        {
            SongInfo song_info_tmp{ song_info };
            song_info_tmp.file_path = out_file_path;
            CTagLibHelper::WriteMpegTag(song_info_tmp);
        }

        //向转换的mp3文件添加专辑封面
        if (pthis->m_write_album_cover)
        {
            SongInfo song_info_tmp;
            song_info_tmp.file_path = file_path;
            CAudioTag audio_tag(song_info_tmp);
            int cover_type;
            wstring album_cover_path = audio_tag.GetAlbumCover(cover_type, ALBUM_COVER_NAME_ENCODE);
            CImage image;
            image.Load(album_cover_path.c_str());
            if (image.IsNull())		//如果没有内嵌的专辑封面，则获取外部封面
            {
                CMusicPlayerCmdHelper helper;
                album_cover_path = helper.SearchAlbumCover(song_info);
            }
            else
                image.Destroy();
            if (!album_cover_path.empty())
            {
                CTagLibHelper::WriteMp3AlbumCover(out_file_path, album_cover_path);
            }
        }
    }

    else if (pthis->m_encode_format == EncodeFormat::WAV)
    {
        SongInfo song_info_tmp{ song_info };
        song_info_tmp.file_path = out_file_path;
        if (pthis->m_write_tag)
        {
            CTagLibHelper::WriteWavTag(song_info_tmp);
        }
        if (pthis->m_write_album_cover)
        {
            SongInfo song_info_tmp;
            song_info_tmp.file_path = file_path;
            CAudioTag audio_tag(song_info_tmp);
            int cover_type;
            wstring album_cover_path = audio_tag.GetAlbumCover(cover_type, ALBUM_COVER_NAME_ENCODE);
            CImage image;
            image.Load(album_cover_path.c_str());
            if (image.IsNull())		//如果没有内嵌的专辑封面，则获取外部封面
            {
                CMusicPlayerCmdHelper helper;
                album_cover_path = helper.SearchAlbumCover(song_info);
            }
            else
                image.Destroy();
            if (!album_cover_path.empty())
            {
                CTagLibHelper::WriteWavAlbumCover(out_file_path, album_cover_path);
            }
        }
    }

    return true;
}


void CFormatConvertDlg::SetProgressInfo(int progress)
{
    CString info;
    if (progress >= 100)
        info = CCommon::LoadText(IDS_COMPLEATE);
    else
        info = CCommon::LoadTextFormat(IDS_PROGRESS_INFO, { progress });
    SetDlgItemText(IDC_PROGRESS_TEXT, info);
}

int CFormatConvertDlg::GetFreq()
{
    CString str;
    m_freq_comb.GetWindowText(str);

    return m_freq_map[str.GetString()];
}

UINT CFormatConvertDlg::ThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    CFormatConvertDlg* pThis{ (CFormatConvertDlg*)lpParam };
    for (size_t i{}; i < pThis->m_file_list.size(); i++)
    {
        if (theApp.m_format_convert_dialog_exit)
            return 0;
        //编码文件
        if (EncodeSingleFile(pThis, i))
            ::PostMessage(pThis->GetSafeHwnd(), WM_CONVERT_PROGRESS, i, 101);
    }
    ::PostMessage(pThis->GetSafeHwnd(), WM_CONVERT_COMPLETE, 0, 0);
    return 0;
}

bool CFormatConvertDlg::IsTaskbarListEnable() const
{
    return CWinVersionHelper::IsWindows7OrLater() && m_pTaskbar != nullptr;
}


void CFormatConvertDlg::OnCbnSelchangeOutFormatCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_format = static_cast<EncodeFormat>(m_encode_format_combo.GetCurSel());
    SetEncodeConfigBtnState();
}


void CFormatConvertDlg::OnBnClickedStartConvertButton()
{
    // TODO: 在此添加控件通知处理程序代码
    m_progress_bar.ShowWindow(SW_SHOW);
    m_progress_bar.SetProgress(0);
    SetProgressInfo(0);
    if (!m_encoder_succeed)
    {
        CString info;
        info = CCommon::LoadTextFormat(IDS_BASS_ENCODER_LOAD_ERROR, { m_encode_dir + L"bassenc.dll" });
        MessageBox(info, NULL, MB_ICONERROR | MB_OK);
        return;
    }

    if (m_out_dir.empty())
    {
        MessageBox(CCommon::LoadText(IDS_SET_OUTPUT_DIR_INFO), NULL, MB_ICONWARNING | MB_OK);
        return;
    }
    else if (!CCommon::FolderExist(m_out_dir))
    {
        CString info;
        info = CCommon::LoadTextFormat(IDS_OUTPUT_DIR_NOT_EXIST, { m_out_dir });
        MessageBox(info, NULL, MB_ICONWARNING | MB_OK);
    }

    //先清除“状态”一列的内容
    for (size_t i{}; i < m_file_list.size(); i++)
    {
        m_file_list_ctrl.SetItemText(i, 2, _T(""));
    }

    EnableControls(false);
    theApp.m_format_convert_dialog_exit = false;
    //创建格式转换的工作线程
    m_pThread = AfxBeginThread(ThreadFunc, this);
    m_thread_runing = true;
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
        m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_INDETERMINATE);
#endif
}


//void CFormatConvertDlg::OnBnClickedBrowseButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//#ifdef COMPILE_IN_WIN_XP
//	CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
//	folderPickerDlg.SetInfo(CCommon::LoadText(IDS_SELECT_OUTPUT_FOLDER));
//#else
//	CFolderPickerDialog folderPickerDlg(m_out_dir.c_str());
//	folderPickerDlg.m_ofn.lpstrTitle = CCommon::LoadText(IDS_SELECT_OUTPUT_FOLDER);		//设置对话框标题
//#endif // COMPILE_IN_WIN_XP
//	if (folderPickerDlg.DoModal() == IDOK)
//	{
//		m_out_dir = folderPickerDlg.GetPathName();
//		if (m_out_dir.back() != L'\\') m_out_dir.push_back(L'\\');	//确保路径末尾有反斜杠
//		SetDlgItemText(IDC_OUT_DIR_EDIT, m_out_dir.c_str());
//	}
//
//}


afx_msg LRESULT CFormatConvertDlg::OnConvertProgress(WPARAM wParam, LPARAM lParam)
{
    CString percent_str;
    int percent = (int)lParam;
    if (percent == 0)
        m_file_list_ctrl.EnsureVisible(wParam, FALSE);		//转换开始时，确保当前列表项可见
    if (percent < 0)
    {
        //显示错误信息
        percent_str += CCommon::LoadText(IDS_ERROR);
        percent_str += L": ";
        if (percent == CONVERT_ERROR_FILE_CONNOT_OPEN)
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_FILE_CONNOT_OPEN);
        else if (percent == CONVERT_ERROR_ENCODE_CHANNEL_FAILED)
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_ENCODE_CHANNEL_FAILED);
        else if (percent == CONVERT_ERROR_ENCODE_PARA_ERROR)
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_ENCODE_PARA_ERROR);
        else if (percent == CONVERT_ERROR_MIDI_NO_SF2)
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_MIDI_NO_SF2);
        else if (percent == CONVERT_ERROR_WMA_NO_WMP9_OR_LATER)
        {
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_ENCODE_CHANNEL_FAILED);
            percent_str += _T(": ");
            percent_str += CCommon::LoadText(IDS_NO_WMP9_OR_LATER);
        }
        else if (percent == CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER)
        {
            percent_str += CCommon::LoadText(IDS_CONVERT_ERROR_ENCODE_CHANNEL_FAILED);
            percent_str += _T(": ");
            percent_str += CCommon::LoadText(IDS_NO_SUPPORTED_ENCODER_WARNING);
        }
        else
            percent_str += std::to_wstring(percent).c_str();
    }
    else if (percent == 101)
    {
        percent_str = CCommon::LoadText(IDS_COMPLEATE);
    }
    else if (percent == 102)
    {
        percent_str = CCommon::LoadText(IDS_SKIPED1);
    }
    else
    {
        percent_str.Format(_T("%d%%"), (int)lParam);
    }
    m_file_list_ctrl.SetItemText(wParam, 2, percent_str);

    //总体的进度
    int position, length;
    length = m_file_list.size() * 100;
    position = wParam * 100 + percent;
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
    {
        m_pTaskbar->SetProgressValue(this->GetSafeHwnd(), position, length);
    }
#endif
    int total_percent = position * 100 / length;
    static int last_percent = -1;
    if (last_percent != total_percent)
    {
        m_progress_bar.SetProgress(total_percent);
        SetProgressInfo(total_percent);
        last_percent = total_percent;
    }

    return 0;
}


afx_msg LRESULT CFormatConvertDlg::OnConvertComplete(WPARAM wParam, LPARAM lParam)
{
    EnableControls(true);
    m_thread_runing = false;
    m_progress_bar.SetProgress(100);
    SetProgressInfo(100);

    if (m_open_output_dir)
    {
        ShellExecute(NULL, _T("open"), _T("explorer"), m_out_dir.c_str(), NULL, SW_SHOWNORMAL);
    }

    return 0;
}


void CFormatConvertDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    theApp.m_format_convert_dialog_exit = true;
    if (m_pThread != nullptr)
        WaitForSingleObject(m_pThread->m_hThread, 2000);	//等待线程退出

    SaveConfig();
    SaveEncoderConfig();

    DestroyWindow();

    //CBaseDialog::OnCancel();
}


void CFormatConvertDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    //theApp.m_format_convert_dialog_exit = true;
    //if (m_pThread != nullptr)
    //	WaitForSingleObject(m_pThread->m_hThread, 2000);	//等待线程退出

    CBaseDialog::OnOK();
}


void CFormatConvertDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CBaseDialog::OnClose();
}


void CFormatConvertDlg::OnBnClickedEncoderConfigButton()
{
    // TODO: 在此添加控件通知处理程序代码
    switch (m_encode_format)
    {
    case EncodeFormat::WAV:
        break;
    case EncodeFormat::MP3:
    {
        CMP3EncodeCfgDlg dlg;
        dlg.m_encode_para = m_mp3_encode_para;
        if (dlg.DoModal() == IDOK)
            m_mp3_encode_para = dlg.m_encode_para;
    }
    break;
    case EncodeFormat::WMA:
    {
        CWmaEncodeCfgDlg dlg;
        dlg.m_encode_para = m_wma_encode_para;
        if (dlg.DoModal() == IDOK)
            m_wma_encode_para = dlg.m_encode_para;
    }
    break;
    case EncodeFormat::OGG:
    {
        COggEncodeCfgDlg dlg;
        dlg.m_encode_quality = m_ogg_encode_quality;
        if (dlg.DoModal() == IDOK)
            m_ogg_encode_quality = dlg.m_encode_quality;
    }
    break;
    default:
        break;
    }
}


void CFormatConvertDlg::OnBnClickedCopyTagCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_write_tag = (((CButton*)GetDlgItem(IDC_COPY_TAG_CHECK))->GetCheck() != 0);
}


void CFormatConvertDlg::OnBnClickedCopyAlbumCoverCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_write_album_cover = (((CButton*)GetDlgItem(IDC_COPY_ALBUM_COVER_CHECK))->GetCheck() != 0);
}


void CFormatConvertDlg::OnCbnSelchangeTargetFileExistCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_file_exist_action = ((CComboBox*)GetDlgItem(IDC_TARGET_FILE_EXIST_COMBO))->GetCurSel();
}


void CFormatConvertDlg::OnBnClickedAddNumberCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_add_file_serial_num = (((CButton*)GetDlgItem(IDC_ADD_NUMBER_CHECK))->GetCheck() != 0);
}


void CFormatConvertDlg::OnNMRClickSongList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
    CMenu* pContextMenu = m_list_popup_menu.GetSubMenu(0); //获取第一个弹出菜单
    m_file_list_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CFormatConvertDlg::OnAddFile()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    //设置过滤器
    wstring filter(CCommon::LoadText(IDS_ALL_SUPPORTED_FORMAT, _T("|")));
    for (const auto& ext : CAudioCommon::m_all_surpported_extensions)
    {
        filter += L"*.";
        filter += ext;
        filter.push_back(L';');
    }
    filter.pop_back();
    filter.push_back(L'|');
    for (const auto& format : CAudioCommon::m_surpported_format)
    {
        filter += format.description;
        filter.push_back(L'|');
        filter += format.extensions_list;
        filter.push_back(L'|');
    }
    filter += CCommon::LoadText(IDS_ALL_FILES, _T("|*.*||"));
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, filter.c_str(), this);
    //设置保存文件名的字符缓冲的大小为128kB（如果以平均一个文件名长度为32字节计算，最多可以打开大约4096个文件）
    fileDlg.m_ofn.nMaxFile = 128 * 1024;
    LPTSTR ch = new TCHAR[fileDlg.m_ofn.nMaxFile];
    fileDlg.m_ofn.lpstrFile = ch;
    //对内存块清零
    ZeroMemory(fileDlg.m_ofn.lpstrFile, sizeof(TCHAR) * fileDlg.m_ofn.nMaxFile);
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        POSITION posFile = fileDlg.GetStartPosition();
        while (posFile != NULL)
        {
            SongInfo item;
            CString file_path = fileDlg.GetNextPathName(posFile);
            item = CSongDataManager::GetInstance().GetSongInfo(wstring(file_path));
            if (!item.info_acquired)	//如果歌曲没有获取过信息，则重新获取
            {
                HSTREAM hStream;
                hStream = BASS_StreamCreateFile(FALSE, item.file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
                BASS_CHANNELINFO channel_info;
                BASS_ChannelGetInfo(hStream, &channel_info);
                if (CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype) != AU_MIDI)
                {
                    bool is_osu = COSUPlayerHelper::IsOsuFile(item.file_path);
                    if (is_osu)
                    {
                        item.file_path = item.file_path;
                        COSUPlayerHelper::GetOSUAudioTitleArtist(item);
                    }
                    else
                    {
                        CBassCore::GetBASSAudioInfo(hStream, item);
                    }
                    //CPlayer::AcquireSongInfo(hStream, item.file_path, item);
                }
                BASS_StreamFree(hStream);
            }
            if (!CCommon::IsItemInVector(m_file_list, [&](const SongInfo song)
                {
                    if (!song.is_cue)
                        return item.file_path == song.file_path;
                    else
                        return (item.file_path == song.file_path && item.track == song.track);
                }))
                m_file_list.push_back(item);
        }
        ShowFileList();
    }
    delete[] ch;
}


void CFormatConvertDlg::OnDeleteSelect()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    int select;
    select = m_file_list_ctrl.GetCurSel();		//获取当前选中项序号
    if (select >= 0 && select < static_cast<int>(m_file_list.size()))
    {
        m_file_list.erase(m_file_list.begin() + select);	//删除选中项
        ShowFileList();
    }
}


void CFormatConvertDlg::OnClearList()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    m_file_list.clear();
    m_file_list_ctrl.DeleteAllItems();
}


void CFormatConvertDlg::OnMoveUp()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    int select;
    select = m_file_list_ctrl.GetCurSel();		//获取当前选中项序号
    if (select < 0 || select >= static_cast<int>(m_file_list.size()))	//当前选中项在索引以外
    {
        return;
    }
    else if (select > 0)
    {
        //交换文件列表中的选中项和前一项
        auto temp = m_file_list[select];
        m_file_list[select] = m_file_list[select - 1];
        m_file_list[select - 1] = temp;
        ShowFileList();		//上移操作完毕重新显示列表
        m_file_list_ctrl.SetCurSel(select - 1);	//设置选中项为前一项
    }
}


void CFormatConvertDlg::OnMoveDown()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    int select;
    select = m_file_list_ctrl.GetCurSel();		//获取当前选中项序号
    if (select < 0 || select >= static_cast<int>(m_file_list.size()))	//当前选中项在索引以外
    {
        return;
    }
    else if (select < static_cast<int>(m_file_list.size()) - 1)
    {
        //交换文件列表中的选中项和后一项
        auto temp = m_file_list[select];
        m_file_list[select] = m_file_list[select + 1];
        m_file_list[select + 1] = temp;
        ShowFileList();		//下移操作完毕重新显示列表
        m_file_list_ctrl.SetCurSel(select + 1);	//设置选中项为前一项
    }
}


BOOL CFormatConvertDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        //按下Ctrl键时
        if (GetKeyState(VK_CONTROL) & 0x80)
        {
            if (pMsg->wParam == VK_UP)		//按Ctr+↑上移
            {
                OnMoveUp();
                return TRUE;
            }
            if (pMsg->wParam == VK_DOWN)	//按Ctr+↓下移
            {
                OnMoveDown();
                return TRUE;
            }
        }
    }
    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CFormatConvertDlg::OnEditTagInfo()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;

    CTagEditDlg dlg(m_file_list, m_item_selected);
    dlg.DoModal();
}


void CFormatConvertDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool item_selected_valid{ m_item_selected >= 0 && m_item_selected < static_cast<int>(m_file_list.size()) };
    pMenu->EnableMenuItem(ID_ADD_FILE, MF_BYCOMMAND | (!m_thread_runing ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_CLEAR_LIST, MF_BYCOMMAND | (!m_thread_runing ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EDIT_TAG_INFO, MF_BYCOMMAND | ((!m_thread_runing && item_selected_valid) ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_SELECT, MF_BYCOMMAND | ((!m_thread_runing && item_selected_valid) ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_MOVE_UP, MF_BYCOMMAND | ((!m_thread_runing && item_selected_valid) ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_MOVE_DOWN, MF_BYCOMMAND | ((!m_thread_runing && item_selected_valid) ? MF_ENABLED : MF_GRAYED));

    pMenu->SetDefaultItem(ID_EDIT_TAG_INFO);
}


void CFormatConvertDlg::OnNMDblclkSongList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_file_list.size()))
        OnEditTagInfo();		//双击打开“编辑标签信息”

    *pResult = 0;
}


void CFormatConvertDlg::OnBnClickedChangeFreqCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CHANGE_FREQ_CHECK);
    m_convert_freq = pBtn->GetCheck() != 0;
    if (m_convert_freq && !m_bass_mix_lib.IsSucceed())
    {
        m_convert_freq = false;
        MessageBox(CCommon::LoadTextFormat(IDS_BASS_MIX_LOAD_ERROR, { m_encode_dir + L"bassmix.dll" }), NULL, MB_ICONWARNING | MB_OK);
        pBtn->SetCheck(FALSE);
    }

    m_freq_comb.EnableWindow(m_convert_freq);
}


void CFormatConvertDlg::OnCbnSelchangeFreqCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    m_freq_comb.GetWindowText(str);
    m_freq_sel = str.GetString();
}


afx_msg LRESULT CFormatConvertDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CBrowseEdit* pEdit = (CBrowseEdit*)lParam;
    CString str;
    if (pEdit == &m_out_dir_edit)
    {
        GetDlgItemText(IDC_OUT_DIR_EDIT, str);
        m_out_dir = str.GetString();
    }
    if (pEdit == &m_out_name_edit)
    {
        GetDlgItemText(IDC_OUT_NAME_EDIT, str);
        m_out_name = str.GetString();
    }
    return 0;
}


void CFormatConvertDlg::OnBnClickedOpenTargetDirCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_open_output_dir = (((CButton*)GetDlgItem(IDC_OPEN_TARGET_DIR_CHECK))->GetCheck() != 0);
}
