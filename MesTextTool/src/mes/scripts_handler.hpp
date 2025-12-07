#pragma once
#include <functional>
#include <vector>
#include <chrono>
#include <mes.hpp>
#include <config.hpp>
#include <script_text.hpp>

namespace mes::scripts
{

	inline constexpr uint32_t defualt_code_page{ 932 };

	class scripts_handler 
	{
		mutable mes::script_helper m_helper{};
		
		std::wstring m_input_directory_or_file{};
		std::wstring m_output_directory{};
		
		mutable std::vector<std::wstring> m_outputs{};

		uint32_t m_input_mes_code_page{ defualt_code_page };

		auto import_text_handle() const -> void;

		auto export_text_handle() const -> void;

		auto export_text(const std::wstring_view path, std::vector<const script_info*>& out_infos) const -> bool;

		auto warning_because_file_not (const std::wstring_view path, const std::wstring_view type) const noexcept -> void;

		auto warning_because_directory(const std::wstring_view path) const noexcept -> void;
		
		auto error_mes_failed_to_parse(const std::wstring_view  mes) const noexcept -> void;

		auto error_create_outdir_failed(const std::wstring_view path, const std::wstring_view name1, const std::wstring_view name2) const noexcept -> void;

	public:

		enum message_level { normal, warning, error };
		using logger_t = std::function<void(message_level level, std::wstring_view message)>;
		using time_t   = double;

		scripts_handler(std::wstring_view  input_directory_or_file, std::wstring_view  output_directory) noexcept;
		
		scripts_handler(std::u8string_view input_directory_or_file, std::u8string_view output_directory) noexcept;

		auto set_script_info(const script_info* const script_info) noexcept -> scripts_handler&;

		auto set_mes_code_page(const uint32_t code_page) noexcept -> scripts_handler&;

		auto process() const -> time_t;

		auto process(logger_t logger) const -> time_t;

	protected:

		mutable logger_t m_logger{};
	};

	using handler = scripts_handler;
}