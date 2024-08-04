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

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

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
