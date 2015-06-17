#ifndef SELECT_REACTOR_H
#define SELECT_REACTOR_H

#include "mywinbittorrent.h"
#include <vector>
using namespace std;

class CSelectReactor : public IWinSocketReactor
{
public:
    CSelectReactor(void);

    void ClearFdSet();

    virtual ~CSelectReactor(void);
    virtual bool AddSocket(IWinSocket *pSocket);
    virtual void RemoveSocket(IWinSocket *pSocket);
    virtual void UpdateMask(IWinSocket *pSocket);
    virtual bool Startup();
    virtual void Update();
    virtual void Shutdown();
    int SelectSocket();
private:
    vector<IWinSocket *> m_vecSockets;
    FD_SET  m_rSet;
    FD_SET  m_wSet;
    int m_nMaxSocketFd;
};

#endif