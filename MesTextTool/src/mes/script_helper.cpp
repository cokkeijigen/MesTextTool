#include <iostream>
#include <algorithm> 
#include <ranges>
#include <xstr.hpp>
#include <console.hpp>
#include <mes.hpp>

namespace mes 
{

	auto script_helper::union_view_t::advtxt_view() const noexcept -> const mes::advtxt_view*
	{
		if (auto&& value = std::get_if<mes::advtxt_view>(&this->m_value))
		{
			return value;
		}
		return nullptr;
	}
	
	auto script_helper::union_view_t::script_view() const noexcept -> const mes::script_view*
	{
		if (auto&& value = std::get_if<mes::script_view>(&this->m_value))
		{
			return value;
		}
		return nullptr;
	}

	script_helper::union_view_t::union_view_t(mes::script_view&& script_view) noexcept
	{
		this->m_value = std::move(script_view);
	}

	script_helper::union_view_t::union_view_t(mes::advtxt_view&& advtxt_view) noexcept
	{
		this->m_value = std::move(advtxt_view);
	}

	auto script_helper::union_view_t::operator=(mes::script_view&& script_view) noexcept -> union_view_t&
	{
		this->m_value = std::move(script_view);
		return *this;
	}

	auto script_helper::union_view_t::operator=(mes::advtxt_view&& advtxt_view) noexcept -> union_view_t&
	{
		this->m_value = std::move(advtxt_view);
		return *this;
	}


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

		const size_t file_size{ file.size() };
		if (file_size == 0x00)
		{
			return *this;
		}

		if (file_size > this->m_buffer.size())
		{
			this->m_buffer.clear();
			this->m_buffer.resize(file_size);
		}

		const size_t bytes_read{ file.read(this->m_buffer, file_size, xfsys::file::pos::begin, 0) };
		if (bytes_read == file_size)
		{
			if (mes::is_advtxt(this->m_buffer))
			{
				this->m_data_view = mes::advtxt_view
				{
					std::span<uint8_t>{ this->m_buffer.data(), file_size }
				};
			}
			else 
			{
				this->m_data_view = mes::script_view
				{ 
					std::span<uint8_t>{ this->m_buffer.data(), file_size },
					this->m_script_info 
				};
			}
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
		{
			const auto script_view{ this->m_data_view.script_view() };
			if (script_view != nullptr)
			{
				const auto count{ file.write(script_view->raw(), xfsys::file::pos::begin) };
				return count == script_view->raw().size();
			}
		}
		{
			const auto advtxt_view{ this->m_data_view.advtxt_view() };
			if (advtxt_view != nullptr)
			{
				const auto count{ file.write(advtxt_view->raw(), xfsys::file::pos::begin) };
				return count == advtxt_view->raw().size();
			}
		}
		return false;
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
		/*xcout::helper.set_cp(cdpg);
		xcout::helper.write("[0x%02X]%s\n", opcode, text.data());
		xcout::helper.reset_attrs();*/

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
		std::vector<script_helper::text_pair_t> result{};

		const mes::script_view* script_view{ this->m_data_view.script_view() };
		if (script_view != nullptr)
		{
			const mes::script_info* info{ script_view->info() };
			const mes::script_view::view_t<uint8_t>&    asmbin{ script_view->asmbin() };
			const std::vector<mes::script_view::token>& tokens{ script_view->tokens() };

			const int32_t base{ absolute_file_offset ? asmbin.offset() : 0 };
			for (const mes::script_view::token& token : tokens)
			{
				//if (info->string.is(token.opcode()))
				//{
				//	// if(token.opcode() != 0x45)
				//	_log_text__({ token.string()->str }, int(token.opcode()), xcout::cdpg::sjis);
				//}

				if (info->encstr.is(token.opcode()))
				{
					std::string  text{ token.encstr()->str };
					const auto offset{ static_cast<int32_t>(token.offset + base) };

					std::ranges::for_each(text, [&](char& ch) {
						ch += this->m_script_view.info()->enckey; // 解密字符串
					});

					result.push_back(text_pair_t{ offset, text });
				}
				else if (token.opcode() != 0x00 && std::ranges::contains(info->opstrs, token.opcode()))
				{
					std::string  text{ token.string()->str };
					const auto offset{ static_cast<int32_t>(token.offset + base) };
					result.push_back(text_pair_t{ offset, text });
				}
			}
		}
		else 
		{
			const mes::advtxt_view* advtxt_view{ this->m_data_view.advtxt_view() };
			if (advtxt_view != nullptr)
			{
				const std::vector<mes::advtxt_view::token>& tokens{ advtxt_view->tokens() };

				for (const mes::advtxt_view::token& token : tokens)
				{
					//mes::advtxt::string_parse(token);
				}
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
		return bool
		{
			this->script_import(texts, absolute_file_offset) ? true :
			this->advtxt_import(texts, absolute_file_offset)
		};
	}

	auto script_helper::script_import(const std::vector<text_pair_t>& texts, bool absolute_file_offset) noexcept -> bool
	{
		const mes::script_view* script_view{ this->m_data_view.script_view() };
		if (script_view == nullptr)
		{
			return false;
		}

		const std::vector<mes::script_view::token>& tokens{ script_view->tokens() };
		if (tokens.empty())
		{
			return false;
		}

		const mes::script_info* info { script_view->info() };
		const mes::script_view::view_t<uint8_t>& asmbin{ script_view->asmbin() };
		const mes::script_view::view_t<int32_t>& labels{ script_view->labels() };
		const size_t data_labels_bytes{ labels.size() * sizeof(int32_t) };

		utils::xmem::buffer<uint8_t> buffer{};
		buffer.resize(this->m_buffer.size());
		buffer.recount(data_labels_bytes); // 先空出写入labels数据的空间

		size_t label_index{};
		const int32_t base{ absolute_file_offset ? asmbin.offset() : 0};

		for (const mes::script_view::token& token : tokens)
		{
			if (labels.offset() == 0x04 && label_index < labels.size())
			{
				const int32_t first_token_length
				{
					labels.offset() == 0x08 ? 0x03 :
					(info->version & 0xFF00u) == 0x00 ?
					0x01 : 0x02
				};
				if (token.offset + first_token_length == labels.data()[label_index])
				{
					int32_t  count{ static_cast<int32_t>(buffer.count()) };
					int32_t offset{ count - asmbin.offset() + first_token_length };
					int32_t& label{ labels.data()[label_index] };
					label = { static_cast<int32_t>(offset) };
					label_index++;
				}
			}

			if (info->encstr.is(token.opcode()))
			{
				const auto&& it{ std::ranges::find(texts, token.offset + base, &text_pair_t::offset) };
				if (it != texts.end())
				{
					std::string text{ it->text().string };
					std::ranges::for_each(text, [&](char& ch) {
						ch += this->m_script_view.info()->enckey; // 解密字符串
					});
					buffer.write(token.opcode()).write(text).write('\0');
					continue;
				}
			}

			if (std::ranges::contains(info->opstrs, token.opcode()))
			{
				const auto&& it{ std::ranges::find(texts, token.offset + base, &text_pair_t::offset) };
				if (it != texts.end())
				{
					buffer.write(token.opcode()).write(it->text().string).write('\0');
					continue;
				}
			}

			if (labels.offset() == 0x08 && (token.opcode() == 0x03 || token.opcode() == 0x04))
			{
				if (label_index < labels.size())
				{
					int32_t  count{ static_cast<int32_t>(buffer.count())   };
					int32_t offset{ count - asmbin.offset() + token.length };
					int32_t& label{ labels.data()[label_index] };
					label = { (label & (0xFF << 0x18)) | offset };
					label_index++;
				}
			}

			buffer.write(asmbin.data() + token.offset, token.length);
		}

		buffer.write(0, reinterpret_cast<const uint8_t*>(labels.data()), data_labels_bytes);

		this->m_buffer    = std::move(buffer);
		this->m_data_view = mes::script_view
		{ 
			std::span<uint8_t>{ this->m_buffer.data(), this->m_buffer.count() },
			this->m_script_info 
		};

		return true;
	}

	auto script_helper::advtxt_import(const std::vector<text_pair_t>& texts, bool absolute_file_offset) noexcept -> bool
	{
		const mes::advtxt_view* advtxt_view{ this->m_data_view.advtxt_view() };
		if (advtxt_view == nullptr)
		{
			return false;
		}

		const std::vector<advtxt_view::token>& tokens{ advtxt_view->tokens() };
		if (tokens.empty())
		{
			return false;
		}

		return true;
	}
}