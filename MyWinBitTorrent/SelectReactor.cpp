#include "SelectReactor.h"
#include <algorithm>

CSelectReactor::CSelectReactor(void)
{
    m_nMaxSocketFd = 0;
}

CSelectReactor::~CSelectReactor(void)
{
}

bool CSelectReactor::AddSocket( IWinSocket *pSocket )
{
    vector<IWinSocket *>::iterator it = find(m_vecSockets.begin(), m_vecSockets.end(), pSocket);
    if (it == m_vecSockets.end())
    {
        m_vecSockets.push_back(pSocket);
        if (pSocket->GetHandle() > m_nMaxSocketFd)
        {
            m_nMaxSocketFd = pSocket->GetHandle();
        }
    }

    UpdateMask(pSocket);

    return true;
}

void CSelectReactor::RemoveSocket( IWinSocket *pSocket )
{
    vector<IWinSocket *>::iterator it = find(m_vecSockets.begin(), m_vecSockets.end(), pSocket);
    if (it != m_vecSockets.end())
    {
        m_vecSockets.erase(it);
    }

    if (pSocket->GetHandleMask() & READ_MASK)
    {
        FD_CLR(pSocket->GetHandle(), &m_rSet);
    }

    if (pSocket->GetHandleMask() & WRITE_MASK)
    {
        FD_CLR(pSocket->GetHandle(), &m_wSet);
    }
    
}

void CSelectReactor::UpdateMask( IWinSocket *pSocket )
{
    FD_CLR(pSocket->GetHandle(), &m_rSet);
    FD_CLR(pSocket->GetHandle(), &m_wSet);

    if (pSocket->GetHandleMask() & READ_MASK)
    {
        FD_SET(pSocket->GetHandle(), &m_rSet);
    }

    if (pSocket->GetHandleMask() & WRITE_MASK)
    {
        FD_SET(pSocket->GetHandle(), &m_wSet);
    }
}

bool CSelectReactor::Startup()
{
    ClearFdSet();
    return true;
}

void CSelectReactor::Update()
{
    SelectSocket();
}

void CSelectReactor::Shutdown()
{
    ClearFdSet();
    m_vecSockets.clear();
}

void CSelectReactor::ClearFdSet()
{
    FD_ZERO(&m_rSet);
    FD_ZERO(&m_wSet);
}

int CSelectReactor::SelectSocket()
{
    int nRet = 0;
    timeval tmval;
    tmval.tv_sec = 1;
    tmval.tv_usec = 0;

    nRet = select(m_nMaxSocketFd + 1, &m_rSet, &m_wSet, NULL, &tmval);

    if (nRet > 0)
    {
        vector<IWinSocket *>::iterator it = m_vecSockets.begin();
        for (; it != m_vecSockets.begin(); ++it)
        {
            if (FD_ISSET((*it)->GetHandle(), &m_rSet))
            {
               int nRes = (*it)->HandleRead();
               if (nRes == -1)
               {
                   (*it)->HandleClose();
                   (*it)->Close();
               }
            }

            if (FD_ISSET((*it)->GetHandle(), &m_wSet))
            {
                int nRes = (*it)->HandleWrite();
                if (nRes == -1)
                {
                    (*it)->HandleClose();
                    (*it)->Close();
                }
            }
        }

    }

    return nRet;
}
