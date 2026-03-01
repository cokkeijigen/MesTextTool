#pragma once
#include <optional>
#include <vector>
#include <xfsys.hpp>

namespace mes 
{

	class config 
	{

		inline static const constexpr auto k_path = "#InputPath"      ;
		inline static const constexpr auto k_cdpg = "#UseCodePage"    ;
		inline static const constexpr auto k_tmin = "#Text-MinLength" ;
		inline static const constexpr auto k_tmax = "#Text-MaxLength" ;
		inline static const constexpr auto k_bfrp = "#Before-Replaces";
		inline static const constexpr auto k_atrp = "#After-Replaces" ;
		inline static const constexpr auto k_name = L".MesTextTool"   ;
		inline static const constexpr auto defcdpg = 936;
		inline static const constexpr auto deftmin = 22;
		inline static const constexpr auto deftmax = 24;

	public:
		
		using vector_t = std::vector<std::pair<std::wstring, std::wstring>>;

		const std::wstring input_path{};
		const uint32_t use_code_page{ defcdpg };
		const int32_t  text_min_length{ deftmin };
		const int32_t  text_max_length{ deftmax };

		const vector_t before_replaces{};
		const vector_t  after_replaces{};

		static auto config_file_exists(const std::string_view  directory) -> bool;
		static auto config_file_exists(const std::wstring_view directory) -> bool;

		static auto read(const xfsys::file& file, config& result) -> bool;
		static auto read(const std::string_view  path, bool defuat_name = true) -> std::optional<config>;
		static auto read(const std::wstring_view path, bool defuat_name = true) -> std::optional<config>;
		static auto read(const std::string_view  path, config& result, bool defuat_name = true) -> bool;
		static auto read(const std::wstring_view path, config& result, bool defuat_name = true) -> bool;

		static auto create(const std::string_view  path, bool defuat_name = true) -> bool;
		static auto create(const std::wstring_view path, bool defuat_name = true) -> bool;
		static auto create(const xfsys::file& file, const config& config) -> bool;
		static auto create(const std::string_view  path, const config& config, bool defuat_name = true) -> bool;
		static auto create(const std::wstring_view path, const config& config, bool defuat_name = true) -> bool;

		static inline constexpr auto defuat_name() { return k_name; };
	};
}