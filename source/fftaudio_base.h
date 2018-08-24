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

#ifndef FFTA__BASE__H__
#define FFTA__BASE__H__


#include	<values.h>
#include	<fftaudio_status.h>


class FFTAudioBase
{
public:
	/*
	 * FuncInitWindowCB Type
	 *
	 * Callback function type for fft window functions
	 *	  void window_func(int frame_size, float &window_sum, float *output)
	 *		frame_size - size of unpadded fft frame, in samples
	 *		window_sum - sum of frame sample multipliers
	 *		output - array of 'frame_size' containing frame sample multipliers
	 */
	typedef	void (*FuncInitWindowCB)(int, float &, float *);

	/*
	 * FuncGetBinCB Type
	 *
	 * Callback function type for post-processing bin results.  Called whenever
	 * getBinValue() is called to allow modification of raw result.
	 *		void get_bin_cb(int bin_index, float &bin_value)
	 *			bin_index - index of bin (0 --> 'padded_frame_size' / 2)
	 *			bin_value - input/output of bin result value
	 *			user_ptr - user pointer associated with callback
	 */
	typedef	void (*FuncGetBinCB)(int, float &, void *);

protected:
	/*
	 * FFTAudioBase class constructor
	 *		window_type - fft window type/function, from fftaudio_windows.h
	 *		sample_rate - sample rate, in hz
	 *		frame_size - frame size, in samples
	 *		padded_frame_size - padded frame size, in samples
	 *			Can be 0, in which case padded frame size == frame_size
	 *		batch_count - Number of fft's to perform
	 */
	FFTAudioBase(FuncInitWindowCB window_type, int sample_rate,
				 int frame_size, int padded_frame_size, int batch_count);

public:
	/*
	 * FFTAudioBase class destructor
	 */
	virtual ~FFTAudioBase();

protected:
	/*
	 * initialize()
	 *
	 * Initialization function, must be called and succeed prior to calling execute()
	 *
	 *	  Returns fftaStatus, any return value besides FFTA_SUCCESS indicates a
	 *			failure occurred and FFTAudio object becomes unuseable.
	 */
	virtual fftaStatus initialize();

public:
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
	virtual bool execute(const short *data) = 0;
	virtual bool execute(const short * const *data_ptrs) = 0;

	/*
	 * getBinValue()
	 *
	 * Retrieves a bin result value after execute() is called
	 */
	float getBinValue(int bin) const;
	float getBinValue(int batch_idx, int bin) const;

	/*
	 * setGetBinValueUserCallback()
	 *
	 * Sets optional user callback for when getBinValue() is called.  This allows
	 * additional processing to be performed before the result value is returned.
	 *
	 * The bin values are automatically processed by the following:
	 *		p1 = real^2 + complex^2
	 *		p2 = sqrt(p1);
	 *		p3 = p2 * 2.0
	 *		p4 = p3 * window_sum
	 *
	 * The callback is called after the automatic post-processing is done.
	 *
	 * cb_func - 'FuncGetBinCB' callback function
	 * user_ptr - optinial user-specified pointer passed to callback function
	 */
	void setGetBinValueUserCallback(FuncGetBinCB cb_func, void *user_ptr = nullptr)
	{
		m_getBinCallback = cb_func;
		m_getBinCallbackUserPointer = user_ptr;
	}

	int getSampleRate() const						{ return m_sampleRate;					}
	int getFrameSize() const						{ return m_frameSize;					}
	int getPaddedFrameSize() const					{ return m_paddedFrameSize;				}
	int getBatchCount() const						{ return m_batchCount;					}
	int getBinCount() const							{ return m_binCount;					}
	float getBinFrequency(int bin) const			{ return (float)bin * m_frequencyStep;	}

protected:
	virtual float _prepare_input_value(int frame_index, short sample_value)
	{
		float ret = (float)sample_value;

		ret /= (float)MAXSHORT + 1.0f;
		ret *= m_windowValues[frame_index];
		return ret;
	}

	virtual float _get_complex_result(int idx) const = 0;

protected:
	bool					m_initialized = false;
	bool					m_initializeFailed = false;
	float					*m_inputBuffer = nullptr;

private:
	int						m_sampleRate = 0;
	int						m_frameSize = 0;
	int						m_paddedFrameSize = 0;
	int						m_batchCount = 0;
	int						m_binCount = 0;
	float					m_frequencyStep = 0.0f;
	float					*m_windowValues = nullptr;
	float					m_windowSum = 0.0f;
	FuncInitWindowCB		m_windowInitCallback = nullptr;
	FuncGetBinCB			m_getBinCallback = nullptr;
	void					*m_getBinCallbackUserPointer = nullptr;
};

#endif // FFTA__BASE__H__

