#include "UDPTracker.h"

CUDPTracker::CUDPTracker(void)
{
}

CUDPTracker::~CUDPTracker(void)
{
}

void CUDPTracker::SetTrackerManager( ITrackManager *pTrackerManager )
{
    
}

bool CUDPTracker::IsProtocolSupported( const char * pProtocol )
{
    return true;
}

void CUDPTracker::SetURL( const char *pUrl )
{

}

void CUDPTracker::Update()
{

}

void CUDPTracker::Shutdown()
{

}

int CUDPTracker::GetSeedCount()
{
    return 0;
}

int CUDPTracker::GetPeerCount()
{
    return 0;
}

int CUDPTracker::GetInterval()
{
    return 0;
}

long long CUDPTracker::GetNextUpdateTick()
{
    return 0;
}

int CUDPTracker::GetTrackerState()
{
    return m_nTrackerState;
}

int CUDPTracker::HandleWrite()
{
    return 0;
}
