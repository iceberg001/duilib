#include "ExtensionDemoWnd.h"
#include "UIDlgBuilder.h"
#include "UIPaintManager.h"
#include "UILabel.h"
#include "ExtensionApi.h"
#include <cstdio>

DUI_BEGIN_MESSAGE_MAP(ExtensionDemoWnd, UIWindowImpBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, ExtensionDemoWnd::OnClose)

DUI_ON_MSGTYPE(DUI_EXT_MSGTYPE_COLORCHANGED, ExtensionDemoWnd::OnColorChanged)
DUI_END_MESSAGE_MAP()

long ExtensionDemoWnd::OnCreate(uint32_t uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
    UIWindowImpBase::OnCreate(uMsg, wParam, lParam, bHandled);
    m_pm.Init(this->GetWND());
    UIDlgBuilder dlgBuilder;
    UIControl * root = dlgBuilder.Create(this->GetSkinFile(),
                                         UIDlgBuilder::SkinType_XmlFile,
                                         this, &m_pm);
    m_pm.AttachDialog(root);
    m_pm.AddNotifier(this);
    return 1;
}

UIControl * ExtensionDemoWnd::CreateControl(const char * pstrClass)
{
    if(strcasecmp("FadeButtonUI", pstrClass) == 0) {
        return new UIFadeButton;
    }
    if(strcasecmp("TimeButtonUI", pstrClass) == 0) {
        return new UITimeButton;
    }
#ifdef WIN32
    if(strcasecmp("ElipsePhotoUI", pstrClass) == 0) {
        return new UIElipsePhoto;
    }
    if(strcasecmp("HotKeyUI", pstrClass) == 0) {
        return new UIHotKey;
    }
    if(strcasecmp("IPAddressUI", pstrClass) == 0) {
        return new UIIPAddress;
    }
    if(strcasecmp("ColorPaletteUI", pstrClass) == 0) {
        return new UIColorPalette;
    }
    if(strcasecmp("DateTimeUI", pstrClass) == 0) {
        return new UIDateTime;
    }
#endif
    return nullptr;
}

void ExtensionDemoWnd::OnClose(TNotifyUI & msg)
{
    if(msg.pSender && msg.pSender->GetName() == u8"closebtn") {
        Close();
    }
}


void ExtensionDemoWnd::OnColorChanged(TNotifyUI & msg)
{
    UILabel * label = static_cast<UILabel*>(m_pm.FindControl(u8"colorresult"));
    if(label) {
        uint32_t color = (uint32_t)(uintptr_t)msg.wParam;
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "#%08X", color);
        label->SetText(UIString{buf});
        label->SetBkColor(color);
    }
}