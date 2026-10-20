#ifndef DUILIB_EXT_UIHOTKEY_H
#define DUILIB_EXT_UIHOTKEY_H

#ifdef WIN32
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif
#include "UILabel.h"
#include "UIButton.h"
#include <windows.h>
#include <commctrl.h>

class UIHotKey;

class UIHotKeyWnd
{
public:
    UIHotKeyWnd();
    ~UIHotKeyWnd();

    void Init(UIHotKey *pOwner);
    RECT CalPos();
    void OnFinalMessage(HWND hWnd);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
    void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
    DWORD GetHotKey() const;
    UIString GetHotKeyName();
    void SetRules(WORD wInvalidComb, WORD wModifiers);
    UIString GetKeyName(UINT vk, BOOL fExtended);

    HWND GetHWND() const { return m_hWnd; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    UIHotKey *m_pOwner;
    HWND      m_hWnd;
    HBRUSH    m_hBkBrush;
    bool      m_bInit;
    WNDPROC   m_oldWndProc;
};

class UIHotKey : public UILabel
{
    friend class UIHotKeyWnd;
public:
    UIHotKey();
    ~UIHotKey();

    UIString GetClass() const override;
    LPVOID GetInterface(const UIString &name) override;
    uint32_t GetControlFlags() const override;
    void SetEnabled(bool bEnable = true) override;

    UIString GetNormalImage() const;
    void SetNormalImage(const UIString &strImage);
    UIString GetHotImage() const;
    void SetHotImage(const UIString &strImage);
    UIString GetFocusedImage() const;
    void SetFocusedImage(const UIString &strImage);
    UIString GetDisabledImage() const;
    void SetDisabledImage(const UIString &strImage);
    void SetNativeBkColor(uint32_t dwBkColor);
    uint32_t GetNativeBkColor() const;

    void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    void SetVisible(bool bVisible = true) override;
    void SetInternVisible(bool bVisible = true) override;
    SIZE EstimateSize(SIZE szAvailable) override;
    void DoEvent(TEventUI &event) override;
    void SetAttribute(const char *pstrName, const char *pstrValue) override;

    void PaintStatusImage(HANDLE_DC hDC) override;
    void PaintText(HANDLE_DC hDC) override;

    void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
    DWORD GetHotKey() const;
    void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

protected:
    UIHotKeyWnd *m_pWindow;
    uint32_t     m_buttonState;
    TDrawInfo    m_diNormal;
    TDrawInfo    m_diHot;
    TDrawInfo    m_diFocused;
    TDrawInfo    m_diDisabled;
    uint32_t     m_hotKeybkColor;

    WORD m_wVirtualKeyCode;
    WORD m_wModifiers;
};

#endif // WIN32
#endif // DUILIB_EXT_UIHOTKEY_H