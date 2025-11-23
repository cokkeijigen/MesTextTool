#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ranges>
#include <algorithm> 
#include <mes.hpp>
#include <file.hpp>
#include <console.hpp>
#include <xmemory.hpp>
#include <xstr.hpp>

namespace mes 
{

	const script_info script_info::infos[]
	{  /*  name         offtype  version   uint8x2         uint8str        string          encstr          uint16x4      enckey  opstrs...         */
		{ "ffexa"     , offset1, 0x7B69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x43 }/*---------*/}, // fortissimo//Akkord:Bsusvier
		{ "ffexs"     , offset1, 0x7B6B, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x4B }, { 0x4c, 0x4F }, { 0x50, 0xFF },  0x20, { 0x35, 0x43, 0x45 } }, // fortissimo EXS//Akkord:nächsten Phase
		{ "ef"        , offset1, 0x466A, { 0x00, 0x28 }, { 0x2A, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x20, { 0x36, 0x46 }/*---*/}, // エターナルファンタジー Eternal Fantasy
		{ "dcxx"      , offset1, 0x005D, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x45 }, { 0x46, 0x49 }, { 0x4A, 0xFF },  0x20, { 0x42 }/*---------*/}, // D.C.〜ダ・カーポ〜　温泉編、MEMORIES
		{ "ktlep"     , offset1, 0x6E69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x45 }/*---------*/}, // ことり Love Ex P
		{ "dcws0"     , offset1, 0x005C, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x45 }, { 0x46, 0x49 }, { 0x4A, 0xFF },  0x20, { 0x42, 0x44 }/*---*/}, // D.C. White Season ~ダ・カーポ ホワイトシーズン~ クリスマス限定版
		{ "dcws"      , offset1, 0x656C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, { 0x46, 0x48 }/*---*/}, // D.C. White Season（D.C.～ダ・カーポ～アーカイブス　SAKURA Edition）
		{ "dcsv"      , offset1, 0x636C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, { 0x46 }/*---------*/}, // D.C. Summer Vacation～ダ・カーポ サマーバケーション～ 
		{ "dcpc"      , offset1, 0x3D63, { 0x00, 0x2C }, { 0xFF, 0xFF }, { 0x2D, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x44 }/*---------*/}, // Ｄ．Ｃ．Ｐ．Ｃ．(Vista)
		{ "dcdx"      , offset1, 0x7769, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x35, 0x45 }/*---*/}, // D.C.Dream X’mas～ダ・カーポ～ドリームクリスマス
		{ "dcas"      , offset1, 0x4E69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x43 }/*---------*/}, // D.C.A.S. 〜ダ・カーポ〜アフターシーズンズ
		{ "dcbs"      , offset1, 0x3166, { 0x00, 0x2E }, { 0xFF, 0xFF }, { 0x2F, 0x4B }, { 0x4C, 0x4F }, { 0x50, 0xFF },  0x20, { 0x48 }/*---------*/}, // D.C.II ～ダ・カーポII～ 春風のアルティメットバトル!
		{ "dc2fl"     , offset1, 0x9C69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x35, 0x45 }/*---*/}, // D.C.II Fall in Love ～ダ・カーポII～フォーリンラブ
		{ "dc2bs"     , offset1, 0x316C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, { 0x48 }/*---------*/}, // D.C.II ～ダ・カーポII～ 春風のアルティメットバトル! （D.C.～ダ・カーポ～アーカイブス　SAKURA Edition）
		{ "dc2dm"     , offset2, 0x9D72, { 0x00, 0x29 }, { 0x2A, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, { 0x44 }/*---------*/}, // D.C.II Dearest Marriage ～ダ・カーポII～ ディアレストマリッジ
		{ "dc2fy"     , offset1, 0x3866, { 0x00, 0x2E }, { 0xFF, 0xFF }, { 0x2F, 0x4B }, { 0x4C, 0x4F }, { 0x50, 0xFF },  0x20, { 0x48 }/*---------*/}, // D.C. II ~Featuring Yun2!~
		{ "dc2cckko"  , offset1, 0x026C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, {/*——*/}/*---------*/}, // D.C.II Character Collection ～ダ・カーポII～ キャラクターコレクション 月島小恋のらぶらぶバスルーム
		{ "dc2cckko"  , offset1, 0xA96C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, {/*——*/}/*---------*/}, // D.C.II Character Collection ～ダ・カーポII～ キャラクターコレクション 月島小恋のらぶらぶバスルーム
		{ "dc2ccotm"  , offset1, 0x016C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, {/*——*/}/*---------*/}, // D.C.II Character Collection ～ダ・カーポII～ キャラクターコレクション 音姫先生のどきどき特別授業
		{ "dc2ccotm"  , offset1, 0xA86C, { 0x00, 0x2B }, { 0x2C, 0x31 }, { 0x32, 0x4C }, { 0x4D, 0x50 }, { 0x51, 0xFF },  0x20, {/*——*/}/*---------*/}, // D.C.II Character Collection ～ダ・カーポII～ キャラクターコレクション 音姫先生のどきどき特別授業
		{ "dc2sc"     , offset1, 0x3B69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.II Spring Celebration ～ダ・カーポII～ スプリング セレブレイション
		{ "dc2ty"     , offset1, 0x5F69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.II To You ～ダ・カーポII～トゥーユー サイドエピソード
		{ "dc2pc"     , offset1, 0x5769, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.II P.C.～ダ・カーポII～
		{ "dc2pc"     , offset1, 0x9969, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.II P.C.～ダ・カーポII～
		{ "dc3rx"     , offset2, 0x9772, { 0x00, 0x2B }, { 0x2C, 0x33 }, { 0x34, 0x4E }, { 0x4F, 0x52 }, { 0x53, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.III R ～ダ・カーポIIIアール～X-rated
		{ "dc3pp"     , offset2, 0x9872, { 0x00, 0x2A }, { 0x2B, 0x32 }, { 0x33, 0x4E }, { 0x4F, 0x51 }, { 0x52, 0xFF },  0x20, { 0x45 }/*---------*/}, // D.C.III P.P. ～ダ・カーポIII プラチナパートナー～
		{ "dc3wy"     , offset2, 0xA09F, { 0x00, 0x38 }, { 0x39, 0x41 }, { 0x42, 0x5F }, { 0x60, 0x63 }, { 0x64, 0xFF },  0x20, { 0x55 }/*---------*/}, // D.C.III With You ～ダ・カーポIII～ ウィズユー
		{ "dc3dd"     , offset2, 0xA5A8, { 0x00, 0x38 }, { 0x39, 0x43 }, { 0x44, 0x62 }, { 0x63, 0x67 }, { 0x68, 0xFF },  0x20, { 0x58 }/*---------*/}, // D.C.Ⅲ DreamDays～ダ・カーポⅢ～ドリームデイズ
		{ "dc4"       , offset2, 0xAAB6, { 0x00, 0x3A }, { 0x3B, 0x47 }, { 0x48, 0x68 }, { 0x69, 0x6D }, { 0x6E, 0xFF },  0x20, { 0x5D }/*---------*/}, // D.C.4 ～ダ・カーポ4～
		{ "dc4ph"     , offset2, 0xABB6, { 0x00, 0x3A }, { 0x3B, 0x47 }, { 0x48, 0x68 }, { 0x69, 0x6D }, { 0x6E, 0xFF },  0x20, { 0x5D }/*---------*/}, // D.C.4 Plus Harmony ～ダ・カーポ4～ プラスハーモニー 
		{ "ds"        , offset2, 0x9F9A, { 0x00, 0x38 }, { 0x39, 0x4A }, { 0x41, 0x5E }, { 0x5F, 0x62 }, { 0x63, 0xFF },  0x20, { 0x54 }/*---------*/}, // D.S. -Dal Segno-
		{ "dsif"      , offset2, 0xA1A1, { 0x00, 0x39 }, { 0x3A, 0x42 }, { 0x43, 0x60 }, { 0x61, 0x64 }, { 0x65, 0xFF },  0x20, { 0x56 }/*---------*/}, // D.S. i.F. -Dal Segno- in Future
		{ "tmpl"      , offset2, 0xA6B4, { 0x00, 0x3B }, { 0x3A, 0x46 }, { 0x46, 0x67 }, { 0x68, 0x6E }, { 0x6D, 0xFF },  0x20, { 0x5C }/*---------*/}, // てんぷれっ!!
		{ "nightshade", offset2, 0x0871, { 0x00, 0x2B }, { 0x2C, 0x33 }, { 0x34, 0x4E }, { 0x4F, 0x52 }, { 0x53, 0xFF },  0x01, { 0x43 }/*---------*/}, // 百花百狼 戦国忍法帖
		{ "puripa"    , offset1, 0x5B6C, { 0x00, 0x28 }, { 0x29, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x20, { 0x44, 0x46 }/*---*/}, // Princess Party ～プリンセスパーティー～
		{ "uni"       , offset1, 0x746A, { 0x00, 0x29 }, { 0x2A, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x20, { 0x36, 0x46 }/*---*/}, // ユニ uni.
		{ "homemaid"  , offset1, 0x1A63, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x48 }, { 0x49, 0x4C }, { 0x4D, 0xFF },  0x20, { 0x31 }/*---------*/}, // ホームメイド -Home maid-
		{ "hmsw"      , offset1, 0x4F69, { 0x00, 0x28 }, { 0x29, 0x2E }, { 0x2F, 0x49 }, { 0x4A, 0x4D }, { 0x4E, 0xFF },  0x20, { 0x35, 0x45 }/*---*/}, // ホームメイド スイーツ
		{ "ag2dc"     , offset1, 0x7C6A, { 0x00, 0x29 }, { 0x2A, 0x2F }, { 0x30, 0x4A }, { 0x4B, 0x4E }, { 0x4F, 0xFF },  0x20, { 0x36, 0x46 }/*---*/}, // A.G.II.D.C.　～あるぴじ学園2.0　サーカス史上最大の危機！？～
		{ "suika"     , offset1, 0x005F, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x46 }, { 0x47, 0x4A }, { 0x4B, 0xFF },  0x20, { 0x42, 0x44 }/*---*/}, // 水夏 ~SUIKA~全年齢版、水夏 ~SUIKAおー・157章~
		{ "suikaas+"  , offset1, 0x2263, { 0x00, 0x2B }, { 0xFF, 0xFF }, { 0x2C, 0x48 }, { 0x49, 0x4C }, { 0x4D, 0xFF },  0x20, { 0x45 }/*---------*/}, // 水夏A.S+ ～アズプラス～
		{ "suika2"    , offset1, 0x8279, { 0x00, 0x2E }, { 0x2F, 0x36 }, { 0x37, 0x54 }, { 0x55, 0x57 }, { 0x58, 0xFF },  0x20, { 0x4C }/*---------*/}, // 水夏弐律
	};

	inline auto script_info::section::its(uint8_t key) const -> bool
	{
		return !(beg == end && beg == 0xFF) && (key >= beg && key <= end);
	}

	auto script_info::query(std::span<uint8_t> data) -> const script_info*
	{

		if (!data.data() || data.empty())
		{
			return nullptr;
		}

		const auto head{ reinterpret_cast<int32_t*>(data.data()) };
		const auto size{ data.size() };

		uint16_t version1{}, version2{};
		if (size > 4)
		{
			auto offset1{ head[0] * 0x04 + 0x04 };
			auto offset2{ head[0] * 0x06 + 0x04 };
			
			if (size > offset1 + 0x02) 
			{
				version1 = *reinterpret_cast<uint16_t*>(data.data() + offset1);
			}
			if (size > offset2 + 0x02)
			{
				version2 = *reinterpret_cast<uint16_t*>(data.data() + offset2);
			}
		}

		if (version1 == 0x00 && version2 == 0x00)
		{
			return nullptr;
		}

		for (const auto& info : script_info::infos)
		{
			// 减少出错概率，head[1]为0x03优先匹配version2
			if (head[1] == 0x03 && version2 != 0x00)
			{
				const auto matched
				{
					info.offset == script_info::offset2 &&
					info.version == version2
				};
				if (matched)
				{
					return &info;
				}
			}

			const auto is_ver1_matched
			{
				version1 != 0x00 &&
				info.offset == script_info::offset1 &&
				info.version == version1
			};

			if (is_ver1_matched)
			{
				return &info;
			}

			const auto is_ver2_matched
			{
				version2 != 0x00 &&
				info.offset == script_info::offset2 &&
				info.version == version2
			};

			if (is_ver2_matched)
			{
				return &info;
			}

			auto is_abyte_ver1_matched
			{
				head[1] != 0x03 && 
				info.offset == script_info::offset1 &&
				(info.version & 0xFF00) == 0x00 &&
				(version1 & 0xFF) == info.version
			};

			if (is_abyte_ver1_matched)
			{
				return &info;
			}
		}

		return nullptr;
	}

	auto script_info::query(std::string_view name) -> const script_info*
	{
		if (!name.empty()) 
		{
			for (const auto& info : script_info::infos)
			{
				if (name == info.name) return &info;
			}
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
		m_raw(view<uint8_t>{ raw.data(), static_cast<int32_t>(raw.size()), 0 }), m_info(info) 
	{
		if (!raw.data() || raw.empty()) 
		{
			return;
		}
		
		if (this->m_info == nullptr)
		{
			this->m_info = script_info::query(raw);
			if (this->m_info == nullptr)
			{
				return;
			}
		}

		const auto size{ static_cast<int32_t>(raw.size()) };
		const auto head{ reinterpret_cast<int32_t*>(raw.data()) };

		if (this->m_info->offset == script_info::offset1)
		{
			const auto offset{ static_cast<int32_t>(head[0] * 0x04 + 0x04) };
			if (size > offset)
			{
				const auto base
				{ 
					head[1] == 0x03 ? 0x03 : 
					(this->m_info->version & 0xFF00) == 0x00 ?
					0x01 : 0x02 
				};
				if (size > offset + base)
				{
					if (base == 0x01)
					{
						this->m_version = static_cast<uint16_t>(raw.data()[offset]);
					}
					else 
					{
						this->m_version = *reinterpret_cast<uint16_t*>(raw.data() + offset);
					}
					this->m_asmbin  = script_view::view<uint8_t>
					{
						.data = raw.data() + offset + base,
						.size = static_cast<int32_t>(raw.size() - offset - base),
						.offset = { offset + base }
					};
				}
				{
					const auto base{ head[1] == 0x03 ? 0x08 : 0x04 };
					this->m_labels = script_view::view<int32_t>
					{
						.data = reinterpret_cast<int32_t*>(raw.data() + base),
						.size = static_cast<int32_t>(head[0]),
						.offset = base
					};
				}
			}
		}
		else if (this->m_info->offset == script_info::offset2)
		{
			const auto offset{ static_cast<int32_t>(head[0] * 0x06 + 0x04) };
			if (size > offset)
			{
				if (size > offset + 0x03)
				{
					this->m_version = *reinterpret_cast<uint16_t*>(raw.data() + offset);
					this->m_asmbin  = script_view::view<uint8_t>
					{
						.data = reinterpret_cast<uint8_t*>(raw.data() + offset + 0x03),
						.size = static_cast<int32_t>(raw.size() - offset - 0x03),
						.offset = { offset + 0x03 }
					};
				}
				this->m_labels = script_view::view<int32_t>
				{
					.data = reinterpret_cast<int32_t*>(raw.data() + 0x08),
					.size = static_cast<int32_t>((offset - 4) / 4),
					.offset = 0x08
				};
			}
		}

		this->token_parse();
	}

	auto script_view::token_parse() -> void 
	{
		this->m_tokens.clear();
		if (nullptr == this->m_info) { return; }
		
		for (int32_t offset{ 0 }; offset < this->m_asmbin.size; )
		{
			script_view::token token 
			{ 
				.offset = offset, 
				.value = this->m_asmbin.data[offset] 
			};

			if (this->m_info->uint8x2.its(token.value)) 
			{
				token.length = 0x03;
			}
			else if (this->m_info->uint8str.its(token.value)) 
			{
				token.length = 0x02;
				auto temp = this->m_asmbin.data + offset + 0x02;
				do { 
					token.length++; 
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_info->string.its(token.value) || this->m_info->encstr.its(token.value))
			{
				auto temp = this->m_asmbin.data + offset;
				do {
					token.length++;
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_info->uint16x4.its(token.value)) 
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
			this->m_tokens.push_back(token);
		}
	}

	auto script_view::raw() const -> script_view::view<uint8_t> 
	{
		return this->m_raw;
	}

	auto script_view::asmbin() const -> script_view::view<uint8_t> 
	{
		return this->m_asmbin;
	}

	auto script_view::info() const -> const script_info* 
	{
		return this->m_info;
	}

	auto script_view::labels() const -> script_view::view<int32_t>
	{
		return this->m_labels;
	}

	auto script_view::tokens() const -> const std::vector<token>&
	{
		return this->m_tokens;
	}

	auto script_view::version() const -> uint16_t 
	{
		return this->m_version;
	}

	script_helper::script_helper() {}

	script_helper::script_helper(std::string_view name) : m_info(script_info::query(name)) {}

	script_helper::script_helper(script_info* info) : m_info(info) {}

	script_helper::~script_helper() { this->m_buffer.clear(); this->m_mesview = {}; }

	auto script_helper::read(std::string_view path, bool check) -> script_helper& 
	{
		this->m_mesview = {};

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
		const auto length{ static_cast<int32_t>(file.tell()) };
		if (length == errno) 
		{
			return *this;
		}

		const int32_t n_size{ length * 2 };
		const int32_t o_size{ static_cast<int32_t>(this->m_buffer.size()) };
		if (n_size > o_size) 
		{
			this->m_buffer.clear();
			this->m_buffer.resize(n_size);
		}

		const size_t count{ file.rewind().read(this->m_buffer.data(), 1, length) };
		if (count == length)
		{
			this->m_mesview = script_view
			{ 
				std::span{ this->m_buffer.data(), size_t(length) }, 
				this->m_info 
			};
		}

		this->m_buffer.recount(length);

		file.close();

		return *this;
	}

	static auto __test_text__(utils::string::buffer text,  int opcode, uint32_t cdpg) -> void
	{
		auto wstr{ text.wstring_buffer(cdpg) };
		for (auto& achar : wstr) 
		{
			if (static_cast<uint16_t>(achar) >= 125)
			{
				xcsl::helper.write(L"[0x%02X]%ls\n", opcode, wstr.data());
				break;
			}
		}
	}

	auto script_helper::fetch_scene_text(bool absolute_file_offset) const -> std::vector<script_helper::text> 
	{
		const auto&& info  { this->m_mesview.info() };
		const auto&& asmbin{ this->m_mesview.asmbin() };

		std::vector<script_helper::text> result{};
		const int32_t base{ absolute_file_offset ? asmbin.offset : 0 };
		for (const script_view::token& token : this->m_mesview.tokens()) 
		{
			if (info->string.its(token.value))
			{
				//if(token.value != 0x46)
				__test_text__({ reinterpret_cast<char*>(asmbin.data + token.offset + 1) }, int(token.value), xcsl::cdpg::sjis);
				//__test_text__({ reinterpret_cast<char*>(asmbin.data + token.offset + 1) }, int(token.value), xcsl::cdpg::gbk);
			}

			if (info->encstr.its(token.value))
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data + token.offset + 1) };
				for (char& ch : text) { ch += this->m_mesview.info()->enckey; } // 解密字符串
				result.push_back({ .offset = token.offset + base, .string = text });
				for (char& ch : text) 
				{
					if (ch == '\n') ch = ' ';
					//ch = this->m_MesView.info()->enckey; 
				} 
				/*xcsl::helper.set_cp(xcsl::cdpg::sjis);
				xcsl::helper.write("[0x%02X:0x%X]%s\n", int(token.value), int(token.offset + base), text.data());
				xcsl::helper.reset_cp();*/
			}
			else if (token.value != 0x00 && std::ranges::contains(info->opstrs, token.value))
			{
				std::string text{ reinterpret_cast<char*>(asmbin.data + token.offset + 1) };
				result.push_back({ .offset = token.offset + base, .string = text });
				/*xcsl::helper.set_cp(xcsl::cdpg::sjis);
				xcsl::helper.write("[0x%02X]%s\n", int(token.value), text.data());
				xcsl::helper.reset_cp();*/
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

		if (this->m_mesview.tokens().empty()) 
		{
			return { false };
		}

		const auto&& info  { this->m_mesview.info() };
		const auto&& asmbin{ this->m_mesview.asmbin() };
		auto&& labels{ this->m_mesview.labels() };

		utils::xmem::buffer<uint8_t> buffer{};
		buffer.resize(this->m_buffer.size());
		buffer.recount(asmbin.offset);

		int32_t label_count{ 0 };
		const int32_t base{ absolute_file_offset ? asmbin.offset : 0 };
		for (const auto& token : this->m_mesview.tokens())
		{
			if (labels.offset == 0x04 && label_count < labels.size)
			{
				const auto first_token_length
				{ 
					labels.offset == 0x08 ? 0x03 :
					(info->version & 0xFF00) == 0x00 ?
					0x01 : 0x02
				};
				if (token.offset + first_token_length == labels.data[label_count])
				{
					auto count { static_cast<int32_t>(buffer.count()) };
					auto offset{ count - asmbin.offset + first_token_length };
					auto&& block{ labels.data[label_count] };
					block = { static_cast<int32_t>(offset) };
					label_count++;
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
						chr -= info->enckey; // 加密字符串
					}
					buffer.write(token.value).write(str).write('\0');
					continue;
				}
			}
			
			if (token.value != 0x00 && std::ranges::contains(info->opstrs, token.value))
			{
				auto&& it{ std::ranges::find(texts, token.offset + base, &text::offset) };
				if (it != texts.end())
				{
					buffer.write(token.value).write(it->string).write('\0');
					continue;
				}
			}

			if (labels.offset == 0x08 && (token.value == 0x03 || token.value == 0x04))
			{
				if (label_count < labels.size)
				{
					auto count  { static_cast<int32_t>(buffer.count()) };
					auto offset { count - asmbin.offset + token.length };
					auto&& block{ labels.data[label_count]      };
					block = { (block & (0xFF << 0x18)) | offset };
					label_count++;
				}
			}

			buffer.write(asmbin.data + token.offset, token.length);
		}
		
		buffer.write(0, this->m_mesview.raw().data, asmbin.offset);
		
		std::span raw { buffer.data(), buffer.count() };
		this->m_mesview = script_view{ raw, this->m_info };
		this->m_buffer  = std::move(buffer);
		return true;
	}

	auto script_helper::get_view() -> const script_view& 
	{
		return this->m_mesview;
	}

	auto script_helper::is_parsed() -> bool 
	{
		return static_cast<bool>(this->m_mesview.tokens().size());
	}

	auto script_helper::set_info(const script_info* info) -> script_helper& 
	{
		this->m_info = info;
		return *this;
	}

};