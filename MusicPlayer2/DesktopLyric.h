#pragma once
#include "LyricsWindow.h"
#include "CommonData.h"

struct LyricStyle
{
    COLORREF color1 = 0;
    COLORREF color2 = 0;
    int gradient_mode = 0;
};

struct LyricStyleDefaultData        //桌面歌词预设数据
{
    LyricStyle normal_style;
    LyricStyle highlight_style;
};
const int LYRIC_DEFAULT_STYLE_NUM = 3;

class CDesktopLyric : public CLyricsWindow
{
public:
    struct UIButton		//界面中绘制的按钮
    {
        CRect rect;				//按钮的矩形区域
        bool hover{ false };	//鼠标是否指向按钮
        bool pressed{ false };	//按钮是否按下
        bool enable{ true };	//按钮是否启用
    };

    enum BtnKey		//标识按钮的类型
    {
        BTN_APP,
        BTN_STOP,				//停止
        BTN_PREVIOUS,			//上一曲
        BTN_PLAY_PAUSE,			//播放/暂停
        BTN_NEXT,				//下一曲
        BTN_LOCK,
        BTN_DOUBLE_LINE,
        BTN_BACKGROUND_PENETRATE,
        BTN_SETTING,
        BTN_DEFAULT_STYLE,
        BTN_CLOSE,
        MAX             //用于指示枚举的最大个数
    };

public:
	CDesktopLyric();
	~CDesktopLyric();

	void Create();
	void ShowLyric();
	void ClearLyric();
	void ApplySettings(const DesktopLyricSettingData& data);
	void SetLyricWindowVisible(bool visible);
	void SetLyricWindowLock(bool locked);
    void SetLyricOpacity(int opacity);
    void SetLyricBackgroundPenetrate(bool penetrate);
    LyricStyleDefaultData GetDefaultStyle(int index);

    static void LyricStyleDefaultDataToLyricSettingData(const LyricStyleDefaultData& style_data, DesktopLyricSettingData& setting_data);

    //绘制工具条
    void DrawToolbar(Gdiplus::Graphics* pGraphics);
    //绘制工具条上的图标
    void DrawToolIcon(Gdiplus::Graphics* pGraphics, IconRes icon, CRect rect, BtnKey btn, bool checked = false);

    //添加鼠标提示
    void AddToolTips();
    //为一个按钮添加鼠标提示
    void AddMouseToolTip(BtnKey btn, LPCTSTR str);
    void UpdateMouseToolTip(BtnKey btn, LPCTSTR str);
    void UpdateToolTipPosition();

protected:
    virtual void PreDrawLyric(Gdiplus::Graphics* pGraphics) override;
    virtual void AfterDrawLyric(Gdiplus::Graphics* pGraphics) override;

private:
    void SetWindowStyle();

private:
	//CLyricsWindow m_lyric_window;
    LyricStyleDefaultData m_default_style[LYRIC_DEFAULT_STYLE_NUM];

    CMenu m_popupMenu;
    CToolTipCtrl m_tool_tip;
    std::map<BtnKey, UIButton> m_buttons;
    bool m_first_draw = true;       //第一次绘制工具条时，则为true
    bool m_bHover = false;                  //鼠标是否在指向窗口
    bool m_bMouseInWindowRect = false;      //鼠标是否在当前窗口区域内
    bool m_bMenuPopedUp = false;
    bool m_bLocked = false;     	//是否锁定歌词窗口
    bool m_lyricBackgroundPenetrate = false;

public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
protected:
    afx_msg LRESULT OnInitmenu(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()
    afx_msg void OnLyricDefaultStyle1();
    afx_msg void OnLyricDefaultStyle2();
    afx_msg void OnLyricDefaultStyle3();
};

