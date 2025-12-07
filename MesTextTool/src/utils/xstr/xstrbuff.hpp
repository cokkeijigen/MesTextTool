#pragma once
#include <string_buffer.hpp>

namespace xstr {

	using namespace utils::xstr;

	template<class T>
	class buffer : public base_xstring_buffer<buffer<T>, T>
	{
	public:
		using base_xstring_buffer<buffer<T>, T>::base_xstring_buffer;
	};

	template<>
	class buffer<char> : public string_buffer
	{
	public:
		using string_buffer::string_buffer;
	};

	template<>
	class buffer<wchar_t> : public wstring_buffer
	{
	public:
		using wstring_buffer::wstring_buffer;
	};

	template<>
	class buffer<char8_t> : public u8string_buffer
	{
	public:
		using u8string_buffer::u8string_buffer;
	};

	template<>
	class buffer<char16_t> : public u16string_buffer
	{
	public:
		using u16string_buffer::u16string_buffer;
	};
}