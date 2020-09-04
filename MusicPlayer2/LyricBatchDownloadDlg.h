#pragma once
#include "afxwin.h"
#include "Common.h"
#include "afxcmn.h"
#include "LyricDownloadCommon.h"
#include "LyricDownloadDlg.h"
#include "ListCtrlEx.h"
#include "PlayerProgressBar.h"
#include "BaseDialog.h"

// CLyricBatchDownloadDlg 对话框

class CLyricBatchDownloadDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CLyricBatchDownloadDlg)

public:
	CLyricBatchDownloadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLyricBatchDownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LYRIC_BATCH_DOWN_DIALOG };
#endif

#define WM_BATCH_DOWNLOAD_COMPLATE (WM_USER+103)		//歌词批量下载完成消息

	//用于向多下载线程传递数据的结构体
	struct ThreadInfo
	{
		CListCtrl* list_ctrl;
		CStatic* static_ctrl;
        CPlayerProgressBar* progress_bar;
		const vector<SongInfo>* playlist;
		bool skip_exist;
		bool download_translate;
		bool save_to_song_folder;
		CodeType save_code;
		HWND hwnd;
		//bool exit;
	};

	//工作线程函数
	static UINT ThreadFunc(LPVOID lpParam);

	ThreadInfo m_thread_info;

	static bool SaveLyric(const wchar_t* path, const wstring& lyric_wcs, CodeType code_type, bool* char_cannot_convert);		//保存歌词，如果将歌词保存为ANSI格式时有无法转换的Unicode字符，则char_cannot_convert置为true

protected:
	CButton m_skip_exist_check;
	CComboBox m_save_code_combo;
	CListCtrlEx m_song_list_ctrl;
	CButton m_download_translate_chk;
	CStatic m_info_static;
    CPlayerProgressBar m_progress_bar;

	bool m_skip_exist{ true };		//跳过已有歌词的曲目
	CodeType m_save_code{};		//保存的编码格式
	bool m_download_translate{ false };		//是否下载歌词翻译
	bool m_save_to_song_folder{ true };		//是否保存到歌曲所在目录
	const vector<SongInfo>& m_playlist{ CPlayer::GetInstance().GetPlayList() };	//播放列表的引用

	bool m_lyric_path_not_exit{ false };

	CWinThread* m_pThread{};		//下载歌词的线程

    virtual CString GetDialogName() const override;
    
    void SaveConfig() const;
	void LoadConfig();

	void EnableControls(bool enable);		//启用或禁用控件

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedStartDownload();
	afx_msg void OnBnClickedSkipExistCheck();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedDownloadTrasnlateCheck2();
protected:
	afx_msg LRESULT OnBatchDownloadComplate(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnClickedSaveToSongFolder();
	afx_msg void OnBnClickedSaveToLyricFolder();
};
