#pragma once
#include "SongInfo.h"
#include "FormatConvertDlg.h"
class CMusicPlayerCmdHelper
{
public:
    CMusicPlayerCmdHelper(CWnd* pOwner = nullptr);
    ~CMusicPlayerCmdHelper();

    void VeiwOnline(SongInfo& song);
    void FormatConvert(const std::vector<SongInfo>& songs);

private:
    CWnd* GetOwner();

private:
    CWnd* m_pOwner{};
};

