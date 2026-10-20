#include "UIFadeButton.h"
#include "UIPaintManager.h"

UIFadeButton::UIFadeButton()
    : CUIAnimation(this),
      m_bFadeAlpha(0),
      m_bMouseHove(false),
      m_bMouseLeave(false),
      m_hasFadeEffect(true)
{
}

UIFadeButton::~UIFadeButton()
{
    StopAnimation();
}

UIString UIFadeButton::GetClass() const
{
    return UIString{u8"FadeButtonUI"};
}

LPVOID UIFadeButton::GetInterface(const UIString &name)
{
    if(name == u8"FadeButtonUI") {
        return static_cast<UIFadeButton*>(this);
    }
    return UIButton::GetInterface(name);
}

void UIFadeButton::DoEvent(TEventUI &event)
{
    if(!m_hasFadeEffect) {
        UIButton::DoEvent(event);
        return;
    }

    if(event.Type == UIEVENT_MOUSEENTER && !IsAnimationRunning(FADE_IN_ID)) {
        m_bFadeAlpha = 0;
        m_bMouseHove = true;
        m_bMouseLeave = false;
        StopAnimation(FADE_OUT_ID);
        StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID);
        Invalidate();
        return;
    }
    if(event.Type == UIEVENT_MOUSELEAVE && !IsAnimationRunning(FADE_OUT_ID)) {
        m_bFadeAlpha = 0;
        m_bMouseLeave = true;
        m_bMouseHove = false;
        StopAnimation(FADE_IN_ID);
        StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
        Invalidate();
        return;
    }
    if(event.Type == UIEVENT_TIMER) {
        OnTimer((int)(intptr_t)event.wParam);
    }
    UIButton::DoEvent(event);
}

void UIFadeButton::SetFadeEffect(bool hasFadeEffect)
{
    m_hasFadeEffect = hasFadeEffect;
}

void UIFadeButton::OnTimer(int nTimerID)
{
    int nAnimID = GetAnimationIDByTimerID((uint32_t)nTimerID);
    if(nAnimID != 0) {
        OnAnimationElapse(nAnimID);
    }
}

void UIFadeButton::PaintStatusImage(HANDLE_DC hDC)
{
    if(!m_hasFadeEffect) {
        UIButton::PaintStatusImage(hDC);
        return;
    }

    if(IsFocused()) m_buttonState |= UISTATE_FOCUSED;
    else m_buttonState &= ~UISTATE_FOCUSED;
    if(!IsEnabled()) m_buttonState |= UISTATE_DISABLED;
    else m_buttonState &= ~UISTATE_DISABLED;

    if((m_buttonState & UISTATE_DISABLED) != 0) {
        if(!m_diDisabled.sDrawString.IsEmpty()) {
            if(!DrawImage(hDC, m_diDisabled)) m_diDisabled.sDrawString.Empty();
            else return;
        }
    }
    else if((m_buttonState & UISTATE_PUSHED) != 0) {
        if(!m_diPushed.sDrawString.IsEmpty()) {
            if(!DrawImage(hDC, m_diPushed)) m_diPushed.sDrawString.Empty();
            else return;
        }
    }
    else if((m_buttonState & UISTATE_FOCUSED) != 0) {
        if(!m_diFocused.sDrawString.IsEmpty()) {
            if(!DrawImage(hDC, m_diFocused)) m_diFocused.sDrawString.Empty();
            else return;
        }
    }

    if(!m_diNormal.sDrawString.IsEmpty()) {
        if(IsAnimationRunning(FADE_IN_ID) || IsAnimationRunning(FADE_OUT_ID)) {
            if(m_bMouseHove) {
                m_bMouseHove = false;
                m_sLastImage = m_diHot.sDrawString;
                if(!DrawImage(hDC, m_diNormal)) m_diNormal.sDrawString.Empty();
                return;
            }

            if(m_bMouseLeave) {
                m_bMouseLeave = false;
                m_sLastImage = m_diNormal.sDrawString;
                if(!DrawImage(hDC, m_diNormal)) m_diHot.sDrawString.Empty();
                return;
            }

            m_sOldImage = m_diNormal.sDrawString;
            m_sNewImage = m_diHot.sDrawString;
            if(IsAnimationRunning(FADE_OUT_ID)) {
                m_sOldImage = m_diHot.sDrawString;
                m_sNewImage = m_diNormal.sDrawString;
            }

            // Draw old image with fade out alpha
            if(!m_sOldImage.IsEmpty()) {
                if(m_sOldImage.Find(m_diNormal.sDrawString) >= 0) {
                    m_diNormal.uFade = (uint8_t)(255 - m_bFadeAlpha);
                    DrawImage(hDC, m_diNormal);
                }
                if(m_sOldImage.Find(m_diHot.sDrawString) >= 0) {
                    m_diHot.uFade = (uint8_t)(255 - m_bFadeAlpha);
                    DrawImage(hDC, m_diHot);
                }
            }

            // Draw new image with fade in alpha
            if(!m_sNewImage.IsEmpty()) {
                if(m_sNewImage.Find(m_diNormal.sDrawString) >= 0) {
                    m_diNormal.uFade = m_bFadeAlpha;
                    DrawImage(hDC, m_diNormal);
                }
                if(m_sNewImage.Find(m_diHot.sDrawString) >= 0) {
                    m_diHot.uFade = m_bFadeAlpha;
                    DrawImage(hDC, m_diHot);
                }
            }
            return;
        }
        else {
            if(m_sLastImage.IsEmpty()) {
                m_sLastImage = m_diNormal.sDrawString;
            }
            if(!DrawImage(hDC, m_diNormal)) m_sLastImage.Empty();
            return;
        }
    }
}

void UIFadeButton::OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID)
{
    m_bFadeAlpha = (uint8_t)((nCurFrame / (double)nTotalFrame) * 255);
    if(m_bFadeAlpha == 0) m_bFadeAlpha = 10;
    Invalidate();
}

void UIFadeButton::SetAttribute(const char *pstrName, const char *pstrValue)
{
    if(strcasecmp(pstrName, "fadeeffect") == 0) {
        SetFadeEffect(strcasecmp(pstrValue, "true") == 0);
    }
    else {
        UIButton::SetAttribute(pstrName, pstrValue);
    }
}