#pragma once
#include <string_buffer.hpp>

namespace xstr 
{

	using namespace utils::xstr;

	template<typename char_t, template<typename> class derived>
	struct buffer_traits
	{
		using base_type = base_xstring_buffer<derived, char_t>;
	};

	template<template<typename> class derived>
	struct buffer_traits<char, derived>
	{
		using base_type = string_buffer;
	};

	template<template<typename> class derived>
	struct buffer_traits<wchar_t, derived>
	{
		using base_type = wstring_buffer;
	};

	template<template<typename> class derived>
	struct buffer_traits<char8_t, derived>
	{
		using base_type = u8string_buffer;
	};

	template<template<typename> class derived>
	struct buffer_traits<char16_t, derived>
	{
		using base_type = u16string_buffer;
	};

	template<class char_t>
	struct buffer : public buffer_traits<char_t, buffer>::base_type
	{
		using base_type = typename buffer_traits<char_t, buffer>::base_type;
		using base_type::base_type;
	};

	template<class char_t>
	buffer(const char_t*) -> buffer<char_t>;

	template<class char_t>
	buffer(const std::basic_string<char_t>&) -> buffer<char_t>;

	template<class char_t>
	buffer(std::basic_string<char_t>&&) -> buffer<char_t>;

	template<class char_t>
	buffer(std::basic_string_view<char_t>&) -> buffer<char_t>;

	template<class char_t>
	buffer(std::basic_string_view<char_t>&&) -> buffer<char_t>;
}