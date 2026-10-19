#include "CNodeDemoWnd.h"
#include "UIDlgBuilder.h"
#include "UIPaintManager.h"
#include "CNodeAddUI.h"
#include "CNodeResultUI.h"
#include "CNodeUIManager.h"

DUI_BEGIN_MESSAGE_MAP(CNodeDemoWnd, UIWindowImpBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, CNodeDemoWnd::OnClose)
DUI_END_MESSAGE_MAP()

long CNodeDemoWnd::OnCreate(uint32_t uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
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

UIControl * CNodeDemoWnd::CreateControl(const char * pstrClass)
{
    if (strcasecmp("CNodeAddUI", pstrClass) == 0)
    {
        return new CNodeAddUI;
    }
    if (strcasecmp("CNodeResultUI", pstrClass) == 0)
    {
        return new CNodeResultUI;
    }
    return nullptr;
}

void CNodeDemoWnd::OnClose(TNotifyUI & msg)
{
    if (msg.pSender && msg.pSender->GetName() == u8"closebtn")
    {
        CNodeUIManager::GetInstance().Clear();
        Close();
    }
}