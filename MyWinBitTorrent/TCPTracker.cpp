#include "TCPTracker.h"

CTCPTracker::CTCPTracker(void)
{
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
    m_nTrackerState = TS_CONNECTING;
    
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
    sprintf(szDstURL, "%s?info_hash=%s&peer_id=%s&port=%u&compact=1&uploaded=%llu&downloaded=%llu&left=%llu&event=%s",
            m_strTrackerURL.c_str());

    return szDstURL;
}
