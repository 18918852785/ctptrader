#pragma once

//apr_handle
class APR_HANDLE {
public:
	int type;
	//0: file  handle 
	//1: mutex handle
	void *pointer;

	APR_HANDLE(apr_file_t *f);
	APR_HANDLE(apr_global_mutex_t *m);

	BOOL CloseHandle_();

	static apr_file_t *getFile(HANDLE h);
	static apr_global_mutex_t *getMutex(HANDLE h);

protected:
	BOOL CloseFile();
	BOOL CloseMutex();
};
//apr_handle end
