#include <iostream>
#include <algorithm> 
#include <ranges>
#include <xstr.hpp>
#include <script_text.hpp>

namespace mes::text 
{
	auto formater::is_disallowed_as_start(const wchar_t wchar) -> bool 
	{
		return std::ranges::contains(L"。、？’”，！～】；：）」』… 　", wchar);
	}

	auto formater::is_disallowed_as_end(const wchar_t wchar) -> bool
	{
		return std::ranges::contains(L"（(「『【‘“", wchar);
	}

	auto formater::is_talking(const std::wstring_view str) -> bool
	{
		if (str.length() < 2) 
		{
			return false;
		}
		
		constexpr const wchar_t* quotes[] = { L"「」", L"『』", L"“”" };
		return std::ranges::any_of(quotes, [&](const auto& quote) -> bool
			{
				return str.starts_with(quote[0]) && str.ends_with(quote[1]);
			}
		);
	}

	auto formater::is_half_width(const wchar_t wchar) -> bool
	{
		return { static_cast<bool>(wchar >= 0x0000 && wchar <= 0x007F) };
	}

	auto formater::parse_text(const std::wstring_view str) -> std::tuple<size_t, size_t>
	{
		const size_t split{ str.find(L'／') };
		if (split == std::wstring_view::npos)
		{
			return { std::wstring_view::npos, std::wstring_view::npos };
		}

		size_t end{ str.find(L'｝', split) };
		return { split, end };
	}

	auto formater::do_format(xstr::wstring_buffer& buffer, const mes::config& config) -> void
	{
		buffer.remove(L"\\n　").remove(L"\\n");

		const auto text_view  { buffer.view()    };
		const auto text_length{ text_view.size() };

		if (text_length <= static_cast<size_t>(config.text_min_length))
		{
			return;
		}

		const auto is_talking{ formater::is_talking(text_view) };
		const auto text_min_length{ static_cast<float>(config.text_min_length) };
		const auto text_max_length{ static_cast<float>(config.text_max_length) };

		float line_char_count{ 0.0f };
		xstr::wstring_buffer new_buffer{ text_length + 0x10 };

		for (size_t index{ 0 }; index < text_length;)
		{
			const wchar_t& this_char = text_view[index];

			if (line_char_count >= text_min_length)
			{
				if (!formater::is_disallowed_as_start(this_char))
				{
					new_buffer.write({ is_talking ? L"\n　" : L"\n" });
					line_char_count = { is_talking ? 1.0f : 0.0f };
				}
			}

			if (this_char == L'｛')
			{
				const auto& [split, end] { formater::parse_text(text_view.substr(index)) };
				if (split != std::wstring_view::npos && end != std::wstring_view::npos)
				{
					const size_t count{ end - split - 1 };
					auto subtext{ text_view.substr(index + split + 1, count) };
					
					float target_count{ 0.0f };
					for (const wchar_t& chr : subtext)
					{
						target_count += formater::is_half_width(chr) ? 0.5f : 1.0f;
					}

					line_char_count += target_count;
					if (line_char_count >= config.text_max_length)
					{
						new_buffer.write({ is_talking ? L"\n　" : L"\n" });
						line_char_count = target_count;
					}

					new_buffer.write(text_view.substr(index, end));

					index += end + 1;
					continue;
				}
			}

			if (!formater::is_half_width(this_char)) 
			{
				line_char_count += 1.0f;
			}
			else if (this_char == L' ' || index == text_length - 1)
			{
				line_char_count += 0.5f;
			}
			else
			{
				auto subtext{ text_view.substr(index) };
				const auto it = std::ranges::find_if_not
				(
					subtext,
					[](const wchar_t& chr) -> bool
					{
						return formater::is_half_width(chr) && chr != L' ';
					}
				);

				if (it != subtext.end())
				{
					const auto count{ std::ranges::distance(subtext.begin(), it) };
					subtext = std::move(subtext.substr(0, count));
				}

				if (!subtext.starts_with(L"@"))
				{
					const auto target_count{ static_cast<float>(subtext.size()) / 2.0f };

					line_char_count += target_count;
					if (line_char_count >= config.text_max_length)
					{
						new_buffer.write(is_talking ? L"\n　" : L"\n");
						line_char_count = target_count;
					}
				}

				new_buffer.write(subtext);
				index += subtext.size();
				continue;
			}

			if (line_char_count >= static_cast<float>(config.text_min_length))
			{
				if (formater::is_disallowed_as_end(this_char))
				{
					line_char_count ={ is_talking ? 2.0f : 1.0f };
					new_buffer.write({ is_talking ? L"\n　" : L"\n" });
				}
			}

			new_buffer.write(this_char);
			index++;
		}

		buffer = std::move(new_buffer);
	}

	auto formater::format(std::string& text, const int32_t input_code_page) const noexcept -> void
	{
		if (text.empty())
		{
			return;
		}

		xstr::wstring_buffer buffer{ xstr::cvt::to_utf16(text, input_code_page) };

		for (const auto& [key, value] : this->m_config.after_replaces)
		{
			buffer.replace(key, value);
		}

		buffer.replace(L"/", L"／");
		buffer.replace(L"{", L"｛");
		buffer.replace(L"}", L"｝");

		bool need_enable_format{ false };

		if (buffer.starts_with(L"@::"))
		{
			buffer.remove(L"@::", 0, 1);
			buffer.replace(L"\\n", L"\n");
		}
		else
		{
			need_enable_format = bool
			{
				(this->m_config.text_max_length != -1 || this->m_config.text_min_length != -1) &&
				this->m_config.text_max_length >= this->m_config.text_min_length
			};
		}

		if (need_enable_format)
		{
			formater::do_format(buffer, this->m_config);
		}

		for (const auto& [key, value] : this->m_config.after_replaces)
		{
			buffer.replace(key, value);
		}

		text = std::move(buffer.string(this->m_config.use_code_page));
	}

	auto parse_format(const xfsys::file& file, std::vector<entry>& output, const text::formater& formater) -> void
	{
		output.clear();

		if (!file.is_open())
		{
			return;
		}

		xstr::u8string_buffer buffer{};
		const auto bytes_read{ file.read(buffer, file.size(), xfsys::file::pos::begin) };
		
		if (bytes_read == 0 || buffer.count() == 0)
		{
			return;
		}

		int32_t offset{ -1 };
		for (const std::u8string_view& line : xstr::line::iter(buffer, true))
		{
			if (line.empty())
			{
				continue;
			}

			if (line.starts_with(u8"#0x"))
			{
				const auto data{ reinterpret_cast<const char*>(line.data()) };
				const int matched{ std::sscanf(data, "#0x%X", &offset) };
				if (matched != 1)
				{
					offset = -1;
				}
				continue;
			}

			if (offset == -1)
			{
				continue;
			}

			if (line.find(u8"//") != std::u8string_view::npos)
			{
				continue;
			}

			const size_t pos{ line.find(u8"◎★") };
			if (pos == std::u8string_view::npos)
			{
				continue;
			}

			const auto _line{ reinterpret_cast<const std::string_view*>(&line) };
			std::string text{ _line->substr(pos + 6) };
			
			formater.format(text, CP_UTF8);
			output.push_back(entry{ offset, text });

			offset = -1;
		}
	}

	auto format_dump(const xfsys::file& file, const std::vector<entry>& input, const int32_t input_code_page) -> bool
	{
		if (!file.is_open())
		{
			return false;
		}

		xstr::string_buffer buffer{}, this_line{};
		for (const auto&& [i, line] : std::views::enumerate(input))
		{
			std::string* const entry_string{ line.string() };

			if (entry_string == nullptr || entry_string->empty())
			{
				continue;
			}

			buffer.write_as_format("#0x%X\n", line.offset());
			
			this_line.reset().write(*entry_string).replace("\n", "\\n");
			
			this_line.convert_to_utf8(input_code_page);

			buffer.write_as_format(u8"★◎  %03d  ◎★//%s\n", i + 1, this_line.data());
			buffer.write_as_format(u8"★◎  %03d  ◎★%s\n\n", i + 1, this_line.data());
		}

		const auto bytes_write{ file.write(buffer, buffer.count(), xfsys::file::pos::begin) };
		const auto result{ bytes_write == buffer.count() };
		buffer.clear(), this_line.clear();

		return result;
	}

	auto parse_format(const xfsys::file& file, const text::formater& formater) -> std::vector<entry>
	{
		std::vector<entry> result{};
		text::parse_format(file, result, formater);
		return result;
	}

	auto parse_format(const std::wstring_view path, std::vector<entry>& output, const text::formater& formater) -> void
	{
		text::parse_format(xfsys::open(path, xfsys::read, false), output, formater);
	}

	auto parse_format(const std::u8string_view path, std::vector<entry>& output, const text::formater& formater) -> void
	{
		text::parse_format(xfsys::open(path, xfsys::read, false), output, formater);
	}

	auto parse_format(const std::wstring_view path, const text::formater& formater) -> std::vector<entry>
	{
		return text::parse_format(xfsys::open(path, xfsys::read, false), formater);
	}

	auto parse_format(const std::u8string_view path, const text::formater& formater) -> std::vector<entry>
	{
		return text::parse_format(xfsys::open(path, xfsys::read, false), formater);
	}

	auto format_dump(const std::u8string_view path, const std::vector<entry>& input, const int32_t input_code_page) -> bool
	{
		return text::format_dump(xfsys::create(path), input, input_code_page);
	}

	auto format_dump(const std::wstring_view path, const std::vector<entry>& input, const int32_t input_code_page) -> bool
	{
		return text::format_dump(xfsys::create(path), input, input_code_page);
	}

}