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


#include	<math.h>
#include	<fftaudio_windows.h>


void
fftaWindow::Rectangle(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 1.0f;
		sum_output += values[i];
	}
}

void
fftaWindow::Triangluar(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 1.0f - ::fabsf(((float)i - N_MINUS_1_DIV_2(frame_size)) / ((float)frame_size / 2.0f));
		sum_output += values[i];
	}
}

void
fftaWindow::Bartlett(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 1.0f - ::fabsf(((float)i - N_MINUS_1_DIV_2(frame_size)) / N_MINUS_1_DIV_2(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::Sine(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = ::sinf(((float)i * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::Hann(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = ::sinf(((float)i * M_PI) / N_MINUS_1(frame_size));
		values[i] *= values[i];
		sum_output += values[i];
	}
}

void
fftaWindow::Hamming(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 0.53836f;
		values[i] -= 0.46164f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::Welch(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = N_MINUS_1_DIV_2(frame_size);
		values[i] = ((float)i - values[i]) / values[i];
		values[i] = 1.0f - (values[i] * values[i]);
		sum_output += values[i];
	}
}

void
fftaWindow::Blackman(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 0.42659f;
		values[i] -= 0.49656f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 0.076849f * ::cosf(((float)i * 4.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::Nuttall(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 0.355768f;
		values[i] -= 0.487396f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 0.144232f * ::cosf(((float)i * 4.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] -= 0.012604f * ::cosf(((float)i * 6.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::BlackmanNuttall(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 0.3635819f;
		values[i] -= 0.4891775f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 0.1365995f * ::cosf(((float)i * 4.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] -= 0.0106411f * ::cosf(((float)i * 6.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::BlackmanHarris(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 0.35875f;
		values[i] -= 0.48829f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 0.14128f * ::cosf(((float)i * 4.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] -= 0.01168f * ::cosf(((float)i * 6.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

void
fftaWindow::FlatTop(int frame_size, float &sum_output, float *values)
{
	sum_output = 0.0f;

	for(int i = 0; i < frame_size; ++i) {
		values[i] = 1.0000f;
		values[i] -= 1.930f * ::cosf(((float)i * 2.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 1.290f * ::cosf(((float)i * 4.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] -= 0.388f * ::cosf(((float)i * 6.0f * M_PI) / N_MINUS_1(frame_size));
		values[i] += 0.028f * ::cosf(((float)i * 8.0f * M_PI) / N_MINUS_1(frame_size));
		sum_output += values[i];
	}
}

