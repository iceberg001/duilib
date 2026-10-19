#ifndef DIRECTUI_CNODEDEMOWND_H
#define DIRECTUI_CNODEDEMOWND_H

#include "UIWindowImpBase.h"
#include "IDialogBuilderCallback.h"

class CNodeDemoWnd : public UIWindowImpBase, public IDialogBuilderCallback
{
    DUI_DECLARE_MESSAGE_MAP()
public:
    long OnCreate(uint32_t uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled) override;

    UIControl * CreateControl(const char * pstrClass) override;

    UIString GetSkinFile() const override
    {
        return UIString{u8"node_demo.xml"};
    }

private:
    void OnClose(TNotifyUI & msg);
};

#endif