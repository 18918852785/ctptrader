// Copyright (c) 2016 Wind Info. All rights reserved.
#pragma once
//#include <DPU_DBManager.h>

typedef long long          my_intptr_t;
typedef unsigned long long my_uint64_t;
typedef long long my_int64_t;

typedef const char* charp;

#define DECLARE_FLAG(type, name)                                               \
  extern type FLAG_##name

#define DEFINE_FLAG(type, name, default_value, comment)                        \
  type FLAG_##name = dpu::Flags::Register_##type(&FLAG_##name,                      \
                                            #name,                             \
                                            default_value,                     \
                                            comment)

#define DEFINE_FLAG_HANDLER(handler, name, comment)                            \
  bool DUMMY_##name = Flags::Register_func(handler, #name, comment)


#if defined(DEBUG)
#define DECLARE_DEBUG_FLAG(type, name) DECLARE_FLAG(type, name)
#define DEFINE_DEBUG_FLAG(type, name, default_value, comment)                  \
  DEFINE_FLAG(type, name, default_value, comment)
#else
#define DECLARE_DEBUG_FLAG(type, name)
#define DEFINE_DEBUG_FLAG(type, name, default_value, comment)
#endif

namespace dpu {

typedef void (*FlagHandler)(bool value);

// Forward declarations.
class Flag;

class Flags {
public:
	static bool Register_bool(bool* addr,
		const char* name,
		bool default_value,
		const char* comment);

	static int Register_int(int* addr,
		const char* name,
		int default_value,
		const char* comment);

	static my_uint64_t Register_uint64_t(my_uint64_t* addr,
		const char* name,
		my_uint64_t default_value,
		const char* comment);

	static const char* Register_charp(charp* addr,
		const char* name,
		const char* default_value,
		const char* comment);

	static bool Register_func(FlagHandler handler,
		const char* name,
		const char* comment);

	static bool ProcessCommandLineFlags(int argc, const char** argv);
#ifdef _WIN32
    static void Init();
#endif

	static Flag* Lookup(const char* name);

	static bool IsSet(const char* name);

	static bool Initialized() { return initialized_; }

	static bool SetFlag(const char* name,
		const char* value,
		const char** error);

	static void PrintFlags();
private:
	static Flag** flags_;
	static my_intptr_t capacity_;
	static my_intptr_t num_flags_;

	static bool initialized_;

	static void AddFlag(Flag* flag);

	static bool SetFlagFromString(Flag* flag, const char* argument);

	static void Parse(const char* option);

	static int CompareFlagNames(const void* left, const void* right);

	//DISALLOW_ALLOCATION();
	//DISALLOW_IMPLICIT_CONSTRUCTORS(Flags);
};

class FlagsCommandLine{
public:
    static char* StringUtilsWin_WideToUtf8(const wchar_t* wide, int len, int* result_len);

    FlagsCommandLine();
    ~FlagsCommandLine();
    char** argv2;
    int argc;
};

}  // namespace dpu

