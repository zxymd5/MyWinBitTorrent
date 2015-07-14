#ifndef PEER_LINK_H
#define PEER_LINK_H

#include "mywinbittorrent.h"
#include "WinSocket.h"

class CPeerLink :
    public IPeerLink,
    public ITimerCallback,
    public CWinSocket,
    public IRateMeasure
{
public:
    CPeerLink(void);
    virtual ~CPeerLink(void);
    virtual void SetPeerManager(IPeerManager *pManager);
    virtual int GetPeerState();
    virtual void Connect(const char *IpAddr, int nPort);
    virtual void CloseLink();
    virtual bool IsAccepted();
    virtual bool ShouldClose();
    virtual void ComputeSpeed();
    
    virtual void OnTimer(int nTimerID);

    virtual void AddClient(IRateMeasureClient *pClient);
    virtual void RemoveClient(IRateMeasureClient *pClient);
    virtual void Update();
    virtual void SetUploadSpeed(long long llSpeed);
    virtual void SetDownloadSpeed(long long llSpeed);
    virtual long long GetUploadSpeed();
    virtual long long GetDownloadSpeed();

private:
    int m_nPeerState;
    IPeerManager *m_pPeerManager;
    string  m_strIPAddr;
    int m_nPort;
    string m_strPeerLinkID;
    int m_nConnTimeoutID;

};

#endif