#include "PeerManager.h"

CPeerManager::CPeerManager(void) : m_pTorrentTask(NULL)
{
}

CPeerManager::~CPeerManager(void)
{
}

bool CPeerManager::Startup()
{
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
}

void CPeerManager::AddPeerInfo( const char *pIpAddr, int nPort )
{

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

}
