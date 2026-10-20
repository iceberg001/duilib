#ifndef DUILIB_EXT_UITRAYICON_H
#define DUILIB_EXT_UITRAYICON_H

#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#include "UIString.h"

#define UI_EXT_TRAYICON_MSG (WM_APP + 100)

class UITrayIcon
{
public:
    UITrayIcon();
    ~UITrayIcon();

    void CreateTrayIcon(HWND recvHwnd, UINT iconIDResource, const UIString &toolTipText = UIString{""}, UINT message = 0);
    void DeleteTrayIcon();

    bool SetTooltipText(const UIString &toolTipText);
    bool SetTooltipText(UINT idResource);
    UIString GetTooltipText() const;

    bool SetIcon(HICON hIcon);
    bool SetIcon(const UIString &iconFile);
    bool SetIcon(UINT idResource);
    HICON GetIcon() const;

    void SetHideIcon();
    void SetShowIcon();
    void RemoveIcon();

    bool Enabled() const { return m_bEnabled; }
    bool IsVisible() const { return !m_bVisible; }

private:
    bool m_bEnabled;
    bool m_bVisible;
    HWND  m_hWnd;
    UINT  m_uMessage;
    HICON m_hIcon;
    NOTIFYICONDATAW m_trayData;
};

#endif // WIN32
#endif // DUILIB_EXT_UITRAYICON_H