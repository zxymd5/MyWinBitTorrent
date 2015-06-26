#ifndef PEER_ACCEPTOR_H
#define PEER_ACCEPTOR_H

#include "mywinbittorrent.h"
#include "WinSocket.h"

class CPeerAcceptor :
    public IPeerAcceptor,
    public CWinSocket
{
public:
    CPeerAcceptor(void);
    ~CPeerAcceptor(void);
    virtual void Startup();
    virtual void Shutdown();
    virtual int GetPort();

private:
    int m_nPort;
};

#endif