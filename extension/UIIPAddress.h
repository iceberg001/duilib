#ifndef DUILIB_EXT_UIIPADDRESS_H
#define DUILIB_EXT_UIIPADDRESS_H

#ifdef WIN32

#include "UILabel.h"
#include <windows.h>
#include <commctrl.h>

class UIIPAddress;

class UIIPAddressWnd
{
public:
    UIIPAddressWnd();
    ~UIIPAddressWnd();

    void Init(UIIPAddress *pOwner);
    RECT CalPos();
    void OnFinalMessage(HWND hWnd);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND GetHWND() const { return m_hWnd; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    UIIPAddress *m_pOwner;
    HWND      m_hWnd;
    HBRUSH    m_hBkBrush;
    bool      m_bInit;
};

class UIIPAddress : public UILabel
{
    friend class UIIPAddressWnd;
public:
    UIIPAddress();
    ~UIIPAddress();

    UIString GetClass() const override;
    LPVOID GetInterface(const UIString &name) override;

    DWORD GetIP();
    void SetIP(DWORD dwIP);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    void DoEvent(TEventUI &event) override;
    void SetAttribute(const char *pstrName, const char *pstrValue) override;

protected:
    DWORD           m_dwIP;
    bool            m_bReadOnly;
    int             m_nIPUpdateFlag;
    UIIPAddressWnd *m_pWindow;
};

#endif // WIN32
#endif // DUILIB_EXT_UIIPADDRESS_H