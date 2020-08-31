#pragma once
class IPropertyTabDlg
{
public:
    IPropertyTabDlg() {}
    virtual ~IPropertyTabDlg() {}

    virtual int SaveModified() = 0;
    virtual void PagePrevious() = 0;
    virtual void PageNext() = 0;
};
