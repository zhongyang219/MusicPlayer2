#pragma once
#include "Time.h"
#include "Common.h"
#include "Player.h"
#include "EditEx.h"
#include "afxwin.h"
#include "MyComboBox.h"
#include "FilePathHelper.h"
#include "TabDlg.h"
#include "IPropertyTabDlg.h"

// CPropertyTabDlg 对话框

class CPropertyTabDlg : public CTabDlg, public IPropertyTabDlg
{
	DECLARE_DYNAMIC(CPropertyTabDlg)

public:
	CPropertyTabDlg(vector<SongInfo>& all_song_info, int& index, CWnd* pParent = NULL, bool read_only = false);   // 标准构造函数
	CPropertyTabDlg(vector<SongInfo>& song_info, CWnd* pParent = NULL);   // 批量编辑
	virtual ~CPropertyTabDlg();

	//int m_playing_index{};	//正在播放的曲目序号
	//wstring m_lyric_name;

	bool GetListRefresh() const { return m_list_refresh; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_DIALOG };
#endif

protected:
	//const vector<wstring>& m_all_file_name;
	int& m_index;		//当前显示项目的曲目序号
	vector<SongInfo>& m_all_song_info;
	bool m_write_enable{ false };		//是否允许写入标签
	bool m_modified{ false };
	bool m_genre_modified{ false };
	bool m_list_refresh{ false };		//用于判断窗口关闭后播放列表是否要刷新
    int m_song_num{};
    bool m_read_only{};

    const bool m_batch_edit;
    int m_no_use{};

	//控件变量
	CEditEx m_file_name_edit;
	CEditEx m_file_path_edit;
	CEditEx m_file_type_edit;
	CEditEx m_song_length_edit;
	CEditEx m_file_size_edit;
	CEditEx m_bit_rate_edit;
	CEditEx m_title_edit;
	CEditEx m_artist_edit;
	CEditEx m_album_edit;
	CEditEx m_track_edit;
	CEditEx m_year_edit;
	//CEditEx m_genre_edit;
	CMyComboBox m_genre_combo;
	CEditEx m_comment_edit;
	CEditEx m_lyric_file_edit;

	void ShowInfo();
	void SetEditReadOnly(bool read_only);
	void SetWreteEnable();
    void SetSaveBtnEnable();
    void ResetEditModified();

    virtual void OnTabEntered() override;      //当标签切换到当前窗口时被调用
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void ModifyTagInfo(const SongInfo& song);
    bool GetTagFromLyrics(SongInfo& song, SongInfo& result);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    virtual void PagePrevious() override;
    virtual void PageNext() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeTitelEdit();
	afx_msg void OnEnChangeArtistEdit();
	afx_msg void OnEnChangeAlbumEdit();
	afx_msg void OnEnChangeTrackEdit();
	afx_msg void OnEnChangeYearEdit();
	afx_msg void OnEnChangeCommentEdit();
	//afx_msg void OnCbnEditchangeGenreCombo();
	virtual int SaveModified() override;
	afx_msg void OnCbnSelchangeGenreCombo();
	//afx_msg void OnBnClickedButton3();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnCbnEditchangeGenreCombo();
    afx_msg void OnBnClickedGetTagOnlineButton();
protected:
    afx_msg LRESULT OnPorpertyOnlineInfoAcquired(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedGetTagFromLyricButton();
    afx_msg void OnBnClickedGetTagFromFileNameButton();
};
