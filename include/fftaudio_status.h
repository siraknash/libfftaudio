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

#ifndef FFTA__STATUS__H__
#define FFTA__STATUS__H__


typedef enum ffta_status_enum {
	// Unknown or non-existent api status
	//    The api status is the failure status code returned by the
	//    underlying api being used (i.e. cuda).  The api status is
	//    retrieved with the getApiStatus() call.
	FFTA_UNKNOWN_API_STATUS = -1,

	// Success
	FFTA_SUCCESS = 0,

	// Initialization function has already been called and failed. Object
	// becomes unuseable when this occurs and shouold be destroyed.
	FFTA_PREVIOUS_INITIALIZE_FAILED,

	// Initialization funtion has already been called and succeeded.  In
	// this case the initialize() call is a no-op and this is returned.
	FFTA_ALREADY_INITIALIZED,

	// An invalid configuration was specified in the arguments to the
	// constructor.
	FFTA_INVALID_ARGUMENT,

	// Memory allocation failed
	FFTA_ALLOC_FAILED,

	// Failed to create a thread
	FFTA_THREAD_CREATE_FAILED,

	// Failed to create a mutex
	FFTA_MUTEX_CREATE_FAILED,

	// Failed to create a transport needed by underlying api
	FFTA_TRANSPORT_CREATE_FAILED,

	// Failed to create a fft plan needed by underlying api
	FFTA_PLAN_CREATE_FAILED
} fftaStatusCode;


////////////////////////////////////////////////////////////

class fftaStatus
{
public:
	fftaStatus(fftaStatusCode status = FFTA_SUCCESS)
	{
		m_statusCode = status;
		m_apiStatus = FFTA_UNKNOWN_API_STATUS;
	}

	fftaStatus(fftaStatusCode status, int api_status)
	{
		m_statusCode = status;
		m_apiStatus = api_status;
	}

	
	virtual ~fftaStatus()
	{
	}

	bool operator!=(const fftaStatusCode &code)
	{
		return (m_statusCode != code);
	}

	bool operator==(const fftaStatusCode &code)
	{
		return (m_statusCode == code);
	}

	fftaStatusCode getStatusCode() const
	{
		return m_statusCode;
	}

	int getApiStatus() const
	{
		return m_apiStatus;
	}

private:
	fftaStatusCode		m_statusCode;
	int					m_apiStatus;
};


#endif // FFTA__STATUS__H__
