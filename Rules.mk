#
# file: Rules.mk
#
# author: Copyright (C) 2016 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
# distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

#-----------------------------------------------------------------------------------------------------------------------
# subdirectories
#-----------------------------------------------------------------------------------------------------------------------

SUBDIRECTORIES += $(DISTORTOS_PATH:%/=%)

#-----------------------------------------------------------------------------------------------------------------------
# compilation flags
#-----------------------------------------------------------------------------------------------------------------------

CXXFLAGS_$(d) := $(CXXFLAGS_$(d)) $(STANDARD_INCLUDES)
CXXFLAGS_$(d) := $(CXXFLAGS_$(d)) $(CHIP_INCLUDES)

#-----------------------------------------------------------------------------------------------------------------------
# standard footer
#-----------------------------------------------------------------------------------------------------------------------

include $(DISTORTOS_PATH)footer.mk

#-----------------------------------------------------------------------------------------------------------------------
# .elf file dependencies - libdistortos.a, all objects from this folder tree (excluding distortos subfolder), linker
# script and this Rules.mk
#-----------------------------------------------------------------------------------------------------------------------

$(ELF_$(d)): $(OUTPUT)libdistortos.a $(OBJECTS_$(d)) $(LDSCRIPT) $(d)Rules.mk
$(ELF_$(d)): $(filter-out $(SUBDIRECTORIES_OBJECTS_$(DISTORTOS_PATH)),$(SUBDIRECTORIES_OBJECTS_$(d)))
