#include "TaskStorage.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "sha1.h"

CTaskStorage::CTaskStorage(void) : m_pTorrentTask(NULL)
{
}

CTaskStorage::~CTaskStorage(void)
{
}

bool CTaskStorage::Startup()
{
    m_strDstDir = m_pTorrentTask->GetDstPath();
    string strName = m_pTorrentTask->GetTorrentFile()->GetName();
    if (strName.size() > 0)
    {
        m_strPsfPath = m_strDstDir + strName + "\\" + m_pTorrentTask->GetTaskName() + ".psf";
    }
    else
    {
        m_strPsfPath = m_strDstDir + m_pTorrentTask->GetTaskName() + ".psf";
    }
    m_clBitSet.Alloc(m_pTorrentTask->GetTorrentFile()->GetPieceCount());
    m_bNewTask = true;
    m_llBanedFileSize = 0;

    MakeRequestRange();
    if (!OpenFiles())
    {
        return false;
    }
    
    GenBanedBitSet();
    if (!m_bNewTask && ShouldCheckFiles())
    {
        CheckFiles();
    }

    m_nFinishedPiece = m_clBitSet.GetSetCount();

    return true;
}

void CTaskStorage::Shutdown()
{
    SaveWriteCacheToDisk();

    list<StorageFileInfo>::iterator it = m_lstStorageFile.begin();
    for (; it != m_lstStorageFile.end(); ++it)
    {
        if (it->bBaned)
        {
            continue;
        }

        close(it->nHandle);
    }

    if (Finished())
    {
        unlink(m_strPsfPath.c_str());
    }
}

long long CTaskStorage::GetLeftCount()
{
    return (m_clBitSet.GetSize() - m_clBanedBitSet.GetSetCount() - m_nFinishedPiece) * (long long)m_pTorrentTask->GetTorrentFile()->GetPieceLength();
}

void CTaskStorage::SetTorrentTask( ITorrentTask *pTorrentTask )
{
    m_pTorrentTask = pTorrentTask;
}

ITorrentTask * CTaskStorage::GetTorrentTask()
{
    return m_pTorrentTask;
}

void CTaskStorage::AbandonPieceTask( int nPieceIndex )
{
    RemoveDownloadingPiece(nPieceIndex);
}

string CTaskStorage::GetBitField()
{
    if (m_clBitSet.IsEmpty())
    {
        return "";
    }

    return m_clBitSet.GetBits();
}

bool CTaskStorage::Finished()
{
    return m_nFinishedPiece == (m_clBitSet.GetSize() - m_clBitSet.GetSetCount());
}

void CTaskStorage::SetBanedFileList( list<int> lstBanedFile )
{
    m_lstBanedFile = lstBanedFile;
}

IBitSet * CTaskStorage::GetBitSet()
{
    return &m_clBitSet;
}

IBitSet * CTaskStorage::GetBanedBitSet()
{
    return &m_clBanedBitSet;
}

int CTaskStorage::GetPieceLength( int nPieceIndex )
{
    if (nPieceIndex < m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
    {
        return m_pTorrentTask->GetTorrentFile()->GetPieceLength();
    }

    return (int)(m_pTorrentTask->GetTorrentFile()->GetTotalFileSize() - (long long)(m_pTorrentTask->GetTorrentFile()->GetPieceLength() * nPieceIndex));
}

int CTaskStorage::GetPieceTask( IBitSet *pBitSet )
{
    for (int i = 0; i < m_lstRange.size(); ++i)
    {
        Range stRange = m_lstRange.front();
        m_lstRange.pop_front();
        m_lstRange.push_back(stRange);

        int nIndex = GetPieceTaskInRange(pBitSet, stRange.nBegin, stRange.nEnd);
        if (nIndex != -1)
        {
            return nIndex;
        }
    }

    list<int>::iterator it = m_lstDownloadingPieces.begin();
    for (; it != m_lstDownloadingPieces.end(); ++it)
    {
        if (pBitSet->IsSet(*it))
        {
            int nIndex = *it;
            m_lstDownloadingPieces.erase(it);
            m_lstDownloadingPieces.push_back(nIndex);
            return nIndex;
        }
    }

    return -1;
}

int CTaskStorage::GetPieceTaskInRange( IBitSet *pBitSet, int nBeginIndex, int nEndIndex )
{
    for (int i = nBeginIndex; i <= nEndIndex; ++i)
    {
        if (pBitSet->IsSet(i) 
            && !m_clBitSet.IsSet(i) 
            && !m_clBanedBitSet.IsSet(i))
        {
            list<int>::iterator it = m_lstDownloadingPieces.begin();
            for (; it != m_lstDownloadingPieces.end(); ++it)
            {
                if (*it == i)
                {
                    break;
                }
            }

            if (it != m_lstDownloadingPieces.end())
            {
                continue;
            }

            m_lstDownloadingPieces.push_back(i);
            return i;
        }
    }

    return -1;
}

void CTaskStorage::RemoveDownloadingPiece( int nPieceIndex )
{
    list<int>::iterator it = m_lstDownloadingPieces.begin();
    for (; it != m_lstDownloadingPieces.end(); ++it)
    {
        if (*it == nPieceIndex)
        {
            m_lstDownloadingPieces.erase(it);
            break;
        }
    }
}

void CTaskStorage::WritePiece( int nPieceIndex, string &strData )
{
    RemoveDownloadingPiece(nPieceIndex);

    if (m_clBitSet.IsSet(nPieceIndex))
    {
        return;
    }

    if (m_clBanedBitSet.IsSet(nPieceIndex))
    {
        return;
    }

    if (nPieceIndex > m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
    {
        return;
    }

    if (m_mapWriteCache.find(nPieceIndex) != m_mapWriteCache.end())
    {
        return;
    }

    m_mapWriteCache[nPieceIndex] = strData;
    if (m_mapWriteCache.size() > GetMaxWriteCacheSize())
    {
        SaveWriteCacheToDisk();
    }

    m_clBitSet.Set(nPieceIndex, true);
    m_nFinishedPiece++;

    if (m_clBitSet.GetSize() - m_clBanedBitSet.GetSetCount() == m_nFinishedPiece)
    {
        m_pTorrentTask->GetPeerManager()->OnDownloadComplete();
        SaveWriteCacheToDisk();
    }
}

void CTaskStorage::SaveWriteCacheToDisk()
{
    map<int, string>::iterator it = m_mapWriteCache.begin();
    for (; it != m_mapWriteCache.end(); ++it)
    {
        WritePieceD(it->first, it->second);
    }

    m_mapWriteCache.clear();

    SaveBitset();
}

void CTaskStorage::WritePieceD( int nPieceIndex, string &strData )
{
    if (nPieceIndex > m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
    {
        return;
    }
    
    int nLeft = strData.size();
    long long llGlobalOffset = (long long)nPieceIndex * (long long)m_pTorrentTask->GetTorrentFile()->GetPieceLength();

    for (; nLeft > 0;)
    {
        StorageFileInfo stInfo = GetFileInfoByOffset(llGlobalOffset);

        if (stInfo.bBaned)
        {
            nLeft -= (stInfo.stFileInfo.llOffset + stInfo.stFileInfo.llFileSize - llGlobalOffset);
            llGlobalOffset = stInfo.stFileInfo.llOffset + stInfo.stFileInfo.llFileSize;
            continue;
        }

        if (stInfo.nHandle == -1)
        {
            return;
        }

        long long llFileOffset = llGlobalOffset - stInfo.stFileInfo.llOffset;
        int nWriteLen = nLeft;
        if (nWriteLen > stInfo.stFileInfo.llFileSize - llFileOffset)
        {
            nWriteLen = stInfo.stFileInfo.llFileSize - llFileOffset;
        }

        _lseeki64(stInfo.nHandle, llFileOffset, SEEK_SET);
        nWriteLen = write(stInfo.nHandle, strData.data() + strData.size() - nLeft, nWriteLen);
        nLeft -= nWriteLen;
        llGlobalOffset += nWriteLen;
    }
}

void CTaskStorage::SaveBitset()
{
    FILE *fp = fopen(m_strPsfPath.c_str(), "w");
    if (fp == NULL)
    {
        return;
    }

    int nPieceCount = m_pTorrentTask->GetTorrentFile()->GetPieceCount();
    for (int i = 0; i < nPieceCount; ++i)
    {
        fputc(m_clBitSet.IsSet(i) ? '1' : '0', fp);
    }

    fclose(fp);
}

StorageFileInfo CTaskStorage::GetFileInfoByOffset( long long llOffset )
{
    list<StorageFileInfo>::iterator it = m_lstStorageFile.begin();
   
    for (; it != m_lstStorageFile.end(); ++it)
    {
        if (it->stFileInfo.llFileSize == 0)
        {
            continue;
        }

        if (it->stFileInfo.llOffset <= llOffset &&
            (it->stFileInfo.llOffset + it->stFileInfo.llFileSize) > llOffset)
        {
            return *it;
        }
    }

    StorageFileInfo stResult;
    stResult.nHandle = -1;
    return stResult;
}

string CTaskStorage::ReadData( int nPieceIndex, long long llOffset, int nLen )
{
    if (nPieceIndex > m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
    {
        return "";
    }

    if (nLen > m_pTorrentTask->GetTorrentFile()->GetPieceLength() - llOffset)
    {
        return "";
    }

    string strPieceData;
    bool bInReadCache = false;
    map<int, PieceCache>::iterator it = m_mapReadCache.find(nPieceIndex);
    if (it != m_mapReadCache.end())
    {
        strPieceData = it->second.strData;
        it->second.llLastAccessTime = GetTickCount();
        bInReadCache = true;
    }

    if (strPieceData.size() == 0)
    {
        map<int, string>::iterator it2 = m_mapWriteCache.find(nPieceIndex);
        if (it2 != m_mapWriteCache.end())
        {
            strPieceData = it2->second;
        }
    }

    if (strPieceData.size() == 0)
    {
        strPieceData = ReadDataD(nPieceIndex, 0, GetPieceLength(nPieceIndex));
    }

    if (strPieceData.size() < llOffset + nLen)
    {
        return "";
    }

    if (!bInReadCache)
    {
        PieceCache stCache;
        stCache.strData = strPieceData;
        stCache.llLastAccessTime = GetTickCount();

        m_mapReadCache[nPieceIndex] = stCache;

        for (; m_mapReadCache.size() > GetMaxReadCacheSize(); )
        {
            map<int, PieceCache>::iterator it = m_mapReadCache.begin();
            map<int, PieceCache>::iterator it2 = it;
            for (; it != m_mapReadCache.end(); ++it)
            {
                if (it2->second.llLastAccessTime > it->second.llLastAccessTime)
                {
                    it2 = it;
                }
            }
            m_mapReadCache.erase(it2);
        }
    }

    return strPieceData.substr((int)llOffset, nLen);
}

string CTaskStorage::ReadDataD( int nPieceIndex, long long llOffset, int nLen )
{
    if (nPieceIndex > m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
    {
        return "";
    }

    if (nLen > m_pTorrentTask->GetTorrentFile()->GetPieceLength() - llOffset)
    {
        return "";
    }

    char *pBuff = new char[nLen];
    int nLeft = nLen;
    long long llGlobalOffset = (long long)nPieceIndex * (long long)m_pTorrentTask->GetTorrentFile()->GetPieceLength() + llOffset;
    for (; nLeft > 0;)
    {
        StorageFileInfo stInfo = GetFileInfoByOffset(llGlobalOffset);
        if (stInfo.nHandle = -1)
        {
            delete [] pBuff;
            return "";
        }

        long long llFileOffset = llGlobalOffset - stInfo.stFileInfo.llOffset;
        int nReadLen = nLeft;
        if (nReadLen > stInfo.stFileInfo.llFileSize - llFileOffset)
        {
            nReadLen = stInfo.stFileInfo.llFileSize - llFileOffset;
        }

        _lseeki64(stInfo.nHandle, llFileOffset, SEEK_SET);
        nReadLen = read(stInfo.nHandle, pBuff + nLen - nLeft, nReadLen);
        nLeft -= nReadLen;

        llGlobalOffset += nReadLen;
    }

    string strDstBuff;
    strDstBuff.append((const char *)pBuff, nLen);
    return strDstBuff;
}

string CTaskStorage::ReadPiece( int nPieceIndex )
{
    return ReadData(nPieceIndex, 0, GetPieceLength(nPieceIndex));
}

bool CTaskStorage::OpenFile( int nIndex, FileInfo stFileInfo )
{
    bool bBaned = false;

    StorageFileInfo stInfo;
    string strFilePath;

    strFilePath = m_strDstDir + stFileInfo.strFilePath;
    list<int>::iterator it = m_lstBanedFile.begin();
    for (; it != m_lstBanedFile.end(); ++it)
    {
        if (*it == nIndex)
        {
            bBaned = true;
            break;
        }
    }

    stInfo.stFileInfo = stFileInfo;

    if (!bBaned)
    {
        CreateDir(strFilePath);
        int nHandle = open(strFilePath.c_str(), O_RDWR | O_CREAT);
        if (nHandle == -1)
        {
            return false;
        }

        struct _stat64 st;
        _fstati64(nHandle, &st);
        if (st.st_size != 0)
        {
            m_bNewTask = false;
        }

        if (st.st_size != stFileInfo.llFileSize)
        {
            _lseeki64(nHandle, stFileInfo.llFileSize, SEEK_SET);
            HANDLE hFile = (HANDLE)_get_osfhandle(nHandle);
            SetEndOfFile(hFile);
        }

        stInfo.nHandle = nHandle;
        stInfo.mtime = st.st_mtime;
        stInfo.bBaned = false;
    }
    else
    {
        stInfo.nHandle = -1;
        stInfo.mtime = 0;
        stInfo.bBaned = true;

        m_llBanedFileSize += stFileInfo.llFileSize;
    }

    m_lstStorageFile.push_back(stInfo);
    return true;
}

bool CTaskStorage::OpenFiles()
{
    int nCount = m_pTorrentTask->GetTorrentFile()->GetFileCount();
    for (int i = 0; i < nCount; i++)
    {
        if (!OpenFile(i, m_pTorrentTask->GetTorrentFile()->GetFileList().at(i)))
        {
            return false;
        }
    }

    return true;
}

bool CTaskStorage::ShouldCheckFiles()
{
    bool bCheck = true;
    int psf = open(m_strPsfPath.c_str(), O_RDWR | O_CREAT);
    if (psf != -1)
    {
        struct stat st;
        fstat(psf, &st);
        if (st.st_size == 0)
        {
            lseek(psf, 1, SEEK_SET);
            HANDLE hFile = (HANDLE)_get_osfhandle(psf);
            SetEndOfFile(hFile);
        }

        bool bFileModified = false;
        list<StorageFileInfo>::iterator it = m_lstStorageFile.begin();
        for (; it != m_lstStorageFile.end(); ++it)
        {
            if (it->bBaned)
            {
                continue;
            }

            if (difftime(it->mtime, st.st_mtime) > 0)
            {
                bFileModified = true;
                break;
            }
        }

        if (!bFileModified)
        {
            FILE *fp = fdopen(psf, "r");
            int nIndex = 0;
            for (;;)
            {
                int nRet = fgetc(fp);
                if (nRet == EOF)
                {
                    break;
                }

                m_clBitSet.Set(nIndex, (char)nRet == '1' ? true : false);
                if (nIndex == m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1)
                {
                    bCheck = false;
                    break;
                }
                nIndex++;
            }
            fclose(fp);
        }
        close(psf);
    }

    return bCheck;
}

void CTaskStorage::CheckFiles()
{
    FILE *fp = fopen(m_strPsfPath.c_str(), "w");
    if (fp == NULL)
    {
        return;
    }

    int nCount = m_pTorrentTask->GetTorrentFile()->GetPieceCount();
    for (int i = 0; i < nCount; ++i)
    {
        string strPiece = ReadPiece(i);
        
        char szInfoHash[20];
        sha1_block((unsigned char *)strPiece.c_str(), strPiece.length(), (unsigned char *)szInfoHash);
        string strHash;
        strHash.append(szInfoHash, 20);

        char c;
        if (strHash == m_pTorrentTask->GetTorrentFile()->GetPieceHash(i))
        {
            m_clBitSet.Set(i, true);
            c = '1';
        }
        else
        {
            m_clBitSet.Set(i, false);
            c = '0';
        }

        fputc(c, fp);
    }

    fclose(fp);
}

void CTaskStorage::MakeRequestRange()
{
    int nNextBegin = 0;
    bool bRangeListDone = false;
    while(!bRangeListDone)
    {
        Range stRange;
        stRange.nBegin = nNextBegin;
        stRange.nEnd = nNextBegin + m_pTorrentTask->GetTorrentFile()->GetPieceCount() / 4;

        if (stRange.nEnd == 0 ||
            stRange.nEnd >= m_pTorrentTask->GetTorrentFile()->GetPieceCount())
        {
            stRange.nEnd = m_pTorrentTask->GetTorrentFile()->GetPieceCount() - 1;
            bRangeListDone = true;
        }
    
        nNextBegin = stRange.nEnd + 1;
        m_lstRange.push_back(stRange);
    }
}

void CTaskStorage::GenBanedBitSet()
{
    m_clBanedBitSet.Alloc(m_pTorrentTask->GetTorrentFile()->GetPieceCount());
    
    list<StorageFileInfo>::iterator it = m_lstStorageFile.begin();
    for (; it != m_lstStorageFile.end(); ++it)
    {
        if (it->stFileInfo.llFileSize == 0)
        {
            continue;
        }

//         if (!it->bBaned)
//         {
//             continue;
//         }

        int nBeginIndex = GetPieceIndexByOffset(it->stFileInfo.llOffset);
        int nEndIndex = GetPieceIndexByOffset(it->stFileInfo.llOffset + it->stFileInfo.llFileSize - 1);

        for (int i = nBeginIndex; i <= nEndIndex; ++i)
        {
            m_clBanedBitSet.Set(i, it->bBaned);
        }
    }

//     it = m_lstStorageFile.begin();
//     for (; it != m_lstStorageFile.end(); ++it)
//     {
//         if (it->stFileInfo.llFileSize == 0)
//         {
//             continue;
//         }
// 
//         if (it->bBaned)
//         {
//             continue;
//         }
// 
//         int nBeginIndex = GetPieceIndexByOffset(it->stFileInfo.llOffset);
//         int nEndIndex = GetPieceIndexByOffset(it->stFileInfo.llOffset + it->stFileInfo.llFileSize - 1);
// 
//         m_clBanedBitSet.Set(nBeginIndex, false);
//         m_clBanedBitSet.Set(nEndIndex, false);
//     }
}

float CTaskStorage::GetFinishedPercent()
{
    long long llTotal = m_clBitSet.GetSize() - m_clBanedBitSet.GetSetCount();
    if (llTotal == 0)
    {
        return 1.0f;
    }

    return (double)m_nFinishedPiece / (double)llTotal;
}

long long CTaskStorage::GetSelectedCount()
{
    return m_pTorrentTask->GetTorrentFile()->GetTotalFileSize() - m_llBanedFileSize;
}

long long CTaskStorage::GetBanedCount()
{
    return m_llBanedFileSize;
}

unsigned int CTaskStorage::GetMaxReadCacheSize()
{
    unsigned int nCachedPieceCount = m_pTorrentTask->GetCacheSize() / m_pTorrentTask->GetTorrentFile()->GetPieceLength();
    if (Finished())
    {
        return nCachedPieceCount;
    }

    return 2 * nCachedPieceCount / 5;
}

unsigned int CTaskStorage::GetMaxWriteCacheSize()
{
    unsigned int nCachedPieceCount = m_pTorrentTask->GetCacheSize() / m_pTorrentTask->GetTorrentFile()->GetPieceLength();
    if (Finished())
    {
        return 0;
    }

    return 3 * nCachedPieceCount / 5;
}

int CTaskStorage::GetPieceIndexByOffset( long long llOffset )
{
    return llOffset / m_pTorrentTask->GetTorrentFile()->GetPieceLength();
}
