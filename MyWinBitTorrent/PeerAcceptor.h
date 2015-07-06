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
    virtual bool Startup();
    virtual void Shutdown();
    virtual int GetPort();
    virtual void SetTorrentTask(ITorrentTask *pTask);
    virtual ITorrentTask *GetTorrentTask();

private:
    int m_nPort;
    ITorrentTask *m_pTask;
};

#endif