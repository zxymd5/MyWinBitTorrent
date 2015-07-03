#ifndef TASK_STORAGE_H
#define TASK_STORAGE_H

#include "mywinbittorrent.h"

class CTaskStorage :
    public ITaskStorage
{
public:
    CTaskStorage(void);
    virtual ~CTaskStorage(void);
    virtual bool Startup();
    virtual void Shutdown();
    virtual long long GetLeftCount();
};

#endif