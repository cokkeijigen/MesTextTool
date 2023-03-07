#pragma once

struct keysection {
	byte start = 0;
	byte end = 0;
	bool with(byte key) {
		return ((!(start == end && start == 0xFF)) && (key >= start && key <= end));
	}
};

struct mesconf {
	uint16_t head_t;
	char name[10];
	keysection uint8x2;
	keysection uint8str;
	keysection str;
	keysection decstr;
	keysection shortx4;
	byte optundec;
};

std::vector<mesconf*> mesConfigs;

void initConf() {

	mesConfigs.push_back(
		new mesconf{
			0x6331, "dcbs",
			{0x00, 0x2B},
			{0xFF, 0xFF}, // 0xFFռλ
			{0x2C, 0x48},
			{0x49, 0x4C},
			{0x4D, 0xFF},
			0x00 // ռλ
		});

	mesConfigs.push_back(
		new mesconf{
			0x6C31, "dc2bs",
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			0x00 // ռλ
		});

	mesConfigs.push_back(
		new mesconf{
			0x729D, "dc2dm",
			{0x00, 0x29},
			{0x2A, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			0x44
		});

	mesConfigs.push_back(
		new mesconf{
			0x6638, "dc2fy2",
			{0x00, 0x2E},
			{0xFF, 0xFF}, // 0xFFռλ
			{0x2F, 0x4B},
			{0x4C, 0x4F},
			{0x50, 0xFF},
			0x48
		});

	mesConfigs.push_back(
		new mesconf{
			0x6C02, "dc2cckko",
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			0x00 // ռλ
		});

	mesConfigs.push_back(
		new mesconf{
			0x6C01, "dc2ccotm",
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			0x00 // ռλ
		});

	mesConfigs.push_back(
		new mesconf{
			0x693B, "dc2sc",
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			0x45
		});

	mesConfigs.push_back(
		new mesconf{
			0x695F, "dc2ty",
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			0x00
		});

	mesConfigs.push_back(
		new mesconf{
			0x6957, "dc2pc",
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			0x45
		});

	mesConfigs.push_back(
		new mesconf{
			0x7297, "dc3rx",
			{0x00, 0x2B},
			{0x2C, 0x33},
			{0x34, 0x4E},
			{0x4F, 0x52},
			{0x53, 0xFF},
			0x45
		});

	mesConfigs.push_back(
		new mesconf{
			0x7298, "dc3pp",
			{0x00, 0x2A},
			{0x2B, 0x32},
			{0x33, 0x4E},
			{0x4F, 0x51},
			{0x52, 0xFF},
			0x45
		});
	
	mesConfigs.push_back(
		new mesconf{
			0x9FA0, "dc3wy",
			{0x00, 0x38},
			{0x39, 0x41},
			{0x42, 0x5F},
			{0x60, 0x63},
			{0x64, 0xFF},
			0x55
		});

	mesConfigs.push_back(
		new mesconf{
			0xA8A5, "dc3dd",
			{0x00, 0x38},
			{0x39, 0x43},
			{0x44, 0x62},
			{0x63, 0x67},
			{0x68, 0xFF},
			0x58
		});

	mesConfigs.push_back(
		new mesconf{
			0xB6AA, "dc4",
			{0x00, 0x3A},
			{0x3B, 0x47},
			{0x48, 0x68},
			{0x69, 0x6D},
			{0x6E, 0xFF},
			0x5d
		});

}