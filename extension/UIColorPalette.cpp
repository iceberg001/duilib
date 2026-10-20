#include "UIColorPalette.h"

#ifdef WIN32
#include "UIPaintManager.h"
#include "UIRenderEngine.h"
#include "UIResourceMgr.h"
#include <math.h>
#include <algorithm>

#define HSLMAX   255
#define RGBMAX   255
#define HSLUNDEFINED (HSLMAX*2/3)

static float HueToRGB(float v1, float v2, float vH)
{
    if(vH < 0.0f) vH += 1.0f;
    if(vH > 1.0f) vH -= 1.0f;
    if((6.0f * vH) < 1.0f) return (v1 + (v2 - v1) * 6.0f * vH);
    if((2.0f * vH) < 1.0f) return (v2);
    if((3.0f * vH) < 2.0f) return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f);
    return (v1);
}

static void RGBToHSL(DWORD clr, float *pHue, float *pSat, float *pLue)
{
    float R = (float)(GetRValue(clr) / 255.0f);
    float G = (float)(GetGValue(clr) / 255.0f);
    float B = (float)(GetBValue(clr) / 255.0f);

    float H, S, L;

    float fMin = std::min(R, std::min(G, B));
    float fMax = std::max(R, std::max(G, B));
    float fDelta = fMax - fMin;

    L = (fMax + fMin) / 2.0f;

    if(fDelta == 0) {
        H = 0.0f;
        S = 0.0f;
    }
    else {
        float del_R, del_G, del_B;

        if(L < 0.5) S = fDelta / (fMax + fMin);
        else         S = fDelta / (2.0f - fMax - fMin);

        del_R = (((fMax - R) / 6.0f) + (fDelta / 2.0f)) / fDelta;
        del_G = (((fMax - G) / 6.0f) + (fDelta / 2.0f)) / fDelta;
        del_B = (((fMax - B) / 6.0f) + (fDelta / 2.0f)) / fDelta;

        if(R == fMax) H = del_B - del_G;
        else if(G == fMax) H = (1.0f / 3.0f) + del_R - del_B;
        else if(B == fMax) H = (2.0f / 3.0f) + del_G - del_R;

        if(H < 0.0f) H += 1.0f;
        if(H > 1.0f)  H -= 1.0f;
    }

    *pHue = H;
    *pSat = S;
    *pLue = L;
}

static DWORD HSLToRGB(float H, float S, float L)
{
    BYTE R, G, B;
    float var_1, var_2;

    if(S == 0) {
        R = G = B = (BYTE)(L * 255.0f);
    }
    else {
        if(L < 0.5) var_2 = L * (1.0f + S);
        else         var_2 = (L + S) - (S * L);

        var_1 = 2.0f * L - var_2;

        R = (BYTE)(255.0f * HueToRGB(var_1, var_2, H + (1.0f / 3.0f)));
        G = (BYTE)(255.0f * HueToRGB(var_1, var_2, H));
        B = (BYTE)(255.0f * HueToRGB(var_1, var_2, H - (1.0f / 3.0f)));
    }

    return RGB(R, G, B);
}

#define _HSLToRGB(h,s,l) (0xFF << 24 | HSLToRGB((float)h / 360.0f,(float)s / 200.0f,l / 200.0f))

UIColorPalette::UIColorPalette()
    : m_buttonState(0),
      m_bIsInBar(false),
      m_bIsInPallet(false),
      m_nCurH(180),
      m_nCurS(200),
      m_nCurB(100),
      m_nPalletHeight(200),
      m_nBarHeight(10),
      m_pBits(nullptr),
      m_MemDc(nullptr),
      m_hMemBitmap(nullptr)
{
    memset(&m_bmInfo, 0, sizeof(m_bmInfo));
    memset(&m_ptLastPalletMouse, 0, sizeof(m_ptLastPalletMouse));
    memset(&m_ptLastBarMouse, 0, sizeof(m_ptLastBarMouse));
}

UIColorPalette::~UIColorPalette()
{
    if(m_pBits) free(m_pBits);
    if(m_hMemBitmap) ::DeleteObject(m_hMemBitmap);
    if(m_MemDc) ::DeleteDC(m_MemDc);
}

uint32_t UIColorPalette::GetSelectColor()
{
    DWORD dwColor = _HSLToRGB(m_nCurH, m_nCurS, m_nCurB);
    return 0xFF << 24 | GetRValue(dwColor) << 16 | GetGValue(dwColor) << 8 | GetBValue(dwColor);
}

void UIColorPalette::SetSelectColor(uint32_t dwColor)
{
    float H = 0, S = 0, B = 0;
    COLORREF dwBkClr = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    RGBToHSL(dwBkClr, &H, &S, &B);
    m_nCurH = (int)(H * 360);
    m_nCurS = (int)(S * 200);
    m_nCurB = (int)(B * 200);
    NeedUpdate();
    UpdatePalletData();
}

UIString UIColorPalette::GetClass() const
{
    return UIString{u8"ColorPaletteUI"};
}

LPVOID UIColorPalette::GetInterface(const UIString &name)
{
    if(name == u8"ColorPaletteUI") return static_cast<UIColorPalette*>(this);
    return UIControl::GetInterface(name);
}

void UIColorPalette::SetPalletHeight(int nHeight) { m_nPalletHeight = nHeight; }
int  UIColorPalette::GetPalletHeight() const { return m_nPalletHeight; }
void UIColorPalette::SetBarHeight(int nHeight)
{
    if(nHeight > 150) nHeight = 150;
    m_nBarHeight = nHeight;
}
int  UIColorPalette::GetBarHeight() const { return m_nBarHeight; }

void UIColorPalette::SetThumbImage(const UIString &pszImage)
{
    if(m_strThumbImage != pszImage) {
        m_strThumbImage = pszImage;
        m_diThumb.Clear();
        m_diThumb.sDrawString = pszImage;
        NeedUpdate();
    }
}

UIString UIColorPalette::GetThumbImage() const { return m_strThumbImage; }

void UIColorPalette::SetAttribute(const char *pstrName, const char *pstrValue)
{
    if(strcasecmp(pstrName, "palletheight") == 0) SetPalletHeight(atoi(pstrValue));
    else if(strcasecmp(pstrName, "barheight") == 0) SetBarHeight(atoi(pstrValue));
    else if(strcasecmp(pstrName, "thumbimage") == 0) SetThumbImage(UIString{pstrValue});
    else UIControl::SetAttribute(pstrName, pstrValue);
}

void UIColorPalette::DoInit()
{
    m_MemDc = CreateCompatibleDC(m_manager->GetPaintDC());
    m_hMemBitmap = CreateCompatibleBitmap(m_manager->GetPaintDC(), 400, 360);
    HBITMAP pOldBit = (HBITMAP)SelectObject(m_MemDc, m_hMemBitmap);

    ::GetObject(m_hMemBitmap, sizeof(m_bmInfo), &m_bmInfo);
    DWORD dwSize = m_bmInfo.bmHeight * m_bmInfo.bmWidthBytes;
    m_pBits = (BYTE *)malloc(dwSize);
    ::GetBitmapBits(m_hMemBitmap, dwSize, m_pBits);
}

void UIColorPalette::SetPos(RECT rc, bool bNeedInvalidate)
{
    UIControl::SetPos(rc, bNeedInvalidate);

    m_ptLastPalletMouse.x = m_nCurH * (m_rcItem.right - m_rcItem.left) / 360 + m_rcItem.left;
    m_ptLastPalletMouse.y = (200 - m_nCurB) * m_nPalletHeight / 200 + m_rcItem.top;

    UpdatePalletData();
    UpdateBarData();
}

void UIColorPalette::DoEvent(TEventUI &event)
{
    UIControl::DoEvent(event);

    if(event.Type == UIEVENT_BUTTONDOWN) {
        if(event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.top &&
           event.ptMouse.x < m_rcItem.right && event.ptMouse.y < m_rcItem.top + m_nPalletHeight) {
            int x = (event.ptMouse.x - m_rcItem.left) * 360 / (m_rcItem.right - m_rcItem.left);
            int y = (event.ptMouse.y - m_rcItem.top) * 200 / m_nPalletHeight;
            x = std::min(std::max(x, 0), 360);
            y = std::min(std::max(y, 0), 200);

            m_ptLastPalletMouse = event.ptMouse;
            if(m_ptLastPalletMouse.x < m_rcItem.left) m_ptLastPalletMouse.x = m_rcItem.left;
            if(m_ptLastPalletMouse.x > m_rcItem.right) m_ptLastPalletMouse.x = m_rcItem.right;
            if(m_ptLastPalletMouse.y < m_rcItem.top) m_ptLastPalletMouse.y = m_rcItem.top;
            if(m_ptLastPalletMouse.y > m_rcItem.top + m_nPalletHeight) m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;

            m_nCurH = x;
            m_nCurB = 200 - y;

            m_buttonState |= UISTATE_PUSHED;
            m_bIsInPallet = true;
            m_bIsInBar = false;

            UpdateBarData();
        }
        if(event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.bottom - m_nBarHeight &&
           event.ptMouse.x < m_rcItem.right && event.ptMouse.y < m_rcItem.bottom) {
            m_nCurS = (event.ptMouse.x - m_rcItem.left) * 200 / (m_rcItem.right - m_rcItem.left);
            m_buttonState |= UISTATE_PUSHED;
            m_bIsInBar = true;
            m_bIsInPallet = false;
            UpdatePalletData();
        }
        Invalidate();
        return;
    }
    if(event.Type == UIEVENT_BUTTONUP) {
        if((m_buttonState | UISTATE_PUSHED) && (IsEnabled())) {
            uint32_t color = GetSelectColor();
            m_manager->SendNotify(this, DUI_EXT_MSGTYPE_COLORCHANGED, (WPARAM)color, 0);
        }
        m_buttonState &= ~UISTATE_PUSHED;
        m_bIsInPallet = false;
        m_bIsInBar = false;
        Invalidate();
        return;
    }
    if(event.Type == UIEVENT_MOUSEMOVE) {
        if(!(m_buttonState & UISTATE_PUSHED)) {
            m_bIsInBar = false;
            m_bIsInPallet = false;
        }
        if(m_bIsInPallet == true) {
            POINT pt = event.ptMouse;
            pt.x -= m_rcItem.left;
            pt.y -= m_rcItem.top;

            if(pt.x >= 0 && pt.y >= 0 && pt.x <= m_rcItem.right && pt.y <= m_rcItem.top + m_nPalletHeight) {
                int x = pt.x * 360 / (m_rcItem.right - m_rcItem.left);
                int y = pt.y * 200 / m_nPalletHeight;
                x = std::min(std::max(x, 0), 360);
                y = std::min(std::max(y, 0), 200);

                m_ptLastPalletMouse = event.ptMouse;
                if(m_ptLastPalletMouse.x < m_rcItem.left) m_ptLastPalletMouse.x = m_rcItem.left;
                if(m_ptLastPalletMouse.x > m_rcItem.right) m_ptLastPalletMouse.x = m_rcItem.right;
                if(m_ptLastPalletMouse.y < m_rcItem.top) m_ptLastPalletMouse.y = m_rcItem.top;
                if(m_ptLastPalletMouse.y > m_rcItem.top + m_nPalletHeight) m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;

                m_nCurH = x;
                m_nCurB = 200 - y;

                UpdateBarData();
            }
        }
        if(m_bIsInBar == true) {
            m_nCurS = (event.ptMouse.x - m_rcItem.left) * 200 / (m_rcItem.right - m_rcItem.left);
            m_nCurS = std::min(std::max(m_nCurS, 0), 200);
            UpdatePalletData();
        }
        Invalidate();
        return;
    }
}

void UIColorPalette::PaintBkColor(HANDLE_DC hDC)
{
    if(!m_MemDc) return;

    int nSaveDC = ::SaveDC(hDC);
    ::SetStretchBltMode(hDC, HALFTONE);

    StretchBlt(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_nPalletHeight, m_MemDc, 0, 0, 360, 199, SRCCOPY);
    StretchBlt(hDC, m_rcItem.left, m_rcItem.bottom - m_nBarHeight, m_rcItem.right - m_rcItem.left, m_nBarHeight, m_MemDc, 0, 210, 200, m_nBarHeight, SRCCOPY);

    if(!m_strThumbImage.IsEmpty()) {
        RECT rcCurSorPaint = {m_ptLastPalletMouse.x - 4, m_ptLastPalletMouse.y - 4, m_ptLastPalletMouse.x + 4, m_ptLastPalletMouse.y + 4};
        DrawImage(hDC, m_diThumb);

        rcCurSorPaint.left = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / 200 - 4;
        rcCurSorPaint.right = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / 200 + 4;
        rcCurSorPaint.top = m_rcItem.bottom - m_nBarHeight / 2 - 4;
        rcCurSorPaint.bottom = m_rcItem.bottom - m_nBarHeight / 2 + 4;
        DrawImage(hDC, m_diThumb);
    }
    ::RestoreDC(hDC, nSaveDC);
}

void UIColorPalette::UpdatePalletData()
{
    if(!m_pBits) return;
    int x, y;
    BYTE *pPiexl;
    DWORD dwColor;
    for(y = 0; y < 200; ++y) {
        for(x = 0; x < 360; ++x) {
            pPiexl = LPBYTE(m_pBits) + ((199 - y) * m_bmInfo.bmWidthBytes) + ((x * m_bmInfo.bmBitsPixel) / 8);
            dwColor = _HSLToRGB(x, m_nCurS, y);
            pPiexl[0] = GetBValue(dwColor);
            pPiexl[1] = GetGValue(dwColor);
            pPiexl[2] = GetRValue(dwColor);
        }
    }
    SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
}

void UIColorPalette::UpdateBarData()
{
    if(!m_pBits) return;
    int x, y;
    BYTE *pPiexl;
    DWORD dwColor;
    for(y = 0; y < m_nBarHeight; ++y) {
        for(x = 0; x < 200; ++x) {
            pPiexl = LPBYTE(m_pBits) + ((210 + y) * m_bmInfo.bmWidthBytes) + ((x * m_bmInfo.bmBitsPixel) / 8);
            dwColor = _HSLToRGB(m_nCurH, x, m_nCurB);
            pPiexl[0] = GetBValue(dwColor);
            pPiexl[1] = GetGValue(dwColor);
            pPiexl[2] = GetRValue(dwColor);
        }
    }
    SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
}

#endif // WIN32