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

void CPeerLink::SendData( const void *pData, int nLen )
{

}

int CPeerLink::ProcRecvData()
{
    return 0;
}

void CPeerLink::SendHandShake()
{

}

void CPeerLink::SendBitField()
{

}

void CPeerLink::SendChoke( bool bChoke )
{

}

void CPeerLink::SendInterested( bool bInterested )
{

}

void CPeerLink::SendHave( int nPieceIndex )
{

}

void CPeerLink::SendPieceRequest( int nPieceIndex, int nOffset, int nLen )
{

}

void CPeerLink::SendPieceData( int nPieceIndex, int nOffset, string &strData )
{

}

void CPeerLink::SendPieceCancel( int nPieceIndex, int nOffset, int nLen )
{

}

void CPeerLink::CheckHandshake( string strInfo )
{

}

void CPeerLink::ProcCmd( int nCmd, void *pData, int nDataLen )
{

}

int CPeerLink::ProcCmdChoke( void *pData, int nDataLen )
{
    return 0;
}

int CPeerLink::ProcCmdUnchoke( void *pData, int nDataLen )
{
    return 0;
}

int CPeerLink::ProcCmdInterested( void *pData, int nDataLen )
{
    return 0;
}
