#pragma once
#include "SimplePropertiesDlg.h"
#include "ListCtrlEx.h"
#include "PlaylistMgr.h"

// CPlaylistPropertiesDlg 对话框

class CPlaylistPropertiesDlg : public CSimplePropertiesDlg
{
    DECLARE_DYNAMIC(CPlaylistPropertiesDlg)

public:
    CPlaylistPropertiesDlg(const PlaylistInfo& playlist_info, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CPlaylistPropertiesDlg();

private:
    PlaylistInfo m_playlist_info;

protected:
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

protected:
    virtual void InitData() override;
};
