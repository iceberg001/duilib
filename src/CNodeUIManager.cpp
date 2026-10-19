#include "CNodeUIManager.h"
#include "CNodeUIBase.h"
#include "UIPaintManager.h"
#include "UIControl.h"

CNodeUIManager & CNodeUIManager::GetInstance()
{
    static CNodeUIManager instance;
    return instance;
}

CNodeUIManager::CNodeUIManager()
    : timerControl_(nullptr)
    , activeTimerId_(0)
    , bTimerActive_(false)
{
}

CNodeUIManager::~CNodeUIManager()
{
    nodes_.clear();
    messageQueue_.clear();
}

void CNodeUIManager::RegisterNode(CNodeUIBase * node)
{
    if (node == nullptr) return;
    for (auto * n : nodes_)
    {
        if (n == node) return;
    }
    nodes_.push_back(node);
}

void CNodeUIManager::UnregisterNode(CNodeUIBase * node)
{
    if (node == nullptr) return;
    for (auto it = nodes_.begin(); it != nodes_.end(); ++it)
    {
        if (*it == node)
        {
            nodes_.erase(it);
            return;
        }
    }
}

void CNodeUIManager::BroadcastMessage(const CNodeMessage & msg, bool b_async)
{
    if (!b_async)
    {
        for (auto * node : nodes_)
        {
            if (node && node->WantNodeMessage())
            {
                node->OnNodeMessage(msg);
            }
        }
        return;
    }

    messageQueue_.push_back(msg);

    if (!bTimerActive_)
    {
        for (auto * node : nodes_)
        {
            if (node && node->GetManager())
            {
                activeTimerId_ = node->GetManager()->SetTimer(node, 30);
                timerControl_ = node;
                bTimerActive_ = true;
                break;
            }
        }
    }
}

void CNodeUIManager::ProcessMessageQueue()
{
    if (bTimerActive_ && timerControl_ && timerControl_->GetManager())
    {
        timerControl_->GetManager()->KillTimer(timerControl_, activeTimerId_);
        bTimerActive_ = false;
        activeTimerId_ = 0;
        timerControl_ = nullptr;
    }

    if (messageQueue_.empty()) return;

    std::vector<CNodeMessage> queue;
    queue.swap(messageQueue_);

    for (auto & msg : queue)
    {
        for (auto * node : nodes_)
        {
            if (node && node->WantNodeMessage())
            {
                node->OnNodeMessage(msg);
            }
        }
    }
}

size_t CNodeUIManager::GetNodeCount() const
{
    return nodes_.size();
}

CNodeUIBase * CNodeUIManager::FindNode(const UIString & name) const
{
    for (auto * node : nodes_)
    {
        if (node && node->GetNodeName() == name)
        {
            return node;
        }
    }
    return nullptr;
}

void CNodeUIManager::Clear()
{
    nodes_.clear();
    messageQueue_.clear();
    bTimerActive_ = false;
    activeTimerId_ = 0;
    timerControl_ = nullptr;
}
