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
	static auto is_space(char_type chr) -> bool 
	{
		return bool
		{
			chr == static_cast<char_type>( ' ') ||
			chr == static_cast<char_type>('\n') ||
			chr == static_cast<char_type>('\t') ||
			chr == static_cast<char_type>('\v') ||
			chr == static_cast<char_type>('\f') ||
			chr == static_cast<char_type>('\r')
		};
	}

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
			size_t count = unused) const noexcept -> std::vector<view<char_type>>;

		auto split_of(std::initializer_list<view_t> strings, 
			size_t count = unused) const noexcept -> std::vector<view<char_type>>;

		auto split_of(
			xstr::union_convertible_of<char_type, view_t> auto one,
			xstr::union_convertible_of<char_type, view_t> auto ...more
		) const noexcept -> std::vector<view<char_type>>;

		auto split_of(size_t count,
			xstr::union_convertible_of<char_type, view_t> auto one,
			xstr::union_convertible_of<char_type, view_t> auto ...more
		) const noexcept -> std::vector<view<char_type>>;

		inline auto trim() noexcept -> view<char_type>&;
	};

	template<class char_type>
	inline auto view<char_type>::split(xstr::union_convertible_of<char_type, view_t> auto elem,
		size_t count) const noexcept -> std::vector<view<char_type>>
	{
		std::vector<view<char_type>> result{};
		const view_t string{ xstr::convert_to_view<char_type>(elem)};
		
		if (!string.empty() && !this->empty()) 
		{
			for (size_t current{ 0 }, found{ 0 }; current < this->size();)
			{
				const size_t pos{ this->find(string, current) };
				if (pos == std::basic_string_view<char_type>::npos)
				{
					auto length{ static_cast<size_t>(this->size() - current) };
					auto substr{ this->substr(current, length) };
					result.push_back(substr);
					break;
				}

				auto length{ static_cast<size_t>(pos - current) };
				auto substr{ this->substr(current, length) };
				result.push_back(substr);

				if (++found == count)
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
		size_t count) const noexcept -> std::vector<view<char_type>>
	{
		std::vector<view<char_type>> result{};

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
	) const noexcept -> std::vector<view<char_type>>
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
	) const noexcept -> std::vector<view<char_type>>
	{
		std::initializer_list<view_t> list
		{
			xstr::convert_to_view<char_type>(one),
			xstr::convert_to_view<char_type>(more)...
		};
		return this->split_of(list, count);
	}

	template<class char_type>
	inline auto view<char_type>::trim() noexcept -> view<char_type>&
	{

		auto begin{ this->begin() }, end{ this->end() - 1 };
		
		bool left_active { true };
		bool right_active{ true };
		while (begin <= end && (left_active || right_active))
		{
			if (left_active)
			{
				if (xstr::is_space<char_type>(*begin))
				{
					begin++;
				}
				else
				{
					left_active = false;
				}
			}

			if (right_active)
			{
				if (begin <= end && xstr::is_space<char_type>(*end))
				{
					end--;
				}
				else
				{
					right_active = false;
				}
			}
		}

		if (end < begin)
		{
			*this = {};
		}
		else 
		{
			*this = view<char_type>{ begin, end + 1 };
		}
		return *this;
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

	template<class char_type>
	inline auto trim(std::basic_string_view<char_type> string_view) -> std::basic_string_view<char_type>
	{
		return xstr::view<char_type>(std::move(string_view)).trim();
	}

	template<class char_type>
	inline auto trim(const std::basic_string<char_type>& string) -> std::basic_string_view<char_type>
	{
		return xstr::view<char_type>(std::move(string)).trim();
	}

	template<class char_type>
	inline auto trim(std::basic_string<char_type>&& string) -> std::basic_string_view<char_type>
	{
		string.assign(xstr::view<char_type>(string).trim());
		return std::move(string);
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

	template<class T = int64_t>
	requires std::is_integral<T>::value
	inline auto to_integer(std::u8string_view str, int radix = 10) -> std::optional<T> 
	{
		return xstr::to_integer<T>(*reinterpret_cast<std::string_view*>(&str), radix);
	}

	template<class T = int64_t>
	requires std::is_integral<T>::value
	inline auto to_integer(std::u16string_view str, int radix = 10) -> std::optional<T> 
	{
		return xstr::to_integer<T>(*reinterpret_cast<std::wstring_view*>(&str), radix);
	}
}

