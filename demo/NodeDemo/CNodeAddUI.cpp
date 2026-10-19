#include "CNodeAddUI.h"
#include "UIDelegate.h"
#include <cstdlib>
#include <cstdio>

CNodeAddUI::CNodeAddUI()
    : editA_(nullptr)
    , editB_(nullptr)
    , btnCalc_(nullptr)
    , lblPlus_(nullptr)
{
    SetNodeName(UIString{u8"NodeAdd"});

    auto * layout = new UIHorizontalLayout;
    layout->SetInset({10, 10, 10, 10});
    layout->SetChildPadding(8);
    layout->SetBkColor(0xFFE8F5E9);
    layout->SetAttribute("border", "1");
    layout->SetAttribute("bordercolor", "#FF4CAF50");

    editA_ = new UIEdit;
    editA_->SetName(UIString{u8"editA"});
    editA_->SetAttribute("width", "80");
    editA_->SetAttribute("height", "28");
    editA_->SetAttribute("bkcolor", "#FFFFFFFF");
    editA_->SetAttribute("border", "1");
    editA_->SetAttribute("bordercolor", "#FF999999");
    editA_->SetNumberOnly(true);
    editA_->SetText(UIString{u8"0"});
    layout->Add(editA_);

    lblPlus_ = new UILabel;
    lblPlus_->SetAttribute("width", "20");
    lblPlus_->SetAttribute("height", "28");
    lblPlus_->SetText(UIString{u8"+"});
    lblPlus_->SetAttribute("align", "center");
    layout->Add(lblPlus_);

    editB_ = new UIEdit;
    editB_->SetName(UIString{u8"editB"});
    editB_->SetAttribute("width", "80");
    editB_->SetAttribute("height", "28");
    editB_->SetAttribute("bkcolor", "#FFFFFFFF");
    editB_->SetAttribute("border", "1");
    editB_->SetAttribute("bordercolor", "#FF999999");
    editB_->SetNumberOnly(true);
    editB_->SetText(UIString{u8"0"});
    layout->Add(editB_);

    btnCalc_ = new UIButton;
    btnCalc_->SetName(UIString{u8"btnCalc"});
    btnCalc_->SetAttribute("width", "50");
    btnCalc_->SetAttribute("height", "28");
    btnCalc_->SetText(UIString{u8"="});
    btnCalc_->SetAttribute("bkcolor", "#FF4CAF50");
    btnCalc_->SetAttribute("align", "center");
    btnCalc_->OnNotify += MakeDelegate(this, &CNodeAddUI::OnButtonNotify);
    layout->Add(btnCalc_);

    this->Add(layout);
}

UIString CNodeAddUI::GetClass() const
{
    return UIString{"CNodeAddUI"};
}

bool CNodeAddUI::OnButtonNotify(void * param)
{
    TNotifyUI * msg = static_cast<TNotifyUI *>(param);
    if (msg == nullptr) return false;
    if (msg->sType != DUI_MSGTYPE_CLICK) return false;

    int a = atoi(editA_->GetText().GetData());
    int b = atoi(editB_->GetText().GetData());
    int result = a + b;

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%d", result);

    PostNodeMessage(UIString{NODE_MSG_ADD_RESULT},
                    UIString{buf},
                    0, 0, true);
    return true;
}