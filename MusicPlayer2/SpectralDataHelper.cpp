#include "stdafx.h"
#include "SpectralDataHelper.h"

const int LINEAR_SAMPLE_COUNT{ 107 };

CSpectralDataHelper::CSpectralDataHelper()
{
    for (int i{}; i < LINEAR_SAMPLE_COUNT; i++)
    {
        int m = i / 2;
        spectrum_map[i] = m;
        if (m >= 0 && m < SPECTRUM_COL)
            spectrum_map_count[m]++;
    }

    for (int i{ LINEAR_SAMPLE_COUNT }; i < FFT_SAMPLE; i++)
    {
        int m = std::log(i) / std::log(FFT_SAMPLE) * SPECTRUM_COL;
        if (m >= 0 && m < SPECTRUM_COL)
        {
            spectrum_map[i] = m;
            spectrum_map_count[m]++;
        }
    }
}

void CSpectralDataHelper::SpectralDataMapOld(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL])
{
    memset(spectral_data, 0, sizeof(float) * SPECTRUM_COL);
    for (int i{}; i < FFT_SAMPLE; i++)
    {
        spectral_data[i / (FFT_SAMPLE / SPECTRUM_COL)] += fft_data[i];
    }

    for (int i{}; i < SPECTRUM_COL; i++)
    {
        spectral_data[i] /= (FFT_SAMPLE / SPECTRUM_COL);
        spectral_data[i] = std::sqrtf(spectral_data[i]);		//对每个频谱柱形的值取平方根，以减少不同频率频谱值的差异
        spectral_data[i] *= 60;			//调整这里的乘数可以调整频谱分析柱形图整体的高度
    }
}

void CSpectralDataHelper::SpectralDataMap(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL])
{
    memset(spectral_data, 0, sizeof(float) * SPECTRUM_COL);
    for (int i{}; i < FFT_SAMPLE; i++)
    {
        spectral_data[spectrum_map[i]] += fft_data[i];
    }

    for (int i{}; i < SPECTRUM_COL; i++)
    {
        spectral_data[i] = spectral_data[i] / spectrum_map_count[i];
        spectral_data[i] = std::sqrtf(spectral_data[i]);		//对每个频谱柱形的值取平方根，以减少不同频率频谱值的差异
        spectral_data[i] *= 60;			//调整这里的乘数可以调整频谱分析柱形图整体的高度
    }
}
