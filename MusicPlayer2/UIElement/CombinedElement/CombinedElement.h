#pragma once
#include "../UIElement.h"
namespace UiElement
{
    class CombinedElement : public Element
    {
    public:
        CombinedElement(UINT res_id);

    protected:
        virtual void InitComplete() override;

    protected:
        UINT m_res_id;
    };
}

