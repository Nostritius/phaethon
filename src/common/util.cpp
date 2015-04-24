/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

// Mostly copied verbatim from ScummVM's

/** @file
 *  Utility templates and functions.
 */

#include "src/common/util.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

void warning(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs("WARNING: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);
#endif
}

void status(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs(buf, stderr);
	std::fputs("\n", stderr);
#endif
}

void NORETURN_PRE error(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs("ERROR: ", stderr);
	std::fputs(buf, stderr);
	std::fputs("!\n", stderr);
#endif

	std::exit(1);
}


// We just directly convert here because most systems have float in IEEE 754-1985
// format anyway. However, should we find another system that has this differently,
// we might have to do something more here...

union floatConvert {
	uint32 dInt;
	float dFloat;
};

float convertIEEEFloat(uint32 data) {

	floatConvert conv;

	conv.dInt = data;

	return conv.dFloat;
}

uint32 convertIEEEFloat(float value) {
	floatConvert conv;

	conv.dFloat = value;

	return conv.dInt;
}


// We just directly convert here because most systems have double in IEEE 754-1985
// format anyway. However, should we find another system that has this differently,
// we might have to do something more here...

union doubleConvert {
	uint64 dInt;
	double dDouble;
};

double convertIEEEDouble(uint64 data) {
	doubleConvert conv;

	conv.dInt = data;

	return conv.dDouble;
}

uint64 convertIEEEDouble(double value) {
	doubleConvert conv;

	conv.dDouble = value;

	return conv.dInt;
}
