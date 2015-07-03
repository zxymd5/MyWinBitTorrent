#ifndef MY_WIN_BIT_TORRENT_H
#define MY_WIN_BIT_TORRENT_H

#include "CommDef.h"
#include <vector>

class IWinSocketReactor;
class IUPnpNAT;
class ITorrentTask;
class ITrackerManager;
class ITracker;
class IPeerAcceptor;
class ITaskStorage;

class IWinSocket
{
public:
    virtual ~IWinSocket(){};
    virtual void SetReactor(IWinSocketReactor *pReactor) = 0;
    virtual IWinSocketReactor *GetReactor() = 0;
    virtual void CreateTCPSocket() = 0;
    virtual void CreateUDPSocket() = 0;
    virtual int GetHandle() = 0;
    virtual int GetHandleMask() = 0;
    virtual void SetHandleMask(int nHandleMask) = 0;
    virtual void RemoveHandleMask(int nHandleMask) = 0;
    virtual void SetNonBlock() = 0;
    virtual void Close() = 0;
    virtual int HandleRead() = 0;
    virtual int HandleWrite() = 0;
    virtual void HandleClose() = 0;
    virtual bool Bind(const char *pIpAddr, int nPort) = 0;
    virtual void Listen() = 0;
    virtual void Connect( const char* pHostName, int nPort) = 0;
};

class IWinSocketReactor
{
public:
    virtual ~IWinSocketReactor() {};
    virtual bool AddSocket(IWinSocket *pSocket) = 0;
    virtual void RemoveSocket(IWinSocket *pSocket) = 0;
    virtual bool Startup() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
};

class ITorrentFile
{
public:
    virtual ~ITorrentFile() {};
    virtual void Load(const char *pFilePath) = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
    virtual const string &GetTorrentFilePath() = 0;
    virtual const string &GetMainAnnounce() = 0;
    virtual const vector<FileInfo> &GetFileList() = 0;
    virtual const vector<string> &GetAnnounceList() = 0;
    virtual int GetPieceLength() = 0;
    virtual const string &GetPiecesHash() = 0;
    virtual const string &GetComment() = 0;
    virtual const string &GetCreatedBy() = 0;
    virtual const string &GetCreationDate() = 0;
    virtual bool IsMultiFiles() = 0;
    virtual const unsigned char *GetInfoHash() = 0;
    virtual long long GetTotalFileSize() = 0;
};

class ITorrentTask
{
public:
    virtual ~ITorrentTask() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual const string &GetPeerID() = 0;
    virtual void LoadTorrentFile(const char *pTorrentFilePath) = 0;
    virtual ITorrentFile *GetTorrentFile() = 0;
    virtual IWinSocketReactor *GetSocketReactor() = 0;
    virtual IUPnpNAT *GetUPnpNAT() = 0;
    virtual IPeerAcceptor *GetAcceptor() = 0;
    virtual ITaskStorage *GetTaskStorage() = 0;

    virtual long long GetDownloadCount() = 0;
    virtual long long GetUploadCount() = 0;
};

class IPeerAcceptor
{
public:
    virtual ~IPeerAcceptor() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual int GetPort() = 0;
};

class IUPnpNAT
{
public:
    virtual ~IUPnpNAT() {};
    virtual void SetSocketReactor(IWinSocketReactor *pReactor) = 0;
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void AddPortMap(int nPort, const char *pProtocol) = 0;
    virtual void RemovePortMap(int nPort, const char *pProtocol) = 0;
};

class ITimerCallback
{
public:
    virtual ~ITimerCallback(){};
    virtual void OnTimer(int nTimerID) = 0;
};

class IRateMeasureClient
{
public:
    IRateMeasureClient() {};
    virtual void BlockWrite(bool bBlock) = 0;
    virtual void BlockRead(bool bBlock) = 0;

    virtual void SetWritePriority(int nPriority) = 0;
    virtual int GetWritePriority() = 0;
    virtual void SetReadPriority(int nPriority) = 0;
    virtual int GetReadPriority() = 0;

    virtual bool CanWrite() = 0;
    virtual bool CanRead() = 0;

    virtual int DoWrite(long long llCount) = 0;
    virtual int DoRead(long long llCount) = 0;
};

class IRateMeasure
{
public:
    virtual ~IRateMeasure() {};
    static const unsigned int nNoLimitedSpeed = 0xFFFFFFFF;
    virtual void AddClient(IRateMeasureClient *pClient) = 0;
    virtual void RemoveClient(IRateMeasureClient *pClient) = 0;
    virtual void Update() = 0;
    virtual void SetUploadSpeed(long long llSpeed) = 0;
    virtual void SetDownloadSpeed(long long llSpeed) = 0;
    virtual long long GetUploadSpeed() = 0;
    virtual long long GetDownloadSpeed() = 0;
};

class ITrackManager
{
public:
    virtual ~ITrackManager() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual long long GetSeedCount() = 0;
    virtual long long GetPeerCount() = 0;
    virtual void SetTorrentTask(ITorrentTask *pTask) = 0;
    virtual ITorrentTask *GetTorrentTask() = 0;
};

class ITracker 
{
public:
    virtual ~ITracker() {};
    virtual void SetTrackerManager(ITrackManager *pTrackerManager) = 0;
    virtual bool IsProtocolSupported(const char * pProtocol) = 0;
    virtual void SetURL(const char *pUrl) = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
    virtual long long GetSeedCount() = 0;
    virtual long long GetPeerCount() = 0;
    virtual long long GetInterval() = 0;
    virtual long long GetNextUpdateTick() = 0;
    virtual int GetTrackerState() = 0;
};

class ITaskStorage
{
public:
    virtual ~ITaskStorage() {};
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;
    virtual long long GetLeftCount() = 0;
};

#endif