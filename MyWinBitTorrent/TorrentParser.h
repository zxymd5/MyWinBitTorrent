#ifndef TORRENT_PARSER_H
#define TORRENT_PARSER_H

#include "CommDef.h"
#include "MyWinBitTorrent.h"
#include <vector>

class CTorrentParser
{
public:
    CTorrentParser(void);
    ~CTorrentParser(void);
    int LoadTorrentFile(const char *pTorrentFile);
    static bool FindPattern(const char *pContent, const char *pPattern, int &nStart, int &nEnd);
    bool GetMainAnnounce(string &strAnnounce);
    bool GetAnnounceList(vector<string> &vecAnnounce);
    bool GetFileList(vector<FileInfo> &vecFileInfo);
    bool GetFile(string &strFileName);
    bool IsMultiFiles();
    bool GetPieceLength(int &nPieceLength);
    bool GetPiecesHash(string &strPieceHash);
    bool GetComment(string &strComment);
    bool GetCreatedBy(string &strCreatedBy);
    bool GetCreationDate(string &strCreationDate);
    bool GetName(string &strName);
    bool GetInfoHash(unsigned char szInfoHash[20]);
    static bool ParseTrackInfo(const char *pAnnounce, string &strTrackerURL, int &nPort, string &strPath );
    static bool ParseTrackResponse(const string &strResponse);
private:
    char *m_lpContent;
    __int64 m_llFileSize;
};

#endif
