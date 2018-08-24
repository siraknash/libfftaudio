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

#ifndef FFTA__FFTW__H__
#define FFTA__FFTW__H__


#include	<cstdlib>
#include	<vector>
#include	<pthread.h>
#include	<fftw3.h>

#include	"fftaudio_base.h"


//
// FFTAudio implementation for fftw api
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
	 * Returns real^2 + complex^2 of fftwf_complex type at bin index 'bin_index'
	 */
	virtual inline float _get_complex_result(int bin_index) const
	{
		return ((m_outputBuffer[bin_index][0] * m_outputBuffer[bin_index][0])
						+ (m_outputBuffer[bin_index][1] * m_outputBuffer[bin_index][1]));
	}

private:
	void 		_run(int thread_index);
	fftaStatus	_init_threads();

private:
	/*
	 * Thread main function
	 */
	static void *_ffta_fftw_main(void *arg);

private:
	/*
	 * Object passed to work threads as argument on creation
	 */
	class threadArgument
	{
	public:
		threadArgument(FFTAudio *ffta_ptr, int thread_idx)
		{
			tdm_ffta = ffta_ptr;
			tdm_threadIndex = thread_idx;
		}

		virtual ~threadArgument() = default;

	public:
		FFTAudio				*tdm_ffta;
		int						tdm_threadIndex;
	};

	/////////////////////////////////////////////////////////

private:
	std::vector<pthread_t>		m_tids;
	std::vector<fftwf_plan>		m_fftwPlans;
	fftwf_complex				*m_outputBuffer = nullptr;
	const short * const 		*m_inputDataPointers = nullptr;
	size_t						m_done = 0;
	pthread_mutex_t				m_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t				m_ctrlCond = PTHREAD_COND_INITIALIZER;
	pthread_cond_t				m_workCond = PTHREAD_COND_INITIALIZER;

private:
	static pthread_mutex_t		sm_planMutex;
};


#endif // FFTA__FFTW__H__
