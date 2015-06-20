#ifndef TORRENT_FILE_H
#define TORRENT_FILE_H

#include "mywinbittorrent.h"

#include <vector>

class CTorrentFile :
    public ITorrentFile
{
public:
    CTorrentFile(void);
    virtual ~CTorrentFile(void);
    virtual void Load(const char *pFilePath);

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

};

#endif
