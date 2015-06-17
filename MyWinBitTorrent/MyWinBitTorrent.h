#ifndef MY_WIN_BIT_TORRENT_H
#define MY_WIN_BIT_TORRENT_H

#include "CommDef.h"

class IWinSocketReactor;

class IWinSocket
{
public:
    virtual ~IWinSocket(){};
    virtual void SetReactor(IWinSocketReactor *pReactor) = 0;
    virtual IWinSocketReactor *GetReactor() = 0;
    virtual void CreateTCPSocket() = 0;
    virtual void CreateUDPSocket() = 0;
    virtual int GetHandle() = 0;
    virtual int GetHandleMask() = 0;
    virtual void SetNonBlock() = 0;
    virtual void Close() = 0;
    virtual int HandleRead() = 0;
    virtual int HandleWrite() = 0;
    virtual void HandleClose() = 0;
};

class IWinSocketReactor
{
public:
    virtual ~IWinSocketReactor() {};
    virtual bool AddSocket(IWinSocket *pSocket) = 0;
    virtual void RemoveSocket(IWinSocket *pSocket) = 0;
    virtual void UpdateMask(IWinSocket *pSocket) = 0;
    virtual bool Startup() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
};

#endif