#pragma once

#include <cstdint>

namespace Colors {

enum class Gruvbox: uint64_t {
	bg0 = 0x282828ff,
	bg1 = 0x32302Fff,
	bg2 = 0x45403Dff,
	bg3 = 0x5A524Cff,
	bg4 = 0x3A3735ff,
	bg5 = 0x504945ff,

	fg = 0xe2cca9ff,

	red = 0xf2594bff,
	bg_red = 0xdb4740ff,

	yellow = 0xe9b143ff,

	green = 0xb0b846ff,

	aqua = 0x8bba7fff,

	blue = 0x80aa9eff,

	purple = 0xd3869bff,

	grey0 = 0x7c6f64ff,
	grey1 = 0x928374ff,
	grey2 = 0xa89984ff,
};

}
