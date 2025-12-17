#include <iostream>
#include <vector>
#include <string>
#include "console.hpp"

#ifdef min
#undef min
#endif

namespace console
{
	const attrs attrs::unset = { 0xFFFF };

	console_helper::~console_helper() noexcept
	{
		if (this->m_Window != nullptr)
		{
			static_cast<void>(::DestroyWindow(this->m_Window));
		}

		this->m_Input  = nullptr;
		this->m_Output = nullptr;
		this->m_Window = nullptr;

		static_cast<void>(::FreeConsole());
	}

	console_helper::console_helper() noexcept 
	{
		::AllocConsole();
		this->m_Output = ::GetStdHandle(STD_OUTPUT_HANDLE);
		this->m_Input  = ::GetStdHandle(STD_INPUT_HANDLE);
		this->m_Window = ::GetConsoleWindow();
	}

	console::console_helper::console_helper(cdpg_t cdpg) noexcept : console_helper()
	{
		this->_cdpg = cdpg;
		this->set_cp(cdpg);
	}

	console_helper::console_helper(const std::wstring_view title, cdpg_t cdpg) noexcept : console_helper(cdpg)
	{
		if (!title.empty())
		{
			static_cast<void>(::SetConsoleTitleW(title.data()));
		}
	}

	console_helper::console_helper(const std::string_view title, cdpg_t cdpg) noexcept : console_helper(cdpg)
	{
		if (!title.empty())
		{
			static_cast<void>(::SetConsoleTitleA(title.data()));
		}
	}

	auto console_helper::show() const noexcept -> void
	{
		if (this->m_Window != nullptr)
		{
			::ShowWindow(this->m_Window, SW_SHOW);
		}
	}

	auto console_helper::hide() const noexcept -> void
	{
		if (this->m_Window != nullptr)
		{
			::ShowWindow(this->m_Window, SW_HIDE);
		}
	}

	auto console_helper::set_attrs(attrs_t attrs) const noexcept -> const console_helper&
	{
		if (attrs != attrs::unset)
		{
			::SetConsoleTextAttribute(this->m_Output, attrs.value);
		}

		return { *this };
	}

	auto console_helper::reset_attrs() const noexcept -> const console_helper&
	{
		this->set_attrs(attrs::color::text_default);
		return { *this };
	}

	auto console_helper::set_cp(uint32_t cdpg) const noexcept -> const console_helper&
	{
		::SetConsoleOutputCP(cdpg);
		return { *this };
	}

	auto console_helper::reset_cp() const noexcept -> const console_helper&
	{
		::SetConsoleOutputCP(this->_cdpg);
		return { *this };
	}

	auto console_helper::clear() const noexcept -> const console_helper&
	{
		DWORD cellsWritten{};
		CONSOLE_SCREEN_BUFFER_INFO csbi{};
		::GetConsoleScreenBufferInfo(this->m_Output, &csbi);
		::SetConsoleCursorPosition(this->m_Output, { NULL });
		::FillConsoleOutputCharacterA(this->m_Output, ' ',
			csbi.dwSize.X * csbi.dwSize.Y, { NULL }, &cellsWritten);
		::FillConsoleOutputAttribute(this->m_Output, csbi.wAttributes,
			csbi.dwSize.X * csbi.dwSize.Y, { NULL }, &cellsWritten);
		return { *this };
	}

	auto console_helper::read_anykey() const noexcept -> const console_helper&
	{
		INPUT_RECORD inputRecord{};
		DWORD numRead{};
		do {
			if (!ReadConsoleInputW(this->m_Input, &inputRecord, 1, &numRead)) break;
		} while (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown);
		return { *this };
	}

	auto console_helper::write(std::wstring_view content) const noexcept -> const console_helper&
	{
		if (this->m_Output != nullptr && !content.empty())
		{
			::WriteConsoleW(this->m_Output, content.data(), content.size(), NULL, NULL);
		}
		return { *this };
	}

	auto console_helper::write(std::string_view content) const noexcept -> const console_helper&
	{
		if (this->m_Output != nullptr && !content.empty())
		{
			::WriteConsoleA(this->m_Output, content.data(), content.size(), NULL, NULL);
		}
		return { *this };
	}

	auto console_helper::write(uint32_t cdpg, std::string_view content) const noexcept -> const console_helper&
	{
		this->set_cp(cdpg).write(content).reset_cp();
		return { *this };
	}

	auto console_helper::write(std::u8string_view content) const noexcept -> const console_helper&
	{
		std::string_view u8string{ reinterpret_cast<const char*>(content.data()), content.size() };
		this->set_cp(cdpg::utf_8).write(u8string).reset_cp();
		return { *this };
	}

	auto console_helper::write(std::u16string_view content) const noexcept -> const console_helper&
	{
		std::wstring_view u16string{ reinterpret_cast<const wchar_t*>(content.data()), content.size() };
		return this->write(u16string);
	}

	auto console_helper::vf_write(const char* fmt, va_list arg_list) const noexcept -> const console_helper&
	{
		if (this->m_Output == nullptr)
		{
			return { *this };
		}

		const auto size{ std::vsnprintf(nullptr, 0, fmt, arg_list) };
		if (size > 0)
		{
			auto buffer = std::string(size + 1, '\0');
			std::vsnprintf(buffer.data(), buffer.size() + 1, fmt, arg_list);
			this->write(std::string_view{ buffer.data(), static_cast<size_t>(size) });
		}
		return { *this };
	}

	auto console_helper::vf_write(const wchar_t* fmt, va_list arg_list) const noexcept -> const console_helper&
	{
		if (this->m_Output == nullptr)
		{
			return { *this };
		}
		
		const auto size{ std::vswprintf(nullptr, 0, fmt, arg_list) };
		if (size > 0)
		{
			auto buffer = std::vector<wchar_t>(size + 1, L'\0');
			std::vswprintf(buffer.data(), buffer.size() + 1, fmt, arg_list);
			this->write(std::wstring_view{ buffer.data(), static_cast<size_t>(size) });
		}
		return { *this };
	}

	auto console_helper::vf_write(const char8_t* fmt, va_list arg_list) const noexcept -> const console_helper&
	{
		return { this->set_cp(cdpg::utf_8).vf_write(reinterpret_cast<const char*>(fmt), arg_list).reset_cp() };
	}

	auto console_helper::vf_write(const char16_t* fmt, va_list arg_list) const noexcept -> const console_helper&
	{
		return { this->write(reinterpret_cast<const wchar_t*>(fmt), arg_list) };
	}

	auto console_helper::writeline(std::wstring_view content) const noexcept -> const console_helper&
	{
		return this->write(content).write('\n');
	}

	auto console_helper::writeline(std::string_view content) const noexcept -> const console_helper&
	{
		return this->write(content).write('\n');
	}

	auto console_helper::writeline(uint32_t cdpg, std::string_view content) const noexcept -> const console_helper&
	{
		return this->set_cp(cdpg).write(content).write('\n').reset_cp();
	}

	auto console_helper::writeline(std::u16string_view content) const noexcept -> const console_helper&
	{
		return this->write(content).write('\n');
	}

	auto console_helper::writeline(std::u8string_view content) const noexcept -> const console_helper&
	{
		return this->write(content).write('\n');
	}

	auto console_helper::writer(uint32_t cdpg, attrs_t attrs) const noexcept -> console_writer
	{
		return console_writer{ *this, cdpg, attrs };
	}

	auto console::console_helper::ostream() const noexcept -> console_ostream
	{
		return console_ostream{ *this };
	}

	auto console_helper::ostream(cdpg_t cdpg) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg, attrs_t::unset };
	}

	auto console_helper::ostream(attrs_t&& attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg::default_cp, attrs };
	}

	auto console::console_helper::ostream(const attrs_t& attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg::default_cp, attrs };
	}

	auto console::console_helper::ostream(attrs_t::color attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg::default_cp, attrs };
	}

	auto console::console_helper::ostream(attrs_t::other attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg::default_cp, attrs };
	}

	auto console::console_helper::ostream(uint32_t cdpg, attrs_t::other attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg, attrs };
	}

	auto console::console_helper::ostream(uint32_t cdpg, attrs_t::color attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg, attrs };
	}

	auto console_helper::ostream(uint32_t cdpg, attrs_t attrs) const noexcept -> console_ostream
	{
		return console_ostream{ *this, cdpg, attrs };
	}

	auto console_writer::write(std::string_view content) const noexcept -> const console_writer&
	{
		this->helper.set_cp(this->_cdpg).set_attrs(this->_attrs).write(content).reset_attrs().reset_cp();
		return { *this };
	}

	auto console_writer::write(const char* fmt, ...) const noexcept -> const console_writer&
	{
		va_list arg_list{};
		va_start(arg_list, fmt);
		this->helper
			.set_cp(this->_cdpg)
			.set_attrs(this->_attrs)
			.write(fmt, arg_list)
			.reset_attrs()
			.reset_cp();
		va_end(arg_list);
		
		return { *this };
	}

	auto console_writer::writeline(std::string_view content) const noexcept -> const console_writer&
	{
		this->helper.set_cp(this->_cdpg).set_attrs(this->_attrs).writeline(content).reset_attrs().reset_cp();
		return { *this };
	}

	std::streambuf::int_type console_streambuf::sync()
	{
		const auto count{ static_cast<size_t>(this->pptr() - this->pbase()) };
		if (count > 0) 
		{
			this->write(std::string_view{ this->pbase(), count });
			this->setp(this->pbase(), this->epptr());
		}
		return 0;
	}

	std::streambuf::int_type console_streambuf::overflow(int_type c)
	{
		if (!this->auto_sync)
		{
			std::streamsize remaining_space{ this->epptr() - this->pptr() };
			if (remaining_space == 1)
			{
				this->sync();
			}
		}
		
		if (c != traits_type::eof())
		{
			*this->pptr() = traits_type::to_char_type(c);
			this->pbump(1);
		}

		if (this->auto_sync)
		{
			this->sync();
		}

		return traits_type::not_eof(c);
	}

	std::streamsize console_streambuf::xsputn(const char_type* s, std::streamsize n)
	{
		if (s == nullptr || n == 0)
		{
			return n;
		}

		if (n >= console_streambuf::buffer_size)
		{
			
			this->sync();
			this->helper.write(std::string_view
				{ 
					reinterpret_cast<const char*>(s),
					static_cast<size_t>(n)
				}
			);
			return n;
		}
		
		std::streamsize remaining_space{ this->epptr() - this->pptr() };
		if (remaining_space <= n)
		{
			this->sync();
		}

		std::memcpy(this->pptr(), s, n);
		this->pbump(static_cast<int>(n));

		if (this->auto_sync)
		{
			this->sync();
		}

		return n;
	}

	auto console_streambuf::write(std::string_view str) const noexcept -> void
	{
		if (!str.empty())
		{
			this->helper.set_cp(this->cdpg).set_attrs(this->attrs);
			this->helper.write(str);
			this->helper.reset_cp().reset_attrs();
		}
	}
	
}