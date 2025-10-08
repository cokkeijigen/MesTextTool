#pragma once
#include <xmemory.hpp>
#ifndef _FUNCTIONAL_
#include <functional>
#endif

namespace mes {

	struct script_info 
	{
		struct section 
		{
			uint8_t beg{}, end{};
			inline auto its(uint8_t key) const -> bool;
		};
		const char name[15];
		uint16_t version;
		section	 uint8x2; // [op: byte] [arg1: uint8] [arg2: uint8]
		section uint8str; // [op: byte] [arg1: uint8] [arg2: string]
		section	  string; // [op: byte] [arg1: string]
		section   encstr; // [op: byte] [arg1: encstr]
		section uint16x4; // [op: byte] [arg1: uint16] [arg2: uint16] [arg3: uint16] [arg4: uint16]
		uint8_t optunenc; // the opcode for unencrypted strings in scene text
		uint8_t deckey;
		
		static const script_info infos[];
		static auto query(std::string_view name) -> const script_info*;
		static auto query(uint16_t version) -> const script_info*;
	};

	class script_view {
	
	public:
		struct token { int32_t offset{}, length{}; uint8_t value{}; };
		template <class T> struct view { T* data{}; int32_t size{}, offset{}; };

		script_view() = default;
		script_view(std::span<uint8_t> raw, const script_info* info = nullptr);
		script_view(std::span<uint8_t> raw, uint16_t version);
		script_view(std::span<uint8_t> raw, const char* name);

		auto info() const -> const script_info*;
		auto raw () const -> view<uint8_t>;
		auto asmbin() const -> view<uint8_t>;
		auto blocks() const -> view<int32_t>;
		auto tokens() const -> const std::vector<token>&;
		auto version() const -> uint16_t;
	private:
		const script_info* m_Info{};
		std::vector<token> m_Tokens{};
		view<int32_t> m_Blocks{};
		view<uint8_t> m_Asmbin{};
		view<uint8_t> m_Raw{};
		uint16_t m_Version{};
		inline auto token_parse() -> void;
	};

	class script_helper {

	public:

		struct text { int32_t offset{}; std::string string{}; };

		script_helper();
		~script_helper();
		script_helper(std::string_view name);
		script_helper(script_info* info);

		auto is_parsed() -> bool;
		auto read(std::string_view path, bool check = true) -> script_helper&;
		auto set_info(const script_info* info) -> script_helper&;
		auto fetch_scene_text(bool absolute_file_offset = true) const -> std::vector<text>;
		auto import_scene_text(const std::vector<text>& texts, bool absolute_file_offset = true) -> bool;
		auto import_scene_text(const std::vector<std::pair<int32_t, std::string>>& texts, bool absolute_file_offset = true) -> bool;
		auto get_view() -> const script_view&;

	private:
		utils::xmem::buffer<uint8_t> m_Buffer{};
		const script_info*           m_Info{};
		script_view                  m_MesView{};
	};

	class multi_script_helper {
	public:
		struct config {

			using replaces = std::vector<std::pair<std::wstring, std::wstring>>;

			inline static constexpr char path[] = "#InputPath";
			inline static constexpr char cdpg[] = "#UseCodePage";
			inline static constexpr char tmin[] = "#Text-MinLength";
			inline static constexpr char tmax[] = "#Text-MaxLength";
			inline static constexpr char bfrp[] = "#Before-Replaces";
			inline static constexpr char atrp[] = "#After-Replaces";
			inline static constexpr char name[] = ".MesTextTool";

			std::string InputPath{};
			int32_t UseCodePage{ 936 };
			int32_t MinLength{ 22 };
			int32_t MaxLength{ 24 };
			replaces Before{};
			replaces After{};

			static auto read(std::string_view path) -> config;
			static auto read(std::string_view path, config& result) -> void;
		};

		class text_formater {

			config& m_Config;

			static auto is_first_char_forbidden(wchar_t chr) -> bool;
			static auto is_last_char_forbidden (wchar_t chr) -> bool;
			static auto is_talking(wchar_t beg, wchar_t end) -> bool;
			static auto is_half_width(wchar_t wchar) -> bool;

		public:
			text_formater(config& config);
			auto format(std::string& text) -> void;
		};

		static auto read_text(std::string_view path, std::vector<script_helper::text>& result) -> bool;

		static inline constexpr auto defualt_code_page{ static_cast<uint32_t>(932) };

		using success_call = std::function<void(std::string_view ipt, std::string_view opt)>;
		using failure_call = std::function<void(std::string_view ipt)>;
		using file_list    = std::vector<std::string>;
	private:

		std::string   m_IptDir{};
		std::string   m_OptDir{};
		file_list   m_FileList{};
		script_helper m_Helper{};
		
		uint32_t   m_IptCodePage{};
		success_call m_OnSuccess{};
		failure_call m_OnFailure{};
		std::string m_ConfigFile{};
		std::string m_ErrorMessage{};
		
		auto export_all_text() -> void;
		auto import_all_text() -> void;

		auto make_config_file(const std::string& opt_dir) -> void;

		inline auto on_failure(std::string_view msg, std::string_view ipt) -> void 
		{
			this->m_ErrorMessage.assign(msg);
			if (this->m_OnFailure) { this->m_OnFailure(ipt); }
		}
		
		inline auto on_success(std::string_view ipt, std::string_view opt) -> void 
		{
			if (this->m_OnSuccess) { this->m_OnSuccess(ipt, opt); };
		}
	public:

		multi_script_helper(std::string_view ipt_dirOrFile, std::string_view opt_dir, const script_info* info = {}, uint32_t ipt_cdpg = { defualt_code_page });
		
		auto get_err_msg() const  -> const std::string&;
		auto set_ipt_cdpg(int32_t cdpg)  -> multi_script_helper&;
		auto run(success_call onSuccess = {}, failure_call onFailure = {}, bool _noexcept = {false}) -> multi_script_helper&;
	};

}