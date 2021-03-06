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

# GUI code using Qt5.

noinst_LTLIBRARIES += src/gui/libgui.la

src_gui_libgui_la_SOURCES =

src_gui_libgui_la_SOURCES += \
    src/gui/icons.h \
    src/gui/mainwindow.h \
    src/gui/resourcetree.h \
    src/gui/resourcetreeitem.h \
    src/gui/proxymodel.h \
    src/gui/statusbar.h \
    src/gui/panelresourceinfo.h \
    src/gui/panelpreviewempty.h \
    src/gui/panelpreviewimage.h \
    src/gui/panelpreviewsound.h \
    src/gui/panelpreviewtext.h \
    $(EMPTY)

src_gui_libgui_la_SOURCES += \
    src/gui/icons.cpp \
    src/gui/mainwindow.cpp \
    src/gui/resourcetree.cpp \
    src/gui/resourcetreeitem.cpp \
    src/gui/proxymodel.cpp \
    src/gui/statusbar.cpp \
    src/gui/panelresourceinfo.cpp \
    src/gui/panelpreviewempty.cpp \
    src/gui/panelpreviewimage.cpp \
    src/gui/panelpreviewsound.cpp \
    src/gui/panelpreviewtext.cpp \
    $(EMPTY)
