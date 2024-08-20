#pragma once


// CFontDialogEx

class CFontDialogEx : public CFontDialog
{
	DECLARE_DYNAMIC(CFontDialogEx)

public:
    CFontDialogEx(LPLOGFONT lplfInitial = nullptr,
        bool show_color = true,     //是否显示颜色
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = nullptr,
		CWnd* pParentWnd = nullptr);

    virtual ~CFontDialogEx();

private:
    bool m_show_color;

protected:
	DECLARE_MESSAGE_MAP()

    void ShowDlgCtrl(int id, bool show);

public:
    virtual BOOL OnInitDialog();
};


