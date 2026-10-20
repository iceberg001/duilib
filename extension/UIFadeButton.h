#ifndef DUILIB_EXT_UIFADEBUTTON_H
#define DUILIB_EXT_UIFADEBUTTON_H

#include "UIButton.h"
#include "UIAnimation.h"

class UIFadeButton : public UIButton, public CUIAnimation
{
public:
    UIFadeButton();
    virtual ~UIFadeButton();

    UIString GetClass() const override;
    LPVOID GetInterface(const UIString &name) override;
    void DoEvent(TEventUI &event) override;
    void SetFadeEffect(bool hasFadeEffect);

    void OnTimer(int nTimerID);
    void PaintStatusImage(HANDLE_DC hDC) override;

    virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) {}
    virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) override;
    virtual void OnAnimationStop(int nAnimationID) {}

    void SetAttribute(const char *pstrName, const char *pstrValue) override;

protected:
    UIString m_sOldImage;
    UIString m_sNewImage;
    UIString m_sLastImage;
    uint8_t  m_bFadeAlpha;
    bool     m_bMouseHove;
    bool     m_bMouseLeave;
    enum {
        FADE_IN_ID    = 8,
        FADE_OUT_ID   = 9,

        FADE_ELLAPSE = 10,
        FADE_FRAME_COUNT = 50,
    };

private:
    bool m_hasFadeEffect;
};

#endif // DUILIB_EXT_UIFADEBUTTON_H