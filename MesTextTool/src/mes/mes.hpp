#pragma once
#include <span>
#include <variant>
#include <xmem.hpp>
#include <xfsys.hpp>
#include <mes_advtxt.hpp>

namespace mes 
{
	struct script_info 
	{
		struct section
		{
			const uint8_t beg{}, end{};
			auto is(const uint8_t key) const noexcept -> bool;
		};

		enum offset_t : uint8_t
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

		static auto query(const uint16_t version)        -> const script_info* const;
		static auto query(const std::span<uint8_t> data) -> const script_info* const;
		static auto query(const std::string_view name)   -> const script_info* const;
		
		auto operator=(const mes::script_info&) -> script_info&;

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

		struct token
		{
			const uint8_t*      data{};
			int32_t offset{}, length{};

			auto uint16x4() const noexcept -> const script_info::uint16x4_t*;
			auto uint8str() const noexcept -> const script_info::uint8str_t*;
			auto uint8x2 () const noexcept -> const script_info::uint8x2_t*;
			auto string  () const noexcept -> const script_info::string_t*;
			auto encstr  () const noexcept -> const script_info::encstr_t*;
			auto opcode  () const noexcept -> uint8_t;
		};

		template<class T>
		class view_t :public std::span<T>
		{
			int32_t m_offset{};

		public:
			using std::span<T>::span;
			using std::span<T>::operator=;

			inline view_t(const std::span<T>& data, int32_t offset) noexcept :
				std::span<T>::span{ data }, m_offset{offset}
			{
			}

			inline view_t(view_t&& other) noexcept 
			{
				this->operator=(std::move(other));
			}

			inline auto operator=(view_t&& other) noexcept -> view_t&
			{
				if (this != &other)
				{
					std::span<T>::operator=(std::move(other));
					this->m_offset = other.m_offset;
				}
				return *this;
			}

			inline auto offset() const noexcept -> int32_t
			{
				return this->m_offset;
			}
		};

		script_view() = default;

		script_view(const std::span<uint8_t> raw, const script_info* const script_info = nullptr);
		script_view(const std::span<uint8_t> raw, const std::string_view script_info_name);
		script_view(const std::span<uint8_t> raw, const uint16_t script_info_version);

		auto raw () const noexcept -> const view_t<uint8_t>&;
		auto info() const noexcept -> const script_info* const;

		auto asmbin () const noexcept -> const view_t<uint8_t>&;
		auto labels () const noexcept -> const view_t<int32_t>&;
		auto tokens () const noexcept -> const std::vector<token>&;
		auto version() const noexcept -> uint16_t;

	protected:
		
		mutable uint16_t m_version{};
		mutable const script_info* m_info{};

		mutable std::vector<token> m_tokens{};
		mutable view_t<int32_t>    m_labels{};
		mutable view_t<uint8_t>    m_asmbin{};
		mutable view_t<uint8_t>    m_raw{};

		auto init_by_offset1() noexcept -> void;
		auto init_by_offset2() noexcept -> void;
		auto token_parse() noexcept -> void;
	};

	class script_helper
	{
	public:

		union string_union_t
		{
			inline ~string_union_t() noexcept
			{
				this->string.~basic_string();
			}

			std::string string;
			std::u8string u8string;
		};

		class text_pair_t
		{
			int32_t m_offset;
			uint8_t m_text[sizeof(string_union_t)]{};
		public:
			
			struct proxy_type
			{
				int32_t offset;
				string_union_t text;
			};
			
			inline ~text_pair_t() noexcept
			{
				this->operator->()->text.string.~basic_string();
			}

			inline text_pair_t(text_pair_t&& other) noexcept 
			{
				this->operator->()->text.string = std::move(other.operator->()->text.string);
				this->operator->()->offset = std::move(other.operator->()->offset);
			}

			inline text_pair_t(int32_t offset, std::string& text) noexcept : m_offset{ offset }
			{
				this->operator->()->text.string = std::move(text);
			}

			inline text_pair_t(int32_t offset, std::u8string& text) noexcept : m_offset{ offset }
			{
				this->operator->()->text.u8string = std::move(text);
			}

			inline auto operator->() noexcept -> proxy_type*
			{
				return reinterpret_cast<proxy_type*>(this);
			}

			inline auto operator->() const noexcept -> const proxy_type*
			{
				return reinterpret_cast<const proxy_type*>(this);
			}

			inline auto offset() const noexcept -> int32_t
			{
				return this->m_offset;
			};

			inline auto text() const noexcept -> const string_union_t&
			{
				return *reinterpret_cast<const string_union_t*>(&this->m_text);
			}
		};

		class union_view_t
		{
		public:

			using variant = std::variant<mes::script_view, mes::advtxt_view>;

			auto script_view() const noexcept -> const mes::script_view*;
			auto advtxt_view() const noexcept -> const mes::advtxt_view*;

			inline union_view_t() noexcept = default;
			union_view_t(mes::script_view&& script_view) noexcept;
			union_view_t(mes::advtxt_view&& advtxt_view) noexcept;

			auto operator=(mes::script_view&& script_view) noexcept -> union_view_t&;
			auto operator=(mes::advtxt_view&& advtxt_view) noexcept -> union_view_t&;

		protected:
			variant m_value{};
		};

		class union_info_t 
		{
		public:
			using variant = std::variant<mes::script_info*, mes::advtxt_info>;


		protected:
			variant m_value{};
		};

		inline script_helper () noexcept {};
		inline ~script_helper() noexcept {};

		script_helper(const std::string_view   using_script_info_name) noexcept;
		script_helper(const mes::script_info* const using_script_info) noexcept;

		auto is_parsed() const noexcept -> bool;
		auto script_view() const noexcept -> const mes::script_view&;
		auto script_info() const noexcept -> const mes::script_info* const;
		auto using_script_info(const mes::script_info* const info) noexcept -> script_helper&;

		auto load(const xfsys::file& file) noexcept -> script_helper&;
		auto load(const std::wstring_view  path, const bool check = true) noexcept -> script_helper&;
		auto load(const std::u8string_view path, const bool check = true) noexcept -> script_helper&;
		
		auto load(const std::wstring_view  directory, const std::wstring_view  name) noexcept -> script_helper&;
		auto load(const std::u8string_view directory, const std::u8string_view name) noexcept -> script_helper&;

		auto save(const xfsys::file& file) noexcept -> bool;
		auto save(const std::wstring_view  path) noexcept -> bool;
		auto save(const std::u8string_view path) noexcept -> bool;

		auto save(const std::wstring_view  directory, const std::wstring_view  name) noexcept -> bool;
		auto save(const std::u8string_view directory, const std::u8string_view name) noexcept -> bool;

		auto export_text(const bool absolute_file_offset = true) const noexcept -> std::vector<text_pair_t>;
		auto import_text(const std::vector<text_pair_t>& texts, bool absolute_file_offset = true) noexcept -> bool;

	protected:

		auto advtxt_import(const std::vector<text_pair_t>& texts, bool absolute_file_offset) noexcept -> bool;
		auto script_import(const std::vector<text_pair_t>& texts, bool absolute_file_offset) noexcept -> bool;

		const  mes::script_info* m_script_info{};
		mutable mes::script_view m_script_view{};
		mutable xmem::buffer<uint8_t> m_buffer{};
		mutable union_view_t m_data_view{};
	};

	namespace script 
	{
		using info = script_info;
		using view = script_view;
		using helper = script_helper;
		using text_pair_t    = script_helper::text_pair_t;
		using string_union_t = script_helper::string_union_t;
	}
}