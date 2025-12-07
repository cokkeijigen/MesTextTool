#pragma once
#define _string_converter_

namespace utils::xstr
{
	
	auto encoding_convert(std::string_view intput, std::string& output, uint32_t current_code_page, uint32_t target_code_page) -> void;
	auto encoding_convert(std::string_view intput, uint32_t current_code_page, uint32_t target_code_page) -> std::string;

	auto encoding_convert(std::wstring_view input, std::string& output, uint32_t target_code_page) -> void;
	auto encoding_convert(std::wstring_view input, uint32_t target_code_page) -> std::string;

	auto convert_to_utf16(std::string_view intput, std::wstring& output, uint32_t current_code_page = 0) -> void;
	auto convert_to_utf16(std::string_view intput, uint32_t current_code_page = 0) -> std::wstring;

	auto convert_to_utf8(std::wstring_view input, std::string& output) -> void;
	auto convert_to_utf8(std::wstring_view input) -> std::string;

	namespace cvt
	{

		template<class T>
		concept string_t = std::is_same<T, std::string>::value || std::is_same<T, std::u8string>::value;
		
		template<class T>
		concept wstring_t = std::is_same<T, std::wstring>::value || std::is_same<T, std::u16string>::value;

		inline auto convert(std::string_view intput, std::string& output, uint32_t current_code_page, uint32_t target_code_page) -> void 
		{
			return xstr::encoding_convert(intput, output, current_code_page, target_code_page);
		}
		
		inline auto convert(std::string_view intput, uint32_t current_code_page, uint32_t target_code_page) -> std::string
		{
			return xstr::encoding_convert(intput, current_code_page, target_code_page);
		}

		inline auto convert(std::wstring_view input, std::string& output, uint32_t target_code_page) -> void 
		{
			return xstr::encoding_convert(input, output, target_code_page);
		}

		inline auto convert(std::wstring_view input, uint32_t target_code_page) -> std::string
		{
			return xstr::encoding_convert(input, target_code_page);
		}

		inline auto convert(std::u8string_view intput, std::string& output, uint32_t target_code_page) -> void
		{
			xstr::encoding_convert(*reinterpret_cast<std::string_view*>(&intput), output, 65001, target_code_page);
		}

		inline auto convert(std::u8string_view intput, uint32_t target_code_page) -> std::string
		{
			return xstr::encoding_convert(*reinterpret_cast<std::string_view*>(&intput), 65001, target_code_page);
		}

		inline auto to_utf16(std::string_view intput, wstring_t auto& output, uint32_t current_code_page = 0) -> void
		{
			xstr::convert_to_utf16(intput, *reinterpret_cast<std::wstring*>(&output), current_code_page);
		}

		template<class R = std::wstring>
		requires wstring_t<R>
		inline auto to_utf16(std::string_view intput, uint32_t current_code_page = 0) -> R
		{
			auto&& result{ xstr::convert_to_utf16(intput, current_code_page) };
			return *reinterpret_cast<R*>(&result);
		}

		template<class R = std::wstring>
		requires wstring_t<R>
		inline auto to_utf16(std::u8string_view intput) -> R
		{
			auto _input{ reinterpret_cast<std::string_view*>(&intput) };
			auto&& result{ xstr::convert_to_utf16(*_input, 65001) };
			return *reinterpret_cast<R*>(&result);
		}

		inline auto to_utf8(std::wstring_view input, string_t auto& output) -> void
		{
			return xstr::convert_to_utf8(input, *reinterpret_cast<std::string*>(&output));
		}

		template<class R = std::string>
		requires string_t<R>
		inline auto to_utf8(std::wstring_view input) -> R
		{
			auto&& result{ xstr::convert_to_utf8(input) };
			return *reinterpret_cast<R*>(&result);
		}

	}
}