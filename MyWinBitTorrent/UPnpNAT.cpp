#include "UPnpNAT.h"

CUPnpNAT::CUPnpNAT(void) : m_pSocketReactor(NULL)
{
}

CUPnpNAT::~CUPnpNAT(void)
{
}

void CUPnpNAT::SetSocketReactor( IWinSocketReactor *pReactor )
{
    m_pSocketReactor = pReactor;
}

void CUPnpNAT::Startup()
{

}

void CUPnpNAT::Shutdown()
{

}

void CUPnpNAT::AddPortMap( int nPort, const char *pProtocol )
{

}

void CUPnpNAT::RemovePortMap( int nPort, const char *pProtocol )
{

}
