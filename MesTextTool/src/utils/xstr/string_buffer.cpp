#include <iostream>
#include <windows.h>
#include "string_buffer.hpp"

namespace utils::xstr 
{

	namespace unsafe 
	{
		// string_converter.cpp
		inline constexpr int string_type{ 1 };
		inline constexpr int vector_type{ 2 };
		extern auto convert_to_utf16(std::string_view buffer, void* out, int out_type, uint32_t cdpg) -> bool;
		extern auto convert_to_string(std::wstring_view buffer, void* out, int out_type, uint32_t cdpg) -> bool;
		extern auto convert_encoding(void* buffer, void* out, uint32_t o_cdpg, uint32_t n_cdpg) -> bool;
	}

	auto string_buffer::wstring(uint32_t cdpg) const noexcept -> std::wstring
	{
		std::wstring _u16str{};
		if (this->m_CharCount > 0 || this->recount() > 0)
		{
			unsafe::convert_to_utf16(this->m_Buffer.data(), &_u16str, unsafe::string_type, cdpg);
		}
		return _u16str;
	}

	auto string_buffer::u8string(uint32_t cdpg) const noexcept -> std::u8string
	{
		std::u8string _u8str{};
		if (this->m_CharCount > 0 || this->recount() > 0)
		{
			std::wstring _u16str{};
			unsafe::convert_to_utf16(this->m_Buffer.data(), &_u16str, unsafe::string_type, cdpg);
			if (!_u16str.empty())
			{
				unsafe::convert_to_string(_u16str, &_u8str, unsafe::string_type, CP_UTF8);
			}
		}
		return _u8str;
	}

	auto string_buffer::u16string(uint32_t cdpg) const noexcept -> std::u16string
	{
		std::u16string _u16str{};
		if (this->m_CharCount > 0 || this->recount() > 0)
		{
			unsafe::convert_to_utf16(this->m_Buffer.data(), &_u16str, unsafe::string_type, cdpg);
		}
		return _u16str;
	}

	auto string_buffer::convert_encoding(uint32_t o_cdpg, uint32_t n_cdpg) -> string_buffer& 
	{
		if (this->m_CharCount > 0 || this->recount() > 0)
		{
			auto result{ unsafe::convert_encoding(&this->m_Buffer, nullptr, o_cdpg, n_cdpg) };
			if (result)
			{
				this->recount();
			}
		}
		return *this;
	}

	auto string_buffer::convert_to_utf8(uint32_t cdpg) -> string_buffer&
	{
		if (CP_UTF8 != cdpg)
		{
			return this->convert_encoding(cdpg, CP_UTF8);
		}
		return *this;
	}

	auto string_buffer::u8string_buffer(uint32_t o_cdpg) const noexcept -> xstr::u8string_buffer
	{
		if (this->m_CharCount == 0 || this->recount() == 0)
		{
			return {};
		}

		if (CP_UTF8 == o_cdpg)
		{
			xstr::u8string_buffer::view_t view
			{ 
				reinterpret_cast<const char8_t*>(this->data()),  
				this->m_CharCount
			};
			return xstr::u8string_buffer{ view };
		}
		else 
		{
			xstr::u8string_buffer buffer{};
			unsafe::convert_encoding(&this->m_Buffer, &buffer.m_Buffer, o_cdpg, CP_UTF8);
			buffer.recount();
			return buffer;
		}
	}

	auto u8string_buffer::string(uint32_t to_codepage) const noexcept -> std::string
	{
		if(this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		if (to_codepage == CP_UTF8)
		{
			auto u8str{ this->u8string() };
			return *reinterpret_cast<std::string*>(&u8str);
		}

		std::string result{};
		{
			std::wstring temp{};
			unsafe::convert_to_utf16(result, &temp, unsafe::string_type, CP_UTF8);
			if (!temp.empty())
			{
				unsafe::convert_to_string(temp, &result, unsafe::string_type, to_codepage);
			}
		}
		return result;
	}

	auto u8string_buffer::wstring() const noexcept -> std::wstring
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		std::wstring result{};
		{
			const auto data{ reinterpret_cast<char*>(this->m_Buffer.data()) };
			unsafe::convert_to_utf16({ data, this->m_CharCount }, &result, unsafe::string_type, CP_UTF8);
		}

		return result;
	}

	auto u8string_buffer::u16string() const noexcept -> std::u16string
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		std::u16string result{};
		{
			const auto data{ reinterpret_cast<char*>(this->m_Buffer.data()) };
			unsafe::convert_to_utf16({ data, this->m_CharCount }, &result, unsafe::string_type, CP_UTF8);
		}

		return result;
	}

	auto u8string_buffer::string_buffer(uint32_t to_codepage) const noexcept -> xstr::string_buffer
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		if (to_codepage == CP_UTF8)
		{
			const auto data{ reinterpret_cast<char*>(this->m_Buffer.data()) };
			return xstr::string_buffer::view_t{ data, this->m_CharCount  };
		}

		xstr::string_buffer result{};
		{
			unsafe::convert_encoding(&this->m_Buffer, &result.m_Buffer, CP_UTF8, to_codepage);
			result.recount();
		}

		return result;
	}

	auto u8string_buffer::wstring_buffer() const noexcept -> xstr::wstring_buffer
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		xstr::wstring_buffer result{};
		{
			const auto data{ reinterpret_cast<char*>(this->m_Buffer.data()) };
			std::string_view buffer{ data, this->m_CharCount };
			unsafe::convert_to_utf16(buffer, &result.m_Buffer, unsafe::vector_type, CP_UTF8);
			result.recount();
		}
		return result;
	}

	auto u8string_buffer::u16string_buffer() const noexcept -> xstr::u16string_buffer
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}

		xstr::u16string_buffer result{};
		{
			const auto data{ reinterpret_cast<char*>(this->m_Buffer.data()) };
			std::string_view buffer{ data, this->m_CharCount };
			unsafe::convert_to_utf16(buffer, &result.m_Buffer, unsafe::vector_type, CP_UTF8);
			result.recount();
		}
		return result;
	}

	auto string_buffer::wstring_buffer(uint32_t cdpg) const noexcept -> xstr::wstring_buffer
	{
		xstr::wstring_buffer result{};
		{
			unsafe::convert_to_utf16(this->view(), &result.m_Buffer, unsafe::vector_type, cdpg);
			result.recount();
		}
		return result;
	}

	auto string_buffer::u16string_buffer(uint32_t cdpg) const noexcept -> xstr::u16string_buffer
	{
		xstr::u16string_buffer result{};
		{
			unsafe::convert_to_utf16(this->view(), &result.m_Buffer, unsafe::vector_type, cdpg);
			result.recount();
		}
		return result;
	}

	auto wstring_buffer::string(uint32_t to_codepage) const noexcept -> std::string
	{
		std::string result{};
		unsafe::convert_to_string(this->view(), &result, unsafe::string_type, to_codepage);
		return result;
	}

	auto wstring_buffer::string_buffer(uint32_t to_codepage) const noexcept -> xstr::string_buffer
	{
		xstr::string_buffer result{};
		{
			unsafe::convert_to_string(this->view(), &result.m_Buffer, unsafe::vector_type, to_codepage);
			result.recount();
		}
		return result;
	}

	auto wstring_buffer::u8string_buffer() const noexcept -> xstr::u8string_buffer
	{
		xstr::u8string_buffer result{};
		{
			unsafe::convert_to_string(this->view(), &result.m_Buffer, unsafe::vector_type, CP_UTF8);
			result.recount();
		}
		return result;
	}

	auto wstring_buffer::u16string_buffer() const noexcept -> xstr::u16string_buffer
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}
		const auto data{ reinterpret_cast<char16_t*>(this->m_Buffer.data()) };
		return xstr::u16string_buffer::view_t{ data, this->m_CharCount };
	}


	auto u16string_buffer::wstring_buffer() const noexcept -> xstr::wstring_buffer
	{
		if (this->m_CharCount == 0 && this->recount() == 0)
		{
			return {};
		}
		const auto data{ reinterpret_cast<wchar_t*>(this->m_Buffer.data()) };
		return xstr::wstring_buffer::view_t{ data, this->m_CharCount };
	}

};