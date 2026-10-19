#ifndef DIRECTUI_CNODEUIMANAGER_H
#define DIRECTUI_CNODEUIMANAGER_H

#include "CNodeMessage.h"
#include "UIString.h"
#include <vector>

class CNodeUIBase;

class CNodeUIManager
{
public:
    static CNodeUIManager & GetInstance();

    void RegisterNode(CNodeUIBase * node);
    void UnregisterNode(CNodeUIBase * node);

    void BroadcastMessage(const CNodeMessage & msg, bool b_async = true);

    void ProcessMessageQueue();

    size_t   GetNodeCount() const;
    CNodeUIBase * FindNode(const UIString & name) const;

    uint32_t GetActiveTimerId() const { return activeTimerId_; }

    void Clear();

private:
    CNodeUIManager();
    ~CNodeUIManager();
    CNodeUIManager(const CNodeUIManager &) = delete;
    CNodeUIManager & operator=(const CNodeUIManager &) = delete;

    std::vector<CNodeUIBase *>   nodes_;
    std::vector<CNodeMessage>    messageQueue_;
    CNodeUIBase *                timerControl_;
    uint32_t                     activeTimerId_;
    bool                         bTimerActive_;
};

#endif
