#ifndef PEER_LINK_H
#define PEER_LINK_H

#include "mywinbittorrent.h"

class CPeerLink :
    public IPeerLink
{
public:
    CPeerLink(void);
    virtual ~CPeerLink(void);
    virtual void SetPeerManager(IPeerManager *pManager);
    virtual int GetPeerState();
    virtual void Connect(const char *IpAddr, int nPort);
    virtual void CloseLink();
};

#endif