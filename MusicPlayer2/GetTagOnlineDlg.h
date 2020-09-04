#pragma once
#include "CoverDownloadDlg.h"
#include "InternetCommon.h"
class CGetTagOnlineDlg : public CCoverDownloadDlg
{
public:
    CGetTagOnlineDlg(const SongInfo& song, CWnd* pParent = nullptr);
    virtual ~CGetTagOnlineDlg();
    const  CInternetCommon::ItemInfo& GetSelectedItem() const { return m_item_info; }

private:
    CInternetCommon::ItemInfo m_item_info;
    const SongInfo& m_song_info;

private:
    virtual CString GetDialogName() const override;
    virtual void OnBnClickedDownloadSelected() override;
    virtual SongInfo GetSongInfo() const override;

public:
    virtual BOOL OnInitDialog();
};

