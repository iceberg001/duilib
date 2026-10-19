#include "CNodeUIBase.h"
#include "CNodeUIManager.h"
#include "UIPaintManager.h"
#include <cstdint>

CNodeUIBase::CNodeUIBase()
    : bRegistered_(false)
{
    SetMouseChildEnabled(true);
}

CNodeUIBase::~CNodeUIBase()
{
    if (bRegistered_)
    {
        CNodeUIManager::GetInstance().UnregisterNode(this);
        bRegistered_ = false;
    }
}

UIString CNodeUIBase::GetClass() const
{
    return UIString{"CNodeUIBase"};
}

void CNodeUIBase::SetManager(UIPaintManager * pManager, UIControl * pParent, bool bInit)
{
    UIPaintManager * oldManager = GetManager();
    UIContainer::SetManager(pManager, pParent, bInit);

    if (pManager != nullptr && oldManager == nullptr && !bRegistered_)
    {
        CNodeUIManager::GetInstance().RegisterNode(this);
        bRegistered_ = true;
    }
    else if (pManager == nullptr && bRegistered_)
    {
        CNodeUIManager::GetInstance().UnregisterNode(this);
        bRegistered_ = false;
    }
}

void CNodeUIBase::SetAttribute(const char * pstrName, const char * pstrValue)
{
    if (strcasecmp(pstrName, "nodename") == 0)
    {
        SetNodeName(UIString{pstrValue});
    }
    else
    {
        UIContainer::SetAttribute(pstrName, pstrValue);
    }
}

void CNodeUIBase::DoEvent(TEventUI & event)
{
    if (event.Type == UIEVENT_TIMER)
    {
        uint32_t activeId = CNodeUIManager::GetInstance().GetActiveTimerId();
        if (activeId != 0 && event.wParam == (WPARAM)(uintptr_t)activeId)
        {
            CNodeUIManager::GetInstance().ProcessMessageQueue();
            return;
        }
    }
    UIContainer::DoEvent(event);
}

void CNodeUIBase::PostNodeMessage(const UIString & msgType,
                                  const UIString & msgData,
                                  WPARAM           wParam,
                                  LPARAM           lParam,
                                  bool             b_async)
{
    CNodeMessage msg(msgType, msgData, this, wParam, lParam);
    CNodeUIManager::GetInstance().BroadcastMessage(msg, b_async);
}

void CNodeUIBase::PostNodeMessage(const UIString & msgType,
                                  void *          data,
                                  size_t          size,
                                  WPARAM           wParam,
                                  LPARAM           lParam,
                                  bool             b_async)
{
    CNodeMessage msg(msgType, data, size, this, wParam, lParam);
    CNodeUIManager::GetInstance().BroadcastMessage(msg, b_async);
}
