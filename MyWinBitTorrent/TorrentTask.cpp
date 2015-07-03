#include "TorrentTask.h"
#include "TorrentFile.h"
#include "SelectReactor.h"
#include "RateMeasure.h"

#include <time.h>

CTorrentTask::CTorrentTask(void) : m_pTorrentFile(NULL), 
                                   m_pSocketReactor(NULL), 
                                   m_pRateMeasure(NULL),
                                   m_pUPnpNAT(NULL),
                                   m_pPeerAcceptor(NULL),
                                   m_pTaskStorage(NULL),
                                   m_hTaskThread(INVALID_HANDLE_VALUE),
                                   m_bExit(false)
{

}

CTorrentTask::~CTorrentTask(void)
{
}

bool CTorrentTask::Startup()
{
    GenPeerID();
    Reset();

    m_pSocketReactor = new CSelectReactor;
    m_pRateMeasure = new CRateMeasure;

    m_pSocketReactor->Startup();

    m_hTaskThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void *)this, 0, NULL);

    return true;
}

const string & CTorrentTask::GetPeerID()
{
    return m_strPeerID;
}

void CTorrentTask::GenPeerID()
{
    char szPeerID[21];
    memset(szPeerID, 0, 21);
    srand((unsigned)time(NULL));
    sprintf(szPeerID, "-WB1111-%12d", rand());
    m_strPeerID.assign(szPeerID);
}

void CTorrentTask::Reset()
{
    m_llDownloadCount = 0;
    m_llUploadCount = 0;
    m_llDownloadSpeed = 0;
    m_llUploadSpeed = 0;
    m_llLastDownloadCount = 0;
    m_llLastUploadCount = 0;
    m_llLastCheckSpeedTime = 0;
    m_vecDownloadSpeed.clear();
    m_vecUploadSpeed.clear();
    m_nSpeedTimerID = 0;
    m_nMaxPeerLink = 100;
    m_nMaxUploadPeerLink = 6;
    m_llCacheSize = 5 * 1024 * 1024;
}

void CTorrentTask::LoadTorrentFile( const char *pTorrentFilePath )
{
    m_pTorrentFile = new CTorrentFile;
    m_pTorrentFile->Load(pTorrentFilePath);
}

void CTorrentTask::Shutdown()
{
    if (m_pTorrentFile)
    {
        delete m_pTorrentFile;
        m_pTorrentFile = NULL;
    }

    if (m_pSocketReactor)
    {
        delete m_pSocketReactor;
        m_pSocketReactor = NULL;
    }
}

unsigned int __stdcall CTorrentTask::ThreadFunc( void *pParam )
{
    CTorrentTask *pTask = (CTorrentTask *)pParam;
    pTask->Svc();
    CloseHandle(pTask->m_hTaskThread);
    return 0;
}

void CTorrentTask::Svc()
{
    if (!m_bExit)
    {
        m_pSocketReactor->Update();
        m_pRateMeasure->Update();
    }
}

IWinSocketReactor * CTorrentTask::GetSocketReactor()
{
    return m_pSocketReactor;
}

IUPnpNAT * CTorrentTask::GetUPnpNAT()
{
    return m_pUPnpNAT;
}

ITorrentFile * CTorrentTask::GetTorrentFile()
{
    return m_pTorrentFile;
}

IPeerAcceptor * CTorrentTask::GetAcceptor()
{
    return m_pPeerAcceptor;
}

long long CTorrentTask::GetDownloadCount()
{
    return m_llDownloadCount;
}

long long CTorrentTask::GetUploadCount()
{
    return m_llUploadCount;
}

ITaskStorage * CTorrentTask::GetTaskStorage()
{
    return m_pTaskStorage;
}
