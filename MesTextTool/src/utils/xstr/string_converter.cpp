#include <iostream>
#include <vector>
#include <windows.h>
#include "string_converter.hpp"

namespace utils::xstr
{

	namespace unsafe
	{
		inline constexpr int string_type{ 1 };
		inline constexpr int vector_type{ 2 };

		auto convert_to_utf16(std::string_view buffer, void* out, int out_type, uint32_t cdpg) -> bool
		{
			union { void* raw; std::wstring* wstring; std::vector<wchar_t>* vector; }_outbuf{ out };

			if (buffer.empty() || out == nullptr)
			{
				return false;
			}

			const auto _count{ ::MultiByteToWideChar(cdpg, 0, buffer.data(), buffer.size(), nullptr, 0) };
			if (_count > 0)
			{
				if (out_type == unsafe::string_type)
				{
					_outbuf.wstring->assign(L"");
					_outbuf.wstring->resize(_count);
				}
				else if (out_type == unsafe::string_type)
				{
					_outbuf.vector->resize(_count + 1);
				}
				else
				{
					return false;
				}
				auto outbuf{ out_type == 1 ? _outbuf.wstring->data() : _outbuf.vector->data() };
				::MultiByteToWideChar(cdpg, 0, buffer.data(), buffer.size(), outbuf, _count);
				outbuf[static_cast<size_t>(_count)] = L'\0';
			}

			return true;
		}

		auto convert_to_string(std::wstring_view buffer, void* out, int out_type, uint32_t cdpg) -> bool
		{
			union { void* raw; std::string* string; std::vector<char>* vector; }_outbuf{ out };
			if (buffer.empty() || out == nullptr)
			{
				return false;
			}

			const auto _count{ ::WideCharToMultiByte(cdpg, 0, buffer.data(), buffer.size(), nullptr, 0, NULL, NULL) };
			if (_count > 0)
			{
				if (out_type == unsafe::string_type)
				{
					_outbuf.string->assign("");
					_outbuf.string->resize(_count);
				}
				else if (out_type == unsafe::vector_type)
				{
					_outbuf.vector->resize(_count + 1);
				}
				else
				{
					return false;
				}
				auto outbuf{ out_type == 1 ? _outbuf.string->data() : _outbuf.vector->data() };
				::WideCharToMultiByte(cdpg, 0, buffer.data(), buffer.size(), outbuf, _count, NULL, NULL);
				outbuf[static_cast<size_t>(_count)] = '\0';
			}
			return true;
		}

		// [buffer: char* or std::vector<char>*] [out: std::string or std::vector<char>]
		auto convert_encoding(void* buffer, int buffer_type, void* out, int out_type, uint32_t o_cdpg, uint32_t n_cdpg) -> bool 
		{
			if (o_cdpg == n_cdpg || buffer == nullptr || out == nullptr)
			{
				return false;
			}
			
			std::wstring _u16str{};
			{
				char* _buffer{};
				if (buffer_type == unsafe::string_type)
				{
					_buffer = reinterpret_cast<char*>(buffer);
				}
				else if (buffer_type == unsafe::vector_type)
				{
					_buffer = reinterpret_cast<std::vector<char>*>(buffer)->data();
				}

				if (_buffer == nullptr)
				{
					return false;
				}

				const bool result{ convert_to_utf16(_buffer, &_u16str, unsafe::string_type, o_cdpg) };
				if (!result)
				{
					return false;
				}
			}

			if (_u16str.empty())
			{
				return false;
			}

			const bool result{ convert_to_string(_u16str, out, out_type, n_cdpg) };
			return result;
		}

		// [buffer -> std::vector<char>*] [out -> std::vector<char>*]
		auto convert_encoding(void* buffer, void* out, uint32_t o_cdpg, uint32_t n_cdpg) -> bool
		{
			auto _buffer{ reinterpret_cast<std::vector<char>*>(buffer) };
			auto _outbuf{ reinterpret_cast<std::vector<char>*>(out != nullptr && buffer != out ? out : buffer) };
			return convert_encoding(_buffer, unsafe::vector_type, _outbuf, unsafe::vector_type, o_cdpg, n_cdpg);
		}
	}

	auto encoding_convert(std::string_view input, std::string& output, uint32_t current_code_page, uint32_t target_code_page) -> void 
	{
		auto buffer{ const_cast<char*>(input.data()) };
		unsafe::convert_encoding(buffer, unsafe::string_type, &output, unsafe::string_type, current_code_page, target_code_page);
	}

	auto encoding_convert(std::string_view input, uint32_t current_code_page, uint32_t target_code_page) -> std::string
	{
		std::string result{};
		xstr::encoding_convert(input, result, current_code_page, target_code_page);
		return result;
	}

	auto encoding_convert(std::wstring_view input, std::string& output, uint32_t target_code_page) -> void
	{
		unsafe::convert_to_string(input, &output, unsafe::string_type, target_code_page);
	}

	auto encoding_convert(std::wstring_view input, uint32_t target_code_page) -> std::string
	{
		std::string result{};
		xstr::encoding_convert(input, result, target_code_page);
		return result;
	}

	auto convert_to_utf16(std::string_view input, std::wstring& output, uint32_t current_code_page) -> void 
	{
		unsafe::convert_to_utf16(input, &output, unsafe::string_type, current_code_page);
	}

	auto convert_to_utf16(std::string_view input, uint32_t current_code_page) -> std::wstring
	{
		std::wstring result{};
		xstr::convert_to_utf16(input, result, current_code_page);
		return result;
	}

	auto convert_to_utf8(std::wstring_view input, std::string& output) -> void 
	{
		unsafe::convert_to_string(input, &output, unsafe::string_type, CP_UTF8);
	}

	auto convert_to_utf8(std::wstring_view input) -> std::string 
	{
		std::string result{};
		unsafe::convert_to_string(input, &result, unsafe::string_type, CP_UTF8);
		return result;
	}

}