#ifndef COMM_DEF_H
#define COMM_DEF_H

#pragma warning(disable:4996)

#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
using namespace std;

enum SocketMask
{
    READ_MASK = 0x01,
    WRITE_MASK = 0x02
};

typedef struct FileInfo
{
    string strFilePath;
    __int64 llFileSize;
} FileInfo;

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

#endif