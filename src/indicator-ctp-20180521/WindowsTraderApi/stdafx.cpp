// stdafx.cpp : 只包括标准包含文件的源文件
// testTraderApi.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <windows.h>

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

#include "../../signalrcpp_hub/hub_factory_impl.h"


signalrcpp::hub_factory* get_task_thread_factory()
{
    static signalrcpp::hub_factory* g_factory;
    if (g_factory) return g_factory;

    g_factory = new signalrcpp_hub::task_factory_impl();
    return g_factory;
}


void clear_screen(){
    HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO cbi;
    COORD origin = {0,0};
    int buf_length;

    GetConsoleScreenBufferInfo(std_out,&cbi);
    buf_length = cbi.dwSize.X*cbi.dwSize.Y;
    FillConsoleOutputCharacter(std_out,0x20,buf_length,origin,0);
    FillConsoleOutputAttribute(std_out,0x07,buf_length,origin,0);
}

bool is_running(){//程序是否已经在运行
    auto hMutex=CreateMutexA(
        NULL,//nosecurityattributes 
        FALSE,//initiallynotowned 
        "Global\\WindowsTraderApi");//命名Mutex是全局对象 
    if(hMutex==NULL||ERROR_ALREADY_EXISTS==::GetLastError())
    { 
        //程序第二次或以后运行时，会得到Mutex已经创建的错误 
        return true; 
    }
    return false;
} 