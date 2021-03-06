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
    virtual IPeerManager *GetPeerManager();
    virtual IRateMeasure *GetRateMeasure();

    virtual void AddDownloadCount(int nCount);
    virtual void AddUploadCount(int nCount);
    virtual long long GetDownloadCount();
    virtual long long GetUploadCount();
    virtual int GetMaxPeerLink();
    virtual void SetMaxPeerLink(int nMaxPeerLink);
    virtual int GetMaxConnectingPeerLink();
    virtual string GetDstPath();
    virtual void SetDstPath(const char *pPath);
    virtual string GetTaskName();
    virtual long long GetCacheSize();
    virtual void SetCacheSize(long long llCacheSize);

    virtual void OnTimer(int nTimerID);
    virtual void SetBanedFileList(list<int> lstBanedFile);

    virtual int GetMaxUploadPeerLink();
    virtual void SetMaxUploadPeerLink(int nMaxUploadPeerLink);
    virtual long long CheckDownloadSpeed();
    virtual long long CheckUploadSpeed();

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
    list<long long> m_lstDownloadSpeed;
    list<long long> m_lstUploadSpeed;
    list<int> m_lstBanedFiles;
    int m_nSpeedTimerID;
    int m_nMaxPeerLink;
    int m_nMaxUploadPeerLink;
    long long m_llCacheSize;
    HANDLE m_hTaskThread;
    bool m_bExit;
    string m_strDstPath;

    ITorrentFile *m_pTorrentFile;
    IWinSocketReactor *m_pSocketReactor;
    IUPnpNAT    *m_pUPnpNAT;
    IRateMeasure *m_pRateMeasure;
    IPeerAcceptor *m_pPeerAcceptor;
    ITaskStorage  *m_pTaskStorage;
    ITrackerManager *m_pTrackerManager;
    IPeerManager *m_pPeerManager;
    
};

#endif