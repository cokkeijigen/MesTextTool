#include <iostream>
#include <algorithm>
#include <ranges>
#include <xfsys.hpp>

namespace xfsys
{
	static auto to_string(const std::wstring_view path, const uint32_t codepage) -> std::string
	{
		if (!path.empty())
		{
			const auto msize{ ::WideCharToMultiByte(codepage, 0, path.data(), path.size(), nullptr, 0, NULL, NULL) };
			if (msize != 0)
			{
				auto buffer{ std::string(msize, '\0') };
				auto data{ reinterpret_cast<char*>(buffer.data()) };
				auto result{ ::WideCharToMultiByte(codepage, 0, path.data(), path.size(), data, msize, NULL, NULL) };
				if (result == msize)
				{
					return std::move(buffer);
				}
			}
		}
		return std::string{ "" };
	}

	static auto to_wstring(const std::string_view  path, const uint32_t codepage) -> std::wstring
	{
		if (!path.empty())
		{
			const auto mstr{ reinterpret_cast<const char*>(path.data()) };
			const auto wsize{ ::MultiByteToWideChar(codepage, 0, mstr, path.size(), nullptr, 0)};

			if (wsize != 0)
			{
				auto buffer{ std::wstring(static_cast<size_t>(wsize), L'\0') };
				auto result{ ::MultiByteToWideChar(codepage, 0, mstr, path.size(), buffer.data(), wsize) };

				if (result == wsize)
				{
					return std::move(buffer);
				}
			}
		}

		return std::wstring{ L"" };
	}

	auto path::name(const std::u8string_view path) -> std::u8string_view
	{
		auto u16path{ to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8) };

		const auto last_index{ u16path.find_last_of(L"\\/") };
		if (last_index != std::wstring_view::npos)
		{
			size_t current_index{}, target_index{};
			while (current_index < path.size() && target_index <= last_index)
			{
				if (target_index == last_index)
				{
					return path.substr(current_index + 1);
				}
				const auto u8char{ path.data()[current_index] };
				if ((u8char & 0x80) == 0x00)
				{
					current_index += 1;
				}
				else if ((u8char & 0xE0) == 0xC0)
				{
					current_index += 2;
				}
				else if ((u8char & 0xF0) == 0xE0)
				{
					current_index += 3;
				}
				else if ((u8char & 0xF8) == 0xF0)
				{
					current_index += 4;
				}
				else 
				{
					current_index++;
				}
				target_index++;
			}
		}
		return path;
	}

	auto path::parent(const std::u8string_view path) -> std::u8string_view
	{
		auto u16path{ to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8) };

		const auto last_index{ u16path.find_last_of(L"\\/") };
		if (last_index != std::wstring_view::npos)
		{
			size_t current_index{}, target_index{};
			while (current_index < path.size() && target_index <= last_index)
			{
				if (target_index == last_index)
				{
					return path.substr(0, current_index + 1);
				}
				const auto u8char{ path.data()[current_index] };
				if ((u8char & 0x80) == 0x00)
				{
					current_index += 1;
				}
				else if ((u8char & 0xE0) == 0xC0)
				{
					current_index += 2;
				}
				else if ((u8char & 0xF0) == 0xE0)
				{
					current_index += 3;
				}
				else if ((u8char & 0xF8) == 0xF0)
				{
					current_index += 4;
				}
				else
				{
					current_index++;
				}
				target_index++;
			}
		}
		return {};
	}

	directory::u8string_path_iterator::u8string_path_iterator(const std::u8string_view path) noexcept:
		base_path_iterator{ to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8) }
	{
	}

	auto directory::u8string_path_iterator::entry::name() const noexcept -> std::u8string 
	{
		auto name{ to_string(base_path_iterator::entry::name(), CP_UTF8) };
		return *reinterpret_cast<std::u8string*>(&name);
	}

	auto directory::u8string_path_iterator::entry::full_path() const noexcept -> std::u8string
	{
		auto full_path{ to_string(base_path_iterator::entry::full_path(), CP_UTF8) };
		return *reinterpret_cast<std::u8string*>(&full_path);
	}

	auto dir::create(const std::string_view path, const bool create_if_no_exists) -> bool
	{
		return create_directory(path, create_if_no_exists);
	}

	auto dir::create(const std::wstring_view path, const bool create_if_no_exists) -> bool
	{
		return create_directory(path, create_if_no_exists);
	}

	auto dir::create(const std::u8string_view path, const bool create_if_no_exists) -> bool
	{
		return create_directory(path, create_if_no_exists);
	}

	auto dir::create(const std::u16string_view path, const bool create_if_no_exists) -> bool
	{
		return create_directory(path, create_if_no_exists);
	}

	auto open(const std::string_view path, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		const auto dwDesiredAccess
		{
			(mode & open_mode_t::read ? GENERIC_READ : 0) |
			(mode & open_mode_t::write ? GENERIC_WRITE : 0)
		};
		const auto dwCreationDisposition
		{
			create_if_no_exists && (mode& open_mode_t::write) ?
			OPEN_ALWAYS : OPEN_EXISTING
		};

		std::string target_path{ path };
		if (target_path.empty())
		{
			return xfsys::file{ nullptr };
		}

		if (target_path.size() > MAX_PATH && !target_path.starts_with("\\\\?\\"))
		{
			target_path.insert(0, "\\\\?\\");
		}

		auto create_file_handle
		{
			::CreateFileA
			(
				target_path.data(), dwDesiredAccess, FILE_SHARE_READ, NULL,
				dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL
			)
		};

		return xfsys::file{ std::move(create_file_handle) };
	}

	auto open(const std::wstring_view path, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		const auto dwDesiredAccess
		{
			(mode & open_mode_t::read  ? GENERIC_READ  : 0) |
			(mode & open_mode_t::write ? GENERIC_WRITE : 0)
		};
		const auto dwCreationDisposition
		{
			create_if_no_exists && (mode & open_mode_t::write) ?
			OPEN_ALWAYS : OPEN_EXISTING
		};

		std::wstring target_path{ path };
		if (target_path.empty())
		{
			return xfsys::file{ nullptr };
		}

		if (target_path.size() > MAX_PATH && !target_path.starts_with(L"\\\\?\\"))
		{
			target_path.insert(0, L"\\\\?\\");
		}

		auto create_file_handle
		{
			::CreateFileW
			(
				target_path.data(), dwDesiredAccess, FILE_SHARE_READ, NULL,
				dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL
			)
		};

		return xfsys::file{ std::move(create_file_handle) };
	}

	auto open(const std::u8string_view path, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		const auto _path{ to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8) };
		return { xfsys::open(_path, mode, create_if_no_exists) };
	}

	auto open(const std::u16string_view path, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		return { xfsys::open(*reinterpret_cast<const std::wstring_view*>(&path), mode, create_if_no_exists) };
	}

	auto open(const std::string_view directory, const std::string_view file, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		if (static_cast<bool>(mode & open_mode_t::read) && !xfsys::is_directory(directory))
		{
			return xfsys::file{ nullptr };
		}

		if (!xfsys::create_directory(directory, true))
		{
			return xfsys::file{ nullptr };
		}

		return xfsys::open(xfsys::path::join(directory, file), mode, create_if_no_exists);
	}

	auto open(const std::wstring_view directory, const std::wstring_view file, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		if (static_cast<bool>(mode & open_mode_t::read) && !xfsys::is_directory(directory))
		{
			return xfsys::file{ nullptr };
		}

		if (!xfsys::create_directory(directory, true))
		{
			return xfsys::file{ nullptr };
		}

		return xfsys::open(xfsys::path::join(directory, file), mode, create_if_no_exists);
	}

	auto open(const std::u8string_view directory, const std::u8string_view file, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		const auto _directory{ reinterpret_cast<const std::string_view*>(&directory) };
		const auto _file{ reinterpret_cast<const std::string_view*>(&file) };
		return xfsys::open(to_wstring(*_directory, CP_UTF8), to_wstring(*_file, CP_UTF8), mode, create_if_no_exists);
	}

	auto open(const std::u16string_view directory, const std::u16string_view file, const open_mode_t mode, const bool create_if_no_exists) -> xfsys::file
	{
		const auto _directory{ reinterpret_cast<const std::wstring_view*>(&directory) };
		const auto _file{ reinterpret_cast<const std::wstring_view*>(&file) };
		return xfsys::open(*_directory, *_file, mode, create_if_no_exists);
	}

	auto create(const std::string_view path) -> xfsys::file
	{
		std::string target_path{ path };
		if (target_path.empty())
		{
			return xfsys::file{ nullptr };
		}

		if (target_path.size() > MAX_PATH && !target_path.starts_with("\\\\?\\"))
		{
			target_path.insert(0, "\\\\?\\");
		}

		auto create_file_handle
		{
			::CreateFileA
			(
				target_path.data(), GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL
			)
		};
		return xfsys::file{ std::move(create_file_handle) };
	}

	auto create(const std::wstring_view path) -> xfsys::file
	{
		std::wstring target_path{ path };
		if (target_path.empty())
		{
			return xfsys::file{ nullptr };
		}

		if (target_path.size() > MAX_PATH && !target_path.starts_with(L"\\\\?\\"))
		{
			target_path.insert(0, L"\\\\?\\");
		}

		auto create_file_handle
		{
			::CreateFileW
			(
				target_path.data(), GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ, NULL, CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, NULL
			)
		};
		return xfsys::file{ std::move(create_file_handle) };
	}

	auto create(const std::u8string_view path) -> xfsys::file
	{
		const auto _path{ to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8) };
		return { xfsys::create(_path) };
	}

	auto create(const std::u16string_view path) -> xfsys::file
	{
		return { xfsys::create(*reinterpret_cast<const std::wstring_view*>(&path)) };
	}

	auto create(const std::string_view directory, std::string_view file) -> xfsys::file
	{
		if (!xfsys::create_directory(directory, true))
		{
			return xfsys::file{ nullptr };
		}
		return xfsys::create(xfsys::path::join(directory, file));
	}

	auto create(const std::wstring_view directory, const std::wstring_view file) -> xfsys::file
	{
		if (!xfsys::create_directory(directory, true))
		{
			return xfsys::file{ nullptr };
		}
		return xfsys::create(xfsys::path::join(directory, file));
	}

	auto create(const std::u8string_view directory, const std::u8string_view file) -> xfsys::file
	{
		const auto _directory{ reinterpret_cast<const std::string_view*>(&directory) };
		const auto _file{ reinterpret_cast<const std::string_view*>(&file) };
		return xfsys::create(to_wstring(*_directory, CP_UTF8), to_wstring(*_file, CP_UTF8));
	}

	auto create(const std::u16string_view directory, const std::u16string_view file) -> xfsys::file
	{
		const auto _directory{ reinterpret_cast<const std::wstring_view*>(&directory) };
		const auto _file{ reinterpret_cast<const std::wstring_view*>(&file) };
		return xfsys::create(*_directory, *_file);
	}

	auto create_directory(const std::string_view path, const bool create_if_no_exists) -> bool
	{
		std::string target_path{ path };
		
		const auto attributes{ ::GetFileAttributesA(target_path.data()) };

		if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			return create_if_no_exists;
		}

		auto start_pos{ static_cast<size_t>(path.length() >= 2 && path[1] == ':' ? 2 : 0) };
		do 
		{
			size_t separator_pos{ path.find_first_of("\\/", start_pos) };
			if (separator_pos != std::string::npos) 
			{
				target_path.data()[separator_pos] = '\0';
			}

			if (!::CreateDirectoryA(target_path.data(), nullptr))
			{
				if (::GetLastError() != ERROR_ALREADY_EXISTS)
				{
					return false;
				}
			}

			if (separator_pos != std::string::npos)
			{
				target_path.data()[separator_pos] = '\\';
				start_pos = separator_pos + 1;
			}
			else 
			{
				break;
			}

		} while (true);

		return true;
	}

	auto create_directory(const std::wstring_view path, const bool create_if_no_exists) -> bool
	{
		std::wstring target_path{ path };

		const auto attributes{ ::GetFileAttributesW(target_path.data()) };

		if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			return create_if_no_exists;
		}
		
		auto start_pos{ static_cast<size_t>(path.length() >= 2 && path[1] == ':' ? 2 : 0) };
		do 
		{
			size_t separator_pos{ path.find_first_of(L"\\/", start_pos) };
			if (separator_pos != std::wstring::npos) 
			{
				target_path.data()[separator_pos] = L'\0';
			}

			if (!::CreateDirectoryW(target_path.data(), nullptr))
			{
				if (::GetLastError() != ERROR_ALREADY_EXISTS)
				{
					return false;
				}
			}

			if (separator_pos != std::string::npos)
			{
				target_path.data()[separator_pos] = L'\\';
				start_pos = separator_pos + 1;
			}
			else 
			{
				break;
			}

		} while (true);

		return true;
	}

	auto create_directory(const std::u8string_view path, const bool create_if_no_exists) -> bool
	{
		const auto _path{ reinterpret_cast<const std::string_view*>(&path)  };
		return xfsys::create_directory(to_wstring(*_path, CP_UTF8), create_if_no_exists);
	}

	auto create_directory(const std::u16string_view path, const bool create_if_no_exists) -> bool
	{
		return xfsys::create_directory(*reinterpret_cast<const std::wstring_view*>(&path));
	}

	auto extname_check(const std::string_view file, const std::string_view ext) -> bool
	{
		const auto dot_pos{ file.rfind('.') };
		if (dot_pos == std::string_view::npos) 
		{
			return false;
		}

		constexpr const auto whitespace{ " \n\t\v\f\r" };

		std::string_view file_ext{ file.substr(dot_pos) };
		{
			const auto end{ file_ext.find_last_not_of(whitespace) };
			if (end != std::string_view::npos)
			{
				file_ext = file_ext.substr(0, end + 1);
			}
		}
		std::string_view target_ext{ ext };
		{
			const auto beg{ target_ext.find_first_not_of(whitespace) };
			if (beg != std::string_view::npos)
			{
				target_ext = target_ext.substr(beg);
				const auto end{ target_ext.find_last_not_of(whitespace) };
				target_ext = target_ext.substr(0, end + 1);
			}
			else 
			{
				return false;
			}
		}

		const auto result = std::ranges::equal(file_ext, target_ext,
			[](const char& c1, const char& c2) -> bool
			{
				const auto _c1 = std::tolower(static_cast<unsigned char>(c1));
				const auto _c2 = std::tolower(static_cast<unsigned char>(c2));
				return { _c1 == _c2 };
			}
		);
		return { result };
	}

	auto extname_check(const std::wstring_view file, const std::wstring_view ext) -> bool
	{
		const auto dot_pos{ file.rfind(L'.') };
		if (dot_pos == std::string_view::npos)
		{
			return false;
		}

		constexpr const auto whitespace{ L" \n\t\v\f\r" };

		std::wstring_view file_ext{ file.substr(dot_pos) };
		{
			const auto end{ file_ext.find_last_not_of(whitespace) };
			if (end != std::wstring_view::npos)
			{
				file_ext = file_ext.substr(0, end + 1);
			}
		}
		std::wstring_view target_ext{ ext };
		{
			const auto beg{ target_ext.find_first_not_of(whitespace) };
			if (beg != std::wstring_view::npos)
			{
				target_ext = target_ext.substr(beg);
				const auto end{ target_ext.find_last_not_of(whitespace) };
				target_ext = target_ext.substr(0, end + 1);
			}
			else
			{
				return false;
			}
		}

		const auto result = std::ranges::equal(file_ext, target_ext,
			[](const wchar_t& c1, const wchar_t& c2) -> bool
			{
				const auto _c1 = std::tolower(static_cast<unsigned char>(c1));
				const auto _c2 = std::tolower(static_cast<unsigned char>(c2));
				return { _c1 == _c2 };
			}
		);
		return { result };
	}

	auto extname_check(const std::u8string_view file, const std::u8string_view ext) -> bool
	{
		const auto _file{ *reinterpret_cast<const std::string_view*>(&file) };
		const auto _ext { *reinterpret_cast<const std::string_view*>(&ext)  };
		return extname_check(to_wstring(_file, CP_UTF8), to_wstring(_ext, CP_UTF8) );
	}

	auto extname_check(const std::u16string_view file, const std::u8string_view ext) -> bool
	{
		const auto _file{ *reinterpret_cast<const std::wstring_view*>(&file) };
		const auto _ext { *reinterpret_cast<const std::wstring_view*>(&ext)  };
		return extname_check(_file, _ext);
	}

	auto extname_change(const std::string_view file, const std::string_view ext) -> std::string 
	{
		size_t dot_pos{ file.find_last_of(".") };
		if (dot_pos != std::string::npos)
		{
			if (ext.front() != L'.')
			{
				dot_pos += 1;
			}
			return std::string{ file.substr(0, dot_pos) }.append(ext);
		}
		else 
		{
			return std::string{ file }.append(ext);
		}
	}
	
	auto extname_change(const std::wstring_view file, const std::wstring_view ext) -> std::wstring 
	{
		size_t dot_pos{ file.find_last_of(L".") };
		if (dot_pos != std::string::npos)
		{
			if (ext.front() != L'.')
			{
				dot_pos += 1;
			}
			return std::wstring{ file.substr(0, dot_pos) }.append(ext);
		}
		else
		{
			return std::wstring{ file }.append(ext);
		}
	}

	auto extname_change(const std::u8string_view  file, const std::u8string_view ext) -> std::u8string 
	{
		const auto _file{ to_wstring(*reinterpret_cast<const std::string_view*>(&file), CP_UTF8) };
		const auto _ext { to_wstring(*reinterpret_cast<const std::string_view*>(&ext ), CP_UTF8) };
		std::string reuslt{ to_string(extname_change(_file, _ext), CP_UTF8) };
		return *reinterpret_cast<std::u8string*>(&reuslt);
	}

	auto extname_change(const std::u16string_view file, const std::u8string_view ext) -> std::u16string 
	{
		const auto _file{ reinterpret_cast<const std::wstring_view*>(&file) };
		const auto _ext { reinterpret_cast<const std::wstring_view*>(&ext ) };
		std::wstring reuslt{ extname_change(*_file, *_ext) };
		return *reinterpret_cast<std::u16string*>(&reuslt);
	}

	auto is_file(const std::string_view path) -> bool
	{
		const std::string target_path{ path };
		const auto attributes{ ::GetFileAttributesA(target_path.data()) };

		if (attributes == INVALID_FILE_ATTRIBUTES) 
		{
			return false;
		}

		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			return false;
		}

		if (attributes & (FILE_ATTRIBUTE_DEVICE)) 
		{
			return false;
		}

		return true;
	}

	auto is_file(const std::wstring_view path) -> bool
	{
		const std::wstring target_path{ path };
		const auto attributes{ ::GetFileAttributesW(target_path.data()) };

		if (attributes == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			return false;
		}

		if (attributes & (FILE_ATTRIBUTE_DEVICE))
		{
			return false;
		}

		return true;
	}

	auto is_directory(const std::string_view path) -> bool
	{
		const std::string target_path{ path };
		const auto attributes{ ::GetFileAttributesA(target_path.data()) };

		if (attributes == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		return { static_cast<bool>(attributes & FILE_ATTRIBUTE_DIRECTORY) };
	}

	auto is_directory(const std::wstring_view path) -> bool
	{
		const std::wstring target_path{ path };
		const auto attributes{ ::GetFileAttributesW(target_path.data()) };

		if (attributes == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		return { static_cast<bool>(attributes & FILE_ATTRIBUTE_DIRECTORY) };
	}

	auto is_file(const std::u8string_view path) -> bool
	{
		return xfsys::is_file(to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8));
	}

	auto is_file(const std::u16string_view path) -> bool
	{
		return xfsys::is_file(*reinterpret_cast<const std::wstring_view*>(&path));
	}

	auto is_directory(const std::u8string_view path) -> bool
	{
		return xfsys::is_directory(to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8));
	}

	auto is_directory(const std::u16string_view path) -> bool
	{
		return xfsys::is_directory(*reinterpret_cast<const std::wstring_view*>(&path));
	}

	auto is_exists(const std::string_view path) -> bool
	{
		const std::string target_path{ path };
		const auto attr{ ::GetFileAttributesA(target_path.data()) };
		return { INVALID_FILE_ATTRIBUTES != attr };
	}

	auto is_exists(const std::wstring_view path) -> bool
	{
		const std::wstring target_path{ path };
		const auto attr{ ::GetFileAttributesW(target_path.data()) };
		return { INVALID_FILE_ATTRIBUTES != attr };
	}

	auto is_exists(const std::u8string_view path) -> bool
	{
		return xfsys::is_exists(to_wstring(*reinterpret_cast<const std::string_view*>(&path), CP_UTF8));
	}

	auto is_exists(const std::u16string_view path) -> bool
	{
		return xfsys::is_exists(*reinterpret_cast<const std::wstring_view*>(&path));
	}

	auto file::write(const void* buffer, size_t count, pos::method relative,
		size_t offset) const noexcept -> size_t
	{
		if (count == 0)
		{
			return 0;
		}

		if (relative != pos::current || offset != 0)
		{
			this->seek(relative, offset);
		}

		DWORD bytes_written{};
		const auto result{ ::WriteFile(this->m_handle, buffer, count, &bytes_written, NULL) };
		return { static_cast<size_t>(result ? bytes_written : 0) };
	}

	auto file::read(void* buffer, size_t count, pos::method relative,
		size_t offset) const noexcept -> size_t
	{
		if (count == 0)
		{
			return 0;
		}

		if (relative != pos::current || offset != 0)
		{
			this->seek(relative, offset);
		}

		DWORD bytes_read{};
		const auto result{ ::ReadFile(this->m_handle, buffer, count, &bytes_read, NULL) };
		return { static_cast<size_t>(result ? bytes_read : 0) };
	}

	auto file::is_open() const noexcept -> bool
	{
		const auto is_not_handle_known_invalid
		{
			this->m_handle == nullptr ||
			INVALID_HANDLE_VALUE == this->m_handle
		};

		if (is_not_handle_known_invalid)
		{
			return false;
		}

		::SetLastError(NULL);
		const auto type{ ::GetFileType(this->m_handle) };
		if (type == FILE_TYPE_UNKNOWN)
		{
			const auto last_error{ ::GetLastError() };
			if (last_error == ERROR_INVALID_HANDLE)
			{
				return false;
			}
		}
		return true;
	}

	auto file::seek(pos::method relative, size_t offset) const noexcept -> size_t
	{
		LARGE_INTEGER liDistanceToMove{ .QuadPart = static_cast<decltype(LARGE_INTEGER::QuadPart)>(offset) };
		const auto success{ ::SetFilePointerEx(this->m_handle, liDistanceToMove, &liDistanceToMove, relative) };
		return { success ? static_cast<size_t>(liDistanceToMove.QuadPart) : file::error() };
	}

	auto file::rewind() const noexcept -> size_t
	{
		return this->seek(pos::begin, 0);
	}

	auto file::tell() const noexcept -> size_t
	{
		return this->seek(pos::current, 0);
	}

	auto file::size() const noexcept -> size_t
	{
		const auto current{ this->seek(pos::current, 0) };
		const auto end{ this->seek(pos::end, 0) };

		if (current != end && current != file::error())
		{
			this->seek(pos::begin, current);
		}
		return { end != file::error() ? end : static_cast<size_t>(0) };
	}

	auto file::close() const noexcept -> void
	{
		if (this->is_open())
		{
			::CloseHandle(this->m_handle);
		}
		this->m_handle = nullptr;
	}

	auto file::path_of_string() const noexcept -> std::string
	{
		if (!this->is_open())
		{
			return {};
		}

		auto length
		{
			::GetFinalPathNameByHandleA
			(
				{ this->m_handle },
				{ nullptr },
				{ 0x00    },
				{ FILE_NAME_NORMALIZED }
			)
		};

		if (length != 0x00)
		{
			do {
				auto buffer = std::string(length, '\0');
				const auto result
				{
					::GetFinalPathNameByHandleA
					(
						{ this->m_handle },
						{ buffer.data() },
						{ length + 1 },
						{ FILE_NAME_OPENED }
					)
				};

				if (result == 0x00)
				{
					break;
				}

				if (result > length)
				{
					length = result;
					continue;
				}

				if (buffer.starts_with(R"(\\?\)"))
				{
					return buffer.substr(4);
				}
				return { std::move(buffer) };
			} while (true);
		}
		return {};
	}

	auto file::path_of_wstring() const noexcept -> std::wstring
	{
		if (!this->is_open())
		{
			return {};
		}

		auto length
		{
			::GetFinalPathNameByHandleW
			(
				{ this->m_handle },
				{ nullptr },
				{ 0x00    },
				{ FILE_NAME_NORMALIZED }
			)
		};

		if (length != 0x00)
		{
			do {
				auto buffer = std::wstring(length, '\0');
				const auto result
				{
					::GetFinalPathNameByHandleW
					(
						{ this->m_handle },
						{ buffer.data() },
						{ length + 1 },
						{ FILE_NAME_NORMALIZED }
					)
				};

				if (result == 0x00)
				{
					break;
				}

				if (result > length)
				{
					length = result;
					continue;
				}

				if (buffer.starts_with(LR"(\\?\)"))
				{
					return buffer.substr(4);
				}
				return { std::move(buffer) };
			} while (true);
		}
		return {};
	}

	auto file::path_of_u8string() const noexcept -> std::u8string
	{
		const auto u16path{ this->path_of_wstring() };
		if (!u16path.empty())
		{
			auto u8path{ xfsys::to_string(u16path, CP_UTF8) };
			return std::move(*reinterpret_cast<std::u8string*>(&u8path));
		}
		return {};
	}

}