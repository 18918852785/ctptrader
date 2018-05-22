#pragma once
#include <memory>

namespace filemapping
{
#define BSNEWS_FILEMAPPING_BUFFER_SIZE		256 // 256 bytes
#define BSNEWS_FILEMAPPING_NAME             "Global\\bsnews-filemapping"
	class filemapping_server
	{
	public:
		filemapping_server();
		~filemapping_server();

		int   copy(const char* msg);

		void* hMapFile;
		void* pBuf;
		int   pBuf_len;
	};

	class filemapping_client
	{
	public:
		filemapping_client();
		~filemapping_client();

		int   copy();

		void* hMapFile;
		void* pBuf;
		int   pBuf_len;
	};

	typedef std::shared_ptr<filemapping_server> filemapping_server_ptr;
}