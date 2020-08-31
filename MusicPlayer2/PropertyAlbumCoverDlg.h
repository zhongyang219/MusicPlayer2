#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "IPropertyTabDlg.h"


// CPropertyAlbumCoverDlg 对话框

class CPropertyAlbumCoverDlg : public CTabDlg, public IPropertyTabDlg
{
	DECLARE_DYNAMIC(CPropertyAlbumCoverDlg)

public:
	CPropertyAlbumCoverDlg(vector<SongInfo>& all_song_info, int& index, bool show_out_album_cover = false, bool read_only = false, CWnd* pParent = nullptr);   // 标准构造函数
	CPropertyAlbumCoverDlg(vector<SongInfo>& all_song_info, CWnd* pParent = nullptr);   //批量编辑
	virtual ~CPropertyAlbumCoverDlg();

    virtual void PagePrevious() override;
    virtual void PageNext() override;
    virtual int SaveModified() override;
    void AdjustColumnWidth();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_ALBUM_COVER_DIALOG };
#endif

protected:
    enum RowIndex
    {
        RI_FILE_PATH,
        RI_COVER_PATH,
        RI_FORMAT,
        RI_WIDTH,
        RI_HEIGHT,
        RI_BPP,
        RI_SIZE,
        RI_COMPRESSED,
        RI_MAX
    };

    CListCtrlEx m_list_ctrl;

    int& m_index;		//当前显示项目的曲目序号
    vector<SongInfo>& m_all_song_info;
    bool m_read_only{};
    bool m_show_out_album_cover{};
    const bool m_batch_edit;
    int m_no_use{};

    bool m_write_enable{};
    bool m_cover_changed{};     //是否手动浏览了一张图片作为专辑封面
    bool m_cover_deleted{};     //是否删除了专辑封面

    CImage m_cover_img;         //内嵌专辑封面
    CImage m_cover_out_img;     //外部专辑封面
    wstring m_out_img_path;     //外部专辑图片的路径

protected:
    void ShowInfo();
    const SongInfo& CurrentSong();      //当前查看的歌曲信息
    CImage& GetCoverImage();            //当前显示的专辑封面图片
    bool IsCurrentSong();               //当前查看的歌曲是否为正在播放的歌曲
    bool IsShowOutAlbumCover();         //显示的专辑封面图片是否为外部图片
    bool IsDeleteEnable();              //删除功能是否可用
    bool HasAlbumCover();
    void SetWreteEnable();
    void EnableControls();
    void SetSaveBtnEnable();        //设置父窗口中的“保存到文件”按钮的可用或禁用状态

    //写入专辑封面，如果delete_file为true则写入后删除图片文件。返回写入文件的个数
    int SaveAlbumCover(const wstring& album_cover_path, bool delete_file = false);
    int SaveEnbedLinkedCoverForBatchEdit();             //批量编辑模式下将关联的专辑封面嵌入到音频文件

    void DeleteLinkedPic(const wstring& file_path, const wstring& album_cover_path);

    virtual void OnTabEntered() override;      //当标签切换到当前窗口时被调用
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnPaint();
    afx_msg void OnBnClickedSaveAlbumCoverButton();
    afx_msg void OnBnClickedDeleteButton();
    afx_msg void OnBnClickedBrowseButton();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnCoverBrowse();
    afx_msg void OnCoverDelete();
    afx_msg void OnCoverSaveAs();
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnBnClickedShowOutAlbumCoverChk();
};
