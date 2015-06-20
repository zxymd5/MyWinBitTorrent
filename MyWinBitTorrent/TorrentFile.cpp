#include "TorrentFile.h"
#include "TorrentParser.h"

CTorrentFile::CTorrentFile(void)
{
    memset(m_szInfoHash, 0, sizeof(m_szInfoHash));
}

CTorrentFile::~CTorrentFile(void)
{
}

void CTorrentFile::Load( const char *pFilePath )
{
    m_strTorrentFilePath.assign(pFilePath);
    CTorrentParser clParser;
    clParser.LoadTorrentFile(pFilePath);
    bool bFind = clParser.GetMainAnnounce(m_strMainAnnounce);
    clParser.GetAnnounceList(m_vecAnnounceList);
    clParser.GetFileList(m_vecFileList);
    m_bMultiFiles = clParser.IsMultiFiles();
    clParser.GetPieceLength(m_nPieceLength);
    clParser.GetPiecesHash(m_strPiecesHash);
    clParser.GetComment(m_strComment);
    clParser.GetCreatedBy(m_strCreatedBy);
    clParser.GetCreationDate(m_strCreationDate);
    clParser.GetInfoHash(m_szInfoHash);

    if (bFind)
    {
        m_vecAnnounceList.push_back(m_strMainAnnounce);
    }
}
