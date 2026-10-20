#include "UIElipsePhoto.h"

#ifdef WIN32
#include "UIResourceMgr.h"
#include "UIPaintManager.h"

static Gdiplus::Bitmap* CreateBitmapFromHBITMAP(HBITMAP hBitmap)
{
    BITMAP bmp = {0};
    if(0 == GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bmp)) {
        return nullptr;
    }

    BYTE *piexlsSrc = nullptr;
    LONG cbSize = bmp.bmWidthBytes * bmp.bmHeight;
    piexlsSrc = new BYTE[cbSize];

    BITMAPINFO bmpInfo = {0};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
    bmpInfo.bmiHeader.biHeight = bmp.bmHeight;
    bmpInfo.bmiHeader.biPlanes = bmp.bmPlanes;
    bmpInfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = CreateDCW(L"DISPLAY", nullptr, nullptr, nullptr);
    LONG cbCopied = GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight,
        piexlsSrc, &bmpInfo, DIB_RGB_COLORS);
    DeleteDC(hdcScreen);
    if(0 == cbCopied) {
        delete[] piexlsSrc;
        return nullptr;
    }

    Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat32bppPARGB);

    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
    if(Gdiplus::Ok != pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead,
        PixelFormat32bppPARGB, &bitmapData)) {
        if(pBitmap) {
            delete pBitmap;
            pBitmap = nullptr;
        }
        delete[] piexlsSrc;
        return nullptr;
    }

    BYTE *pixelsDest = (BYTE*)bitmapData.Scan0;
    int nLinesize = bmp.bmWidth * sizeof(UINT);
    int nHeight = bmp.bmHeight;

    for(int y = 0; y < nHeight; y++) {
        memcpy_s(
            (pixelsDest + y * nLinesize),
            nLinesize,
            (piexlsSrc + (nHeight - y - 1) * nLinesize),
            nLinesize);
    }

    if(Gdiplus::Ok != pBitmap->UnlockBits(&bitmapData)) {
        delete pBitmap;
        pBitmap = nullptr;
    }

    delete[] piexlsSrc;
    return pBitmap;
}

UIElipsePhoto::UIElipsePhoto()
    : m_bkGDIPimage(nullptr)
{
}

UIElipsePhoto::~UIElipsePhoto()
{
    if(m_bkGDIPimage) {
        delete m_bkGDIPimage;
        m_bkGDIPimage = nullptr;
    }
}

void UIElipsePhoto::SetAttribute(const char *pstrName, const char *pstrValue)
{
    if(strcasecmp(pstrName, "bkimage") == 0) {
        SetBkImage(UIString{pstrValue});
    }
    else {
        UIControl::SetAttribute(pstrName, pstrValue);
    }
}

void UIElipsePhoto::SetBkImage(const UIString &strImage)
{
    UIControl::SetBkImage(strImage);

    TImageInfo* data = UIResourceMgr::GetInstance().GetImage(strImage);
    if(data && data->hBitmap) {
        if(m_bkGDIPimage) {
            delete m_bkGDIPimage;
            m_bkGDIPimage = nullptr;
        }
        m_bkGDIPimage = CreateBitmapFromHBITMAP(data->hBitmap);
    }
}

void UIElipsePhoto::PaintBkImage(HANDLE_DC hDC)
{
    if(!m_bkGDIPimage) return;

    int nWidth = this->GetWidth();
    int nHeight = this->GetHeight();
    RECT rt = this->GetPos();

    Gdiplus::Bitmap bitmapTmp(m_bkGDIPimage->GetWidth() + 2, m_bkGDIPimage->GetHeight() + 2);
    Gdiplus::Graphics gBig(&bitmapTmp);
    Gdiplus::Graphics g(hDC);
    gBig.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    path.AddEllipse(1, 1, m_bkGDIPimage->GetWidth(), m_bkGDIPimage->GetHeight());
    gBig.FillPath(&Gdiplus::TextureBrush(m_bkGDIPimage), &path);
    Gdiplus::SmoothingMode mode = g.GetSmoothingMode();
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.DrawImage(&bitmapTmp, rt.left, rt.top, nWidth, nHeight);
    g.SetSmoothingMode(mode);
}

#endif // WIN32