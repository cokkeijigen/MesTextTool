#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm> 
#include <mes.hpp>
#include <file.hpp>
#include <console.hpp>
#include <xmemory.hpp>

namespace mes {

	const script_info script_info::infos[]
	{  /*  name          version   uint8x2         uint8str        string          encstr          uint16x4     optunenc deckey */
		{ "ffexa"      , 0x7B69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x43,  0x20 },
		{ "ffexs"      , 0x7B6B, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x4B }, { 0x4c, 0x4F }, { 0x50, 0xFF },  0x43,  0x20 },
		{ "ef"         , 0x466A, { 0x00, 0x28 }, { 0x2A, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x46,  0x20 },
		{ "dcos"       , 0x315D, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x45 }, { 0x46, 0x49 }, { 0x4A, 0xFF },  0x42,  0x20 },
		{ "ktlep"      , 0x6E69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x45,  0x20 },
		{ "dcws"       , 0x656C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x48,  0x20 },
		{ "dcsv"       , 0x636C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x46,  0x20 },
		{ "dcpc"       , 0x3D63, { 0x00, 0x2C }, { 0xFF, 0xFF }, { 0x2D, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x44,  0x20 },
		{ "dcmems"     , 0x315D, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x45 }, { 0x46, 0x49 }, { 0x4A, 0xFF },  0x42,  0x20 },
		{ "dcdx"       , 0x7769, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x45,  0x20 },
		{ "dcas"       , 0x4E69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x43,  0x20 },
		{ "dcbs"       , 0x3163, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x48 }, { 0x49, 0x4C }, { 0x4D, 0xFF },  NULL,  0x20 },
		{ "dc2fl"      , 0x9C69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x45,  0x20 },
		{ "dc2bs"      , 0x316C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  NULL,  0x20 },
		{ "dc2dm"      , 0x9D72, { 0x00, 0x29 }, { 0x2A, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x44,  0x20 },
		{ "dc2fy"      , 0x3866, { 0x00, 0x2E }, { 0xFF, 0xFF }, { 0x2F, 0x4B }, { 0x4C, 0x4F }, { 0x50, 0xFF },  0x48,  0x20 },
		{ "dc2cckko"   , 0x026C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  NULL,  0x20 },
		{ "dc2ccotm"   , 0x016C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  NULL,  0x20 },
		{ "dc2sc"      , 0x3B69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x45,  0x20 },
		{ "dc2ty"      , 0x5F69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  NULL,  0x20 },
		{ "dc2pc"      , 0x5769, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x45,  0x20 },
		{ "dc3rx"      , 0x9772, { 0x00, 0x2B }, { 0x2C, 0x33 }, { 0x34, 0x4E }, { 0x4F, 0x52 }, { 0x53, 0xFF },  0x45,  0x20 },
		{ "dc3pp"      , 0x9872, { 0x00, 0x2A }, { 0x2B, 0x32 }, { 0x33, 0x4E }, { 0x4F, 0x51 }, { 0x52, 0xFF },  0x45,  0x20 },
		{ "dc3wy"      , 0xA09F, { 0x00, 0x38 }, { 0x39, 0x41 }, { 0x42, 0x5F }, { 0x60, 0x63 }, { 0x64, 0xFF },  0x55,  0x20 },
		{ "dc3dd"      , 0xA5A8, { 0x00, 0x38 }, { 0x39, 0x43 }, { 0x44, 0x62 }, { 0x63, 0x67 }, { 0x68, 0xFF },  0x58,  0x20 },
		{ "dc4"        , 0xAAB6, { 0x00, 0x3A }, { 0x3B, 0x47 }, { 0x48, 0x68 }, { 0x69, 0x6D }, { 0x6E, 0xFF },  0x5D,  0x20 },
		{ "dc4ph"      , 0xABB6, { 0x00, 0x3A }, { 0x3B, 0x47 }, { 0x48, 0x68 }, { 0x69, 0x6D }, { 0x6E, 0xFF },  0x5D,  0x20 },
		{ "ds"         , 0x9F9A, { 0x00, 0x38 }, { 0x39, 0x4A }, { 0x41, 0x5E }, { 0x5F, 0x62 }, { 0x63, 0xFF },  0x54,  0x20 },
		{ "dsif"       , 0xA1A1, { 0x00, 0x39 }, { 0x3A, 0x42 }, { 0x43, 0x60 }, { 0x61, 0x64 }, { 0x65, 0xFF },  0x56,  0x20 },
		{ "tmpl"       , 0xA6B4, { 0x00, 0x3B }, { 0x3A, 0x46 }, { 0x46, 0x67 }, { 0x68, 0x6E }, { 0x6D, 0xFF },  0x5C,  0x20 },
		{ "nightshade" , 0x0871, { 0x00, 0x2B }, { 0x2C, 0x33 }, { 0x34, 0x4E }, { 0x4F, 0x52 }, { 0x53, 0xFF },  0x43,  0x01 },
		{ "puripa"     , 0x5B6C, { 0x00, 0x28 }, { 0x29, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x46,  0x20 },
		{ "uni"        , 0x746A, { 0x00, 0x2B }, { 0x2A, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x46,  0x20 },
	};

	inline auto script_info::section::its(uint8_t key) const -> bool
	{
		return ((!(beg == end && beg == 0xFF)) && (key >= beg && key <= end));
	}

	auto script_info::query(std::string_view name) -> const script_info* 
	{
		if(!name.empty()) for (const auto& info : script_info::infos)
		{
			if (name == info.name) return &info;
		}
		return nullptr;
	}

	auto script_info::query(uint16_t version) -> const script_info* 
	{
		for (const auto& info : script_info::infos) 
		{
			if (version == info.version) return &info;
		}
		return nullptr;
	}

	script_view::script_view(std::span<uint8_t> raw, uint16_t version) : script_view(raw, script_info::query(version)) 
	{
	}

	script_view::script_view(std::span<uint8_t> raw, const char* name) : script_view(raw, script_info::query(name)) 
	{
	}

	script_view::script_view(std::span<uint8_t> raw, const script_info* info) : 
		m_Raw(view<uint8_t>{ raw.data(), static_cast<int32_t>(raw.size()), 0 }), m_Info(info) 
	{
		if (!raw.data() || raw.empty()) return;
		
		if (auto head{ reinterpret_cast<int32_t*>(raw.data()) }; head[1] == 0x03)
		{
			auto offset{ static_cast<int32_t>(head[0] * 0x06 + 0x04) };
			if (auto size{ static_cast<int32_t>(raw.size()) }; size > offset)
			{
				if (size > offset + 0x03) 
				{
					this->m_Version = *reinterpret_cast<uint16_t*>(raw.data() + offset);
					if(!this->m_Info) this->m_Info = script_info::query(this->m_Version);
					this->m_Asmbin = script_view::view<uint8_t>
					{
						.data = reinterpret_cast<uint8_t*>(raw.data() + offset + 0x03),
						.size = static_cast<int32_t>(raw.size() - offset - 0x03),
						.offset = offset + 0x03
					};
				}
				this->m_Blocks = script_view::view<int32_t> 
				{
					.data = reinterpret_cast<int32_t*>(raw.data() + 0x08),
					.size = static_cast<int32_t>((offset - 4) / 4),
					.offset = 0x08
				};
			}
		}
		else 
		{
			auto offset{ static_cast<int32_t>(head[0] * 0x04 + 0x04) };
			if (auto size{ static_cast<int32_t>(raw.size()) }; size > offset)
			{
				if (size > offset + 0x02) 
				{
					this->m_Version = *reinterpret_cast<uint16_t*>(raw.data() + offset);
					if (!this->m_Info) this->m_Info = script_info::query(this->m_Version);
					this->m_Asmbin = script_view::view<uint8_t>
					{
						.data = raw.data() + offset + 0x02,
						.size = static_cast<int32_t>(raw.size() - offset - 0x02),
						.offset = offset + 0x02
					};
				}
				this->m_Blocks = script_view::view<int32_t>
				{
					.data = reinterpret_cast<int32_t*>(raw.data() + 0x04),
					.size = static_cast<int32_t>(head[0]),
					.offset = 0x04
				};
			}
		}
		this->token_parse();
	}

	auto script_view::token_parse() -> void 
	{
		if (nullptr == this->m_Info) return;
		
		this->m_Tokens.clear();
		for (int32_t offset{ 0 }; offset < this->m_Asmbin.size; )
		{
			script_view::token token 
			{ 
				.offset = offset, 
				.value = this->m_Asmbin.data[offset] 
			};

			if (this->m_Info->uint8x2.its(token.value)) 
			{
				token.length = 0x03;
			}
			else if (this->m_Info->uint8str.its(token.value)) 
			{
				token.length = 0x02;
				auto temp = this->m_Asmbin.data + offset + 0x02;
				do { 
					token.length++; 
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_Info->string.its(token.value) || this->m_Info->encstr.its(token.value))
			{
				auto temp = this->m_Asmbin.data + offset;
				do {
					token.length++;
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_Info->uint16x4.its(token.value)) 
			{
				token.length = 0x09;
			}
			else
			{
				throw std::exception
				{ 
					"script_view::token_parse -> unknown section of opcodes!" 
				};
			}
			offset += token.length;
			this->m_Tokens.push_back(token);
		}
	}

	auto script_view::raw() const -> script_view::view<uint8_t> 
	{
		return this->m_Raw;
	}

	auto script_view::asmbin() const -> script_view::view<uint8_t> 
	{
		return this->m_Asmbin;
	}

	auto script_view::info() const -> const script_info* 
	{
		return this->m_Info;
	}

	auto script_view::blocks() const -> script_view::view<int32_t>
	{
		return this->m_Blocks;
	}

	auto script_view::tokens() const -> const std::vector<token>&
	{
		return this->m_Tokens;
	}

	auto script_view::version() const -> uint16_t 
	{
		return this->m_Version;
	}

	script_helper::script_helper() {}

	script_helper::script_helper(std::string_view name) : m_Info(script_info::query(name)) {}

	script_helper::script_helper(script_info* info) : m_Info(info) {}

	script_helper::~script_helper() { this->m_Buffer.clear(); this->m_MesView = {}; }

	auto script_helper::read(std::string_view path, bool check) -> script_helper& 
	{
		this->m_MesView = {};

		if (path.empty())
		{
			return *this;
		}

		if (check && !utils::extension_check(path, ".mes"))
		{
			return *this;
		}

		utils::file file(path.data(), "rb");
		if (!file.is_open())
		{
			return *this;
		}

		file.seek(0, SEEK_END);
		auto length = static_cast<int32_t>(file.tell());
		if (length == errno) 
		{
			return *this;
		}

		int32_t n_size = length * 2;
		int32_t o_size = static_cast<int32_t>(this->m_Buffer.size());
		if (n_size > o_size) 
		{
			this->m_Buffer.clear();
			this->m_Buffer.resize(n_size);
		}

		size_t count = file.rewind().read(this->m_Buffer.data(), 1, length);
		if (count == length)
		{
			this->m_MesView = script_view
			{ 
				std::span{ this->m_Buffer.data(), size_t(length) }, 
				this->m_Info 
			};
		}

		this->m_Buffer.recount(length);

		file.close();

		return *this;
	}

	auto script_helper::fetch_scene_text(bool absolute_file_offset) const -> std::vector<script_helper::text> 
	{
		const auto&& info  { this->m_MesView.info() };
		const auto&& asmbin{ this->m_MesView.asmbin() };

		std::vector<script_helper::text> result{};
		int32_t base{ absolute_file_offset ? asmbin.offset : 0 };

		for (const script_view::token& token : this->m_MesView.tokens()) 
		{
			if (info->encstr.its(token.value)) 
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data + token.offset + 1) };
				for (char& ch : text) { ch += this->m_MesView.info()->deckey; } // 解密字符串
				result.push_back({ .offset = token.offset + base, .string = text });
			}
			else if (token.value != NULL && info->optunenc == token.value)
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data + token.offset + 1) };
				result.push_back({ .offset = token.offset + base, .string = text });
			}
		}
		return result;
	}

	auto script_helper::import_scene_text(const std::vector<std::pair<int32_t, std::string>>& texts, bool absolute_file_offset) -> bool 
	{
		auto&& _texts{ *reinterpret_cast<const std::vector<script_helper::text>*>(&texts) };
		return this->import_scene_text(_texts, absolute_file_offset);
	}

	auto script_helper::import_scene_text(const std::vector<script_helper::text>& texts, bool absolute_file_offset) -> bool 
	{
		if (texts.empty())
		{
			return { false };
		}

		if (this->m_MesView.tokens().empty()) 
		{
			return { false };
		}

		const auto&& info  { this->m_MesView.info() };
		const auto&& asmbin{ this->m_MesView.asmbin() };
		auto&& blocks{ this->m_MesView.blocks() };

		utils::xmem::buffer<uint8_t> buffer{};
		buffer.resize(this->m_Buffer.size());
		buffer.recount(asmbin.offset);

		int32_t block_count{ 0 };
		int32_t base{ absolute_file_offset ? asmbin.offset : 0 };

		for (const auto& token : this->m_MesView.tokens())
		{

			if (blocks.offset == 0x04 && block_count < blocks.size)
			{
				if (token.offset + 2 == blocks.data[block_count])
				{
					auto count  { static_cast<int32_t>(buffer.count()) };
					auto offset { count - asmbin.offset + 2 };
					auto&& block{ blocks.data[block_count] };
					block = { static_cast<int32_t>(offset) };
					block_count++;
				}
			}

			if (info->encstr.its(token.value))
			{
				
				auto&& it{ std::ranges::find(texts, token.offset + base, &text::offset) };

				if (it != texts.end())
				{
					std::string str{ it->string };
					for (char& chr : str)
					{
						chr -= this->m_MesView.info()->deckey; // 加密字符串
					}
					buffer.write(token.value).write(str).write('\0');
					continue;
				}
			}
			
			if (token.value != NULL && info->optunenc == token.value) 
			{
				auto&& it{ std::ranges::find(texts, token.offset + base, &text::offset) };
				if (it != texts.end())
				{
					buffer.write(token.value).write(it->string).write('\0');
					continue;
				}
			}

			if (blocks.offset == 0x08 && (token.value == 0x03 || token.value == 0x04 ))
			{
				if (block_count < blocks.size)
				{
					auto count  { static_cast<int32_t>(buffer.count()) };
					auto offset { count - asmbin.offset + token.length };
					auto&& block{ blocks.data[block_count]      };
					block = { (block & (0xFF << 0x18)) | offset };
					block_count++;
				}
			}

			buffer.write(asmbin.data + token.offset, token.length);
		}
		
		buffer.write(0, this->m_MesView.raw().data, asmbin.offset);
		
		std::span raw { buffer.data(), buffer.count() };
		this->m_MesView = script_view{ raw, this->m_Info };
		this->m_Buffer  = std::move(buffer);
		return true;
	}

	auto script_helper::get_view() -> const script_view& 
	{
		return this->m_MesView;
	}

	auto script_helper::is_parsed() -> bool 
	{
		return static_cast<bool>(this->m_MesView.tokens().size());
	}

	auto script_helper::set_info(const script_info* info) -> script_helper& 
	{
		this->m_Info = info;
		return *this;
	}

};