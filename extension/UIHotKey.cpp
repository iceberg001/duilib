#include "UIHotKey.h"

#ifdef WIN32
#include "UIPaintManager.h"
#include "UIRenderEngine.h"
#include "UIResourceMgr.h"
#include "UIFont.h"
#include <cstdio>

static const wchar_t* g_HotKeyWndClass = L"DuiLibExtHotKeyWnd";
static bool g_HotKeyWndClassRegistered = false;

static void RegisterHotKeyWndClass()
{
    if(g_HotKeyWndClassRegistered) return;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = UIHotKeyWnd::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = g_HotKeyWndClass;
    RegisterClassW(&wc);
    g_HotKeyWndClassRegistered = true;
}

UIHotKeyWnd::UIHotKeyWnd()
    : m_pOwner(nullptr), m_hWnd(nullptr), m_hBkBrush(nullptr), m_bInit(false), m_oldWndProc(nullptr)
{
}

UIHotKeyWnd::~UIHotKeyWnd()
{
    if(m_hWnd && IsWindow(m_hWnd)) {
        DestroyWindow(m_hWnd);
    }
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
}

LRESULT CALLBACK UIHotKeyWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UIHotKeyWnd *pThis = (UIHotKeyWnd*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if(pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void UIHotKeyWnd::Init(UIHotKey *pOwner)
{
    m_pOwner = pOwner;
    if(!m_pOwner) return;

    RegisterHotKeyWndClass();

    RECT rcPos = CalPos();
    m_hWnd = CreateWindowExW(0, g_HotKeyWndClass, L"", WS_CHILD | ES_AUTOHSCROLL,
        rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_pOwner->GetManager()->GetPaintWindow(), nullptr, GetModuleHandle(nullptr), nullptr);
    if(!m_hWnd) return;

    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    // Create native HotKey control as child
    HWND hHotKey = CreateWindowExW(0, HOTKEY_CLASSW, L"", WS_CHILD | WS_VISIBLE,
        0, 0, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_hWnd, nullptr, GetModuleHandle(nullptr), nullptr);
    if(hHotKey) {
        m_oldWndProc = (WNDPROC)SetWindowLongPtr(hHotKey, GWLP_WNDPROC, (LONG_PTR)DefWindowProcW);
        SetHotKey(m_pOwner->m_wVirtualKeyCode, m_pOwner->m_wModifiers);
    }

    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);
    m_bInit = true;
}

RECT UIHotKeyWnd::CalPos()
{
    RECT rcPos = m_pOwner->GetPos();
    RECT rcInset = m_pOwner->GetTextPadding();
    rcPos.left += rcInset.left;
    rcPos.top += rcInset.top;
    rcPos.right -= rcInset.right;
    rcPos.bottom -= rcInset.bottom;
    return rcPos;
}

void UIHotKeyWnd::OnFinalMessage(HWND /*hWnd*/)
{
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
    m_pOwner->m_pWindow = nullptr;
    delete this;
}

LRESULT UIHotKeyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_KILLFOCUS) {
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    if(uMsg == WM_NCDESTROY) {
        OnFinalMessage(m_hWnd);
        return 0;
    }
    return DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}

void UIHotKeyWnd::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
{
    HWND hChild = GetWindow(m_hWnd, GW_CHILD);
    if(hChild) ::SendMessageW(hChild, HKM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0L);
}

DWORD UIHotKeyWnd::GetHotKey() const
{
    HWND hChild = GetWindow(m_hWnd, GW_CHILD);
    if(!hChild) return 0;
    return (::SendMessageW(hChild, HKM_GETHOTKEY, 0, 0L));
}

void UIHotKeyWnd::GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
{
    DWORD dw = GetHotKey();
    wVirtualKeyCode = LOBYTE(LOWORD(dw));
    wModifiers = HIBYTE(LOWORD(dw));
}

void UIHotKeyWnd::SetRules(WORD wInvalidComb, WORD wModifiers)
{
    HWND hChild = GetWindow(m_hWnd, GW_CHILD);
    if(hChild) ::SendMessageW(hChild, HKM_SETRULES, wInvalidComb, MAKELPARAM(wModifiers, 0));
}

UIString UIHotKeyWnd::GetKeyName(UINT vk, BOOL fExtended)
{
    UINT nScanCode = ::MapVirtualKeyExW(vk, 0, ::GetKeyboardLayout(0));
    switch(vk) {
    case VK_INSERT: case VK_DELETE: case VK_HOME: case VK_END:
    case VK_NEXT: case VK_PRIOR: case VK_LEFT: case VK_RIGHT:
    case VK_UP: case VK_DOWN:
        nScanCode |= 0x100;
    }
    if(fExtended) nScanCode |= 0x01000000L;

    wchar_t szStr[MAX_PATH] = {0};
    ::GetKeyNameTextW(nScanCode << 16, szStr, MAX_PATH);

    UIString ret;
    int ulen = WideCharToMultiByte(CP_UTF8, 0, szStr, -1, nullptr, 0, nullptr, nullptr);
    if(ulen > 0) {
        char *buf = new char[ulen];
        WideCharToMultiByte(CP_UTF8, 0, szStr, -1, buf, ulen, nullptr, nullptr);
        ret = UIString{buf};
        delete[] buf;
    }
    return ret;
}

UIString UIHotKeyWnd::GetHotKeyName()
{
    UIString strKeyName;
    WORD wCode = 0;
    WORD wModifiers = 0;
    GetHotKey(wCode, wModifiers);
    if(wCode != 0 || wModifiers != 0) {
        if(wModifiers & HOTKEYF_CONTROL) {
            strKeyName += GetKeyName(VK_CONTROL, FALSE);
            strKeyName += UIString{u8" + "};
        }
        if(wModifiers & HOTKEYF_SHIFT) {
            strKeyName += GetKeyName(VK_SHIFT, FALSE);
            strKeyName += UIString{u8" + "};
        }
        if(wModifiers & HOTKEYF_ALT) {
            strKeyName += GetKeyName(VK_MENU, FALSE);
            strKeyName += UIString{u8" + "};
        }
        strKeyName += GetKeyName(wCode, wModifiers & HOTKEYF_EXT);
    }
    return strKeyName;
}

UIHotKey::UIHotKey()
    : m_pWindow(nullptr), m_buttonState(0), m_hotKeybkColor(0xFFFFFFFF),
      m_wVirtualKeyCode(0), m_wModifiers(0)
{
    RECT pad = {4, 3, 4, 3};
    SetTextPadding(pad);
    SetBkColor(0xFFFFFFFF);
}

UIHotKey::~UIHotKey()
{
    if(m_pWindow) {
        delete m_pWindow;
        m_pWindow = nullptr;
    }
}

UIString UIHotKey::GetClass() const { return UIString{u8"HotKeyUI"}; }

LPVOID UIHotKey::GetInterface(const UIString &name)
{
    if(name == u8"HotKeyUI") return static_cast<UIHotKey*>(this);
    return UILabel::GetInterface(name);
}

uint32_t UIHotKey::GetControlFlags() const
{
    if(!IsEnabled()) return UIControl::GetControlFlags();
    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void UIHotKey::SetEnabled(bool bEnable)
{
    UIControl::SetEnabled(bEnable);
    if(!IsEnabled()) m_buttonState = 0;
}

UIString UIHotKey::GetNormalImage() const { return m_diNormal.sDrawString; }
void UIHotKey::SetNormalImage(const UIString &strImage) { m_diNormal.Clear(); m_diNormal.sDrawString = strImage; Invalidate(); }
UIString UIHotKey::GetHotImage() const { return m_diHot.sDrawString; }
void UIHotKey::SetHotImage(const UIString &strImage) { m_diHot.Clear(); m_diHot.sDrawString = strImage; Invalidate(); }
UIString UIHotKey::GetFocusedImage() const { return m_diFocused.sDrawString; }
void UIHotKey::SetFocusedImage(const UIString &strImage) { m_diFocused.Clear(); m_diFocused.sDrawString = strImage; Invalidate(); }
UIString UIHotKey::GetDisabledImage() const { return m_diDisabled.sDrawString; }
void UIHotKey::SetDisabledImage(const UIString &strImage) { m_diDisabled.Clear(); m_diDisabled.sDrawString = strImage; Invalidate(); }

void UIHotKey::SetNativeBkColor(uint32_t dwBkColor) { m_hotKeybkColor = dwBkColor; }
uint32_t UIHotKey::GetNativeBkColor() const { return m_hotKeybkColor; }

void UIHotKey::SetPos(RECT rc, bool bNeedInvalidate)
{
    UILabel::SetPos(rc, bNeedInvalidate);
    if(m_pWindow) {
        RECT rcPos = m_pWindow->CalPos();
        ::SetWindowPos(m_pWindow->GetHWND(), nullptr, rcPos.left, rcPos.top,
            rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void UIHotKey::SetVisible(bool bVisible)
{
    UILabel::SetVisible(bVisible);
    if(!IsVisible() && m_pWindow) m_manager->SetFocus(nullptr);
}

void UIHotKey::SetInternVisible(bool bVisible)
{
    if(!IsVisible() && m_pWindow) m_manager->SetFocus(nullptr);
}

SIZE UIHotKey::EstimateSize(SIZE szAvailable)
{
    if(m_cxyFixed.cy == 0) return SIZE{m_cxyFixed.cx, (long)UIResourceMgr::GetInstance().GetFontHeight(GetFont(), m_manager->GetPaintDC()) + 6};
    return UILabel::EstimateSize(szAvailable);
}

void UIHotKey::DoEvent(TEventUI &event)
{
    if(!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if(m_parent) m_parent->DoEvent(event);
        else UILabel::DoEvent(event);
        return;
    }

    if(event.Type == UIEVENT_SETCURSOR && IsEnabled()) {
        ::SetCursor(::LoadCursor(nullptr, IDC_IBEAM));
        return;
    }
    if(event.Type == UIEVENT_SETFOCUS && IsEnabled()) {
        if(!m_pWindow) {
            m_pWindow = new UIHotKeyWnd();
            m_pWindow->Init(this);
        }
        Invalidate();
    }
    if(event.Type == UIEVENT_KILLFOCUS && IsEnabled()) {
        Invalidate();
    }
    if(event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) {
        if(IsEnabled()) {
            if(IsFocused() && !m_pWindow) {
                m_pWindow = new UIHotKeyWnd();
                m_pWindow->Init(this);
            }
        }
        return;
    }
    if(event.Type == UIEVENT_MOUSEENTER) {
        if(IsEnabled()) { m_buttonState |= UISTATE_HOT; Invalidate(); }
        return;
    }
    if(event.Type == UIEVENT_MOUSELEAVE) {
        if(IsEnabled()) { m_buttonState &= ~UISTATE_HOT; Invalidate(); }
        return;
    }
    UILabel::DoEvent(event);
}

void UIHotKey::SetAttribute(const char *pstrName, const char *pstrValue)
{
    if(strcasecmp(pstrName, "normalimage") == 0) SetNormalImage(UIString{pstrValue});
    else if(strcasecmp(pstrName, "hotimage") == 0) SetHotImage(UIString{pstrValue});
    else if(strcasecmp(pstrName, "focusedimage") == 0) SetFocusedImage(UIString{pstrValue});
    else if(strcasecmp(pstrName, "disabledimage") == 0) SetDisabledImage(UIString{pstrValue});
    else if(strcasecmp(pstrName, "nativebkcolor") == 0) {
        if(*pstrValue == '#') pstrValue = CharNext(pstrValue);
        char *pstr = nullptr;
        uint32_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetNativeBkColor(clrColor);
    }
    else UILabel::SetAttribute(pstrName, pstrValue);
}

void UIHotKey::PaintStatusImage(HANDLE_DC hDC)
{
    if(IsFocused()) m_buttonState |= UISTATE_FOCUSED;
    else m_buttonState &= ~UISTATE_FOCUSED;
    if(!IsEnabled()) m_buttonState |= UISTATE_DISABLED;
    else m_buttonState &= ~UISTATE_DISABLED;

    if((m_buttonState & UISTATE_DISABLED) != 0) {
        if(!m_diDisabled.sDrawString.IsEmpty()) { if(DrawImage(hDC, m_diDisabled)) return; }
    }
    else if((m_buttonState & UISTATE_FOCUSED) != 0) {
        if(!m_diFocused.sDrawString.IsEmpty()) { if(DrawImage(hDC, m_diFocused)) return; }
    }
    else if((m_buttonState & UISTATE_HOT) != 0) {
        if(!m_diHot.sDrawString.IsEmpty()) { if(DrawImage(hDC, m_diHot)) return; }
    }
    if(!m_diNormal.sDrawString.IsEmpty()) { DrawImage(hDC, m_diNormal); }
}

void UIHotKey::PaintText(HANDLE_DC hDC)
{
    if(m_textColor == 0) m_textColor = m_manager->GetDefaultFontColor();
    if(m_disabledTextColor == 0) m_disabledTextColor = m_manager->GetDefaultDisabledColor();
    if(m_text.IsEmpty()) return;

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

    uint32_t clrColor = IsEnabled() ? m_textColor : m_disabledTextColor;
    UIRenderEngine::DrawText(hDC, rc, m_text, clrColor, m_fontId, DT_SINGLELINE | m_textStyle);
}

DWORD UIHotKey::GetHotKey() const { return (MAKEWORD(m_wVirtualKeyCode, m_wModifiers)); }
void UIHotKey::GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
{
    wVirtualKeyCode = m_wVirtualKeyCode;
    wModifiers = m_wModifiers;
}

void UIHotKey::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
{
    m_wVirtualKeyCode = wVirtualKeyCode;
    m_wModifiers = wModifiers;
    if(m_pWindow) return;
    m_pWindow = new UIHotKeyWnd();
    m_pWindow->Init(this);
    Invalidate();
}

#endif // WIN32