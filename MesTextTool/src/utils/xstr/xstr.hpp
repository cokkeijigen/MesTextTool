#pragma once
#define _xtsr_
#include <optional>
#include <charconv>
#include <ranges>
#include "xstrbuff.hpp"
#include "string_converter.hpp"
#include "string_lines_parser.hpp"

namespace xstr 
{
	using namespace utils::xstr;

	using buffer_x = string_buffer;
	using buffer_w = string_buffer;

	template<class ...T, class char_t = decltype(std::declval<std::tuple_element_t<0, std::tuple<T...>>>()[0])>
	requires (std::convertible_to<T, std::basic_string_view<std::decay_t<char_t>>> && ...)
	inline auto join(T&& ... args) -> std::basic_string<std::decay_t<char_t>>
	{
		std::initializer_list strs{ std::basic_string_view<std::decay_t<char_t>>{args}... };
		return { strs | std::views::join | std::ranges::to<std::basic_string<std::decay_t<char_t>>>() };
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
		T number{};
		auto beg{ str.data() };
		auto end{ beg + str.size() };
		std::from_chars_result result
		{
			std::from_chars(beg, end, number, radix)
		};

		if (result.ec == std::errc())
		{
			return { number };
		}
		else
		{
			return { std::nullopt };
		}
	}

	template<class T = int64_t>
	requires std::is_integral<T>::value
	inline auto to_integer(std::wstring_view str, int radix = 10) -> std::optional<T> 
	{
		return to_integer<T>(xstr::cvt::to_utf8(str), radix);
	}
}

