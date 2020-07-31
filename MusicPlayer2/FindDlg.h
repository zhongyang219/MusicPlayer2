#pragma once
#include "afxcmn.h"
#include "AudioCommon.h"
#include "afxwin.h"
#include "ListCtrlEx.h"
#include "Common.h"
#include "BaseDialog.h"


// CFindDlg 对话框

class CFindDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CFindDlg)

public:
	CFindDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindDlg();

	//wstring m_config_path;

	//int GetSelectedTrack() const;
	bool IsFindCurrentPlaylist() const;
	void GetSongsSelected(vector<wstring>& files) const;
	void GetSongsSelected(vector<SongInfo>& files) const;
	void SaveConfig();
	void LoadConfig();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIND_DIALOG };
#endif

protected:
    enum ColumeIndex
    {
        COL_INDEX = 0,
        COL_FILE_NAME,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_PATH,
    };

	//const vector<SongInfo>& m_playlist;		//播放列表
	//vector<int> m_find_result;			//储存当前播放列表的查找结果（曲目序号）
	wstring m_key_word;				//查找的字符串
	vector<SongInfo> m_find_result;		//储存所有播放列表的查找结果

	CListCtrlEx m_find_result_list;		//查找结果控件
	int m_item_selected{ -1 };		//鼠标选中的项目序号
    vector<int> m_items_selected;
	//CMenu m_menu;

	//int m_dpi;

	CButton m_find_file_check;
	CButton m_find_title_check;
	CButton m_find_artist_check;
	CButton m_find_album_check;

	bool m_find_current_playlist{ true };		//如果查找范围为当前播放列表，则为true，如果是所有播放列表，则为false
    bool m_result_in_current_playlist{ true };  //查找结果是否为当前播放列表
	bool m_find_file{ true };
	bool m_find_title{ true };
	bool m_find_artist{ true };
	bool m_find_album{ true };

	int m_find_option_data{};		//保存查找选项的数据，用每一个bit位表示每个查找选项是否选中

	CString m_selected_string;

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ShowFindResult();
	void ShowFindInfo();
    bool _OnAddToNewPlaylist(std::wstring& playlist_path);       //执行添加到新建播放列表命令，成功返回true，playlist_path用于接收新播放列表的路径
    static UINT ViewOnlineThreadFunc(LPVOID lpParam);	//执行在线查看的线程函数
    void GetCurrentSongList(std::vector<SongInfo>& song_list);

public: 
	void ClearFindResult();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnEnChangeFindEdit();
	afx_msg void OnNMClickFindList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFindButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkFindList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedFindFileCheck();
	afx_msg void OnBnClickedFindTitleCheck();
	afx_msg void OnBnClickedFindArtistCheck();
	afx_msg void OnBnClickedFindAlbumCheck();
	afx_msg void OnBnClickedFindCurrentPlaylistRadio();
	afx_msg void OnBnClickedFindAllPlaylistRadio();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPlayItem();
	afx_msg void OnExploreTrack();
	afx_msg void OnNMRClickFindList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCopyText();
    afx_msg void OnPlayItemInFolderMode();
    afx_msg void OnAddToNewPlaylist();
    afx_msg void OnExploreOnline();
    afx_msg void OnFormatConvert();
    afx_msg void OnItemProperty();
    virtual void OnOK();
    afx_msg void OnAddToNewPalylistAndPlay();
private:
public:
    afx_msg void OnInitMenu(CMenu* pMenu);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDeleteFromDisk();
};
