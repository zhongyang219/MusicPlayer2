#pragma once
#include "LyricDownloadCommon.h"
#include "afxcmn.h"
#include "Lyric.h"
#include "afxwin.h"
#include "ListCtrlEx.h"
#include "SongInfo.h"
#include "BaseDialog.h"


// CLyricDownloadDlg 对话框

class CLyricDownloadDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CLyricDownloadDlg)

public:
	CLyricDownloadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLyricDownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LYRIC_DOWNLOAD_DIALOG };
#endif

#define WM_SEARCH_COMPLATE (WM_USER+101)		//歌曲搜索完成消息
#define WM_DOWNLOAD_COMPLATE (WM_USER+102)		//歌词下载完成消息

	//用于向歌词搜索线程传递数据的结构体
	struct SearchThreadInfo
	{
		wstring url;
		wstring result;
		int rtn;
		HWND hwnd;
		//bool exit;
	};
	//歌词搜索线程函数
	static UINT LyricSearchThreadFunc(LPVOID lpParam);

	//用于向歌词下载线程传递数据的结构体
	struct DownloadThreadInfo
	{
		wstring song_id;
		wstring result;
		bool success;
		bool download_translate;
		bool save_as;		//下载完成后是弹出“另存为”对话框还是直接保存
		HWND hwnd;
		//bool exit;
	};
	//歌词下载线程函数
	static UINT LyricDownloadThreadFunc(LPVOID lpParam);

	SearchThreadInfo m_search_thread_info;
	DownloadThreadInfo m_download_thread_info;

protected:
    SongInfo m_song;       //要查找歌词的歌曲
	wstring m_lyric_name;	        //保存的歌词文件的文件名（不含扩展名）
	wstring m_lyric_dir;			//当前要保存的歌词文件的目录
	wstring m_search_result;	//查找结果字符串
	wstring m_lyric_str;	//下载的歌词
	vector<CInternetCommon::ItemInfo> m_down_list;	//搜索结果的列表

	int m_item_selected{ -1 };		//搜索结果列表中选中的项目
	bool m_download_translate{ false };
	bool m_save_to_song_folder{ true };		//是否保存到歌曲所在目录
	CodeType m_save_code{};		//保存的编码格式
	int m_search_max_item{ 30 };		//查找歌曲时返回的最大数量

	CMenu m_menu;
	CListCtrlEx m_down_list_ctrl;
	CButton m_download_translate_chk;
	CComboBox m_save_code_combo;
	//CToolTipCtrl m_tool_tip;		//鼠标指向时的工具提示
	CLinkCtrl m_unassciate_lnk;

	CWinThread* m_pSearchThread;		//搜索歌词的线程
	CWinThread* m_pDownThread;			//下载歌词的线程

	void ShowDownloadList();		//将搜索结果显示出来
	bool SaveLyric(const wchar_t* path, CodeType code_type);	//保存歌词

	void SaveConfig() const;
	void LoadConfig();

    wstring GetSavedDir();
    wstring GetSavedPath();

    virtual CString GetDialogName() const override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	bool IsItemSelectedValid() const;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSearchButton2();
	afx_msg void OnEnChangeTitleEdit1();
	afx_msg void OnEnChangeArtistEdit1();
	afx_msg void OnNMClickLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDownloadSelected();
	afx_msg void OnBnClickedDownloadTranslateCheck1();
	afx_msg void OnDestroy();
protected:
	afx_msg LRESULT OnSearchComplate(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg LRESULT OnDownloadComplate(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedSaveToSongFolder1();
	afx_msg void OnBnClickedSaveToLyricFolder1();
	afx_msg void OnBnClickedSelectedSaveAs();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnLdLyricDownload();
	afx_msg void OnLdLyricSaveas();
	afx_msg void OnLdCopyTitle();
	afx_msg void OnLdCopyArtist();
	afx_msg void OnLdCopyAlbum();
	afx_msg void OnLdCopyId();
	afx_msg void OnLdViewOnline();
	afx_msg void OnNMDblclkLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClickUnassociateLink(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLdPreview();
    afx_msg void OnLdRelate();
};
