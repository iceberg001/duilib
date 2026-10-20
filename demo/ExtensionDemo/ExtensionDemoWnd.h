#ifndef DIRECTUI_EXTENSIONDEMOWND_H
#define DIRECTUI_EXTENSIONDEMOWND_H

#include "UIWindowImpBase.h"
#include "IDialogBuilderCallback.h"

class ExtensionDemoWnd : public UIWindowImpBase, public IDialogBuilderCallback
{
    DUI_DECLARE_MESSAGE_MAP()
public:
    long OnCreate(uint32_t uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled) override;

    UIControl * CreateControl(const char * pstrClass) override;

    UIString GetSkinFile() const override
    {
        return UIString{u8"extension_demo.xml"};
    }

private:
    void OnClose(TNotifyUI & msg);
    void OnColorChanged(TNotifyUI & msg);

};

#endif