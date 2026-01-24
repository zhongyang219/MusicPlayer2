#include "stdafx.h"
#include "PlayerFormulaHelper.h"
#include "Player.h"
#include "UIElement/PlaylistIndicator.h"
#include "SongInfoHelper.h"
#include "CPlayerUIBase.h"
#include "CPlayerUIHelper.h"
#include "MusicPlayerDlg.h"

const std::map<std::wstring, PlayerVariable> VariableNameMap{
    { L"FileName", PlayerVariable::FileName },
    { L"FilePath" , PlayerVariable::FilePath },
    { L"Title" , PlayerVariable::Title },
    { L"Artist" , PlayerVariable::Artist },
    { L"Album" , PlayerVariable::Album },
    { L"AlbumArtist" , PlayerVariable::AlbumArtist },
    { L"ArtistAndAlbum" , PlayerVariable::ArtistAndAlbum },
    { L"TrackNum" , PlayerVariable::TrackNum },
    { L"Year" , PlayerVariable::Year },
    { L"Genre" , PlayerVariable::Genre },
    { L"Comment" , PlayerVariable::Comment },
    { L"FileType" , PlayerVariable::FileType },
    //{ L"FileSize" , PlayerVariable::FileSize },
    { L"BitRate" , PlayerVariable::BitRate },
    { L"Channels" , PlayerVariable::Channels },
    { L"SampleFreq" , PlayerVariable::SampleFreq },
    { L"BitDepth" , PlayerVariable::BitDepth },
    { L"LyricsFile" , PlayerVariable::LyricsFile },
    { L"Format" , PlayerVariable::Format },
    { L"MidiSoundFont" , PlayerVariable::MidiSoundFont },
    { L"MidiSpeed" , PlayerVariable::MidiSpeed },
    { L"MidiPosition" , PlayerVariable::MidiPosition },
    { L"MidiLength" , PlayerVariable::MidiLength },
    { L"PlayingStatus" , PlayerVariable::PlayingStatus },
    { L"PlayModeName" , PlayerVariable::PlayModeName },
    { L"PlayListName" , PlayerVariable::PlayListName },
    { L"CurrentPosition" , PlayerVariable::CurrentPosition },
    { L"SongLength" , PlayerVariable::SongLength },
    { L"CurrentLyric" , PlayerVariable::CurrentLyric },
    { L"RepeatMode" , PlayerVariable::RepeatMode },
    { L"Volume" , PlayerVariable::Volume },
    { L"FPS" , PlayerVariable::FPS },
    { L"UiRefreshInterval" , PlayerVariable::UiRefreshInterval },
    { L"PlayQueueNum" , PlayerVariable::PlayQueueNum },
    { L"PlayQueueIndex", PlayerVariable::PlayQueueIndex },
};

std::wstring CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable variable_type)
{
    switch (variable_type)
    {
    case PlayerVariable::None:
        return std::wstring();
    case PlayerVariable::FileName:
        return CPlayer::GetInstance().GetDisplayName();
    case PlayerVariable::FilePath:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().file_path;
    case PlayerVariable::Title:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().GetTitle();
    case PlayerVariable::Artist:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist();
    case PlayerVariable::Album:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().GetAlbum();
    case PlayerVariable::AlbumArtist:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().album_artist;
    case PlayerVariable::ArtistAndAlbum:
    {
        //优先使用唱片集艺术家，如果为空，则使用艺术家
        std::wstring artist_display{ CPlayer::GetInstance().GetSafeCurrentSongInfo().album_artist };
        if (artist_display.empty())
            artist_display = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist();
        return artist_display + L" - " + CPlayer::GetInstance().GetSafeCurrentSongInfo().GetAlbum();
    }
    case PlayerVariable::TrackNum:
        return std::to_wstring(CPlayer::GetInstance().GetSafeCurrentSongInfo().track);
    case PlayerVariable::Year:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().GetYear();
    case PlayerVariable::Genre:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().GetGenre();
    case PlayerVariable::Comment:
        return CPlayer::GetInstance().GetSafeCurrentSongInfo().comment;
    case PlayerVariable::FileType:
        return CPlayer::GetInstance().GetCurrentFileType();
    //case PlayerVariable::FileSize:
    //    break;
    case PlayerVariable::BitRate:
        return std::to_wstring(CPlayer::GetInstance().GetSafeCurrentSongInfo().bitrate);
    case PlayerVariable::Channels:
    {
        int chans = CPlayer::GetInstance().GetChannels();
        return CSongInfoHelper::GetChannelsString(static_cast<BYTE>(chans));
    }
    case PlayerVariable::SampleFreq:
    {
        int freq = CPlayer::GetInstance().GetFreq();
        wchar_t buff[64]{};
        swprintf_s(buff, L"%.1f", freq / 1000.0f);
        return std::wstring(buff);
    }
    case PlayerVariable::BitDepth:
    {
        return CSongInfoHelper::GetBitsString(CPlayer::GetInstance().GetSafeCurrentSongInfo()).GetString();
    }
    case PlayerVariable::LyricsFile:
    {
        if (CPlayer::GetInstance().IsInnerLyric())
            return theApp.m_str_table.LoadText(L"TXT_INNER_LYRIC");
        else if (!CPlayer::GetInstance().GetSafeCurrentSongInfo().lyric_file.empty())
            return CPlayer::GetInstance().GetSafeCurrentSongInfo().lyric_file;
        else
            return theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_FILE_LRC_FILE_NO_LINK");
    }
    case PlayerVariable::Format:
        return CPlayerUIBase::GetDisplayFormatString();
    case PlayerVariable::MidiSoundFont:
        return CPlayer::GetInstance().GetPlayerCore()->GetSoundFontName();
    case PlayerVariable::MidiSpeed:
    {
        const MidiInfo& midi_info{ CPlayer::GetInstance().GetMidiInfo() };
        return std::to_wstring(midi_info.speed);
    }
    case PlayerVariable::MidiPosition:
    {
        const MidiInfo& midi_info{ CPlayer::GetInstance().GetMidiInfo() };
        return std::to_wstring(midi_info.midi_position);
    }
    case PlayerVariable::MidiLength:
    {
        const MidiInfo& midi_info{ CPlayer::GetInstance().GetMidiInfo() };
        return std::to_wstring(midi_info.midi_length);
    }
    case PlayerVariable::PlayingStatus:
        return CPlayer::GetInstance().GetPlayingState();
    case PlayerVariable::PlayModeName:
    {
        UiElement::PlaylistIndicator::m_list_cache.reload();
        return UiElement::PlaylistIndicator::m_list_cache.at(0).GetTypeDisplayName();
    }
    case PlayerVariable::PlayListName:
    {
        UiElement::PlaylistIndicator::m_list_cache.reload();
        return UiElement::PlaylistIndicator::m_list_cache.at(0).GetDisplayName();
    }
    case PlayerVariable::CurrentPosition:
    {
        CPlayTime current_position(CPlayer::GetInstance().GetCurrentPosition());
        wchar_t buff[64];
        swprintf_s(buff, L"%d:%.2d", current_position.min, current_position.sec);
        return wstring(buff);
    }
    case PlayerVariable::SongLength:
    {
        CPlayTime song_length(CPlayer::GetInstance().GetSongLength());
        wchar_t buff[64];
        swprintf_s(buff, L"%d:%.2d", song_length.min, song_length.sec);
        return wstring(buff);
    }
    case PlayerVariable::CurrentLyric:
    {
        if (CPlayerUIHelper::IsMidiLyric())
        {
            return CPlayer::GetInstance().GetMidiLyric();
        }
        else
        {
            auto& now_lyrics{ CPlayer::GetInstance().m_Lyrics };
            CPlayTime time{ CPlayer::GetInstance().GetCurrentPosition() };
            const bool karaoke{ theApp.m_lyric_setting_data.lyric_karaoke_disp };
            const bool ignore_blank{ theApp.m_lyric_setting_data.donot_show_blank_lines };
            CLyrics::Lyric current_lyric{ now_lyrics.GetLyric(time, false, ignore_blank, karaoke) };
            return current_lyric.text;
        }
    }
        break;
    case PlayerVariable::RepeatMode:
    {
        std::wstring mode_str;
        auto repeat_mode = CPlayer::GetInstance().GetRepeatMode();
        switch (repeat_mode)
        {
        case RM_PLAY_ORDER: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ORDER"); break;
        case RM_PLAY_SHUFFLE:  mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_SHUFFLE"); break;
        case RM_PLAY_RANDOM: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_RANDOM"); break;
        case RM_LOOP_PLAYLIST: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_PLAYLIST"); break;
        case RM_LOOP_TRACK: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_TRACK"); break;
        case RM_PLAY_TRACK: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ONCE"); break;
        default: mode_str = L"<repeat mode error>";
        }
        return mode_str;
    }
    case PlayerVariable::Volume:
        return std::to_wstring(CPlayer::GetInstance().GetVolume());
    case PlayerVariable::FPS:
        return std::to_wstring(theApp.m_fps);
    case PlayerVariable::UiRefreshInterval:
    {
        std::wstring ui_refresh_interval;
        CMusicPlayerDlg* dlg = CMusicPlayerDlg::GetInstance();
        if (dlg != nullptr)
            ui_refresh_interval = std::to_wstring(dlg->GetUiRefreshInterval());
        return ui_refresh_interval;
    }
    case PlayerVariable::PlayQueueNum:
        return std::to_wstring(CPlayer::GetInstance().GetSafeSongNum());
    case PlayerVariable::PlayQueueIndex:
        return std::to_wstring(CPlayer::GetInstance().GetIndex() + 1);
    default:
        break;
    }
    return std::wstring();
}

std::wstring CPlayerFormulaHelper::GetPlayerVariableValue(const std::wstring& str_variable)
{
    PlayerVariable variable_type{};
    auto iter = VariableNameMap.find(str_variable);
    if (iter != VariableNameMap.end())
        variable_type = iter->second;
    return GetPlayerVariableValue(variable_type);
}

bool CPlayerFormulaHelper::IsValidVariable(const std::wstring& str_variable)
{
    PlayerVariable variable_type{};
    auto iter = VariableNameMap.find(str_variable);
    if (iter != VariableNameMap.end())
        variable_type = iter->second;
    return variable_type != PlayerVariable::None;
}

void CPlayerFormulaHelper::ReplaceStringFormula(std::wstring& str)
{
    size_t index{};
    while ((index = str.find(L"%(", index)) != wstring::npos)
    {
        size_t right_bracket_index = str.find(L')', index + 2);
        if (right_bracket_index == wstring::npos)
            break;
        wstring key_str{ str.begin() + index + 2 , str.begin() + right_bracket_index };
        if (IsValidVariable(key_str))
        {
            wstring value_str = GetPlayerVariableValue(key_str);
            str.replace(index, right_bracket_index - index + 1, value_str);
        }
        index = right_bracket_index + 1;
    }
}
