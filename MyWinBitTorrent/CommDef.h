#ifndef COMM_DEF_H
#define COMM_DEF_H

#pragma warning(disable:4996)

#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

enum SocketMask
{
    READ_MASK = 0x01,
    WRITE_MASK = 0x02
};

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

#endif