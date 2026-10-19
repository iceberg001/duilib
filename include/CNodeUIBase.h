#ifndef DIRECTUI_CNODEUIBASE_H
#define DIRECTUI_CNODEUIBASE_H

#include "UIContainer.h"
#include "CNodeMessage.h"

class CNodeUIManager;

class CNodeUIBase : public UIContainer
{
public:
    CNodeUIBase();
    virtual ~CNodeUIBase();

    UIString GetClass() const override;

    void SetManager(UIPaintManager * pManager, UIControl * pParent, bool bInit = true) override;

    void SetAttribute(const char * pstrName, const char * pstrValue) override;

    void DoEvent(TEventUI & event) override;

    void PostNodeMessage(const UIString & msgType,
                         const UIString & msgData = UIString{""},
                         WPARAM           wParam = 0,
                         LPARAM           lParam = 0,
                         bool             b_async = true);

    void PostNodeMessage(const UIString & msgType,
                         void *          data,
                         size_t          size,
                         WPARAM           wParam = 0,
                         LPARAM           lParam = 0,
                         bool             b_async = true);

    virtual bool WantNodeMessage() const { return false; }

    virtual void OnNodeMessage(const CNodeMessage & msg) {}

    UIString GetNodeName() const { return nodeName_; }
    void     SetNodeName(const UIString & name) { nodeName_ = name; }

    bool IsRegisteredToManager() const { return bRegistered_; }

protected:
    UIString    nodeName_;
    bool        bRegistered_;
};

#endif
