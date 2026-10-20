#include "UIDateTime.h"

#ifdef WIN32
#include "UIPaintManager.h"
#include "UIRenderEngine.h"
#include "UIResourceMgr.h"
#include "UIFont.h"
#include <cstdio>

#define DT_NONE   0
#define DT_UPDATE 1
#define DT_DELETE 2
#define DT_KEEP   3

static const wchar_t* g_DateTimeWndClass = L"DuiLibExtDateTimeWnd";
static bool g_DateTimeWndClassRegistered = false;

static void RegisterDateTimeWndClass()
{
    if(g_DateTimeWndClassRegistered) return;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = UIDateTimeWnd::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = g_DateTimeWndClass;
    RegisterClassW(&wc);
    g_DateTimeWndClassRegistered = true;
}

UIDateTimeWnd::UIDateTimeWnd()
    : m_pOwner(nullptr), m_hWnd(nullptr), m_hBkBrush(nullptr), m_bInit(false), m_bDropOpen(false)
{
    memset(&m_oldSysTime, 0, sizeof(m_oldSysTime));
}

UIDateTimeWnd::~UIDateTimeWnd()
{
    if(m_hWnd && IsWindow(m_hWnd)) DestroyWindow(m_hWnd);
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
}

LRESULT CALLBACK UIDateTimeWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UIDateTimeWnd *pThis = (UIDateTimeWnd*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if(pThis) return pThis->HandleMessage(uMsg, wParam, lParam);
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void UIDateTimeWnd::Init(UIDateTime *pOwner)
{
    m_pOwner = pOwner;
    m_pOwner->m_nDTUpdateFlag = DT_NONE;

    RegisterDateTimeWndClass();

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_DATE_CLASSES;
    InitCommonControlsEx(&icc);

    RECT rcPos = CalPos();
    m_hWnd = CreateWindowExW(0, g_DateTimeWndClass, L"", WS_CHILD,
        rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_pOwner->GetManager()->GetPaintWindow(), nullptr, GetModuleHandle(nullptr), nullptr);
    if(!m_hWnd) return;

    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    HWND hDTP = CreateWindowExW(0, DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE,
        0, 0, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_hWnd, nullptr, GetModuleHandle(nullptr), nullptr);
    if(hDTP) {
        if(m_pOwner->GetText().IsEmpty()) {
            ::GetLocalTime(&m_pOwner->m_sysTime);
        }
        memcpy(&m_oldSysTime, &m_pOwner->m_sysTime, sizeof(SYSTEMTIME));
        ::SendMessageW(hDTP, DTM_SETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
    }

    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);
    m_bInit = true;
}

RECT UIDateTimeWnd::CalPos()
{
    RECT rcPos = m_pOwner->GetPos();
    return rcPos;
}

void UIDateTimeWnd::OnFinalMessage(HWND /*hWnd*/)
{
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
    m_pOwner->m_pWindow = nullptr;
    delete this;
}

LRESULT UIDateTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        memcpy(&m_pOwner->m_sysTime, &m_oldSysTime, sizeof(SYSTEMTIME));
        m_pOwner->m_nDTUpdateFlag = DT_UPDATE;
        m_pOwner->UpdateText();
        PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    if(uMsg == WM_NOTIFY) {
        NMHDR* pHeader = (NMHDR*)lParam;
        if(pHeader) {
            if(pHeader->code == DTN_DATETIMECHANGE) {
                HWND hChild = GetWindow(m_hWnd, GW_CHILD);
                if(hChild) ::SendMessageW(hChild, DTM_GETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
                m_pOwner->m_nDTUpdateFlag = DT_UPDATE;
                m_pOwner->UpdateText();
            }
            else if(pHeader->code == DTN_DROPDOWN) {
                m_bDropOpen = true;
            }
            else if(pHeader->code == DTN_CLOSEUP) {
                HWND hChild = GetWindow(m_hWnd, GW_CHILD);
                if(hChild) ::SendMessageW(hChild, DTM_GETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
                m_pOwner->m_nDTUpdateFlag = DT_UPDATE;
                m_pOwner->UpdateText();
                PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
                m_bDropOpen = false;
            }
        }
        return 0;
    }
    if(uMsg == WM_KILLFOCUS) {
        if(!m_bDropOpen) PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    if(uMsg == WM_NCDESTROY) {
        OnFinalMessage(m_hWnd);
        return 0;
    }
    return DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}

UIDateTime::UIDateTime()
    : m_bReadOnly(false), m_pWindow(nullptr), m_nDTUpdateFlag(DT_UPDATE)
{
    ::GetLocalTime(&m_sysTime);
    UpdateText();
    m_nDTUpdateFlag = DT_NONE;
}

UIDateTime::~UIDateTime()
{
    if(m_pWindow) { delete m_pWindow; m_pWindow = nullptr; }
}

UIString UIDateTime::GetClass() const { return UIString{u8"DateTimeUI"}; }

LPVOID UIDateTime::GetInterface(const UIString &name)
{
    if(name == u8"DateTimeUI") return static_cast<UIDateTime*>(this);
    return UILabel::GetInterface(name);
}

SYSTEMTIME& UIDateTime::GetTime() { return m_sysTime; }

void UIDateTime::SetTime(SYSTEMTIME *pst)
{
    m_sysTime = *pst;
    Invalidate();
    m_nDTUpdateFlag = DT_UPDATE;
    UpdateText();
    m_nDTUpdateFlag = DT_NONE;
}

void UIDateTime::SetReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; Invalidate(); }
bool UIDateTime::IsReadOnly() const { return m_bReadOnly; }

void UIDateTime::UpdateText()
{
    if(m_nDTUpdateFlag == DT_DELETE) {
        SetText(UIString{u8""});
    }
    else if(m_nDTUpdateFlag == DT_UPDATE) {
        char sText[64] = {0};
        snprintf(sText, sizeof(sText), "%04d-%02d-%02d", m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
        SetText(UIString{sText});
    }
}

void UIDateTime::DoEvent(TEventUI &event)
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
            m_pWindow = new UIDateTimeWnd();
            m_pWindow->Init(this);
        }
    }
    if(event.Type == UIEVENT_KILLFOCUS && IsEnabled()) {
        Invalidate();
    }
    if(event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) {
        if(IsEnabled()) {
            if(IsFocused() && !m_pWindow) {
                m_pWindow = new UIDateTimeWnd();
                m_pWindow->Init(this);
            }
        }
        return;
    }
    UILabel::DoEvent(event);
}

#endif // WIN32