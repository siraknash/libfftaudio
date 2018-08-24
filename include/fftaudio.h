///////////////////////////////////////////////////////////////////////////
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Library to provide a common interface (fftw3, nvidia cuda) for performing
// fft on audio data.  An executable can switch between versions without any
// coude changes, by just linking to a different library.
//
// See 'readme.api" for brief api description, 'fftaudio_base.h' has more
// detailed descriptions.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef FFTA__EXTERN__H__
#define FFTA__EXTERN__H__

//
// Choose implementation by USE_API_CUDA (or USE_API_FFTW) defines
//

#ifdef USE_API_CUDA
	#include	<../source/fftaudio_cuda.h>
#else
	#include	<../source/fftaudio_fftw.h>
#endif

#include	<fftaudio_status.h>
#include	<fftaudio_windows.h>

#endif // FFTA__EXTERN__H__

