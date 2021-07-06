#pragma once
#include "ListCtrlEx.h"
#include "BASSEncodeLibrary.h"
#include "AudioTag.h"
#include "MP3EncodeCfgDlg.h"
#include "OggEncodeCfgDlg.h"
#include "BASSWmaLibrary.h"
#include "WmaEncodeCfgDlg.h"
#include "TagEditDlg.h"
#include "FolderBrowserDlg.h"
#include "PlayerProgressBar.h"
#include "BassMixLibrary.h"
#include <map>
#include "BrowseEdit.h"
#include "MusicPlayer2.h"
#include "BaseDialog.h"

//格式转换错误代码
#define CONVERT_ERROR_FILE_CONNOT_OPEN (-1)			//源文件无法读取
#define CONVERT_ERROR_ENCODE_CHANNEL_FAILED (-2)	//编码通道创建失败
#define CONVERT_ERROR_ENCODE_PARA_ERROR (-3)		//找不到编码器或编码器参数错误
#define CONVERT_ERROR_MIDI_NO_SF2 (-4)				//没有MIDI音色库
#define CONVERT_ERROR_WMA_NO_WMP9_OR_LATER (-5)     //没有安装 Windows Media Player 9 或更高版本。
#define CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER (-6) //无法找到可支持请求的采样格式和比特率的编解码器。

//单个文件转换进度的消息
//wParam: 文件在m_file_list列表中的索引
//lParam: 转换的进度。0~100: 已完成的百分比; 101: 已完成; 102: 已跳过; <0:出错
#define WM_CONVERT_PROGRESS (WM_USER+111)
//全部文件转换已完成的消息
#define WM_CONVERT_COMPLETE (WM_USER+112)

#define ALBUM_COVER_NAME_ENCODE L"TempAlbumCover-MusicPlayer2-CkV8E0YI"

// CFormatConvertDlg 对话框

class CFormatConvertDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CFormatConvertDlg)

public:
	enum class EncodeFormat { WAV, MP3, WMA, OGG };

	CFormatConvertDlg(CWnd* pParent = nullptr);   // 标准构造函数
	CFormatConvertDlg(const vector<SongInfo>& items, CWnd* pParent = nullptr);
	virtual ~CFormatConvertDlg();

	//工作线程函数
	static UINT ThreadFunc(LPVOID lpParam);

	bool IsTaskbarListEnable() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMAT_CONVERT_DIALOG };
#endif

protected:
	//控件变量
	CListCtrlEx m_file_list_ctrl;
	CComboBox m_encode_format_combo;
    CPlayerProgressBar m_progress_bar;
    CMenu m_list_popup_menu;
    CComboBox m_freq_comb;
    CBrowseEdit m_out_dir_edit;
    CBrowseEdit m_out_name_edit;

//#ifndef COMPILE_IN_WIN_XP
	ITaskbarList3* m_pTaskbar{ theApp.GetITaskbarList3() };          //用于支持任务栏显示播放进度
//#endif

    vector<SongInfo> m_file_list;   // 要转换格式的文件列表
    wstring m_out_dir;              // 输出目录
    wstring m_out_name;             // 输出文件名格式字符串
	EncodeFormat m_encode_format{ EncodeFormat::MP3 };
	CWinThread* m_pThread{};		//格式转换的线程
	wstring m_encode_dir;

	bool m_encoder_succeed;
	bool m_thread_runing{};

	MP3EncodePara m_mp3_encode_para;		//MP3编码参数
	int m_ogg_encode_quality{ 3 };				//OGG编码质量
	WmaEncodePara m_wma_encode_para;			//wma编码参数

	static CBASSEncodeLibrary m_bass_encode_lib;
	static CBASSWmaLibrary m_bass_wma_lib;
    static CBassMixLibrary m_bass_mix_lib;

	int m_item_selected;

	//选项数据
	bool m_write_tag;			//向目标文件写入标签信息
	bool m_write_album_cover;	//向目标文件写入专辑封面
	int m_file_exist_action;	//当目标文件存在时的动作（0: 自动重命名; 1: 忽略; 2: 覆盖）
	bool m_add_file_serial_num;	//为目标文件添加数字编号
    bool m_convert_freq{ false };   //是否转换采样频率
    wstring m_freq_sel{};       //采样频率下拉列表中的选中项
	bool m_open_output_dir{ false };	//转换完成后打开输出目录

    std::map<wstring, int> m_freq_map;

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void LoadConfig();
	void SaveConfig() const;
	void LoadEncoderConfig();
	void SaveEncoderConfig() const;

	void EnableControls(bool enable);
	void SetEncodeConfigBtnState();
	void ShowFileList();
	bool InitEncoder();
	//编码单个文件（在线程函数中调用）
	//pthis: 当前对话框的指针
	//file_index: 编码的文件在m_file_list中的索引
	static bool EncodeSingleFile(CFormatConvertDlg* pthis, int file_index);

    void SetProgressInfo(int progress);
    int GetFreq();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeOutFormatCombo();
	afx_msg void OnBnClickedStartConvertButton();
	//afx_msg void OnBnClickedBrowseButton();
protected:
	afx_msg LRESULT OnConvertProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConvertComplete(WPARAM wParam, LPARAM lParam);
public:
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnBnClickedEncoderConfigButton();
	afx_msg void OnBnClickedCopyTagCheck();
	afx_msg void OnBnClickedCopyAlbumCoverCheck();
	afx_msg void OnCbnSelchangeTargetFileExistCombo();
	afx_msg void OnBnClickedAddNumberCheck();
	afx_msg void OnNMRClickSongList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddFile();
	afx_msg void OnDeleteSelect();
	afx_msg void OnClearList();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEditTagInfo();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnNMDblclkSongList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedChangeFreqCheck();
    afx_msg void OnCbnSelchangeFreqCombo();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOpenTargetDirCheck();
};
