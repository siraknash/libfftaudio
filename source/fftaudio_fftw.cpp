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
#include	<vector>
#include	<pthread.h>
#include	<fftw3.h>

#include	<fftaudio_status.h>
#include	<fftaudio_fftw.h>


pthread_mutex_t		FFTAudio::sm_planMutex = PTHREAD_MUTEX_INITIALIZER;


/***************************************************************
 * FFTAudio Constructor (fftw3)
 ***************************************************************/

FFTAudio::FFTAudio(FuncInitWindowCB window_type, int sample_rate,
				   int frame_size, int padded_frame_size, int batch_count) :
	FFTAudioBase(window_type, sample_rate, frame_size, padded_frame_size, batch_count)
{
}


/***************************************************************
 * FFTAudio Destructor (fftw3)
 ***************************************************************/

FFTAudio::~FFTAudio()
{
	for(size_t i = 0; i < m_tids.size(); ++i) {
		::pthread_cancel(m_tids[i]);
	}

	::pthread_cond_destroy(&m_workCond);
	::pthread_cond_destroy(&m_ctrlCond);
	::pthread_mutex_destroy(&m_mutex);

	/*
	 * fftw create/destroy plan are not thread-safe, so plan destroys are wrapped with a static mutex
	 */
	::pthread_mutex_lock(&sm_planMutex);

	for(size_t i = 0; i < m_fftwPlans.size(); ++i) {
		/*
		 * Destroy each existing plan
		 */
		fftwf_destroy_plan(m_fftwPlans.back());
		m_fftwPlans.pop_back();
	}

	::pthread_mutex_unlock(&sm_planMutex);

	if(m_outputBuffer != nullptr) {
		::fftwf_free(m_outputBuffer);
	}

	if(m_inputBuffer != nullptr) {
		::fftwf_free(m_inputBuffer);
	}
}


/***************************************************************
 * FFTAudio::initialize() (fftw3)
 ***************************************************************/

fftaStatus
FFTAudio::initialize()
{
	fftaStatus		ret;
	size_t			alloc_sz;

	if((ret = FFTAudioBase::initialize()) != FFTA_SUCCESS) {
		return ret;
	}

	/*
	 * Allocate input buffer, one buffer shared by all threads
	 */
	alloc_sz = (size_t)(this->getBatchCount() * this->getPaddedFrameSize() * sizeof(float));

	m_inputBuffer = (float *)::fftwf_malloc(alloc_sz);
	if(m_inputBuffer == nullptr) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	/*
	 * Allocate output buffer, one buffer shared by all threads
	 */
	alloc_sz = (size_t)(this->getBatchCount() * (this->getBinCount() + 1) * sizeof(fftwf_complex));

	m_outputBuffer = (fftwf_complex *)fftwf_malloc(alloc_sz);
	if(m_outputBuffer == nullptr) {
		m_initializeFailed = true;
		return FFTA_ALLOC_FAILED;
	}

	/*
	 * fftw create/destroy plan are not thread-safe, so plan creates are wrapped with a static mutex
	 */
	::pthread_mutex_lock(&sm_planMutex);

	fftwf_plan	p;

	for(int i = 0; i < this->getBatchCount(); ++i) {
		/*
		 * Create and store an fftw plan for each batch/thread
		 */
		p = fftwf_plan_dft_r2c_1d(this->getPaddedFrameSize(),
								  &m_inputBuffer[i * this->getPaddedFrameSize()],
								  &m_outputBuffer[i * (this->getBinCount() + 1)], 0);
		if(p == NULL) {
			::pthread_mutex_unlock(&sm_planMutex);
			m_initializeFailed = true;
			return FFTA_PLAN_CREATE_FAILED;
		}

		m_fftwPlans.push_back(p);
	}

	::pthread_mutex_unlock(&sm_planMutex);

	/*
	 * Start all threads and do initial synchronization
	 */
	ret = this->_init_threads();

	if(ret != FFTA_SUCCESS) {
		m_initializeFailed = true;
		return ret;
	}

	m_initialized = true;
	return FFTA_SUCCESS;
}


/***************************************************************
 * FFTAudio::execute()
 ***************************************************************/

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

	m_done = 0;
	m_inputDataPointers = data_ptrs;

	/*
	 * Wake up all threads
	 */
	pthread_cond_broadcast(&m_workCond);

	/*
	 * Wait for all threads to finish (m_done == m_tids.size)
	 */
	do {
		// This condition is signaled when each work thread is done
		pthread_cond_wait(&m_ctrlCond, &m_mutex);
	} while(m_done < m_tids.size());

	m_inputDataPointers = nullptr;
	return true;
}


/***************************************************************
 ****************** Private Member Functions *******************
 ***************************************************************/

/***************************************************************
 * FFTAudio::run()
 ***************************************************************/

void
FFTAudio::_run(int thread_index)
{
	int		frame_start_idx;

	::pthread_mutex_lock(&m_mutex);
	++m_done;
	pthread_cond_signal(&m_ctrlCond);

	do {
		do {
			pthread_cond_wait(&m_workCond, &m_mutex);
		} while(m_inputDataPointers == nullptr);

		pthread_mutex_unlock(&m_mutex);

		frame_start_idx = this->getPaddedFrameSize() * thread_index;

		for(int i = 0; i < this->getFrameSize(); ++i) {			
			m_inputBuffer[frame_start_idx + i] = _prepare_input_value(i, m_inputDataPointers[thread_index][i]);
		}

		fftwf_execute(m_fftwPlans[thread_index]);

		pthread_mutex_lock(&m_mutex);
		++m_done;
		pthread_cond_signal(&m_ctrlCond);
	} while(true);
}


/*
 * Static work thread main() function
 */
void *
FFTAudio::_ffta_fftw_main(void *arg)
{	
	threadArgument	*thr_data = (threadArgument *)arg;
	FFTAudio		*ffta = thr_data->tdm_ffta;
	int				thr_idx = thr_data->tdm_threadIndex;

	delete thr_data;

	ffta->_run(thr_idx);
	return nullptr;
}


/***************************************************************
 * FFTAudio::_init_threads()
 ***************************************************************/

/*
 * Called by initialization function to perform thread creation
 */
fftaStatus
FFTAudio::_init_threads()
{
	pthread_t		tid;
	threadArgument	*thr_arg;

	::pthread_mutex_lock(&m_mutex);

	for(int i = 0; i < this->getBatchCount(); ++i) {
		thr_arg = new threadArgument(this, i);

		if(::pthread_create(&tid, nullptr, _ffta_fftw_main, thr_arg) != 0) {
			::pthread_mutex_unlock(&m_mutex);
			delete thr_arg;
			return FFTA_THREAD_CREATE_FAILED;
		}

		m_tids.push_back(tid);
	}

	m_done = 0;

	do {
		::pthread_cond_wait(&m_ctrlCond, &m_mutex);
	} while(m_done < m_tids.size());

	return FFTA_SUCCESS;
}

