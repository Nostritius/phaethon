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

/** @file
 *  Command line handling.
 */

#include "src/common/version.h"

#include "src/cline.h"

Job parseCommandLine(int argc, char **argv) {
	Job job;

	// No options at all means we operate on an empty path
	job.operation = kOperationPath;

	// Go through all arguments
	for (int i = 1; i < argc; i++) {
		// Find --help and --version
		if        (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			job.operation = kOperationHelp;
			break;
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
			job.operation = kOperationVersion;
			break;
		}

		// We only allow one path, so a second one makes the command line invalid
		if (!job.path.empty()) {
			job.operation = kOperationInvalid;
			break;
		}

		job.path = argv[i];
	}

	return job;
}

Common::UString createVersionText() {
	Common::UString text;

	text += Common::UString::format("%s\n", PHAETHON_NAMEVERSIONFULL);
	text += Common::UString::format("%s\n", PHAETHON_URL);
	text += Common::UString::format("\n");
	text += Common::UString::format("Copyright (c) %s by the following:\n", PHAETHON_COPYRIGHTYEAR);
	text += Common::UString::format("\n");
	text += Common::UString::format("%s\n", PHAETHON_COPYRIGHTAUTHOR);
	text += Common::UString::format("\n");
	text += Common::UString::format("%s is free software; you can redistribute it and/or\n", PHAETHON_NAME);
	text += Common::UString::format("modify it under the terms of the GNU General Public License\n");
	text += Common::UString::format("as published by the Free Software Foundation; either version 3\n");
	text += Common::UString::format("of the License, or (at your option) any later version.\n");
	text += Common::UString::format("\n");
	text += Common::UString::format("%s is distributed in the hope that it will be useful,\n", PHAETHON_NAME);
	text += Common::UString::format("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	text += Common::UString::format("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n");
	text += Common::UString::format("GNU General Public License for more details.\n");
	text += Common::UString::format("\n");
	text += Common::UString::format("You should have received a copy of the GNU General Public License\n");
	text += Common::UString::format("along with %s. If not, see <http://www.gnu.org/licenses/>.\n", PHAETHON_NAME);
	text += Common::UString::format("\n");
	text += Common::UString::format("The Infinity engine, Aurora engine, Odyssey engine and Eclipse engine are\n");
	text += Common::UString::format("registered trademarks of BioWare Corp., a subsidiary of Electronic Arts, Inc.\n");
	text += Common::UString::format("The Electron engine and Lycium engine are trademarks of BioWare Corp., a\n");
	text += Common::UString::format("subsidiary of Electronic Arts, Inc. Bink is a registered trademark of RAD Game\n");
	text += Common::UString::format("Tools, Inc. Neverwinter Nights is a registered trademark of Wizards of the\n");
	text += Common::UString::format("Coast, a subsidiary of Hasbro Inc. Star Wars, Knights of the Old Republic and\n");
	text += Common::UString::format("The Sith Lords are registered trademarks of Lucasfilm Entertainment Company\n");
	text += Common::UString::format("Ltd., a subsidiary of The Walt Disney Company. Jade Empire is a registered\n");
	text += Common::UString::format("trademark of BioWare Corp., a subsidiary of Electronic Arts, Inc. Sonic the\n");
	text += Common::UString::format("Hedgehog is a registered trademark of Sega Corporation. The Witcher is a\n");
	text += Common::UString::format("registered trademark of CD Projekt S.A. Dragon Age is a registered trademark\n");
	text += Common::UString::format("of Electronic Arts, Inc. All rights reserved.");

	return text;
}

Common::UString createHelpText(const char *name) {
	Common::UString text;

	text += Common::UString::format("%s - A FLOSS resource explorer for BioWare's Aurora engine games\n", PHAETHON_NAME);
	text += Common::UString::format("Usage: %s [options] [<path>]\n", name);
	text += Common::UString::format("  -h      --help              Display this text and exit.\n");
	text += Common::UString::format("  -v      --version           Display version information and exit.");

	return text;
}
