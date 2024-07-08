// FormatConvertDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "FormatConvertDlg.h"
#include "BassCore.h"
#include "MusicPlayerCmdHelper.h"
#include "SongDataManager.h"
#include "FileNameFormDlg.h"
#include "MP3EncodeCfgDlg.h"
#include "WmaEncodeCfgDlg.h"
#include "OggEncodeCfgDlg.h"
#include "FlacEncodeCfgDlg.h"
#include "FilterHelper.h"
#include "IniHelper.h"
#include "TagEditDlg.h"

#define MAX_ALBUM_COVER_SIZE (128 * 1024)                           //编码器支持的最大专辑封面大小
#define CONVERT_TEMP_ALBUM_COVER_NAME L"cover_R1hdyFy6CoEK7Gu8"     //临时的专辑封面文件名
#define COMPRESSED_ALBUM_COVER_PIXEL 512                            //要将专辑封面图片压缩的尺寸


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
    for (SongInfo item : items)
    {
        item.Normalize();
        m_file_list.push_back(std::move(item));
    }
    // 确保音频文件信息更新到媒体库，并解析cue，(对于大量不存在于媒体库的新文件此方法很慢)
    int cnt{}, percent{};
    bool exit_flag{ false };
    CAudioCommon::GetAudioInfo(m_file_list, cnt, exit_flag, percent, MR_MIN_REQUIRED, false);
    // 去重
    std::unordered_set<SongKey> song_key;
    song_key.reserve(m_file_list.size());
    auto new_end = std::remove_if(m_file_list.begin(), m_file_list.end(),
        [&](const SongInfo& song) { return !song_key.emplace(song).second; }); // emplace失败说明此项目已存在，返回true移除当前项目
    m_file_list.erase(new_end, m_file_list.end());
    // 加载歌曲信息（与播放列表一致）
    CSongDataManager::GetInstance().LoadSongsInfo(m_file_list);

    //如果文件是 MIDI 音乐，则把SF2音色库信息添加到注释信息
    wstring midi_comment = L"Converted from MIDI by MusicPlayer2. SF2: " + CPlayer::GetInstance().GetSoundFontName();
    for (auto& song : m_file_list)
    {
        if (CAudioCommon::GetAudioTypeByFileName(song.file_path) == AU_MIDI)
            song.comment = midi_comment;
    }

    m_freq_map.emplace_back(L"8 kHz", 8000);
    m_freq_map.emplace_back(L"16 kHz", 16000);
    m_freq_map.emplace_back(L"22 kHz", 22050);
    m_freq_map.emplace_back(L"24 kHz", 24000);
    m_freq_map.emplace_back(L"32 kHz", 32000);
    m_freq_map.emplace_back(L"44.1 kHz", 44100);
    m_freq_map.emplace_back(L"48 kHz", 48000);
}

CFormatConvertDlg::~CFormatConvertDlg()
{
    CPlayer::GetInstance().GetPlayerCore()->UnInitEncoder();
}

CString CFormatConvertDlg::GetDialogName() const
{
    return _T("FormatConvertDlg");

}

bool CFormatConvertDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_FORMAT_CONVERT");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_OUT_FORMAT_SEL");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_OUT_FORMAT_SEL_STATIC, temp.c_str());
    // IDC_OUT_FORMAT_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_SETTING");
    SetDlgItemTextW(IDC_ENCODER_CONFIG_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_FILE_LIST");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_FILE_LIST_STATIC, temp.c_str());
    // IDC_SONG_LIST1
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_OPT");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_CHANGE_FREQ");
    SetDlgItemTextW(IDC_CHANGE_FREQ_CHECK, temp.c_str());
    // IDC_FREQ_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_COPY_TAG");
    SetDlgItemTextW(IDC_COPY_TAG_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_COPY_COVER");
    SetDlgItemTextW(IDC_COPY_ALBUM_COVER_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_OUT_FILE_NAME");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_OUT_FILE_NAME_STATIC, temp.c_str());
    // IDC_OUT_NAME_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_ADD_SERIAL_NUMBER");
    SetDlgItemTextW(IDC_ADD_NUMBER_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_FILE_EXIST_SEL");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_FILE_EXIST_SEL_STATIC, temp.c_str());
    // IDC_TARGET_FILE_EXIST_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_OUT_DIR");
    SetDlgItemTextW(IDC_TXT_FORMAT_CONVERT_OUT_DIR_STATIC, temp.c_str());
    // IDC_OUT_DIR_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_OUT_DIR_OPEN_WHEN_COMPLETE");
    SetDlgItemTextW(IDC_OPEN_TARGET_DIR_CHECK, temp.c_str());
    temp = L"";
    SetDlgItemTextW(IDC_PROGRESS_BAR, temp.c_str());    // 此控件持有的文本会影响接下来的重排，需要先清空
    SetDlgItemTextW(IDC_PROGRESS_TEXT, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_START_CONVERT");
    SetDlgItemTextW(IDC_START_CONVERT_BUTTON, temp.c_str());
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_FORMAT_CONVERT_OUT_FORMAT_SEL_STATIC },
        { CtrlTextInfo::C0, IDC_OUT_FORMAT_COMBO },
        { CtrlTextInfo::R1, IDC_ENCODER_CONFIG_BUTTON, CtrlTextInfo::W32 }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::L2, IDC_CHANGE_FREQ_CHECK, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_FREQ_COMBO }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_FORMAT_CONVERT_OUT_FILE_NAME_STATIC },
        { CtrlTextInfo::C0, IDC_OUT_NAME_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_FORMAT_CONVERT_OUT_DIR_STATIC },
        { CtrlTextInfo::C0, IDC_OUT_DIR_EDIT }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_PROGRESS_BAR },
        { CtrlTextInfo::C0, IDC_PROGRESS_TEXT },
        { CtrlTextInfo::R1, IDC_START_CONVERT_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        }, CtrlTextInfo::W128);
    return true;
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
    m_out_name = ini.GetString(L"format_convert", L"out_name_formular", CFileNameFormDlg::FORMULAR_ORIGINAL.c_str());
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
    CIniHelper ini(theApp.m_config_dir + L"Encoder\\encoder.ini");

    m_mp3_encode_para.encode_type = ini.GetInt(L"mp3_encoder", L"encode_type", 0);
    m_mp3_encode_para.cbr_bitrate = ini.GetInt(L"mp3_encoder", L"cbr_bitrate", 128);
    m_mp3_encode_para.abr_bitrate = ini.GetInt(L"mp3_encoder", L"abr_bitrate", 128);
    m_mp3_encode_para.vbr_quality = ini.GetInt(L"mp3_encoder", L"vbr_quality", 4);
    m_mp3_encode_para.cmd_para = ini.GetString(L"mp3_encoder", L"cmd_para", L"");
    m_mp3_encode_para.joint_stereo = ini.GetBool(L"mp3_encoder", L"joint_stereo", true);
    if (m_mp3_encode_para.encode_type == 3)    // 旧版兼容
    {
        m_mp3_encode_para.encode_type = 0;
        m_mp3_encode_para.cmd_para.clear();
    }
    if (!m_mp3_encode_para.cmd_para.empty())
    {
        m_mp3_encode_para.user_define_para = true;
    }
    CMP3EncodeCfgDlg::EncodeParaToCmdline(m_mp3_encode_para);

    m_wma_encode_para.cbr = ini.GetBool(L"wma_encoder", L"cbr", true);
    m_wma_encode_para.cbr_bitrate = ini.GetInt(L"wma_encoder", L"cbr_bitrate", 64);
    m_wma_encode_para.vbr_quality = ini.GetInt(L"wma_encoder", L"vbr_quality", 75);

    m_ogg_encode_para.encode_quality = ini.GetInt(L"ogg_encoder", L"quality", 4);

    m_flac_encode_para.compression_level = ini.GetInt(L"flac_encoder", L"compression_level", 8);
    m_flac_encode_para.user_define_para = ini.GetBool(L"flac_encoder", L"user_define_para", false);
    m_flac_encode_para.cmd_para = ini.GetString(L"flac_encoder", L"cmd_para", L"");
}

void CFormatConvertDlg::SaveEncoderConfig() const
{
    std::wstring encoder_dir = theApp.m_config_dir + L"Encoder\\";
    CCommon::CreateDir(encoder_dir);
    CIniHelper ini(encoder_dir + L"encoder.ini");

    ini.WriteInt(L"mp3_encoder", L"encode_type", m_mp3_encode_para.encode_type);
    ini.WriteInt(L"mp3_encoder", L"cbr_bitrate", m_mp3_encode_para.cbr_bitrate);
    ini.WriteInt(L"mp3_encoder", L"abr_bitrate", m_mp3_encode_para.abr_bitrate);
    ini.WriteInt(L"mp3_encoder", L"vbr_quality", m_mp3_encode_para.vbr_quality);
    ini.WriteString(L"mp3_encoder", L"cmd_para", m_mp3_encode_para.user_define_para ? m_mp3_encode_para.cmd_para : L"");
    ini.WriteBool(L"mp3_encoder", L"joint_stereo", m_mp3_encode_para.joint_stereo);

    ini.WriteBool(L"wma_encoder", L"cbr", m_wma_encode_para.cbr);
    ini.WriteInt(L"wma_encoder", L"cbr_bitrate", m_wma_encode_para.cbr_bitrate);
    ini.WriteInt(L"wma_encoder", L"vbr_quality", m_wma_encode_para.vbr_quality);

    ini.WriteInt(L"ogg_encoder", L"quality", m_ogg_encode_para.encode_quality);

    ini.WriteInt(L"flac_encoder", L"compression_level", m_flac_encode_para.compression_level);
    ini.WriteBool(L"flac_encoder", L"user_define_para", m_flac_encode_para.user_define_para);
    ini.WriteString(L"flac_encoder", L"cmd_para", m_flac_encode_para.cmd_para);

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

    SetIcon(IconMgr::IconType::IT_Convert, FALSE);
    SetIcon(IconMgr::IconType::IT_Convert, TRUE);
    SetButtonIcon(IDC_ENCODER_CONFIG_BUTTON, IconMgr::IconType::IT_Setting);
    SetButtonIcon(IDC_START_CONVERT_BUTTON, IconMgr::IconType::IT_Convert);

    LoadConfig();
    LoadEncoderConfig();

    m_encoder_succeed = InitEncoder();
    if (!CPlayer::GetInstance().GetPlayerCore()->IsFreqConvertAvailable())
        m_convert_freq = false;

    //初始化文件列表
    CRect rect;
    m_file_list_ctrl.GetWindowRect(rect);
    int width0{ theApp.DPI(40) }, width1, width2{ theApp.DPI(65) };
    width1 = rect.Width() - width0 - width2 - theApp.DPI(20) - 1;
    //插入列
    m_file_list_ctrl.SetExtendedStyle(m_file_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_file_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width0);
    m_file_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_FILE_NAME").c_str(), LVCFMT_LEFT, width1);
    m_file_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS").c_str(), LVCFMT_LEFT, width2);
    //插入项目
    ShowFileList();
    //设置主题颜色
    //m_file_list_ctrl.SetColor(theApp.m_app_setting_data.theme_color);

    //初始化转换格式的下拉列表
    m_encode_format_combo.AddString(_T("WAV"));
    m_encode_format_combo.AddString(theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_ENCODER_NAME_MP3_LAME").c_str());
    m_encode_format_combo.AddString(_T("WMA"));
    m_encode_format_combo.AddString(_T("OGG"));
    m_encode_format_combo.AddString(_T("FLAC"));
    m_encode_format_combo.SetCurSel(static_cast<int>(m_encode_format));

    //初始化选项控件的状态
    ((CButton*)GetDlgItem(IDC_COPY_TAG_CHECK))->SetCheck(m_write_tag);
    ((CButton*)GetDlgItem(IDC_COPY_ALBUM_COVER_CHECK))->SetCheck(m_write_album_cover);
    ((CButton*)GetDlgItem(IDC_ADD_NUMBER_CHECK))->SetCheck(m_add_file_serial_num);
    CComboBox* file_exist_combo = (CComboBox*)GetDlgItem(IDC_TARGET_FILE_EXIST_COMBO);
    file_exist_combo->AddString(theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_FILE_EXIST_AUTO_RENAME").c_str());
    file_exist_combo->AddString(theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_FILE_EXIST_IGNORE").c_str());
    file_exist_combo->AddString(theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_FILE_EXIST_OVERWRITE").c_str());
    file_exist_combo->SetCurSel(m_file_exist_action);
    ((CButton*)GetDlgItem(IDC_OPEN_TARGET_DIR_CHECK))->SetCheck(m_open_output_dir);

    int freq_comb_sel{ -1 };
    for (size_t i{}; i < m_freq_map.size(); ++i)
    {
        m_freq_comb.AddString(m_freq_map[i].first.c_str());
        if (m_freq_map[i].first == m_freq_sel)
            freq_comb_sel = i;
        if (m_freq_map[i].second == 44100)          // 默认值
            m_freq_comb.SetCurSel(i);
    }
    if (freq_comb_sel != -1)
        m_freq_comb.SetCurSel(freq_comb_sel);
    m_freq_comb.EnableWindow(m_convert_freq);
    ((CButton*)GetDlgItem(IDC_CHANGE_FREQ_CHECK))->SetCheck(m_convert_freq);

    m_out_name_edit.SetWindowText(m_out_name.c_str());
    m_out_name_edit.SetEditBrowseMode(CBrowseEdit::EditBrowseMode::RENAME);

    if (!m_out_dir.empty() && m_out_dir.back() != L'\\')
        m_out_dir.push_back(L'\\');
    m_out_dir_edit.SetWindowText(m_out_dir.c_str());
    m_out_dir_edit.EnableFolderBrowseButton(theApp.m_str_table.LoadText(L"TITLE_FOLDER_BROWSER_OUTPUT_FOLDER").c_str());
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
    GetDlgItem(IDC_START_CONVERT_BUTTON)->EnableWindow(enable);
    GetDlgItem(IDC_ENCODER_CONFIG_BUTTON)->EnableWindow(enable);
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
    return CPlayer::GetInstance().GetPlayerCore()->InitEncoder();
}

bool CFormatConvertDlg::EncodeSingleFile(CFormatConvertDlg* pthis, int file_index)
{
    //设置输出文件路径
    SongInfo& song_info{ pthis->m_file_list[file_index] };

    // 输出文件路径
    wstring out_file_path{ pthis->m_out_dir };

    // 为目标文件添加序号
    if (pthis->m_add_file_serial_num)
    {
        CString index_str;
        if (pthis->m_file_list.size() < 10)
            index_str.Format(_T("%d."), file_index + 1);
        else if (pthis->m_file_list.size() < 100)
            index_str.Format(_T("%02d."), file_index + 1);
        else
            index_str.Format(_T("%03d."), file_index + 1);
        out_file_path += index_str;
    }

    // 按照格式字符串生成输出文件名(这里缺少长度检查)
    out_file_path += CFileNameFormDlg::FileNameFromTag(pthis->m_out_name, song_info);

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
    case EncodeFormat::FLAC:
        out_file_path += L".flac";
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

    void* para{};
    switch (pthis->m_encode_format)
    {
    case EncodeFormat::MP3:
        para = &pthis->m_mp3_encode_para;
        break;
    case EncodeFormat::WMA:
        para = &pthis->m_wma_encode_para;
        break;
    case EncodeFormat::OGG:
        para = &pthis->m_ogg_encode_para;
        break;
    case EncodeFormat::FLAC:
        para = &pthis->m_flac_encode_para;
    default:
        break;

    }

    int freq{};
    if (CPlayer::GetInstance().GetPlayerCore()->IsFreqConvertAvailable() && pthis->m_convert_freq)
        freq = pthis->GetFreq();

    static int _file_index{};
    _file_index = file_index;
    static CFormatConvertDlg* _pthis{};
    _pthis = pthis;
    //执行转换格式
    if (!CPlayer::GetInstance().GetPlayerCore()->EncodeAudio(song_info, out_file_path, pthis->m_encode_format, para, freq, [](int progress)
        {
            ::PostMessage(_pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, _file_index, progress);
        }))
        return false;

    //转换完成后向目标文件写入标签信息和专辑封面
    SongInfo song_info_out{ song_info };
    song_info_out.file_path = out_file_path;
    song_info_out.is_cue = false;
    //写入标签信息
    if (pthis->m_write_tag)
    {
        CAudioTag audio_tag_out(song_info_out);
        audio_tag_out.WriteAudioTag();
    }
    //写入专辑封面
    if (pthis->m_write_album_cover)
    {
        //获取原始文件的专辑封面
        SongInfo song_info_tmp;
        song_info_tmp.file_path = song_info.file_path;
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
        //将专辑封面写入目标文件
        if (!album_cover_path.empty())
        {
            CAudioTag audio_tag_out(song_info_out);
            audio_tag_out.WriteAlbumCover(album_cover_path);
        }
    }

    return true;
}


void CFormatConvertDlg::SetProgressInfo(int progress)
{
    wstring info;
    if (progress >= 100)
        info = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_PROGRESS_INFO_COMPLETE");
    else
        info = theApp.m_str_table.LoadTextFormat(L"TXT_FORMAT_CONVERT_PROGRESS_INFO", { progress });
    SetDlgItemText(IDC_PROGRESS_TEXT, info.c_str());
}

int CFormatConvertDlg::GetFreq()
{
    int sel_index = m_freq_comb.GetCurSel();
    return m_freq_map[sel_index].second;
}

UINT CFormatConvertDlg::ThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
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
        CBassCore* bass_core = dynamic_cast<CBassCore*>(CPlayer::GetInstance().GetPlayerCore());
        wstring info;
        if (bass_core != nullptr)
            info = theApp.m_str_table.LoadTextFormat(L"MSG_FORMAT_CONVERT_BASS_LOAD_ERROR", { bass_core->GetEncoderDir() + L"bassenc.dll" });
        else
            info = theApp.m_str_table.LoadText(L"MSG_FORMAT_CONVERT_INIT_ERROR");
        MessageBox(info.c_str(), NULL, MB_ICONERROR | MB_OK);
        return;
    }

    if (m_out_dir.empty())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_FORMAT_CONVERT_SET_OUTPUT_DIR");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        return;
    }
    else if (!CCommon::FolderExist(m_out_dir))
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_FORMAT_CONVERT_OUTPUT_DIR_NOT_EXIST", { m_out_dir });
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
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
    if (theApp.IsTaskbarInteractionEnabled())
        theApp.GetITaskbarList3()->SetProgressState(this->GetSafeHwnd(), TBPF_INDETERMINATE);
}


afx_msg LRESULT CFormatConvertDlg::OnConvertProgress(WPARAM wParam, LPARAM lParam)
{
    wstring status_str;
    int percent = (int)lParam;
    if (percent == 0)
        m_file_list_ctrl.EnsureVisible(wParam, FALSE);		//转换开始时，确保当前列表项可见
    if (percent < 0)
    {
        //显示错误信息
        if (percent == CONVERT_ERROR_FILE_CANNOT_OPEN)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_FILE_CANNOT_OPEN");
        else if (percent == CONVERT_ERROR_ENCODE_CHANNEL_FAILED)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_ENCODE_CHANNEL_FAILED");
        else if (percent == CONVERT_ERROR_ENCODE_PARA_ERROR)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_ENCODE_PARA_ERROR");
        else if (percent == CONVERT_ERROR_MIDI_NO_SF2)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_MIDI_NO_SF2");
        else if (percent == CONVERT_ERROR_WMA_NO_WMP9_OR_LATER)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_NO_WMP9_OR_LATER");
        else if (percent == CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER)
            status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_ERROR_NO_SUPPORTED_ENCODER");
        else
            status_str = theApp.m_str_table.LoadTextFormat(L"TXT_FORMAT_CONVERT_STAUS_ERROR_OTHER", { percent });
    }
    else if (percent == 101)
    {
        status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_COMPLETE");
    }
    else if (percent == 102)
    {
        status_str = theApp.m_str_table.LoadText(L"TXT_FORMAT_CONVERT_STAUS_SKIPPED");
    }
    else
    {
        status_str = std::to_wstring(static_cast<int>(lParam)) + L'%';
    }
    m_file_list_ctrl.SetItemText(wParam, 2, status_str.c_str());

    //总体的进度
    int position, length;
    length = m_file_list.size() * 100;
    position = wParam * 100 + percent;
    if (theApp.IsTaskbarInteractionEnabled())
        theApp.GetITaskbarList3()->SetProgressValue(this->GetSafeHwnd(), position, length);
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
        dlg.m_encode_para = m_ogg_encode_para;
        if (dlg.DoModal() == IDOK)
            m_ogg_encode_para = dlg.m_encode_para;
    }
    break;
    case EncodeFormat::FLAC:
    {
        CFlacEncodeCfgDlg dlg;
        dlg.m_encode_para = m_flac_encode_para;
        if (dlg.DoModal() == IDOK)
            m_flac_encode_para = dlg.m_encode_para;
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
    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::FcListMenu);
    m_file_list_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CFormatConvertDlg::OnAddFile()
{
    // TODO: 在此添加命令处理程序代码
    if (m_thread_runing)
        return;
    //设置过滤器
    wstring filter = FilterHelper::GetAudioFileFilter();
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, filter.c_str(), this);
    //设置保存文件名的字符缓冲的大小为128kB（如果以平均一个文件名长度为32字节计算，最多可以打开大约4096个文件）
    fileDlg.m_ofn.nMaxFile = 128 * 1024;
    std::vector<wchar_t> buffer(fileDlg.m_ofn.nMaxFile);
    fileDlg.m_ofn.lpstrFile = buffer.data();
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        wchar_t* pos = buffer.data();
        wchar_t* end = pos + buffer.size();
        while (pos < end && *pos != L'\0')
        {
            wstring file_name(pos);
            // 将指针移动到下一个文件名的起始位置
            pos += file_name.size() + 1;
            m_file_list.push_back(SongInfo(file_name));
        }
        // 音频文件信息更新到媒体库，并解析cue，(对于大量不存在于媒体库的新文件此方法很慢)
        int cnt{}, percent{};
        bool exit_flag{ false };
        CAudioCommon::GetAudioInfo(m_file_list, cnt, exit_flag, percent, MR_MIN_REQUIRED, false);
        // 去重
        std::unordered_set<SongKey> song_key;
        song_key.reserve(m_file_list.size());
        auto new_end = std::remove_if(m_file_list.begin(), m_file_list.end(),
            [&](const SongInfo& song) { return !song_key.emplace(song).second; }); // emplace失败说明此项目已存在，返回true移除当前项目
        m_file_list.erase(new_end, m_file_list.end());
        // 加载歌曲信息（与播放列表一致）
        CSongDataManager::GetInstance().LoadSongsInfo(m_file_list);
        //如果文件是 MIDI 音乐，则把SF2音色库信息添加到注释信息
        wstring midi_comment = L"Converted from MIDI by MusicPlayer2. SF2: " + CPlayer::GetInstance().GetSoundFontName();
        for (auto& song : m_file_list)
        {
            if (CAudioCommon::GetAudioTypeByFileName(song.file_path) == AU_MIDI)
                song.comment = midi_comment;
        }
        // 刷新显示
        ShowFileList();
    }
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
    if (m_convert_freq && !CPlayer::GetInstance().GetPlayerCore()->IsFreqConvertAvailable())
    {
        m_convert_freq = false;
        CBassCore* bass_core = dynamic_cast<CBassCore*>(CPlayer::GetInstance().GetPlayerCore());
        if (bass_core != nullptr)
        {
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_FORMAT_CONVERT_BASS_MIX_LOAD_ERROR", { bass_core->GetEncoderDir() + L"bassmix.dll" });
            MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        }
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
