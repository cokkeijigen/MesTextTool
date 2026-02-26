#include <iostream>
#include <xstr.hpp>
#include <algorithm>
#include <scripts_handler.hpp>

namespace mes::scripts
{

	scripts_handler::scripts_handler(std::wstring_view input_directory_or_file, std::wstring_view output_directory) noexcept :
		m_input_directory_or_file{ xstr::trim(input_directory_or_file) }, m_output_directory{ xstr::trim(output_directory) }
	{
	}

	scripts_handler::scripts_handler(std::u8string_view input_directory_or_file, std::u8string_view output_directory) noexcept :
		m_input_directory_or_file{ xstr::cvt::to_utf16(xstr::trim(input_directory_or_file)) },
		m_output_directory{ xstr::cvt::to_utf16(xstr::trim(output_directory)) }
	{
	}

	auto scripts_handler::export_text(const std::wstring_view file, std::vector<mes::script::union_info_t>& output_infos) const -> bool
	{
		if (!xfsys::extname_check(file, L".mes"))
		{
			return false;
		}

		if (!this->m_helper.load(file).is_parsed())
		{
			return false;
		}

		std::wstring output_directory{};
		{
			const mes::script::union_info_t info{ this->m_helper.data_view().info() };
			const std::string_view name{ info.name() };
			if (name.empty())
			{
				return false;
			}
			output_infos.push_back(info);
			const std::wstring u16name{ xstr::cvt::to_utf16(name).append(L"_text") };
			output_directory.assign(xfsys::path::join(this->m_output_directory, u16name));
		}
		
		if (!xfsys::create_directory(output_directory, true))
		{
			return false;
		}

		std::wstring output_file_path{};
		{
			std::wstring_view name{ xfsys::path::name(file) };
			const size_t dotpos{ name.find_last_of(L".") };
			if (dotpos != std::string::npos)
			{
				name = name.substr(0, dotpos);
			}
			output_file_path.assign(xfsys::path::join(output_directory, xstr::join(name, L".txt")));
		}

		const std::vector<mes::script::text_pair_t> texts{ this->m_helper.export_text() };
		const bool completed
		{
			mes::text::format_dump(output_file_path, texts, this->m_input_mes_code_page)
		};

		return completed;
	}

	auto scripts_handler::export_text_handle() const -> void
	{
		std::wstring_view input_path{};
		std::vector<mes::script::union_info_t> output_script_infos{};

		if (xfsys::is_directory(this->m_input_directory_or_file))
		{
			input_path = this->m_input_directory_or_file;
			for (const auto& entry : xfsys::dir::iter(this->m_input_directory_or_file))
			{
				if (!entry.is_file())
				{
					continue;
				}
				this->export_text(xstr::trim(entry.full_path()), output_script_infos);
			}
		}
		else if(xfsys::is_file(this->m_input_directory_or_file))
		{
			input_path = xfsys::path::parent(this->m_input_directory_or_file);
			this->export_text(this->m_input_directory_or_file, output_script_infos);
		}

		if (output_script_infos.empty())
		{
			return;
		}

		const mes::config config { .input_path{ input_path }};
		for (const mes::script::union_info_t& info : output_script_infos)
		{
			const std::wstring dirs{ xstr::cvt::to_utf16(xstr::join(info.name(), "_text")) };
			const std::wstring path{ xfsys::path::join(this->m_output_directory, dirs)     };
			const bool is_make_config{ mes::config::create(path, config) };
		}
	}

	auto scripts_handler::import_text_handle() const -> void
	{
		const auto config{ mes::config::read(this->m_input_directory_or_file) };
		if (!config.has_value())
		{
			if (this->m_logger)
			{
				constexpr wchar_t message[]{ L"Error! cannot read configuration file from:\n- " };
				this->m_logger(message_level::error, xstr::join(message, this->m_input_directory_or_file, L"\n"));
			}
			return;
		}

		if (!xfsys::is_directory(config->input_path))
		{
			if (this->m_logger)
			{
				constexpr wchar_t message[]{ L"Error! mes directory not exist:\n- " };
				this->m_logger(message_level::error, xstr::join(message, config->input_path, L"\n"));
			}
			return;
		}

		const mes::text::formater formater{ config.value() };
		for (const auto& entry : xfsys::dir::iter(this->m_input_directory_or_file))
		{
			if (entry.is_directory())
			{
				continue;
			}

			if (!xfsys::extname_check(entry.name(), L".txt"))
			{
				continue;
			}
			
			std::wstring mesname{};
			{
				auto name{ entry.name() };
				auto const dot_pos{ name.find_last_of(L".") };
				if (dot_pos != std::string::npos)
				{
					name = name.substr(0, dot_pos);
				}
				mesname.assign(xstr::join(name, L".mes"));
			}

			const auto path_mes{ xfsys::path::join(config->input_path, mesname) };
			if (!xfsys::is_file(path_mes))
			{
				if (this->m_logger)
				{
					constexpr wchar_t info[]{ L"Error! corresponding .mes file not found:\n- [y] " };
					const auto message{ xstr::join(info, entry.full_path(), L"\n- [n] ", path_mes, L"\n")};
					this->m_logger(message_level::error, message);
				}
				continue;
			}

			const auto path_txt{ entry.full_path() };
			const auto texts{ mes::text::parse_format(path_txt, formater) };

			if (texts.empty())
			{
				if (this->m_logger)
				{
					constexpr wchar_t info[]{ L"Error! texts is empty or cannot be parsed:\n- " };
					this->m_logger(message_level::error, xstr::join(info, path_txt, L"\n"));
				}
				continue;
			}

			if (!this->m_helper.load(path_mes).is_parsed())
			{
				continue;
			}

			const auto imported{ this->m_helper.import_text(texts) };
			if (!imported)
			{
				continue;
			}

			const auto dirs{ xstr::cvt::to_utf16(this->m_helper.get_info_name()).append(L"_mes")   };
			const auto path{ xfsys::path::join(this->m_output_directory, dirs) };
			if (!xfsys::create_directory(path))
			{
				continue;
			}
			
			const auto path_out{ xfsys::path::join(path, mesname) };
			const auto is_saved{ this->m_helper.save(path_out)    };

			if (is_saved && this->m_logger)
			{
				const auto message = xstr::join
				(
					L"Import successful:",
					L"\n- txt: ", path_txt,
					L"\n- raw: ", path_mes,
					L"\n- out: ", path_out,
					L"\n"
				);
				this->m_logger(message_level::normal, message);
			}
			else if(this->m_logger)
			{
				const auto message = xstr::join
				(
					L"Import failed, Unknown error:",
					L"\n- txt: ", path_txt,
					L"\n- raw: ", path_mes,
					L"\n"
				);
				this->m_logger(message_level::normal, message);
			}
		}
	}

	auto scripts_handler::set_script_info(const mes::script::helper::union_info_t info) noexcept -> scripts_handler&
	{
		this->m_helper.using_script_info(info);
		return *this;
	}

	auto scripts_handler::set_mes_code_page(const uint32_t code_page) noexcept -> scripts_handler&
	{
		this->m_input_mes_code_page = code_page;
		return *this;
	}

	auto scripts_handler::process() const -> time_t
	{
		const auto beg{ std::chrono::high_resolution_clock::now() };

		if (mes::config::config_file_exists(this->m_input_directory_or_file))
		{
			if (this->m_logger)
			{
				this->m_logger(message_level::normal, L"[SCRIPTS_HANDLER::PROCESS] IMPORT_TEXT_HANDLE\n");
			}

			this->import_text_handle();
		}
		else if(xfsys::is_exists(this->m_input_directory_or_file))
		{
			if (this->m_logger)
			{
				this->m_logger(message_level::normal, L"[SCRIPTS_HANDLER::PROCESS] EXPORT_TEXT_HANDLE\n");
			}

			this->export_text_handle();
		}
		else if (this->m_logger)
		{
			constexpr wchar_t info[]{ L"Error! input path does not exists:\n- " };
			const auto message{ xstr::join(info, this->m_input_directory_or_file, L"\n") };
			this->m_logger(message_level::error, message);
		}
		
		const auto end{ std::chrono::high_resolution_clock::now() };
		const auto dur{ std::chrono::duration_cast<std::chrono::microseconds>(end - beg) };
		return std::chrono::duration_cast<std::chrono::duration<time_t>>(dur).count();
	}

	auto scripts_handler::process(logger_t logger) const -> time_t
	{
		this->m_logger = logger;
		return this->process();
	}
}

