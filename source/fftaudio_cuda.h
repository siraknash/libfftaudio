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

#ifndef FFTA__CUDA__H__
#define FFTA__CUDA__H__


#include	<cuda.h>
#include	<cuda_runtime_api.h>
#include	<cufft.h>

#include	"fftaudio_base.h"


//
// FFTAudio implementation for cuda api
//
class FFTAudio : public FFTAudioBase
{
public:
	/*
	 * FFTAudio class constructor
	 *		window_type - fft window type/function, from fftaudio_windows.h
	 *		sample_rate - sample rate, in hz
	 *		frame_size - frame size, in samples
	 *		padded_frame_size - padded frame size, in samples
	 *			Can be 0, in which case padded frame size == frame_size
	 *		batch_count - Number of fft's to perform
	 */
	FFTAudio(FuncInitWindowCB window_type, int sample_rate,
			 int frame_size, int padded_frame_size, int batch_count = 1);

	/*
	 * FFTAudio class destructor
	 */
	virtual ~FFTAudio();

	/*
	 * initialize()
	 *
	 * Initialization function, must be called and succeed prior to calling execute()
	 *
	 *	  Returns fftaStatus, any return value besides FFTA_SUCCESS indicates a
	 *			failure occurred and FFTAudio object becomes unuseable.
	 */
	virtual fftaStatus initialize();

	/*
	 * execute()
	 *
	 * Executes a batch of fft's
	 *
	 * data - Pointer to sample data (signed 16-bit), number of samples must equal
	 *			'frame_size' * 'batch_count'.
	 * data_ptrs - 'batch_count' array of pointers to sample data (signed 16-bit),
	 *			each pointer must point to an array of 'frame_size' samples.
	 */
	virtual bool execute(const short *data);
	virtual bool execute(const short * const *data_ptrs);

protected:
	/*
	 * Returns real^2 + complex^2 of xufftComplex type at bin index 'bin_index'
	 */
	virtual float _get_complex_result(int idx) const
	{
		return ((m_outputBuffer[idx].x * m_outputBuffer[idx].x)
					+ (m_outputBuffer[idx].y * m_outputBuffer[idx].y));
	}

private:
	cudaStream_t			m_stream = nullptr;
	cufftHandle				m_cudaPlan = 0;
	float					*m_cudaInputBuffer = nullptr;
	cufftComplex			*m_outputBuffer = nullptr;
	cufftComplex			*m_cudaOutputBuffer = nullptr;
};

#endif // FFTA__CUDA__H__
