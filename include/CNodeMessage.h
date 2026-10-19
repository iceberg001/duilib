#ifndef DIRECTUI_CNODEMESSAGE_H
#define DIRECTUI_CNODEMESSAGE_H

#include "UIString.h"
#include "UIDefine.h"
#include <cstddef>

class CNodeUIBase;

struct CNodeMessage
{
    UIString        msgType;
    UIString        msgData;
    void *          dataPtr_;
    size_t          dataSize_;
    CNodeUIBase *   sender_;
    WPARAM          wParam;
    LPARAM          lParam;

    CNodeMessage()
        : dataPtr_(nullptr), dataSize_(0)
        , sender_(nullptr), wParam(0), lParam(0) {}

    CNodeMessage(const UIString & type,
                 const UIString & data = UIString{""},
                 CNodeUIBase *    sender = nullptr,
                 WPARAM           w = 0,
                 LPARAM           l = 0)
        : msgType(type), msgData(data)
        , dataPtr_(nullptr), dataSize_(0)
        , sender_(sender), wParam(w), lParam(l) {}

    CNodeMessage(const UIString & type,
                 void *          data,
                 size_t          size,
                 CNodeUIBase *   sender = nullptr,
                 WPARAM          w = 0,
                 LPARAM          l = 0)
        : msgType(type)
        , dataPtr_(data), dataSize_(size)
        , sender_(sender), wParam(w), lParam(l) {}

    template <typename T>
    T * GetDataAs() const
    {
        return static_cast<T *>(dataPtr_);
    }
};

#endif
