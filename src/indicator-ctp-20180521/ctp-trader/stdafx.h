// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
//// Windows ͷ�ļ�:
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
