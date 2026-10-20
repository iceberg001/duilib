#include "UIAnimation.h"
#include <algorithm>
#include "UIPaintManager.h"

CUIAnimation::CUIAnimation(UIControl* pOwner)
{
    assert(pOwner != nullptr);
    m_pControl = pOwner;
}

bool CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, int nAnimationID, bool bLoop)
{
    CAnimationData* pData = GetAnimationDataByID(nAnimationID);
    if(nullptr != pData
        || nElapse <= 0
        || nTotalFrame <= 0
        || nullptr == m_pControl)
    {
        assert(false);
        return false;
    }

    CAnimationData* pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
    if(nullptr == pAnimation) return false;

    uint32_t nTimerID = m_pControl->GetManager()->SetTimer(m_pControl, (uint32_t)nElapse);
    if(nTimerID != 0)
    {
        pAnimation->m_nTimerID = nTimerID;
        m_arAnimations.push_back(pAnimation);
        return true;
    }
    delete pAnimation;
    return false;
}

void CUIAnimation::StopAnimation(int nAnimationID)
{
    if(m_pControl == nullptr) return;

    if(nAnimationID != 0)
    {
        CAnimationData* pData = GetAnimationDataByID(nAnimationID);
        if(nullptr != pData)
        {
            m_pControl->GetManager()->KillTimer(m_pControl, pData->m_nTimerID);
            m_arAnimations.erase(std::remove(m_arAnimations.begin(), m_arAnimations.end(), pData), m_arAnimations.end());
            delete pData;
            return;
        }
    }
    else
    {
        size_t nCount = m_arAnimations.size();
        for(size_t i = 0; i < nCount; ++i)
        {
            m_pControl->GetManager()->KillTimer(m_pControl, m_arAnimations[i]->m_nTimerID);
            delete m_arAnimations[i];
        }
        m_arAnimations.clear();
    }
}

bool CUIAnimation::IsAnimationRunning(int nAnimationID)
{
    CAnimationData* pData = GetAnimationDataByID(nAnimationID);
    return nullptr != pData;
}

int CUIAnimation::GetCurrentFrame(int nAnimationID)
{
    CAnimationData* pData = GetAnimationDataByID(nAnimationID);
    if(nullptr == pData)
    {
        assert(false);
        return -1;
    }
    return pData->m_nCurFrame;
}

bool CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID)
{
    CAnimationData* pData = GetAnimationDataByID(nAnimationID);
    if(nullptr == pData)
    {
        assert(false);
        return false;
    }

    if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
    {
        pData->m_nCurFrame = nFrame;
        return true;
    }
    else
    {
        assert(false);
    }
    return false;
}

void CUIAnimation::OnAnimationElapse(int nAnimationID)
{
    if(m_pControl == nullptr) return;

    CAnimationData* pData = GetAnimationDataByID(nAnimationID);
    if(nullptr == pData) return;

    int nCurFrame = pData->m_nCurFrame;
    if(nCurFrame == 0)
    {
        OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
        pData->m_bFirstLoop = false;
    }

    OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);

    if(nCurFrame >= pData->m_nTotalFrame)
    {
        OnAnimationStop(nAnimationID);
        if(pData->m_bLoop)
        {
            pData->m_nCurFrame = 0;
        }
        else
        {
            m_pControl->GetManager()->KillTimer(m_pControl, pData->m_nTimerID);
            m_arAnimations.erase(std::remove(m_arAnimations.begin(), m_arAnimations.end(), pData), m_arAnimations.end());
            delete pData;
            pData = nullptr;
        }
    }

    if(nullptr != pData)
    {
        ++(pData->m_nCurFrame);
    }
}

CAnimationData* CUIAnimation::GetAnimationDataByID(int nAnimationID)
{
    CAnimationData* pRet = nullptr;
    size_t nCount = m_arAnimations.size();
    for(size_t i = 0; i < nCount; ++i)
    {
        if(m_arAnimations[i]->m_nAnimationID == nAnimationID)
        {
            pRet = m_arAnimations[i];
            break;
        }
    }

    return pRet;
}

int CUIAnimation::GetAnimationIDByTimerID(uint32_t nTimerID)
{
    size_t nCount = m_arAnimations.size();
    for(size_t i = 0; i < nCount; ++i)
    {
        if(m_arAnimations[i]->m_nTimerID == nTimerID)
        {
            return m_arAnimations[i]->m_nAnimationID;
        }
    }
    return 0;
}