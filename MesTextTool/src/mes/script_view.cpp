#include <iostream>
#include "mes.hpp"

namespace mes 
{

	script_view::script_view(const std::span<uint8_t> raw, const script_info* const info)
		: m_raw{ raw, 0x00 }, m_info{ info }
	{
		if (!this->m_raw.data() || raw.empty())
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

		if (this->m_info->offset == script_info::offset1)
		{
			this->init_by_offset1();
		}
		else if (this->m_info->offset == script_info::offset2)
		{
			this->init_by_offset2();
		}
		else 
		{
			return;
		}

		this->token_parse();
	}

	auto script_view::init_by_offset1() noexcept -> void
	{
		const auto size{ static_cast<int32_t>(this->m_raw.size()) };
		const auto head{ reinterpret_cast<int32_t*>(this->m_raw.data()) };

		const auto offset{ static_cast<int32_t>(head[0] * 0x04 + 0x04) };
		if (size <= offset)
		{
			return;
		}

		const auto version_token_length
		{
			head[1] == 0x03 ? 0x03 :
			(this->m_info->version & 0xFF00) == 0x00 ?
			0x01 : 0x02
		};

		if (size <= offset + version_token_length)
		{
			return;
		}

		if (version_token_length == 0x01)
		{
			this->m_version = static_cast<uint16_t>(this->m_raw.data()[offset]);
		}
		else
		{
			this->m_version = *reinterpret_cast<uint16_t*>(this->m_raw.data() + offset);
		}

		this->m_asmbin = script_view::view_t<uint8_t>
		{
			std::span<uint8_t>
			{
				reinterpret_cast<uint8_t*>(this->m_raw.data() + offset + version_token_length),
				static_cast<size_t>(this->m_raw.size() - offset - version_token_length)
			},
			{ static_cast<int32_t>(offset + version_token_length) }
		};

		const auto labels_data_offset{ head[1] == 0x03 ? 0x08 : 0x04 };
		this->m_labels = script_view::view_t<int32_t>
		{
			std::span<int32_t>
			{
				reinterpret_cast<int32_t*>(this->m_raw.data() + labels_data_offset),
				static_cast<size_t>(head[0])
			},
			{ static_cast<int32_t>(labels_data_offset)   }
		};
	}

	auto script_view::init_by_offset2() noexcept -> void
	{
		const auto size{ static_cast<int32_t>(this->m_raw.size()) };
		const auto head{ reinterpret_cast<int32_t*>(this->m_raw.data()) };
		
		const auto offset{ static_cast<int32_t>(head[0] * 0x06 + 0x04) };
		if (size <= offset || size <= offset + 0x03)
		{
			return;
		}

		this->m_version = *reinterpret_cast<uint16_t*>(this->m_raw.data() + offset);
		this->m_asmbin = script_view::view_t<uint8_t>
		{
			std::span<uint8_t>
			{
				reinterpret_cast<uint8_t*>(this->m_raw.data() + offset + 0x03) ,
				static_cast<size_t>(this->m_raw.size() - offset - 0x03)
			},
			{ static_cast<int32_t>(offset + 0x03) }
		};

		this->m_labels = script_view::view_t<int32_t>
		{
			std::span<int32_t>
			{
				reinterpret_cast<int32_t*>(this->m_raw.data() + 0x08),
				static_cast<size_t>((offset - 4) / 4)
			},
			{ static_cast<int32_t>(0x08) }
		};
	}

	auto script_view::token_parse() noexcept -> void
	{
		this->m_tokens.clear();
		if (this->m_info == nullptr)
		{
			return;
		}

		for (size_t offset{}; offset < this->m_asmbin.size(); ) 
		{
			mes::token token
			{
				.data   = &this->m_asmbin[offset],
				.offset = static_cast<int32_t>(offset)
			};

			if (this->m_info->uint8x2.is(token.opcode()))
			{
				token.length = 0x03;
			}
			else if (this->m_info->uint8str.is(token.opcode()))
			{
				token.length  = 0x02;
				uint8_t* temp = this->m_asmbin.data() + offset + 0x02;
				do {
					token.length++;
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_info->string.is(token.opcode()) || this->m_info->encstr.is(token.opcode()))
			{
				uint8_t* temp = this->m_asmbin.data() + offset;
				do {
					token.length++;
					temp++;
				} while (*(temp - 1));
			}
			else if (this->m_info->uint16x4.is(token.opcode()))
			{
				token.length = 0x09;
			}
			else
			{
				this->m_tokens.clear();
				return;
			}

			offset += token.length;
			this->m_tokens.push_back(token);
		}
	}
}