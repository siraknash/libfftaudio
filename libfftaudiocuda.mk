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
ProjectName            :=libfftaudiocuda
Preprocessors          :=-DUSE_CUDA_API 
CXXFLAGS               :=-Wall -std=c++11 -fPIC $(Preprocessors)
CFLAGS                 :=-Wall -fPIC $(Preprocessors)

ifdef DEBUG

CXXFLAGS               += -g -O0
CFLAGS                 += -g -O0

else

CXXFLAGS               += -O3 -fassociative-math -freciprocal-math -ffast-math
CFLAGS                 += -O3 -fassociative-math -freciprocal-math -ffast-math

endif

CudaPath               :=/usr/local/cuda
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
IncludePath            := $(IncludeSwitch). $(IncludeSwitch)./source $(IncludeSwitch)./include $(IncludeSwitch)$(CudaPath)/include 
LibPath                := $(LibraryPathSwitch)$(CudaPath)/lib64
SharedLibs             := $(LibrarySwitch)cudart $(LibrarySwitch)cufft 
SharedLinkerOptions    := -Wl,-rpath=$(CudaPath)/lib64
StaticLibs             := $(CudaPath)/lib64/libcudart_static.a $(CudaPath)/lib64/libcufft_static.a
StaticLinkerOptions    :=

##
## User defined environment variables
##
Objects=$(IntermediateDirectory)/fftaudio_windows.cpp$(ObjectSuffix) $(IntermediateDirectory)/fftaudio_cuda.cpp$(ObjectSuffix) $(IntermediateDirectory)/fftaudio_base.cpp$(ObjectSuffix) 

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

$(IntermediateDirectory)/fftaudio_cuda.cpp$(ObjectSuffix): source/fftaudio_cuda.cpp $(IntermediateDirectory)/fftaudio_cuda.cpp$(DependSuffix)
	$(CXX) $(SourceSwitch) "./source/fftaudio_cuda.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fftaudio_cuda.cpp$(ObjectSuffix) $(IncludePath)

$(IntermediateDirectory)/fftaudio_cuda.cpp$(DependSuffix): source/fftaudio_cuda.cpp
	$(CXX) $(CXXFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fftaudio_cuda.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fftaudio_cuda.cpp$(DependSuffix) -MM source/fftaudio_cuda.cpp

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

