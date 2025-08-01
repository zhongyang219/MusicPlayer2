#pragma once
#include "CoverDownloadDlg.h"
#include "LyricDownloadCommon.h"
class CGetTagOnlineDlg : public CCoverDownloadDlg
{
public:
    CGetTagOnlineDlg(const SongInfo& song, CWnd* pParent = nullptr);
    virtual ~CGetTagOnlineDlg();
    const  CLyricDownloadCommon::ItemInfo& GetSelectedItem() const { return m_item_info; }

private:
    CLyricDownloadCommon::ItemInfo m_item_info;
    const SongInfo& m_song_info;

private:
    virtual CString GetDialogName() const override;
    virtual void OnBnClickedDownloadSelected() override;
    virtual SongInfo GetSongInfo() const override;

public:
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnInitMenu(CMenu* pMenu);
};

