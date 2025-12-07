#pragma once
#include <base_string_buffer.hpp>
#define _string_buffer_

namespace utils::xstr 
{

	class string_buffer;
	class wstring_buffer;
	class u8string_buffer;
	class u16string_buffer;

	class string_buffer : public base_xstring_buffer<string_buffer, char>
	{
		friend wstring_buffer;
		friend u8string_buffer;
		friend u16string_buffer;

	public:
		using base_xstring_buffer::base_xstring_buffer;

		inline auto string() const noexcept -> std::string
		{
			return std::string{ this->substr(0) };
		}
		
		auto wstring(uint32_t codepage) const noexcept -> std::wstring;

		auto u8string(uint32_t codepage) const noexcept -> std::u8string;

		auto u16string(uint32_t codepage) const noexcept -> std::u16string;

		auto convert_encoding(uint32_t o_codepage, uint32_t n_codepage) -> string_buffer&;

		auto convert_to_utf8(uint32_t codepage) -> string_buffer&;

		auto wstring_buffer(uint32_t codepage) const noexcept -> wstring_buffer;
		
		auto u16string_buffer(uint32_t codepage) const noexcept -> u16string_buffer;

		auto u8string_buffer(uint32_t codepage = 65001) const noexcept -> u8string_buffer;
	};

	class u8string_buffer : public base_xstring_buffer<u8string_buffer, char8_t>
	{
		friend string_buffer;
		friend wstring_buffer;
		friend u16string_buffer;

	public:
		using base_xstring_buffer::base_xstring_buffer;

		inline auto u8string() const noexcept -> std::u8string
		{
			return std::u8string{ this->substr(0) };
		}

		auto string(uint32_t to_codepage = 65001) const noexcept -> std::string;

		auto wstring() const noexcept -> std::wstring;

		auto u16string() const noexcept -> std::u16string;

		auto string_buffer(uint32_t to_codepage = 65001) const noexcept -> string_buffer;

		auto wstring_buffer() const noexcept -> wstring_buffer;

		auto u16string_buffer() const noexcept -> u16string_buffer;

	};

	class wstring_buffer : public base_xstring_buffer<wstring_buffer, wchar_t>
	{
		friend string_buffer;
		friend u8string_buffer;
		friend u16string_buffer;

	public:
		using base_xstring_buffer::base_xstring_buffer;

		inline auto wstring() const noexcept -> std::wstring
		{
			return std::wstring{ this->substr(0) };
		}

		inline auto u16string()  const noexcept -> std::u16string 
		{
			const auto temp{ this->substr(0) };
			const auto data{ reinterpret_cast<const char16_t*>(temp.data()) };
			return std::u16string{ data, temp.size() };
		}

		inline auto u8string() const noexcept -> std::u8string 
		{
			auto u8str{ this->string(65001) };
			return *reinterpret_cast<std::u8string*>(&u8str);
		}
		
		auto string(uint32_t to_codepage = 0) const noexcept -> std::string;

		auto string_buffer(uint32_t to_codepage = 0) const noexcept -> string_buffer;

		auto u8string_buffer() const noexcept -> u8string_buffer;

		auto u16string_buffer() const noexcept -> u16string_buffer;
	};

	class u16string_buffer : public base_xstring_buffer<u16string_buffer, char16_t>
	{
		friend string_buffer;
		friend wstring_buffer;
		friend u8string_buffer;

	public:
		using base_xstring_buffer::base_xstring_buffer;

		inline auto u16string() const noexcept -> std::u16string
		{
			return std::u16string{ this->substr(0) };
		}

		inline auto wstring()  const noexcept -> std::wstring
		{
			const auto temp{ this->substr(0) };
			const auto data{ reinterpret_cast<const wchar_t*>(temp.data()) };
			return std::wstring{ data, temp.size() };
		}

		inline auto string(uint32_t to_codepage = 0) const noexcept -> std::string 
		{
			return reinterpret_cast<const xstr::wstring_buffer*>(this)->string(to_codepage);
		}

		inline auto u8string() const noexcept -> std::u8string 
		{
			auto result{ this->string(65001) };
			return *reinterpret_cast<std::u8string*>(&result);
		}

		inline auto string_buffer(uint32_t to_codepage = 0) const noexcept -> string_buffer 
		{
			return reinterpret_cast<const xstr::wstring_buffer*>(this)->string_buffer(to_codepage);
		}

		inline auto u8string_buffer() const noexcept -> u8string_buffer 
		{
			return reinterpret_cast<const xstr::wstring_buffer*>(this)->u8string_buffer();
		}

		auto wstring_buffer() const noexcept -> wstring_buffer;

	};
}

namespace utils::string 
{
	using buffer = utils::xstr::string_buffer;
}

namespace utils::wstring 
{
	using buffer = utils::xstr::wstring_buffer;
}

namespace utils::u8string 
{
	using buffer = utils::xstr::u8string_buffer;
}

namespace utils::u16string 
{
	using buffer = utils::xstr::u16string_buffer;
}
