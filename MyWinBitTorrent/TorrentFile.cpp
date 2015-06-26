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

const string & CTorrentFile::GetTorrentFilePath()
{
    return m_strTorrentFilePath;
}

const string & CTorrentFile::GetMainAnnounce()
{
    return m_strMainAnnounce;
}

const vector<FileInfo> & CTorrentFile::GetFileList()
{
    return m_vecFileList;
}

int CTorrentFile::GetPieceLength()
{
    return m_nPieceLength;
}

const string & CTorrentFile::GetPiecesHash()
{
    return m_strPiecesHash;
}

const string & CTorrentFile::GetComment()
{
    return m_strComment;
}

const string & CTorrentFile::GetCreatedBy()
{
    return m_strCreatedBy;
}

const string & CTorrentFile::GetCreationDate()
{
    return m_strCreationDate;
}

bool CTorrentFile::IsMultiFiles()
{
    return m_bMultiFiles;
}

const unsigned char * CTorrentFile::GetInfoHash()
{
    return m_szInfoHash;
}

long long CTorrentFile::GetTotalFileSize()
{
    long long llTotalSize = 0;
    vector<FileInfo>::iterator it = m_vecFileList.begin();
    for (; it != m_vecFileList.end(); ++it)
    {
        llTotalSize += it->llFileSize;
    }

    return llTotalSize;
}
