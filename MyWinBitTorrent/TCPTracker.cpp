#include "TCPTracker.h"
#include "TorrentParser.h"
#include <errno.h>
#include "curl/curl.h"

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
    CURL *pHandle = curl_easy_init();

    m_strTrackerResponse.clear();
    m_nTrackerState = TS_CONNECTING;
    m_nCurrentEvent = GetCurrentEvent();

    string strURL = GenTrackerURL(Event2Str(m_nCurrentEvent));

    curl_easy_setopt(pHandle, CURLOPT_URL, strURL.c_str());
    curl_easy_setopt(pHandle, CURLOPT_WRITEFUNCTION, OnRecvData);
    curl_easy_setopt(pHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(pHandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(pHandle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(pHandle, CURLOPT_TIMEOUT, 15);
    CURLcode ret = curl_easy_perform(pHandle);

    if(ret == CURLE_OK)
    {
        ParseTrackerResponse();

    }
    else
    {
        m_nTrackerState = TS_ERROR;
//         _state_str = curl_easy_strerror(ret);
//         LOG_INFO("TCPTracker Error(curl) : "<<_state_str);

        m_llNextUpdateTick =  GetTickCount() + 60*1000;
    }

    curl_easy_cleanup(pHandle);
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

const char *CTCPTracker::Event2Str( int nEvent )
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
    memset(szDstURL, 0, 1024);
    int iIndex = 0;
    iIndex +=sprintf(szDstURL, "%s?info_hash=%s&peer_id=%s&port=%d&compact=1&uploaded=%lld&downloaded=%lld&left=%lld",
            m_strTrackerURL.c_str(),
            URLEncode(m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetInfoHash(), 20).c_str(),
            URLEncode((const unsigned char *)(m_pTrackerManager->GetTorrentTask()->GetPeerID().c_str()), 20).c_str(),
            m_pTrackerManager->GetTorrentTask()->GetAcceptor()->GetPort(),
            m_pTrackerManager->GetTorrentTask()->GetDownloadCount(),
            m_pTrackerManager->GetTorrentTask()->GetUploadCount(),
            //m_pTrackerManager->GetTorrentTask()->GetTaskStorage()->GetLeftCount(),
            //m_pTrackerManager->GetTorrentTask()->GetTorrentFile()->GetTotalFileSize(),
            11890);
    sprintf(szDstURL + iIndex, "&event=%s", pEvent);

    //strcpy(szDstURL, "GET http://torrent.ubuntu.com:6969/announce?info_hash=H%96%fd%e1N%fb%c0%f6j%27M*i%10O%bbW%fb%d2%cb&peer_id=-BM0002-%8a%aa%80c%b7%00%00%00%1d%02%00%00&port=7681&compact=1&uploaded=0&downloaded=0&left=11890&event=started HTTP/1.1\r\nHost:torrent.ubuntu.com:6969\r\nUser-Agent:MyWinBittorrent\r\nConnection:keep-alive");

    return szDstURL;
}

int CTCPTracker::GetTrackerState()
{
    return m_nTrackerState;
}

void CTCPTracker::ParseTrackerResponse()
{
    int m = 0;
}

size_t CTCPTracker::OnRecvData( void *pBuffer, size_t nSize, size_t nMemb, void *ptr )
{
    CTCPTracker *pTracker = (CTCPTracker *)ptr;
    pTracker->m_strTrackerResponse.append((char *)pBuffer, nSize * nMemb);

    return nSize * nMemb;
}
