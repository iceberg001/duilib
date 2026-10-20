#include "UITimeButton.h"
#include "UIPaintManager.h"
#include <cstdio>

UITimeButton::UITimeButton()
{
    m_textBak = UIString{u8""};
    m_textPadding = UIString{u8"S"};
    m_nTimeLong = 0;
    m_nTimerNow = 0;
    m_nTimerID = 0;
}

UITimeButton::~UITimeButton()
{
}

void UITimeButton::SetTimeLong(unsigned long nValue)
{
    if(nValue <= 0) return;
    m_nTimeLong = nValue;
    m_textBak = this->GetText();
}

void UITimeButton::SetPaddingText(const UIString &str)
{
    m_textPadding = str;
}

void UITimeButton::UpShow()
{
    m_nTimerNow--;
    if(m_nTimerNow <= 0) {
        this->SetEnabled(true);
        this->SetText(m_textBak);
    }
    else {
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "%lu", m_nTimerNow);
        UIString text = UIString{buf} + m_textPadding;
        this->SetText(text);
        this->SetEnabled(false);
    }
}

bool UITimeButton::NeedKillTimer()
{
    if(m_nTimerNow <= 0 || m_nTimerNow > m_nTimeLong) {
        return true;
    }
    return false;
}

void UITimeButton::SetAttribute(const char *pstrName, const char *pstrValue)
{
    if(strcasecmp(pstrName, "sectime") == 0) {
        unsigned long nValue = (unsigned long)atoi(pstrValue);
        m_nTimeLong = nValue;
    }
    else {
        UIButton::SetAttribute(pstrName, pstrValue);
    }
}

void UITimeButton::DoEvent(TEventUI &event)
{
    if(event.Type == UIEVENT_TIMER) {
        OnTimer((int)(intptr_t)event.wParam);
    }
    UIButton::DoEvent(event);
}

void UITimeButton::OnTimer(int nTimerID)
{
    if((uint32_t)nTimerID == m_nTimerID) {
        if(NeedKillTimer()) {
            this->GetManager()->KillTimer(this, m_nTimerID);
            m_nTimerID = 0;
        }
        else {
            UpShow();
        }
    }
}

bool UITimeButton::Activate()
{
    if(m_nTimeLong > 0) {
        m_textBak = this->GetText();
        m_nTimerNow = m_nTimeLong;
        m_nTimerNow++;
        UpShow();
        m_nTimerID = this->GetManager()->SetTimer(this, 1000);
    }
    bool bRet = UIButton::Activate();
    return bRet;
}