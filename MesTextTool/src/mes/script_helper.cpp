#include <iostream>
#include <algorithm> 
#include <ranges>
#include <xstr.hpp>
#include <console.hpp>
#include <mes.hpp>

namespace mes 
{
	script_helper::script_helper(const std::string_view using_script_info_name) noexcept
		: m_script_info{ script_info::query(using_script_info_name) }
	{
	}

	script_helper::script_helper(const mes::script_info* const using_script_info) noexcept
		: m_script_info{ using_script_info }
	{
	}

	auto script_helper::is_parsed() const noexcept -> bool
	{
		return !this->m_script_view.tokens().empty();
	}

	auto script_helper::script_view() const noexcept -> const mes::script_view&
	{
		return this->m_script_view;
	}

	auto script_helper::script_info() const noexcept -> const mes::script_info* const
	{
		return this->m_script_info;
	}

	auto script_helper::using_script_info(const mes::script_info* const info) noexcept -> mes::script_helper&
	{
		this->m_script_info = info;
		return *this;
	}

	auto script_helper::load(const xfsys::file& file) noexcept -> script_helper&
	{
		if (!file.is_open())
		{
			return *this;
		}

		const auto file_size{ file.size() };
		if (file_size == 0x00)
		{
			return *this;
		}

		const auto size_needed{ file_size * 2 };
		if (size_needed > this->m_buffer.size())
		{
			this->m_buffer.clear();
			this->m_buffer.resize(size_needed);
		}

		const auto bytes_read{ file.read(this->m_buffer, file_size) };

		if (bytes_read == file_size)
		{
			this->m_script_view = mes::script_view
			{
				std::span{ this->m_buffer.data(), size_t(file_size) },
				this->m_script_info
			};
		}

		this->m_buffer.recount(file_size);

		return *this;
	}

	auto script_helper::load(const std::wstring_view path, const bool check) noexcept -> script_helper&
	{
		this->m_script_view = {};
		
		if (path.empty())
		{
			return *this;
		}

		if (check && !xfsys::extname_check(path, L".mes"))
		{
			return *this;
		}

		return { this->load(xfsys::open(path, xfsys::read, false)) };
	}

	auto script_helper::load(const std::u8string_view path, const bool check) noexcept -> script_helper&
	{
		this->m_script_view = {};
		
		if (path.empty())
		{
			return *this;
		}

		if (check && !xfsys::extname_check(path, u8".mes"))
		{
			return *this;
		}

		return { this->load(xfsys::open(path, xfsys::read, false)) };
	}

	auto script_helper::load(const std::wstring_view directory, const std::wstring_view name) noexcept -> script_helper&
	{
		if (!xfsys::is_directory(directory))
		{
			return *this;
		}

		std::wstring path{};
		if (!xfsys::extname_check(name, L".mes")) 
		{
			path.assign(xfsys::path::join(directory, xstr::join(name, L"mes")));
		}
		else 
		{
			path.assign(xfsys::path::join(directory, name));
		}

		return this->load(xfsys::open(path, xfsys::read, false));
	}

	auto script_helper::load(const std::u8string_view directory, const std::u8string_view name) noexcept -> script_helper&
	{
		return this->load(xstr::cvt::to_utf16(directory), xstr::cvt::to_utf16(name));
	}

	auto script_helper::save(const xfsys::file& file) noexcept -> bool
	{
		if (!file.is_open())
		{
			return false;
		}

		if (this->m_script_view.raw().empty())
		{
			return false;
		}
		
		const auto count{ file.write(this->m_script_view.raw(), xfsys::file::pos::begin) };
		
		return count == this->m_script_view.raw().size();
	}

	auto script_helper::save(const std::wstring_view path) noexcept -> bool
	{
		return this->save(xfsys::create(path));
	}

	auto script_helper::save(const std::u8string_view path) noexcept -> bool
	{
		return this->save(xfsys::create(path));
	}

	auto script_helper::save(const std::wstring_view  directory, const std::wstring_view  name) noexcept -> bool
	{
		if (!xfsys::create_directory(directory, true))
		{
			return false;
		}

		std::wstring path{};
		if (!xfsys::extname_check(name, L".mes"))
		{
			path.assign(xfsys::path::join(directory, xstr::join(name, L"mes")));
		}
		else 
		{
			path.assign(xfsys::path::join(directory, name));
		}
		
		return this->save(xfsys::create(path));
	}

	auto script_helper::save(const std::u8string_view  directory, const std::u8string_view  name) noexcept -> bool
	{
		return this->save(xstr::cvt::to_utf16(directory), xstr::cvt::to_utf16(name));
	}

	static auto _log_text__(utils::string::buffer text, int opcode, uint32_t cdpg) -> void
	{
		auto wstr{ text.wstring_buffer(cdpg) };
		for (auto& achar : wstr)
		{
			if (static_cast<uint16_t>(achar) >= 125)
			{
				xcout::helper.write(L"[0x%02X]%ls\n", opcode, wstr.data());
				break;
			}
		}
	}

	auto script_helper::export_text(const bool absolute_file_offset) const noexcept -> std::vector<text_pair_t>
	{
		const auto& info  { this->m_script_view.info()   };
		const auto& asmbin{ this->m_script_view.asmbin() };

		std::vector<script_helper::text_pair_t> result{};
		const auto base{ absolute_file_offset ? asmbin.offset() : 0 };

		for (const script_view::token& token : this->m_script_view.tokens())
		{
			/*if (info->string.its(token.value))
			{
				_log_text__({ reinterpret_cast<char*>(asmbin.data() + token.offset + 1) }, int(token.value), xcsl::cdpg::sjis);
			}*/

			if (info->encstr.its(token.value))
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data() + token.offset + 1)};
				std::ranges::for_each(text, [&](char& ch) {
					ch += this->m_script_view.info()->enckey; // 解密字符串
				});
				result.push_back(text_pair_t{ static_cast<int32_t>(token.offset + base), text });
			}
			else if (token.value != 0x00 && std::ranges::contains(info->opstrs, token.value))
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data() + token.offset + 1)};
				result.push_back(text_pair_t{ static_cast<int32_t>(token.offset + base), text });
			}
		}

		return result;
	}

	auto script_helper::import_text(const std::vector<text_pair_t>& texts, bool absolute_file_offset) noexcept -> bool
	{
		if (texts.empty())
		{
			return false;
		}

		if (this->m_script_view.tokens().empty())
		{
			return { false };
		}

		const auto& info  { this->m_script_view.info()   };
		const auto& asmbin{ this->m_script_view.asmbin() };
		const auto& labels{ this->m_script_view.labels() };
		const auto& tokens{ this->m_script_view.tokens() };
		const auto data_bytes{ labels.size() * sizeof(int32_t) };

		utils::xmem::buffer<uint8_t> buffer{};
		buffer.resize(this->m_buffer.size());
		buffer.recount(data_bytes); // 先空出写入labels数据的空间

		size_t label_index{};
		const auto base{ absolute_file_offset ? asmbin.offset() : 0};

		for (const script_view::token& token : tokens)
		{
			if (labels.offset() == 0x04 && label_index < labels.size())
			{
				const auto first_token_length
				{
					labels.offset() == 0x08 ? 0x03 :
					(info->version & 0xFF00) == 0x00 ?
					0x01 : 0x02
				};
				if (token.offset + first_token_length == labels.data()[label_index])
				{
					auto  count{ static_cast<int32_t>(buffer.count()) };
					auto offset{ count - asmbin.offset() + first_token_length};
					auto& label{ labels.data()[label_index] };
					label = { static_cast<int32_t>(offset) };
					label_index++;
				}
			}

			if (info->encstr.its(token.value))
			{
				const auto&& it{ std::ranges::find(texts, token.offset + base, &text_pair_t::offset) };
				if (it != texts.end())
				{
					std::string text{ it->text().string };
					std::ranges::for_each(text, [&](char& ch) {
						ch += this->m_script_view.info()->enckey; // 解密字符串
					});
					buffer.write(token.value).write(text).write('\0');
					continue;
				}
			}

			if (std::ranges::contains(info->opstrs, token.value))
			{
				const auto&& it{ std::ranges::find(texts, token.offset + base, &text_pair_t::offset) };
				if (it != texts.end())
				{
					buffer.write(token.value).write(it->text().string).write('\0');
					continue;
				}
			}

			if (labels.offset() == 0x08 && (token.value == 0x03 || token.value == 0x04))
			{
				if (label_index < labels.size())
				{
					auto  count{ static_cast<int32_t>(buffer.count()) };
					auto offset{ count - asmbin.offset() + token.length};
					auto& label{ labels.data()[label_index] };
					label = { (label & (0xFF << 0x18)) | offset };
					label_index++;
				}
			}

			buffer.write(asmbin.data() + token.offset, token.length);
		}

		buffer.write(0, reinterpret_cast<const uint8_t*>(labels.data()), data_bytes);

		return true;
	}
}