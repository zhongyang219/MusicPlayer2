#pragma once
#include "UserUi.h"

class CMiniModeUserUi : public CUserUi
{
public:
    CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path, UIData& ui_data);
    CMiniModeUserUi(CWnd* pMainWnd, UINT id, UIData& ui_data);   // id为xml资源ID
    ~CMiniModeUserUi();

    bool GetUiSize(int& width, int& height, int& height_with_playlist);
    std::shared_ptr<UiElement::Playlist> GetPlaylist() const;

private:
    void InitUiPlaylist();

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual void PreDrawInfo() override;

    virtual bool ButtonClicked(BtnKey btn_type, const UIButton& btn) override;
    virtual bool ButtonRClicked(BtnKey btn_type, const UIButton& btn) override;

    bool IsShowUiPlaylist() const;

private:
    std::shared_ptr<UiElement::Element> m_ui_element;       //迷你模式中的“ui”节点
    std::shared_ptr<UiElement::Element> m_playlist_emelment;   //ui中的播放列表
};

