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

	auto scripts_handler::export_text(const std::wstring_view file, std::vector<mes::unioninfo>& output_infos) const -> bool
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
			const mes::unioninfo info{ this->m_helper.data_view().info() };
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

		const std::vector<mes::text::entry> texts{ this->m_helper.export_text() };
		const bool completed
		{
			mes::text::format_dump(output_file_path, texts, this->m_input_mes_code_page)
		};

		return completed;
	}

	auto scripts_handler::export_text_handle() const -> void
	{
		std::wstring_view input_path{};
		std::vector<mes::unioninfo> output_script_infos{};

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
		for (const mes::unioninfo& info : output_script_infos)
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
				const xstr::str msg
				{
					L"Error! Failed to read the configuration file from:\n- ",
					this->m_input_directory_or_file,
					L"\n"
				};
				this->m_logger(message_level::error, msg);
			}
			return;
		}

		if (!xfsys::is_directory(config->input_path))
		{
			if (this->m_logger)
			{
				const xstr::str msg
				{
					L"The directory for .mes files does not exist:\n- ",
					config->input_path,
					L"\n"
				};
				this->m_logger(message_level::error, msg);
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

			const std::wstring mesname{ xfsys::extname_change(entry.name(), L".mes")   };
			const std::wstring mespath{ xfsys::path::join(config->input_path, mesname) };
			if (!xfsys::is_file(mespath))
			{
				continue;
			}

			if (!this->m_helper.load(mespath).is_parsed())
			{
				continue;
			}

			const bool entry_wstring{ this->m_helper.data_view().type() == unionmes_view::advtxt_type };
			const std::vector<text::entry> texts { text::parse_format(entry.full_path(), formater, entry_wstring) };

			if (texts.empty())
			{
				continue;
			}

			const auto imported{ this->m_helper.import_text(texts, config->use_code_page, true) };
			if (!imported)
			{
				continue;
			}

			const std::wstring dirs{ xstr::cvt::to_utf16(this->m_helper.last_info_name()).append(L"_mes")   };
			const std::wstring path{ xfsys::path::join(this->m_output_directory, dirs) };
			if (!xfsys::create_directory(path))
			{
				continue;
			}
			
			const std::wstring path_out{ xfsys::path::join(path, mesname) };
			const bool is_saved{ this->m_helper.save(path_out)    };
		}
	}

	auto scripts_handler::set_script_info(const mes::unioninfo info) noexcept -> scripts_handler&
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
			const xstr::str msg
			{
				L"Error! The input path might not exist:\n- ",
				this->m_input_directory_or_file,
				L"\n"
			};
			this->m_logger(message_level::error, msg);
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

