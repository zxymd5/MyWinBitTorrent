#ifndef UDP_TRACKER_H
#define UDP_TRACKER_H

#include "mywinbittorrent.h"

class CUDPTracker :
    public ITracker
{
public:
    CUDPTracker(void);
    virtual ~CUDPTracker(void);

    virtual void SetTrackerManager(ITrackManager *pTrackerManager);
    virtual bool IsProtocolSupported(const char * pProtocol);
    virtual void SetURL(const char *pUrl);
    virtual void Update();
    virtual void Shutdown();
    virtual long long GetSeedCount();
    virtual long long GetPeerCount();
    virtual long long GetInterval();
    virtual long long GetNextUpdateTick();
};

#endif