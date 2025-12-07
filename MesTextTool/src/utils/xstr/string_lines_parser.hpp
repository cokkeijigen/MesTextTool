#pragma once
#include "base_string_buffer.hpp"
#include <functional>

namespace utils::xstr
{

	template<class elem_t>
	class string_lines_parser
	{
		inline static constexpr const elem_t LF{ static_cast<elem_t>('\n') };
		inline static constexpr const elem_t CR{ static_cast<elem_t>('\r') };

		inline static constexpr const elem_t CRLF[3]
		{
			static_cast<elem_t>('\r'),
			static_cast<elem_t>('\n'),
			static_cast<elem_t>('\0')
		};

		inline static constexpr const elem_t WHITESPACE[7]
		{
			static_cast<elem_t>(' ' ),
			static_cast<elem_t>('\n'),
			static_cast<elem_t>('\t'),
			static_cast<elem_t>('\v'),
			static_cast<elem_t>('\f'),
			static_cast<elem_t>('\r'),
			static_cast<elem_t>('\0')
		};

	public:

		using string_view_t = std::basic_string_view<elem_t, std::char_traits<elem_t>>;

		class iterator
		{
			friend string_lines_parser<elem_t>;

			const string_lines_parser<elem_t>* m_parser{};
			mutable size_t m_index, m_count;

			inline iterator(iterator&& other) noexcept;
			inline iterator(const string_lines_parser<elem_t>* parser, size_t index, size_t count) noexcept;

			inline auto operator=(iterator&& other) noexcept -> iterator&;

		public:

			inline auto operator*() const noexcept -> string_view_t;
			inline auto operator++() noexcept -> iterator&;
			inline auto operator==(const iterator& other) const noexcept -> bool;
			inline auto operator!=(const iterator& other) const noexcept -> bool;
		};

		inline string_lines_parser(const string_view_t string_view, const bool trim_need = false) noexcept;

		inline string_lines_parser(const base_string_buffer<elem_t>& buffer, const bool trim_need = false) noexcept;

		inline auto next() const noexcept -> iterator;
		inline auto begin() const noexcept -> iterator;
		inline auto end() const noexcept -> iterator;

		inline auto current_index() const noexcept -> size_t;
		inline auto set_index(const size_t index) noexcept -> void;

	protected:

		const bool m_trim_need;
		const string_view_t m_string_view;
		mutable  size_t m_current_index{};
	};

	template<class elem_t>
	using line_iterator = string_lines_parser<elem_t>;

	namespace line 
	{
		template<class elem_t>
		using iter = string_lines_parser<elem_t>;
	}

	template<class elem_t>
	inline string_lines_parser<elem_t>::iterator::iterator(const string_lines_parser<elem_t>* parser, 
		size_t index, size_t count) noexcept : m_parser{ parser }, m_index{ index }, m_count{ count }
	{
	}

	template<class elem_t>
	inline string_lines_parser<elem_t>::iterator::iterator(iterator&& other) noexcept
	{
		this->operator=(std::move(other));
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::iterator::operator=(iterator&& other) noexcept -> iterator&
	{
		if (this != &other)
		{
			this->m_parser = std::exchange(other.m_parser, nullptr);
			this->m_index  = std::exchange(other.m_index, 0);
			this->m_count  = std::exchange(other.m_count, 0);
		}
		return *this;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::iterator::operator*() const noexcept -> string_view_t
	{
		if (this->m_parser == nullptr)
		{
			return {};
		}

		if (this->m_parser->m_string_view.empty())
		{
			return {};
		}
		
		const auto length{ this->m_parser->m_string_view.size() };
		if (this->m_index >= length)
		{
			return {};
		}
		
		if (this->m_index + this->m_count > length)
		{
			return {};
		}

		auto line{ this->m_parser->m_string_view.substr(this->m_index, this->m_count) };
		if (this->m_parser->m_trim_need)
		{
			const auto beg{ line.find_first_not_of(string_lines_parser<elem_t>::WHITESPACE) };
			if (beg != std::u8string_view::npos)
			{
				line = line.substr(beg);
				const auto end{ line.find_last_not_of(string_lines_parser<elem_t>::WHITESPACE) };
				line = line.substr(0, end + 1);
			}
			else 
			{
				line = {};
			}
		}
		return line;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::iterator::operator++() noexcept -> iterator&
	{
		const auto original_index{ this->m_parser->m_current_index };
		this->m_parser->m_current_index = this->m_index + this->m_count;
		this->operator=(this->m_parser->next());
		this->m_parser->m_current_index = original_index;
		return *this;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::iterator::operator==(const iterator& other) const noexcept -> bool
	{
		return this->m_parser == other.m_parser && this->m_index == other.m_index && this->m_count == other.m_count;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::iterator::operator!=(const iterator& other) const noexcept -> bool
	{
		return this->m_parser != other.m_parser || this->m_index != other.m_index || this->m_count != other.m_count;
	}

	template<class elem_t>
	inline string_lines_parser<elem_t>::string_lines_parser(const string_view_t string_view, 
		const bool trim_need) noexcept : m_string_view{ string_view }, m_trim_need{ trim_need }
	{
	}

	template<class elem_t>
	inline string_lines_parser<elem_t>::string_lines_parser(const base_string_buffer<elem_t>& buffer,
		const bool trim_need) noexcept: m_string_view{ buffer.view() }, m_trim_need{ trim_need }
	{
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::next() const noexcept -> iterator
	{
		if (this->m_string_view.empty())
		{
			return this->end();
		}

		if (this->m_current_index >= this->m_string_view.size())
		{
			return this->end();
		}

		auto next_index{ this->m_string_view.find_first_of(string_lines_parser<elem_t>::CRLF, this->m_current_index) };

		if (next_index == string_lines_parser<elem_t>::string_view_t::npos)
		{
			next_index = this->m_string_view.size();
		}
		else if (this->m_string_view[next_index++] == string_lines_parser<elem_t>::CR
			&& next_index < this->m_string_view.size())
		{
			if (this->m_string_view[next_index] == string_lines_parser<elem_t>::LF)
			{
				++next_index;
			}
		}

		iterator result{ this, this->m_current_index, next_index - this->m_current_index };
		this->m_current_index = next_index;

		return result;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::begin() const noexcept -> iterator
	{
		const auto index{ this->m_current_index };
		this->m_current_index = 0;
		
		auto result{ this->next() };
		this->m_current_index = index;

		return result;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::end() const noexcept -> iterator
	{
		return iterator{ this, this->m_string_view.size(), 0 };
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::current_index() const noexcept -> size_t
	{
		return this->m_current_index;
	}

	template<class elem_t>
	inline auto string_lines_parser<elem_t>::set_index(const size_t index) noexcept -> void
	{
		this->m_current_index = index;
	}

}