#include "Flags.h"
//#include "DPU_DBManager.h"
//#include "logging.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef _WIN32
#define strtoll _strtoi64
#endif

namespace dpu {

#define LOG_ASSERT assert
DEFINE_FLAG(bool, print_flags, false, "Print flags as they are being parsed.");
DEFINE_FLAG(bool, ignore_unrecognized_flags, true, "Ignore unrecognized flags.");

bool Flags::initialized_ = false;

// List of registered flags.
Flag** Flags::flags_ = 0;
my_intptr_t Flags::capacity_ = 0;
my_intptr_t Flags::num_flags_ = 0;

class Flag {
public:
	enum FlagType {
		kBoolean,
		kInteger,
		kUint64,
		kString,
		kFunc,
		kNumFlagTypes
	};

	Flag(const char* name, const char* comment, void* addr, FlagType type)
		: name_(name), comment_(comment), addr_(addr), type_(type) {
	}
	Flag(const char* name, const char* comment, FlagHandler handler)
		: name_(name), comment_(comment), handler_(handler), type_(kFunc) {
	}

	void Print() {
		if (IsUnrecognized()) {
			printf("%s: unrecognized\n", name_);
			return;
		}
		switch (type_) {
		case kBoolean: {
			printf("%s: %s (%s)\n",
				name_, *this->bool_ptr_ ? "true" : "false", comment_);
			break;
						}
		case kInteger: {
			printf("%s: %d (%s)\n", name_, *this->int_ptr_, comment_);
			break;
						}
		case kUint64: {
			printf("%s: %lld (%s)\n", name_, *this->uint64_ptr_, comment_);
			break;
						}
		case kString: {
			if (*this->charp_ptr_ != NULL) {
				printf("%s: '%s' (%s)\n", name_, *this->charp_ptr_, comment_);
			} else {
				printf("%s: (null) (%s)\n", name_, comment_);
			}
			break;
						}
		case kFunc: {
			printf("%s: (%s)\n", name_, comment_);
			break;
					}
		default:
			assert(false&&"UNREACHABLE");
			break;
		}
	}

	bool IsUnrecognized() const {
		return (type_ == kBoolean) && (bool_ptr_ == NULL);
	}

	const char* name_;
	const char* comment_;
	union {
		void* addr_;
		bool* bool_ptr_;
		int* int_ptr_;
		my_uint64_t* uint64_ptr_;
		charp* charp_ptr_;
		FlagHandler handler_;
	};
	FlagType type_;
	bool changed_;
};


Flag* Flags::Lookup(const char* name) {
	for (my_intptr_t i = 0; i < num_flags_; i++) {
		Flag* flag = flags_[i];
		if (strcmp(flag->name_, name) == 0) {
			return flag;
		}
	}
	return NULL;
}


bool Flags::IsSet(const char* name) {
	Flag* flag = Lookup(name);
	return (flag != NULL) &&
		(flag->type_ == Flag::kBoolean) &&
		(flag->bool_ptr_ != NULL) &&
		(*flag->bool_ptr_ == true);
}


void Flags::AddFlag(Flag* flag) {
	if (num_flags_ == capacity_) {
		if (flags_ == NULL) {
			capacity_ = 256;
			flags_ = new Flag*[capacity_];
		} else {
			my_intptr_t new_capacity = capacity_ * 2;
			Flag** new_flags = new Flag*[new_capacity];
			for (my_intptr_t i = 0; i < num_flags_; i++) {
				new_flags[i] = flags_[i];
			}
			delete [] flags_;
			flags_ = new_flags;
			capacity_ = new_capacity;
		}
	}
	flags_[num_flags_++] = flag;
}


bool Flags::Register_bool(bool* addr,
	const char* name,
	bool default_value,
	const char* comment) {
		Flag* flag = Lookup(name);
		if (flag != NULL) {
			LOG_ASSERT(flag->IsUnrecognized());
			return default_value;
		}
		flag = new Flag(name, comment, addr, Flag::kBoolean);
		AddFlag(flag);
		return default_value;
}


int Flags::Register_int(int* addr,
	const char* name,
	int default_value,
	const char* comment) {
		LOG_ASSERT(Lookup(name) == NULL);

		Flag* flag = new Flag(name, comment, addr, Flag::kInteger);
		AddFlag(flag);

		return default_value;
}


my_uint64_t Flags::Register_uint64_t(my_uint64_t* addr,
	const char* name,
	my_uint64_t default_value,
	const char* comment) {
		LOG_ASSERT(Lookup(name) == NULL);

		Flag* flag = new Flag(name, comment, addr, Flag::kUint64);
		AddFlag(flag);

		return default_value;
}


const char* Flags::Register_charp(charp* addr,
	const char* name,
	const char* default_value,
	const char* comment) {
		LOG_ASSERT(Lookup(name) == NULL);
		Flag* flag = new Flag(name, comment, addr, Flag::kString);
		AddFlag(flag);
		return default_value;
}


bool Flags::Register_func(FlagHandler handler,
	const char* name,
	const char* comment) {
		LOG_ASSERT(Lookup(name) == NULL);
		Flag* flag = new Flag(name, comment, handler);
		AddFlag(flag);
		return false;
}


static void Normalize(char* s) {
	my_intptr_t len = strlen(s);
	for (my_intptr_t i = 0; i < len; i++) {
		if (s[i] == '-') {
			s[i] = '_';
		}
	}
}


bool Flags::SetFlagFromString(Flag* flag, const char* argument) {
	LOG_ASSERT(!flag->IsUnrecognized());
	switch (flag->type_) {
	case Flag::kBoolean: {
        *flag->bool_ptr_ = false;
        if (strcmp(argument, "true") == 0) {
			*flag->bool_ptr_ = true;
        } else if (strcmp(argument, "false") == 0) {
            *flag->bool_ptr_ = false;
        }else if (strcmp(argument, "T") == 0) {
            *flag->bool_ptr_ = true;
        } else if (strcmp(argument, "F") == 0) {
            *flag->bool_ptr_ = false;
        }else if (strcmp(argument, "1") == 0) {
            *flag->bool_ptr_ = true;
        } else if (strcmp(argument, "0") == 0) {
            *flag->bool_ptr_ = false;
		} else {
			return false;
		}
		break;
	}
	case Flag::kString: {
		*flag->charp_ptr_ = argument == NULL ? NULL : strdup(argument);
		break;
	}
	case Flag::kInteger: {
		char* endptr = NULL;
		const my_intptr_t len = strlen(argument);
		int base = 10;
		if ((len > 2) && (argument[0] == '0') && (argument[1] == 'x')) {
			base = 16;
		}
		int val = strtol(argument, &endptr, base);
		if (endptr == argument + len) {
			*flag->int_ptr_ = val;
		} else {
			return false;
		}
		break;
	}
	case Flag::kUint64: {
		char* endptr = NULL;
		const my_intptr_t len = strlen(argument);
		int base = 10;
		if ((len > 2) && (argument[0] == '0') && (argument[1] == 'x')) {
			base = 16;
		}
		my_int64_t val = strtoll (argument, &endptr, base);
		if (endptr == argument + len) {
			*flag->uint64_ptr_ = static_cast<my_uint64_t>(val);
		} else {
			return false;
		}
		break;
	}
	case Flag::kFunc: {
		if (strcmp(argument, "true") == 0) {
			(flag->handler_)(true);
		} else if (strcmp(argument, "false") == 0) {
			(flag->handler_)(false);
		} else {
			return false;
		}
		break;
	}
	default: {
		assert(false&&"UNREACHABLE");
		return false;
	}
	}
	flag->changed_ = true;
	return true;
}


void Flags::Parse(const char* option) {
	// Find the beginning of the option argument, if it exists.
	const char* equals = option;
	while ((*equals != '\0') && (*equals != '=')) {
		equals++;
	}

	const char* argument = NULL;

	// Determine if this is an option argument.
	if (*equals != '=') {
		// No explicit option argument. Determine if there is a "no_" prefix
		// preceding the name.
		const char* kNo1Prefix = "no_";
		const char* kNo2Prefix = "no-";
		const my_intptr_t kNo1PrefixLen = strlen(kNo1Prefix);
		const my_intptr_t kNo2PrefixLen = strlen(kNo2Prefix);
		if (strncmp(option, kNo1Prefix, kNo1PrefixLen) == 0) {
			option += kNo1PrefixLen;  // Skip the "no_" when looking up the name.
			argument = "false";
		} else if (strncmp(option, kNo2Prefix, kNo2PrefixLen) == 0) {
			option += kNo2PrefixLen;  // Skip the "no-" when looking up the name.
			argument = "false";
		} else {
			argument = "true";
		}
	} else {
		// The argument for the option starts right after the equals sign.
		argument = equals + 1;
	}

	// Initialize the flag name.
	my_intptr_t name_len = equals - option;
	char* name = new char[name_len + 1];
	strncpy(name, option, name_len);
	name[name_len] = '\0';
	Normalize(name);

	Flag* flag = Flags::Lookup(name);
	if (flag == NULL) {
		// Collect unrecognized flags.
		char* new_flag = new char[name_len + 1];
		strncpy(new_flag, option, name_len);
		new_flag[name_len] = '\0';
		Flags::Register_bool(NULL, new_flag, true, NULL);
	} else {
		// Only set values for recognized flags, skip collected
		// unrecognized flags.
		if (!flag->IsUnrecognized()) {
			if (!SetFlagFromString(flag, argument)) {
				printf("Ignoring flag: %s %s is an invalid value for flag", 
					argument, name);
			}
		}
	}

	delete[] name;
}


static bool IsValidFlag(const char* name,
	const char* prefix,
	my_intptr_t prefix_length) {
		my_intptr_t name_length = strlen(name);
		return ((name_length > prefix_length) &&
			(strncmp(name, prefix, prefix_length) == 0));
}


int Flags::CompareFlagNames(const void* left, const void* right) {
	const Flag* left_flag = *reinterpret_cast<const Flag* const *>(left);
	const Flag* right_flag = *reinterpret_cast<const Flag* const *>(right);
	return strcmp(left_flag->name_, right_flag->name_);
}


bool Flags::ProcessCommandLineFlags(int number_of_vm_flags,
	const char** vm_flags) {
		if (initialized_) {
			return false;
		}

		qsort(flags_, num_flags_, sizeof flags_[0], CompareFlagNames);

		const char* kPrefix = "--";
		const my_intptr_t kPrefixLen = strlen(kPrefix);

		int i = 0;
		while (i < number_of_vm_flags) {
			if (IsValidFlag(vm_flags[i], kPrefix, kPrefixLen)) {
				const char* option = vm_flags[i] + kPrefixLen;
				Parse(option);
			}
			i++;
		}

		if (!FLAG_ignore_unrecognized_flags) {
			int unrecognized_count = 0;
			for (my_intptr_t j = 0; j < num_flags_; j++) {
				Flag* flag = flags_[j];
				if (flag->IsUnrecognized()) {
					if (unrecognized_count == 0) {
						fprintf(stderr, "Unrecognized flags: %s", flag->name_);
					} else {
						fprintf(stderr, ", %s", flag->name_);
					}
					unrecognized_count++;
				}
			}
			if (unrecognized_count > 0) {
				fprintf(stderr, "\n");
				exit(255);
			}
		}
		if (FLAG_print_flags) {
			PrintFlags();
		}

		initialized_ = true;
		return true;
}

bool Flags::SetFlag(const char* name,
	const char* value,
	const char** error) {
		Flag* flag = Lookup(name);
		if (flag == NULL) {
			*error = "Cannot set flag: flag not found";
			return false;
		}
		if (!SetFlagFromString(flag, value)) {
			*error = "Cannot set flag: invalid value";
			return false;
		}
		return true;
}

void Flags::PrintFlags() {
	printf("Flag settings:\n");
	for (my_intptr_t i = 0; i < num_flags_; ++i) {
		flags_[i]->Print();
	}
}
}  // namespace dpu

#ifdef _WIN32
#include <Windows.h>
//#include "StringUtilsWin.h"

using namespace dpu;

char* FlagsCommandLine::StringUtilsWin_WideToUtf8(const wchar_t* wide, int len, int* result_len) {
    // If len is -1 then WideCharToMultiByte will include the terminating
    // NUL byte in the length.
    int utf8_len = WideCharToMultiByte(
        CP_UTF8, 0, wide, (int)len, NULL, (int)0, NULL, NULL);
    char* utf8 = reinterpret_cast<char*>(malloc(utf8_len+4));
    memset(utf8,0, utf8_len+4);
    WideCharToMultiByte(CP_UTF8, 0, wide, (int)len, utf8, utf8_len, NULL, NULL);
    if (result_len != NULL) {
        *result_len = utf8_len;
    }
    return utf8;
}


    FlagsCommandLine::FlagsCommandLine() {
        argc = 0;
        LPCWSTR cmd = GetCommandLineW();
        wchar_t** argvw = (wchar_t**)CommandLineToArgvW(cmd, &argc);
        argv2 = new char*[argc];
        for (int i = 0; i < argc; ++i) {
            argv2[i] = StringUtilsWin_WideToUtf8(argvw[i],wcslen(argvw[i]),0);
        }
        LocalFree(argvw);
    }
    FlagsCommandLine::~FlagsCommandLine(){
        for (int i = 0; i < argc; ++i)
            free(argv2[i]);
        delete argv2;
    }


class FlagsInitializer {
public:
    FlagsInitializer() {
        FlagsCommandLine cmd;
        Flags::ProcessCommandLineFlags(cmd.argc - 1, (const char**)cmd.argv2 + 1);
    }
};


void Flags::Init(){
    static FlagsInitializer flags_initializer;
}

#else//mac
dpu::FlagsCommandLine::FlagsCommandLine(){
    argc=1;
    argv2 = new char*[argc];
    argv2[0] = (char*)malloc(10);
    argv2[0][0]=0;
}
dpu::FlagsCommandLine::~FlagsCommandLine(){
    free(argv2[0]);
    delete argv2;
}
#endif // mac
