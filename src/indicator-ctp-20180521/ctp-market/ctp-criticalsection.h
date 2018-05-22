#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

class ctp_criticalsection{
    friend class ctp_locker;
public:
    ctp_criticalsection(){::InitializeCriticalSection(&cs);}
    ~ctp_criticalsection(){::DeleteCriticalSection(&cs);}

private:
    void enter(){::EnterCriticalSection(&cs);}
    void leave(){::LeaveCriticalSection(&cs);}
    CRITICAL_SECTION cs;
};

class ctp_locker{
public:
    std::shared_ptr<ctp_criticalsection> l;
    ctp_locker(std::shared_ptr<ctp_criticalsection> p):l(p){
        assert(p);
        l->enter();
    }
    ~ctp_locker(){
        l->leave();
    }
};
