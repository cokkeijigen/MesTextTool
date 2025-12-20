#include <iostream>
#include <xfsys.hpp>
#include <xstr.hpp>
#include "config.hpp"
namespace mes
{
	auto config::config_file_exists(const std::string_view directory) -> bool
	{
		return xfsys::is_file(xfsys::path::join(directory, xstr::cvt::convert(config::k_name, 0)));
	}

	auto config::config_file_exists(const std::wstring_view directory) -> bool
	{
		return xfsys::is_file(xfsys::path::join(directory, config::k_name));
	}

	auto config::read(const xfsys::file& file,config& result) -> bool
	{
		if (!file.is_open())
		{
			return false;
		}

		xstr::u8string_buffer buffer{};
		const auto bytes_read{ file.read(buffer, file.size(), xfsys::file::pos::begin) };

		if (bytes_read == 0 || buffer.count() == 0)
		{
			return false;
		}

		int flag{ -1 };
		for (const std::u8string_view& line : xstr::line::iter(buffer, true))
		{

			if (line.empty())
			{
				continue;
			}

			if (line == reinterpret_cast<const char8_t*>(config::k_path))
			{
				flag = 1;
			}
			else if (line == reinterpret_cast<const char8_t*>(config::k_cdpg))
			{
				flag = 2;
			}
			else if (line == reinterpret_cast<const char8_t*>(config::k_tmin))
			{
				flag = 3;
			}
			else if (line == reinterpret_cast<const char8_t*>(config::k_tmax))
			{
				flag = 4;
			}
			else if (line == reinterpret_cast<const char8_t*>(config::k_bfrp))
			{
				flag = 5;
			}
			else if (line == reinterpret_cast<const char8_t*>(config::k_atrp))
			{
				flag = 6;
			}
			else if (flag == 1)
			{
				auto input_path{ const_cast<std::wstring*>(&result.input_path) };
				input_path->assign(xstr::trim(xstr::cvt::to_utf16(line)));
				if (!input_path->empty())
				{
					flag = 0;
					if (!input_path->ends_with(L"/") && !input_path->ends_with(L"\\"))
					{
						input_path->append(L"\\");
					}
				}
			}
			else if (flag > 1 && flag < 5)
			{
				int value{};
				int matched = std::sscanf(reinterpret_cast<const char*>(line.data()), "%d", &value);
				if (matched == 1)
				{
					if (flag == 2)
					{
						using value_t = std::decay_t<decltype(result.use_code_page)>;
						auto use_code_page{ const_cast<value_t*>(&result.use_code_page) };
						*use_code_page = static_cast<value_t>(value);
					}
					else if (flag == 3)
					{
						using value_t = std::decay_t<decltype(result.text_min_length)>;
						auto text_min_length{ const_cast<value_t*>(&result.text_min_length) };
						*text_min_length = static_cast<value_t>(value);
					}
					else
					{
						using value_t = std::decay_t<decltype(result.text_min_length)>;
						auto text_max_length{ const_cast<value_t*>(&result.text_max_length) };
						*text_max_length = static_cast<value_t>(value);
					}
					flag = 0;
				}
			}
			else if (flag == 5 || flag == 6)
			{
				if (!line.starts_with(u8"[") || !line.ends_with(u8"]"))
				{
					continue;
				}
				const size_t offset{ line.rfind(u8"]:[") };
				if (offset == std::u8string_view::npos || offset == 1)
				{
					continue;
				}

				std::u8string_view key{ line.substr(1, offset - 1) };
				if (key.empty()) { continue; }

				const size_t value_count{ line.size() - (offset + 4) };
				std::u8string_view value{ line.substr(offset + 3, value_count) };

				;
				auto pair{ std::make_pair(xstr::cvt::to_utf16(key), xstr::cvt::to_utf16(value)) };
				if (flag == 5)
				{
					auto before_replaces{ const_cast<vector_t*>(&result.before_replaces) };
					before_replaces->push_back(pair);
				}
				else
				{
					auto after_replaces{ const_cast<vector_t*>(&result.after_replaces) };
					after_replaces->push_back(pair);
				}
			}
		}

		return true;
	}

	auto config::create(const xfsys::file& file, const config& config) -> bool
	{
		if (!file.is_open())
		{
			return false;
		}
		
		xstr::u8string_buffer buffer{};

		std::string input_path{};
		if (!config.input_path.empty())
		{
			input_path.assign(xstr::cvt::to_utf8(config.input_path));
		}
		else 
		{
			input_path.assign("D:\\YourGames\\Name\\Advdata\\MES");
		}
		buffer.write_as_format("%s\n%s\n\n", config::k_path, input_path.data());
		buffer.write_as_format("%s\n%d\n\n", config::k_cdpg, config.use_code_page);
		buffer.write_as_format("%s\n%d\n\n", config::k_tmin, config.text_min_length);
		buffer.write_as_format("%s\n%d\n\n", config::k_tmax, config.text_max_length);

		buffer.write(reinterpret_cast<const char8_t*>(config::k_bfrp)).write('\n');
		if (!config.before_replaces.empty())
		{
			for (const auto& [key, value] : config.before_replaces)
			{
				buffer.write_as_format("[%s]:[%s]\n", key.data(), value.data());
			}
			buffer.write(u8"\n\n");
		}
		else 
		{
			buffer.write(u8"[]:[]\n\n");
		}

		buffer.write(reinterpret_cast<const char8_t*>(config::k_atrp)).write('\n');
		if (!config.after_replaces.empty())
		{
			for (const auto& [key, value] : config.after_replaces)
			{
				buffer.write_as_format("[%s]:[%s]\n", key.data(), value.data());
			}
			buffer.write(u8"\n\n");
		}
		else 
		{
			buffer.write(u8"[]:[]\n\n");
		}

		file.write(buffer, buffer.count(), xfsys::file::pos::begin);
		file.close();

		return true;
	}

	auto config::read(const std::string_view directory, config& result) -> bool
	{
		if (directory.empty())
		{
			return false;
		}

		const auto path{ xfsys::path::join(directory, xstr::cvt::to_utf8(config::k_name)) };
		return config::read(xfsys::open(path, xfsys::read, false), result);
	}

	auto config::read(const std::wstring_view directory, config& result) -> bool
	{
		if (directory.empty())
		{
			return false;
		}

		const auto path{ xfsys::path::join(directory, config::k_name) };
		return config::read(xfsys::open(path, xfsys::read, false), result);
	}

	auto config::read(const std::string_view directory) -> std::optional<config>
	{
		config result{};
		if (config::read(directory, result))
		{
			return result;
		}
		return std::nullopt;
	}

	auto config::read(const std::wstring_view directory) -> std::optional<config>
	{
		config result{};
		if (config::read(directory, result))
		{
			return result;
		}
		return std::nullopt;
	}

	auto config::create(const std::string_view directory) -> bool
	{
		const config config{};
		return config::create(directory, config);
	}

	auto config::create(const std::wstring_view directory) -> bool
	{
		const config config{};
		return config::create(directory, config);
	}

	auto config::create(const std::string_view directory, const config& config) -> bool
	{
		if (directory.empty())
		{
			return false;
		}

		const auto path{ xfsys::path::join(directory, xstr::cvt::to_utf8(config::k_name)) };
		return config::create(xfsys::create(path), config);
	}

	auto config::create(std::wstring_view directory, const config& config) -> bool
	{
		if (directory.empty())
		{
			return false;
		}
		
		const auto path{ xfsys::path::join(directory, config::k_name) };
		return config::create(xfsys::create(path), config);
	}
}