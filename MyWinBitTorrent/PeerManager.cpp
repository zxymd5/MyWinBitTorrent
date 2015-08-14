#include "PeerManager.h"
#include "PeerLink.h"

CPeerManager::CPeerManager(void) : m_pTorrentTask(NULL)
{
}

CPeerManager::~CPeerManager(void)
{
}

bool CPeerManager::Startup()
{
    InitializeCriticalSection(&m_csUnusedPeer);

    m_nConnectTimerID = m_pTorrentTask->GetSocketReactor()->AddTimer(this, 2000, false);

    m_nChokeTimerID = m_pTorrentTask->GetSocketReactor()->AddTimer(this, 20000, false);

    return true;
}

void CPeerManager::Shutdown()
{
    m_pTorrentTask->GetSocketReactor()->RemoveTimer(m_nConnectTimerID);
    m_pTorrentTask->GetSocketReactor()->RemoveTimer(m_nChokeTimerID);

    map<string, PeerInfo>::iterator it = m_mapConnectedPeer.begin();
    for (; it != m_mapConnectedPeer.end(); ++it)
    {
        if (it->second.pPeerLink != NULL)
        {
            it->second.pPeerLink->CloseLink();
            delete it->second.pPeerLink;
            it->second.pPeerLink = NULL;
        }
    }

    it = m_mapConnectingPeer.begin();
    for (; it != m_mapConnectingPeer.end(); ++it)
    {
        if (it->second.pPeerLink != NULL)
        {
            it->second.pPeerLink->CloseLink();
            delete it->second.pPeerLink;
            it->second.pPeerLink = NULL;
        }
    }

    m_mapUnusedPeer.clear();
    m_mapConnectingPeer.clear();
    m_mapConnectedPeer.clear();
    m_mapBanedPeer.clear();
    DeleteCriticalSection(&m_csUnusedPeer);
}

void CPeerManager::AddPeerInfo( const char *pIpAddr, int nPort )
{
    string strPeerLinkID = GenPeerLinkID(pIpAddr, nPort);
    if (PeerExists(strPeerLinkID))
    {
        return;
    }

    PeerInfo stPeerInfo;
    stPeerInfo.strLinkID = strPeerLinkID;
    stPeerInfo.strIPAddr = pIpAddr;
    stPeerInfo.nPort = nPort;
    stPeerInfo.pPeerLink = NULL;
    
    EnterCriticalSection(&m_csUnusedPeer);
    m_mapUnusedPeer[strPeerLinkID] = stPeerInfo;
    LeaveCriticalSection(&m_csUnusedPeer);
}

void CPeerManager::SetTorrentTask( ITorrentTask *pTask )
{
    m_pTorrentTask = pTask;
}

ITorrentTask * CPeerManager::GetTorrentTask()
{
    return m_pTorrentTask;
}

void CPeerManager::OnTimer( int nTimerID )
{
    if (nTimerID == m_nConnectTimerID)
    {
        CheckPeerConnection();
    }
    
    if (nTimerID == m_nChokeTimerID)
    {
        CheckPeerChoke();
    }
}

string CPeerManager::GenPeerLinkID( const char *pIPAddr, int nPort )
{
    char szBuff[100];
    sprintf(szBuff, "%s:%u", pIPAddr, nPort);

    return szBuff;
}

bool CPeerManager::PeerExists( string strPeerLinkID )
{
    bool bInUnusedPeer = false;
    EnterCriticalSection(&m_csUnusedPeer);
    bInUnusedPeer = (m_mapUnusedPeer.find(strPeerLinkID) != m_mapUnusedPeer.end());
    LeaveCriticalSection(&m_csUnusedPeer);

    return bInUnusedPeer
           || m_mapConnectingPeer.find(strPeerLinkID) != m_mapConnectingPeer.end()
           || m_mapConnectedPeer.find(strPeerLinkID) != m_mapConnectedPeer.end()
           || m_mapBanedPeer.find(strPeerLinkID) != m_mapBanedPeer.end();
}

void CPeerManager::CheckPeerConnection()
{
    map<string, PeerInfo>::iterator it = m_mapConnectingPeer.begin();
    for (; it != m_mapConnectingPeer.end(); )
    {
        if (it->second.pPeerLink == NULL)
        {
            m_mapConnectingPeer.erase(it++);
            continue;
        }

        //Move to connected peer map
        if (it->second.pPeerLink->GetPeerState() == PS_ESTABLISHED)
        {
            it->second.nConnFailedCount = 0;
            m_mapConnectedPeer[it->second.strLinkID] = it->second;
            m_mapConnectingPeer.erase(it++);
            continue;
        }

        if (it->second.pPeerLink->GetPeerState() == PS_CONNECTFAILED)
        {
            it->second.nConnFailedCount++;
            it->second.pPeerLink->CloseLink();
            delete it->second.pPeerLink;
            it->second.pPeerLink = NULL;

            if (it->second.nConnFailedCount >= 3)
            {
                m_mapBanedPeer[it->second.strLinkID] = it->second;
            }
            else
            {
                EnterCriticalSection(&m_csUnusedPeer);
                m_mapUnusedPeer[it->second.strLinkID] = it->second;
                LeaveCriticalSection(&m_csUnusedPeer);
            }

            m_mapConnectingPeer.erase(it++);
            continue;
        }

        //Move closed connection to unused peer
        if (it->second.pPeerLink->GetPeerState() == PS_CLOSED)
        {
            it->second.pPeerLink->CloseLink();
            delete it->second.pPeerLink;
            it->second.pPeerLink = NULL;

            EnterCriticalSection(&m_csUnusedPeer);
            m_mapUnusedPeer[it->second.strLinkID] = it->second;
            LeaveCriticalSection(&m_csUnusedPeer);

            m_mapConnectingPeer.erase(it++);
            continue;
        }

        ++it;
    }

    it = m_mapConnectedPeer.begin();
    for (; it != m_mapConnectedPeer.end();)
    {
        if (it->second.pPeerLink->GetPeerState() == PS_CLOSED)
        {
            bool bAccepted = it->second.pPeerLink->IsAccepted();
            it->second.pPeerLink->CloseLink();
            delete it->second.pPeerLink;
            it->second.pPeerLink = NULL;

            if (!bAccepted)
            {
                EnterCriticalSection(&m_csUnusedPeer);
                m_mapUnusedPeer[it->second.strLinkID] = it->second;
                LeaveCriticalSection(&m_csUnusedPeer);
            }

            m_mapConnectedPeer.erase(it++);
            continue;
        }
        ++it;
    }

    EnterCriticalSection(&m_csUnusedPeer);
    //Add new connection
    if (m_mapConnectedPeer.size() < m_pTorrentTask->GetMaxPeerLink())
    {
        for(;;)
        {
            if (m_mapUnusedPeer.size() == 0)
            {
                break;
            }

            if (m_mapConnectedPeer.size() >= m_pTorrentTask->GetMaxPeerLink())
            {
                break;
            }

            if (m_mapConnectingPeer.size() < m_pTorrentTask->GetMaxConnectingPeerLink())
            {
                PeerInfo stPeerInfo = m_mapUnusedPeer.begin()->second;
                stPeerInfo.pPeerLink = new CPeerLink();
                stPeerInfo.pPeerLink->SetPeerManager(this);
                stPeerInfo.pPeerLink->Connect(stPeerInfo.strIPAddr.c_str(), stPeerInfo.nPort);

                m_mapUnusedPeer.erase(m_mapUnusedPeer.begin());
                m_mapConnectingPeer[stPeerInfo.strLinkID] = stPeerInfo;
            }
            else
            {
                break;
            }
        }
    }
    LeaveCriticalSection(&m_csUnusedPeer);

    //close some useless peers
    if (m_mapConnectedPeer.size() > m_pTorrentTask->GetMaxPeerLink() - 10)
    {
        int i = 0;
        it = m_mapConnectedPeer.begin();
        for (; it != m_mapConnectedPeer.end(); )
        {
          if (it->second.pPeerLink->ShouldClose())
          {
              it->second.pPeerLink->CloseLink();
              i++;
          }
          if (i >= 5)
          {
              break;
          }
          ++it;
        }
    }
}

void CPeerManager::CheckPeerChoke()
{
    ComputePeerSpeed();

    int nDownloaderCount = 0;
    map<string, PeerInfo>::iterator it = m_mapConnectedPeer.begin();

    for (; it != m_mapConnectedPeer.end(); ++it)
    {
        IPeerLink *pPeerLink = it->second.pPeerLink;
        if (pPeerLink == NULL)
        {
            continue;
        }
        
        if (!pPeerLink->PeerInterested())
        {
            pPeerLink->ChokePeer(true);
        }
        if (!pPeerLink->PeerChoked())
        {
            nDownloaderCount++;
        } 
    }

    for (; nDownloaderCount < m_pTorrentTask->GetMaxUploadPeerLink(); )
    {
        it = m_mapConnectedPeer.begin();
        map<string, PeerInfo>::iterator it2 = m_mapConnectedPeer.begin();
        for (; it != m_mapConnectedPeer.end(); ++it)
        {
            IPeerLink *pPeerLink = it->second.pPeerLink;
            if (pPeerLink == NULL)
            {
                continue;
            }

            if (pPeerLink->PeerChoked() && pPeerLink->PeerInterested())
            {
                if (it2 == m_mapConnectedPeer.end())
                {
                    it2 = it;
                }
                else if (pPeerLink->GetDownloadSpeed() > it2->second.pPeerLink->GetDownloadSpeed())
                {
                    it2 = it;
                }
            }
        }

        if (it2 != m_mapConnectedPeer.end())
        {
            it2->second.pPeerLink->ChokePeer(false);
            nDownloaderCount++;
        }
        else
        {
            break;
        }
    }

    if (nDownloaderCount >= m_pTorrentTask->GetMaxUploadPeerLink())
    {
        //获取当前上传peer中下载速度最慢的一个

        it = m_mapConnectedPeer.begin();
        map<string, PeerInfo>::iterator worstIt = m_mapConnectedPeer.end();

        for (; it != m_mapConnectedPeer.end(); ++it)
        {
            IPeerLink *pPeerLink = it->second.pPeerLink;
            if (pPeerLink == NULL)
            {
                continue;
            }

            if (!pPeerLink->PeerChoked() && pPeerLink->PeerInterested())
            {
                if (worstIt == m_mapConnectedPeer.end())
                {
                    worstIt = it;
                }
                else if (pPeerLink->GetDownloadSpeed() < worstIt->second.pPeerLink->GetDownloadSpeed())
                {
                    worstIt = it;
                }
            }
        }
        
        //得到非上传peer中下载速度最快的一个
        it = m_mapConnectedPeer.begin();
        map<string, PeerInfo>::iterator bestIt = m_mapConnectedPeer.begin();

        for (; it != m_mapConnectedPeer.end(); ++it)
        {
            IPeerLink *pPeerLink = it->second.pPeerLink;
            if (pPeerLink == NULL)
            {
                continue;
            }

            if (pPeerLink->PeerChoked() && pPeerLink->PeerInterested())
            {
                if (bestIt == m_mapConnectedPeer.end())
                {
                    bestIt = it;
                }
                else if (pPeerLink->GetDownloadSpeed() > bestIt->second.pPeerLink->GetDownloadSpeed())
                {
                    bestIt = it;
                }
            }
        }

        if (worstIt != m_mapConnectedPeer.end()
            && bestIt != m_mapConnectedPeer.end()
            && worstIt->second.pPeerLink->GetDownloadSpeed() < bestIt->second.pPeerLink->GetDownloadSpeed())
        {
            //阻塞下载最慢的downloader
            worstIt->second.pPeerLink->ChokePeer(true);

            //给下载最快的downloader机会
            bestIt->second.pPeerLink->ChokePeer(false);
        }
        
    }

}

void CPeerManager::ComputePeerSpeed()
{
    map<string, PeerInfo>::iterator it = m_mapConnectedPeer.begin();
    for (; it != m_mapConnectedPeer.end(); ++it)
    {
        if (it->second.pPeerLink != NULL)
        {
            it->second.pPeerLink->ComputeSpeed();
        }
    }
}

void CPeerManager::OnDownloadComplete()
{
    map<string, PeerInfo>::iterator it = m_mapConnectedPeer.begin();

    for (; it != m_mapConnectedPeer.end(); ++it)
    {
        if (it->second.pPeerLink != NULL)
        {
            it->second.pPeerLink->OnDownloadComplete();
        }
    }
}

void CPeerManager::BroadcastHave( int nPieceIndex )
{
    map<string, PeerInfo>::iterator it = m_mapConnectedPeer.begin();
    for (; it != m_mapConnectedPeer.end(); ++it)
    {
        if (it->second.pPeerLink != NULL)
        {
            it->second.pPeerLink->NotifyHavePiece(nPieceIndex);
        }
    }
}

bool CPeerManager::AddAcceptedPeer( int nHandle, const char *pIpAddr, int nPort )
{
    if (m_mapConnectedPeer.size() >= m_pTorrentTask->GetMaxPeerLink())
    {
        return false;
    }

    string strPeerLinkID = GenPeerLinkID(pIpAddr, nPort);

    PeerInfo stInfo;
    stInfo.strLinkID = strPeerLinkID;
    stInfo.strIPAddr = pIpAddr;
    stInfo.nPort = nPort;
    stInfo.nConnFailedCount = 0;
    stInfo.pPeerLink = new CPeerLink();
    stInfo.pPeerLink->Attach(nHandle, pIpAddr, nPort, this);

    m_mapConnectedPeer[strPeerLinkID] = stInfo;

    return true;
}

int CPeerManager::GetConnectedPeerCount()
{
    return m_mapConnectedPeer.size();
}
