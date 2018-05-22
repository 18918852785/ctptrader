

#include "stdafx.h"


#include "../../include/hub_factory_impl.h"


signalrcpp::hub_factory* get_task_thread_factory()
{
	static signalrcpp::hub_factory* g_factory;
	if (g_factory) return g_factory;

	g_factory = new signalrcpp_hub::task_factory_impl();
	return g_factory;
}
