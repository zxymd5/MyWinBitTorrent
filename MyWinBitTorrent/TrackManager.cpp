#include "TrackManager.h"
#include "TorrentParser.h"
#include "TCPTracker.h"
#include "UDPTracker.h"

CTrackerManager::CTrackerManager(void) : m_pTorrentTask(NULL),
                                         m_llPeerCount(0),
                                         m_llSeedCount(0)
{
}

CTrackerManager::~CTrackerManager(void)
{
}

bool CTrackerManager::Startup()
{
    m_llPeerCount = 0;
    m_llSeedCount = 0;

    CreateTrackers();
    m_bExit = false;
    m_hTrackerThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void *)this, 0, NULL);
    return true;
}

void CTrackerManager::Shutdown()
{
    m_bExit = true;
    ShutdownTrackers();
    
    if (m_hTrackerThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hTrackerThread);
        m_hTrackerThread = INVALID_HANDLE_VALUE;
    }

    DestroyTrackers();
}

long long CTrackerManager::GetSeedCount()
{
    return 0;
}

long long CTrackerManager::GetPeerCount()
{
    return 0;
}

void CTrackerManager::SetTorrentTask( ITorrentTask *pTask )
{
    m_pTorrentTask = pTask;
}

ITorrentTask * CTrackerManager::GetTorrentTask()
{
    return m_pTorrentTask;
}

void CTrackerManager::Svc()
{
    while(!m_bExit)
    {
        vector<ITracker *>::iterator it = m_vecTrackers.begin();
        for (; it != m_vecTrackers.end(); ++it)
        {
            if (m_bExit)
            {
                break;
            }

            if ((*it)->GetPeerCount() > m_llPeerCount)
            {
                m_llPeerCount = (*it)->GetPeerCount();
            }

            if ((*it)->GetSeedCount() > m_llSeedCount)
            {
                m_llSeedCount = (*it)->GetSeedCount();
            }

            if (GetTickCount() < (*it)->GetNextUpdateTick())
            {
                continue;
            }

            (*it)->Update();
        }

        Sleep(100);
    }
}

unsigned int __stdcall CTrackerManager::ThreadFunc( void *pParam )
{
    CTrackerManager *Manager = (CTrackerManager *)pParam;
    Manager->Svc();

    return 0;
}

void CTrackerManager::CreateTrackers()
{
    vector<string> vecAnnouce = m_pTorrentTask->GetTorrentFile()->GetAnnounceList();

    vector<string>::iterator it = vecAnnouce.begin();
    for (; it != vecAnnouce.end(); ++it)
    {
        ITracker *pTracker = NULL;
        int nStart = 0;
        int nEnd = 0;
        if (CTorrentParser::FindPattern((*it).c_str(), "HTTP:", nStart, nEnd) == true ||
            CTorrentParser::FindPattern((*it).c_str(), "http:", nStart, nEnd) == true)
        {
            pTracker = new CTCPTracker;
        }

        if (CTorrentParser::FindPattern((*it).c_str(), "UDP:", nStart, nEnd) == true ||
            CTorrentParser::FindPattern((*it).c_str(), "udp:", nStart, nEnd) == true)
        {
            pTracker = new CUDPTracker;
        }

        if (pTracker != NULL)
        {
            pTracker->SetURL((*it).c_str());
            pTracker->SetTrackerManager(this);
            m_vecTrackers.push_back(pTracker);
        }
    }
}

void CTrackerManager::DestroyTrackers()
{
    vector<ITracker *>::iterator it = m_vecTrackers.begin();
    for (; it != m_vecTrackers.end(); ++it)
    {
        delete (*it);
    }

    m_vecTrackers.clear();
}

void CTrackerManager::ShutdownTrackers()
{
    vector<ITracker *>::iterator it = m_vecTrackers.begin();
    for (; it != m_vecTrackers.end(); ++it)
    {
        (*it)->Shutdown();
    }
}
