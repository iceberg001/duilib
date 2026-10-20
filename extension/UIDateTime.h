#ifndef DUILIB_EXT_UIDATETIME_H
#define DUILIB_EXT_UIDATETIME_H

#ifdef WIN32
#include "UILabel.h"
#include <windows.h>
#include <commctrl.h>

class UIDateTime;

class UIDateTimeWnd
{
public:
    UIDateTimeWnd();
    ~UIDateTimeWnd();

    void Init(UIDateTime *pOwner);
    RECT CalPos();
    void OnFinalMessage(HWND hWnd);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND GetHWND() const { return m_hWnd; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    UIDateTime *m_pOwner;
    HWND      m_hWnd;
    HBRUSH    m_hBkBrush;
    bool      m_bInit;
    bool      m_bDropOpen;
    SYSTEMTIME m_oldSysTime;
};

class UIDateTime : public UILabel
{
    friend class UIDateTimeWnd;
public:
    UIDateTime();
    ~UIDateTime();

    UIString GetClass() const override;
    LPVOID GetInterface(const UIString &name) override;

    SYSTEMTIME& GetTime();
    void SetTime(SYSTEMTIME *pst);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    void DoEvent(TEventUI &event) override;

protected:
    SYSTEMTIME    m_sysTime;
    int           m_nDTUpdateFlag;
    bool          m_bReadOnly;
    UIDateTimeWnd *m_pWindow;
};

#endif // WIN32
#endif // DUILIB_EXT_UIDATETIME_H