#pragma once
class IMouseEvent
{
public:
    virtual bool LButtonUp(CPoint point) = 0;
    virtual bool LButtonDown(CPoint point) = 0;
    virtual bool MouseMove(CPoint point) = 0;
    virtual bool RButtonUp(CPoint point) = 0;
    virtual bool RButtonDown(CPoint point) = 0;
    virtual bool MouseWheel(int delta, CPoint point) = 0;
    virtual bool DoubleClick(CPoint point) = 0;
    virtual bool MouseLeave() = 0;

};
