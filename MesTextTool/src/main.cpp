#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <mes.hpp>
#include <console.hpp>
#include <utils.hpp>
#include <xstr.hpp>
#include <xmem.hpp>
#include <file.hpp>
#include <main.hpp>

static auto run(int argc, const char** args) -> void
{
	try {

		bool enable_log{ false };

		if (argc < 2)
		{
			throw std::exception
			{
				"[ILLEGAL PARAMETER] "
				"At least 1 or 2 valid parameters are required.\n"
				"Example: MesTextTool.exe [-LOG<optional>] "
				"D:\\YourGames\\Name\\Advdata\\MES\n"
			};
		}
		else if (argc == 3)
		{
			std::string_view arg{ args[1] };
			enable_log = { "-log" == arg || "-LOG" == arg };
		}

		auto input_info{ static_cast<const mes::script_info*>(nullptr) };
		auto input_cdpg{ static_cast<uint32_t>(mes::multi_script_helper::defualt_code_page) };
		auto input_path{ std::string_view{ argc == 3 ? args[2] : args[1] } };
		auto self_path { std::string_view{ args[0] }};
		auto work_path { std::string_view{ ".\\" }  };

		size_t find_pos{ self_path.find_last_of("\\/") };
		if (find_pos != std::string_view::npos)
		{
			work_path = self_path.substr(0, find_pos + 1);

			utils::string::buffer exename{ self_path.substr(find_pos + 1) };
			auto splits{ exename.split_of('.', '-', '_') };
			for (auto& arg : std::ranges::reverse_view(splits))
			{
				if (nullptr == input_info)
				{
					input_info = mes::script_info::query(arg);
				}

				if (!arg.starts_with("cp"))
				{
					continue;
				}

				auto value{ xstr::to_integer<uint32_t>(arg.substr(2)) };
				if (value.has_value())
				{
					input_cdpg = value.value();
					if (input_info) { break; }
				}
			}
		}

		if (!enable_log)
		{
			xcsl::helper.writeline("[PROCESSING]...\n");
		}

		mes::multi_script_helper helper{ input_path, work_path, input_info, input_cdpg };
		helper.run(
			// on success callback
			[&](std::string_view ipt, std::string_view opt) -> void
			{
				if (enable_log) 
				{
					xcsl::helper.write("[SUCCESSFULLY] \n%s\n\n", opt.data());
				}
			},
			// on failure callback
			[&](std::string_view ipt) -> void
			{
				if (enable_log) 
				{
					xcsl::helper.set_attrs(console::attrs::text_dark_red);
					xcsl::helper.write("[ERROR: %s]\n%s\n\n", helper.get_err_msg().data(), ipt.data());
					xcsl::helper.reset_attrs();
				}
			}
		);

		if (!enable_log)
		{
			//xcsl::helper.clear();
			xcsl::helper.writeline("[COMPLETE]\n");
		}
	}
	catch (const std::exception& err)
	{
		xcsl::helper.set_attrs(console::attrs::text_dark_red);
		xcsl::helper.write("%s\n", err.what());
		xcsl::helper.reset_attrs();
	}
}

auto main(const int argc, const char** args) -> int 
{
	auto time { utils::time_counter<double>([&]() { run(argc, args); }) };
	xcsl::helper.set_attrs(console::attrs::text_dark_yellow);
	xcsl::helper.write("MesTextTool: Ver %s.\n", _tool_version_name_);
	xcsl::helper.write("GitHub: https://github.com/cokkeijigen/MesTextTool\n");
	xcsl::helper.write("Time: %llf\n", time);
	xcsl::helper.reset_attrs();
	xcsl::helper.read_anykey();
	return { 0x114514 };
}