#pragma once
class CSpectralDataHelper
{
public:
    CSpectralDataHelper();

    //使用线性的方式根据fft数据计算频谱的高度
    // @para[in]:
    //  fft_data: fft数据
    //  scale: 柱形放大的比例，调整这个值可以调整频谱分析柱形图整体的高度
    // @para[out]:
    //  spectral_data: 频谱的高度
    static void SpectralDataMapOld(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL], int scale = 60);

    //使用线性+对数的方式根据fft数据计算频谱的高度（https://github.com/zhongyang219/MusicPlayer2/issues/169）
    // @para[in]:
    //  fft_data: fft数据
    //  scale: 柱形放大的比例，调整这个值可以调整频谱分析柱形图整体的高度
    // @para[out]:
    //  spectral_data: 频谱的高度
    void SpectralDataMap(float fft_data[FFT_SAMPLE], float spectral_data[SPECTRUM_COL], int scale = 60);

private:
    int spectrum_map[FFT_SAMPLE]{};
    int spectrum_map_count[SPECTRUM_COL]{};

};
