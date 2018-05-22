// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
//// Windows 头文件:
//#include <windows.h>

#if _MSC_VER >= 1700 //vs2012
#include <thread>
#else//vs2010
#include "../../include/thread/task_thread.h"
#endif

#include <iostream>
#include <assert.h>

#include "TraderSpi.h"

#include <iostream>

// TODO: 在此处引用程序需要的其他头文件
