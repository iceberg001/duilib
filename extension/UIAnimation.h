#ifndef DUILIB_EXT_UIANIMATION_H
#define DUILIB_EXT_UIANIMATION_H

#include <vector>
#include <cassert>
#include "UIControl.h"

class IUIAnimation
{
public:
    virtual ~IUIAnimation() {}

    virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false) = 0;
    virtual void StopAnimation(int nAnimationID = 0) = 0;
    virtual bool IsAnimationRunning(int nAnimationID) = 0;
    virtual int  GetCurrentFrame(int nAnimationID = 0) = 0;
    virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0) = 0;

    virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) = 0;
    virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) = 0;
    virtual void OnAnimationStop(int nAnimationID) = 0;

    virtual void OnAnimationElapse(int nAnimationID) = 0;
};

class CAnimationData
{
public:
    CAnimationData(int nElipse, int nFrame, int nID, bool bLoop)
    {
        m_bFirstLoop = true;
        m_nCurFrame = 0;
        m_nElapse = nElipse;
        m_nTotalFrame = nFrame;
        m_bLoop = bLoop;
        m_nAnimationID = nID;
        m_nTimerID = 0;
    }

public:
    friend class CUIAnimation;

    int  m_nAnimationID;
    int  m_nElapse;
    uint32_t m_nTimerID;

    int  m_nTotalFrame;
    int  m_nCurFrame;

    bool m_bLoop;
    bool m_bFirstLoop;
};

class CUIAnimation : public IUIAnimation
{
public:
    CUIAnimation(UIControl* pOwner);

    virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false);
    virtual void StopAnimation(int nAnimationID = 0);
    virtual bool IsAnimationRunning(int nAnimationID);
    virtual int  GetCurrentFrame(int nAnimationID = 0);
    virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0);

    virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) {}
    virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) {}
    virtual void OnAnimationStop(int nAnimationID) {}

    virtual void OnAnimationElapse(int nAnimationID);

    // Map timer ID (from UIPaintManager::SetTimer) to animation ID
    int GetAnimationIDByTimerID(uint32_t nTimerID);

protected:
    CAnimationData* GetAnimationDataByID(int nAnimationID);

protected:
    UIControl* m_pControl;
    std::vector<CAnimationData*> m_arAnimations;
};

#endif // DUILIB_EXT_UIANIMATION_H