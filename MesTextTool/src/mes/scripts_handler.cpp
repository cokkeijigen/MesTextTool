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

	auto scripts_handler::warning_because_file_not(const std::wstring_view path, const std::wstring_view 
		type) const noexcept -> void
	{
		if (!this->m_logger)
		{
			return;
		}

		const auto message{ xstr::join(L"Skipped, because it is not a ", type, L"file:\n- ", path, L"\n") };
		this->m_logger(message_level::warning, message);
	}

	auto scripts_handler::warning_because_directory(const std::wstring_view path) const noexcept -> void
	{
		if (!this->m_logger)
		{
			return;
		}

		constexpr wchar_t message[]{ L"Skipped, because it is a directory:\n- " };
		this->m_logger(message_level::warning, xstr::join(message, path, L"\n"));
	}

	auto scripts_handler::error_mes_failed_to_parse(const std::wstring_view mes) const noexcept -> void
	{
		if (!this->m_logger)
		{
			return;
		}

		const auto info{ this->m_helper.script_view().info() };
		if (info == nullptr)
		{
			constexpr wchar_t message[]
			{
				L"Failed to parse .mes file, unknown version."
				"Please try specifying game name manually!\n- "
			};
			this->m_logger(message_level::error, xstr::join(message, mes, L"\n"));
		}
		else if (this->m_helper.script_info() != info)
		{
			const auto message = xstr::join
			(
				L"Failed to parse .mes file by \"",
				xstr::cvt::to_utf16(info->name),
				L"(auto-select)\", Please try specifying game name manually!\n- ",
				mes, L"\n"
			);
			this->m_logger(message_level::error, message);
		}
		else
		{
			constexpr wchar_t message[]
			{
				L"Failed to parse .mes file, Unknown error, "
				"Please retry specifying game name manually!\n- "
			};
			this->m_logger(message_level::error, xstr::join(message, mes, L"\n"));
		}
	}

	auto scripts_handler::error_create_outdir_failed(const std::wstring_view path, const std::wstring_view name1,
		const std::wstring_view name2) const noexcept -> void
	{
		if (!this->m_logger)
		{
			return;
		}

		const auto message = xstr::join
		(
			L"Failed to create output directory:", L"\n- ", path, 
			L"\n- ", name1, L" <=> ", name2, L"\n"
		);

		this->m_logger(message_level::error, message);
	}

	auto scripts_handler::export_text(const std::wstring_view file, std::vector<const script_info*>& out_infos) const -> bool
	{
		if (!xfsys::extname_check(file, L".mes"))
		{
			if (this->m_logger)
			{
				this->warning_because_file_not(file, L"mes");
			}

			return false;
		}

		if (!this->m_helper.load(file).is_parsed())
		{
			if (this->m_logger) { this->error_mes_failed_to_parse(file); }
			return false;
		}

		const auto texts{ this->m_helper.export_text() };
		const auto info { this->m_helper.script_view().info() };
		const auto dirs { xstr::cvt::to_utf16(info->name).append(L"_text") };
		const auto path { xfsys::path::join(this->m_output_directory, dirs) };

		std::wstring txtname{};
		{
			auto name{ xfsys::path::name(file) };
			auto const dot_pos{ name.find_last_of(L".") };
			if (dot_pos != std::string::npos)
			{
				name = name.substr(0, dot_pos);
			}
			txtname.assign(xstr::join(name, L".txt"));
		}

		if (!xfsys::create_directory(path, true))
		{
			if (this->m_logger)
			{
				this->error_create_outdir_failed(path, xfsys::path::name(file), txtname);
			}
			return false;
		}

		const auto output_file{ xfsys::path::join(path, txtname) };
		const auto result{ mes::text::format_dump(output_file, texts, this->m_input_mes_code_page) };

		if (this->m_logger)
		{
			const auto desc   { result ? L"successful:" : L"failed, Unknown error:" };
			const auto message{ xstr::join(L"Export ", desc, L"\n- raw: ", file, L"\n- out: ", output_file, L"\n") };
			this->m_logger(result ? message_level::normal : message_level::error, message);
		}

		auto script_info{ this->m_helper.script_view().info() };
		if (!std::ranges::contains(out_infos, script_info))
		{
			out_infos.push_back(script_info);
		}

		return result;
	}

	auto scripts_handler::export_text_handle() const -> void
	{
		std::wstring_view input_path{};
		std::vector<const script_info*> output_script_infos{};

		if (xfsys::is_directory(this->m_input_directory_or_file))
		{
			input_path = this->m_input_directory_or_file;
			for (const auto& entry : xfsys::dir::iter(this->m_input_directory_or_file))
			{
				if (entry.is_file())
				{
					this->export_text(xstr::trim(entry.full_path()), output_script_infos);
					continue;
				}

				if (this->m_logger) 
				{
					this->warning_because_directory(entry.full_path());
				}
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
		for (const auto& info : output_script_infos)
		{
			const auto dirs{ xstr::cvt::to_utf16(xstr::join(info->name, "_text")) };
			const auto path{ xfsys::path::join(this->m_output_directory, dirs)};
			const auto make_config{ mes::config::create(path, config) };
			if (!this->m_logger)
			{
				continue;
			}

			const auto config_path{ xfsys::path::join(path, mes::config::defuat_name()) };
			if (make_config)
			{
				constexpr wchar_t message[]{ L"Created configuration file:\n- " };
				this->m_logger(message_level::normal, xstr::join(message, config_path, L"\n"));
			}
			else
			{
				constexpr wchar_t message[]{ L"Failed to create configuration file:\n- " };
				this->m_logger(message_level::warning, xstr::join(message, config_path, L"\n"));
			}
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
				if (this->m_logger)
				{
					this->warning_because_directory(entry.full_path());
				}
				continue;
			}

			if (!xfsys::extname_check(entry.name(), L".txt"))
			{
				if (this->m_logger)
				{
					this->warning_because_file_not(entry.full_path(), L"txt");
				}
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
				if (this->m_logger) { this->error_mes_failed_to_parse(path_mes); }
				continue;
			}

			const auto imported{ this->m_helper.import_text(texts) };
			if (!imported)
			{
				if (this->m_logger)
				{
					constexpr wchar_t message[]{ L"Failed to import text into .mes file!\n- " };
					this->m_logger(message_level::error, xstr::join(message, path_txt, L"\n- ", path_mes, L"\n"));
				}
				continue;
			}

			const auto info{ this->m_helper.script_view().info() };
			const auto dirs{ xstr::cvt::to_utf16(info->name).append(L"_mes")   };
			const auto path{ xfsys::path::join(this->m_output_directory, dirs) };
			if (!xfsys::create_directory(path))
			{
				if (this->m_logger)
				{
					this->error_create_outdir_failed(path, entry.name(), mesname);
				}
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

	auto scripts_handler::set_script_info(const script_info* const script_info) noexcept -> scripts_handler&
	{
		this->m_helper.using_script_info(script_info);
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
		else 
		{
			if (this->m_logger)
			{
				this->m_logger(message_level::normal, L"[SCRIPTS_HANDLER::PROCESS] EXPORT_TEXT_HANDLE\n");
			}

			this->export_text_handle();
		}
		
		const auto end{ std::chrono::high_resolution_clock::now() };
		const auto dur{ std::chrono::duration_cast<std::chrono::microseconds>(end - beg) };
		return std::chrono::duration_cast<std::chrono::duration<double>>(dur).count();
	}

	auto scripts_handler::process(logger_t logger) const -> time_t
	{
		this->m_logger = logger;
		return this->process();
	}
}

