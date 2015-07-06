#ifndef TORRENT_TASK_H
#define TORRENT_TASK_H

#include "mywinbittorrent.h"
#include <vector>
#include <process.h>

using namespace std;

class CTorrentTask :
    public ITorrentTask,
    public ITimerCallback
{
public:
    CTorrentTask(void);
    ~CTorrentTask(void);
    virtual bool Startup();
    virtual void Shutdown();
    const string &GetPeerID();
    void GenPeerID();
    void Reset();
    void Svc();
    virtual void LoadTorrentFile(const char *pTorrentFilePath);
    virtual ITorrentFile *GetTorrentFile();
    virtual IWinSocketReactor *GetSocketReactor();
    virtual IUPnpNAT *GetUPnpNAT();
    virtual IPeerAcceptor *GetAcceptor();
    virtual ITaskStorage *GetTaskStorage();

    virtual long long GetDownloadCount();
    virtual long long GetUploadCount();

    virtual void OnTimer(int nTimerID);

    static unsigned int __stdcall ThreadFunc(void *pParam);

private:
    string m_strPeerID;
    long long m_llDownloadCount;
    long long m_llUploadCount;
    long long m_llDownloadSpeed;
    long long m_llUploadSpeed;
    long long m_llLastDownloadCount;
    long long m_llLastUploadCount;
    long long m_llLastCheckSpeedTime;
    vector<long long> m_vecDownloadSpeed;
    vector<long long> m_vecUploadSpeed;
    int m_nSpeedTimerID;
    int m_nMaxPeerLink;
    int m_nMaxUploadPeerLink;
    long long m_llCacheSize;
    HANDLE m_hTaskThread;
    bool m_bExit;

    ITorrentFile *m_pTorrentFile;
    IWinSocketReactor *m_pSocketReactor;
    IUPnpNAT    *m_pUPnpNAT;
    IRateMeasure *m_pRateMeasure;
    IPeerAcceptor *m_pPeerAcceptor;
    ITaskStorage  *m_pTaskStorage;
    ITrackManager *m_pTrackerManager;
    
};

#endif