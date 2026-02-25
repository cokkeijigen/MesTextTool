#include <iostream>
#include <mes_advtxt.hpp>

namespace mes 
{
	const char* const advtxt_info::supports[]
	{
		"utaeho4", "infantaria", "suikademo"
	};

	const advtxt_info advtxt_info::infos[]
	{
		{ "advtxt"   , { 0x00       } },  // default
		{ "utaeho4"  , { 0x00, 0x1A } },
		{ "suikademo", { 0x00, 0x16 } }
	};

	auto advtxt_info::get(const std::string_view name) -> const advtxt_info*
	{
		static std::string temp_name{};
		static advtxt_info temp_info{ advtxt_info::infos[0] };

		if (name.empty())
		{
			return &advtxt_info::infos[0];
		}

		const bool is_supported = std::any_of
		(
			std::begin(advtxt_info::supports),
			std::end(advtxt_info::supports),
			[name](const char* s)
			{
				return s != nullptr && name == s;
			}
		);

		if (!is_supported)
		{
			return nullptr;
		}

		for (const auto& info : advtxt_info::infos)
		{
			if (info.name == name)
			{
				return &info;
			}
		}

		temp_name.assign(name);
		temp_info.name = temp_name.data();
		return &temp_info;
	}

	advtxt_view::advtxt_view(const std::span<uint8_t> raw) noexcept 
		: m_raw{ raw, 0x00 }
	{
		if (raw.size() < sizeof(advtxt_view::magic) || raw.data() == nullptr)
		{
			return;
		}

		const auto header{ reinterpret_cast<header_t*>(this->m_raw.data()) };
		if (std::memcmp(header->magic, advtxt_view::magic, sizeof(advtxt_view::magic)) != 0)
		{
			return;
		}

		const size_t offset{ (header->entry.count * 2) + sizeof(advtxt_view::magic) };
		this->m_asmbin = view_t<uint8_t>
		{
			std::span<uint8_t> { raw.data() + offset, raw.size() - offset },
			{ static_cast<int32_t>(offset) }
		};

		this->token_parse();
	}

	auto advtxt_view::token_parse() noexcept -> void
	{
		this->m_tokens.clear();

		if (this->m_raw.empty() || this->m_raw.data() == nullptr)
		{
			return;
		}

		size_t current{};
		while (true)
		{
			const size_t token_end{ this->m_asmbin.find(current, { 0x0A, 0x0D }) }; // 0A0D -> \n\r
			if (token_end == view_t<uint8_t>::nops)
			{
				break;
			}

			this->m_tokens.push_back
			(
				advtxt_view::token
				{
					.data   = this->m_asmbin.data() + current,
					.offset = static_cast<int32_t>(current),
					.length = static_cast<int32_t>(token_end - current)
				}
			);

			current = token_end + 2;

			if (current >= this->m_asmbin.size())
			{
				break;
			}
		}
	}

	auto is_advtxt(const std::span<const uint8_t> data) -> bool 
	{
		if (data.size() < sizeof(mes::advtxt_view::header_t) || data.data() == nullptr)
		{
			return false;
		}

		const auto header{ reinterpret_cast<const mes::advtxt_view::header_t*>(data.data()) };
		const auto result{ std::memcmp(header->magic, mes::advtxt_view::magic, sizeof(mes::advtxt_view::magic)) };
		return result == 0;
	}

	auto advtxt::decrypt_string(const std::span<const uint8_t> str) -> std::string
	{
		if (str.empty()) return {};

		std::string result{};
		result.reserve(str.size());

		for (size_t i = 0; i < str.size(); i += 2)
		{
			if (str[i] == 0x00)
			{
				break;
			}
			if (i + 1 < str.size())
			{
				result.push_back(static_cast<char>(str[i + 1]));
			}
			result.push_back(static_cast<char>(str[i]));
		}

		return result;
	}

	auto advtxt::string_parse(const view::token& token) -> std::string
	{
		if (token.data != nullptr && token.length > 1)
		{
			const std::span<const uint8_t> str
			{
				token.data + 1,
				static_cast<size_t>(token.length - 1)
			};
			return advtxt::decrypt_string(str);
		}
		return {};
	}
	
}