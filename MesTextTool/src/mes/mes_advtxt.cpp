#include <iostream>
#include <mes_advtxt.hpp>

namespace mes 
{

	const char* const advtxt_info::advtxt_supports[]
	{
		"utaeho4", "infantaria", "suikademo"
	};

	std::vector<advtxt_info> advtxt_info::advtxt_infos
	{
		{ "advtxt"   , { 0x00       } },  // default
		{ "utaeho4"  , { 0x00, 0x1A } },
		{ "suikademo", { 0x00, 0x16 } }
	};

	auto advtxt_info::supports() -> const std::span<const char* const>
	{
		return std::span<const char* const>{ std::begin(advtxt_supports), std::end(advtxt_supports) };
	}

	auto advtxt_info::infos() -> const std::vector<advtxt_info>&
	{
		return advtxt_info::advtxt_infos;
	}

	auto advtxt_info::get(const std::string_view name) -> const advtxt_info*
	{
		if (name.empty())
		{
			return &advtxt_info::advtxt_infos[0];
		}

		for (const auto& info : advtxt_info::advtxt_infos)
		{
			if (info.name == name)
			{
				return &info;
			}
		}

		const bool is_supported = std::any_of
		(
			std::begin(advtxt_info::advtxt_supports),
			std::end(advtxt_info::advtxt_supports),
			[name](const char* s)
			{
				return s != nullptr && name == s;
			}
		);

		if (!is_supported)
		{
			return nullptr;
		}

		advtxt_infos.push_back(advtxt_info
		{
			.name    = std::string{ name },
			.encstrs = advtxt_info::advtxt_infos[0].encstrs
		});

		return &advtxt_infos.back();
	}

	auto advtxt_info::make(std::string_view name, std::vector<uint8_t>&& encstrs) -> const advtxt_info*
	{

		for (const auto& info : advtxt_info::advtxt_infos)
		{
			if (info.name != name)
			{
				continue;
			}
			std::vector<uint8_t>& encstrs{ *const_cast<std::vector<uint8_t>*>(&info.encstrs) };
			encstrs = std::move(encstrs);
			return &info;
		}

		advtxt_infos.push_back(advtxt_info
		{
			.name    = std::string{ name },
			.encstrs = std::move(encstrs)
		});

		return &advtxt_infos.back();
	}

	advtxt_view::advtxt_view(const std::span<uint8_t> raw, const advtxt_info* info) noexcept
		: m_raw{ raw, 0x00 }, m_info{ info }
	{
		if (raw.size() < sizeof(mes::advtxt::magic) || raw.data() == nullptr)
		{
			return;
		}

		const auto header{ reinterpret_cast<header_t*>(this->m_raw.data()) };
		if (std::memcmp(header->magic, mes::advtxt::magic, sizeof(mes::advtxt::magic)) != 0)
		{
			return;
		}

		const size_t offset{ (header->entries.count * 2) + sizeof(mes::advtxt::magic) };
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
			const size_t token_end{ this->m_asmbin.find(current, mes::advtxt::endtoken) }; // 0A0D -> \n\r
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
		const auto result{ std::memcmp(header->magic, mes::advtxt::magic, sizeof(mes::advtxt::magic)) };
		return result == 0;
	}

	auto advtxt::string_encdec(const std::span<const uint8_t> str) -> std::string
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

	auto advtxt::string_encdec(const std::string_view str) -> std::string
	{
		return advtxt::string_encdec(std::span
		{ 
			reinterpret_cast<const uint8_t*>(str.data()), 
			str.size() 
		});
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
			return advtxt::string_encdec(str);
		}
		return {};
	}
	
}