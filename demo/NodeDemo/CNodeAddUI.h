#ifndef DIRECTUI_CNODEADDUI_H
#define DIRECTUI_CNODEADDUI_H

#include "CNodeUIBase.h"
#include "UIEdit.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIHorizontalLayout.h"

#define NODE_MSG_ADD_RESULT  "add_result"

class CNodeAddUI : public CNodeUIBase
{
public:
    CNodeAddUI();

    UIString GetClass() const override;

private:
    bool OnButtonNotify(void * param);

    UIEdit *            editA_;
    UIEdit *            editB_;
    UIButton *          btnCalc_;
    UILabel *           lblPlus_;
};

#endif