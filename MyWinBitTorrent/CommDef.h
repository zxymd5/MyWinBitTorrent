#ifndef COMM_DEF_H
#define COMM_DEF_H

#pragma warning(disable:4996)

#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;

enum SocketMask
{
    READ_MASK = 0x01,
    WRITE_MASK = 0x02
};

enum TrackerState
{
    TS_INIT,
    TS_CONNECTING,
    TS_REQUESTING,
    TS_ERROR,
    TS_OK
};

enum TrackerEvent
{
    TE_START,
    TE_STOP,
    TE_COMPLETE,
    TE_NONE
};

typedef struct FileInfo
{
    string strFilePath;
    __int64 llFileSize;
} FileInfo;

static const int MAX_PRIORITY_LEVEL = 5;

static void HandleErrMsg(char *pErrMsg,char *pFileName,int nErrCode,int nLineNumber)
{
    if(pErrMsg==NULL)
    {
        return;
    }

    char szErrBuff[1024];
    sprintf(szErrBuff, "%s File:%s,Error Code:%d,Line:%d.\n",pErrMsg,pFileName,nErrCode,nLineNumber);
    ::MessageBox(NULL,szErrBuff, "¥ÌŒÛ–≈œ¢", MB_OK|MB_ICONWARNING); 
}

static __int64 StringToInt64(const char *pStr, int nLen)
{
    __int64 nResult = 0;
    for (int i = 0; i < nLen; i++)
    {
        nResult = nResult * 10 + static_cast<__int64>(*(pStr + i) - '0');
    }

    return nResult;
}

static string URLEncode(const unsigned char *str, size_t len)
{
    static const char szUnreserved[] = "-_.!~*()ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    stringstream ret;
    ret << hex << setfill('0');

    for (size_t i = 0; i < len; ++i)
    {
        if (count(szUnreserved, szUnreserved + sizeof(szUnreserved) - 1, *str) > 0)
        {
            ret << *str;
        }
        else
        {
            ret << '%' << setw(2) << (int)(*str);
        }
        ++str;
    }

    return ret.str();
}

#endif