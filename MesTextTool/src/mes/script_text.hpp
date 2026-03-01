#pragma once
#include <tuple>
#include <vector>
#include <xstr.hpp>
#include <xfsys.hpp>
#include <mes.hpp>
#include <config.hpp>

namespace mes::text 
{
	
	class entry
	{
	public:
		using variant = std::variant<std::nullptr_t, std::string, std::wstring>;

		inline auto    offset() const noexcept -> int32_t;
		inline auto  u8string() const noexcept -> std::u8string*;
		inline auto u16string() const noexcept -> std::u16string*;
		inline auto    string() const noexcept -> std::string*;
		inline auto   wstring() const noexcept -> std::wstring*;

		inline entry() noexcept : m_text{ nullptr } {};
		inline entry(std::nullptr_t) noexcept : m_text{ nullptr } {};

		inline entry(int32_t offset, std::u8string_view   u8str) noexcept;
		inline entry(int32_t offset, std::u16string_view u16str) noexcept;
		inline entry(int32_t offset, std::string_view  str) noexcept;
		inline entry(int32_t offset, std::wstring_view str) noexcept;

		inline entry(const entry& other) noexcept;
		inline entry(entry&&      other) noexcept;

		inline auto operator=(const entry& other) noexcept -> entry&;
		inline auto operator=(entry&& other) noexcept -> entry&;

		inline auto empty() const noexcept -> bool;

	protected:
		int32_t m_offset{};
		variant m_text{};
	};

	class formater 
	{
		const mes::config& m_config;
		mutable bool m_needs_transcoding;

		static auto is_disallowed_as_start(const wchar_t wchar) -> bool;
		static auto is_disallowed_as_end  (const wchar_t wchar) -> bool;

		static auto is_talking(const std::wstring_view str) -> bool;
		static auto is_half_width(const wchar_t wchar) -> bool;

		static auto parse_text(const std::wstring_view str) -> std::tuple<size_t, size_t>;

	public:

		inline formater(const config& config, bool needs_transcoding = true) noexcept
			: m_config{ config }, m_needs_transcoding{ needs_transcoding }
		{
		}

		inline auto transcoding(const bool needs) noexcept -> void;

		auto format(std::string&  text,  const uint32_t input_code_page) const noexcept -> void;
		auto format(std::wstring& text) const noexcept -> void;

		static auto do_format(xstr::buffer<wchar_t>& buffer, const config& config) -> void;
	};

	extern auto format_dump(const xfsys::file& file, const std::vector<entry>& input, const int32_t input_code_page) -> bool;
	extern auto format_dump(const std::u8string_view path, const std::vector<entry>& input, const int32_t input_code_page) -> bool;
	extern auto format_dump(const std::wstring_view  path, const std::vector<entry>& input, const int32_t input_code_page) -> bool;

	extern auto parse_format(const xfsys::file& file, std::vector<entry>& output, const text::formater& formater, bool entry_wstring = false) -> void;
	extern auto parse_format(const std::wstring_view  path, std::vector<entry>& output, const text::formater& formater, bool entry_wstring = false) -> void;
	extern auto parse_format(const std::u8string_view path, std::vector<entry>& output, const text::formater& formater, bool entry_wstring = false) -> void;
	
	extern auto parse_format(const xfsys::file& file, const text::formater& formater, bool entry_wstring = false) -> std::vector<entry>;
	extern auto parse_format(const std::wstring_view  path, const text::formater& formater, bool entry_wstring = false) -> std::vector<entry>;
	extern auto parse_format(const std::u8string_view path, const text::formater& formater, bool entry_wstring = false) -> std::vector<entry>;
	

	inline auto formater::transcoding(const bool needs) noexcept -> void
	{
		this->m_needs_transcoding = needs;
	}

	inline auto entry::offset() const noexcept -> int32_t
	{
		return this->m_offset;
	}

	inline auto entry::string() const noexcept -> std::string*
	{
		if (auto&& value = std::get_if<std::string>(&this->m_text))
		{
			return const_cast<std::string*>(value);
		}
		return nullptr;
	}

	inline auto entry::wstring() const noexcept -> std::wstring*
	{
		if (auto&& value = std::get_if<std::wstring>(&this->m_text))
		{
			return const_cast<std::wstring*>(value);
		}
		return nullptr;
	}

	inline auto entry::u8string() const noexcept -> std::u8string*
	{
		return reinterpret_cast<std::u8string*>(this->string());
	}

	inline auto entry::u16string() const noexcept -> std::u16string*
	{
		return reinterpret_cast<std::u16string*>(this->wstring());
	}

	inline entry::entry(const entry& other) noexcept
	{
		entry::operator=(other);
	}

	inline entry::entry(entry&& other) noexcept
	{
		this->m_offset = std::move(other.m_offset);
		this->m_text   = std::move(other.m_text);
	}

	inline entry::entry(int32_t offset, std::u8string_view u8str) noexcept
	{
		this->m_offset = offset;
		this->m_text = std::string{ reinterpret_cast<const char*>(u8str.data()), u8str.size() };
	}

	inline entry::entry(int32_t offset, std::u16string_view u16str) noexcept
	{
		this->m_offset = offset;
		this->m_text = std::wstring{ reinterpret_cast<const wchar_t*>(u16str.data()), u16str.size() };
	}

	inline entry::entry(int32_t offset, std::string_view  str) noexcept
	{
		this->m_offset = offset;
		this->m_text = std::string{ str };
	}

	inline entry::entry(int32_t offset, std::wstring_view str) noexcept
	{
		this->m_offset = offset;
		this->m_text = std::wstring{ str };
	}

	inline auto entry::empty() const noexcept -> bool
	{
		return this->m_text.index() == 0 || this->m_text.valueless_by_exception();
	}

	inline auto entry::operator=(const entry& other) noexcept -> entry&
	{
		if (this != &other)
		{
			this->m_offset = other.m_offset;
			this->m_text   = other.m_text;
		}
		return *this;
	}

	inline auto entry::operator=(entry&& other) noexcept -> entry&
	{
		this->m_offset = std::move(other.m_offset);
		this->m_text   = std::move(other.m_text);
		return *this;
	}
}