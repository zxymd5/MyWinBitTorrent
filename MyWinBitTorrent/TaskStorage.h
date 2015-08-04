#ifndef TASK_STORAGE_H
#define TASK_STORAGE_H

#include "mywinbittorrent.h"
#include "BitSet.h"
#include <map>

class CTaskStorage :
    public ITaskStorage
{
public:
    CTaskStorage(void);
    virtual ~CTaskStorage(void);
    virtual bool Startup();
    virtual void Shutdown();
    virtual void SetBanedFileList(list<int> lstBanedFile);
    virtual bool Finished();
    virtual IBitSet *GetBitSet();
    virtual IBitSet *GetBanedBitSet();
    virtual int GetPieceLength(int nPieceIndex);
    virtual int GetPieceTask(IBitSet *pBitSet);
    virtual void WritePiece(int nPieceIndex, string &strData);
    virtual string ReadData(int nPieceIndex, long long llOffset, int nLen);
    virtual string ReadPiece(int nPieceIndex);
    virtual float GetFinishedPercent();
    virtual long long GetLeftCount();
    virtual long long GetSelectedCount();
    virtual long long GetBanedCount();
    virtual void SetTorrentTask(ITorrentTask *pTorrentTask);
    virtual ITorrentTask *GetTorrentTask();
    virtual string GetBitField();
    virtual void AbandonPieceTask(int nPieceIndex);
private:
    ITorrentTask *m_pTorrentTask;
    string m_strDstDir;
    string m_strPsfPath;
    bool m_bNewTask;
    list<StorageFileInfo> m_lstStorageFile;
    CBitSet m_clBitSet;
    CBitSet m_clBanedBitSet;
    list<Range> m_lstRange;
    list<int> m_lstDownloadingPieces;
    unsigned int m_nFinishedPiece;
    map<int, string> m_mapWriteCache;
    map<int, PieceCache> m_mapReadCache;
    list<int> m_lstBanedFile;
    __int64 m_llBanedFileSize;

private:
    bool OpenFiles();
    bool ShouldCheckFiles();
    void CheckFiles();
    void MakeRequestRange();
    void SaveBitset();
    bool OpenFile(int nIndex, FileInfo stFileInfo);
    StorageFileInfo GetFileInfoByOffset(long long llOffset);
    unsigned int GetMaxReadCacheSize();
    unsigned int GetMaxWriteCacheSize();
    void WritePieceD(int nPieceIndex, string &strData);
    void SaveWriteCacheToDisk();
    string ReadDataD(int nPieceIndex, long long llOffset, int nLen);

    int GetPieceTaskInRange(IBitSet *pBitSet, int nBeginIndex, int nEndIndex);
    void RemoveDownloadingPiece(int nPieceIndex);

    void GenBanedBitSet();
    int GetPieceIndexByOffset(long long llOffset);
};

#endif