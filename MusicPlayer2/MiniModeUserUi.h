#pragma once
#include "UserUi.h"

class CMiniModeUserUi : public CUserUi
{
public:
    CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path);
    CMiniModeUserUi(CWnd* pMainWnd, UINT id);   // id为xml资源ID
    ~CMiniModeUserUi();

    bool GetUiSize(int& width, int& height, int& height_with_playlist);
    std::shared_ptr<UiElement::Playlist> GetPlaylist() const;

private:
    void InitUiPlaylist();

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual void PreDrawInfo() override;
    virtual bool LButtonUp(CPoint point) override;
    virtual bool IsDrawLargeIcon() const override { return false; }
    virtual bool IsDrawStatusBar() const override { return false; }
    virtual bool IsDrawTitleBar() const override { return false; }
    virtual bool IsDrawMenuBar() const override { return false; }
    virtual bool PointInControlArea(CPoint point) const override;
    virtual const std::vector<std::shared_ptr<UiElement::Element>>& GetStackElements() const;

    bool IsShowUiPlaylist() const;

private:
    std::shared_ptr<UiElement::Element> m_ui_element;       //迷你模式中的“ui”节点
    std::shared_ptr<UiElement::Element> m_playlist_emelment;   //ui中的播放列表
};

