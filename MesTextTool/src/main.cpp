#include <iostream>
#include <windows.h>
#include <mes.hpp>
#include <console.hpp>
#include <scripts_handler.hpp>

console::helper xcsl::helper{ L"" PROJECT_NAME " v" PROJECT_VERSION };

namespace mes_text_tool 
{

	inline static constexpr std::wstring_view information
	{
		L"----------------------------------------------------\n"
		L" " PROJECT_NAME " v" PROJECT_VERSION " by iTsukezigen.\n"
		L" GitHub: https://github.com/cokkeijigen/MesTextTool\n"
	};

	static auto get_value_from_exename(const wchar_t* args, bool& log, const mes::script_info*& info, uint32_t& cdpg) -> void
	{
		xstr::wstring_buffer exename{ xfsys::path::name(args) };
		const auto splits{ exename.to_lower().split_of(L'.', L'-', L'_') };

		for (const auto& arg : std::ranges::reverse_view(splits)) 
		{
			if (nullptr == info)
			{
				info = mes::script_info::query(xstr::cvt::to_utf8(arg));
			}

			if (arg.starts_with(L"cp"))
			{
				auto value{ xstr::to_integer<uint32_t>(arg.substr(2)) };
				if (value.has_value())
				{
					cdpg = value.value();
				}
				continue;
			}
			
			if (arg == L"log")
			{
				log = true;
			}
		}
	}

	static auto get_value_from_arg(const std::wstring_view& arg, bool& log, const mes::script_info*& info, uint32_t& cdpg)
	{
		if (!arg.empty() && arg.starts_with(L"-"))
		{
			if (arg.starts_with(L"-cp"))
			{
				auto value{ xstr::to_integer<uint32_t>(arg.substr(3)) };
				if (value.has_value())
				{
					cdpg = value.value();
				}
			}
			else if(arg == L"-log")
			{
				log = true;
			}
			else
			{
				const auto _info{ mes::script_info::query(xstr::cvt::to_utf8(arg.substr(1))) };
				if (_info != nullptr)
				{
					info = _info;
				}
			}
		}
	}

	static auto get_value_from_argv(const int argc, wchar_t* const argv[], bool& log, const mes::script_info*& info, uint32_t& cdpg)
	{
		xstr::wstring_buffer arg1{ argc > 2 ? argv[1] : L"" };
		xstr::wstring_buffer arg2{ argc > 3 ? argv[2] : L"" };
		xstr::wstring_buffer arg3{ argc > 4 ? argv[3] : L"" };

		if (arg1.count() > 0) 
		{
			get_value_from_arg(arg1.to_lower().view(), log, info, cdpg);
		}

		if (arg2.count() > 0)
		{
			get_value_from_arg(arg2.to_lower().view(), log, info, cdpg);
		}

		if (arg3.count() > 0)
		{
			get_value_from_arg(arg3.to_lower().view(), log, info, cdpg);
		}
	}

	static auto main(const int argc, wchar_t* const argv[]) -> void
	{

		double time{};

		if (argc < 2)
		{
			constexpr char message[]
			{
				"[ILLEGAL PARAMETER] "
				"At least 1 or 2 valid parameters are required.\n"
				"Args: -log<optional> -cp[codepage]<optional> -[game]<optional>\n"
				"Example: MesTextTool.exe -log -cp932 -dc3wy "
				"D:\\YourGames\\DC3WY\\Advdata\\MES\n"
			};

			xcsl::helper.set_attrs(console::attrs::text_dark_red);
			xcsl::helper.writeline(message);
			xcsl::helper.reset_attrs();

		}
		else 
		{
			bool enable_console_log{ false };
			const mes::script_info* input_script_info{ nullptr };
			uint32_t input_code_page{ mes::scripts::defualt_code_page };

			get_value_from_exename(argv[0], enable_console_log, input_script_info, input_code_page);
			get_value_from_argv(argc, argv, enable_console_log, input_script_info, input_code_page);

			const std::wstring_view  input_path{ argv[argc - 1] };
			const std::wstring_view output_path{ xfsys::path::parent(argv[0]) };

			xstr::wstring_buffer logs{};
			mes::scripts::handler handler{ input_path, output_path };

			handler.set_script_info(input_script_info);
			handler.set_mes_code_page(input_code_page);

			if (!enable_console_log)
			{
				xcsl::helper.writeline("[PROCESSING]...\n");
			}

			time = handler.process(
				[&](mes::scripts::handler::message_level level, std::wstring_view message) -> void
				{
					logs.write(message).write(L'\n');

					if (!enable_console_log)
					{
						return;
					}

					switch (level)
					{
					case mes::scripts::handler::message_level::normal:
					{
						xcsl::helper.writeline(message);
						return;
					}
					case mes::scripts::handler::message_level::warning:
					{
						xcsl::helper.set_attrs(xcsl::attrs::text_dark_yellow);
						xcsl::helper.writeline(message);
						xcsl::helper.reset_attrs();
						return;
					}
					case mes::scripts::handler::message_level::error:
					{
						xcsl::helper.set_attrs(xcsl::attrs::text_dark_red);
						xcsl::helper.writeline(message);
						xcsl::helper.reset_attrs();
						return;
					}
					};
				}
			);

			if (!enable_console_log)
			{
				xcsl::helper.clear();
				xcsl::helper.writeline("[COMPLETE]\n");
			}

			logs.write(information).write_as_format(L" Time: %llfs\n", time);
			const auto file{ xfsys::create(output_path, L"output.log") };
			file.write(logs.u8string(), xfsys::file::pos::begin);
			file.close(), logs.clear();
			
			xcsl::helper.set_attrs(xcsl::attrs::text_dark_yellow);
		}

		xcsl::helper.write(information);
		xcsl::helper.write(L" Time: %llfs\n", time);
		xcsl::helper.reset_attrs();
		xcsl::helper.read_anykey();
	}

	extern "C" auto main(void) -> int 
	{
		int argc{};
		const auto cmds{ ::GetCommandLineW() };
		const auto argv{ ::CommandLineToArgvW(cmds, &argc) };
		mes_text_tool::main(argc, argv);

		return { 0x114514 };
	}
}