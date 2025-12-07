#pragma once
#include <tuple>
#include <vector>
#include <xstr.hpp>
#include <xfsys.hpp>
#include "mes.hpp"
#include "config.hpp"

namespace mes::text 
{
	
	class formater 
	{
		const mes::config& m_config;

		static auto is_disallowed_as_start(const wchar_t wchar) -> bool;
		static auto is_disallowed_as_end  (const wchar_t wchar) -> bool;

		static auto is_talking(const std::wstring_view str) -> bool;
		static auto is_half_width(const wchar_t wchar) -> bool;

		static auto parse_text(const std::wstring_view str) -> std::tuple<size_t, size_t>;

	public:

		inline formater(const config& config) noexcept: m_config{ config } 
		{
		}

		auto format(std::string& text, const int32_t input_code_page) const noexcept -> void;

		static auto do_format(xstr::wstring_buffer& buffer, const config& config) -> void;
	};

	extern auto format_dump(const xfsys::file& file, const std::vector<script::text_pair_t>& input, const int32_t input_code_page) -> bool;
	extern auto format_dump(const std::u8string_view path, const std::vector<script::text_pair_t>& input, const int32_t input_code_page) -> bool;
	extern auto format_dump(const std::wstring_view  path, const std::vector<script::text_pair_t>& input, const int32_t input_code_page) -> bool;
		
	extern auto parse_format(const xfsys::file& file, std::vector<script::text_pair_t>& output, const text::formater& formater) -> void;
	extern auto parse_format(const std::wstring_view  path, std::vector<script::text_pair_t>& output, const text::formater& formater) -> void;
	extern auto parse_format(const std::u8string_view path, std::vector<script::text_pair_t>& output, const text::formater& formater) -> void;
	
	extern auto parse_format(const xfsys::file& file, const text::formater& formater) -> std::vector<script::text_pair_t>;
	extern auto parse_format(const std::wstring_view  path, const text::formater& formater) -> std::vector<script::text_pair_t>;
	extern auto parse_format(const std::u8string_view path, const text::formater& formater) -> std::vector<script::text_pair_t>;
}