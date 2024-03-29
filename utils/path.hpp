#ifndef ROLA_UTILS_PATH_HPP
#define ROLA_UTILS_PATH_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <ostream>
#include <fstream>

#if defined(_WIN32)
# include <windows.h>
# include <ShlObj.h>
#include <io.h>
#include <direct.h>
#pragma comment(lib, "shell32.lib")
using mode_t = uint32_t;
#else
# include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <queue>
#endif
#include <sys/stat.h>

#if defined(__linux)
# include <linux/limits.h>
#endif

namespace rola
{
    enum class path_type {
        windows_path = 0,
        posix_path = 1,
#if defined(_WIN32)
        native_path = windows_path
#else
        native_path = posix_path
#endif
    };

	class path
	{
    public: 
        path() noexcept
            : m_type(rola::path_type::native_path)
            , m_absolute(false)
            , m_smb(false) 
        {}

        path(const path& path) noexcept
            : m_type(path.m_type)
            , m_path(path.m_path)
            , m_absolute(path.m_absolute)
            , m_smb(path.m_smb) 
        {}

        path(path&& path) noexcept
            : m_type(path.m_type)
            , m_path(std::move(path.m_path))
            , m_absolute(path.m_absolute)
            , m_smb(path.m_smb) 
        {}

        path(const char* string) { set(string); }

        path(const std::string& string) { set(string); }

#if defined(_WIN32)
        path(const std::wstring& wstring) { set(wstring); }
        path(const wchar_t* wstring) { set(wstring); }
#endif

        path& operator=(const path& path) {
            m_type = path.m_type;
            m_path = path.m_path;
            m_absolute = path.m_absolute;
            m_smb = path.m_smb;
            return *this;
        }

        path& operator=(path&& path) noexcept {
            if (this != &path) {
                m_type = path.m_type;
                m_path = std::move(path.m_path);
                m_absolute = path.m_absolute;
                m_smb = path.m_smb;
            }
            return *this;
        }

    public:
        size_t length() const { return m_path.size(); }

        bool empty() const { return m_path.empty(); }

        bool is_absolute() const { return m_absolute; }

        std::vector<std::string>& paths()
        {
            return m_path;
        }

        path& replace(const size_t index, const std::string& newpath)
        {
            if (index >= m_path.size())
                return *this;

            m_path[index] = newpath;
            return *this;
        }

        path make_absolute() const {
#if !defined(_WIN32)
            char temp[PATH_MAX];
            if (realpath(str().c_str(), temp) == NULL)
                throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
            return path(temp);
#else
            std::wstring value = wstr(), out(MAX_PATH_WINDOWS, '\0');
            DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH_WINDOWS, &out[0], NULL);
            if (length == 0)
                throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
            return path(out.substr(0, length));
#endif
        }

        bool exists() const {
#if defined(_WIN32)
            return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
            struct stat sb;
            return stat(str().c_str(), &sb) == 0;
#endif
        }

        size_t file_size() const {
#if defined(_WIN32)
            struct _stati64 sb;
            if (_wstati64(wstr().c_str(), &sb) != 0)
                throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#else
            struct stat sb;
            if (stat(str().c_str(), &sb) != 0)
                throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#endif
            return (size_t)sb.st_size;
        }

		size_t file_size_noexcept() const noexcept {
#if defined(_WIN32)
			struct _stati64 sb;
            if (_wstati64(wstr().c_str(), &sb) != 0)
                return 0;
#else
			struct stat sb;
            if (stat(str().c_str(), &sb) != 0)
                return 0;
#endif
			return (size_t)sb.st_size;
		}

        bool is_directory() const {
#if defined(_WIN32)
            DWORD result = GetFileAttributesW(wstr().c_str());
            if (result == INVALID_FILE_ATTRIBUTES)
                return false;
            return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
            struct stat sb;
            if (stat(str().c_str(), &sb))
                return false;
            return S_ISDIR(sb.st_mode);
#endif
        }

        bool is_file() const {
#if defined(_WIN32)
            DWORD attr = GetFileAttributesW(wstr().c_str());
            return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
            struct stat sb;
            if (stat(str().c_str(), &sb))
                return false;
            return S_ISREG(sb.st_mode);
#endif
        }

        std::string extension() const {
            const std::string& name = filename();
            size_t pos = name.find_last_of(".");
            if (pos == std::string::npos)
                return "";
            return name.substr(pos + 1);
        }

        std::string filename() const {
            if (empty())
                return "";
            const std::string& last = m_path[m_path.size() - 1];
            return last;
        }

        path parent_path() const {
            path result;
            result.m_absolute = m_absolute;
            result.m_smb = m_smb;

            if (m_path.empty()) {
                if (!m_absolute)
                    result.m_path.push_back("..");
            }
            else {
                size_t until = m_path.size() - 1;
                for (size_t i = 0; i < until; ++i)
                    result.m_path.push_back(m_path[i]);
            }
            return result;
        }

        std::string str(path_type type = rola::path_type::native_path) const {
            std::ostringstream oss;

            if (m_absolute) {
                if (m_type == rola::path_type::posix_path)
                    oss << "/";
                else {
                    size_t length = 0;
                    for (size_t i = 0; i < m_path.size(); ++i)
                        // No special case for the last segment to count the NULL character
                        length += m_path[i].length() + 1;
                    if (m_smb)
                        length += 2;

                    // Windows requires a \\?\ prefix to handle paths longer than MAX_PATH
                    // (including their null character). NOTE: relative paths >MAX_PATH are
                    // not supported at all in Windows.
                    if (length > MAX_PATH_WINDOWS_LEGACY) {
                        if (m_smb)
                            oss << "\\\\?\\UNC\\";
                        else
                            oss << "\\\\?\\";
                    }
                    else if (m_smb)
                        oss << "\\\\";
                }
            }

            for (size_t i = 0; i < m_path.size(); ++i) {
                oss << m_path[i];
                if (i + 1 < m_path.size()) {
                    if (type == rola::path_type::posix_path)
                        oss << '/';
                    else
                        oss << '\\';
                }
            }

            return oss.str();
        }

        void set(const std::string& str, path_type type = rola::path_type::native_path) {
            m_type = type;
            if (type == rola::path_type::windows_path) {
                std::string tmp = str;

                // Long windows paths (sometimes) begin with the prefix \\?\. It should only
                // be used when the path is >MAX_PATH characters long, so we remove it
                // for convenience and add it back (if necessary) in str()/wstr().
                static const std::string LONG_PATH_PREFIX = "\\\\?\\";
                if (tmp.length() >= LONG_PATH_PREFIX.length()
                    && std::mismatch(std::begin(LONG_PATH_PREFIX), std::end(LONG_PATH_PREFIX), std::begin(tmp)).first == std::end(LONG_PATH_PREFIX)) {
                    tmp.erase(0, LONG_PATH_PREFIX.length());
                }

                // Special-case handling of absolute SMB paths, which start with the prefix "\\".
                if (tmp.length() >= 2 && tmp[0] == '\\' && tmp[1] == '\\') {
                    m_path = {};
                    tmp.erase(0, 2);

                    // Interestingly, there is a special-special case where relative paths may be specified as beginning with a "\\"
                    // when a non-SMB file with a more-than-260-characters-long absolute _local_ path is double-clicked. This seems to
                    // only happen with single-segment relative paths, so we can check for this condition by making sure no further
                    // path separators are present.
                    if (tmp.find_first_of("/\\") != std::string::npos)
                        m_absolute = m_smb = true;
                    else
                        m_absolute = m_smb = false;

                    // Special-case handling of absolute SMB paths, which start with the prefix "UNC\"
                }
                else if (tmp.length() >= 4 && tmp[0] == 'U' && tmp[1] == 'N' && tmp[2] == 'C' && tmp[3] == '\\') {
                    m_path = {};
                    tmp.erase(0, 4);
                    m_absolute = true;
                    m_smb = true;
                    // Special-case handling of absolute local paths, which start with the drive letter and a colon "X:\"
                }
                else if (tmp.length() >= 3 && std::isalpha(tmp[0]) && tmp[1] == ':' && (tmp[2] == '\\' || tmp[2] == '/')) {
                    m_path = { tmp.substr(0, 2) };
                    tmp.erase(0, 3);
                    m_absolute = true;
                    m_smb = false;
                    // Relative path
                }
                else {
                    m_path = {};
                    m_absolute = false;
                    m_smb = false;
                }

                std::vector<std::string> tokenized = tokenize(tmp, "/\\");
                m_path.insert(std::end(m_path), std::begin(tokenized), std::end(tokenized));
            }
            else {
                m_path = tokenize(str, "/");
                m_absolute = !str.empty() && str[0] == '/';
            }
        }
    
        bool remove_file() {
#if !defined(_WIN32)
            return std::remove(str().c_str()) == 0;
#else
            //return DeleteFileW(wstr().c_str()) != 0;
            return DeleteFileA(str().c_str()) != 0;
#endif
        }

        bool resize_file(size_t target_length) {
#if !defined(_WIN32)
            return ::truncate(str().c_str(), (off_t)target_length) == 0;
#else
            HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            LARGE_INTEGER size;
            size.QuadPart = (LONGLONG)target_length;
            if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0) {
                CloseHandle(handle);
                return false;
            }
            if (SetEndOfFile(handle) == 0) {
                CloseHandle(handle);
                return false;
            }
            CloseHandle(handle);
            return true;
#endif
        }

        static path getcwd() {
#if !defined(_WIN32)
            char temp[PATH_MAX];
            if (::getcwd(temp, PATH_MAX) == NULL)
                throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
            return path(temp);
#else
            std::wstring temp(MAX_PATH_WINDOWS, '\0');
            if (!_wgetcwd(&temp[0], MAX_PATH_WINDOWS))
                throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
            return path(temp.c_str());
#endif
        }

#if defined(_WIN32)
        std::wstring wstr(path_type type = rola::path_type::native_path) const {
            std::string temp = str(type);
            int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
            std::wstring result(size, 0);
            MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
            return result;
        }


        void set(const std::wstring& wstring, path_type type = rola::path_type::native_path) {
            std::string string;
            if (!wstring.empty()) {
                int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
                    NULL, 0, NULL, NULL);
                string.resize(size, 0);
                WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
                    &string[0], size, NULL, NULL);
            }
            set(string, type);
        }

        path& operator=(const std::wstring& str) { set(str); return *this; }
#endif

        bool operator==(const path& p) const { return p.m_path == m_path; }
        bool operator!=(const path& p) const { return p.m_path != m_path; }

    public:
        friend std::ostream& operator<<(std::ostream& os, const path& path) {
            os << path.str();
            return os;
        }

        path operator/(const path& other) const {
            if (other.m_absolute)
                throw std::runtime_error("path::operator/(): expected a relative path!");
            if (m_type != other.m_type)
                throw std::runtime_error("path::operator/(): expected a path of the same type!");

            path result(*this);

            for (size_t i = 0; i < other.m_path.size(); ++i)
                result.m_path.push_back(other.m_path[i]);

            return result;
        }

    protected:
#if defined(_WIN32)
        static const size_t MAX_PATH_WINDOWS = 32767;
#endif
        static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
        path_type m_type;
        std::vector<std::string> m_path;
        bool m_absolute;
        bool m_smb; // Unused, except for on Windows

    protected:
        static std::vector<std::string> tokenize(const std::string& string, const std::string& delim) {
            std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
            std::vector<std::string> tokens;

            while (lastPos != std::string::npos) {
                if (pos != lastPos)
                    tokens.push_back(string.substr(lastPos, pos - lastPos));
                lastPos = pos;
                if (lastPos == std::string::npos || lastPos + 1 == string.length())
                    break;
                pos = string.find_first_of(delim, ++lastPos);
            }

            return tokens;
        }
	}; // class path

    inline bool create_directory(const path& p) {
#if defined(_WIN32)
        return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
        return mkdir(p.str().c_str(), S_IRWXU) == 0;
#endif
    }

    inline bool create_directories(const path& p) {
#if defined(_WIN32)
        return SHCreateDirectory(nullptr, p.make_absolute().wstr().c_str()) == ERROR_SUCCESS;
#else
        if (create_directory(p.str().c_str()))
            return true;

        if (p.empty())
            return false;

        if (errno == ENOENT) {
            if (create_directories(p.parent_path()))
                return mkdir(p.str().c_str(), S_IRWXU) == 0;
            else
                return false;
        }
        return false;
#endif
    }

    inline bool remove_directory(const path& p)
    {
        if (!p.exists()) return false;
        if (!p.is_directory()) return false;

#if defined(_WIN32)
        return RemoveDirectoryW(p.wstr().c_str()) != 0;
#else
        return rmdir(p.str().c_str()) == 0;
#endif
    }

    inline void remove_directories(const path& p)
    {
        if (!p.exists()) return;
        if (!p.is_directory()) return;

#if defined(_WIN32)
        intptr_t h_file = 0;
        _finddata_t fileinfo;
        if ((h_file = _findfirst(p.str().append("\\*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                if ((fileinfo.attrib & _A_SUBDIR))
                {
                    if (strcmp(fileinfo.name, ".") != 0 &&
                        strcmp(fileinfo.name, "..") != 0)
                        remove_directories(p / path(fileinfo.name));
                }
                else
                {
                    auto pfile = p / path(fileinfo.name);
                    pfile.remove_file();
                }
            } while (_findnext(h_file, &fileinfo) == 0);
            _findclose(h_file);
            remove_directory(p);
        }
#else
        dirent* dent;
        DIR* dir;
        dir = opendir(p.str().c_str());
        while ((dent = readdir(dir)) != nullptr)
        {
            if (strcmp(dent->d_name, ".") == 0 ||
                strcmp(dent->d_name, "..") == 0)
                continue;

            auto new_path = p / path(dent->d_name);
            if (new_path.is_file())
                new_path.remove_file();
            else if (new_path.is_directory())
                remove_directories(new_path);
        }
        closedir(dir);
        remove_directory(p);

#endif
    }

    inline bool path_files(const char* lpPath, std::vector<std::string>& result)
    {
#if defined(_WIN32)
		char szFind[MAX_PATH];
		char szFile[MAX_PATH];

		WIN32_FIND_DATAA FindFileData;

		strcpy_s(szFind, lpPath);
		strcat_s(szFind, "\\*");

		HANDLE hFind = ::FindFirstFileA(szFind, &FindFileData);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}

		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (FindFileData.cFileName[0] != '.')
				{
					strcpy_s(szFile, lpPath);
					strcat_s(szFile, "\\");
					strcat_s(szFile, FindFileData.cFileName);
					path_files(szFile, result);
				}
			}
			else
			{
				if (szFile[0])
				{
					std::string filePath = lpPath;
					filePath += "\\";
					filePath += FindFileData.cFileName;
					result.push_back(filePath);
				}
				else
				{
					std::string filePath = szFile;
					filePath += FindFileData.cFileName;
					result.push_back(filePath);
				}
			}

		} while (::FindNextFileA(hFind, &FindFileData));

		::FindClose(hFind);
		return true;
#else
		std::queue<std::string> queue;
		std::string dirname;

		DIR* dir;
		if (!(dir = opendir(lpPath))) {
			return false;
		}
		queue.push(lpPath);

		struct dirent* ent;
		while (!queue.empty())
		{
			dirname = queue.front();
			dir = opendir(dirname.c_str());
			queue.pop();
			if (!dir) { continue; }

			while (ent = readdir(dir))
			{
				if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
				{
					continue;
				}
				if (ent->d_type == DT_DIR)
				{
					queue.push(dirname + "/" + ent->d_name);
				}
				else
				{
					result.push_back(dirname + "/" + ent->d_name);
				}
			}

			closedir(dir);
		}
		return true;
#endif
    }

    inline void create_file(const std::string& filepath)
    {
		std::ofstream fout(filepath, std::ios::trunc | std::ios::binary);
		if (!fout.is_open())
			return;

        fout.close();
    }

#if defined(unix)

#define FP_SPECIAL 1
#define STR_SIZE sizeof("rwxrwxrwx")

    namespace detail
    {
		inline char* file_perm_str(mode_t perm, int flags)
		{
			static char str[STR_SIZE];
			snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
				(perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
				(perm & S_IXUSR) ?
				(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
				(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
				(perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
				(perm & S_IXGRP) ?
				(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
				(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
				(perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
				(perm & S_IXOTH) ?
				(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
				(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));

			return str;
		}

        inline char file_perm_val(std::string const& mode_block)
        {
            char mode = 0;
            if ((mode_block[0] == 'r') || (mode_block[0] == 'R'))
                mode += 4;
            if ((mode_block[1] == 'w') || (mode_block[1] == 'W'))
                mode += 2;
            if ((mode_block[2] == 'x') || (mode_block[2] == 'X'))
                mode += 1;

            return mode;
        }

        inline std::string file_perm_valstr(mode_t perm)
        {
            std::string perm_str = file_perm_str(perm, 0);
            char usr = file_perm_val(perm_str.substr(0, 3));
            char grp = file_perm_val(perm_str.substr(3, 3));
            char oth = file_perm_val(perm_str.substr(6, 3));
            std::ostringstream oss;
            oss << static_cast<int>(usr) << static_cast<int>(grp) << static_cast<int>(oth);
            return oss.str();
        }
    } // namespace detail

#endif

    // Linux: "rwxrw-rw-" -> "766"
    // windows: "---"
    // error: "---"
    inline std::string file_per_valstr(const char* file_path)
    {
#if defined(_WIN32)
        return "---";
#else
        struct stat file_stat;
        if (::stat(file_path, &file_stat) < 0)
            return "---";
        if (S_ISREG(file_stat.st_mode))
        {
            return detail::file_perm_valstr(file_stat.st_mode);
        }
        return "---";
#endif
    }
} // namespace rola

#endif // !ROLA_UTILS_PATH_HPP
