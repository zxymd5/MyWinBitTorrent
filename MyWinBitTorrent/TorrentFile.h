#ifndef TORRENT_FILE_H
#define TORRENT_FILE_H

#include "mywinbittorrent.h"

class CTorrentFile :
    public ITorrentFile
{
public:
    CTorrentFile(void);
    virtual ~CTorrentFile(void);
    virtual void Load(const char *pFilePath);
    virtual void SetTorrentTask(ITorrentTask *pTask);
    virtual ITorrentTask *GetTorrentTask();
    virtual const string &GetTorrentFilePath();
    virtual const string &GetMainAnnounce();
    virtual const vector<FileInfo> &GetFileList();
    virtual const vector<string> &GetAnnounceList();
    virtual int GetPieceLength();
    virtual const string &GetPiecesHash();
    virtual const string &GetComment();
    virtual const string &GetCreatedBy();
    virtual const string &GetCreationDate();
    virtual bool IsMultiFiles();
    virtual const unsigned char *GetInfoHash();
    virtual long long GetTotalFileSize();

private:
    string m_strTorrentFilePath;
    string m_strMainAnnounce;
    vector<FileInfo> m_vecFileList;
    vector<string> m_vecAnnounceList;
    int m_nPieceLength;
    string m_strPiecesHash;
    string m_strComment;
    string m_strCreatedBy;
    string m_strCreationDate;
    bool m_bMultiFiles;
    unsigned char m_szInfoHash[20];
    
    ITorrentTask *m_pTorrentTask;
};

#endif
