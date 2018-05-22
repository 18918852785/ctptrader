// stdafx.cpp : 只包括标准包含文件的源文件
// ctp-market.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

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
