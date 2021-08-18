#ifndef ROLA_UTILS_STACK_TRACER_HPP
#define ROLA_UTILS_STACK_TRACER_HPP

#include <exception>
#include <fstream>
#include <chrono>
#include <typeinfo>
#include <type_traits>
#include <string>

#include "stlex/chrono_ex.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <Psapi.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")

#pragma pack( push, before_imagehlp, 8 )
#include <imagehlp.h>
#pragma pack( pop, before_imagehlp )

#else
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unwind.h>
#include <pthread.h>
#include <sstream>

#endif

namespace rola
{
#if defined(_WIN32)
	class stack_tracer
	{
		struct module_data
		{
			std::string image_name;
			std::string module_name;
			void* base_address;
			DWORD load_size;
		};

		class symbol_handler
		{
			HANDLE p;
		public:
			symbol_handler(HANDLE process, char const* path = NULL, bool intrude = false) :
				p(process)
			{
				if (!SymInitialize(p, path, intrude))
					throw(std::logic_error("Unable to initialize symbol handler"));
			}
			~symbol_handler() noexcept
			{
				SymCleanup(p);
			}
		};

		class get_mod_info
		{
			HANDLE process;
			static const int buffer_length = 4096;
		public:
			get_mod_info(HANDLE h) noexcept : process(h)
			{
			}

			module_data
				operator () (HMODULE module) noexcept
			{
				module_data ret;
				char temp[buffer_length];
				MODULEINFO mi;

				GetModuleInformation(process, module, &mi, sizeof(mi));
				ret.base_address = mi.lpBaseOfDll;
				ret.load_size = mi.SizeOfImage;

				GetModuleFileNameExA(process, module, temp, sizeof(temp));
				ret.image_name = temp;
				GetModuleBaseNameA(process, module, temp, sizeof(temp));
				ret.module_name = temp;
				std::vector<char> img(ret.image_name.begin(), ret.image_name.end());
				std::vector<char> mod(ret.module_name.begin(), ret.module_name.end());
				SymLoadModule64(process, 0, &img[0], &mod[0], (DWORD64)ret.base_address, ret.load_size);
				return ret;
			}
		};

		class symbol
		{
			typedef IMAGEHLP_SYMBOL64 sym_type;
			sym_type* sym;
			static const int max_name_len = 1024;

		public:
			symbol(HANDLE process, DWORD64 address) :
				sym((sym_type*)::operator new(sizeof(*sym) + max_name_len))
			{
				memset(sym, '\0', sizeof(*sym) + max_name_len);
				sym->SizeOfStruct = sizeof(*sym);
				sym->MaxNameLength = max_name_len;
				DWORD64 displacement;

				if (!SymGetSymFromAddr64(process, address, &displacement, sym))
					throw(std::logic_error("Bad symbol"));
			}

			std::string
				name() noexcept
			{
				return std::string(sym->Name);
			}

			std::string
				undecorated_name() noexcept
			{
				std::vector<char> und_name(max_name_len);
				UnDecorateSymbolName(sym->Name, &und_name[0], max_name_len, UNDNAME_COMPLETE);
				return std::string(&und_name[0], strlen(&und_name[0]));
			}

			static
				void
				options(DWORD add, DWORD remove = 0) noexcept
			{
				DWORD symOptions = SymGetOptions();
				symOptions |= add;
				symOptions &= ~remove;
				SymSetOptions(symOptions);
			}
		};

	public:
		static
			std::string
			stack_trace(const std::string& prefix = "", const std::string& surfix = "") noexcept
		{
			std::ostringstream os;

			HANDLE process = GetCurrentProcess();
			HANDLE hThread = GetCurrentThread();

			CONTEXT c;
			memset(&c, 0, sizeof(CONTEXT));

#ifdef _M_IX86
			c.ContextFlags = CONTEXT_CONTROL;

			__asm
			{
			Label:
				mov[c.Ebp], ebp;
				mov[c.Esp], esp;
				mov eax, [Label];
				mov[c.Eip], eax;
			}
#else
			c.ContextFlags = CONTEXT_FULL;
			RtlCaptureContext(&c);
#endif

			int frame_number = 0;
			DWORD offset_from_symbol = 0;
			IMAGEHLP_LINE64 line = { 0 };

			symbol_handler handler(process);
			symbol::options(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

			void* base = _load_modules_symbols(process, GetCurrentProcessId());

			STACKFRAME64 s = _init_stack_frame(c);

			line.SizeOfStruct = sizeof line;

			IMAGE_NT_HEADERS* h = ImageNtHeader(base);
			DWORD image_type = h->FileHeader.Machine;

			if (prefix != "") os << "\n" << "\t\t" << prefix << "\n";
			do
			{
				if (!StackWalk64(image_type, process, hThread, &s, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
					return std::string();

				os << std::setw(3) << "\n" << frame_number << "\t";
				if (s.AddrPC.Offset != 0)
				{
					os << symbol(process, s.AddrPC.Offset).undecorated_name();

					if (SymGetLineFromAddr64(process, s.AddrPC.Offset, &offset_from_symbol, &line))
						os << "\t" << line.FileName << "(" << line.LineNumber << ")";
				}
				else
					os << "(No Symbols: PC == 0)";

				++frame_number;
			} while (s.AddrReturn.Offset != 0);

			if (surfix != "") os << "\n\n" << "\t\t" << surfix << "\n";

			return os.str();
		}

		static
			std::string
			stack_trace2(const std::string& prefix = "", const std::string& surfix = "") noexcept
		{
			static std::mutex m;
			std::lock_guard<std::mutex> lock(m);

			HANDLE process = GetCurrentProcess();
			HANDLE thread = GetCurrentThread();

			CONTEXT context;
			memset(&context, 0, sizeof(CONTEXT));

#ifdef _M_IX86
			context.ContextFlags = CONTEXT_CONTROL;

			__asm
			{
			Label:
				mov[context.Ebp], ebp;
				mov[context.Esp], esp;
				mov eax, [Label];
				mov[context.Eip], eax;
			}
#else
			context.ContextFlags = CONTEXT_FULL;
			RtlCaptureContext(&context);
#endif

			SymInitialize(process, NULL, TRUE);

			DWORD image;
			STACKFRAME64 stackframe;
			ZeroMemory(&stackframe, sizeof(STACKFRAME64));

#ifdef _M_IX86
			image = IMAGE_FILE_MACHINE_I386;
			stackframe.AddrPC.Offset = context.Eip;
			stackframe.AddrPC.Mode = AddrModeFlat;
			stackframe.AddrFrame.Offset = context.Ebp;
			stackframe.AddrFrame.Mode = AddrModeFlat;
			stackframe.AddrStack.Offset = context.Esp;
			stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
			image = IMAGE_FILE_MACHINE_AMD64;
			stackframe.AddrPC.Offset = context.Rip;
			stackframe.AddrPC.Mode = AddrModeFlat;
			stackframe.AddrFrame.Offset = context.Rsp;
			stackframe.AddrFrame.Mode = AddrModeFlat;
			stackframe.AddrStack.Offset = context.Rsp;
			stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
			image = IMAGE_FILE_MACHINE_IA64;
			stackframe.AddrPC.Offset = context.StIIP;
			stackframe.AddrPC.Mode = AddrModeFlat;
			stackframe.AddrFrame.Offset = context.IntSp;
			stackframe.AddrFrame.Mode = AddrModeFlat;
			stackframe.AddrBStore.Offset = context.RsBSP;
			stackframe.AddrBStore.Mode = AddrModeFlat;
			stackframe.AddrStack.Offset = context.IntSp;
			stackframe.AddrStack.Mode = AddrModeFlat;
#endif

			std::ostringstream os;
			if (prefix != "") os << "\n" << "\t" << prefix << "\n\n";

			while (StackWalk64(image, process, thread, &stackframe, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			{
				char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
				PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbol->MaxNameLen = MAX_SYM_NAME;

				DWORD64 displacement = 0;

				SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol) ?
					os << symbol->Name << std::endl :
					os << "??????" << std::endl;
			}

			SymCleanup(process);
			if (surfix != "") os << "\n" << "\t" << surfix << "\n";
			return os.str();
		}

	private:
		static
			void*
			_load_modules_symbols(HANDLE process, DWORD pid) noexcept
		{
			std::vector<module_data> modules;
			DWORD cbNeeded;
			std::vector<HMODULE> module_handles(1);
			EnumProcessModules(process, &module_handles[0], DWORD(module_handles.size() * sizeof(HMODULE)), &cbNeeded);
			module_handles.resize(cbNeeded / sizeof(HMODULE));
			EnumProcessModules(process, &module_handles[0], DWORD(module_handles.size() * sizeof(HMODULE)), &cbNeeded);

			std::transform(module_handles.begin(), module_handles.end(), std::back_inserter(modules), get_mod_info(process));
			return modules[0].base_address;
		}

		static
			STACKFRAME64
			_init_stack_frame(CONTEXT c) noexcept
		{
#ifdef _M_X64
			STACKFRAME64 s;
			s.AddrPC.Offset = c.Rip;
			s.AddrPC.Mode = AddrModeFlat;
			s.AddrStack.Offset = c.Rsp;
			s.AddrStack.Mode = AddrModeFlat;
			s.AddrFrame.Offset = c.Rbp;
			s.AddrFrame.Mode = AddrModeFlat;
			return s;
#else
			STACKFRAME64 s;
			s.AddrPC.Offset = c.Eip;
			s.AddrPC.Mode = AddrModeFlat;
			s.AddrStack.Offset = c.Esp;
			s.AddrStack.Mode = AddrModeFlat;
			s.AddrFrame.Offset = c.Ebp;
			s.AddrFrame.Mode = AddrModeFlat;
			return s;
#endif
		}
	};

#else
#ifdef __cplusplus
	extern "C" {
#endif
		struct stacktrace_frame {
			void* addr;
			char* file;
			char* func;
			int line;
		};

		struct file_map {
			unsigned long long start;
			unsigned long long end;
			unsigned long long  offset;
			char* file;
		};


		struct stacktrace {
			char* exe;  /* Name of executable file */
			char* maps; /* Process memory map for this snapshot */

			size_t frames_size;
			size_t frames_len;
			struct stacktrace_frame* frames;

			size_t files_len;
			struct file_map* files;

			unsigned skip;
		};

		static char* read_whole_file(char* fname) {

			/* process files don't have size, so read chunks until EOF is reached */
			char* data = NULL;
			FILE* f;

			f = fopen(fname, "r");
			if (f != NULL) {
				int n, len = 0;
				int size = 1024;

                                data = (char*)malloc(size);
				for (;;) {
					int max = size - len;

					n = fread(data + len, 1, max, f);
					if (n > 0) {
						len += n;
					}

					if (n != max) {
						break;
					}
					size *= 2;
                                        data = (char*)realloc(data, size);
				}
				data[len] = '\0';
				fclose(f);
			}
			return data;
		}

		static _Unwind_Reason_Code collect(struct _Unwind_Context* ctx, void* p) {
                        struct stacktrace* trace = (struct stacktrace*)p;
			struct stacktrace_frame frame;

			if (trace->skip > 0) {
				trace->skip--;
				return _URC_NO_REASON;
			}

			frame.addr = (void*)_Unwind_GetIP(ctx);
			frame.file = NULL;
			frame.func = NULL;
			frame.line = 0;

			if (trace->frames_len == trace->frames_size) {
				trace->frames_size = trace->frames_size * 2;
                                trace->frames = (stacktrace_frame*)realloc(trace->frames, sizeof(struct stacktrace_frame) * trace->frames_size);
			}
			trace->frames[trace->frames_len++] = frame;
			return _URC_NO_REASON;
		}

		inline struct stacktrace* stacktrace_get(unsigned skip) {
			char procf[512];
			int len, n;

                        struct stacktrace* trace = (struct stacktrace*)malloc(sizeof(struct stacktrace));
			trace->skip = skip + 1;
			trace->maps = NULL;
			trace->exe = NULL;

			trace->frames_size = 128;
			trace->frames_len = 0;
                        trace->frames = (stacktrace_frame*)malloc(sizeof(struct stacktrace_frame) * trace->frames_size);

			trace->files_len = 0;
			trace->files = NULL;

			snprintf(procf, sizeof(procf), "/proc/%d/exe", (int)getpid());

			for (len = 512; ; len *= 2) {
                                trace->exe = (char*)realloc(trace->exe, len);

				n = readlink(procf, trace->exe, len);
				if (n == -1) {
					break;
				}
				if (n < len) {
					break;
				}
			}

			snprintf(procf, sizeof(procf), "/proc/%d/maps", (int)getpid());
			trace->maps = read_whole_file(procf);

			_Unwind_Backtrace(collect, trace);

			return trace;
		}

		inline void stacktrace_free(struct stacktrace* trace) {
			if (trace != NULL) {
				free(trace->exe);
				free(trace->maps);
				if (trace->frames != NULL) {
					int i;
					for (i = 0; i < trace->frames_len; i++) {
						free(trace->frames[i].func);
						free(trace->frames[i].file);
					}
				}
				free(trace->frames);
				free(trace->files);
				free(trace);
			}
		}

		static void read_map(struct stacktrace* trace) {
			char* saveptr;
			char* line;

			if (trace->files_len > 0) {
				return;
			}

			size_t files_size = 1;
                        trace->files = (file_map*)malloc(sizeof(struct file_map) * files_size);

			line = strtok_r(trace->maps, "\n", &saveptr);
			while (line != NULL) {
				char* p;
				char* saveptr2;
				unsigned long long start, end, offset;
				char* name;

				if (trace->files_len >= files_size) {
					files_size *= 2;
                                        trace->files = (file_map*)realloc(trace->files, sizeof(struct file_map) * files_size);
				}

				/* sscanf requires different format strings for 32/64 bits :( */
				p = strtok_r(line, "-", &saveptr2);
				if (p != NULL) {
					start = strtoull(p, NULL, 16);
					p = strtok_r(NULL, " ", &saveptr2);
					if (p != NULL) {
						end = strtoull(p, NULL, 16);
						p = strtok_r(NULL, " ", &saveptr2);
						if (p != NULL) {
							p = strtok_r(NULL, " ", &saveptr2);
							if (p != NULL) {
								offset = strtoull(p, NULL, 16);
								p = strtok_r(NULL, " ", &saveptr2);
								if (p != NULL) {
									p = strtok_r(NULL, " ", &saveptr2);
									if (p != NULL) {
										p = strtok_r(NULL, " ", &saveptr2);
										if (p != NULL) {
											name = p;
										}
										else {
											name = trace->exe;
										}

										trace->files[trace->files_len].start = start;
										trace->files[trace->files_len].end = end;
										trace->files[trace->files_len].offset = offset;
										trace->files[trace->files_len].file = name;
										trace->files_len++;
									}
								}
							}
						}
					}
				}

				line = strtok_r(NULL, "\n", &saveptr);
			}
		}

		static struct file_map* _find_file(struct stacktrace* trace, unsigned long long addr) {
			int i;

			for (i = 0; i < trace->files_len; i++) {
				if (trace->files[i].start <= addr && addr <= trace->files[i].end) {
					return &trace->files[i];
				}
			}
			return NULL;
		}

		static void _addr2line(struct stacktrace_frame* frame, struct file_map* file) {
			FILE* f;
			char cmd[1024];
			char line[1024];
			snprintf(cmd, sizeof(cmd), "addr2line -C -f -e %s 0x%llx",
				file->file, (unsigned long long)(file->offset + frame->addr));
			/*printf("CMD: %s\n", cmd);*/
			f = popen(cmd, "r");
			if (f == NULL) {
				return;
			}
			if (fgets(line, sizeof(line), f) != NULL) {
				char* p = strchr(line, '\n');
				if (p != NULL) {
					*p = '\0';
				}
				frame->func = strdup(line);
				if (fgets(line, sizeof(line), f) != NULL) {
					p = strchr(line, ':');

					if (p != NULL) {
						*p++ = '\0';
						frame->line = atoi(p);
					}
					if (strcmp(line, "??") == 0) {
						frame->file = strdup(file->file);
					}
					else {
						frame->file = strdup(line);
					}
				}
			}
			pclose(f);
		}

		inline void stacktrace_resolve(struct stacktrace* trace) {
			int i;

			read_map(trace);

			for (i = 0; i < trace->frames_len; i++) {
				struct file_map* file;
				file = _find_file(trace, (unsigned long long)trace->frames[i].addr);
				if (file == NULL) {
					continue;
				}
				_addr2line(&trace->frames[i], file);
			}
		}

#ifdef __cplusplus
	}
#endif

        inline std::string stacktrace_print(struct stacktrace* trace)
	{
		int i;
		std::ostringstream oss;

                stacktrace_resolve(trace);
		for (i = 0; i < trace->frames_len; i++) {
			char cmd[1024];
			struct stacktrace_frame* frame = &trace->frames[i];
			snprintf(cmd, sizeof(cmd), "#%d %p - %s in %s:%d\n", i, frame->addr,
				frame->func ? frame->func : "??", frame->file ? frame->file : "??", frame->line);
			oss << cmd;
		}

		return oss.str();
	}
#endif

inline std::string print_stacktrace(const std::string& pre = "", const std::string& sur = "")
{
#if defined(_WIN32)
	return stack_tracer::stack_trace(pre, sur);
#else        
	struct stacktrace* trace = stacktrace_get(1);
	std::string s_trace = stacktrace_print(trace);
	stacktrace_free(trace);
	std::ostringstream oss;
	oss << "\n" << pre << "\n";
	oss << s_trace;
	oss << "\n" << sur << "\n";
	return oss.str();
#endif
}
} // namespace rola

std::string get_current_process_name()
{
#if defined(__APPLE__) || defined(__FreeBSD__)
	return getprogname();
#elif defined(_GNU_SOURCE)
	return program_invocation_name;
#elif defined(_WIN32)
	return __argv[0];
#else
	return "?";
#endif
}

inline std::string& terminate_dump_file()
{
	static std::string dpfile;
	return dpfile;
}

void _rola_on_terminate_()
{
	using namespace rola;

	std::string& fname = terminate_dump_file();
	if (fname.empty())
		fname = "terminate_dump.txt";
	
	std::ofstream dump_file(fname.c_str(), std::ios::app | std::ios::out);
	dump_file << "\n\n@@ --------------------------------------------------------------------------------------- @@\n\n";
	dump_file << std::chrono::system_clock::now() << ", process name: " << get_current_process_name() << "\n";

	try {
		auto unknown = std::current_exception();
		if (unknown) {
			std::rethrow_exception(unknown);
		}
		else {
			 dump_file << "normal termination\n";
		}
	}

	catch (const std::exception& e) { // for proper `std::` exceptions
		dump_file << "unexpected exception:\n";
		dump_file << "type: " << typeid(std::decay_t<decltype(e)>).name();
		dump_file << "\nwhat: " << e.what() << "\n";
	}
	catch (...) { // last resort for things like `throw 1;`
		dump_file << "unknown exception\n";
	}

	dump_file << "call stack trace:\n";
	dump_file << rola::print_stacktrace("\n", "\n");
}

namespace
{
	static const bool SET_ROLA_TERMINATE = std::set_terminate(_rola_on_terminate_);
} // anonymous namespace

#endif // ROLA_UTILS_STACK_TRACER_HPP
