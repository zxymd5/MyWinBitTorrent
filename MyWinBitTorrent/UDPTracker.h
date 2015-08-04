#ifndef UDP_TRACKER_H
#define UDP_TRACKER_H

#include "mywinbittorrent.h"
#include "WinSocket.h"

class CUDPTracker :
    public ITracker, 
    public CWinSocket
{
public:
    CUDPTracker(void);
    virtual ~CUDPTracker(void);

    virtual void SetTrackerManager(ITrackerManager *pTrackerManager);
    virtual bool IsProtocolSupported(const char * pProtocol);
    virtual void SetURL(const char *pUrl);
    virtual void Update();
    virtual void Shutdown();
    virtual int GetSeedCount();
    virtual int GetPeerCount();
    virtual int GetInterval();
    virtual long long GetNextUpdateTick();
    virtual int GetTrackerState();

    virtual int HandleWrite(); 
    virtual int HandleRead();
    virtual void HandleClose();

private:
    void SendConnectMsg();
    void SendAnnounceMsg();
    void ProcessMsg(unsigned int nActionID, char *pData, int nLen);
    int GetCurrentEvent();

    string m_strTrackerURL;
    string m_strTrackerResponse;

    int m_nTrackerState;
    int m_nCompletePeer;
    int m_nInterval;
    long long m_llNextUpdateTick;
    int m_nPeerCount;
    int m_nCurrentEvent;
    bool m_bSendStartEvent;
    bool m_bSendCompleteEvent;

    ITrackerManager *m_pTrackerManager;
    struct sockaddr_in m_stServerAddr;
    unsigned int m_nTransID;
    unsigned long long m_llConnectionID;

};

#endif