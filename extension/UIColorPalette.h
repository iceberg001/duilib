#ifndef DUILIB_EXT_UICOLORPALETTE_H
#define DUILIB_EXT_UICOLORPALETTE_H

#ifdef WIN32
#include "UIControl.h"
#include "UIString.h"
#include <windows.h>

#define DUI_EXT_MSGTYPE_COLORCHANGED "colorchanged"

class UIColorPalette : public UIControl
{
public:
    UIColorPalette();
    virtual ~UIColorPalette();

    uint32_t GetSelectColor();
    void SetSelectColor(uint32_t dwColor);

    UIString GetClass() const override;
    LPVOID GetInterface(const UIString &name) override;
    void SetAttribute(const char *pstrName, const char *pstrValue) override;

    void SetPalletHeight(int nHeight);
    int  GetPalletHeight() const;
    void SetBarHeight(int nHeight);
    int  GetBarHeight() const;
    void SetThumbImage(const UIString &pszImage);
    UIString GetThumbImage() const;

    void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    void DoInit() override;
    void DoEvent(TEventUI &event) override;
    void PaintBkColor(HANDLE_DC hDC) override;

protected:
    void UpdatePalletData();
    void UpdateBarData();

private:
    HDC         m_MemDc;
    HBITMAP     m_hMemBitmap;
    BITMAP      m_bmInfo;
    BYTE       *m_pBits;
    uint32_t    m_buttonState;
    bool        m_bIsInBar;
    bool        m_bIsInPallet;
    int         m_nCurH;
    int         m_nCurS;
    int         m_nCurB;

    int         m_nPalletHeight;
    int         m_nBarHeight;
    POINT       m_ptLastPalletMouse;
    POINT       m_ptLastBarMouse;
    UIString    m_strThumbImage;
    TDrawInfo   m_diThumb;
};

#endif // WIN32
#endif // DUILIB_EXT_UICOLORPALETTE_H