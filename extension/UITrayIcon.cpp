#include "UITrayIcon.h"

#ifdef WIN32
#include <cstdio>

UITrayIcon::UITrayIcon()
{
    memset(&m_trayData, 0, sizeof(m_trayData));
    m_bEnabled = false;
    m_bVisible = false;
    m_hWnd = nullptr;
    m_hIcon = nullptr;
    m_uMessage = UI_EXT_TRAYICON_MSG;
}

UITrayIcon::~UITrayIcon()
{
    DeleteTrayIcon();
}

void UITrayIcon::CreateTrayIcon(HWND recvHwnd, UINT iconIDResource, const UIString &toolTipText, UINT message)
{
    if(!recvHwnd || iconIDResource <= 0) {
        return;
    }
    if(message != 0) m_uMessage = message;
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(recvHwnd, GWLP_HINSTANCE);
    m_hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(iconIDResource));
    m_trayData.cbSize = sizeof(NOTIFYICONDATAW);
    m_trayData.hWnd   = recvHwnd;
    m_trayData.uID    = iconIDResource;
    m_trayData.hIcon  = m_hIcon;
    m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_trayData.uCallbackMessage = m_uMessage;
    if(!toolTipText.IsEmpty()) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, toolTipText.GetData(), -1, nullptr, 0);
        if(wlen > 0 && wlen <= 128) {
            MultiByteToWideChar(CP_UTF8, 0, toolTipText.GetData(), -1, m_trayData.szTip, wlen);
        }
    }
    Shell_NotifyIconW(NIM_ADD, &m_trayData);
    m_bEnabled = true;
}

void UITrayIcon::DeleteTrayIcon()
{
    if(!m_bEnabled) return;
    Shell_NotifyIconW(NIM_DELETE, &m_trayData);
    m_bEnabled = false;
    m_bVisible = false;
    m_hWnd = nullptr;
    m_uMessage = UI_EXT_TRAYICON_MSG;
}

bool UITrayIcon::SetTooltipText(const UIString &toolTipText)
{
    if(!toolTipText.IsEmpty()) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, toolTipText.GetData(), -1, nullptr, 0);
        if(wlen > 0 && wlen <= 128) {
            MultiByteToWideChar(CP_UTF8, 0, toolTipText.GetData(), -1, m_trayData.szTip, wlen);
        }
    }
    if(!m_bEnabled) return false;
    m_trayData.uFlags = NIF_TIP;
    return Shell_NotifyIconW(NIM_MODIFY, &m_trayData) == TRUE;
}

bool UITrayIcon::SetTooltipText(UINT idResource)
{
    wchar_t mbuf[256] = {0};
    LoadStringW(GetModuleHandle(nullptr), idResource, mbuf, 256);
    UIString text;
    int ulen = WideCharToMultiByte(CP_UTF8, 0, mbuf, -1, nullptr, 0, nullptr, nullptr);
    if(ulen > 0) {
        char *buf = new char[ulen];
        WideCharToMultiByte(CP_UTF8, 0, mbuf, -1, buf, ulen, nullptr, nullptr);
        text = UIString{buf};
        delete[] buf;
    }
    return SetTooltipText(text);
}

UIString UITrayIcon::GetTooltipText() const
{
    if(m_trayData.szTip[0] == 0) return UIString{""};
    int ulen = WideCharToMultiByte(CP_UTF8, 0, m_trayData.szTip, -1, nullptr, 0, nullptr, nullptr);
    if(ulen <= 0) return UIString{""};
    char *buf = new char[ulen];
    WideCharToMultiByte(CP_UTF8, 0, m_trayData.szTip, -1, buf, ulen, nullptr, nullptr);
    UIString ret = UIString{buf};
    delete[] buf;
    return ret;
}

bool UITrayIcon::SetIcon(HICON hIcon)
{
    if(hIcon) m_hIcon = hIcon;
    m_trayData.uFlags = NIF_ICON;
    m_trayData.hIcon = hIcon;
    if(!m_bEnabled) return false;
    return Shell_NotifyIconW(NIM_MODIFY, &m_trayData) == TRUE;
}

bool UITrayIcon::SetIcon(const UIString &iconFile)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, iconFile.GetData(), -1, nullptr, 0);
    if(wlen <= 0) return false;
    wchar_t *wbuf = new wchar_t[wlen];
    MultiByteToWideChar(CP_UTF8, 0, iconFile.GetData(), -1, wbuf, wlen);
    HICON hIcon = (HICON)LoadImageW(nullptr, wbuf, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    delete[] wbuf;
    if(!hIcon) return false;
    return SetIcon(hIcon);
}

bool UITrayIcon::SetIcon(UINT idResource)
{
    HINSTANCE hInst = (HINSTANCE)GetModuleHandle(nullptr);
    HICON hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(idResource));
    return SetIcon(hIcon);
}

HICON UITrayIcon::GetIcon() const
{
    return m_trayData.hIcon;
}

void UITrayIcon::SetHideIcon()
{
    if(IsVisible()) {
        SetIcon((HICON)nullptr);
        m_bVisible = true;
    }
}

void UITrayIcon::SetShowIcon()
{
    if(!IsVisible()) {
        SetIcon(m_hIcon);
        m_bVisible = false;
    }
}

void UITrayIcon::RemoveIcon()
{
    m_trayData.uFlags = 0;
    Shell_NotifyIconW(NIM_DELETE, &m_trayData);
    m_bEnabled = false;
}

#endif // WIN32