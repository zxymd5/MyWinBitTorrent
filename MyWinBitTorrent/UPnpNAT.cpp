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

bool CUPnpNAT::Startup()
{
    return true;
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
