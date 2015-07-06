#include "TCPTracker.h"
#include "TorrentParser.h"
#include <errno.h>

CTCPTracker::CTCPTracker(void) : m_nTrackerState(TS_INIT)
{
    m_llCompletedPeer = 0;
    m_llInterval = 0;
    m_llNextUpdateTick = 0;
    m_llPeerCount = 0;
    m_nCurrentEvent = 0;
    m_bSendStartEvent = false;
    m_bSendCompleteEvent = false;

    m_pTrackerManager = NULL;
}

CTCPTracker::~CTCPTracker(void)
{
}

void CTCPTracker::SetTrackerManager( ITrackManager *pTrackerManager )
{
    m_pTrackerManager = pTrackerManager;
}

void CTCPTracker::SetURL( const char *pUrl )
{
    m_strTrackerURL = pUrl;
}

void CTCPTracker::Update()
{
    m_strTrackerResponse.clear();

    if (m_nTrackerState == TS_INIT)
    {
        CWinSocket::CreateTCPSocket();
        CWinSocket::SetReactor(m_pTrackerManager->GetTorrentTask()->GetSocketReactor());
        CWinSocket::SetHandleMask(WRITE_MASK);

        string strHost;
        int nPort;
        CTorrentParser::ParseTrackInfo(m_strTrackerURL.c_str(), strHost, nPort);
        
        
        CWinSocket::Connect(strHost.c_str(), nPort);

        m_nTrackerState = TS_CONNECTING;
        m_llNextUpdateTick = GetTickCount() + 60 * 1000;
    }

//     if (m_nTrackerState == TS_CONNECTING)
//     {
//         m_nCurrentEvent = GetCurrentEvent();
//         string strReq = GenTrackerURL(Event2Str(m_nCurrentEvent));
//         send(GetHandle(), strReq.c_str(), strReq.length(), 0);
//         m_nTrackerState = TS_REQUESTING;
//         m_llNextUpdateTick = GetTickCount() + 60 * 1000;
//     }

}

void CTCPTracker::Shutdown()
{

}

long long CTCPTracker::GetSeedCount()
{
    return m_llCompletedPeer;
}

long long CTCPTracker::GetPeerCount()
{
    return m_llPeerCount;
}

long long CTCPTracker::GetInterval()
{
    return 0;
}

long long CTCPTracker::GetNextUpdateTick()
{
    return m_llNextUpdateTick;
}

bool CTCPTracker::IsProtocolSupported( const char * pProtocol )
{
    return true;
}

int CTCPTracker::GetCurrentEvent()
{
    if (!m_bSendStartEvent)
    {
        return TE_START;
    }

//     if (!m_bSendCompleteEvent)
//     {
//         return TE_COMPLETE;
//     }

    return TE_NONE;
}

const char * CTCPTracker::Event2Str( int nEvent )
{
    switch(nEvent)
    {
    case TE_START:
        return "started";
        break;
    case TE_STOP:
        return "stopped";
        break;
    case TE_COMPLETE:
        return "completed";
        break;
    case TE_NONE:
        return "";
        break;
    }

    return "";
}

string CTCPTracker::GenTrackerURL( const char *pEvent )
{
    char szDstURL[1024];
    sprintf(szDstURL, "GET %s?info_hash=%s&peer_id=%s&port=%d&compact=1&uploaded=%lld&downloaded=%lld&left=%lld&event=%s&numwant=20 HTTP/1.1\r\nUser-Agent: Bittorrent",
            m_strTrackerURL.c_str(),
            URLEncode(m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetInfoHash(), 20).c_str(),
            URLEncode((const unsigned char *)(m_pTrackerManager->GetTorrentTask()->GetPeerID().c_str()), 20).c_str(),
            m_pTrackerManager->GetTorrentTask()->GetAcceptor()->GetPort(),
            m_pTrackerManager->GetTorrentTask()->GetDownloadCount(),
            m_pTrackerManager->GetTorrentTask()->GetUploadCount(),
            //m_pTrackerManager->GetTorrentTask()->GetTaskStorage()->GetLeftCount(),
            m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetTotalFileSize(),
            pEvent);

    return szDstURL;
}

int CTCPTracker::GetTrackerState()
{
    return m_nTrackerState;
}

int CTCPTracker::HandleWrite()
{
    if (m_nTrackerState == TS_CONNECTING)
    {
        m_nTrackerState = TS_ESTABLISHED;
        CWinSocket::SetHandleMask(NONE_MASK);
        OnConnect();
    }

    return 0;
}

void CTCPTracker::OnConnect()
{
    CWinSocket::SetHandleMask(READ_MASK);
    m_nCurrentEvent = GetCurrentEvent();
    string strReq = GenTrackerURL(Event2Str(m_nCurrentEvent));
    send(GetHandle(), strReq.c_str(), strReq.length(), 0);
    m_nTrackerState = TS_REQUESTING;
    m_llNextUpdateTick = GetTickCount() + 60 * 1000;
}

int CTCPTracker::HandleRead()
{
    m_strTrackerResponse.clear();
    char buff[1024];
    for (;;)
    {
        int nRet = recv(GetHandle(), buff, sizeof(buff), 0);
        if (nRet == 0)
        {
            CWinSocket::RemoveHandleMask(READ_MASK);
            return -1;
        }

        if (nRet == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            if (errno != EAGAIN)
            {
                CWinSocket::RemoveHandleMask(READ_MASK);
                return -1;
            }
            
            break;
        }

        m_strTrackerResponse.append(buff);
    }

    if (m_strTrackerResponse.size())
    {
        ParseTrackerResponse();
    }

    m_llNextUpdateTick = GetTickCount() + 60 * 1000;
    return 0;
}

void CTCPTracker::ParseTrackerResponse()
{
    
}
