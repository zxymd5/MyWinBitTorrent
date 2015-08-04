#ifndef TRACKER_MANAGER_H
#define TRACKER_MANAGER_H

#include "mywinbittorrent.h"
#include <vector>
#include <process.h>

using namespace std;

class CTrackerManager :
    public ITrackerManager
{
public:
    CTrackerManager(void);
    virtual ~CTrackerManager(void);
    virtual bool Startup();
    virtual void Shutdown();
    virtual long long GetSeedCount();
    virtual long long GetPeerCount();
    virtual void SetTorrentTask(ITorrentTask *pTask);
    virtual ITorrentTask *GetTorrentTask();
    void Svc();
    void CreateTrackers();
    void DestroyTrackers();
    void ShutdownTrackers();
    static unsigned int __stdcall ThreadFunc(void *pParam);

private:
    ITorrentTask *m_pTorrentTask;
    long long   m_llPeerCount;
    long long   m_llSeedCount;
    HANDLE m_hTrackerThread;
    vector<ITracker *> m_vecTracker;
    bool m_bExit;

};

#endif