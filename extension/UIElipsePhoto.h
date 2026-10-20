#ifndef DUILIB_EXT_UIELIPSEPHOTO_H
#define DUILIB_EXT_UIELIPSEPHOTO_H

#ifdef WIN32
#include "UIControl.h"
#include <windows.h>
#include <gdiplus.h>

class UIElipsePhoto : public UIControl
{
public:
    UIElipsePhoto();
    virtual ~UIElipsePhoto();

    void SetAttribute(const char *pstrName, const char *pstrValue) override;
    void SetBkImage(const UIString &strImage);
    void PaintBkImage(HANDLE_DC hDC) override;

private:
    Gdiplus::Bitmap* m_bkGDIPimage;
};

#endif // WIN32
#endif // DUILIB_EXT_UIELIPSEPHOTO_H