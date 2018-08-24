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

#ifndef FFTA__WINDOWS__H__
#define FFTA__WINDOWS__H__

//
// Window functions, for use in constructors
//		The Rectangle window is equivalent to using no window function
//
class fftaWindow
{
public:
	static void Rectangle(int frame_size, float &sum_output, float *values);
	static void Triangluar(int frame_size, float &sum_output, float *values);
	static void Bartlett(int frame_size, float &sum_output, float *values);
	static void Sine(int frame_size, float &sum_output, float *values);
	static void Hann(int frame_size, float &sum_output, float *values);
	static void Hamming(int frame_size, float &sum_output, float *values);
	static void Welch(int frame_size, float &sum_output, float *values);
	static void Blackman(int frame_size, float &sum_output, float *values);
	static void Nuttall(int frame_size, float &sum_output, float *values);
	static void BlackmanNuttall(int frame_size, float &sum_output, float *values);
	static void BlackmanHarris(int frame_size, float &sum_output, float *values);
	static void FlatTop(int frame_size, float &sum_output, float *values);

private:
	static inline float N_MINUS_1(int N)
	{
		return (float)(N - 1);
	}

	static inline float N_MINUS_1_DIV_2(int N)
	{
		return N_MINUS_1(N) / 2.0f;
	}

private:
	fftaWindow() = default;
	~fftaWindow() = default;
};


#endif // FFTA__WINDOWS__H__
