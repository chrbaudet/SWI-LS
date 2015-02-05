# /******************************************************************************/
# /*                                                                            */
# /*   This file is part of SWI-LS.                                             */
# /*                                                                            */
# /*   SWI-LS is free software: you can redistribute it and/or modify           */
# /*   it under the terms of the GNU General Public License as published by     */
# /*   the Free Software Foundation, either version 2 of the License, or        */
# /*   any later version.                                                       */
# /*                                                                            */
# /*   SWI-LS is distributed in the hope that it will be useful,                */
# /*   but WITHOUT ANY WARRANTY; without even the implied warranty of           */
# /*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
# /*   GNU General Public License for more details.                             */
# /*                                                                            */
# /*   You should have received a copy of the GNU General Public License        */
# /*   along with SWI-LS.  If not, see <http://www.gnu.org/licenses/>.          */
# /*                                                                            */
# /******************************************************************************/

# Makefile : SWI-LS : Heuristics

.SUFFIXES : .hpp .cpp .d .o

CPP=c++

STDLIB=c++11

CFLAGS=-Wall -g -O2 -std=$(STDLIB)

INCLUDES=-Iheaders

LIBRARIES=

BASICSOURCES=sources/permutation/permutation.cpp sources/problems/problems.cpp \
	sources/heuristics/heuristics.cpp

SOURCES1=$(BASICSOURCES) sources/exec/processBinaryDatabase.cpp

SOURCES2=$(BASICSOURCES) sources/exec/processPermutation.cpp

SOURCES3=sources/exec/statistics.cpp

EXECUTABLE1=processBinaryDatabase

EXECUTABLE2=processPermutation

EXECUTABLE3=statistics

OBJECTS1=$(SOURCES1:.cpp=.o)

OBJECTS2=$(SOURCES2:.cpp=.o)

OBJECTS3=$(SOURCES3:.cpp=.o)

DEPENDENCIES=$(BASICSOURCES:.cpp=.d)

.cpp.d:
	@$(CPP) $(INCLUDES) -std=$(STDLIB) -MM $< > $@

.cpp.o:
	@echo "---------------------------------------------------------------------------"
	@echo
	$(CPP) $(INCLUDES) $(CFLAGS) -c $< -o $@
	@echo
	@echo "---------------------------------------------------------------------------"
	@echo

all: $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3)

$(EXECUTABLE1): $(OBJECTS1) $(DEPENDENCIES)
	@echo "---------------------------------------------------------------------------"
	@echo
	$(CPP) $(INCLUDES) $(CFLAGS) $(OBJECTS1) -o $(EXECUTABLE1) $(LIBRARIES)
	@echo
	@echo "---------------------------------------------------------------------------"
	@echo

$(EXECUTABLE2): $(OBJECTS2) $(DEPENDENCIES)
	@echo "---------------------------------------------------------------------------"
	@echo
	$(CPP) $(INCLUDES) $(CFLAGS) $(OBJECTS2) -o $(EXECUTABLE2) $(LIBRARIES)
	@echo
	@echo "---------------------------------------------------------------------------"
	@echo

$(EXECUTABLE3): $(OBJECTS3)
	@echo "---------------------------------------------------------------------------"
	@echo
	$(CPP) $(INCLUDES) $(CFLAGS) $(OBJECTS3) -o $(EXECUTABLE3) $(LIBRARIES)
	@echo
	@echo "---------------------------------------------------------------------------"
	@echo

clean:
	@echo "Cleaning-up the mess..."
	@rm -f $(OBJECTS1) $(EXECUTABLE1)
	@rm -f $(OBJECTS2) $(EXECUTABLE2)
	@rm -f $(OBJECTS3) $(EXECUTABLE3)
	@rm -f $(DEPENDENCIES) *~
	@echo "Done!"

-include $(DEPENDENCIES)
