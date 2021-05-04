#pragma once
class CSpectralDataHelper
{
public:
    CSpectralDataHelper();

    //使用线性的方式根据fft数据计算频谱的高度
    // @para[in]:
    //  fft_data: fft数据
    // @para[out]:
    //  spectral_data: 频谱的高度
    static void SpectralDataMapOld(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL]);

    //使用线性+对数的方式根据fft数据计算频谱的高度（https://github.com/zhongyang219/MusicPlayer2/issues/169）
    // @para[in]:
    //  fft_data: fft数据
    // @para[out]:
    //  spectral_data: 频谱的高度
    void SpectralDataMap(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL]);

private:
    int spectrum_map[FFT_SAMPLE]{};
    int spectrum_map_count[SPECTRUM_COL]{};

};
