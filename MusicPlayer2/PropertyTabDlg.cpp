// PropertyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PropertyTabDlg.h"
#include "afxdialogex.h"
#include "COSUPlayerHelper.h"
#include "PropertyDlgHelper.h"
#include "SongDataManager.h"
#include "GetTagOnlineDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "TagFromFileNameDlg.h"
#include "SongInfoHelper.h"

// CPropertyTabDlg 对话框

static void CopyMultiTagInfo(const wstring& str_src, wstring& str_dest)
{
    static wstring str_multi_value = CCommon::LoadText(IDS_MULTI_VALUE).GetString();
    if (str_src != str_multi_value)
        str_dest = str_src;
}

//更改标签信息后更新
static void UpdateSongInfo(SongInfo song)
{
    ////重新从文件读取该歌曲的标签
    //HSTREAM hStream;
    //hStream = BASS_StreamCreateFile(FALSE, song.file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    ////CAudioCommon::GetAudioTags(hStream, AudioType::AU_MP3, CPlayer::GetInstance().GetCurrentDir(), m_all_song_info[m_index]);
    //CAudioTag audio_tag(song, hStream);
    //audio_tag.GetAudioTag();
    //BASS_StreamFree(hStream);
    CSongDataManager::GetInstance().GetSongInfoRef(song.file_path).CopyAudioTag(song);
    CSongDataManager::GetInstance().SetSongDataModified();
}

IMPLEMENT_DYNAMIC(CPropertyTabDlg, CTabDlg)

CPropertyTabDlg::CPropertyTabDlg(vector<SongInfo>& all_song_info, int& index, CWnd* pParent /*=NULL*/, bool read_only)
    : CTabDlg(IDD_PROPERTY_DIALOG, pParent), m_all_song_info{ all_song_info }, m_index{ index }, m_read_only{ read_only },
    m_batch_edit{ false }
{
    m_song_num = static_cast<int>(all_song_info.size());
}

CPropertyTabDlg::CPropertyTabDlg(vector<SongInfo>& song_info, CWnd* pParent /*= NULL*/)
    : CTabDlg(IDD_PROPERTY_DIALOG, pParent), m_all_song_info{ song_info }, m_index{ m_no_use }, m_batch_edit{ true }
{
    m_song_num = static_cast<int>(song_info.size());
}

CPropertyTabDlg::~CPropertyTabDlg()
{
}

void CPropertyTabDlg::ShowInfo()
{
    if (m_batch_edit)
    {
        CPropertyDlgHelper helper(m_all_song_info);
        m_file_name_edit.SetWindowText(helper.GetMultiFileName().c_str());
        m_file_path_edit.SetWindowText(helper.GetMultiFilePath().c_str());
        m_file_type_edit.SetWindowText(helper.GetMultiType().c_str());
        m_song_length_edit.SetWindowText(helper.GetMultiLength().c_str());
        m_file_size_edit.SetWindowText(helper.GetMultiSize().c_str());
        m_bit_rate_edit.SetWindowText(helper.GetMultiBitrate().c_str());
        SetDlgItemText(IDC_SAMPLE_FREQ, helper.GetMultiFreq().c_str());
        SetDlgItemText(IDC_BITS_DIPTH, helper.GetMultiBits().c_str());
        SetDlgItemText(IDC_CHANNELS, helper.GetMultiChannels().c_str());
        m_lyric_file_edit.SetWindowText(_T(""));
        m_title_edit.SetWindowText(helper.GetMultiTitle().c_str());
        m_artist_edit.SetWindowText(helper.GetMultiArtist().c_str());
        m_album_edit.SetWindowText(helper.GetMultiAlbum().c_str());
        m_year_edit.SetWindowText(helper.GetMultiYear().c_str());
        m_track_edit.SetWindowText(helper.GetMultiTrack().c_str());
        m_genre_combo.SetWindowText(helper.GetMultiGenre().c_str());
        m_comment_edit.SetWindowText(helper.GetMultiComment().c_str());
        SetDlgItemText(IDC_TAG_TYPE_STATIC, _T(""));
    }
    else
    {
        //显示文件名
        m_file_name_edit.SetWindowText(m_all_song_info[m_index].GetFileName().c_str());

        //显示文件路径
        m_file_path_edit.SetWindowText((m_all_song_info[m_index].file_path).c_str());

        //显示文件类型
        wstring file_type;
        CFilePathHelper file_path{ m_all_song_info[m_index].file_path };
        file_type = file_path.GetFileExtension();
        //if (file_type == _T("mp3"))
        //  m_file_type_edit.SetWindowText(_T("MP3音频文件"));
        //else if (file_type == _T("wma"))
        //  m_file_type_edit.SetWindowText(_T("Windows Media 音频文件"));
        //else if (file_type == _T("wav"))
        //  m_file_type_edit.SetWindowText(_T("WAV音频文件"));
        //else if (file_type == _T("mid"))
        //  m_file_type_edit.SetWindowText(_T("MIDI序列"));
        //else if (file_type == _T("ogg"))
        //  m_file_type_edit.SetWindowText(_T("OGG音频文件"));
        //else if (file_type == _T("m4a"))
        //  m_file_type_edit.SetWindowText(_T("MPEG-4 音频文件"));
        //else
        //  m_file_type_edit.SetWindowText((file_type + _T("文件")).c_str());
        m_file_type_edit.SetWindowText((CAudioCommon::GetAudioDescriptionByExtension(file_type)).c_str());

        //显示文件长度
        wstring song_length;
        if (m_all_song_info[m_index].lengh.isZero())
            song_length = CCommon::LoadText(IDS_CANNOT_GET_SONG_LENGTH);
        else
            song_length = m_all_song_info[m_index].lengh.toString2();
        m_song_length_edit.SetWindowText(song_length.c_str());

        //显示文件大小
        size_t file_size;
        file_size = CCommon::GetFileSize(m_all_song_info[m_index].file_path);
        m_file_size_edit.SetWindowText(CCommon::DataSizeToString(file_size));

        //显示比特率
        m_bit_rate_edit.SetWindowText(CSongInfoHelper::GetBitrateString(m_all_song_info[m_index]));

        //显示采样频率
        SetDlgItemText(IDC_SAMPLE_FREQ, CSongInfoHelper::GetFreqString(m_all_song_info[m_index]));

        //显示位深度
        SetDlgItemText(IDC_BITS_DIPTH, CSongInfoHelper::GetBitsString(m_all_song_info[m_index]));

        //显示位声道数
        SetDlgItemText(IDC_CHANNELS, CSongInfoHelper::GetChannelsString(m_all_song_info[m_index]));

        //显示歌词路径
        if(m_all_song_info[m_index].IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()) && CPlayer::GetInstance().IsInnerLyric())
            m_lyric_file_edit.SetWindowText(CCommon::LoadText(IDS_INNER_LYRIC));
        else if(!m_all_song_info[m_index].lyric_file.empty())
            m_lyric_file_edit.SetWindowText(m_all_song_info[m_index].lyric_file.c_str());
        else
            m_lyric_file_edit.SetWindowText(CCommon::LoadText(IDS_NO_MATCHED_LYRIC));

        //显示音频信息
        //CString info;
        m_title_edit.SetWindowText(m_all_song_info[m_index].GetTitle().c_str());
        m_artist_edit.SetWindowText(m_all_song_info[m_index].GetArtist().c_str());
        m_album_edit.SetWindowText(m_all_song_info[m_index].GetAlbum().c_str());
        m_year_edit.SetWindowText(m_all_song_info[m_index].GetYear().c_str());
        CString info;
        if (m_all_song_info[m_index].track != 0)
            info.Format(_T("%d"), m_all_song_info[m_index].track);
        else
            info = _T("");
        m_track_edit.SetWindowText(info);
        m_genre_combo.SetWindowText(m_all_song_info[m_index].GetGenre().c_str());
        m_comment_edit.SetWindowText(m_all_song_info[m_index].comment.c_str());

        //显示标签类型
        auto tag_type = m_all_song_info[m_index].tag_type;
        if (tag_type != 0)
        {
            CString tag_type_str{ CCommon::LoadText(IDS_MP3_TAG_TYPE) };
            //if (tag_type == 0)
            //{
            //    tag_type_str += CCommon::LoadText(IDS_OTHER);
            //}
            //else
            //{
            if (tag_type & T_ID3V1)
                tag_type_str += _T("ID3v1 ");
            if (tag_type & T_ID3V2)
                tag_type_str += _T("ID3v2 ");
            if (tag_type & T_APE)
                tag_type_str += _T("APE ");
            if (tag_type & T_RIFF)
                tag_type_str += _T("RIFF ");
            if (tag_type & T_MP4)
                tag_type_str += _T("MP4 ");
            //}
            SetDlgItemText(IDC_TAG_TYPE_STATIC, tag_type_str);
        }
        else
        {
            SetDlgItemText(IDC_TAG_TYPE_STATIC, _T(""));
        }
    }
}

void CPropertyTabDlg::SetEditReadOnly(bool read_only)
{
    m_title_edit.SetReadOnly(read_only);
    m_artist_edit.SetReadOnly(read_only);
    m_album_edit.SetReadOnly(read_only);
    m_track_edit.SetReadOnly(read_only);
    m_year_edit.SetReadOnly(read_only);
    //m_genre_edit.SetReadOnly(read_only);
    //((CEdit*)m_genre_combo.GetWindow(GW_CHILD))->SetReadOnly(read_only);      //将combo box设为只读
    m_genre_combo.SetReadOnly(read_only);
    m_comment_edit.SetReadOnly(read_only);
}

void CPropertyTabDlg::SetWreteEnable()
{
    if (m_batch_edit)
    {
        CPropertyDlgHelper helper(m_all_song_info);
        m_write_enable = helper.IsMultiWritable();
    }
    else
    {
        CFilePathHelper file_path{ m_all_song_info[m_index].file_path };
        m_write_enable = (!m_all_song_info[m_index].is_cue && !COSUPlayerHelper::IsOsuFile(file_path.GetFilePath()) && CAudioTag::IsFileTypeTagWriteSupport(file_path.GetFileExtension())/* && m_all_song_info[m_index].tag_type != 2*/);
    }
    m_write_enable &= !m_read_only;
    SetEditReadOnly(!m_write_enable);
    SetSaveBtnEnable();
    if (m_write_enable)
        m_year_edit.SetLimitText(4);
    else
        m_year_edit.SetLimitText(-1);

    EnableDlgCtrl(IDC_GET_TAG_FROM_LYRIC_BUTTON, m_write_enable);
    EnableDlgCtrl(IDC_GET_TAG_ONLINE_BUTTON, m_write_enable);
    EnableDlgCtrl(IDC_GET_TAG_FROM_FILE_NAME_BUTTON, m_write_enable);
}


void CPropertyTabDlg::SetSaveBtnEnable()
{
    bool enable = m_write_enable && m_modified;
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        pParent->SendMessage(WM_PROPERTY_DIALOG_MODIFIED, enable);
}

void CPropertyTabDlg::ResetEditModified()
{
    m_title_edit.ResetModified();
    m_artist_edit.ResetModified();
    m_album_edit.ResetModified();
    m_track_edit.ResetModified();
    m_year_edit.ResetModified();
    m_comment_edit.ResetModified();
    m_genre_combo.ResetModified();
}

void CPropertyTabDlg::OnTabEntered()
{
    SetWreteEnable();
    ShowInfo();
}

void CPropertyTabDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILE_NAME_EDIT, m_file_name_edit);
    DDX_Control(pDX, IDC_FILE_PATH_EDIT, m_file_path_edit);
    DDX_Control(pDX, IDC_FILE_TYPE_EDIT, m_file_type_edit);
    DDX_Control(pDX, IDC_SONG_LENGTH_EDIT, m_song_length_edit);
    DDX_Control(pDX, IDC_FILE_SIZE_EDIT, m_file_size_edit);
    DDX_Control(pDX, IDC_BIT_RATE_EDIT, m_bit_rate_edit);
    DDX_Control(pDX, IDC_TITEL_EDIT, m_title_edit);
    DDX_Control(pDX, IDC_ARTIST_EDIT, m_artist_edit);
    DDX_Control(pDX, IDC_ALBUM_EDIT, m_album_edit);
    DDX_Control(pDX, IDC_TRACK_EDIT, m_track_edit);
    DDX_Control(pDX, IDC_YEAR_EDIT, m_year_edit);
    //DDX_Control(pDX, IDC_GENRE_EDIT, m_genre_edit);
    DDX_Control(pDX, IDC_COMMENT_EDIT, m_comment_edit);
    DDX_Control(pDX, IDC_LYRIC_FILE_EDIT, m_lyric_file_edit);
    DDX_Control(pDX, IDC_GENRE_COMBO, m_genre_combo);
}


BEGIN_MESSAGE_MAP(CPropertyTabDlg, CTabDlg)
    ON_WM_MOUSEWHEEL()
    ON_EN_CHANGE(IDC_TITEL_EDIT, &CPropertyTabDlg::OnEnChangeTitelEdit)
    ON_EN_CHANGE(IDC_ARTIST_EDIT, &CPropertyTabDlg::OnEnChangeArtistEdit)
    ON_EN_CHANGE(IDC_ALBUM_EDIT, &CPropertyTabDlg::OnEnChangeAlbumEdit)
    ON_EN_CHANGE(IDC_TRACK_EDIT, &CPropertyTabDlg::OnEnChangeTrackEdit)
    ON_EN_CHANGE(IDC_YEAR_EDIT, &CPropertyTabDlg::OnEnChangeYearEdit)
    ON_EN_CHANGE(IDC_COMMENT_EDIT, &CPropertyTabDlg::OnEnChangeCommentEdit)
    //ON_CBN_EDITCHANGE(IDC_GENRE_COMBO, &CPropertyTabDlg::OnCbnEditchangeGenreCombo)
    ON_CBN_SELCHANGE(IDC_GENRE_COMBO, &CPropertyTabDlg::OnCbnSelchangeGenreCombo)
    //ON_BN_CLICKED(IDC_BUTTON3, &CPropertyTabDlg::OnBnClickedButton3)
    ON_WM_MOUSEWHEEL()
    ON_CBN_EDITCHANGE(IDC_GENRE_COMBO, &CPropertyTabDlg::OnCbnEditchangeGenreCombo)
    ON_BN_CLICKED(IDC_GET_TAG_ONLINE_BUTTON, &CPropertyTabDlg::OnBnClickedGetTagOnlineButton)
    ON_MESSAGE(WM_PORPERTY_ONLINE_INFO_ACQUIRED, &CPropertyTabDlg::OnPorpertyOnlineInfoAcquired)
    ON_BN_CLICKED(IDC_GET_TAG_FROM_LYRIC_BUTTON, &CPropertyTabDlg::OnBnClickedGetTagFromLyricButton)
    ON_BN_CLICKED(IDC_GET_TAG_FROM_FILE_NAME_BUTTON, &CPropertyTabDlg::OnBnClickedGetTagFromFileNameButton)
END_MESSAGE_MAP()


// CPropertyTabDlg 消息处理程序


BOOL CPropertyTabDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_modified = false;
    m_genre_modified = false;
    m_list_refresh = false;

    //初始化流派列表
    CAudioCommon::EmulateGenre([&](const wstring& genre_str)
    {
        m_genre_combo.AddString(genre_str.c_str());
    }, true);
    CRect rect;
    m_genre_combo.SetMinVisibleItems(15);       //设置下拉列表的高度

    //m_genre_combo.SetEditReadOnly();

    ShowDlgCtrl(IDC_GET_TAG_ONLINE_BUTTON, !m_batch_edit && !m_read_only);
    ShowDlgCtrl(IDC_GET_TAG_FROM_LYRIC_BUTTON, !m_read_only);
    ShowDlgCtrl(IDC_GET_TAG_FROM_FILE_NAME_BUTTON, !m_read_only);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPropertyTabDlg::PagePrevious()
{
    m_modified = false;
    m_genre_modified = false;
    m_index--;
    if (m_index < 0) m_index = m_song_num - 1;
    if (m_index < 0) m_index = 0;
    SetWreteEnable();
    ShowInfo();
}


void CPropertyTabDlg::PageNext()
{
    m_modified = false;
    m_genre_modified = false;
    m_index++;
    if (m_index >= m_song_num) m_index = 0;
    SetWreteEnable();
    ShowInfo();
}


BOOL CPropertyTabDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    //if (pMsg->message == WM_KEYDOWN)
    //{
    //  if (pMsg->wParam == VK_UP)
    //  {
    //      PagePrevious();
    //      return TRUE;
    //  }
    //  if (pMsg->wParam == VK_DOWN)
    //  {
    //      PageNext();
    //      return TRUE;
    //  }
    //}
    return CTabDlg::PreTranslateMessage(pMsg);
}


void CPropertyTabDlg::OnEnChangeTitelEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_title_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnEnChangeArtistEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_artist_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnEnChangeAlbumEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_album_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnEnChangeTrackEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_track_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnEnChangeYearEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_year_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnEnChangeCommentEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_modified = (m_comment_edit.GetModify() != 0);
    SetSaveBtnEnable();
}


//void CPropertyTabDlg::OnCbnEditchangeGenreCombo()
//{
//  // TODO: 在此添加控件通知处理程序代码
//  m_modified = true;
//}


int CPropertyTabDlg::SaveModified()
{
    if (!m_write_enable) return false;
    CWaitCursor wait_cursor;
    SongInfo song_info;
    CString str_temp;
    m_title_edit.GetWindowText(str_temp);
    song_info.title = str_temp;
    m_artist_edit.GetWindowText(str_temp);
    song_info.artist = str_temp;
    m_album_edit.GetWindowText(str_temp);
    song_info.album = str_temp;
    CString str_track;
    m_track_edit.GetWindowText(str_track);
    song_info.track = static_cast<BYTE>(_wtoi(str_track));
    CString str_year;
    m_year_edit.GetWindowText(str_year);
    song_info.SetYear(str_year);
    m_genre_combo.GetWindowText(str_temp);
    song_info.genre = str_temp;
    if (m_genre_modified)
        song_info.genre_idx = CAudioCommon::GenreIndex(song_info.genre);
    else
        song_info.genre_idx = m_all_song_info[m_index].genre_idx;       //如果流派没有修改，则将原来的流派号写回文件中
    m_comment_edit.GetWindowText(str_temp);
    song_info.comment = str_temp;

    song_info.Normalize();

    if (m_batch_edit)
    {
        int saved_count{};
        for (int i{}; i < m_song_num; i++)
        {
            SongInfo& cur_song = m_all_song_info[i];
            CopyMultiTagInfo(song_info.title, cur_song.title);
            CopyMultiTagInfo(song_info.artist, cur_song.artist);
            CopyMultiTagInfo(song_info.album, cur_song.album);
            if (str_year != CCommon::LoadText(IDS_MULTI_VALUE))
            {
                cur_song.SetYear(str_year);
            }
            CopyMultiTagInfo(song_info.genre, cur_song.genre);
            CopyMultiTagInfo(song_info.comment, cur_song.comment);
            if (str_track != CCommon::LoadText(IDS_MULTI_VALUE))
            {
                cur_song.track = static_cast<BYTE>(_wtoi(str_track));
            }
            CPlayer::ReOpen reopen(cur_song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()));       //如果保存的是正在播放的曲目，则保存前需要关闭，保存后重新打开
            CAudioTag audio_tag(cur_song);
            if (audio_tag.WriteAudioTag())
            {
                UpdateSongInfo(cur_song);
                saved_count++;
            }
        }
        if (saved_count > 0)
        {
            m_modified = false;
            SetSaveBtnEnable();
            ResetEditModified();
        }
        return saved_count;
    }
    else
    {
        song_info.file_path = m_all_song_info[m_index].file_path;
        CPlayer::ReOpen reopen(song_info.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()));       //如果保存的是正在播放的曲目，则保存前需要关闭，保存后重新打开
        CAudioTag audio_tag(song_info);
        bool saved = audio_tag.WriteAudioTag();
        if (saved)
        {
            //重新从文件读取该歌曲的标签
            HSTREAM hStream;
            hStream = BASS_StreamCreateFile(FALSE, song_info.file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
            //CAudioCommon::GetAudioTags(hStream, AudioType::AU_MP3, CPlayer::GetInstance().GetCurrentDir(), m_all_song_info[m_index]);
            CAudioTag audio_tag(m_all_song_info[m_index], hStream);
            audio_tag.GetAudioTag();
            BASS_StreamFree(hStream);
            CSongDataManager::GetInstance().GetSongInfoRef2(m_all_song_info[m_index].file_path).CopyAudioTag(m_all_song_info[m_index]);
            CSongDataManager::GetInstance().SetSongDataModified();

            m_modified = false;
            SetSaveBtnEnable();
            ResetEditModified();
            m_list_refresh = true;
        }
        return saved;
    }
}


void CPropertyTabDlg::OnCbnSelchangeGenreCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_modified = true;
    m_genre_modified = true;
    SetSaveBtnEnable();
}


//用于测试
//void CPropertyTabDlg::OnBnClickedButton3()
//{
//  // TODO: 在此添加控件通知处理程序代码
//  wstring str = CCommon::GetRandomString(32);
//
//  //重新从文件读取该歌曲的标签
//  wstring file_path;
//  file_path = CPlayer::GetInstance().GetCurrentDir() + m_all_song_info[m_index].file_name;
//  HSTREAM hStream;
//  hStream = BASS_StreamCreateFile(FALSE, file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
//  CAudioCommon::GetAudioTags(hStream, AudioType::AU_MP3, m_all_song_info[m_index]);
//  BASS_StreamFree(hStream);
//}


BOOL CPropertyTabDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    //return CTabDlg::OnMouseWheel(nFlags, zDelta, pt);
    return TRUE;
}


void CPropertyTabDlg::OnCbnEditchangeGenreCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    CEdit* pEdit = m_genre_combo.GetEditCtrl();
    if (pEdit!=nullptr)
        m_modified = (pEdit->GetModify() != 0);
    SetSaveBtnEnable();
}


void CPropertyTabDlg::OnBnClickedGetTagOnlineButton()
{
    // TODO: 在此添加控件通知处理程序代码

    CGetTagOnlineDlg dlg(m_all_song_info[m_index], this);
    dlg.DoModal();
}


afx_msg LRESULT CPropertyTabDlg::OnPorpertyOnlineInfoAcquired(WPARAM wParam, LPARAM lParam)
{
    CInternetCommon::ItemInfo* pItem = (CInternetCommon::ItemInfo*)wParam;
    if (pItem != nullptr)
    {
        SongInfo song;
        song.title = pItem->title;
        song.artist = pItem->artist;
        song.album = pItem->album;
        if (m_comment_edit.GetWindowTextLength() <= 0)
        {
            CString comment{ _T("id:") };
            comment += pItem->id.c_str();
            song.comment = comment;
        }

        ModifyTagInfo(song);
    }
    return 0;
}


void CPropertyTabDlg::OnBnClickedGetTagFromLyricButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CWaitCursor wait_cursor;
    if (!m_batch_edit)
    {
        SongInfo& song{ m_all_song_info[m_index] };

        SongInfo result;
        if (GetTagFromLyrics(song, result))
            ModifyTagInfo(result);
        else
            MessageBox(CCommon::LoadText(IDS_NO_LYRICS_FOUND_INFO), NULL, MB_ICONWARNING | MB_OK);
    }
    else
    {
        int count{};
        vector<SongInfo> list_ori = m_all_song_info;    //保存修改前的曲目列表
        for (auto& song : m_all_song_info)
        {
            SongInfo result;
            if (GetTagFromLyrics(song, song))
            {
                count++;
            }
            if (count > 0)
            {
                ShowInfo();
                m_modified = true;
                SetSaveBtnEnable();

                //判断哪些字段有修改
                CPropertyDlgHelper helper(m_all_song_info);
                if (helper.IsTitleModified(list_ori))
                    m_title_edit.SetModify();
                if (helper.IsArtistModified(list_ori))
                    m_artist_edit.SetModify();
                if (helper.IsAlbumModified(list_ori))
                    m_album_edit.SetModify();
            }
        }
    }
}


void CPropertyTabDlg::OnBnClickedGetTagFromFileNameButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CTagFromFileNameDlg dlg;
    dlg.HideOriginalBtn(true);
    if (dlg.DoModal() == IDOK)
    {
        CWaitCursor wait_cursor;
        wstring str_formular = dlg.GetFormularSelected();
        if (!m_batch_edit)
        {
            SongInfo song;
            wstring file_name = CFilePathHelper(m_all_song_info[m_index].file_path).GetFileNameWithoutExtension();
            CPropertyDlgHelper::GetTagFromFileName(file_name, str_formular, song);
            ModifyTagInfo(song);
        }
        else
        {
            vector<SongInfo> list_ori = m_all_song_info;    //保存修改前的曲目列表
            for (auto& song : m_all_song_info)
            {
                wstring file_name = CFilePathHelper(song.file_path).GetFileNameWithoutExtension();
                CPropertyDlgHelper::GetTagFromFileName(file_name, str_formular, song);
            }
            ShowInfo();
            m_modified = true;
            SetSaveBtnEnable();

            //判断哪些字段有修改
            CPropertyDlgHelper helper(m_all_song_info);
            if (helper.IsTitleModified(list_ori))
                m_title_edit.SetModify();
            if (helper.IsArtistModified(list_ori))
                m_artist_edit.SetModify();
            if (helper.IsAlbumModified(list_ori))
                m_album_edit.SetModify();
            if (helper.IsTrackModified(list_ori))
                m_track_edit.SetModify();
            if (helper.IsGenreModified(list_ori))
                m_genre_combo.SetModify();
            if (helper.IsYearModified(list_ori))
                m_year_edit.SetModify();
            if (helper.IsCommentModified(list_ori))
                m_comment_edit.SetModify();
        }
    }
}


void CPropertyTabDlg::ModifyTagInfo(const SongInfo& song)
{
    bool modified{ false };
    if (!song.title.empty() && m_title_edit.GetText() != song.title.c_str())
    {
        m_title_edit.SetWindowText(song.title.c_str());
        m_title_edit.SetModify();
        modified = true;
    }
    if (!song.artist.empty() && m_artist_edit.GetText() != song.artist.c_str())
    {
        m_artist_edit.SetWindowText(song.artist.c_str());
        m_artist_edit.SetModify();
        modified = true;
    }
    if (!song.album.empty() && m_album_edit.GetText() != song.album.c_str())
    {
        m_album_edit.SetWindowText(song.album.c_str());
        m_album_edit.SetModify();
        modified = true;
    }
    if (song.track != 0 && m_track_edit.GetText() != std::to_wstring(song.track).c_str())
    {
        m_track_edit.SetWindowText(std::to_wstring(song.track).c_str());
        m_track_edit.SetModify();
        modified = true;
    }
    if (!song.genre.empty() && m_genre_combo.GetText() != song.genre.c_str())
    {
        m_genre_combo.SetWindowText(song.genre.c_str());
        m_genre_combo.SetModify();
        modified = true;
    }
    if (!song.IsYearEmpty() && m_year_edit.GetText() != song.GetYear().c_str())
    {
        m_year_edit.SetWindowText(song.get_year().c_str());
        m_year_edit.SetModify();
        modified = true;
    }
    if (!song.comment.empty() && m_comment_edit.GetText() != song.comment.c_str())
    {
        m_comment_edit.SetWindowText(song.comment.c_str());
        m_comment_edit.SetModify();
        modified = true;
    }

    if (modified)
    {
        m_modified = true;
        SetSaveBtnEnable();
    }

}

bool CPropertyTabDlg::GetTagFromLyrics(SongInfo& song, SongInfo& result)
{
    //从歌词获取标签信息前，如果还未获取过歌词，从在这里获取一次歌词
    if (!song.is_cue && song.lyric_file.empty())
    {
        CMusicPlayerCmdHelper helper;
        wstring lyric_path = helper.SearchLyricFile(song, theApp.m_lyric_setting_data.lyric_fuzzy_match);
        song.lyric_file = lyric_path;
        if (!lyric_path.empty())
        {
            //获取到歌词后同时更新song data
            CSongDataManager::GetInstance().GetSongInfoRef(song.file_path).lyric_file = lyric_path;
            m_lyric_file_edit.SetWindowText(lyric_path.c_str());
        }
    }

    if (!song.lyric_file.empty())
    {
        CLyrics lyrics{ song.lyric_file };
        if (!lyrics.GetTitle().empty())
            result.title = lyrics.GetTitle();
        if (!lyrics.GetAritst().empty())
            result.artist = lyrics.GetAritst();
        if (!lyrics.GetAlbum().empty())
            result.album = lyrics.GetAlbum();
        return true;
    }
    return false;
}
