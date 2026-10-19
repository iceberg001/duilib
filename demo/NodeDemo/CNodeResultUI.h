#ifndef DIRECTUI_CNODERESULTUI_H
#define DIRECTUI_CNODERESULTUI_H

#include "CNodeUIBase.h"
#include "UIEdit.h"
#include "UILabel.h"
#include "UIHorizontalLayout.h"

class CNodeResultUI : public CNodeUIBase
{
public:
    CNodeResultUI();

    UIString GetClass() const override;

    bool WantNodeMessage() const override { return true; }

    void OnNodeMessage(const CNodeMessage & msg) override;

private:
    UILabel *   lblResult_;
    UIEdit *    editResult_;
};

#endif