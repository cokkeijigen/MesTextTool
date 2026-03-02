#pragma once
#define _xtsr_
#include <optional>
#include <charconv>
#include <ranges>
#include <xstrbuff.hpp>
#include <string_converter.hpp>
#include <string_lines_parser.hpp>
#include <string_builder.hpp>

namespace xstr
{
	using namespace utils::xstr;

	template<class char_type>
	struct view : public std::basic_string_view<char_type>
	{
		using char_t = char_type;
		using view_t = std::basic_string_view<char_type>;
		using std::basic_string_view<char_t>::basic_string_view;
		using std::basic_string_view<char_t>::operator=;

		constexpr view(view_t v) noexcept : view_t(v) {}

		template<class traits, class alloc>
		constexpr view(const std::basic_string<char_type, traits, alloc>& s) 
			noexcept: view_t(s) {}
		
		static constexpr inline auto unused{ static_cast<size_t>(-1) };

		auto split(xstr::union_convertible_of<char_type, view_t> auto elem,
			size_t count = unused) const noexcept -> std::vector<view_t>;

		auto split_of(std::initializer_list<view_t> strings, 
			size_t count = unused) const noexcept -> std::vector<view_t>;

		auto split_of(
			xstr::union_convertible_of<char_type, view_t> auto one,
			xstr::union_convertible_of<char_type, view_t> auto ...more
		) const noexcept -> std::vector<view_t>;

		auto split_of(size_t count,
			xstr::union_convertible_of<char_type, view_t> auto one,
			xstr::union_convertible_of<char_type, view_t> auto ...more
		) const noexcept -> std::vector<view_t>;
	};

	template<class char_type>
	inline auto view<char_type>::split(xstr::union_convertible_of<char_type, view_t> auto elem,
		size_t count) const noexcept -> std::vector<view_t>
	{
		std::vector<view_t> result{};
		const view_t string{ xstr::convert_to_view<char_type>(elem)};
		
		if (!string.empty() && !this->empty()) 
		{
			for (size_t current{ 0 }, find{ 0 }; current < this->size();)
			{
				size_t pos = this->find(string, current);
				if (pos == std::string_view::npos)
				{
					auto length{ static_cast<size_t>(this->size() - current) };
					auto substr{ this->substr(current, length) };
					result.push_back(substr);
					break;
				}

				auto length{ static_cast<size_t>(pos - current) };
				auto substr{ this->substr(current, length) };
				result.push_back(substr);

				if (++find == count)
				{
					break;
				}
				current = pos + string.size();
			}
		}
		else
		{
			result.push_back(*this);
		}

		return result;
	}

	template<class char_type>
	inline auto view<char_type>::split_of(std::initializer_list<view_t> strings, 
		size_t count) const noexcept -> std::vector<view_t>
	{
		std::vector<view_t> result{};

		if (strings.size() > 0) 
		{
			for (size_t beg{ 0 }, end{ 0 }, find{ 0 }; end <= this->size(); ++end) 
			{
				bool is_sep{ false };
				for (const view_t& sep : strings)
				{
					auto split{ this->substr(end, sep.size()) };
					if (split == sep) 
					{
						is_sep = true;
						auto substr{ this->substr(beg, end - beg) };
						result.push_back(substr);
						end = end + sep.size();
						beg = end;
						find++;
						break;
					}
				}
				if (!is_sep && end == this->size())
				{
					auto substr{ this->substr(beg, end - beg) };
					result.push_back(substr);
				}
				else if (find == count)
				{
					auto substr{ this->substr(end, this->size() - end) };
					result.push_back(substr);
					break;
				}
			}
		}
		else
		{
			result.push_back(*this);
		}
		return result;
	}

	template<class char_type>
	inline auto view<char_type>::split_of(
		xstr::union_convertible_of<char_type, view_t> auto one, 
		xstr::union_convertible_of<char_type, view_t> auto ...more
	) const noexcept -> std::vector<view_t>
	{
		std::initializer_list<view_t> list
		{
			xstr::convert_to_view<char_type>(one),
			xstr::convert_to_view<char_type>(more)...
		};
		return this->split_of(list);
	}

	template<class char_type>
	inline auto view<char_type>::split_of(size_t count, 
		xstr::union_convertible_of<char_type, view_t> auto one,
		xstr::union_convertible_of<char_type, view_t> auto ...more
	) const noexcept -> std::vector<view_t>
	{
		std::initializer_list<view_t> list
		{
			xstr::convert_to_view<char_type>(one),
			xstr::convert_to_view<char_type>(more)...
		};
		return this->split_of(list, count);
	}

	template<class char_type>
	view(const char_type*) -> view<char_type>;
	
	template<class char_type, class traits, class alloc>
	view(const std::basic_string<char_type, traits, alloc>&) -> view<char_type>;

	template<class char_type>
	struct strs : public string_builder<char_type>
	{
		using string_builder<char_type>::string_builder;

		template<class... V>
		requires (std::convertible_to<V, std::basic_string_view<char_type>> && ...)
		inline strs(V&&... args) noexcept : string_builder<char_type>(std::forward<V>(args)...) {}
	};

	template<class first, class... rest>
	strs(first&&, rest&&...) -> strs<char_type_v<first>>;

	template<class char_type>
	struct str : public std::basic_string<char_type>
	{
		using char_t = char_type;
		using std::basic_string<char_t>::basic_string;

		template<class ...T>
		requires (std::convertible_to<T, std::basic_string_view<char_t>> && ...)
		inline str(T&& ... args) noexcept
		{
			std::initializer_list strs{ std::basic_string_view<char_t>{args}... };
			this->assign_range(strs | std::views::join);
		}
	};

	template<class first, class... rest>
	str(first&&, rest&&...) -> str<char_type_v<first>>;

	template<class ...T, class char_t = char_type_v<std::common_type_t<T...>>>
	requires (std::convertible_to<T, std::basic_string_view<char_t>> && ...)
	inline auto join(T&& ... args) -> std::basic_string<char_t>
	{
		std::initializer_list strs{ std::basic_string_view<char_t>{args}... };
		return { strs | std::views::join | std::ranges::to<std::basic_string<char_t>>() };
	}
	
	inline auto trim(const std::string_view string) -> std::string_view 
	{
		constexpr const char WHITESPACE[]{" \n\t\v\f\r"};

		std::string_view result{ string };
		const auto beg{ result.find_first_not_of(WHITESPACE) };
		if (beg != std::string_view::npos)
		{
			result = result.substr(beg);
			const auto end{ result.find_last_not_of(WHITESPACE) };
			result = result.substr(0, end + 1);
		}
		else
		{
			result = {};
		}

		return result;
	}
	
	inline auto trim(const std::wstring_view string) -> std::wstring_view 
	{
		constexpr const wchar_t WHITESPACE[]{L" \n\t\v\f\r"};

		std::wstring_view result{ string };
		const auto beg{ result.find_first_not_of(WHITESPACE) };
		if (beg != std::wstring_view::npos)
		{
			result = result.substr(beg);
			const auto end{ result.find_last_not_of(WHITESPACE) };
			result = result.substr(0, end + 1);
		}
		else
		{
			result = {};
		}

		return result;
	}

	inline auto trim(const std::u8string_view string) -> std::u8string_view
	{
		auto result{ trim(*reinterpret_cast<const std::string_view*>(&string)) };
		return *reinterpret_cast<std::u8string_view*>(&result);
	}

	inline auto trim(const std::u16string_view string) -> std::u16string_view
	{
		auto result{ trim(*reinterpret_cast<const std::wstring_view*>(&string)) };
		return *reinterpret_cast<std::u16string_view*>(&result);
	}

	template<class T = int64_t>
	requires std::is_integral<T>::value
	inline auto to_integer(std::string_view str, int radix = 10) -> std::optional<T>
	{
		bool is_negative{};
		auto beg{ str.data() };
		auto end{ beg + str.size() };

		if (*beg == '-') 
		{
			is_negative = true;
			++beg;
		}
		else if (*beg == '+') 
		{
			is_negative = false;
			++beg;
		}

		if (str.size() >= 2 && beg[0] == '0')
		{
			if (beg[1] == 'x' || beg[1] == 'X')
			{
				beg += 2;
				radix = 16;
			}
			else if (beg[1] == 'b' || beg[1] == 'B')
			{
				beg += 2;
				radix = 2;
			}
		}

		uint64_t number{};
		std::from_chars_result result
		{
			std::from_chars(beg, end, number, radix)
		};

		if (result.ec == std::errc())
		{
			if constexpr (std::is_signed_v<T>)
			{
				if (is_negative) 
				{
					number = static_cast<uint64_t>(-(static_cast<int64_t>(number)));
				}
			}
			return static_cast<T>(number);
		}
		else
		{
			return std::nullopt;
		}
	}

	template<class T = int64_t>
	requires std::is_integral<T>::value
	inline auto to_integer(std::wstring_view str, int radix = 10) -> std::optional<T> 
	{
		std::string number{};
		number.reserve(str.size());

		for (const wchar_t wc : str)
		{
			if (wc > 127) 
			{
				return std::nullopt;
			}

			const bool is_numeric_char
			{
				(wc >= L'0' && wc <= L'9') ||
				(wc >= L'a' && wc <= L'z') ||
				(wc >= L'A' && wc <= L'Z') ||
				 wc == L'+' || wc == L'-'
			};

			if (!is_numeric_char)
			{
				return std::nullopt;
			}

			number.push_back(static_cast<char>(wc));
		}

		return xstr::to_integer<T>(number, radix);
	}

}

