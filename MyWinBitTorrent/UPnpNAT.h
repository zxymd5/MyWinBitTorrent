#ifndef UPNPNAT_H
#define UPNPNAT_H

#include "MyWinBitTorrent.h"

class CUPnpNAT :
    public IUPnpNAT
{
public:
    CUPnpNAT(void);
    virtual ~CUPnpNAT(void);
    virtual void SetSocketReactor(IWinSocketReactor *pReactor);
    virtual bool Startup();
    virtual void Shutdown();
    virtual void AddPortMap(int nPort, const char *pProtocol);
    virtual void RemovePortMap(int nPort, const char *pProtocol);

private:
    IWinSocketReactor *m_pSocketReactor;
};

#endif
