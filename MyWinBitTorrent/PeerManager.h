#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include "mywinbittorrent.h"
#include <map>

class CPeerManager :
    public IPeerManager,
    public ITimerCallback
{
public:
    CPeerManager(void);
    virtual ~CPeerManager(void);
    virtual bool Startup();
    virtual void Shutdown();
    virtual void AddPeerInfo(const char *pIpAddr, int nPort);
    virtual void SetTorrentTask(ITorrentTask *pTask);
    virtual ITorrentTask *GetTorrentTask();
    virtual void OnTimer(int nTimerID);

private:
    ITorrentTask *m_pTorrentTask;
    map<string, PeerInfo> m_mapUnusedPeer;
    map<string, PeerInfo> m_mapConnectingPeer;
    map<string, PeerInfo> m_mapConnectedPeer;
    map<string, PeerInfo> m_mapBanedPeer;
    int m_nConnectTimerID;
    int m_nChokeTimerID;


};

#endif