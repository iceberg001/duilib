#include "CNodeResultUI.h"
#include "CNodeAddUI.h"
#include "UIPaintManager.h"

CNodeResultUI::CNodeResultUI()
    : lblResult_(nullptr)
    , editResult_(nullptr)
{
    SetNodeName(UIString{u8"NodeResult"});

    auto * layout = new UIHorizontalLayout;
    layout->SetInset({10, 10, 10, 10});
    layout->SetChildPadding(8);
    layout->SetBkColor(0xFFFFF3E0);
    layout->SetAttribute("border", "1");
    layout->SetAttribute("bordercolor", "#FFFF9800");

    lblResult_ = new UILabel;
    lblResult_->SetAttribute("width", "60");
    lblResult_->SetAttribute("height", "28");
    lblResult_->SetText(UIString{u8"结果 ="});
    lblResult_->SetAttribute("align", "center");
    layout->Add(lblResult_);

    editResult_ = new UIEdit;
    editResult_->SetName(UIString{u8"editResult"});
    editResult_->SetAttribute("width", "120");
    editResult_->SetAttribute("height", "28");
    editResult_->SetAttribute("bkcolor", "#FFFFFFFF");
    editResult_->SetAttribute("border", "1");
    editResult_->SetAttribute("bordercolor", "#FF999999");
    editResult_->SetReadOnly(true);
    editResult_->SetText(UIString{u8"0"});
    layout->Add(editResult_);

    this->Add(layout);
}

UIString CNodeResultUI::GetClass() const
{
    return UIString{"CNodeResultUI"};
}

void CNodeResultUI::OnNodeMessage(const CNodeMessage & msg)
{
    if (msg.msgType == NODE_MSG_ADD_RESULT)
    {
        editResult_->SetText(msg.msgData);
        if (GetManager())
        {
            RECT rc = editResult_->GetPos();
            GetManager()->Invalidate(rc);
        }
    }
}