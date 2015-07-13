#ifndef SELECT_REACTOR_H
#define SELECT_REACTOR_H

#include "mywinbittorrent.h"
#include <list>
using namespace std;

class CSelectReactor : public IWinSocketReactor
{
public:
    CSelectReactor(void);

    void ClearFdSet();

    virtual ~CSelectReactor(void);
    virtual bool AddSocket(IWinSocket *pSocket);
    virtual void RemoveSocket(IWinSocket *pSocket);
    void AddToFdSet();
    virtual bool Startup();
    virtual void Update();
    virtual void Shutdown();
    int SelectSocket();
    int AddTimer(ITimerCallback *pCallback, int nInterval, bool bOneShot);
    void RemoveTimer(int nTimerID);
    void UpdateTimerList();

private:
    vector<IWinSocket *> m_vecSockets;
    FD_SET  m_rSet;
    FD_SET  m_wSet;
    int m_nMaxSocketFd;
    int m_nFreeTimerID;
    list<int> m_lstFreeTimerID;
    list<TimerInfo> m_lstTimerInfo;
    list<TimerInfo> m_lstAddedTimerInfo;
};

#endif