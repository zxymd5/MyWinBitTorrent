#include "PeerAcceptor.h"

CPeerAcceptor::CPeerAcceptor(void)
{
}

CPeerAcceptor::~CPeerAcceptor(void)
{
}

bool CPeerAcceptor::Startup()
{
    for (int i = 7681; i < 7690; ++i)
    {
        CreateTCPSocket();

        if (CWinSocket::Bind(NULL, i) == true)
        {
            m_nPort = i;
            CWinSocket::Listen();
            CWinSocket::SetReactor(m_pTask->GetSocketReactor());
            CWinSocket::SetHandleMask(READ_MASK);
            //m_pTask->GetUPnpNAT()->AddPortMap(m_nPort, "TCP");

            return true;
        }
    }

    return false;
}

void CPeerAcceptor::Shutdown()
{
    RemoveHandleMask(READ_MASK);
    CWinSocket::SetReactor(NULL);
    CWinSocket::Close();
}

int CPeerAcceptor::GetPort()
{
    return m_nPort;
}

void CPeerAcceptor::SetTorrentTask( ITorrentTask *pTask )
{
    m_pTask = pTask;
}

ITorrentTask * CPeerAcceptor::GetTorrentTask()
{
    return m_pTask;
}

int CPeerAcceptor::HandleRead()
{
    string strIpAddr;
    int nPort;
    int fd = CWinSocket::Accept(strIpAddr, nPort);
    if (fd != -1)
    {
        if (m_pTask->GetPeerManager()->AddAcceptedPeer(fd, strIpAddr.c_str(), nPort))
        {
            closesocket(fd);
        }
    }

    return 0;
}

int CPeerAcceptor::HandleWrite()
{
    RemoveHandleMask(WRITE_MASK);
    return 0;
}

void CPeerAcceptor::HandleClose()
{

}
