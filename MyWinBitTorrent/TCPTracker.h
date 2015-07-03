#ifndef TCP_TRACKER_H
#define TCP_TRACKER_H

#include "mywinbittorrent.h"
#include "WinSocket.h"

class CTCPTracker :
    public ITracker,
    public CWinSocket
{
public:
    CTCPTracker(void);
    virtual ~CTCPTracker(void);
    virtual void SetTrackerManager(ITrackManager *pTrackerManager);
    virtual bool IsProtocolSupported(const char * pProtocol);
    virtual void SetURL(const char *pUrl);
    virtual void Update();
    virtual void Shutdown();
    virtual long long GetSeedCount();
    virtual long long GetPeerCount();
    virtual long long GetInterval();
    virtual long long GetNextUpdateTick();
    int GetCurrentEvent();
    const char *Event2Str(int nEvent);
    string GenTrackerURL(const char *pEvent);
    virtual int GetTrackerState();
    virtual int HandleWrite();
    virtual int HandleRead();
    void OnConnect();
    void ParseTrackerResponse();

private:
    string m_strTrackerURL;
    string m_strTrackerResponse;

    int m_nTrackerState;
    long long m_llCompletedPeer;
    long long m_llInterval;
    long long m_llNextUpdateTick;
    long long m_llPeerCount;
    int m_nCurrentEvent;
    bool m_bSendStartEvent;
    bool m_bSendCompleteEvent;

    ITrackManager *m_pTrackerManager;
};

#endif