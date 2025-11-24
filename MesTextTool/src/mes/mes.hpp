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

		enum offset_t: uint8_t 
		{ 
			offset1, // head[0] * 0x04 + 0x04
			offset2  // head[0] * 0x06 + 0x04
		};

		const char* name;
		const offset_t  offset;
		const uint16_t version;
		const section  uint8x2; // [op: byte] [arg1: uint8] [arg2: uint8]
		const section uint8str; // [op: byte] [arg1: uint8] [arg2: string]
		const section	string; // [op: byte] [arg1: string]
		const section   encstr; // [op: byte] [arg1: encstr]
		const section uint16x4; // [op: byte] [arg1: uint16] [arg2: uint16] [arg3: uint16] [arg4: uint16]
		const uint8_t   enckey;
		const std::initializer_list<uint8_t> opstrs; // the opcode for unencrypted strings in scene text
		
		static const script_info infos[];
		static auto query(std::span<uint8_t> data) -> const script_info*;
		static auto query(std::string_view name) -> const script_info*;
		static auto query(uint16_t version) -> const script_info*;
		auto operator =(const mes::script_info&) -> script_info&;

		#pragma pack(push, 1)
		struct uint8x2_t 
		{
			uint8_t op;
			uint8_t arg1;
			uint8_t arg2;
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct uint8str_t 
		{
			uint8_t op;
			uint8_t arg1;
			char str[];
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct string_t 
		{
			uint8_t op;
			char str[];
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct encstr_t
		{
			uint8_t op;
			char str[];
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct uint16x4_t
		{
			uint8_t op;
			uint16_t arg1;
			uint16_t arg2;
			uint16_t arg3;
			uint16_t arg4;
		};
		#pragma pack(pop)
	};

	class script_view 
	{
	
	public:
		struct token { int32_t offset{}, length{}; uint8_t value{}; };
		template <class T> struct view { T* data{}; int32_t size{}, offset{}; };

		script_view() = default;
		script_view(std::span<uint8_t> raw, const script_info* info = nullptr);
		script_view(std::span<uint8_t> raw, uint16_t version);
		script_view(std::span<uint8_t> raw, const char* name);

		auto info() const -> const script_info*;
		auto raw () const -> view<uint8_t>;
		auto asmbin () const -> view<uint8_t>;
		auto labels () const -> view<int32_t>;
		auto tokens () const -> const std::vector<token>&;
		auto version() const -> uint16_t;
	private:
		const script_info* m_info{};
		std::vector<token> m_tokens{};
		view<int32_t> m_labels{};
		view<uint8_t> m_asmbin{};
		view<uint8_t> m_raw{};
		uint16_t m_version{};

		inline auto token_parse() -> void;
	};

	class script_helper 
	{

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
		utils::xmem::buffer<uint8_t> m_buffer {};
		const script_info*           m_info   {};
		script_view                  m_mesview{};
	};

	class multi_script_helper 
	{
	public:
		struct config 
		{
			using vector_t = std::vector<std::pair<std::wstring, std::wstring>>;

			inline static const constexpr char k_path[]{ "#InputPath"       };
			inline static const constexpr char k_cdpg[]{ "#UseCodePage"     };
			inline static const constexpr char k_tmin[]{ "#Text-MinLength"  };
			inline static const constexpr char k_tmax[]{ "#Text-MaxLength"  };
			inline static const constexpr char k_bfrp[]{ "#Before-Replaces" };
			inline static const constexpr char k_atrp[]{ "#After-Replaces"  };
			inline static const constexpr char k_name[]{ ".MesTextTool"     };
			inline static const constexpr auto def_cdpg{ 936 };
			inline static const constexpr auto def_tmin{ 22 };
			inline static const constexpr auto def_tmax{ 24 };

		public:
			std::string path{};
			int32_t cdpg{ def_cdpg };
			int32_t tmin{ def_tmin };
			int32_t tmax{ def_tmax };
			vector_t bfrp{};
			vector_t atrp{};

			static auto read(std::string_view path) -> config;
			static auto read(std::string_view path, config& result) -> void;
		};

		class text_formater
		{
			const config& m_config;
			static auto is_first_char_forbidden(wchar_t chr) -> bool;
			static auto is_last_char_forbidden (wchar_t chr) -> bool;
			static auto is_talking(std::wstring_view str) -> bool;
			static auto is_half_width(wchar_t wchar) -> bool;
		public:
			text_formater(const config& config);
			auto format(std::string& text) -> void;
		};

		static auto read_text(std::string_view path, std::vector<script_helper::text>& result) -> bool;

		static inline constexpr auto defualt_code_page{ static_cast<uint32_t>(932) };

		using success_call_t = std::function<void(std::string_view ipt, std::string_view opt)>;
		using failure_call_t = std::function<void(std::string_view ipt)>;
		using file_list_t    = std::vector<std::string>;

	private:

		script_helper m_helper {};
		std::string   m_iptdir {};
		std::string   m_optdir {};
		uint32_t      m_iptcdpg{};

		file_list_t    m_filelist {};
		success_call_t m_onsuccess{};
		failure_call_t m_onfailure{};

		std::string m_configfile{};
		std::string m_error_message{};
		
		auto export_all_text() -> void;
		auto import_all_text() -> void;

		auto make_config_file(const std::string& opt_dir) -> void;

		inline auto on_failure(std::string_view msg, std::string_view ipt) -> void 
		{
			this->m_error_message.assign(msg);
			if (this->m_onfailure) { this->m_onfailure(ipt); }
		}
		
		inline auto on_success(std::string_view ipt, std::string_view opt) -> void 
		{
			if (this->m_onsuccess) { this->m_onsuccess(ipt, opt); };
		}

	public:

		multi_script_helper(std::string_view ipt_dir_or_file, std::string_view opt_dir, const script_info* info = {}, uint32_t ipt_cdpg = { defualt_code_page });
		
		auto get_err_msg() const  -> const std::string&;
		auto set_iptcdpg(int32_t cdpg)  -> multi_script_helper&;
		auto run(success_call_t onSuccess = {}, failure_call_t onFailure = {}, bool _noexcept = {false}) -> multi_script_helper&;
	};

}