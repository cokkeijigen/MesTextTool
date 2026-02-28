#pragma once
#include <span>
#include <variant>
#include <xmem.hpp>
#include <xfsys.hpp>
#include <mes_advtxt.hpp>

namespace mes 
{
	using advtxt_view = advtxt::view;
	using advtxt_info = advtxt::info;

	namespace text 
	{
		class entry;
	}

	struct token
	{
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

		const uint8_t*      data{};
		int32_t offset{}, length{};

		auto uint16x4() const noexcept -> const uint16x4_t*;
		auto uint8str() const noexcept -> const uint8str_t*;
		auto uint8x2 () const noexcept -> const uint8x2_t*;
		auto string  () const noexcept -> const string_t*;
		auto encstr  () const noexcept -> const encstr_t*;
		auto opcode  () const noexcept -> uint8_t;
	};

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
	};

	class script_view
	{
	public:

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


	class unioninfo
	{
	public:
		using variant = std::variant<std::nullptr_t, const mes::script_info*, const mes::advtxt_info*>;

		inline auto script_info() const noexcept -> const mes::script_info*;
		inline auto advtxt_info() const noexcept -> const mes::advtxt_info*;

		inline unioninfo() noexcept : m_value{ nullptr } {};
		inline unioninfo(std::nullptr_t) noexcept : m_value{ nullptr } {};
		inline unioninfo(const mes::script_info* script_info) noexcept;
		inline unioninfo(const mes::advtxt_info* advtxt_info) noexcept;

		inline auto empty() const noexcept -> bool;
		inline auto  name() const noexcept -> std::string_view;
		inline auto operator=(const mes::script_info* script_info) noexcept -> unioninfo&;
		inline auto operator=(const mes::advtxt_info* advtxt_info) noexcept -> unioninfo&;
		inline auto operator=(std::nullptr_t) noexcept -> unioninfo&;
	protected:
		variant m_value{};
	};

	class unionmes_view
	{
	public:

		enum view_type: uint8_t
		{
			mes         = 0,
			mes_type    = 0,
			advtxt      = 1,
			advtxt_type = 1
		};

		using variant = std::variant<std::nullptr_t, mes::script_view, mes::advtxt_view>;

		inline auto script_view() const noexcept -> const mes::script_view*;
		inline auto advtxt_view() const noexcept -> const mes::advtxt_view*;

		inline unionmes_view() noexcept : m_value{ nullptr } {};
		inline unionmes_view(std::nullptr_t) noexcept : m_value{ nullptr } {};

		inline unionmes_view(mes::script_view&& script_view) noexcept;
		inline unionmes_view(mes::advtxt_view&& advtxt_view) noexcept;

		inline auto empty() const noexcept -> bool;
		inline auto type() const noexcept -> view_type;
		inline auto info() const noexcept -> unioninfo;
		inline auto operator=(mes::script_view&& script_view) noexcept -> unionmes_view&;
		inline auto operator=(mes::advtxt_view&& advtxt_view) noexcept -> unionmes_view&;
		inline auto operator=(std::nullptr_t) noexcept -> unionmes_view&;

	protected:
		variant m_value{};
		view_type m_type{};
	};

	class script_helper
	{
	public:

		inline script_helper () noexcept {};
		inline ~script_helper() noexcept {};

		script_helper(const unioninfo            using_script_info) noexcept;
		script_helper(const std::string_view   using_script_info_name) noexcept;

		auto is_parsed() const noexcept -> bool;
		auto data_view() const noexcept -> const unionmes_view&;
		auto view_info() const noexcept -> const unioninfo&;
		auto using_script_info(const unioninfo info) noexcept -> script_helper&;

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

		auto export_text(const bool absolute_file_offset = true) const noexcept -> std::vector<text::entry>;
		auto import_text(const std::vector<text::entry>& texts, uint32_t use_code_page = 932, bool absolute_file_offset = true) noexcept -> bool;

		auto last_info_name() const noexcept -> std::string_view;

	protected:

		auto advtxt_import(const std::vector<text::entry>& texts, uint32_t use_code_page, bool absolute_file_offset) noexcept -> bool;
		auto script_import(const std::vector<text::entry>& texts, uint32_t use_code_page, bool absolute_file_offset) noexcept -> bool;

		auto script_export(std::vector<text::entry>& texts, bool absolute_file_offset) const noexcept -> bool;
		auto advtxt_export(std::vector<text::entry>& texts, bool absolute_file_offset) const noexcept -> bool;

		mutable unioninfo m_view_info{};
		mutable unionmes_view m_data_view{};
		mutable xmem::buffer<uint8_t> m_buffer{};
	};

	inline auto token::uint16x4() const noexcept -> const token::uint16x4_t*
	{
		return reinterpret_cast<const token::uint16x4_t*>(this->data);
	}

	inline auto token::uint8str() const noexcept -> const token::uint8str_t*
	{
		return reinterpret_cast<const token::uint8str_t*>(this->data);
	}

	inline auto token::uint8x2() const noexcept -> const token::uint8x2_t*
	{
		return reinterpret_cast<const token::uint8x2_t*>(this->data);
	}

	inline auto token::string() const noexcept -> const token::string_t*
	{
		return reinterpret_cast<const token::string_t*>(this->data);
	}

	inline auto  token::encstr() const noexcept -> const token::encstr_t*
	{
		return reinterpret_cast<const token::encstr_t*>(this->data);
	}

	inline auto token::opcode() const noexcept -> uint8_t
	{
		return static_cast<uint8_t>(this->data[0]);
	}

	inline auto script_info::section::is(const uint8_t key) const noexcept -> bool
	{
		return !(beg == end && beg == 0xFF) && (key >= beg && key <= end);
	}

	inline auto script_info::operator=(const mes::script_info& other) -> script_info&
	{
		if (this != reinterpret_cast<const script_info*>(&other))
		{
			std::memcpy(this, &other, sizeof(script_info));
		}
		return *this;
	}
	
	inline script_view::script_view(const std::span<uint8_t> raw, const uint16_t version)
		: script_view{ raw, script_info::query(version) }
	{
	}

	inline script_view::script_view(const std::span<uint8_t> raw, const std::string_view name)
		:script_view{ raw, script_info::query(name) }
	{
	}

	inline auto script_view::raw() const noexcept -> const script_view::view_t<uint8_t>&
	{
		return this->m_raw;
	}

	inline auto script_view::info() const noexcept -> const script_info* const
	{
		return this->m_info;
	}

	inline auto script_view::asmbin() const noexcept -> const view_t<uint8_t>&
	{
		return this->m_asmbin;
	}

	inline auto script_view::labels() const noexcept -> const view_t<int32_t>&
	{
		return this->m_labels;
	}

	inline auto script_view::tokens() const noexcept -> const std::vector<token>&
	{
		return this->m_tokens;
	}

	inline auto script_view::version() const noexcept -> uint16_t
	{
		return this->m_version;
	}
	
	inline auto unionmes_view::advtxt_view() const noexcept -> const mes::advtxt_view*
	{
		if (auto&& value = std::get_if<mes::advtxt_view>(&this->m_value))
		{
			return value;
		}
		return nullptr;
	}
	
	inline auto unionmes_view::script_view() const noexcept -> const mes::script_view*
	{
		if (auto&& value = std::get_if<mes::script_view>(&this->m_value))
		{
			return value;
		}
		return nullptr;
	}

	inline unionmes_view::unionmes_view(mes::script_view&& script_view) noexcept
	{
		this->m_value = std::move(script_view);
		this->m_type  = view_type::mes;
	}

	inline unionmes_view::unionmes_view(mes::advtxt_view&& advtxt_view) noexcept
	{
		this->m_value = std::move(advtxt_view);
		this->m_type  = view_type::advtxt;
	}

	inline auto unionmes_view::operator=(mes::script_view&& script_view) noexcept -> unionmes_view&
	{
		this->m_value = std::move(script_view);
		this->m_type  = view_type::mes;
		return *this;
	}

	inline auto unionmes_view::operator=(mes::advtxt_view&& advtxt_view) noexcept -> unionmes_view&
	{
		this->m_value = std::move(advtxt_view);
		this->m_type = view_type::advtxt;
		return *this;
	}

	inline auto unionmes_view::operator=(std::nullptr_t) noexcept -> unionmes_view&
	{
		this->m_value = nullptr;
		return *this;
	}

	inline auto unionmes_view::empty() const noexcept -> bool
	{
		return this->m_value.index() == 0 || this->m_value.valueless_by_exception();
	}

	inline auto unionmes_view::type() const noexcept -> view_type
	{
		return this->m_type;
	}

	inline auto unionmes_view::info() const noexcept -> unioninfo
	{
		if (auto&& value = std::get_if<mes::script_view>(&this->m_value))
		{
			return value->info();
		}
		
		if (auto&& value = std::get_if<mes::advtxt_view>(&this->m_value))
		{
			return value->info();
		}

		return nullptr;
	}

	inline auto unioninfo::advtxt_info() const noexcept -> const mes::advtxt_info*
	{
		if (auto&& value = std::get_if<const mes::advtxt_info*>(&this->m_value))
		{
			return *value;
		}
		return nullptr;
	}

	inline auto unioninfo::script_info() const noexcept -> const mes::script_info*
	{
		if (auto&& value = std::get_if<const mes::script_info*>(&this->m_value))
		{
			return *value;
		}
		return nullptr;
	}

	inline unioninfo::unioninfo(const mes::script_info* script_info) noexcept
	{
		this->m_value = script_info;
	}

	inline unioninfo::unioninfo(const mes::advtxt_info* advtxt_info) noexcept
	{
		this->m_value = advtxt_info;
	}

	inline auto unioninfo::operator=(const mes::script_info* script_info) noexcept -> unioninfo&
	{
		this->m_value = script_info;
		return *this;
	}

	inline auto unioninfo::operator=(const mes::advtxt_info* advtxt_info) noexcept -> unioninfo&
	{
		this->m_value = advtxt_info;
		return *this;
	}

	inline auto unioninfo::operator=(std::nullptr_t) noexcept -> unioninfo&
	{
		this->m_value = nullptr;
		return *this;
	}

	inline auto unioninfo::empty() const noexcept -> bool 
	{
		const auto info_1 = std::get_if<const mes::script_info*>(&this->m_value);
		const auto info_2 = std::get_if<const mes::advtxt_info*>(&this->m_value);
		return bool
		{
			(info_1 == nullptr || *info_1 == nullptr) &&
			(info_2 == nullptr || *info_2 == nullptr)
		};
	}

	inline auto unioninfo::name() const noexcept -> std::string_view
	{
		if (!this->empty())
		{
			if (auto&& value = std::get_if<const mes::script_info*>(&this->m_value))
			{
				return (*value)->name;
			}
			if (auto&& value = std::get_if<const mes::advtxt_info*>(&this->m_value))
			{
				return (*value)->name;
			}
		}
		return "";
	}

	inline script_helper::script_helper(const std::string_view using_script_info_name) noexcept
	{
		const mes::script_info* script_info{ script_info::query(using_script_info_name) };
		if (script_info != nullptr)
		{
			this->m_view_info = script_info;
			return;
		}

		const mes::advtxt_info* advtxt_info{ mes::advtxt_info::get(using_script_info_name) };
		if (advtxt_info != nullptr)
		{
			this->m_view_info = advtxt_info;
		}
	}

	inline script_helper::script_helper(const unioninfo using_script_info) noexcept
		: m_view_info{ using_script_info }
	{
	}

	inline auto script_helper::is_parsed() const noexcept -> bool
	{
		const auto script_view{ this->m_data_view.script_view() };
		if (script_view != nullptr && !script_view->tokens().empty())
		{
			return true;
		}

		const auto advtxt_view{ this->m_data_view.advtxt_view() };
		if (advtxt_view != nullptr && !advtxt_view->tokens().empty())
		{
			return true;
		}

		return false;
	}

	inline auto script_helper::last_info_name() const noexcept -> std::string_view
	{
		if (this->m_view_info.empty())
		{
			const auto script_view{ this->data_view().script_view() };
			if (script_view != nullptr)
			{
				const auto info{ script_view->info() };
				return info != nullptr ? info->name : "";
			}
		}
		else
		{
			const auto script_info{ this->m_view_info.script_info() };
			if(script_info != nullptr)
			{
				return script_info->name;
			}

			const auto advtxt_info{ this->m_view_info.advtxt_info() };
			if(advtxt_info != nullptr)
			{
				return advtxt_info->name;
			}
		}

		return "";
	}

	inline auto script_helper::data_view() const noexcept -> const unionmes_view&
	{
		return this->m_data_view;
	}

	inline auto script_helper::view_info() const noexcept -> const unioninfo&
	{
		return this->m_view_info;
	}

	inline auto script_helper::using_script_info(const unioninfo info) noexcept -> mes::script_helper&
	{
		this->m_view_info = info;
		return *this;
	}

	namespace script 
	{
		using info   = script_info;
		using view   = script_view;
		using helper = script_helper;
	}
}