#pragma once
#include <memory>
#include <functional>

#ifdef _WIN32
#ifdef INDICATORZMQ_EXPORTS
#include "../../../build/rabbitmq-c/target/indicator/include/string/string_map.h"
#else
#include "../string/string_map.h"
#endif
#else//linux
#include "../string/string_map.h"
#endif

namespace indicator
{
	struct zmq_handler
	{
		virtual int on_message (cosmos::string_item* msg) = 0;
	};

	struct zmq_hub
	{
		virtual int create() = 0;
		virtual int destroy() = 0;
		
		virtual void* get_object() = 0;

		virtual void* set_option(int option_id, void* data, int data_len) = 0;

		virtual int send_data(const char* data) = 0;
		virtual int send_data(void* data, int len) = 0;

		virtual int recv_data  (void** ppdata, int* plen) = 0;
		virtual int recv_string(char** ppdata) = 0;
		virtual int recv_string(char** ppdata, int* ptimeout_ret, int timeout_msec) = 0;
		
		virtual int free_data  (void*  pdata) = 0;

		virtual int get_high_water_mark() const = 0;
		virtual int set_high_water_mark(int v) = 0;

		virtual int get_env_string(const char* env_name, cosmos::string_item* pret) = 0;
		virtual int get_env_int(const char* env_name, int* pret) = 0;
		virtual int get_env_int(const char* env_name, int i_default) = 0;
	};

	struct zmq_sub 
	{
		virtual zmq_hub* get_hub() = 0;
		virtual int run_loop(const char* sub_filter, zmq_handler*) = 0;
		virtual int run_loop(const char** sub_filters, int sub_filters_count,zmq_handler*) = 0;
	};
	typedef std::shared_ptr<zmq_sub> zmq_sub_ptr;

	struct zmq_pub
	{
		virtual zmq_hub* get_hub() = 0;
		virtual int      run_loop(zmq_handler*) = 0;
	};
	typedef std::shared_ptr<zmq_pub> zmq_pub_ptr;

	struct zmq_request
	{
		virtual zmq_hub* get_hub() = 0;
		virtual int      run_loop(zmq_handler*) = 0;
	};
	typedef std::shared_ptr<zmq_request> zmq_request_ptr;

	struct zmq_reply
	{
		virtual zmq_hub* get_hub() = 0;
		virtual int      run_loop(zmq_handler*) = 0;
	};
	typedef std::shared_ptr<zmq_reply> zmq_reply_ptr;


	struct zmq_reply_thread
	{
		virtual int start(
			indicator::zmq_handler* collector_handler,
			indicator::zmq_handler* reply_handler) = 0;
		virtual int send_to_thread(const char* msg) = 0;
		virtual int destroy() = 0;
	};
	typedef std::shared_ptr<zmq_reply_thread> zmq_reply_thread_ptr;

	struct zyre_chat
	{
		virtual int run_loop(const char* myname) = 0;
	};
	typedef std::shared_ptr<zyre_chat> zyre_chat_ptr;

	struct zmq_dll
	{
		virtual int init() = 0;
		virtual int term() = 0;

		/////////////////////////////////////////////////////////////////////
		virtual zmq_pub_ptr create_pub(const char* endpoint) = 0;
		virtual zmq_sub_ptr create_sub(const char* endpoint) = 0;
		virtual zmq_sub_ptr create_sub(const char** endpoints, int endpoint_count) = 0;

		/////////////////////////////////////////////////////////////////////
		virtual zmq_request_ptr create_request(const char* endpoint) = 0;
		virtual zmq_reply_ptr   create_reply  (const char* endpoint) = 0;

		/////////////////////////////////////////////////////////////////////
		virtual zmq_reply_thread_ptr create_reply_thread(const char* endpoint,
			const char* thread_id) = 0;

		//////////////////////////////////////////////////////////////////////////
		virtual zyre_chat_ptr create_zyre_chat(const char* myname) = 0;
	};
	typedef std::shared_ptr<zmq_dll> zmq_dll_ptr;

}