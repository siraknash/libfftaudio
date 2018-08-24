##/////////////////////////////////////////////////////////////////////////
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <https://www.gnu.org/licenses/>.
##
##/////////////////////////////////////////////////////////////////////////

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
AR       := /usr/bin/ar

ifdef DEBUG
BuildType	:=Debug
else
BuildType	:=Release
endif

##
## Build Configuration
##
ProjectName            :=libfftaudiofftw
Preprocessors          :=-DUSE_FFTW_API 
CXXFLAGS               :=-Wall -std=c++11 -fPIC $(Preprocessors)
CFLAGS                 :=-Wall -fPIC $(Preprocessors)

ifdef DEBUG

CXXFLAGS               += -g -O0
CFLAGS                 += -g -O0

else

CXXFLAGS               += -O3 -fassociative-math -freciprocal-math -ffast-math
CFLAGS                 += -O3 -fassociative-math -freciprocal-math -ffast-math

endif

ConfigurationName      :=$(BuildType)
IntermediateDirectory  :=./$(BuildType)
OutDir                 :=$(IntermediateDirectory)
SharedOutputFile       :=./$(BuildType)/${ProjectName}.so
StaticOutputFile       :=./$(BuildType)/${ProjectName}.a
SharedLinkerName       :=$(CXX)
SharedLinkerFlags      :=-shared -fPIC
StaticLinkerName       :=$(AR)
StaticLinkerFlags      :=rcs
ObjectSuffix           :=.o
DependSuffix           :=.o.d
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
SourceSwitch           :=-c 
ObjectSwitch           :=-o 
MakeDirCommand         :=mkdir -p
IncludePath            :=$(IncludeSwitch). $(IncludeSwitch)./source $(IncludeSwitch)./include
LibPath                :=
SharedLibs             :=$(LibrarySwitch)fftw3f
SharedLinkerOptions    :=
StaticLibs             :=/usr/lib/x86_64-linux-gnu/libfftw3.a
StaticLinkerOptions    :=

##
## User defined environment variables
##
Objects=$(IntermediateDirectory)/fftaudio_windows.cpp$(ObjectSuffix) $(IntermediateDirectory)/fftaudio_fftw.cpp$(ObjectSuffix) $(IntermediateDirectory)/fftaudio_base.cpp$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean MakeIntermediateDirs
all: $(SharedOutputFile)

$(SharedOutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	$(SharedLinkerName) $(SharedLinkerFlags) $(OutputSwitch)$(SharedOutputFile) $(Objects) $(LibPath) $(SharedLibs) $(SharedLinkerOptions)

MakeIntermediateDirs:
	@test -d ./$(BuildType) || $(MakeDirCommand) ./$(BuildType)


$(IntermediateDirectory)/.d:
	@test -d ./$(BuildType) || $(MakeDirCommand) ./$(BuildType)

##
## Objects
##
$(IntermediateDirectory)/fftaudio_windows.cpp$(ObjectSuffix): source/fftaudio_windows.cpp $(IntermediateDirectory)/fftaudio_windows.cpp$(DependSuffix)
	$(CXX) $(SourceSwitch) "./source/fftaudio_windows.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fftaudio_windows.cpp$(ObjectSuffix) $(IncludePath)

$(IntermediateDirectory)/fftaudio_windows.cpp$(DependSuffix): source/fftaudio_windows.cpp
	$(CXX) $(CXXFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fftaudio_windows.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fftaudio_windows.cpp$(DependSuffix) -MM source/fftaudio_windows.cpp

$(IntermediateDirectory)/fftaudio_fftw.cpp$(ObjectSuffix): source/fftaudio_fftw.cpp $(IntermediateDirectory)/fftaudio_fftw.cpp$(DependSuffix)
	$(CXX) $(SourceSwitch) "./source/fftaudio_fftw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fftaudio_fftw.cpp$(ObjectSuffix) $(IncludePath)

$(IntermediateDirectory)/fftaudio_fftw.cpp$(DependSuffix): source/fftaudio_fftw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fftaudio_fftw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fftaudio_fftw.cpp$(DependSuffix) -MM source/fftaudio_fftw.cpp

$(IntermediateDirectory)/fftaudio_base.cpp$(ObjectSuffix): source/fftaudio_base.cpp $(IntermediateDirectory)/fftaudio_base.cpp$(DependSuffix)
	$(CXX) $(SourceSwitch) "./source/fftaudio_base.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fftaudio_base.cpp$(ObjectSuffix) $(IncludePath)

$(IntermediateDirectory)/fftaudio_base.cpp$(DependSuffix): source/fftaudio_base.cpp
	$(CXX) $(CXXFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fftaudio_base.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fftaudio_base.cpp$(DependSuffix) -MM source/fftaudio_base.cpp

-include $(IntermediateDirectory)/*$(DependSuffix)

##
## Clean
##
clean:
	$(RM) -r ./Debug/
	$(RM) -r ./Release/

