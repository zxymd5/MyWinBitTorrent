#include "TaskStorage.h"

CTaskStorage::CTaskStorage(void) : m_pTorrentTask(NULL)
{
}

CTaskStorage::~CTaskStorage(void)
{
}

bool CTaskStorage::Startup()
{
    return true;
}

void CTaskStorage::Shutdown()
{

}

long long CTaskStorage::GetLeftCount()
{
    return 0;
}

void CTaskStorage::SetTorrentTask( ITorrentTask *pTorrentTask )
{
    m_pTorrentTask = pTorrentTask;
}

ITorrentTask * CTaskStorage::GetTorrentTask()
{
    return m_pTorrentTask;
}
