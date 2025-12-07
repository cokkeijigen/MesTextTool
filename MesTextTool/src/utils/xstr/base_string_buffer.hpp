#pragma once
#define _base_string_buffer_
#include <vector>
#include <ranges>

namespace utils::xstr 
{

	template <class T, class elem_t>
	concept valid_iterator_t = requires(T it) 
	{
		{ ++it } -> std::same_as<T&>;
		{ it++ } -> std::convertible_to<T>;
		{ it - it } -> std::convertible_to<size_t>;
		requires sizeof(decltype(*it)) == sizeof(elem_t);
	};

	template<class T, class elem_t, class object_t>
	concept union_convertible_of = (sizeof(T) == sizeof(elem_t) && std::convertible_to<T, elem_t>) || std::convertible_to<T, object_t>;

	template<class elem_t>
	class base_string_buffer 
	{

	public:

		using view_t   = std::basic_string_view<elem_t, std::char_traits<elem_t>>;
		using string_t = std::basic_string<elem_t, std::char_traits<elem_t>, std::allocator<elem_t>>;
		using vector_t = std::vector<elem_t>;

		static constexpr inline elem_t empty[] { static_cast<elem_t>( 0) };
		static constexpr inline auto npos      { view_t::npos };
		static constexpr inline auto unused    { static_cast<size_t>(-1) };

		class iterator 
		{
			elem_t* m_Ptr;
		public:
			iterator(elem_t* ptr) : m_Ptr(ptr) {}
			elem_t& operator* () const { return *m_Ptr; }
			iterator& operator++() { ++m_Ptr; return *this; }
			iterator& operator--() { --m_Ptr; return *this; }
			iterator  operator+=(size_t n) { m_Ptr += n; return *this; }
			iterator  operator-=(size_t n) { m_Ptr -= n; return *this; }
			iterator  operator++(int) { iterator tmp = *this; ++m_Ptr; return tmp; }
			iterator  operator--(int) { iterator tmp = *this; --m_Ptr; return tmp; }
			iterator  operator+(size_t n) { return iterator(m_Ptr + n); }
			iterator  operator-(size_t n) { return iterator(m_Ptr - n); }
			bool operator==(const iterator& other) const { return m_Ptr == other.m_Ptr; }
			bool operator!=(const iterator& other) const { return m_Ptr != other.m_Ptr; }
		};

	protected:

		mutable vector_t m_Buffer{};
		mutable size_t m_CharCount{};

		inline auto check(const size_t length) -> void;
		
	 	auto static convert_to_view(auto&& value) -> view_t;

	public:

		inline base_string_buffer() = default;

		inline ~base_string_buffer() = default;

		inline base_string_buffer(size_t size);

		inline base_string_buffer(view_t str);

		template <class iterator_t>
		requires xstr::valid_iterator_t<iterator_t, elem_t>
		auto write(iterator_t begin, iterator_t end) -> void;

		auto write_as_format(const elem_t* fmt, ...) -> void;

		auto write(const view_t str) -> void;

		auto write(elem_t elem) -> void;

		auto resize(size_t size) -> void;

		auto data() const noexcept -> elem_t*;

		auto view() const noexcept -> view_t;

		auto count() const noexcept -> size_t;

		auto size() const noexcept -> size_t;

		auto reset() -> void;

		auto clear() -> void;

		auto recount(size_t count = unused) const noexcept -> size_t;

		auto trim() -> void;

		auto split(xstr::union_convertible_of<elem_t, view_t> auto elem, size_t count = unused) -> std::vector<view_t>;

		auto split_of(std::initializer_list<view_t> strings, size_t count = unused) -> std::vector<view_t>;

		auto split_of(
			xstr::union_convertible_of<elem_t, view_t> auto one, 
			xstr::union_convertible_of<elem_t, view_t> auto ...more
		) -> std::vector<view_t>;
		
		auto split_of(size_t count, 
			xstr::union_convertible_of<elem_t, view_t> auto one,
			xstr::union_convertible_of<elem_t, view_t> auto ...more
		) -> std::vector<view_t>;

		auto replace(
			xstr::union_convertible_of<elem_t, view_t> auto o_str_or_char,
			xstr::union_convertible_of<elem_t, view_t> auto n_str_or_char,
			size_t offset = 0, size_t count = unused
		) -> void;

		auto remove(
			xstr::union_convertible_of<elem_t, view_t> auto str_or_char,
			size_t offset = 0, size_t count = unused
		) -> void;

		auto contrast(const elem_t* str, size_t offset, size_t count) -> bool;

		auto contrast(view_t str, size_t offset) -> bool;

		auto contrast(view_t str) -> bool;

		auto rfind(view_t str, size_t ofs = npos) -> size_t;

		auto find(view_t str, size_t ofs = 0) -> size_t;

		auto starts_with(view_t str) -> bool;

		auto ends_with(view_t str) -> bool;

		auto substr(size_t offset, size_t count = unused) const noexcept -> view_t;

		auto at(size_t index, bool begin = true) -> elem_t;

		auto to_upper() -> void;

		auto to_lower() -> void;

		auto begin() -> iterator;

		auto end() -> iterator;

		auto operator[](size_t index) -> elem_t;
	};

	template<class elem_t>
	inline base_string_buffer<elem_t>::base_string_buffer(size_t size)
	{
		this->m_Buffer.resize(size);
	}

	template<class elem_t>
	inline base_string_buffer<elem_t>::base_string_buffer(view_t str)
	{
		this->write(str.begin(), str.end());
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::check(const size_t length) -> void
	{
		size_t count { this->m_CharCount + length + 1 };
		if (count > this->m_Buffer.size())
		{
			size_t size{ (count + 1023) & ~1023 };
			this->m_Buffer.resize(size);
		}
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::convert_to_view(auto&& value) -> view_t
	{
		if constexpr (sizeof(decltype(value)) == sizeof(elem_t))
		{
			return view_t{ reinterpret_cast<const elem_t*>(&value), 1 };
		}
		else if constexpr (std::convertible_to<decltype(value), view_t>)
		{
			return view_t{ value };
		}
		else
		{
			static_assert(false, "Unsupported type conversion.");
		}
	}

	template<class elem_t>
	template<class iterator_t>
	requires xstr::valid_iterator_t<iterator_t, elem_t>
	auto base_string_buffer<elem_t>::write(iterator_t begin, iterator_t end) -> void
	{
		if (begin <= end)
		{
			auto length = static_cast<size_t>(end - begin);
			this->check(length);

			std::copy(begin, end, &this->m_Buffer[this->m_CharCount]);
			this->m_CharCount = this->m_CharCount + length;
			this->m_Buffer[this->m_CharCount] = empty[0];
		}
	}

	template<class elem_t>
	auto base_string_buffer<elem_t>::write_as_format(const elem_t* fmt, ...) -> void
	{
		constexpr auto elem_size = sizeof(elem_t);

		if constexpr (elem_size == sizeof(char))
		{
			va_list args{ nullptr };
			__crt_va_start(args, fmt);

			auto&& c_fmt  = reinterpret_cast<const char*>(fmt);
			size_t length = std::vsnprintf(nullptr, 0, c_fmt, args);
			this->check(length);

			auto&& buffer = &this->m_Buffer[this->m_CharCount];
			size_t size   = this->m_Buffer.size() - this->m_CharCount;
			std::vsnprintf(reinterpret_cast<char*>(buffer), size, c_fmt, args);

			this->m_CharCount = this->m_CharCount + length;
			this->m_Buffer[this->m_CharCount] = empty[0];

			__crt_va_end(args);
		}
		else if constexpr (elem_size == sizeof(wchar_t))
		{
			va_list args{ nullptr };
			__crt_va_start(args, fmt);

			auto&& c_fmt  = reinterpret_cast<const wchar_t*>(fmt);
			size_t length = std::vswprintf(nullptr, 0, c_fmt, args);
			this->check(length);

			auto&& buffer = &this->m_Buffer[this->m_CharCount];
			size_t size   = this->m_Buffer.size() - this->m_CharCount;
			std::vswprintf(reinterpret_cast<wchar_t*>(buffer), size, c_fmt, args);

			this->m_CharCount = this->m_CharCount + length;
			this->m_Buffer[this->m_CharCount] = empty[0];

			__crt_va_end(args);
		}
		else 
		{
			static_assert(false, "Format unsupported string element type.");
		}

	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::write(const view_t str) -> void
	{
		this->write(str.begin(), str.end());
	}

	template<class elem_t>
	auto base_string_buffer<elem_t>::write(const elem_t elem) -> void
	{
		this->check(1);
		this->m_Buffer[this->m_CharCount] = elem;
		this->m_CharCount++;
		this->m_Buffer[this->m_CharCount] = empty[0];
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::resize(size_t size) -> void
	{
		if (this->m_Buffer.size() != size + 1)
		{
			this->m_Buffer.resize(size + 1);
			this->m_Buffer[size] = static_cast<elem_t>(0);
			this->recount();
		}
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::data() const noexcept -> elem_t*
	{
		return this->m_Buffer.data();
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::view() const noexcept -> view_t
	{
		if (this->m_CharCount == 0) 
		{
			this->recount();
		}
		return view_t{ this->m_Buffer.data(), this->m_CharCount };
	}

	template<class elem_t>
	auto base_string_buffer<elem_t>::recount(const size_t count) const noexcept -> size_t
	{
		if (count == unused)
		{
			this->m_CharCount = 0;

			if (this->m_Buffer.size() == 0)
			{
				return 0;
			}

			const view_t view(this->m_Buffer.data());
			this->m_CharCount = view.size();
		}
		else if (this->m_Buffer.size() > count)
		{
			this->m_CharCount = count;
		}
		return this->m_CharCount;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::count() const noexcept -> size_t
	{
		return this->m_CharCount;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::size() const noexcept -> size_t
	{
		return this->m_Buffer.size();
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::reset() -> void
	{
		this->m_CharCount = 0;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::clear() -> void
	{
		this->m_Buffer.clear();
		this->m_CharCount = 0x00;
	}

	template<class elem_t>
	auto base_string_buffer<elem_t>::trim() -> void
	{
		if (this->m_CharCount > this->m_Buffer.size())
		{
			this->recount();
		}

		if (this->count() == 0) {
			return;
		}

		elem_t* data { this->m_Buffer.data() };
		elem_t* begin{ data };
		elem_t* end  { data + this->m_CharCount - 1 };

		while (begin <= end) {

			if (*begin == static_cast<elem_t>(0x20) ||
				*begin == static_cast<elem_t>(0x0A) ||
				*begin == static_cast<elem_t>(0x0D))
			{
				begin++;
			}
			else if (*end == static_cast<elem_t>(0x20) ||
				*end == static_cast<elem_t>(0x0A) ||
				*end == static_cast<elem_t>(0x0D))
			{
				end--;
			}
			else
			{
				break;
			}

		}

		if (end < begin) {
			this->m_Buffer[0] = empty[0];
			this->m_CharCount = 0;
			return;
		}

		size_t count((end - begin) + 1);
		size_t offset(begin - data);

		if (offset != 0) {
			auto beg{ this->m_Buffer.begin() + offset };
			auto end{ beg + count };
			std::copy(beg, end, this->m_Buffer.begin());
		}

		this->m_Buffer[count] = empty[0];
		this->m_CharCount = count;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::split(xstr::union_convertible_of<elem_t, view_t> auto elem, size_t count) -> std::vector<view_t>
	{
		std::vector<view_t> result{};
		view_t string { base_string_buffer<elem_t>::convert_to_view(elem) };
		view_t view   { this->m_Buffer.data(), this->m_CharCount };

		if (!string.empty() && !view.empty())
		{
			size_t length{ string.size() };
			for (size_t current{ 0 }, find{ 0 }; current < view.size();)
			{
				size_t pos = view.find(string, current);
				if (pos == npos)
				{
					auto count{ static_cast<size_t>(this->m_CharCount - current) };
					auto substr{ view.substr(current, count) };
					result.push_back(substr);
					break;
				}

				auto count{ static_cast<size_t>(pos - current) };
				auto substr{ view.substr(current, count) };
				result.push_back(substr);

				if (++find == count)
				{
					break;
				}
				current = pos + length;
			}
		}
		else
		{
			result.push_back(view);
		}
		return result;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::split_of(std::initializer_list<view_t> strings, size_t count) -> std::vector<view_t>
	{
		
		std::vector<view_t> result{};
		view_t view{ this->m_Buffer.data(), this->m_CharCount };
		
		if (strings.size() > 0)
		{
			for (size_t beg{ 0 }, end{ 0 }, find{ 0 }; end <= view.size(); ++end)
			{
				bool is_sep{ false };
				for (const view_t& sep : strings)
				{
					auto split{ view.substr(end, sep.size()) };
					if (split == sep)
					{
						is_sep = true;
						auto substr{ view.substr(beg, end - beg) };
						result.push_back(substr);

						end = end + sep.size();
						beg = end;
						find++;
						break;
					}
				}

				if (!is_sep && end == view.size())
				{
					auto substr{ view.substr(beg, end - beg) };
					result.push_back(substr);
				}
				else if(find == count)
				{
					auto substr{ view.substr(end, view.size() - end) };
					result.push_back(substr);
					break;
				}
			}
		}
		else 
		{
			result.push_back(view);
		}

		return result;
	}


	template<class elem_t>
	inline auto base_string_buffer<elem_t>::split_of(
		xstr::union_convertible_of<elem_t, view_t> auto one, 
		xstr::union_convertible_of<elem_t, view_t> auto ...more
	) -> std::vector<view_t>
	{
		std::initializer_list<view_t> list
		{ 
			base_string_buffer<elem_t>::convert_to_view(one),
			base_string_buffer<elem_t>::convert_to_view(more)...
		};
		return this->split_of(list);
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::split_of(size_t count,
		xstr::union_convertible_of<elem_t, view_t> auto one, 
		xstr::union_convertible_of<elem_t, view_t> auto ...more
	) -> std::vector<view_t>
	{
		std::initializer_list<view_t> list
		{
			base_string_buffer<elem_t>::convert_to_view(one),
			base_string_buffer<elem_t>::convert_to_view(more)...
		};
		return this->split_of(list, count);
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::replace(
		xstr::union_convertible_of<elem_t, view_t> auto o_str_or_char, 
		xstr::union_convertible_of<elem_t, view_t> auto n_str_or_char, 
		size_t offset, size_t count) -> void
	{
		auto o_string{ base_string_buffer<elem_t>::convert_to_view(o_str_or_char) };
		auto n_string{ base_string_buffer<elem_t>::convert_to_view(n_str_or_char) };

		if (this->m_CharCount == 0 || o_string.empty())
		{
			return;
		}

		if (offset >= this->m_CharCount)
		{
			return;
		}

		const size_t o_length{ o_string.size() };
		const size_t n_length{ n_string.size() };
		std::vector<size_t> finds{};
		{
			view_t view{ this->m_Buffer.data(), this->m_CharCount };

			for (size_t current{ offset }, find{ 0 }; current < view.size();)
			{
				size_t pos = view.find(o_string, current);
				if (pos == npos)
				{
					break;
				}

				finds.push_back(pos);
				if (++find == count)
				{
					break;
				}
				current = pos + o_length;
			}

			if (finds.empty())
			{
				return;
			}
		}

		if (n_length > o_length)
		{
			const size_t n_size{ (n_length - o_length) * finds.size() };
			const size_t n_count{ this->m_CharCount + n_size };
			if (n_count >= this->m_Buffer.size())
			{
				std::vector<elem_t> o_buffer{ std::move(this->m_Buffer) };
				this->check(n_count);
				this->reset();

				size_t current{ 0 };
				elem_t* data{ o_buffer.data() };
				for (const size_t& pos : finds)
				{
					elem_t* first{ data + current };
					elem_t* last{ data + pos };
					size_t  count{ static_cast<size_t>(last - first) };

					view_t substr{ first,  count };
					if (!substr.empty())
					{
						this->write(substr);
						current += substr.size();
					}
					this->write(n_string);
					current += o_length;
				}
			}
			else
			{
				size_t cur_pos{ n_count - 1 };
				size_t end_pos{ this->m_CharCount };
				elem_t* data{ this->m_Buffer.data() };
				for (const size_t& pos : std::ranges::reverse_view(finds))
				{
					elem_t* first{ data + pos + o_length };
					elem_t* last { data + end_pos };
					size_t  count{ static_cast<size_t>(last - first) };

					view_t substr{ first,  count };
					if (!substr.empty())
					{
						auto size{ substr.size() };
						end_pos -= size;
						cur_pos -= size;
						elem_t* dest{ data + cur_pos + 1 };
						std::copy(substr.begin(), substr.end(), dest);
					}

					end_pos -= o_length;
					cur_pos -= n_length;
					elem_t* dest{ data + cur_pos + 1 };
					std::copy(n_string.begin(), n_string.end(), dest);
				}
				this->m_CharCount = n_count;
			}
		}
		else if (n_length == o_length)
		{
			elem_t* data{ this->m_Buffer.data() };
			for (const size_t& pos : finds)
			{
				auto dest{ data + pos };
				std::copy(n_string.begin(), n_string.end(), dest);
			}
		}
		else
		{
			elem_t* data{ this->m_Buffer.data() };
			size_t diffs{ o_length - n_length };

			for (size_t i = 0, count{ 0 }, size{ finds.size() }; i < size; i++)
			{
				size_t cur{ finds[i] };
				auto find { reinterpret_cast<elem_t*>(data + cur - count) };
				std::copy(n_string.begin(), n_string.end(), find);

				auto str{ reinterpret_cast<elem_t*>(find + o_length + count) };
				auto len
				{
					i + 1 != size ?
					size_t{ finds[i + 1] - (cur + o_length) } :
					size_t{ this->m_CharCount - ((cur - count) + o_length + count) }
				};
				auto substr = view_t{ str, len };
				std::copy(substr.begin(), substr.end(), find + n_length);
				count = { count + diffs };
			}
			this->m_CharCount -= diffs * finds.size();
		}
		this->m_Buffer[this->m_CharCount] = empty[0];
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::remove(
		xstr::union_convertible_of<elem_t, view_t> auto str_or_char, 
		size_t offset, size_t count) -> void
	{
		auto string{ base_string_buffer<elem_t>::convert_to_view(str_or_char) };
		this->replace(string, view_t{ empty }, offset, count);
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::contrast(const elem_t* str, size_t offset, size_t count) -> bool
	{
		if (this->m_CharCount < count)
		{
			return false;
		}

		elem_t* data{ this->m_Buffer.data() + offset };
		for (size_t i = 0; i < count; i++)
		{
			if (data[i] != str[i])
			{
				return false;
			}
		}

		return true;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::contrast(view_t str, size_t offset) -> bool
	{
		return this->contrast(str.data(), offset, str.size());
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::contrast(view_t str) -> bool
	{
		return this->contrast(str.data(), 0, str.size());
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::rfind(view_t str, size_t ofs) -> size_t
	{
		size_t result = this->view().rfind(str, ofs);
		return result;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::find(view_t str, size_t ofs) -> size_t
	{
		size_t result = this->view().find(str, ofs);
		return result;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::starts_with(view_t str) -> bool
	{
		bool result = this->view().starts_with(str);
		return result;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::ends_with(view_t str) -> bool
	{
		bool result = this->view().ends_with(str);
		return result;
	}

	template<class elem_t>
	auto base_string_buffer<elem_t>::substr(size_t offset, size_t count) const noexcept -> view_t
	{
		if (offset > this->m_CharCount)
		{
			return view_t{};
		}
		
		const elem_t* data = this->m_Buffer.data() + offset;
		size_t free = this->m_CharCount - offset;

		if (count > free)
		{
			return view_t(data, free);
		}
		else {
			return view_t(data, count);
		}

	}
	
	template<class elem_t>
	inline auto base_string_buffer<elem_t>::begin() -> iterator
	{
		return iterator{ this->m_Buffer.data() };
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::end() -> iterator
	{
		return iterator{ this->m_Buffer.data() + this->m_CharCount };
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::at(size_t index, bool begin) -> elem_t
	{
		auto target { begin ? index : this->m_CharCount - 1 - index };

		if (target >= this->m_CharCount)
		{
			throw std::exception
			{
				"base_string_buffer::at(size_t, bool): "
				"index out of bounds!"
			};
		}

		elem_t result = this->m_Buffer[target];
		return result;
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::to_upper() -> void
	{
		for (elem_t& elem : *this)
		{
			if (elem >= static_cast<elem_t>('a') && elem <= static_cast<elem_t>('z'))
			{
				elem = elem - static_cast<elem_t>('a') + static_cast<elem_t>('A');
			}
		}
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::to_lower() -> void
	{
		for (elem_t& elem : *this)
		{
			if (elem >= static_cast<elem_t>('A') && elem <= static_cast<elem_t>('Z'))
			{
				elem = elem - static_cast<elem_t>('A') + static_cast<elem_t>('a');
			}
		}
	}

	template<class elem_t>
	inline auto base_string_buffer<elem_t>::operator[](size_t index) -> elem_t
	{
		if (index >= this->m_Buffer.size())
		{
			throw std::exception
			{
				"base_string_buffer::operator[](size_t): "
				"index out of bounds!" 
			};
		}

		elem_t result = this->m_Buffer[index];
		return result;
	}

	template<class derived_t, class elem_t>
	class base_xstring_buffer : public base_string_buffer<elem_t>
	{
		static constexpr inline auto unused
		{
			base_string_buffer<elem_t>::unused
		};

	protected:

		constexpr inline auto self() -> derived_t&
		{
			return *reinterpret_cast<derived_t*>(this);
		}

	public:

		using view_t = base_string_buffer<elem_t>::view_t;
		using string_t = base_string_buffer<elem_t>::string_t;
		using base_string_buffer<elem_t>::base_string_buffer;

		auto replace(
			xstr::union_convertible_of<elem_t, view_t> auto o_str_or_char,
			xstr::union_convertible_of<elem_t, view_t> auto n_str_or_char,
			size_t offset = 0, size_t count = unused
		) -> derived_t& 
		{
			base_string_buffer<elem_t>::replace(o_str_or_char, n_str_or_char, offset, count);
			return this->self();
		}

		auto remove(
			xstr::union_convertible_of<elem_t, view_t> auto str_or_char,
			size_t offset = 0, size_t count = unused
		) -> derived_t&
		{
			base_string_buffer<elem_t>::remove(str_or_char, offset, count);
			return this->self();
		}

		template<class char_t>
		requires (!std::is_same<elem_t, char_t>::value) && (sizeof(char_t) == sizeof(elem_t))
		inline auto replace(const char_t* o_string, const char_t* n_string, size_t offset = 0, size_t count = unused) -> derived_t&
		{
			auto o_str = reinterpret_cast<const elem_t*>(o_string);
			auto n_str = reinterpret_cast<const elem_t*>(n_string);
			base_string_buffer<elem_t>::replace(o_str, n_str, offset, count);
			return this->self();
		}

		template<class char_t>
		requires (!std::is_same<elem_t, char_t>::value) && (sizeof(char_t) == sizeof(elem_t))
		inline auto remove(const char_t* string, size_t offset = 0, size_t count = unused) -> derived_t&
		{
			base_string_buffer<elem_t>::remove(reinterpret_cast<const elem_t*>(string), offset, count);
			return this->self();
		}

		template<class char_t, class ...T>
		requires (sizeof(char_t) == sizeof(elem_t))
		inline auto write_as_format(const char_t* fmt, T&& ...args) -> derived_t&
		{
			base_string_buffer<elem_t>::write_as_format(reinterpret_cast<const elem_t*>(fmt), std::forward<T&&>(args)...);
			return this->self();
		}

		template<class ...T>
		auto write_as_format(const string_t& fmt, T&& ...args) -> derived_t&
		{
			const auto&& str = reinterpret_cast<const elem_t*>(fmt.data());
			base_string_buffer<elem_t>::write_as_format(str, std::forward<T&&>(args)...);
			return this->self();
		}

		template<class ...T>
		auto write_as_format(const string_t&& fmt, T&& ...args) -> derived_t&
		{
			const auto&& str = reinterpret_cast<const elem_t*>(fmt.data());
			base_string_buffer<elem_t>::write_as_format(str, std::forward<T&&>(args)...);
			return this->self();
		}

		template<class ...T>
		auto write_as_format(const view_t& fmt, T&& ...args) -> derived_t&
		{
			const auto&& str = string_t(fmt.begin(), fmt.end());
			return this->write_as_format(str, std::forward<T&&>(args)...);
		}

		template<class ...T>
		auto write_as_format(const view_t&& fmt, T&& ...args) -> derived_t&
		{
			const auto&& str = string_t(fmt.begin(), fmt.end());
			return this->write_as_format(str, std::forward<T&&>(args)...);
		}

		template <class iterator_t>
		inline auto write(iterator_t begin, iterator_t end) -> derived_t&
		{
			base_string_buffer<elem_t>::write(begin, end);
			return this->self();
		}

		inline auto write(const view_t str) -> derived_t&
		{
			base_string_buffer<elem_t>::write(str);
			return this->self();
		}

		inline auto write(const elem_t elem) -> derived_t&
		{
			base_string_buffer<elem_t>::write(elem);
			return this->self();
		}

		inline auto reset() -> derived_t&
		{
			base_string_buffer<elem_t>::reset();
			return this->self();
		}

		inline auto clear() -> derived_t&
		{
			base_string_buffer<elem_t>::clear();
			return this->self();
		}

		inline auto trim() -> derived_t&
		{
			base_string_buffer<elem_t>::trim();
			return this->self();
		}

		inline auto to_upper() -> derived_t& 
		{
			base_string_buffer<elem_t>::to_upper();
			return this->self();
		}

		inline auto to_lower() -> derived_t& 
		{
			base_string_buffer<elem_t>::to_lower();
			return this->self();
		}
	};

}