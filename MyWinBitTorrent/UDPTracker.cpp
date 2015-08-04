#include <WS2tcpip.h>
#include "UDPTracker.h"
#include "TorrentParser.h"

CUDPTracker::CUDPTracker(void)
{
    m_nTrackerState = TS_INIT;
    memset(&m_stServerAddr, 0, sizeof(m_stServerAddr));
    m_nCompletePeer = 0;
    m_nPeerCount = 0;
    m_nInterval = 0;
    m_llNextUpdateTick = GetTickCount();
}

CUDPTracker::~CUDPTracker(void)
{
}

void CUDPTracker::SetTrackerManager( ITrackerManager *pTrackerManager )
{
    m_pTrackerManager = pTrackerManager;
}

bool CUDPTracker::IsProtocolSupported( const char * pProtocol )
{
    return true;
}

void CUDPTracker::SetURL( const char *pUrl )
{
    m_strTrackerURL = pUrl;
}

void CUDPTracker::Update()
{
    if (m_nTrackerState == TS_INIT)
    {
        if (m_stServerAddr.sin_port == 0)
        {
            string strHost;
            string strPath;
            int nPort;
            
            if (!CTorrentParser::ParseTrackInfo(m_strTrackerURL.c_str(), strHost, nPort, strPath))
            {
                m_nTrackerState = TS_ERROR;
                m_nInterval = GetTickCount() + 60 * 60 * 1000;
                return;
            }

            if (!CWinSocket::GetRemotAddrInfo(strHost.c_str(), nPort, m_stServerAddr))
            {
                return;
            }
        }

        CWinSocket::CreateUDPSocket();
        CWinSocket::SetReactor(m_pTrackerManager->GetTorrentTask()->GetSocketReactor());
        SetHandleMask(READ_MASK);

        SendConnectMsg();
        m_nTrackerState = TS_CONNECTING;
        m_llNextUpdateTick = GetTickCount() + 15 * 1000;
    }

    if (m_nTrackerState == TS_ERROR || 
        m_nTrackerState == TS_CONNECTING)
    {
        SendConnectMsg();
        m_nTrackerState = TS_CONNECTING;
        m_llNextUpdateTick = GetTickCount() + 15 * 1000;
    }

    if (m_nTrackerState == TS_OK ||
        m_nTrackerState == TS_REQUESTING)
    {
        SendAnnounceMsg();
    }

}

void CUDPTracker::Shutdown()
{
    CWinSocket::SetReactor(NULL);
    CWinSocket::Close();
}

int CUDPTracker::GetSeedCount()
{
    return m_nCompletePeer;
}

int CUDPTracker::GetPeerCount()
{
    return m_nPeerCount;
}

int CUDPTracker::GetInterval()
{
    return m_nInterval;
}

long long CUDPTracker::GetNextUpdateTick()
{
    return m_llNextUpdateTick;
}

int CUDPTracker::GetTrackerState()
{
    return m_nTrackerState;
}

int CUDPTracker::HandleWrite()
{
    return 0;
}

void CUDPTracker::SendConnectMsg()
{
    m_nTransID = rand() * GetTickCount();
    ConnectMsg stMsg;
    
    stMsg.llConnectionID = 0x41727101980LL;
    stMsg.llConnectionID = htonll(stMsg.llConnectionID);
    stMsg.nActionID = 0;
    stMsg.nTransactionID = htonl(m_nTransID);

    sendto(GetHandle(), (const char *)&stMsg, sizeof(stMsg), 0, (const struct sockaddr*)&m_stServerAddr, sizeof(m_stServerAddr));
}

void CUDPTracker::SendAnnounceMsg()
{
    m_nTransID = rand() * GetTickCount();
    AnnounceMsg stMsg;
    stMsg.llConnectionID = ntohll(m_llConnectionID);
    stMsg.nActionID = htonl(1);
    stMsg.nTransactionID = htonl(m_nTransID);
    memcpy(stMsg.szInfoHash, m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetInfoHash(), 20);
    memcpy(stMsg.szPeerID, m_pTrackerManager->GetTorrentTask()->GetPeerID().c_str(), 20);
    stMsg.llDownloaded = htonl(m_pTrackerManager->GetTorrentTask()->GetDownloadCount());
    stMsg.llLeft = htonl(m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetTotalFileSize() - m_pTrackerManager->GetTorrentTask()->GetDownloadCount());
    stMsg.llUploaded = htonl(m_pTrackerManager->GetTorrentTask()->GetUploadCount());
    stMsg.nEvent = htonl(GetCurrentEvent());
    stMsg.nIP = htonl(0);
    stMsg.nKey = htonl(m_nTransID * rand());
    stMsg.nNumWant = htonl(100);
    stMsg.nPort = htons(m_pTrackerManager->GetTorrentTask()->GetAcceptor()->GetPort());
    stMsg.nExtensions = 0;

    sendto(GetHandle(), (const char *)&stMsg, sizeof(&stMsg), 0, (const struct sockaddr*)&m_stServerAddr, sizeof(m_stServerAddr));
}

void CUDPTracker::ProcessMsg(unsigned int nActionID, char *pData, int nLen)
{
    if (nActionID == 3)
    {
        m_nTrackerState = TS_ERROR;
        m_llNextUpdateTick = GetTickCount() + 15 * 1000;
    }

    if (nActionID == 0)
    {
        m_llConnectionID = *((long long*)pData);
        m_llConnectionID = ntohll(m_llConnectionID);
        m_nTrackerState = TS_REQUESTING;
        SendAnnounceMsg();
        m_llNextUpdateTick = GetTickCount() + 15 * 1000;
    }

    if (nActionID == 1)
    {
        unsigned int nInterval = *((unsigned int *)pData);
        nInterval = htonl(nInterval);

        unsigned int nLeechers = *((unsigned int *)(pData + 4));
        nLeechers = htonl(nLeechers);

        unsigned int nSeeders = *((unsigned int *)(pData + 4));
        nSeeders = htonl(nSeeders);

        m_nInterval = nInterval;
        m_nPeerCount = nLeechers + nSeeders;
        m_nCompletePeer = nSeeders;

        char *pBuff = NULL;
        for (int i = 0; i < (nLen - 12)/6; ++i)
        {
            pBuff = pData + 12 + i * 6;
            unsigned int Ip = *((unsigned int *)pBuff);
            unsigned short nPort = *((unsigned short *)(pBuff + 4));
            nPort = ntohs(nPort);

            if (Ip != 0 && nPort != 0)
            {
                char szBuff[256];
                string strIPAddr = inet_ntop(AF_INET, &Ip, szBuff, 256);
                m_pTrackerManager->GetTorrentTask()->GetPeerManager()->AddPeerInfo(strIPAddr.c_str(), nPort);
            }
        }

        m_nTrackerState = TS_OK;
        int nConnectedPeerCount = m_pTrackerManager->GetTorrentTask()->GetPeerManager()->GetConnectedPeerCount();
        int nMaxPeerLinkCount = m_pTrackerManager->GetTorrentTask()->GetMaxPeerLink();

        if (nConnectedPeerCount >= nMaxPeerLinkCount ||
            nConnectedPeerCount > m_nPeerCount / 2 ||
            m_nInterval <= 2 * 60)
        {
            m_llNextUpdateTick = GetTickCount() + m_nInterval * 1000;
        }
        else
        {
            m_llNextUpdateTick = GetTickCount() + 2 * 60 * 1000;
        }

        if (m_nCurrentEvent == 2)
        {
            m_bSendStartEvent = true;
        }
        else if(m_nCurrentEvent == 1)
        {
            m_bSendCompleteEvent = true;
        }
    }
}

int CUDPTracker::GetCurrentEvent()
{
    if (!m_bSendStartEvent)
    {
        m_nCurrentEvent = 2;
        return 2;
    }
    if (m_pTrackerManager->GetTorrentTask()->GetTaskStorage()->Finished()
        && m_pTrackerManager->GetTorrentTask()->GetTaskStorage()->GetBanedCount() == 0
        && !m_bSendCompleteEvent)
    {
        m_nCurrentEvent = 1;
        return 1;
    }

    return 0;
}

int CUDPTracker::HandleRead()
{
    char *pBuff = new char[RECV_BUFFER_SIZE];
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(&addr));
    int nLen = sizeof(addr);

    int nRet = recvfrom(GetHandle(), pBuff, RECV_BUFFER_SIZE, 0, (struct sockaddr*)&addr, &nLen);
    if (memcmp(&addr.sin_addr.S_un.S_addr, &m_stServerAddr.sin_addr.S_un.S_addr, sizeof(addr.sin_addr.S_un.S_addr)) == 0 && nRet > 8)
    {
        unsigned int nActionID = *((unsigned int *)pBuff);
        nActionID = ntohl(nActionID);

        unsigned int nTransID = *((unsigned int *)(pBuff + 4));
        if (nTransID == m_nTransID)
        {
            ProcessMsg(nActionID, pBuff + 8, nRet - 8);
        }
    }

    delete [] pBuff;
    return 0;
}

void CUDPTracker::HandleClose()
{

}
