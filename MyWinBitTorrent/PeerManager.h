#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include "mywinbittorrent.h"
#include <map>
#include <process.h>

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
    string GenPeerLinkID(const char *pIPAddr, int nPort);
    bool PeerExists(string strPeerLinkID);
    void CheckPeerConnection();
    void CheckPeerChoke();
    void ComputePeerSpeed();

private:
    ITorrentTask *m_pTorrentTask;
    map<string, PeerInfo> m_mapUnusedPeer;
    map<string, PeerInfo> m_mapConnectingPeer;
    map<string, PeerInfo> m_mapConnectedPeer;
    map<string, PeerInfo> m_mapBanedPeer;
    int m_nConnectTimerID;
    int m_nChokeTimerID;
    
    CRITICAL_SECTION m_csUnusedPeer;
};

#endif