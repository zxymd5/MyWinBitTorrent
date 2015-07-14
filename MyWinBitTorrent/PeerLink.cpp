#include "PeerLink.h"

CPeerLink::CPeerLink(void)
{
}

CPeerLink::~CPeerLink(void)
{
}

void CPeerLink::SetPeerManager( IPeerManager *pManager )
{

}

int CPeerLink::GetPeerState()
{
    return 0;
}

void CPeerLink::Connect( const char *IpAddr, int nPort )
{

}

void CPeerLink::CloseLink()
{

}

bool CPeerLink::IsAccepted()
{
    return true;
}

bool CPeerLink::ShouldClose()
{
    return true;
}

void CPeerLink::ComputeSpeed()
{

}

void CPeerLink::OnTimer( int nTimerID )
{

}

void CPeerLink::AddClient( IRateMeasureClient *pClient )
{

}

void CPeerLink::RemoveClient( IRateMeasureClient *pClient )
{

}

void CPeerLink::Update()
{

}

void CPeerLink::SetUploadSpeed( long long llSpeed )
{

}

void CPeerLink::SetDownloadSpeed( long long llSpeed )
{

}

long long CPeerLink::GetUploadSpeed()
{
    return 0;
}

long long CPeerLink::GetDownloadSpeed()
{
    return 0;
}
