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
#include	<cstring>
#include	<math.h>
#include	<values.h>
#include	<vector>

#include	<cuda.h>
#include	<cuda_runtime_api.h>

#include	<fftaudio_cuda.h>


/***************************************************************
 * FFTAudio Constructor (cuda)
 ***************************************************************/

FFTAudio::FFTAudio(FuncInitWindowCB window_type, int sample_rate,
				   int frame_size, int padded_frame_size, int batch_count) :
	FFTAudioBase(window_type, sample_rate, frame_size, padded_frame_size, batch_count)
{
}


/***************************************************************
 * FFTAudio Destructor (cuda)
 ***************************************************************/

FFTAudio::~FFTAudio()
{
	if(m_cudaPlan != 0) {
		cufftDestroy(m_cudaPlan);
	}

	if(m_stream != nullptr) {
		cudaStreamDestroy(m_stream);
	}

	if(m_cudaOutputBuffer != NULL) {
		cudaFree(m_outputBuffer);
	}

	if(m_cudaInputBuffer != NULL) {
		cudaFree(m_cudaInputBuffer);
	}

	if(m_inputBuffer != NULL) {
		cudaFreeHost(m_inputBuffer);
	}

	if(m_outputBuffer != NULL) {
		cudaFreeHost(m_outputBuffer);
	}
}


/***************************************************************
 * FFTAudio::initialize()
 ***************************************************************/

fftaStatus
FFTAudio::initialize()
{
	fftaStatus		ret;
	size_t			alloc_sz;
	int				padded_frame_sz;

	if((ret = FFTAudioBase::initialize()) != FFTA_SUCCESS) {
		return ret;
	}

	/*
	 * Create cuda stream
	 */
	if(cudaStreamCreateWithFlags(&m_stream, cudaStreamNonBlocking) != cudaSuccess) {
		m_initializeFailed = true;
		return FFTA_TRANSPORT_CREATE_FAILED;
	}

	/*
	 * Allocate input buffers on host and on cuda device
	 */
	alloc_sz = (size_t)(this->getBatchCount() * this->getPaddedFrameSize() * sizeof(float));

	if(cudaMalloc((void **)&m_cudaInputBuffer, alloc_sz) != cudaSuccess) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	if(cudaMallocHost((void **)&m_inputBuffer, alloc_sz) != cudaSuccess) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	cudaMemsetAsync(m_cudaInputBuffer, 0, alloc_sz, m_stream);
	::memset(m_inputBuffer, 0, alloc_sz);

	/*
	 * Allocate output buffers on host and on cuda device
	 */
	alloc_sz = (size_t)(this->getBatchCount() * (this->getBinCount() + 1) * sizeof(cufftComplex));

	if(cudaMalloc((void **)&m_cudaOutputBuffer, alloc_sz) != cudaSuccess) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	if(cudaMallocHost((void **)&m_outputBuffer, alloc_sz) != cudaSuccess) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	cudaMemsetAsync(m_cudaOutputBuffer, 0, alloc_sz, m_stream);
	::memset(m_outputBuffer, 0, alloc_sz);

	/*
	 * Create cuda plan
	 */
	padded_frame_sz = this->getPaddedFrameSize();

	if(cufftPlanMany(&m_cudaPlan, 1, &padded_frame_sz, NULL, 0, 0,
					 NULL, 0, 0, CUFFT_R2C, this->getBatchCount()) != CUFFT_SUCCESS) {
		m_initializeFailed = true;
		return FFTA_PLAN_CREATE_FAILED;
	}

	cufftSetStream(m_cudaPlan, m_stream);
	m_initialized = true;
	return FFTA_SUCCESS;
}


//////////////////////////////////////////////////////////////////////

bool
FFTAudio::execute(const short *data)
{
	std::vector<const short *>	data_ptrs;

	for(int i = 0; i < this->getBatchCount(); ++i) {
		data_ptrs.push_back(&data[i * this->getPaddedFrameSize()]);
	}

	return this->execute(data_ptrs.data());
}


bool
FFTAudio::execute(const short * const *data_ptrs)
{
	if(!m_initialized) {
		return false;
	}

	int		frame_start_idx;
	size_t	mem_sz;

	for(int i = 0; i < this->getBatchCount(); ++i) {
		frame_start_idx = i * this->getPaddedFrameSize();

		for(int j = 0; j < this->getFrameSize(); ++j) {
			m_inputBuffer[frame_start_idx + j] = this->_prepare_input_value(j, data_ptrs[i][j]);
		}
	}

	/*
	 * Copy input to device, execute, and copy output to host.
	 * Synchronize the stream so output buffer is gauranteed to be populated before returning.
	 */
	mem_sz = (size_t)(this->getBatchCount() * this->getPaddedFrameSize() * sizeof(float));
	cudaMemcpyAsync(&m_cudaInputBuffer[0], &m_inputBuffer[0], mem_sz, cudaMemcpyHostToDevice, m_stream);

	cufftExecR2C(m_cudaPlan, m_cudaInputBuffer, m_cudaOutputBuffer);

	mem_sz = (size_t)(this->getBatchCount() * (this->getBinCount() + 1) * sizeof(cufftComplex));
	cudaMemcpyAsync(&m_outputBuffer[0], &m_cudaOutputBuffer[0], mem_sz,  cudaMemcpyDeviceToHost, m_stream);

	cudaStreamSynchronize(m_stream);
	return true;
}


/////////////////////////////////////////////////
