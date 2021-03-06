# Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
#
# Phaethon is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# Phaethon is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# Phaethon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Phaethon. If not, see <http://www.gnu.org/licenses/>.

# Aurora file loaders and related support code.

noinst_LTLIBRARIES += src/aurora/libaurora.la
src_aurora_libaurora_la_SOURCES =

src_aurora_libaurora_la_SOURCES += \
    src/aurora/types.h \
    src/aurora/util.h \
    src/aurora/language.h \
    src/aurora/language_strings.h \
    src/aurora/locstring.h \
    src/aurora/aurorafile.h \
    src/aurora/archive.h \
    src/aurora/zipfile.h \
    src/aurora/erffile.h \
    src/aurora/rimfile.h \
    src/aurora/keyfile.h \
    src/aurora/keydatafile.h \
    src/aurora/biffile.h \
    src/aurora/bzffile.h \
    $(EMPTY)

src_aurora_libaurora_la_SOURCES += \
    src/aurora/util.cpp \
    src/aurora/language.cpp \
    src/aurora/locstring.cpp \
    src/aurora/aurorafile.cpp \
    src/aurora/archive.cpp \
    src/aurora/zipfile.cpp \
    src/aurora/erffile.cpp \
    src/aurora/rimfile.cpp \
    src/aurora/keyfile.cpp \
    src/aurora/keydatafile.cpp \
    src/aurora/biffile.cpp \
    src/aurora/bzffile.cpp \
    $(EMPTY)
