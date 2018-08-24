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

#include	<cstdlib>
#include	<cstdint>
#include	<cstring>
#include	<math.h>
#include	<values.h>
#include	<vector>

#include	<fftaudio_base.h>
#include	<fftaudio_status.h>
#include	<fftaudio_windows.h>


/***************************************************************
 * FFTAudioBase Constructor Implementation
 ***************************************************************/

FFTAudioBase::FFTAudioBase(FuncInitWindowCB window_type, int sample_rate,
						   int frame_size, int padded_frame_size, int batch_count)
{
	m_sampleRate = sample_rate;
	m_frameSize = frame_size;
	m_paddedFrameSize = padded_frame_size;
	m_batchCount = batch_count;
	m_binCount = m_paddedFrameSize / 2;
	m_frequencyStep = (float)m_sampleRate / (float)m_paddedFrameSize;
	m_windowInitCallback = window_type;
	m_getBinCallback = nullptr;
	m_getBinCallbackUserPointer = nullptr;
}


/***************************************************************
 * FFTAudioBase Destructor Implementation
 ***************************************************************/

FFTAudioBase::~FFTAudioBase()
{
	if(m_windowValues != nullptr) {
		::free(m_windowValues);
	}

	// Note: m_inputBuffer is a member of this base class, but is allocated
	// and free'd in the derived classes as they have their own custom
	// allocation functions.
}


/***************************************************************
 * FFTAudioBase::initialize()
 ***************************************************************/

fftaStatus
FFTAudioBase::initialize()
{
	/*
	 * Previous initialization failed
	 */
	if(m_initializeFailed) {
		return FFTA_PREVIOUS_INITIALIZE_FAILED;
	}

	/*
	 * Already initialized, this function was already called
	 */
	if(m_initialized) {
		return FFTA_ALREADY_INITIALIZED;
	}

	/*
	 * If padded frame size is 0, set it to match frame size, otherwise padded
	 * frame size is less than frame size, which is invalid.
	 */
	if(m_paddedFrameSize == 0) {
		m_paddedFrameSize = m_frameSize;
	}
	else if(m_paddedFrameSize < m_frameSize) {
		m_initializeFailed = true;
		return FFTA_INVALID_ARGUMENT;
	}

	m_windowValues = (float *)::malloc(m_frameSize * sizeof(float));
	if(m_windowValues == nullptr) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	::memset(m_windowValues, 0, m_frameSize * sizeof(float));

	/*
	 * If window initialization function is null, set to Rectangle
	 */
	if(m_windowInitCallback == nullptr) {
		m_windowInitCallback = fftaWindow::Rectangle;
	}

	/*
	 * Call the specified window initialization function to initialize the
	 * m_windowSum and m_windowValues variables.
	 */
	(*m_windowInitCallback)(m_frameSize, m_windowSum, m_windowValues);
	return FFTA_SUCCESS;
}


/***************************************************************
 * FFTAudioBase::getBinValue()
 ***************************************************************/

float
FFTAudioBase::getBinValue(int batch_index, int bin_index) const
{
	int		idx;
	float	ret;

	idx = (batch_index * (m_binCount + 1)) + bin_index;

	/*
	 * Call virtual function _get_complex_result(), which uses the underlying
	 * api's complex type and returns: real^2 + complex^2
	 */
	ret = this->_get_complex_result(idx);

	/*
	 * Default bin result post-processing
	 */
	ret = sqrtf(ret);
	ret *= 2.0f;
	ret /= m_windowSum;

	/*
	 * Call user-specified post-processing function if set
	 */
	if(m_getBinCallback != nullptr) {
		(*m_getBinCallback)(bin_index, ret, m_getBinCallbackUserPointer);
	}

	return ret;
}


float
FFTAudioBase::getBinValue(int bin_index) const
{
	return getBinValue(0, bin_index);
}
