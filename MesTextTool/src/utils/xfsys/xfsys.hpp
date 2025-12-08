#pragma once
#include <windows.h>
#include <algorithm>
#include <vector>

namespace xfsys 
{
	enum open_mode_t
	{
		read  = 0x01, r = 0x01,
		write = 0x10, w = 0x10,
		read_and_write  = r | w, rw = r | w,
	};

	template<class T>
	concept write_buffer_t = requires(T t)
	{
		{ t.size() } -> std::convertible_to<size_t>;
		{ t.data() } -> std::convertible_to<const void*>;
		sizeof(decltype(*t.data())) * t.size();
	};

	template<class T>
	concept read_buffer_t = requires(T t)
	{
		t.resize(0);
		{ t.data() } -> std::convertible_to<void*>;
		sizeof(decltype(*t.data()))* t.size();
	};

	template <typename T>
	concept path_string_t =
		std::is_same_v<std::decay_t<T>, std::string   > ||
		std::is_same_v<std::decay_t<T>, std::wstring  > ||
		std::is_same_v<std::decay_t<T>, std::u8string > ||
		std::is_same_v<std::decay_t<T>, std::u16string>;

	class file
	{
		using handle_t = void*;
		
		mutable handle_t m_handle{};

		auto path_of_string   () const noexcept -> std::string;
		auto path_of_wstring  () const noexcept -> std::wstring;
		auto path_of_u8string () const noexcept -> std::u8string;

	public:

		struct pos
		{
			enum method : DWORD
			{ 
				begin   = FILE_BEGIN, 
				current = FILE_CURRENT,
				end     = FILE_END
			};
		};

		static constexpr auto error() { return static_cast<size_t>(-1); }

		inline ~file() noexcept { this->close(); };

		inline file(const file& other) noexcept = delete;
		inline file& operator=(const file& other) = delete;

		inline file(file&& other) noexcept;
		inline file& operator=(file&& other) noexcept;

		inline file(handle_t&& handle) noexcept : m_handle{ handle } {};
		
		auto seek(pos::method relative, size_t offset) const noexcept -> size_t;

		auto write(write_buffer_t auto&& buffer, bool end_always = false) const noexcept -> size_t;

		auto write(write_buffer_t auto&& buffer, pos::method relative = pos::current,
			size_t offset = 0) const noexcept -> size_t;

		auto write(write_buffer_t auto&& buffer, size_t count, pos::method relative = pos::current,
			size_t offset = 0) const noexcept -> size_t;

		auto write(const void* buffer, size_t count, pos::method relative = pos::current,
			size_t offset = 0) const noexcept -> size_t;

		auto read(void* buffer, size_t count, pos::method relative = pos::current,
			size_t offset = 0) const noexcept -> size_t;

		auto read(read_buffer_t auto& buffer, size_t count, pos::method relative = pos::current,
			size_t offset = 0) const noexcept -> size_t;

		template<class T>
		requires read_buffer_t<T>&& requires{ T{}; }
		auto read(size_t count, pos::method relative = pos::current, 
			size_t offset = 0) const noexcept -> T;

		template<xfsys::path_string_t T = std::string>
		auto path() const noexcept -> T;

		template<xfsys::path_string_t T = std::string>
		auto name() const noexcept -> T;

		template<xfsys::path_string_t T = std::string>
		auto extension() const noexcept -> T;

		auto is_open() const noexcept -> bool;

		auto size() const noexcept -> size_t;

		auto rewind() const noexcept -> size_t;

		auto tell() const noexcept -> size_t;

		auto close() const noexcept -> void;
	};

	inline file::file(file&& other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = nullptr;
	}

	inline file& file::operator=(file&& other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = nullptr;
	}

	inline auto file::write(write_buffer_t auto&& buffer, bool end_always) const noexcept -> size_t
	{
		return { this->write(buffer, { end_always ? pos::end : pos::current }, 0) };
	}

	inline auto file::write(write_buffer_t auto&& buffer, pos::method relative,
		size_t offset) const noexcept -> size_t
	{
		const auto _buffer{ reinterpret_cast<const void*>(buffer.data()) };
		const auto _length{ sizeof(decltype(*buffer.data())) * buffer.size() };
		const auto _wbytes{ this->write(_buffer, _length, relative, offset) };
		if (_wbytes != 0)
		{
			const auto count{ _wbytes / sizeof(decltype(*buffer.data())) };
			return count;
		}
		return 0;
	}

	inline auto file::write(write_buffer_t auto&& buffer, size_t count, pos::method relative,
		size_t offset) const noexcept -> size_t
	{
		const auto _buffer{ reinterpret_cast<const void*>(buffer.data()) };
		const auto _length{ sizeof(decltype(*buffer.data())) * count };
		const auto _wbytes{ this->write(_buffer, _length, relative, offset) };
		if (_wbytes != 0)
		{
			const auto count{ _wbytes / sizeof(decltype(*buffer.data())) };
			return count;
		}
		return 0;
	}

	auto file::read(read_buffer_t auto& buffer, size_t count, pos::method relative,
		size_t offset) const noexcept -> size_t
	{
		if (buffer.size() < count)
		{
			if constexpr (requires{ buffer.clear(); })
			{
				buffer.clear();
			}
			buffer.resize(count);
		}

		const auto _buffer{ reinterpret_cast<void*>(buffer.data())   };
		const auto _length{ sizeof(decltype(*buffer.data())) * count };
		const auto _rbytes{ this->read(_buffer, _length, relative, offset) };
		if (_rbytes != 0)
		{
			const auto count{ _rbytes / sizeof(decltype(*buffer.data())) };
			if constexpr (requires{ buffer.recount(count); })
			{
				buffer.recount(count);
			}
			return count;
		}
		return 0;
	}

	template<class T>
	requires read_buffer_t<T>&& requires{ T{}; }
	inline auto file::read(size_t count, pos::method relative, size_t offset) const noexcept -> T
	{
		T buffer{};
		this->read(buffer, count, relative, offset);
		return { buffer };
	}

	template<xfsys::path_string_t T>
	inline auto file::path() const noexcept -> T
	{
		if constexpr (std::is_same_v<std::decay_t<T>, std::string   >)
		{
			return this->path_of_string();
		}
		if constexpr (std::is_same_v<std::decay_t<T>, std::wstring  >)
		{
			return this->path_of_wstring();
		}
		if constexpr (std::is_same_v<std::decay_t<T>, std::u8string >)
		{
			return this->path_of_u8string();
		}
		if constexpr (std::is_same_v<std::decay_t<T>, std::u16string>)
		{
			auto wstr{ this->path_of_wstring() };
			return std::move(*reinterpret_cast<std::u16string*>(&wstr));
		}
	}

	template<xfsys::path_string_t T>
	inline auto file::name() const noexcept -> T
	{
		auto path{ this->path<T>() };
		if (!path.empty())
		{
			auto pos{ path.rfind('\\') };
			if (pos != T::npos)
			{
				return path.substr(pos + 1);
			}
		}
		return {};
	}

	template<xfsys::path_string_t T>
	inline auto file::extension() const noexcept -> T
	{
		auto path{ this->path<T>() };
		if (!path.empty())
		{
			auto pos{ path.rfind('.') };
			if (pos != T::npos)
			{
				return path.substr(pos);
			}
		}
		return {};
	}

	namespace path 
	{

		template<class ...T, class char_t = decltype(std::declval<std::tuple_element_t<0, std::tuple<T...>>>()[0])>
		requires (std::convertible_to<T, std::basic_string_view<std::decay_t<char_t>>> && ...)
		inline auto join(T&& ... args) -> std::basic_string<std::decay_t<char_t>>
		{
			constexpr const std::decay_t<char_t> WHITESPACE[7]
			{
				static_cast<std::decay_t<char_t>>(' '),
				static_cast<std::decay_t<char_t>>('\n'),
				static_cast<std::decay_t<char_t>>('\t'),
				static_cast<std::decay_t<char_t>>('\v'),
				static_cast<std::decay_t<char_t>>('\f'),
				static_cast<std::decay_t<char_t>>('\r'),
				static_cast<std::decay_t<char_t>>('\0')
			};

			std::vector<std::decay_t<char_t>> buffer{};
			for (auto path : std::initializer_list{ std::basic_string_view<std::decay_t<char_t>>{args}... })
			{
				const auto whitespace_beg{ path.find_first_not_of(WHITESPACE) };
				if (whitespace_beg != std::basic_string_view<std::decay_t<char_t>>::npos)
				{
					path = path.substr(whitespace_beg);
					const auto whitespace_end{ path.find_last_not_of(WHITESPACE) };
					path = path.substr(0, whitespace_end + 1);
				}
				else
				{
					continue;
				}

				if (buffer.empty())
				{
					buffer.append_range(path);
					continue;
				}

				constexpr const std::decay_t<char_t> slashs[3]
				{
					static_cast<std::decay_t<char_t>>('\\'),
					static_cast<std::decay_t<char_t>>('/'),
					0
				};

				if (buffer.back() == slashs[0] || buffer.back() == slashs[1])
				{
					const auto first{ path.find_first_not_of(slashs) };
					if (first != std::basic_string_view<std::decay_t<char_t>>::npos)
					{
						path = path.substr(first);
					}
					buffer.append_range(path);
				}
				else
				{
					if (path.front() != slashs[0] && path.front() != slashs[1])
					{
						buffer.push_back(slashs[0]);
					}
					buffer.append_range(path);
				}
			}
			return std::basic_string<std::decay_t<char_t>>{ std::move(buffer.data()), buffer.size() };
		}

		template<class ...T, class char_t = decltype(std::declval<std::tuple_element_t<0, std::tuple<T...>>>()[0])>
		requires (std::convertible_to<T, std::basic_string_view<std::decay_t<char_t>>> && ...)
		inline auto join_std(T&& ... args) -> std::basic_string<std::decay_t<char_t>>
		{
			constexpr const std::decay_t<char_t> WHITESPACE[7]
			{
				static_cast<std::decay_t<char_t>>(' '),
				static_cast<std::decay_t<char_t>>('\n'),
				static_cast<std::decay_t<char_t>>('\t'),
				static_cast<std::decay_t<char_t>>('\v'),
				static_cast<std::decay_t<char_t>>('\f'),
				static_cast<std::decay_t<char_t>>('\r'),
				static_cast<std::decay_t<char_t>>('\0')
			};
			std::vector<std::decay_t<char_t>> buffer{};
			for (auto path : std::initializer_list{ std::basic_string_view<std::decay_t<char_t>>{args}... })
			{
				const auto whitespace_beg{ path.find_first_not_of(WHITESPACE) };
				if (whitespace_beg == std::basic_string_view<std::decay_t<char_t>>::npos)
				{
					continue;
				}
				else
				{
					path = path.substr(whitespace_beg);
					const auto whitespace_end{ path.find_last_not_of(WHITESPACE) };
					path = path.substr(0, whitespace_end + 1);
				}
				
				size_t current{};
				do 
				{
					constexpr const std::decay_t<char_t> slashs[3]
					{
						static_cast<std::decay_t<char_t>>('\\'),
						static_cast<std::decay_t<char_t>>('/'),
						0
					};
					const auto next{ path.find_first_of(slashs, current) };
					if (next != std::string::npos)
					{
						auto temp{ path.substr(current, next - current) };
						current = next + 1;
						
						if (temp.empty())
						{
							continue;
						}
						buffer.append_range(temp);
						buffer.push_back('\\');
					}
					else 
					{
						auto temp{ path.substr(current) };
						if (!temp.empty())
						{
							buffer.append_range(temp);
							buffer.push_back('\\');
						}
						break;
					}
				} while (true);
			}
			return std::basic_string<std::decay_t<char_t>>{ buffer.data(), buffer.size() };
		}

		inline auto parent(const std::string_view path) -> std::string_view
		{
			const auto last{ path.find_last_of("\\/") };
			if (last != std::string_view::npos)
			{
				return path.substr(0, last + 1);
			}
			return {};
		}

		inline auto parent(const std::wstring_view path) -> std::wstring_view
		{
			const auto last{ path.find_last_of(L"\\/") };
			if (last != std::wstring_view::npos)
			{
				return path.substr(0, last + 1);
			}
			return {};
		}

		inline auto parent(const std::u16string_view path) -> std::u16string_view
		{
			auto _parent{ parent(*reinterpret_cast<const std::wstring_view*>(&path)) };
			return *reinterpret_cast<const std::u16string_view*>(&_parent);
		}

		template<class T, class char_t = std::decay_t<decltype(std::declval<T>()[0])>>
		requires std::convertible_to<T, std::basic_string_view<char_t>>
		inline auto name(T&& path) -> std::basic_string_view<char_t>
		{
			std::basic_string_view<char_t> path_view{ path };
			constexpr const char_t slash[3]{ static_cast<char_t>('\\'), static_cast<char_t>('/'), 0 };

			const auto last{ path_view.find_last_of(slash) };
			if (last != std::basic_string_view<char_t>::npos)
			{
				return path_view.substr(last + 1);
			}
			return path;
		}

		auto name(const std::u8string_view path) -> std::u8string_view;
		auto parent(const std::u8string_view path) -> std::u8string_view;
	}
	
	namespace directory 
	{
		template<class char_t>
		class base_path_iterator
		{
		protected:

			std::basic_string<char_t> m_path{};

		public:

			class entry;
			class iterator;

			using handle_t = void*;
			using win32_find_data_t = std::conditional<sizeof(char_t) == 1, WIN32_FIND_DATAA, WIN32_FIND_DATAW>::type;

			inline base_path_iterator(const std::basic_string_view<char_t> path) noexcept;
			
			inline auto begin() const noexcept -> iterator;

			inline auto end() const noexcept -> iterator;

			class entry 
			{
				friend iterator;

			protected:

				const std::basic_string_view<char_t>& m_path;
				const win32_find_data_t& m_find_data;

				inline entry(const std::basic_string_view<char_t>& path, const win32_find_data_t& find_data) noexcept;

			public:

				inline auto name() const noexcept -> std::basic_string_view<char_t>;
				inline auto full_path() const noexcept -> std::basic_string<char_t>;

				inline auto is_file() const noexcept -> bool;
				inline auto is_directory() const noexcept -> bool;
			};

			class iterator 
			{

			protected:
				
				friend base_path_iterator<char_t>;

				bool m_is_end{};
				std::basic_string_view<char_t> m_path{};

				win32_find_data_t m_find_data{};
				handle_t m_find_handle{ INVALID_HANDLE_VALUE };

				inline iterator(std::basic_string_view<char_t> path) noexcept;
				inline iterator() noexcept;

			public:

				inline ~iterator() noexcept;
				inline auto next() noexcept -> bool;

				inline auto operator* () const noexcept -> entry;
				inline auto operator++() noexcept -> iterator&;
				inline auto operator==(const iterator& other) const noexcept -> bool;
				inline auto operator!=(const iterator& other) const noexcept -> bool;
			};
		};

		class u8string_path_iterator : public base_path_iterator<wchar_t> 
		{
			class entry;
			class iterator;
		public:

			u8string_path_iterator(const std::u8string_view path) noexcept;

			class entry : public base_path_iterator<wchar_t>::entry 
			{
				friend u8string_path_iterator::iterator;

				inline entry(const std::wstring_view& path, const win32_find_data_t& find_data) noexcept;

			public:

				auto name() const noexcept -> std::u8string;
				auto full_path() const noexcept -> std::u8string;
			};

			class iterator : public base_path_iterator<wchar_t>::iterator
			{
				friend u8string_path_iterator;

				inline iterator(std::wstring_view path) noexcept;
				inline iterator() noexcept;

			public:

				inline auto operator*() const noexcept -> u8string_path_iterator::entry;
			};

			inline auto begin() const noexcept -> u8string_path_iterator::iterator;

			inline auto end() const noexcept -> u8string_path_iterator::iterator;
		};

		class u16string_path_iterator : public base_path_iterator<char16_t>
		{
		public:
			using base_path_iterator<char16_t>::base_path_iterator;
		};

		class string_path_iterator : public base_path_iterator<char>
		{
		public:
			using base_path_iterator<char>::base_path_iterator;
		};

		class wstring_path_iterator : public base_path_iterator<wchar_t>
		{
		public:
			using base_path_iterator<wchar_t>::base_path_iterator;
		};

		inline u8string_path_iterator::entry::entry(const std::wstring_view& path, const win32_find_data_t& 
			find_data) noexcept : base_path_iterator::entry{ path, find_data }
		{
		}

		inline u8string_path_iterator::iterator::iterator(std::wstring_view path) 
			noexcept: base_path_iterator::iterator{ path }
		{
		}

		inline u8string_path_iterator::iterator::iterator() 
			noexcept: base_path_iterator::iterator{}
		{
		}

		inline auto u8string_path_iterator::begin() const noexcept -> u8string_path_iterator::iterator
		{
			return u8string_path_iterator::iterator{ this->m_path };
		}

		inline auto u8string_path_iterator::end() const noexcept -> u8string_path_iterator::iterator
		{
			return u8string_path_iterator::iterator{};
		}

		inline auto u8string_path_iterator::iterator::operator*() const noexcept -> u8string_path_iterator::entry 
		{
			return u8string_path_iterator::entry{ this->m_path, this->m_find_data };
		}

		template<class char_t>
		inline base_path_iterator<char_t>::base_path_iterator(const std::basic_string_view<char_t> path) noexcept: m_path{ path }
		{
			std::replace(this->m_path.begin(), this->m_path.end(), static_cast<char_t>('/'), static_cast<char_t>('\\'));
			if (this->m_path.back() != static_cast<char_t>('\\'))
			{
				constexpr const char_t backslash[2]{ static_cast<char_t>('\\'), 0 };
				this->m_path.append(backslash);
			}
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::begin() const noexcept -> iterator
		{
			return base_path_iterator<char_t>::iterator{ this->m_path };
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::end() const noexcept -> iterator
		{
			return base_path_iterator<char_t>::iterator{};
		}

		template<class char_t>
		inline base_path_iterator<char_t>::entry::entry(const std::basic_string_view<char_t>& path,
			const win32_find_data_t& find_data) noexcept: m_path{ path }, m_find_data{ find_data }
		{
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::entry::name() const noexcept -> std::basic_string_view<char_t>
		{
			return std::basic_string_view<char_t>{ reinterpret_cast<const char_t*>(this->m_find_data.cFileName) };
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::entry::full_path() const noexcept -> std::basic_string<char_t>
		{
			return path::join(this->m_path, this->name());
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::entry::is_file() const noexcept -> bool
		{
			return !this->is_directory();
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::entry::is_directory() const noexcept -> bool
		{
			return static_cast<bool>(this->m_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		template<class char_t>
		inline base_path_iterator<char_t>::iterator::iterator() noexcept : m_is_end{ true }
		{
		}

		template<class char_t>
		inline base_path_iterator<char_t>::iterator::~iterator() noexcept
		{
			if (this->m_find_handle != INVALID_HANDLE_VALUE)
			{
				::FindClose(this->m_find_handle);
			}
			this->m_find_handle = INVALID_HANDLE_VALUE;
			this->m_is_end      = true;
			this->m_find_data   = {};
		}

		template<class char_t>
		inline base_path_iterator<char_t>::iterator::iterator(std::basic_string_view<char_t> path) noexcept: m_path{ path }
		{
			constexpr const char_t prefix[5]
			{
				static_cast<char_t>('\\'),
				static_cast<char_t>('\\'),
				static_cast<char_t>('?'),
				static_cast<char_t>('\\'),
				static_cast<char_t>('\0'),
			};

			constexpr const char_t suffix[2]
			{
				static_cast<char_t>('*'),
				static_cast<char_t>('\0'),
			};

			const std::basic_string<char_t> search_path{ path::join(prefix, this->m_path, suffix) };

			if constexpr (sizeof(char_t) == 1)
			{
				const auto str{ reinterpret_cast<const char*>(search_path.data()) };
				this->m_find_handle = ::FindFirstFileA(str, &this->m_find_data);
			}
			else
			{
				const auto str{ reinterpret_cast<const wchar_t*>(search_path.data()) };
				this->m_find_handle = ::FindFirstFileW(str, &this->m_find_data);
			}

			if (this->m_find_handle != INVALID_HANDLE_VALUE)
			{
				std::basic_string_view<char_t> name{ reinterpret_cast<const char_t*>(this->m_find_data.cFileName) };
				constexpr const char_t dn_1[2]{ static_cast<char_t>('.'), 0 };
				constexpr const char_t dn_2[3]{ static_cast<char_t>('.'), static_cast<char_t>('.'), 0 };
				if (name == dn_1 || name == dn_2)
				{
					this->next();
				}
			}
			else
			{
				this->m_is_end = true;
			}
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::iterator::next() noexcept -> bool
		{
			if (this->m_is_end)
			{
				return false;
			}

			if (this->m_find_handle == INVALID_HANDLE_VALUE)
			{
				this->m_is_end = true;
				return false;
			}

			while (true)
			{
				BOOL find{};
				if constexpr (sizeof(char_t) == 1)
				{
					find = ::FindNextFileA(this->m_find_handle, &this->m_find_data);
				}
				else
				{
					find = ::FindNextFileW(this->m_find_handle, &this->m_find_data);
				}

				if (!find)
				{
					::FindClose(this->m_find_handle);
					this->m_find_handle = INVALID_HANDLE_VALUE;
					this->m_is_end = true;
					this->m_find_data = {};
					return false;
				}

				std::basic_string_view<char_t> name{ reinterpret_cast<const char_t*>(this->m_find_data.cFileName) };
				constexpr const char_t dn_1[2]{ static_cast<char_t>('.'), 0 };
				constexpr const char_t dn_2[3]{ static_cast<char_t>('.'), static_cast<char_t>('.'), 0 };
				if (name != dn_1 && name != dn_2)
				{
					return true;
				}
			}
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::iterator::operator*() const noexcept -> entry 
		{
			return base_path_iterator<char_t>::entry{ this->m_path, this->m_find_data };
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::iterator::operator++() noexcept -> iterator& 
		{
			this->next();
			return *this;
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::iterator::operator==(const iterator& other) const noexcept -> bool
		{
			return this->m_is_end == other.m_is_end;
		}

		template<class char_t>
		inline auto base_path_iterator<char_t>::iterator::operator!=(const iterator& other) const noexcept -> bool
		{
			return this->m_is_end != other.m_is_end;
		}

		inline auto iterator(std::string_view path) -> string_path_iterator
		{
			return string_path_iterator{ path };
		}

		inline auto iterator(std::wstring_view path) -> wstring_path_iterator
		{
			return wstring_path_iterator{ path };
		}

		inline auto iterator(std::u16string_view path) -> u16string_path_iterator
		{
			return u16string_path_iterator{ path };
		}

		inline auto iterator(std::u8string_view path) -> u8string_path_iterator
		{
			return u8string_path_iterator{ path };
		}

	}

	namespace dir 
	{
		using namespace directory;

		inline auto iter(std::string_view path) -> string_path_iterator
		{
			return directory::iterator(path);
		}

		inline auto iter(std::wstring_view path) -> wstring_path_iterator
		{
			return directory::iterator(path);
		}

		inline auto iter(std::u16string_view path) -> u16string_path_iterator
		{
			return directory::iterator(path);
		}

		inline auto iter(std::u8string_view path) -> u8string_path_iterator
		{
			return directory::iterator(path);
		}

		auto create(const std::string_view  path, const bool create_if_no_exists = true) -> bool;
		auto create(const std::wstring_view path, const bool create_if_no_exists = true) -> bool;

		auto create(const std::u8string_view  path, const bool create_if_no_exists = true) -> bool;
		auto create(const std::u16string_view path, const bool create_if_no_exists = true) -> bool;
	}

	auto open(const std::string_view  path, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	auto open(const std::wstring_view path, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	
	auto open(const std::u8string_view  path, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	auto open(const std::u16string_view path, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;

	auto open(const std::string_view  directory, const std::string_view  file, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	auto open(const std::wstring_view directory, const std::wstring_view file, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	
	auto open(const std::u8string_view  directory, const std::u8string_view  file, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;
	auto open(const std::u16string_view directory, const std::u16string_view file, const open_mode_t mode = rw, const bool create_if_no_exists = true) -> xfsys::file;

	auto create(const std::string_view  path) -> xfsys::file;
	auto create(const std::wstring_view path) -> xfsys::file;

	auto create(const std::u8string_view  path) -> xfsys::file;
	auto create(const std::u16string_view path) -> xfsys::file;

	auto create(const std::string_view  directory, const std::string_view  file) -> xfsys::file;
	auto create(const std::wstring_view directory, const std::wstring_view file) -> xfsys::file;

	auto create(const std::u8string_view  directory, const std::u8string_view  file) -> xfsys::file;
	auto create(const std::u16string_view directory, const std::u16string_view file) -> xfsys::file;

	auto create_directory(const std::string_view  path, const bool create_if_no_exists = true) -> bool;
	auto create_directory(const std::wstring_view path, const bool create_if_no_exists = true) -> bool;

	auto create_directory(const std::u8string_view  path, const bool create_if_no_exists = true) -> bool;
	auto create_directory(const std::u16string_view path, const bool create_if_no_exists = true) -> bool;

	auto extname_check(const std::string_view  file, const std::string_view  ext) -> bool;
	auto extname_check(const std::wstring_view file, const std::wstring_view ext) -> bool;

	auto extname_check(const std::u8string_view  file, const std::u8string_view ext) -> bool;
	auto extname_check(const std::u16string_view file, const std::u8string_view ext) -> bool;

	auto is_file(const std::string_view  path) -> bool;
	auto is_file(const std::wstring_view path) -> bool;
	
	auto is_file(const std::u8string_view  path) -> bool;
	auto is_file(const std::u16string_view path) -> bool;

	auto is_directory(const std::string_view  path) -> bool;
	auto is_directory(const std::wstring_view path) -> bool;

	auto is_directory(const std::u8string_view  path) -> bool;
	auto is_directory(const std::u16string_view path) -> bool;

	auto is_exists(const std::string_view  path) -> bool;
	auto is_exists(const std::wstring_view path) -> bool;

	auto is_exists(const std::u8string_view  path) -> bool;
	auto is_exists(const std::u16string_view path) -> bool;
}