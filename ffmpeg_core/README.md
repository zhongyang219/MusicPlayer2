# FFMPEG 核心
## 编译需求
### 第三方库
* `FFMPEG` 库，包含
    * `libavutil`
    * `libavcodec`
    * `libavformat`
    * `libavdevice`
    * `libavfilter`
    * `libswresample`
* `SDL2` 库

FFMPEG库采用pkg-config来寻找，请确保正确的设置了环境变量`PKG_CONFIG_PATH`和CMAKE选项`CMAKE_PREFIX_PATH`
### FFMPEG 库要求
* 需要链接任意一TLS库以支持HTTPS（例如 `gnutls` / `openssl`）
* 需要链接 `libcdio` 以支持播放CD（ffmpeg官网的预编译版本可能无法正常工作即使其链接了 `libcdio`）
#### libavfilter
以下 `filters` 在核心中被使用到：
* `volume`：用于调节声音大小
* `atempo`：用于调节速度
* `equalizer`：用于均衡器
* `aresample`：用于格式自动转换

其他 `filters` 可以删除以减小体积
