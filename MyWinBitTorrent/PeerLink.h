#ifndef PEER_LINK_H
#define PEER_LINK_H

#include "mywinbittorrent.h"
#include "WinSocket.h"
#include "PieceRequest.h"
#include "BitSet.h"

class CPeerLink :
    public IPeerLink,
    public ITimerCallback,
    public CWinSocket,
    public IRateMeasure
{
public:
    CPeerLink(void);
    virtual ~CPeerLink(void);
    virtual void SetPeerManager(IPeerManager *pManager);
    virtual int GetPeerState();
    virtual void Connect(const char *IpAddr, int nPort);
    virtual void CloseLink();
    virtual bool IsAccepted();
    virtual bool ShouldClose();
    virtual void ComputeSpeed();
    
    virtual void OnTimer(int nTimerID);

    virtual void AddClient(IRateMeasureClient *pClient);
    virtual void RemoveClient(IRateMeasureClient *pClient);
    virtual void Update();
    virtual void SetUploadSpeed(long long llSpeed);
    virtual void SetDownloadSpeed(long long llSpeed);
    virtual long long GetUploadSpeed();
    virtual long long GetDownloadSpeed();

private:
    int m_nPeerState;
    IPeerManager *m_pPeerManager;
    string  m_strIPAddr;
    int m_nPort;
    string m_strPeerLinkID;
    bool m_bAccepted;
    int m_nConnTimeoutID;

    CPieceRequest   m_clPieceRequest;
    string  m_strSendBuffer;
    string  m_strRecvBuffer;

    CBitSet m_clBitSet;
    bool m_bBitSetRecved;
    bool m_bHandShaked;
    bool m_bAmChoking;
    bool m_bAmInterested;
    bool m_bPeerChoking;
    bool m_bPeerInterested;

    unsigned int m_nDownloadCount;
    unsigned int m_nUploadCount;
    long long m_llLastCountSpeedTime;
    unsigned int m_nLastDownloadCount;
    unsigned int m_nLastUploadCount;
    unsigned int m_nUploadSpeed;
    unsigned int m_nDownloadSpeed;

    list<PeerPieceRequest> m_lstPeerPieceRequest;

    bool m_bCanRead;
    bool m_bCanWrite;
    int m_nWritePriority;
    int m_nReadPriority;

private:
    void SendData(const void *pData, int nLen);
    int ProcRecvData();
    void SendHandShake();
    void SendBitField();
    void SendChoke(bool bChoke);
    void SendInterested(bool bInterested);
    void SendHave(int nPieceIndex);
    void SendPieceRequest(int nPieceIndex, int nOffset, int nLen);
    void SendPieceData(int nPieceIndex, int nOffset, string &strData);
    void SendPieceCancel(int nPieceIndex, int nOffset, int nLen);
    void CheckHandshake(string strInfo);

    void ProcCmd(int nCmd, void *pData, int nDataLen);
    int ProcCmdChoke(void *pData, int nDataLen);
    int ProcCmdUnchoke(void *pData, int nDataLen);
    int ProcCmdInterested(void *pData, int nDataLen);

};

#endif