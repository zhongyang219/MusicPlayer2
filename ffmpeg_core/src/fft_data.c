#include "fft_data.h"

#include <math.h>
#include "libavcodec/avfft.h"

int ffmpeg_core_get_fft_data(MusicHandle* handle, float* fft_data, int len) {
    if (!handle || !fft_data) return FFMPEG_CORE_ERR_NULLPTR;
    if (len > FFT_SAMPLE / 2) return FFMPEG_CORE_ERR_TOO_BIG_FFT_DATA_LEN;
    int cal_samples = FFT_SAMPLE;
    AVFrame* f = av_frame_alloc(), * f2 = NULL;
    RDFTContext* context = NULL;
    SwrContext* swr = NULL;
    DWORD re = 0;
    int r = FFMPEG_CORE_ERR_OK;
    int nbits = log2(cal_samples);
    int total_samples = FFT_SAMPLE * 10;
    float* datas = NULL;
    int inv = cal_samples / len;
    if (!f) {
        return FFMPEG_CORE_ERR_OOM;
    }
    f->format = handle->target_format;
    f->nb_samples = total_samples;
    f->channel_layout = handle->output_channel_layout;
    f->channels = handle->sdl_spec.channels;
    if ((r = av_frame_get_buffer(f, 0)) < 0) {
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    if ((r = av_frame_make_writable(f)) < 0) {
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    re = WaitForSingleObject(handle->mutex, INFINITE);
    if (re != WAIT_OBJECT_0) {
        r = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    if (av_audio_fifo_size(handle->buffer) < total_samples) {
        ReleaseMutex(handle->mutex);
        r = FFMPEG_CORE_ERR_OK;
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    if ((r = av_audio_fifo_peek(handle->buffer, (void**)f->data, total_samples)) < 0) {
        ReleaseMutex(handle->mutex);
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    ReleaseMutex(handle->mutex);
    r = 0;
    if (!(context = av_rdft_init(nbits, DFT_R2C))) {
        r = FFMPEG_CORE_ERR_OOM;
        memset(fft_data, 0, sizeof(float) * len);
        goto end;
    }
    if (f->format == AV_SAMPLE_FMT_FLT && f->channels == 1) {
        for (int j = 0; j < 10; j++) {
            av_rdft_calc(context, (FFTSample*)f->data[0] + (size_t)FFT_SAMPLE * j);
        }
    } else {
        swr = swr_alloc_set_opts(NULL, av_get_default_channel_layout(1), AV_SAMPLE_FMT_FLT, handle->sdl_spec.freq, handle->output_channel_layout, handle->target_format, handle->sdl_spec.freq, 0, NULL);
        if (!swr) {
            r = FFMPEG_CORE_ERR_OOM;
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        if ((r = swr_init(swr)) < 0) {
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        f2 = av_frame_alloc();
        if (!f2) {
            r = FFMPEG_CORE_ERR_OOM;
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        f2->format = AV_SAMPLE_FMT_FLT;
        f2->channels = 1;
        f2->channel_layout = av_get_default_channel_layout(1);
        f2->nb_samples = total_samples;
        if ((r = av_frame_get_buffer(f2, 0)) < 0) {
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        if ((r = av_frame_make_writable(f2)) < 0) {
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        if ((r = swr_convert(swr, f2->data, f2->nb_samples, (const uint8_t**)f->data, f->nb_samples)) < 0) {
            memset(fft_data, 0, sizeof(float) * len);
            goto end;
        }
        r = 0;
        for (int j = 0; j < 10; j++)
            av_rdft_calc(context, (FFTSample*)f2->data[0] + (size_t)j * FFT_SAMPLE);
    }
    datas = f2 ? (float*)f2->data[0] : (float*)f->data[0];
    memset(fft_data, 0, sizeof(float) * len);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < len; i++) {
            if (i == 0)
                fft_data[i] += datas[i + FFT_SAMPLE * j] / FFT_SAMPLE / 10;
            else
                fft_data[i] += datas[i + FFT_SAMPLE * j] / FFT_SAMPLE / 5;
        }
    }
    r = 0;
end:
    if (f) av_frame_free(&f);
    if (context) av_rdft_end(context);
    if (swr) swr_free(&swr);
    if (f2) av_frame_free(&f2);
    return r;
}
