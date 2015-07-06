#include "WinSocket.h"
#include <errno.h>

CWinSocket::CWinSocket(void)
            : m_nHandle(INVALID_SOCKET), 
              m_bInReactor(false),
              m_pReactor(NULL),
              m_nHandleMask(0)
{
}

CWinSocket::~CWinSocket(void)
{
    Close();
}

void CWinSocket::SetReactor( IWinSocketReactor *pReactor )
{
    if (pReactor != NULL)
    {
        if (m_bInReactor)
        {
            pReactor->RemoveSocket(this);
            m_bInReactor = false;
        }
        m_pReactor = NULL;
    }

    if (pReactor != NULL)
    {
        m_pReactor = pReactor;
        m_pReactor->AddSocket(this);
        m_bInReactor = true;
    }
}

IWinSocketReactor * CWinSocket::GetReactor()
{
    return m_pReactor;
}

void CWinSocket::CreateTCPSocket()
{
    m_nHandle = socket(AF_INET, SOCK_STREAM, 0);
    if (m_nHandle == INVALID_SOCKET)
    {
        HandleErrMsg("Create socket failed", __FILE__, WSAGetLastError(), __LINE__);
        return;
    }
    SetNonBlock();
}

void CWinSocket::CreateUDPSocket()
{
    m_nHandle = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_nHandle == INVALID_SOCKET)
    {
        HandleErrMsg("Create socket failed", __FILE__, WSAGetLastError(), __LINE__);
        return;
    }
    SetNonBlock();
}

int CWinSocket::GetHandle()
{
    return m_nHandle;
}

int CWinSocket::GetHandleMask()
{
    return m_nHandleMask;
}

void CWinSocket::SetNonBlock()
{
    u_long iMode = 1;
    if (ioctlsocket(m_nHandle, FIONBIO, &iMode) != 0)
    {
        HandleErrMsg("SetNoBlock failed", __FILE__, WSAGetLastError(), __LINE__);
    }
}

void CWinSocket::Close()
{
    if (m_nHandle != INVALID_SOCKET)
    {
        if (closesocket(m_nHandle) != 0)
        {
            HandleErrMsg("Close failed", __FILE__, WSAGetLastError(), __LINE__);
        }
        m_nHandle = INVALID_SOCKET;
    }
}

int CWinSocket::HandleRead()
{
    return 0;
}

int CWinSocket::HandleWrite()
{
    return 0;
}

void CWinSocket::HandleClose()
{
    GetReactor()->RemoveSocket(this);
}

bool CWinSocket::Bind( const char *pIpAddr, int nPort )
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    if (pIpAddr != NULL)
    {
        addr.sin_addr.s_addr = inet_addr(pIpAddr);
    }
    else
    {
        addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    }
    addr.sin_port = htons(nPort);

    return bind(m_nHandle, (const sockaddr*)&addr, sizeof(addr)) == 0;
}

void CWinSocket::Listen()
{
    listen(m_nHandle, 5);
}

void CWinSocket::SetHandleMask( int nHandleMask )
{
    m_nHandleMask = nHandleMask;
}

void CWinSocket::RemoveHandleMask( int nHandleMask )
{
    m_nHandleMask = m_nHandleMask & (~nHandleMask);
}

void CWinSocket::GetRemotAddrInfo( const char* pHostName, int nPort, sockaddr_in &stRemoteAddr )
{
    stRemoteAddr.sin_family=AF_INET;
    stRemoteAddr.sin_port=htons(nPort);

    hostent *hst=NULL;
    struct in_addr ia;
    hst=gethostbyname(pHostName);

    if (hst == NULL)
    {
        HandleErrMsg("Gethostbyname failed", __FILE__, WSAGetLastError(), __LINE__);
        return;
    }

    memcpy(&ia.S_un.S_addr,hst->h_addr_list[0],sizeof(ia.S_un.S_addr));

    //Fill RemoteAddr
    stRemoteAddr.sin_addr=ia;    
}

void CWinSocket::Connect( const char* pHostName, int nPort )
{
    struct sockaddr_in stRemoteAddr;
    GetRemotAddrInfo(pHostName, nPort, stRemoteAddr);
    connect(m_nHandle, (const sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
}
