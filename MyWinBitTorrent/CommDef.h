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
#include <time.h>
#include <ShlObj.h>
#include "sha1.h"

using namespace std;

enum SocketMask
{
    NONE_MASK = 0x00,
    READ_MASK = 0x01,
    WRITE_MASK = 0x02
};

enum TrackerState
{
    TS_INIT,
    TS_CONNECTING,
    TS_ESTABLISHED,
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

enum PeerState
{
    PS_INIT,
    PS_CONNECTING,
    PS_CONNECTFAILED,
    PS_ESTABLISHED,
    PS_CLOSED
};

static const int MAX_PRIORITY_LEVEL = 5;
static const int REQUEST_BLOCK_SIZE = 16 * 1024;
static const int RECV_BUFFER_SIZE = 8 * 1024;

static void HandleErrMsg(char *pErrMsg,char *pFileName,int nErrCode,int nLineNumber)
{
    if(pErrMsg==NULL)
    {
        return;
    }

    char szErrBuff[1024];
    sprintf(szErrBuff, "%s File:%s,Error Code:%d,Line:%d.\n",pErrMsg,pFileName,nErrCode,nLineNumber);
    ::MessageBox(NULL,szErrBuff, "������Ϣ", MB_OK|MB_ICONWARNING); 
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

static bool InitNetwork()
{
    int nRet = 0;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    nRet = WSAStartup(wVersionRequested, &wsaData);

    if (nRet != 0)
    {
        printf("WSAStartup failed");
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        HandleErrMsg("Could not find a usable version of Winsock.dll", __FILE__, GetLastError(), __LINE__);
        WSACleanup();
        return false;
    }
    return true;
}

static int StopNetwork()
{
    int n;
    if ((n = WSACleanup()) != 0)
    {
        HandleErrMsg("WSACleanup failed", __FILE__, GetLastError(), __LINE__);
    }

    return n;
}

static unsigned int GetCurrentTick()
{
    time_t clock;
    struct tm curr_tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    curr_tm.tm_year = wtm.wYear;
    curr_tm.tm_mon = wtm.wMonth;
    curr_tm.tm_mday = wtm.wDay;
    curr_tm.tm_hour = wtm.wHour;
    curr_tm.tm_min = wtm.wMinute;
    curr_tm.tm_sec = wtm.wSecond;
    curr_tm.tm_isdst = -1;
    clock = mktime(&curr_tm);

    return (unsigned int)(clock * 1000 + wtm.wMilliseconds);
}

static bool CreateDir(string strPath)
{
    string::size_type pos = strPath.rfind('\\');
    if (pos == string::npos)
    {
        return false;
    }
    if (pos != strPath.size() - 1)
    {
        strPath.erase(pos + 1, strPath.size() - pos - 1);
    }

    SHCreateDirectoryEx(NULL, strPath.c_str(), NULL);
    return true;
}

static string ShaString(char *pData, int nLen)
{
    unsigned char szBuff[20];
    sha1_block((unsigned char *)pData, nLen, szBuff);

    string strResult;
    strResult.append((const char *)szBuff, sizeof(szBuff));
    return strResult;
}

static unsigned long long ntohll(unsigned long long llv)
{
    union
    {
        unsigned int lv[2];
        unsigned long long llv;
    } u;
    u.llv = llv;

    return ((unsigned long long)ntohl(u.lv[0]) << 32) | (unsigned long long)ntohl(u.lv[1]);
}


static unsigned long long htonll(unsigned long long v)
{
    union
    {
        unsigned int lv[2];
        unsigned long long llv;
    } u;

    u.lv[0] = htonl(v >> 32);
    u.lv[1] = htonl(v & 0xFFFFFFFFULL);

    return u.llv;
}

#endif