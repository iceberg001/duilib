#ifndef DUILIB_EXT_UITIMEBUTTON_H
#define DUILIB_EXT_UITIMEBUTTON_H

#include "UIButton.h"

class UITimeButton : public UIButton
{
public:
    UITimeButton();
    ~UITimeButton();

    void SetTimeLong(unsigned long nValue);
    void SetPaddingText(const UIString &str);

    void DoEvent(TEventUI &event) override;
    void OnTimer(int nTimerID);
    bool Activate() override;
    void UpShow();
    bool NeedKillTimer();
    void SetAttribute(const char *pstrName, const char *pstrValue) override;

private:
    UIString        m_textBak;
    UIString        m_textPadding;
    unsigned long   m_nTimeLong;
    unsigned long   m_nTimerNow;
    uint32_t        m_nTimerID;
};

#endif // DUILIB_EXT_UITIMEBUTTON_H