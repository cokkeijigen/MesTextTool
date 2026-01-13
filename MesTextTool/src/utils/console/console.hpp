#pragma once
#define _console_
#include <windows.h>
#include <type_traits>
#include <streambuf>
#include <array>

namespace console 
{
	template<class T>
	concept use_char_t = std::disjunction_v<
		std::is_same<T, char>,
		std::is_same<T, char8_t>,
		std::is_same<T, wchar_t>,
		std::is_same<T, char16_t>
	>;

	struct attrs
	{
		enum other : uint16_t
		{
			leading_byte    = 0x0100,
			trailing_byte   = 0x0200,
			grid_horizontal = 0x0400,
			grid_lvertical  = 0x0800,
			grid_rvertical  = 0x1000,
			reverse_video   = 0x4000,
			underscore      = 0x8000,
		};

		enum color : uint16_t
		{
			text_dark_blue         = 0x01,
			text_dark_green        = 0x02,
			text_dark_teal         = 0x03,
			text_dark_red          = 0x04,
			text_dark_pink         = 0x05,
			text_dark_yellow       = 0x06,
			text_dark_white        = 0x07,
			text_dark_gray         = 0x08,
			text_blue              = 0x09,
			text_green             = 0x0A,
			text_teal              = 0x0B,
			text_red               = 0x0C,
			text_pink              = 0x0D,
			text_yellow            = 0x0E,
			text_white             = 0x0F,
			text_default           = 0x0F,
			background_dark_blue   = 0x10,
			background_dark_green  = 0x20,
			background_dark_teal   = 0x30,
			background_dark_red    = 0x40,
			background_dark_pink   = 0x50,
			background_dark_yellow = 0x60,
			background_dark_white  = 0x70,
			background_dark_gray   = 0x80,
			background_blue        = 0x90,
			background_green       = 0xA0,
			background_teal        = 0xB0,
			background_red         = 0xC0,
			background_pink        = 0xD0,
			background_yellow      = 0xE0,
			background_white       = 0xF0
		};

		static const attrs unset;

		uint16_t value{};

		inline attrs() noexcept {}

		inline attrs(auto value) noexcept : value(static_cast<uint16_t>(value))
		{
		}

		inline attrs(attrs&& value) noexcept : value(value.value)
		{
		}

		inline attrs(const attrs& value) noexcept : value(value.value)
		{
		}

		inline auto operator=(const attrs& other) -> attrs&
		{
			if (this == &other) 
			{
				return *this;
			}
			this->value = other.value;
			return *this;
		}

		inline auto operator=(uint16_t value) -> attrs&
		{
			this->value = value;
			return *this;
		}

		inline auto operator==(const attrs& other) const -> bool
		{
			return this->value == other.value;
		}

		inline auto operator==(uint16_t value) const -> bool
		{
			return this->value == value;
		}

		inline auto operator!=(const attrs& other) const -> bool
		{
			return !(*this == other);
		}

		inline auto operator!=(uint16_t value) const -> bool
		{
			return !(*this == value);
		}
		
	};

	inline auto operator|(attrs::other lhs, attrs::other rhs) -> attrs
	{
		return attrs{ static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs) };
	}

	inline auto operator|(attrs::color lhs, attrs::color rhs) -> attrs
	{
		return attrs{ static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs) };
	}

	inline auto operator|(attrs::other lhs, attrs::color rhs) -> attrs
	{
		return attrs{ static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs) };
	}

	inline auto operator|(attrs::color lhs, attrs::other rhs) -> attrs
	{
		return attrs{ static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs) };
	}

	inline auto operator|(attrs lhs, attrs::color rhs) -> attrs
	{
		return attrs{ lhs.value | static_cast<uint16_t>(rhs) };
	}

	inline auto operator|(attrs lhs, attrs::other rhs) -> attrs
	{
		return attrs{ lhs.value | static_cast<uint16_t>(rhs) };
	}

	enum cdpg : uint32_t
	{
		default_cp = 0x0000u,
		acp        = 0x0000u,
		oemcp      = 0x0001u,
		maccp      = 0x0002u,
		thread_acp = 0x0003u,
		symbol     = 0x002Au,
		utf_7      = 0xFDE8u,
		utf_8      = 0xFDE9u,
		sjis       = 0x03A4u,
		gbk        = 0x03A8u,
	};

	namespace xout
	{
		struct _out;
	}

	using cdpg_t  = cdpg;
	using attrs_t = attrs;

	class console_writer;
	class console_helper;
	class console_ostream;
	class console_streambuf;

	using helper_t    = console_helper;
	using ostream_t   = console_ostream;
	using streambuf_t = console_streambuf;
	using writer_t    = console_writer;

	class console_helper
	{
		cdpg_t _cdpg{ ::GetACP() };
		friend xout::_out;

	protected:
		HWND  m_Window{};
		HANDLE m_Output{};
		HANDLE m_Input{};

		auto vf_write(const char*     fmt, va_list arg_list) const noexcept -> const console_helper&;
		auto vf_write(const wchar_t*  fmt, va_list arg_list) const noexcept -> const console_helper&;
		auto vf_write(const char8_t*  fmt, va_list arg_list) const noexcept -> const console_helper&;
		auto vf_write(const char16_t* fmt, va_list arg_list) const noexcept -> const console_helper&;

	public:

		~console_helper() noexcept;
		console_helper () noexcept;
		console_helper(cdpg_t cdpg) noexcept;
		console_helper(const std::wstring_view title, cdpg_t cdpg = cdpg_t(::GetACP())) noexcept;
		console_helper(const std::string_view  title, cdpg_t cdpg = cdpg_t(::GetACP())) noexcept;

		auto show() const noexcept -> void;
		auto hide() const noexcept -> void;

		auto set_attrs(attrs_t attrs) const noexcept -> const console_helper&;
		auto reset_attrs() const noexcept -> const console_helper&;

		auto set_cp(uint32_t cdpg) const noexcept -> const console_helper&;
		auto reset_cp() const noexcept -> const console_helper&;

		auto clear() const noexcept -> const console_helper&;

		auto read_anykey() const noexcept -> const console_helper&;

		auto write(std::wstring_view content) const noexcept -> const console_helper&;
		auto write(std::string_view  content) const noexcept -> const console_helper&;
		auto write(uint32_t cdpg, std::string_view  content) const noexcept -> const console_helper&;

		auto write(std::u8string_view   content) const noexcept -> const console_helper&;
		auto write(std::u16string_view  content) const noexcept -> const console_helper&;

		inline auto write(const use_char_t auto one) const noexcept -> const console_helper&;
		inline auto write(uint32_t cdpg, const use_char_t auto* fmt, ...) const noexcept -> const console_helper&;
		inline auto write(const use_char_t auto* fmt, ...) const noexcept -> const console_helper&;

		auto writeline(std::wstring_view content) const noexcept -> const console_helper&;
		auto writeline(std::string_view  content) const noexcept -> const console_helper&;
		auto writeline(uint32_t cdpg, std::string_view  content) const noexcept -> const console_helper&;

		auto writeline(std::u16string_view  content) const noexcept -> const console_helper&;
		auto writeline(std::u8string_view   content) const noexcept -> const console_helper&;

		auto writer(uint32_t cdpg, attrs_t attrs = attrs::unset) const noexcept -> console_writer;

		auto ostream() const noexcept -> console_ostream;
		auto ostream(cdpg_t     cdpg) const noexcept -> console_ostream;
		auto ostream(attrs_t&& attrs) const noexcept -> console_ostream;
		auto ostream(const attrs_t& attrs) const noexcept -> console_ostream;
		auto ostream(attrs_t::color attrs) const noexcept -> console_ostream;
		auto ostream(attrs_t::other attrs) const noexcept -> console_ostream;
		auto ostream(uint32_t cdpg, attrs_t::other attrs) const noexcept -> console_ostream;
		auto ostream(uint32_t cdpg, attrs_t::color attrs) const noexcept -> console_ostream;
		auto ostream(uint32_t cdpg, attrs_t attrs = attrs::unset) const noexcept -> console_ostream;

		template <class... T>
		inline auto println(const std::format_string<T...> fmt, T&& ...args) const noexcept -> void;

		template <class... T>
		inline auto print(const std::format_string<T...> fmt, T&& ...args) const noexcept -> void;
	};

	class console_writer
	{
		uint32_t _cdpg{};
		attrs_t _attrs{};
		const console_helper& helper;
	public:

		inline console_writer(const console_helper& helper, uint32_t cdpg, attrs_t attrs = attrs::unset)
			noexcept : helper{ helper }, _attrs{ attrs }, _cdpg{ cdpg }
		{
		}

		inline auto cdpg(uint32_t cdpg) noexcept -> console_writer&
		{
			this->_cdpg = cdpg;
			return { *this };
		};

		inline auto attrs(attrs_t attrs) noexcept -> console_writer&
		{
			this->_attrs = attrs;
			return { *this };
		};

		auto write(const char* fmt,     ...) const noexcept -> const console_writer&;
		auto write(std::string_view content) const noexcept -> const console_writer&;

		auto writeline(std::string_view content) const noexcept -> const console_writer&;
	};

	class console_streambuf : public std::streambuf 
	{
		friend xout::_out;
		friend console_ostream;
		static inline constexpr size_t buffer_size = 512;

	protected:

		bool auto_sync{ true };
		const console_helper& helper;
		
		uint32_t cdpg{ cdpg::default_cp };
		attrs_t attrs{ attrs_t::unset };
		std::unique_ptr<char> buffer{ new char[buffer_size] };

		auto write(std::string_view str) const noexcept -> void;
	public:
		
		inline console_streambuf(const console_helper& helper) noexcept : helper{ helper }
		{
			this->setp(this->buffer.get(), this->buffer.get() + buffer_size);
		}
		
		inline console_streambuf(const console_helper& helper, bool auto_sync) noexcept 
			: helper{ helper }, auto_sync{ auto_sync }
		{
			this->setp(this->buffer.get(), this->buffer.get() + buffer_size);
		}

		inline console_streambuf(const console_helper& helper, uint32_t cdpg, attrs_t attrs) 
			noexcept : helper{ helper }, cdpg{ cdpg }, attrs{ attrs }
		{
			this->setp(this->buffer.get(), this->buffer.get() + buffer_size);
		}

		inline console_streambuf(const console_helper& helper, uint32_t cdpg, attrs_t attrs, bool auto_sync) 
			noexcept : helper{ helper }, cdpg{ cdpg }, attrs{ attrs }, auto_sync{ auto_sync }
		{
			this->setp(this->buffer.get(), this->buffer.get() + buffer_size);
		}

		inline ~console_streambuf() noexcept
		{
			this->sync();
		}

		virtual int sync() override;
		virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
		virtual int_type overflow(int_type c) override;
	};

	class console_ostream : public std::ostream 
	{
		friend xout::_out;
		console_streambuf streambuf;
		const console_helper& helper;

		friend inline auto operator<<(ostream_t& out, std::u8string_view   u8str) -> ostream_t&;
		friend inline auto operator<<(ostream_t& out, std::u16string_view u16str) -> ostream_t&;
		friend inline auto operator<<(ostream_t& out, std::wstring_view     wstr) -> ostream_t&;
		friend inline auto operator<<(ostream_t& out, const char8_t*   u8str) -> ostream_t&;
		friend inline auto operator<<(ostream_t& out, const char16_t* u16str) -> ostream_t&;
		friend inline auto operator<<(ostream_t& out, const wchar_t*    wstr) -> ostream_t&;
	public:

		inline ~console_ostream() noexcept { this->flush(); }
		
		inline console_ostream(const console_helper& helper) noexcept 
			: streambuf{ helper }, helper{ helper }, std::ostream(&streambuf){}

		inline console_ostream(const console_helper& helper, uint32_t cdpg, attrs_t attrs) noexcept 
			: streambuf{ helper, cdpg, attrs }, helper{ helper }, std::ostream(&streambuf) {}

		inline auto cdpg(uint32_t cdpg) noexcept -> console_ostream&
		{
			this->streambuf.cdpg = cdpg;
			return { *this };
		};

		inline auto attrs(attrs_t attrs) noexcept -> console_ostream&
		{
			this->streambuf.attrs = attrs;
			return { *this };
		};

		inline auto attrs(attrs_t::color color) noexcept -> console_ostream&
		{
			if (this->streambuf.attrs == attrs_t::unset)
			{
				this->streambuf.attrs = color;
			}
			else if (color & 0xF0 && color & 0x0F)
			{
				this->streambuf.attrs.value &= 0xFF00;
				this->streambuf.attrs.value |= color & 0xFF;
			}
			else if (color & 0xF0)
			{
				this->streambuf.attrs.value &= 0xFF0F;
				this->streambuf.attrs.value |= color & 0xF0;
			}
			else
			{
				this->streambuf.attrs.value &= 0xFFF0;
				this->streambuf.attrs.value |= color & 0x0F;
			}
			return { *this };
		};

		inline auto attrs(attrs_t::other other) noexcept -> console_ostream&
		{
			if (this->streambuf.attrs == attrs_t::unset)
			{
				this->streambuf.attrs = other;
			}
			else 
			{
				this->streambuf.attrs.value &= 0x00FF;
				this->streambuf.attrs.value |= other & 0xFF00;
			}
			return { *this };
		}

		console_ostream(const console_ostream&) = delete;
		console_ostream& operator=(const console_ostream&) = delete;

	};

	inline auto operator<<(ostream_t& out, cdpg_t cdpg) -> ostream_t&
	{
		auto raw{ dynamic_cast<console_ostream*>(&out) };
		if (raw != nullptr) 
		{
			raw->cdpg(cdpg);
		}
		return out;
	}
	
	template<class T>
	requires std::is_same_v<T, attrs_t> || std::is_same_v<T, attrs_t::color> || std::is_same_v<T, attrs_t::other>
	inline auto operator<<(ostream_t& out, T attrs) -> ostream_t&
	{
		auto raw{ dynamic_cast<console_ostream*>(&out) };
		if (raw != nullptr) 
		{
			raw->attrs(attrs);
		}
		return out;
	}

	inline auto operator<<(ostream_t& out, std::u8string_view u8str) -> ostream_t&
	{
		auto raw{ dynamic_cast<console_ostream*>(&out) };
		if (raw != nullptr)
		{
			raw->helper.set_cp(console::cdpg::utf_8);
			raw->streambuf.xsputn(reinterpret_cast<const char*>(u8str.data()), u8str.size());
			raw->helper.reset_cp();
		}
		else 
		{
			out << std::string_view{ reinterpret_cast<const char*>(u8str.data()), u8str.size() };
		}
		return out;
	}

	inline auto operator<<(ostream_t& out, std::u16string_view u16str) -> ostream_t&
	{
		auto raw{ dynamic_cast<console_ostream*>(&out) };
		if (raw != nullptr)
		{
			raw->helper.write(u16str);
		}
		return out;
	}

	auto operator<<(ostream_t& out, std::wstring_view wstr) -> ostream_t&
	{
		auto raw{ dynamic_cast<console_ostream*>(&out) };
		if (raw != nullptr)
		{
			raw->helper.write(wstr);
		}
		return out;
	}

	inline auto operator<<(ostream_t& out, const char8_t* u8str) -> ostream_t&
	{
		out << std::u8string_view{ u8str };
		return out;
	}

	auto operator<<(ostream_t& out, const char16_t* u16str) -> ostream_t&
	{
		out << std::u16string_view{ u16str };
		return out;
	}

	auto operator<<(ostream_t& out, const wchar_t* wstr) -> ostream_t&
	{
		out << std::u16string_view{ reinterpret_cast<const char16_t*>(wstr) };
		return out;
	}

	inline auto console_helper::write(const use_char_t auto one) const noexcept -> const console_helper&
	{
		return this->write(std::basic_string_view<std::decay_t<decltype(one)>>{ &one, 1 });
	}
	
	inline auto console_helper::write(uint32_t cdpg, const use_char_t auto* fmt, ...) const noexcept -> const console_helper&
	{
		va_list arg_list{};
		va_start(arg_list, fmt);
		this->set_cp(cdpg);
		this->vf_write(fmt, arg_list);
		this->reset_cp();
		va_end(arg_list);

		return { *this };
	}

	inline auto console_helper::write(const use_char_t auto* fmt, ...) const noexcept -> const console_helper&
	{
		va_list arg_list{};
		va_start(arg_list, fmt);
		this->vf_write(fmt, arg_list);
		va_end(arg_list);
		return { *this };
	}

	template<class ...T>
	inline auto console_helper::println(const std::format_string<T...> fmt, T&& ...args) const noexcept -> void
	{
		console_ostream out{ *this };
		std::println(out, fmt, std::forward<T&&>(args)...);
	}

	template<class ...T>
	inline auto console_helper::print(const std::format_string<T...> fmt, T&& ...args) const noexcept -> void
	{
		console_ostream out{ *this };
		std::print(out, fmt, std::forward<T&&>(args)...);
	}

	extern helper_t helper;
}

namespace console::xout
{
	using color = console::attrs::color;
	using other = console::attrs::other;
	using attrs = console::attrs;
	using cdpg  = console::cdpg;
	using color_t = color;
	using other_t = other;
	using attrs_t = attrs;
	using cdpg_t  = cdpg;

	inline struct _out
	{
		using func_wrapper_t = std::ostream&(*)(std::ostream&);
		inline auto get() -> console::ostream_t&
		{
			static auto&& __out__{ std::make_unique<console::ostream_t>(console::helper) };
			return { *__out__ };
		}

		inline auto operator*() -> console::ostream_t&
		{
			return { this->get() };
		}

		template<class T>
		requires requires(T&& t) { std::declval<_out>().get() << t; }
		inline auto put(T&& value) -> _out& 
		{
			this->get() << value;
			return { *this };
		}

		inline auto put(func_wrapper_t pf) -> _out& 
		{
			pf(this->get());
			return { *this };
		}

		inline auto printf(const console::use_char_t auto* fmt, ...) -> void
		{
			va_list arg_list{};
			va_start(arg_list, fmt);
			if constexpr (std::is_same_v<decltype(fmt[0]), char>)
			{
				helper.set_cp(this->get().streambuf.cdpg);
			}
			helper.set_attrs(this->get().streambuf.attrs);
			helper.vf_write(fmt, arg_list).reset_attrs();
			if constexpr (std::is_same_v<decltype(fmt[0]), char>)
			{
				helper.reset_cp();
			}
			va_end(arg_list);
		}

	} out{};

	template<class T, class R = decltype(out.get() << std::declval<T>())>
	inline auto operator<<(_out& o, T&& t) -> R
	{
		return { out.get() << t };
	}

	template<class R = decltype(std::declval<_out::func_wrapper_t>()(out.get()))>
	inline auto operator<<(_out& o, _out::func_wrapper_t pf) -> R
	{
		return { pf(out.get()) };
	}

	template<class ...T>
	inline auto println(const std::format_string<T...> fmt, T&& ...args) -> void
	{
		std::println(*out, fmt, std::forward<T&&>(args)...);
	}

	template<class ...T>
	inline auto print(const std::format_string<T...> fmt, T && ...args) -> void
	{
		std::print(*out, fmt, std::forward<T&&>(args)...);
	}

	template<class T, class R = decltype(out.put(std::declval<T>()))>
	inline auto put(T&& value) -> R
	{
		return { out.put(value) };
	}

	inline auto put(_out::func_wrapper_t pf) -> _out&
	{
		pf(out.get());
		return { out };
	}

	template<class ...V>
	inline auto printf(const console::use_char_t auto* fmt, V&&... args) -> void
	{
		out.printf(fmt, args...);
	}

	inline auto clear() -> void 
	{
		helper.clear();
	}

	inline auto read_anykey() -> void
	{
		helper.read_anykey();
	}
};
namespace xcout = console::xout;