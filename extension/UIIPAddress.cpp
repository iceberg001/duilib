#include "UIIPAddress.h"

#ifdef WIN32
#include "UIPaintManager.h"
#include "UIRenderEngine.h"
#include "UIResourceMgr.h"
#include "UIFont.h"

#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

static DWORD GetLocalIpAddress()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    if(WSAStartup(wVersionRequested, &wsaData) != 0) return 0;
    char local[255] = {0};
    gethostname(local, sizeof(local));
    hostent* ph = gethostbyname(local);
    if(ph == nullptr) return 0;
    in_addr addr;
    memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));
    DWORD dwIP = MAKEIPADDRESS(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
    return dwIP;
}

#define IP_NONE   0
#define IP_UPDATE 1
#define IP_DELETE 2
#define IP_KEEP   3

static const wchar_t* g_IPAddrWndClass = L"DuiLibExtIPAddrWnd";
static bool g_IPAddrWndClassRegistered = false;

static void RegisterIPAddrWndClass()
{
    if(g_IPAddrWndClassRegistered) return;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = UIIPAddressWnd::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = g_IPAddrWndClass;
    RegisterClassW(&wc);
    g_IPAddrWndClassRegistered = true;
}

UIIPAddressWnd::UIIPAddressWnd()
    : m_pOwner(nullptr), m_hWnd(nullptr), m_hBkBrush(nullptr), m_bInit(false)
{
}

UIIPAddressWnd::~UIIPAddressWnd()
{
    if(m_hWnd && IsWindow(m_hWnd)) DestroyWindow(m_hWnd);
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
}

LRESULT CALLBACK UIIPAddressWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UIIPAddressWnd *pThis = (UIIPAddressWnd*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if(pThis) return pThis->HandleMessage(uMsg, wParam, lParam);
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void UIIPAddressWnd::Init(UIIPAddress *pOwner)
{
    m_pOwner = pOwner;
    m_pOwner->m_nIPUpdateFlag = IP_NONE;

    RegisterIPAddrWndClass();

    INITCOMMONCONTROLSEX CommCtrl;
    CommCtrl.dwSize = sizeof(CommCtrl);
    CommCtrl.dwICC = ICC_INTERNET_CLASSES;
    InitCommonControlsEx(&CommCtrl);

    RECT rcPos = CalPos();
    m_hWnd = CreateWindowExW(0, g_IPAddrWndClass, L"", WS_CHILD | WS_VISIBLE,
        rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_pOwner->GetManager()->GetPaintWindow(), nullptr, GetModuleHandle(nullptr), nullptr);
    if(!m_hWnd) return;

    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    HWND hIP = CreateWindowExW(0, WC_IPADDRESSW, L"", WS_CHILD | WS_VISIBLE,
        0, 0, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
        m_hWnd, nullptr, GetModuleHandle(nullptr), nullptr);
    if(hIP) {
        ::SendMessageW(hIP, IPM_SETADDRESS, 0, m_pOwner->m_dwIP);
    }

    ::SetFocus(m_hWnd);
    m_bInit = true;
}

RECT UIIPAddressWnd::CalPos() { return m_pOwner->GetPos(); }

void UIIPAddressWnd::OnFinalMessage(HWND /*hWnd*/)
{
    if(m_hBkBrush) ::DeleteObject(m_hBkBrush);
    m_pOwner->m_pWindow = nullptr;
    delete this;
}

LRESULT UIIPAddressWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_KILLFOCUS) {
        if(m_pOwner->m_nIPUpdateFlag == IP_NONE) {
            HWND hChild = GetWindow(m_hWnd, GW_CHILD);
            if(hChild) ::SendMessageW(hChild, IPM_GETADDRESS, 0, (LPARAM)&m_pOwner->m_dwIP);
            m_pOwner->m_nIPUpdateFlag = IP_UPDATE;
            m_pOwner->UpdateText();
        }
        ::ShowWindow(m_hWnd, SW_HIDE);
        return 0;
    }
    if(uMsg == WM_NCDESTROY) {
        OnFinalMessage(m_hWnd);
        return 0;
    }
    return DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}

UIIPAddress::UIIPAddress()
    : m_dwIP(GetLocalIpAddress()), m_bReadOnly(false), m_pWindow(nullptr), m_nIPUpdateFlag(IP_UPDATE)
{
    UpdateText();
    m_nIPUpdateFlag = IP_NONE;
}

UIIPAddress::~UIIPAddress()
{
    if(m_pWindow) { delete m_pWindow; m_pWindow = nullptr; }
}

UIString UIIPAddress::GetClass() const { return UIString{u8"IPAddressUI"}; }

LPVOID UIIPAddress::GetInterface(const UIString &name)
{
    if(name == u8"IPAddressUI") return static_cast<UIIPAddress*>(this);
    return UILabel::GetInterface(name);
}

DWORD UIIPAddress::GetIP() { return m_dwIP; }
void UIIPAddress::SetIP(DWORD dwIP) { m_dwIP = dwIP; UpdateText(); }
void UIIPAddress::SetReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; Invalidate(); }
bool UIIPAddress::IsReadOnly() const { return m_bReadOnly; }

void UIIPAddress::UpdateText()
{
    if(m_nIPUpdateFlag == IP_DELETE) {
        SetText(UIString{u8""});
    }
    else if(m_nIPUpdateFlag == IP_UPDATE) {
        in_addr addr;
        addr.S_un.S_addr = m_dwIP;
        char szIP[MAX_PATH] = {0};
        snprintf(szIP, sizeof(szIP), "%d.%d.%d.%d",
            addr.S_un.S_un_b.s_b4, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b1);
        SetText(UIString{szIP});
    }
}

void UIIPAddress::DoEvent(TEventUI &event)
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
            m_pWindow = new UIIPAddressWnd();
            m_pWindow->Init(this);
        }
    }
    if(event.Type == UIEVENT_KILLFOCUS && IsEnabled()) {
        Invalidate();
    }
    if(event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) {
        if(IsEnabled()) {
            if(IsFocused() && !m_pWindow) {
                m_pWindow = new UIIPAddressWnd();
                m_pWindow->Init(this);
            }
        }
        return;
    }
    UILabel::DoEvent(event);
}

void UIIPAddress::SetAttribute(const char *pstrName, const char *pstrValue)
{
    UILabel::SetAttribute(pstrName, pstrValue);
}

#endif // WIN32