#pragma once

//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_com{
	typedef std::string                     memory_buffer;
	typedef std::shared_ptr<memory_buffer>  memory_buffer_ptr;

	class hub_com_response_impl: public signalrcpp::hub_response
	{
	public:
		hub_com_response_impl()
		{
			event_id=0;
			set_pointer();
		}

		std::string   classs_name_;
		std::string   object_name_;
		memory_buffer data_;
		memory_buffer raw_;

		static void set_data(memory_buffer& b, void* data, int data_len);
		static void set_data(memory_buffer& b, std::vector<char>& d);
		static void set_data(memory_buffer& b, std::string&       d);

		void set_pointer(signalrcpp::string_ptr& to, memory_buffer& from);
		void set_pointer()
		{
			set_pointer(classs_name,classs_name_);
			set_pointer(object_name,object_name_);
			set_pointer(data,data_);
			set_pointer(raw,raw_);
		}
	};

}