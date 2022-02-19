#include "cda.h"

#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include "cfileop.h"
#include "cstr_util.h"
#include "err.h"

#ifndef _O_BINARY
#define _O_BINARY 0x8000
#endif

#ifndef _SH_DENYWR
#define _SH_DENYWR 0x20
#endif

#ifndef _S_IREAD
#define _S_IREAD 0x100
#endif

#define CDA_FILE_SIZE 44
#define u8_buf(buf, offset) ((const uint8_t*)buf + offset)

int is_cda_file(const char* url) {
    if (!url) return 0;
    char* dir = fileop_dirname(url);
    if (!dir) return 0;
    // 判断是否为根盘符
    if (!fileop_isdrive(dir)) {
        free(dir);
        return 0;
    }
    free(dir);
    char* l = strrchr(url, '.');
    if (!l || cstr_stricmp(l, ".cda")) {
        return 0;
    }
    size_t size = 0;
    if (!fileop_get_file_size(url, &size)) return 0;
    if (size != CDA_FILE_SIZE) return 0;
    return 1;
}

// CDA 文件格式见 https://en.wikipedia.org/wiki/.cda_file
int read_cda_file(MusicHandle* handle, const char* url) {
    if (!handle || !url) return FFMPEG_CORE_ERR_NULLPTR;
    int fd = 0;
    int re = 0;
    FILE* f = NULL;
    char buf[CDA_FILE_SIZE];
    int num_read = 0;
    uint32_t chunk_size = 0;
    if ((re = fileop_open(url, &fd, O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD))) {
        char* errmsg = err_get_errno_message(re);
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%d)\n", url, errmsg ? errmsg : "", re);
        if (errmsg) free(errmsg);
        return FFMPEG_CORE_ERR_FAILED_OPEN_FILE;
    }
    f = fileop_fdopen(fd, "r");
    if (!f) {
        fileop_close(fd);
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": Can not open file descriptor %d\n", url, fd);
        return FFMPEG_CORE_ERR_FAILED_OPEN_FILE;
    }
    handle->cda = malloc(sizeof(CDAData));
    if (!handle->cda) {
        re = FFMPEG_CORE_ERR_OOM;
        goto end;
    }
    memset(handle->cda, 0, sizeof(CDAData));
    if ((num_read = fread(buf, 1, CDA_FILE_SIZE, f)) < CDA_FILE_SIZE) {
        av_log(NULL, AV_LOG_FATAL, "Failed to read file \"%s\": %d bytes is needed, but only bytes was readed.\n", url, CDA_FILE_SIZE, num_read);
        re = FFMPEG_CORE_ERR_FAILED_READ_FILE;
        goto end;
    }
    if (strncmp(buf, "RIFF", 4)) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    chunk_size = cstr_read_uint32(u8_buf(buf, 4), 0);
    if (chunk_size != 36) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    if (strncmp(buf + 8, "CDDAfmt ", 8)) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    chunk_size = cstr_read_uint32(u8_buf(buf, 16), 0);
    if (chunk_size != 24) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    handle->cda->cd_format_version = cstr_read_uint16(u8_buf(buf, 20), 0);
    handle->cda->no = cstr_read_uint16(u8_buf(buf, 22), 0);
    handle->cda->range_offset = cstr_read_uint32(u8_buf(buf, 28), 0);
    handle->cda->duration = cstr_read_uint32(u8_buf(buf, 32), 0);
    re = FFMPEG_CORE_ERR_OK;
end:
    if (f) fileop_fclose(f);
    return re;
}

int read_cda_file2(MusicInfoHandle* handle, const char* url) {
    if (!handle || !url) return FFMPEG_CORE_ERR_NULLPTR;
    int fd = 0;
    int re = 0;
    FILE* f = NULL;
    char buf[CDA_FILE_SIZE];
    int num_read = 0;
    uint32_t chunk_size = 0;
    if ((re = fileop_open(url, &fd, O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD))) {
        char* errmsg = err_get_errno_message(re);
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%d)\n", url, errmsg ? errmsg : "", re);
        if (errmsg) free(errmsg);
        return FFMPEG_CORE_ERR_FAILED_OPEN_FILE;
    }
    f = fileop_fdopen(fd, "r");
    if (!f) {
        fileop_close(fd);
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": Can not open file descriptor %d\n", url, fd);
        return FFMPEG_CORE_ERR_FAILED_OPEN_FILE;
    }
    handle->cda = malloc(sizeof(CDAData));
    if (!handle->cda) {
        re = FFMPEG_CORE_ERR_OOM;
        goto end;
    }
    memset(handle->cda, 0, sizeof(CDAData));
    if ((num_read = fread(buf, 1, CDA_FILE_SIZE, f)) < CDA_FILE_SIZE) {
        av_log(NULL, AV_LOG_FATAL, "Failed to read file \"%s\": %d bytes is needed, but only bytes was readed.\n", url, CDA_FILE_SIZE, num_read);
        re = FFMPEG_CORE_ERR_FAILED_READ_FILE;
        goto end;
    }
    if (strncmp(buf, "RIFF", 4)) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    chunk_size = cstr_read_uint32(u8_buf(buf, 4), 0);
    if (chunk_size != 36) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    if (strncmp(buf + 8, "CDDAfmt ", 8)) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    chunk_size = cstr_read_uint32(u8_buf(buf, 16), 0);
    if (chunk_size != 24) {
        re = FFMPEG_CORE_ERR_INVALID_CDA_FILE;
        goto end;
    }
    handle->cda->cd_format_version = cstr_read_uint16(u8_buf(buf, 20), 0);
    handle->cda->no = cstr_read_uint16(u8_buf(buf, 22), 0);
    handle->cda->range_offset = cstr_read_uint32(u8_buf(buf, 28), 0);
    handle->cda->duration = cstr_read_uint32(u8_buf(buf, 32), 0);
    re = FFMPEG_CORE_ERR_OK;
end:
    if (f) fileop_fclose(f);
    return re;
}

const AVInputFormat* find_libcdio() {
    const AVInputFormat* f = NULL;
    f = av_input_audio_device_next(f);
    while (f) {
        if (f && !strcmp(f->name, "libcdio")) return f;
        f = av_input_audio_device_next(f);
    }
    return NULL;
}

int open_cd_device(MusicHandle* handle, const char* device) {
    if (!handle || !device) return FFMPEG_CORE_ERR_NULLPTR;
    avdevice_register_all();
    const AVInputFormat* f = find_libcdio();
    AVRational cda_time_base = { 1, 75 };
    if (!f) {
        return FFMPEG_CORE_ERR_NO_LIBCDIO;
    }
    int re = 0;
    AVDictionary* d = NULL;
    av_dict_set(&d, "paranoia_mode", "verify", 0);
    if ((re = avformat_open_input(&handle->fmt, device, f, &d)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%i)\n", device, av_err2str(re), re);
        av_dict_free(&d);
        return re;
    }
    if ((re = avformat_find_stream_info(handle->fmt, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to find streams in \"%s\": %s (%i)\n", device, av_err2str(re), re);
        av_dict_free(&d);
        return re;
    }
    handle->only_part = 1;
    handle->part_start_pts = av_rescale_q_rnd(handle->cda->range_offset, cda_time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    handle->part_end_pts = av_rescale_q_rnd((int64_t)handle->cda->duration + handle->cda->range_offset, cda_time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    // handle->fmt->flags |= AVFMT_FLAG_FAST_SEEK;  // 允许快速定位
    av_dict_free(&d);
    return FFMPEG_CORE_ERR_OK;
}

int open_cd_device2(MusicInfoHandle* handle, const char* device) {
    if (!handle || !device) return FFMPEG_CORE_ERR_NULLPTR;
    avdevice_register_all();
    const AVInputFormat* f = find_libcdio();
    AVRational cda_time_base = { 1, 75 };
    if (!f) {
        return FFMPEG_CORE_ERR_NO_LIBCDIO;
    }
    int re = 0;
    if ((re = avformat_open_input(&handle->fmt, device, f, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%i)\n", device, av_err2str(re), re);
        return re;
    }
    if ((re = avformat_find_stream_info(handle->fmt, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to find streams in \"%s\": %s (%i)\n", device, av_err2str(re), re);
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}

int64_t get_cda_duration(CDAData* d) {
    if (!d) return 0;
    AVRational t = { 1, 75 };
    return av_rescale_q_rnd(d->duration, t, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
}
