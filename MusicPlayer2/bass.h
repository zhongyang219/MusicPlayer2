/*
	BASS 2.4 C/C++ header file
	Copyright (c) 1999-2021 Un4seen Developments Ltd.

	See the BASS.CHM file for more detailed documentation
*/

#ifndef BASS_H
#define BASS_H

#ifdef _WIN32
#ifdef WINAPI_FAMILY
#include <winapifamily.h>
#endif
#include <wtypes.h>
typedef unsigned __int64 QWORD;
#else
#include <stdint.h>
#define WINAPI
#define CALLBACK
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
#ifdef __OBJC__
typedef int BOOL32;
#define BOOL BOOL32 // override objc's BOOL
#else
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define LOBYTE(a) (BYTE)(a)
#define HIBYTE(a) (BYTE)((a)>>8)
#define LOWORD(a) (WORD)(a)
#define HIWORD(a) (WORD)((a)>>16)
#define MAKEWORD(a,b) (WORD)(((a)&0xff)|((b)<<8))
#define MAKELONG(a,b) (DWORD)(((a)&0xffff)|((b)<<16))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BASSVERSION			0x204	// API version
#define BASSVERSIONTEXT		"2.4"

#ifndef BASSDEF
#define BASSDEF(f) WINAPI f
#else
#define NOBASSOVERLOADS
#endif

typedef DWORD HMUSIC;		// MOD music handle
typedef DWORD HSAMPLE;		// sample handle
typedef DWORD HCHANNEL;		// sample playback handle
typedef DWORD HSTREAM;		// sample stream handle
typedef DWORD HRECORD;		// recording handle
typedef DWORD HSYNC;		// synchronizer handle
typedef DWORD HDSP;			// DSP handle
typedef DWORD HFX;			// effect handle
typedef DWORD HPLUGIN;		// plugin handle

// Error codes returned by BASS_ErrorGetCode
#define BASS_OK				0	// all is OK
#define BASS_ERROR_MEM		1	// memory error
#define BASS_ERROR_FILEOPEN	2	// can't open the file
#define BASS_ERROR_DRIVER	3	// can't find a free/valid driver
#define BASS_ERROR_BUFLOST	4	// the sample buffer was lost
#define BASS_ERROR_HANDLE	5	// invalid handle
#define BASS_ERROR_FORMAT	6	// unsupported sample format
#define BASS_ERROR_POSITION	7	// invalid position
#define BASS_ERROR_INIT		8	// BASS_Init has not been successfully called
#define BASS_ERROR_START	9	// BASS_Start has not been successfully called
#define BASS_ERROR_SSL		10	// SSL/HTTPS support isn't available
#define BASS_ERROR_REINIT	11	// device needs to be reinitialized
#define BASS_ERROR_ALREADY	14	// already initialized/paused/whatever
#define BASS_ERROR_NOTAUDIO	17	// file does not contain audio
#define BASS_ERROR_NOCHAN	18	// can't get a free channel
#define BASS_ERROR_ILLTYPE	19	// an illegal type was specified
#define BASS_ERROR_ILLPARAM	20	// an illegal parameter was specified
#define BASS_ERROR_NO3D		21	// no 3D support
#define BASS_ERROR_NOEAX	22	// no EAX support
#define BASS_ERROR_DEVICE	23	// illegal device number
#define BASS_ERROR_NOPLAY	24	// not playing
#define BASS_ERROR_FREQ		25	// illegal sample rate
#define BASS_ERROR_NOTFILE	27	// the stream is not a file stream
#define BASS_ERROR_NOHW		29	// no hardware voices available
#define BASS_ERROR_EMPTY	31	// the MOD music has no sequence data
#define BASS_ERROR_NONET	32	// no internet connection could be opened
#define BASS_ERROR_CREATE	33	// couldn't create the file
#define BASS_ERROR_NOFX		34	// effects are not available
#define BASS_ERROR_NOTAVAIL	37	// requested data/action is not available
#define BASS_ERROR_DECODE	38	// the channel is/isn't a "decoding channel"
#define BASS_ERROR_DX		39	// a sufficient DirectX version is not installed
#define BASS_ERROR_TIMEOUT	40	// connection timedout
#define BASS_ERROR_FILEFORM	41	// unsupported file format
#define BASS_ERROR_SPEAKER	42	// unavailable speaker
#define BASS_ERROR_VERSION	43	// invalid BASS version (used by add-ons)
#define BASS_ERROR_CODEC	44	// codec is not available/supported
#define BASS_ERROR_ENDED	45	// the channel/file has ended
#define BASS_ERROR_BUSY		46	// the device is busy
#define BASS_ERROR_UNSTREAMABLE	47	// unstreamable file
#define BASS_ERROR_PROTOCOL	48	// unsupported protocol
#define BASS_ERROR_UNKNOWN	-1	// some other mystery problem

// BASS_SetConfig options
#define BASS_CONFIG_BUFFER			0
#define BASS_CONFIG_UPDATEPERIOD	1
#define BASS_CONFIG_GVOL_SAMPLE		4
#define BASS_CONFIG_GVOL_STREAM		5
#define BASS_CONFIG_GVOL_MUSIC		6
#define BASS_CONFIG_CURVE_VOL		7
#define BASS_CONFIG_CURVE_PAN		8
#define BASS_CONFIG_FLOATDSP		9
#define BASS_CONFIG_3DALGORITHM		10
#define BASS_CONFIG_NET_TIMEOUT		11
#define BASS_CONFIG_NET_BUFFER		12
#define BASS_CONFIG_PAUSE_NOPLAY	13
#define BASS_CONFIG_NET_PREBUF		15
#define BASS_CONFIG_NET_PASSIVE		18
#define BASS_CONFIG_REC_BUFFER		19
#define BASS_CONFIG_NET_PLAYLIST	21
#define BASS_CONFIG_MUSIC_VIRTUAL	22
#define BASS_CONFIG_VERIFY			23
#define BASS_CONFIG_UPDATETHREADS	24
#define BASS_CONFIG_DEV_BUFFER		27
#define BASS_CONFIG_REC_LOOPBACK	28
#define BASS_CONFIG_VISTA_TRUEPOS	30
#define BASS_CONFIG_IOS_SESSION		34
#define BASS_CONFIG_IOS_MIXAUDIO	34
#define BASS_CONFIG_DEV_DEFAULT		36
#define BASS_CONFIG_NET_READTIMEOUT	37
#define BASS_CONFIG_VISTA_SPEAKERS	38
#define BASS_CONFIG_IOS_SPEAKER		39
#define BASS_CONFIG_MF_DISABLE		40
#define BASS_CONFIG_HANDLES			41
#define BASS_CONFIG_UNICODE			42
#define BASS_CONFIG_SRC				43
#define BASS_CONFIG_SRC_SAMPLE		44
#define BASS_CONFIG_ASYNCFILE_BUFFER 45
#define BASS_CONFIG_OGG_PRESCAN		47
#define BASS_CONFIG_MF_VIDEO		48
#define BASS_CONFIG_AIRPLAY			49
#define BASS_CONFIG_DEV_NONSTOP		50
#define BASS_CONFIG_IOS_NOCATEGORY	51
#define BASS_CONFIG_VERIFY_NET		52
#define BASS_CONFIG_DEV_PERIOD		53
#define BASS_CONFIG_FLOAT			54
#define BASS_CONFIG_NET_SEEK		56
#define BASS_CONFIG_AM_DISABLE		58
#define BASS_CONFIG_NET_PLAYLIST_DEPTH	59
#define BASS_CONFIG_NET_PREBUF_WAIT	60
#define BASS_CONFIG_ANDROID_SESSIONID	62
#define BASS_CONFIG_WASAPI_PERSIST	65
#define BASS_CONFIG_REC_WASAPI		66
#define BASS_CONFIG_ANDROID_AAUDIO	67
#define BASS_CONFIG_SAMPLE_ONEHANDLE	69
#define BASS_CONFIG_DEV_TIMEOUT		70
#define BASS_CONFIG_NET_META		71
#define BASS_CONFIG_NET_RESTRATE	72

// BASS_SetConfigPtr options
#define BASS_CONFIG_NET_AGENT		16
#define BASS_CONFIG_NET_PROXY		17
#define BASS_CONFIG_IOS_NOTIFY		46
#define BASS_CONFIG_LIBSSL			64

#define BASS_CONFIG_THREAD			0x40000000 // flag: thread-specific setting

// BASS_CONFIG_IOS_SESSION flags
#define BASS_IOS_SESSION_MIX		1
#define BASS_IOS_SESSION_DUCK		2
#define BASS_IOS_SESSION_AMBIENT	4
#define BASS_IOS_SESSION_SPEAKER	8
#define BASS_IOS_SESSION_DISABLE	16

// BASS_Init flags
#define BASS_DEVICE_8BITS		1		// unused
#define BASS_DEVICE_MONO		2		// mono
#define BASS_DEVICE_3D			4		// unused
#define BASS_DEVICE_16BITS		8		// limit output to 16-bit
#define BASS_DEVICE_REINIT		128		// reinitialize
#define BASS_DEVICE_LATENCY		0x100	// unused
#define BASS_DEVICE_CPSPEAKERS	0x400	// unused
#define BASS_DEVICE_SPEAKERS	0x800	// force enabling of speaker assignment
#define BASS_DEVICE_NOSPEAKER	0x1000	// ignore speaker arrangement
#define BASS_DEVICE_DMIX		0x2000	// use ALSA "dmix" plugin
#define BASS_DEVICE_FREQ		0x4000	// set device sample rate
#define BASS_DEVICE_STEREO		0x8000	// limit output to stereo
#define BASS_DEVICE_HOG			0x10000	// hog/exclusive mode
#define BASS_DEVICE_AUDIOTRACK	0x20000	// use AudioTrack output
#define BASS_DEVICE_DSOUND		0x40000	// use DirectSound output
#define BASS_DEVICE_SOFTWARE	0x80000	// disable hardware/fastpath output

// DirectSound interfaces (for use with BASS_GetDSoundObject)
#define BASS_OBJECT_DS		1	// IDirectSound
#define BASS_OBJECT_DS3DL	2	// IDirectSound3DListener

// Device info structure
typedef struct {
#if defined(_WIN32_WCE) || (defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
	const wchar_t *name;	// description
	const wchar_t *driver;	// driver
#else
	const char *name;	// description
	const char *driver;	// driver
#endif
	DWORD flags;
} BASS_DEVICEINFO;

// BASS_DEVICEINFO flags
#define BASS_DEVICE_ENABLED		1
#define BASS_DEVICE_DEFAULT		2
#define BASS_DEVICE_INIT		4
#define BASS_DEVICE_LOOPBACK	8
#define BASS_DEVICE_DEFAULTCOM	128

#define BASS_DEVICE_TYPE_MASK			0xff000000
#define BASS_DEVICE_TYPE_NETWORK		0x01000000
#define BASS_DEVICE_TYPE_SPEAKERS		0x02000000
#define BASS_DEVICE_TYPE_LINE			0x03000000
#define BASS_DEVICE_TYPE_HEADPHONES		0x04000000
#define BASS_DEVICE_TYPE_MICROPHONE		0x05000000
#define BASS_DEVICE_TYPE_HEADSET		0x06000000
#define BASS_DEVICE_TYPE_HANDSET		0x07000000
#define BASS_DEVICE_TYPE_DIGITAL		0x08000000
#define BASS_DEVICE_TYPE_SPDIF			0x09000000
#define BASS_DEVICE_TYPE_HDMI			0x0a000000
#define BASS_DEVICE_TYPE_DISPLAYPORT	0x40000000

// BASS_GetDeviceInfo flags
#define BASS_DEVICES_AIRPLAY	0x1000000

typedef struct {
	DWORD flags;	// device capabilities (DSCAPS_xxx flags)
	DWORD hwsize;	// unused
	DWORD hwfree;	// unused
	DWORD freesam;	// unused
	DWORD free3d;	// unused
	DWORD minrate;	// unused
	DWORD maxrate;	// unused
	BOOL eax;		// unused
	DWORD minbuf;	// recommended minimum buffer length in ms
	DWORD dsver;	// DirectSound version
	DWORD latency;	// average delay (in ms) before start of playback
	DWORD initflags; // BASS_Init "flags" parameter
	DWORD speakers; // number of speakers available
	DWORD freq;		// current output rate
} BASS_INFO;

// BASS_INFO flags (from DSOUND.H)
#define DSCAPS_EMULDRIVER		0x00000020	// device does not have hardware DirectSound support
#define DSCAPS_CERTIFIED		0x00000040	// device driver has been certified by Microsoft

#define DSCAPS_HARDWARE			0x80000000	// hardware mixed

// Recording device info structure
typedef struct {
	DWORD flags;	// device capabilities (DSCCAPS_xxx flags)
	DWORD formats;	// supported standard formats (WAVE_FORMAT_xxx flags)
	DWORD inputs;	// number of inputs
	BOOL singlein;	// TRUE = only 1 input can be set at a time
	DWORD freq;		// current input rate
} BASS_RECORDINFO;

// BASS_RECORDINFO flags (from DSOUND.H)
#define DSCCAPS_EMULDRIVER		DSCAPS_EMULDRIVER	// device does not have hardware DirectSound recording support
#define DSCCAPS_CERTIFIED		DSCAPS_CERTIFIED	// device driver has been certified by Microsoft

// defines for formats field of BASS_RECORDINFO (from MMSYSTEM.H)
#ifndef WAVE_FORMAT_1M08
#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, Mono,   16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, Stereo, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, Mono,   8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, Mono,   16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, Stereo, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, Mono,   8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, Mono,   16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, Stereo, 16-bit */
#endif

// Sample info structure
typedef struct {
	DWORD freq;		// default playback rate
	float volume;	// default volume (0-1)
	float pan;		// default pan (-1=left, 0=middle, 1=right)
	DWORD flags;	// BASS_SAMPLE_xxx flags
	DWORD length;	// length (in bytes)
	DWORD max;		// maximum simultaneous playbacks
	DWORD origres;	// original resolution
	DWORD chans;	// number of channels
	DWORD mingap;	// minimum gap (ms) between creating channels
	DWORD mode3d;	// BASS_3DMODE_xxx mode
	float mindist;	// minimum distance
	float maxdist;	// maximum distance
	DWORD iangle;	// angle of inside projection cone
	DWORD oangle;	// angle of outside projection cone
	float outvol;	// delta-volume outside the projection cone
	DWORD vam;		// unused
	DWORD priority;	// unused
} BASS_SAMPLE;

#define BASS_SAMPLE_8BITS		1	// 8 bit
#define BASS_SAMPLE_FLOAT		256	// 32 bit floating-point
#define BASS_SAMPLE_MONO		2	// mono
#define BASS_SAMPLE_LOOP		4	// looped
#define BASS_SAMPLE_3D			8	// 3D functionality
#define BASS_SAMPLE_SOFTWARE	16	// unused
#define BASS_SAMPLE_MUTEMAX		32	// mute at max distance (3D only)
#define BASS_SAMPLE_VAM			64	// unused
#define BASS_SAMPLE_FX			128	// unused
#define BASS_SAMPLE_OVER_VOL	0x10000	// override lowest volume
#define BASS_SAMPLE_OVER_POS	0x20000	// override longest playing
#define BASS_SAMPLE_OVER_DIST	0x30000 // override furthest from listener (3D only)

#define BASS_STREAM_PRESCAN		0x20000 // scan file for accurate seeking and length
#define BASS_STREAM_AUTOFREE	0x40000	// automatically free the stream when it stops/ends
#define BASS_STREAM_RESTRATE	0x80000	// restrict the download rate of internet file stream
#define BASS_STREAM_BLOCK		0x100000 // download internet file stream in small blocks
#define BASS_STREAM_DECODE		0x200000 // don't play the stream, only decode
#define BASS_STREAM_STATUS		0x800000 // give server status info (HTTP/ICY tags) in DOWNLOADPROC

#define BASS_MP3_IGNOREDELAY	0x200 // ignore LAME/Xing/VBRI/iTunes delay & padding info
#define BASS_MP3_SETPOS			BASS_STREAM_PRESCAN

#define BASS_MUSIC_FLOAT		BASS_SAMPLE_FLOAT
#define BASS_MUSIC_MONO			BASS_SAMPLE_MONO
#define BASS_MUSIC_LOOP			BASS_SAMPLE_LOOP
#define BASS_MUSIC_3D			BASS_SAMPLE_3D
#define BASS_MUSIC_FX			BASS_SAMPLE_FX
#define BASS_MUSIC_AUTOFREE		BASS_STREAM_AUTOFREE
#define BASS_MUSIC_DECODE		BASS_STREAM_DECODE
#define BASS_MUSIC_PRESCAN		BASS_STREAM_PRESCAN	// calculate playback length
#define BASS_MUSIC_CALCLEN		BASS_MUSIC_PRESCAN
#define BASS_MUSIC_RAMP			0x200	// normal ramping
#define BASS_MUSIC_RAMPS		0x400	// sensitive ramping
#define BASS_MUSIC_SURROUND		0x800	// surround sound
#define BASS_MUSIC_SURROUND2	0x1000	// surround sound (mode 2)
#define BASS_MUSIC_FT2PAN		0x2000	// apply FastTracker 2 panning to XM files
#define BASS_MUSIC_FT2MOD		0x2000	// play .MOD as FastTracker 2 does
#define BASS_MUSIC_PT1MOD		0x4000	// play .MOD as ProTracker 1 does
#define BASS_MUSIC_NONINTER		0x10000	// non-interpolated sample mixing
#define BASS_MUSIC_SINCINTER	0x800000 // sinc interpolated sample mixing
#define BASS_MUSIC_POSRESET		0x8000	// stop all notes when moving position
#define BASS_MUSIC_POSRESETEX	0x400000 // stop all notes and reset bmp/etc when moving position
#define BASS_MUSIC_STOPBACK		0x80000	// stop the music on a backwards jump effect
#define BASS_MUSIC_NOSAMPLE		0x100000 // don't load the samples

// Speaker assignment flags
#define BASS_SPEAKER_FRONT	0x1000000	// front speakers
#define BASS_SPEAKER_REAR	0x2000000	// rear/side speakers
#define BASS_SPEAKER_CENLFE	0x3000000	// center & LFE speakers (5.1)
#define BASS_SPEAKER_REAR2	0x4000000	// rear center speakers (7.1)
#define BASS_SPEAKER_N(n)	((n)<<24)	// n'th pair of speakers (max 15)
#define BASS_SPEAKER_LEFT	0x10000000	// modifier: left
#define BASS_SPEAKER_RIGHT	0x20000000	// modifier: right
#define BASS_SPEAKER_FRONTLEFT	BASS_SPEAKER_FRONT|BASS_SPEAKER_LEFT
#define BASS_SPEAKER_FRONTRIGHT	BASS_SPEAKER_FRONT|BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_REARLEFT	BASS_SPEAKER_REAR|BASS_SPEAKER_LEFT
#define BASS_SPEAKER_REARRIGHT	BASS_SPEAKER_REAR|BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_CENTER		BASS_SPEAKER_CENLFE|BASS_SPEAKER_LEFT
#define BASS_SPEAKER_LFE		BASS_SPEAKER_CENLFE|BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_REAR2LEFT	BASS_SPEAKER_REAR2|BASS_SPEAKER_LEFT
#define BASS_SPEAKER_REAR2RIGHT	BASS_SPEAKER_REAR2|BASS_SPEAKER_RIGHT

#define BASS_ASYNCFILE			0x40000000	// read file asynchronously
#define BASS_UNICODE			0x80000000	// UTF-16

#define BASS_RECORD_PAUSE		0x8000	// start recording paused
#define BASS_RECORD_ECHOCANCEL	0x2000
#define BASS_RECORD_AGC			0x4000

// DX7 voice allocation & management flags
#define BASS_VAM_HARDWARE		1
#define BASS_VAM_SOFTWARE		2
#define BASS_VAM_TERM_TIME		4
#define BASS_VAM_TERM_DIST		8
#define BASS_VAM_TERM_PRIO		16

// Channel info structure
typedef struct {
	DWORD freq;		// default playback rate
	DWORD chans;	// channels
	DWORD flags;
	DWORD ctype;	// type of channel
	DWORD origres;	// original resolution
	HPLUGIN plugin;
	HSAMPLE sample;
	const char *filename;
} BASS_CHANNELINFO;

#define BASS_ORIGRES_FLOAT		0x10000

// BASS_CHANNELINFO types
#define BASS_CTYPE_SAMPLE		1
#define BASS_CTYPE_RECORD		2
#define BASS_CTYPE_STREAM		0x10000
#define BASS_CTYPE_STREAM_VORBIS	0x10002
#define BASS_CTYPE_STREAM_OGG	0x10002
#define BASS_CTYPE_STREAM_MP1	0x10003
#define BASS_CTYPE_STREAM_MP2	0x10004
#define BASS_CTYPE_STREAM_MP3	0x10005
#define BASS_CTYPE_STREAM_AIFF	0x10006
#define BASS_CTYPE_STREAM_CA	0x10007
#define BASS_CTYPE_STREAM_MF	0x10008
#define BASS_CTYPE_STREAM_AM	0x10009
#define BASS_CTYPE_STREAM_SAMPLE	0x1000a
#define BASS_CTYPE_STREAM_DUMMY		0x18000
#define BASS_CTYPE_STREAM_DEVICE	0x18001
#define BASS_CTYPE_STREAM_WAV	0x40000 // WAVE flag (LOWORD=codec)
#define BASS_CTYPE_STREAM_WAV_PCM	0x50001
#define BASS_CTYPE_STREAM_WAV_FLOAT	0x50003
#define BASS_CTYPE_MUSIC_MOD	0x20000
#define BASS_CTYPE_MUSIC_MTM	0x20001
#define BASS_CTYPE_MUSIC_S3M	0x20002
#define BASS_CTYPE_MUSIC_XM		0x20003
#define BASS_CTYPE_MUSIC_IT		0x20004
#define BASS_CTYPE_MUSIC_MO3	0x00100 // MO3 flag

typedef struct {
	DWORD ctype;		// channel type
#if defined(_WIN32_WCE) || (defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
	const wchar_t *name;	// format description
	const wchar_t *exts;	// file extension filter (*.ext1;*.ext2;etc...)
#else
	const char *name;	// format description
	const char *exts;	// file extension filter (*.ext1;*.ext2;etc...)
#endif
} BASS_PLUGINFORM;

typedef struct {
	DWORD version;					// version (same form as BASS_GetVersion)
	DWORD formatc;					// number of formats
	const BASS_PLUGINFORM *formats;	// the array of formats
} BASS_PLUGININFO;

// 3D vector (for 3D positions/velocities/orientations)
typedef struct BASS_3DVECTOR {
#ifdef __cplusplus
	BASS_3DVECTOR() {};
	BASS_3DVECTOR(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
#endif
	float x;	// +=right, -=left
	float y;	// +=up, -=down
	float z;	// +=front, -=behind
} BASS_3DVECTOR;

// 3D channel modes
#define BASS_3DMODE_NORMAL		0	// normal 3D processing
#define BASS_3DMODE_RELATIVE	1	// position is relative to the listener
#define BASS_3DMODE_OFF			2	// no 3D processing

// software 3D mixing algorithms (used with BASS_CONFIG_3DALGORITHM)
#define BASS_3DALG_DEFAULT	0
#define BASS_3DALG_OFF		1
#define BASS_3DALG_FULL		2
#define BASS_3DALG_LIGHT	3

// BASS_SampleGetChannel flags
#define BASS_SAMCHAN_NEW		1	// get a new playback channel
#define BASS_SAMCHAN_STREAM		2	// create a stream

typedef DWORD (CALLBACK STREAMPROC)(HSTREAM handle, void *buffer, DWORD length, void *user);
/* User stream callback function.
handle : The stream that needs writing
buffer : Buffer to write the samples in
length : Number of bytes to write
user   : The 'user' parameter value given when calling BASS_StreamCreate
RETURN : Number of bytes written. Set the BASS_STREAMPROC_END flag to end the stream. */

#define BASS_STREAMPROC_END		0x80000000	// end of user stream flag

// Special STREAMPROCs
#define STREAMPROC_DUMMY		(STREAMPROC*)0		// "dummy" stream
#define STREAMPROC_PUSH			(STREAMPROC*)-1		// push stream
#define STREAMPROC_DEVICE		(STREAMPROC*)-2		// device mix stream
#define STREAMPROC_DEVICE_3D	(STREAMPROC*)-3		// device 3D mix stream

// BASS_StreamCreateFileUser file systems
#define STREAMFILE_NOBUFFER		0
#define STREAMFILE_BUFFER		1
#define STREAMFILE_BUFFERPUSH	2

// User file stream callback functions
typedef void (CALLBACK FILECLOSEPROC)(void *user);
typedef QWORD (CALLBACK FILELENPROC)(void *user);
typedef DWORD (CALLBACK FILEREADPROC)(void *buffer, DWORD length, void *user);
typedef BOOL (CALLBACK FILESEEKPROC)(QWORD offset, void *user);

typedef struct {
	FILECLOSEPROC *close;
	FILELENPROC *length;
	FILEREADPROC *read;
	FILESEEKPROC *seek;
} BASS_FILEPROCS;

// BASS_StreamPutFileData options
#define BASS_FILEDATA_END		0	// end & close the file

// BASS_StreamGetFilePosition modes
#define BASS_FILEPOS_CURRENT	0
#define BASS_FILEPOS_DECODE		BASS_FILEPOS_CURRENT
#define BASS_FILEPOS_DOWNLOAD	1
#define BASS_FILEPOS_END		2
#define BASS_FILEPOS_START		3
#define BASS_FILEPOS_CONNECTED	4
#define BASS_FILEPOS_BUFFER		5
#define BASS_FILEPOS_SOCKET		6
#define BASS_FILEPOS_ASYNCBUF	7
#define BASS_FILEPOS_SIZE		8
#define BASS_FILEPOS_BUFFERING	9
#define BASS_FILEPOS_AVAILABLE	10

typedef void (CALLBACK DOWNLOADPROC)(const void *buffer, DWORD length, void *user);
/* Internet stream download callback function.
buffer : Buffer containing the downloaded data... NULL=end of download
length : Number of bytes in the buffer
user   : The 'user' parameter value given when calling BASS_StreamCreateURL */

// BASS_ChannelSetSync types
#define BASS_SYNC_POS			0
#define BASS_SYNC_END			2
#define BASS_SYNC_META			4
#define BASS_SYNC_SLIDE			5
#define BASS_SYNC_STALL			6
#define BASS_SYNC_DOWNLOAD		7
#define BASS_SYNC_FREE			8
#define BASS_SYNC_SETPOS		11
#define BASS_SYNC_MUSICPOS		10
#define BASS_SYNC_MUSICINST		1
#define BASS_SYNC_MUSICFX		3
#define BASS_SYNC_OGG_CHANGE	12
#define BASS_SYNC_DEV_FAIL		14
#define BASS_SYNC_DEV_FORMAT	15
#define BASS_SYNC_THREAD		0x20000000	// flag: call sync in other thread
#define BASS_SYNC_MIXTIME		0x40000000	// flag: sync at mixtime, else at playtime
#define BASS_SYNC_ONETIME		0x80000000	// flag: sync only once, else continuously

typedef void (CALLBACK SYNCPROC)(HSYNC handle, DWORD channel, DWORD data, void *user);
/* Sync callback function.
handle : The sync that has occured
channel: Channel that the sync occured in
data   : Additional data associated with the sync's occurance
user   : The 'user' parameter given when calling BASS_ChannelSetSync */

typedef void (CALLBACK DSPPROC)(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user);
/* DSP callback function.
handle : The DSP handle
channel: Channel that the DSP is being applied to
buffer : Buffer to apply the DSP to
length : Number of bytes in the buffer
user   : The 'user' parameter given when calling BASS_ChannelSetDSP */

typedef BOOL (CALLBACK RECORDPROC)(HRECORD handle, const void *buffer, DWORD length, void *user);
/* Recording callback function.
handle : The recording handle
buffer : Buffer containing the recorded sample data
length : Number of bytes
user   : The 'user' parameter value given when calling BASS_RecordStart
RETURN : TRUE = continue recording, FALSE = stop */

// BASS_ChannelIsActive return values
#define BASS_ACTIVE_STOPPED			0
#define BASS_ACTIVE_PLAYING			1
#define BASS_ACTIVE_STALLED			2
#define BASS_ACTIVE_PAUSED			3
#define BASS_ACTIVE_PAUSED_DEVICE	4

// Channel attributes
#define BASS_ATTRIB_FREQ			1
#define BASS_ATTRIB_VOL				2
#define BASS_ATTRIB_PAN				3
#define BASS_ATTRIB_EAXMIX			4
#define BASS_ATTRIB_NOBUFFER		5
#define BASS_ATTRIB_VBR				6
#define BASS_ATTRIB_CPU				7
#define BASS_ATTRIB_SRC				8
#define BASS_ATTRIB_NET_RESUME		9
#define BASS_ATTRIB_SCANINFO		10
#define BASS_ATTRIB_NORAMP			11
#define BASS_ATTRIB_BITRATE			12
#define BASS_ATTRIB_BUFFER			13
#define BASS_ATTRIB_GRANULE			14
#define BASS_ATTRIB_USER			15
#define BASS_ATTRIB_TAIL			16
#define BASS_ATTRIB_PUSH_LIMIT		17
#define BASS_ATTRIB_MUSIC_AMPLIFY	0x100
#define BASS_ATTRIB_MUSIC_PANSEP	0x101
#define BASS_ATTRIB_MUSIC_PSCALER	0x102
#define BASS_ATTRIB_MUSIC_BPM		0x103
#define BASS_ATTRIB_MUSIC_SPEED		0x104
#define BASS_ATTRIB_MUSIC_VOL_GLOBAL 0x105
#define BASS_ATTRIB_MUSIC_ACTIVE	0x106
#define BASS_ATTRIB_MUSIC_VOL_CHAN	0x200 // + channel #
#define BASS_ATTRIB_MUSIC_VOL_INST	0x300 // + instrument #

// BASS_ChannelSlideAttribute flags
#define BASS_SLIDE_LOG				0x1000000

// BASS_ChannelGetData flags
#define BASS_DATA_AVAILABLE	0			// query how much data is buffered
#define BASS_DATA_NOREMOVE	0x10000000	// flag: don't remove data from recording buffer
#define BASS_DATA_FIXED		0x20000000	// flag: return 8.24 fixed-point data
#define BASS_DATA_FLOAT		0x40000000	// flag: return floating-point sample data
#define BASS_DATA_FFT256	0x80000000	// 256 sample FFT
#define BASS_DATA_FFT512	0x80000001	// 512 FFT
#define BASS_DATA_FFT1024	0x80000002	// 1024 FFT
#define BASS_DATA_FFT2048	0x80000003	// 2048 FFT
#define BASS_DATA_FFT4096	0x80000004	// 4096 FFT
#define BASS_DATA_FFT8192	0x80000005	// 8192 FFT
#define BASS_DATA_FFT16384	0x80000006	// 16384 FFT
#define BASS_DATA_FFT32768	0x80000007	// 32768 FFT
#define BASS_DATA_FFT_INDIVIDUAL 0x10	// FFT flag: FFT for each channel, else all combined
#define BASS_DATA_FFT_NOWINDOW	0x20	// FFT flag: no Hanning window
#define BASS_DATA_FFT_REMOVEDC	0x40	// FFT flag: pre-remove DC bias
#define BASS_DATA_FFT_COMPLEX	0x80	// FFT flag: return complex data
#define BASS_DATA_FFT_NYQUIST	0x100	// FFT flag: return extra Nyquist value

// BASS_ChannelGetLevelEx flags
#define BASS_LEVEL_MONO		1	// get mono level
#define BASS_LEVEL_STEREO	2	// get stereo level
#define BASS_LEVEL_RMS		4	// get RMS levels
#define BASS_LEVEL_VOLPAN	8	// apply VOL/PAN attributes to the levels
#define BASS_LEVEL_NOREMOVE	16	// don't remove data from recording buffer

// BASS_ChannelGetTags types : what's returned
#define BASS_TAG_ID3		0	// ID3v1 tags : TAG_ID3 structure
#define BASS_TAG_ID3V2		1	// ID3v2 tags : variable length block
#define BASS_TAG_OGG		2	// OGG comments : series of null-terminated UTF-8 strings
#define BASS_TAG_HTTP		3	// HTTP headers : series of null-terminated ASCII strings
#define BASS_TAG_ICY		4	// ICY headers : series of null-terminated ANSI strings
#define BASS_TAG_META		5	// ICY metadata : ANSI string
#define BASS_TAG_APE		6	// APE tags : series of null-terminated UTF-8 strings
#define BASS_TAG_MP4 		7	// MP4/iTunes metadata : series of null-terminated UTF-8 strings
#define BASS_TAG_WMA		8	// WMA tags : series of null-terminated UTF-8 strings
#define BASS_TAG_VENDOR		9	// OGG encoder : UTF-8 string
#define BASS_TAG_LYRICS3	10	// Lyric3v2 tag : ASCII string
#define BASS_TAG_CA_CODEC	11	// CoreAudio codec info : TAG_CA_CODEC structure
#define BASS_TAG_MF			13	// Media Foundation tags : series of null-terminated UTF-8 strings
#define BASS_TAG_WAVEFORMAT	14	// WAVE format : WAVEFORMATEEX structure
#define BASS_TAG_AM_NAME	16	// Android Media codec name : ASCII string
#define BASS_TAG_ID3V2_2	17	// ID3v2 tags (2nd block) : variable length block
#define BASS_TAG_AM_MIME	18	// Android Media MIME type : ASCII string
#define BASS_TAG_LOCATION	19	// redirected URL : ASCII string
#define BASS_TAG_RIFF_INFO	0x100 // RIFF "INFO" tags : series of null-terminated ANSI strings
#define BASS_TAG_RIFF_BEXT	0x101 // RIFF/BWF "bext" tags : TAG_BEXT structure
#define BASS_TAG_RIFF_CART	0x102 // RIFF/BWF "cart" tags : TAG_CART structure
#define BASS_TAG_RIFF_DISP	0x103 // RIFF "DISP" text tag : ANSI string
#define BASS_TAG_RIFF_CUE	0x104 // RIFF "cue " chunk : TAG_CUE structure
#define BASS_TAG_RIFF_SMPL	0x105 // RIFF "smpl" chunk : TAG_SMPL structure
#define BASS_TAG_APE_BINARY	0x1000	// + index #, binary APE tag : TAG_APE_BINARY structure
#define BASS_TAG_MUSIC_NAME		0x10000	// MOD music name : ANSI string
#define BASS_TAG_MUSIC_MESSAGE	0x10001	// MOD message : ANSI string
#define BASS_TAG_MUSIC_ORDERS	0x10002	// MOD order list : BYTE array of pattern numbers
#define BASS_TAG_MUSIC_AUTH		0x10003	// MOD author : UTF-8 string
#define BASS_TAG_MUSIC_INST		0x10100	// + instrument #, MOD instrument name : ANSI string
#define BASS_TAG_MUSIC_CHAN		0x10200	// + channel #, MOD channel name : ANSI string
#define BASS_TAG_MUSIC_SAMPLE	0x10300	// + sample #, MOD sample name : ANSI string

// ID3v1 tag structure
typedef struct {
	char id[3];
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[30];
	BYTE genre;
} TAG_ID3;

// Binary APE tag structure
typedef struct {
	const char *key;
	const void *data;
	DWORD length;
} TAG_APE_BINARY;

// BWF "bext" tag structure
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
#endif
#pragma pack(push,1)
typedef struct {
	char Description[256];			// description
	char Originator[32];			// name of the originator
	char OriginatorReference[32];	// reference of the originator
	char OriginationDate[10];		// date of creation (yyyy-mm-dd)
	char OriginationTime[8];		// time of creation (hh-mm-ss)
	QWORD TimeReference;			// first sample count since midnight (little-endian)
	WORD Version;					// BWF version (little-endian)
	BYTE UMID[64];					// SMPTE UMID
	BYTE Reserved[190];
#if defined(__GNUC__) && __GNUC__<3
	char CodingHistory[0];			// history
#elif 1 // change to 0 if compiler fails the following line
	char CodingHistory[];			// history
#else
	char CodingHistory[1];			// history
#endif
} TAG_BEXT;
#pragma pack(pop)

// BWF "cart" tag structures
typedef struct
{
	DWORD dwUsage;					// FOURCC timer usage ID
	DWORD dwValue;					// timer value in samples from head
} TAG_CART_TIMER;

typedef struct
{
	char Version[4];				// version of the data structure
	char Title[64];					// title of cart audio sequence
	char Artist[64];				// artist or creator name
	char CutID[64];					// cut number identification
	char ClientID[64];				// client identification
	char Category[64];				// category ID, PSA, NEWS, etc
	char Classification[64];		// classification or auxiliary key
	char OutCue[64];				// out cue text
	char StartDate[10];				// yyyy-mm-dd
	char StartTime[8];				// hh:mm:ss
	char EndDate[10];				// yyyy-mm-dd
	char EndTime[8];				// hh:mm:ss
	char ProducerAppID[64];			// name of vendor or application
	char ProducerAppVersion[64];	// version of producer application
	char UserDef[64];				// user defined text
	DWORD dwLevelReference;			// sample value for 0 dB reference
	TAG_CART_TIMER PostTimer[8];	// 8 time markers after head
	char Reserved[276];
	char URL[1024];					// uniform resource locator
#if defined(__GNUC__) && __GNUC__<3
	char TagText[0];				// free form text for scripts or tags
#elif 1 // change to 0 if compiler fails the following line
	char TagText[];					// free form text for scripts or tags
#else
	char TagText[1];				// free form text for scripts or tags
#endif
} TAG_CART;

// RIFF "cue " tag structures
typedef struct
{
	DWORD dwName;
	DWORD dwPosition;
	DWORD fccChunk;
	DWORD dwChunkStart;
	DWORD dwBlockStart;
	DWORD dwSampleOffset;
} TAG_CUE_POINT;

typedef struct
{
	DWORD dwCuePoints;
#if defined(__GNUC__) && __GNUC__<3
	TAG_CUE_POINT CuePoints[0];
#elif 1 // change to 0 if compiler fails the following line
	TAG_CUE_POINT CuePoints[];
#else
	TAG_CUE_POINT CuePoints[1];
#endif
} TAG_CUE;

// RIFF "smpl" tag structures
typedef struct
{
	DWORD dwIdentifier;
	DWORD dwType;
	DWORD dwStart;
	DWORD dwEnd;
	DWORD dwFraction;
	DWORD dwPlayCount;
} TAG_SMPL_LOOP;

typedef struct
{
	DWORD dwManufacturer;
	DWORD dwProduct;
	DWORD dwSamplePeriod;
	DWORD dwMIDIUnityNote;
	DWORD dwMIDIPitchFraction;
	DWORD dwSMPTEFormat;
	DWORD dwSMPTEOffset;
	DWORD cSampleLoops;
	DWORD cbSamplerData;
#if defined(__GNUC__) && __GNUC__<3
	TAG_SMPL_LOOP SampleLoops[0];
#elif 1 // change to 0 if compiler fails the following line
	TAG_SMPL_LOOP SampleLoops[];
#else
	TAG_SMPL_LOOP SampleLoops[1];
#endif
} TAG_SMPL;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// CoreAudio codec info structure
typedef struct {
	DWORD ftype;					// file format
	DWORD atype;					// audio format
	const char *name;				// description
} TAG_CA_CODEC;

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
#pragma pack(push,1)
typedef struct tWAVEFORMATEX
{
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#pragma pack(pop)
#endif

// BASS_ChannelGetLength/GetPosition/SetPosition modes
#define BASS_POS_BYTE			0		// byte position
#define BASS_POS_MUSIC_ORDER	1		// order.row position, MAKELONG(order,row)
#define BASS_POS_OGG			3		// OGG bitstream number
#define BASS_POS_END			0x10	// trimmed end position
#define BASS_POS_LOOP			0x11	// loop start positiom
#define BASS_POS_FLUSH			0x1000000 // flag: flush decoder/FX buffers
#define BASS_POS_RESET			0x2000000 // flag: reset user file buffers
#define BASS_POS_RELATIVE		0x4000000 // flag: seek relative to the current position
#define BASS_POS_INEXACT		0x8000000 // flag: allow seeking to inexact position
#define BASS_POS_DECODE			0x10000000 // flag: get the decoding (not playing) position
#define BASS_POS_DECODETO		0x20000000 // flag: decode to the position instead of seeking
#define BASS_POS_SCAN			0x40000000 // flag: scan to the position

// BASS_ChannelSetDevice/GetDevice option
#define BASS_NODEVICE		0x20000

// BASS_RecordSetInput flags
#define BASS_INPUT_OFF		0x10000
#define BASS_INPUT_ON		0x20000

#define BASS_INPUT_TYPE_MASK		0xff000000
#define BASS_INPUT_TYPE_UNDEF		0x00000000
#define BASS_INPUT_TYPE_DIGITAL		0x01000000
#define BASS_INPUT_TYPE_LINE		0x02000000
#define BASS_INPUT_TYPE_MIC			0x03000000
#define BASS_INPUT_TYPE_SYNTH		0x04000000
#define BASS_INPUT_TYPE_CD			0x05000000
#define BASS_INPUT_TYPE_PHONE		0x06000000
#define BASS_INPUT_TYPE_SPEAKER		0x07000000
#define BASS_INPUT_TYPE_WAVE		0x08000000
#define BASS_INPUT_TYPE_AUX			0x09000000
#define BASS_INPUT_TYPE_ANALOG		0x0a000000

// BASS_ChannelSetFX effect types
#define BASS_FX_DX8_CHORUS			0
#define BASS_FX_DX8_COMPRESSOR		1
#define BASS_FX_DX8_DISTORTION		2
#define BASS_FX_DX8_ECHO			3
#define BASS_FX_DX8_FLANGER			4
#define BASS_FX_DX8_GARGLE			5
#define BASS_FX_DX8_I3DL2REVERB		6
#define BASS_FX_DX8_PARAMEQ			7
#define BASS_FX_DX8_REVERB			8
#define BASS_FX_VOLUME				9

typedef struct {
	float       fWetDryMix;
	float       fDepth;
	float       fFeedback;
	float       fFrequency;
	DWORD       lWaveform;	// 0=triangle, 1=sine
	float       fDelay;
	DWORD       lPhase;		// BASS_DX8_PHASE_xxx
} BASS_DX8_CHORUS;

typedef struct {
	float   fGain;
	float   fAttack;
	float   fRelease;
	float   fThreshold;
	float   fRatio;
	float   fPredelay;
} BASS_DX8_COMPRESSOR;

typedef struct {
	float   fGain;
	float   fEdge;
	float   fPostEQCenterFrequency;
	float   fPostEQBandwidth;
	float   fPreLowpassCutoff;
} BASS_DX8_DISTORTION;

typedef struct {
	float   fWetDryMix;
	float   fFeedback;
	float   fLeftDelay;
	float   fRightDelay;
	BOOL    lPanDelay;
} BASS_DX8_ECHO;

typedef struct {
	float       fWetDryMix;
	float       fDepth;
	float       fFeedback;
	float       fFrequency;
	DWORD       lWaveform;	// 0=triangle, 1=sine
	float       fDelay;
	DWORD       lPhase;		// BASS_DX8_PHASE_xxx
} BASS_DX8_FLANGER;

typedef struct {
	DWORD       dwRateHz;               // Rate of modulation in hz
	DWORD       dwWaveShape;            // 0=triangle, 1=square
} BASS_DX8_GARGLE;

typedef struct {
	int     lRoom;                  // [-10000, 0]      default: -1000 mB
	int     lRoomHF;                // [-10000, 0]      default: 0 mB
	float   flRoomRolloffFactor;    // [0.0, 10.0]      default: 0.0
	float   flDecayTime;            // [0.1, 20.0]      default: 1.49s
	float   flDecayHFRatio;         // [0.1, 2.0]       default: 0.83
	int     lReflections;           // [-10000, 1000]   default: -2602 mB
	float   flReflectionsDelay;     // [0.0, 0.3]       default: 0.007 s
	int     lReverb;                // [-10000, 2000]   default: 200 mB
	float   flReverbDelay;          // [0.0, 0.1]       default: 0.011 s
	float   flDiffusion;            // [0.0, 100.0]     default: 100.0 %
	float   flDensity;              // [0.0, 100.0]     default: 100.0 %
	float   flHFReference;          // [20.0, 20000.0]  default: 5000.0 Hz
} BASS_DX8_I3DL2REVERB;

typedef struct {
	float   fCenter;
	float   fBandwidth;
	float   fGain;
} BASS_DX8_PARAMEQ;

typedef struct {
	float   fInGain;                // [-96.0,0.0]            default: 0.0 dB
	float   fReverbMix;             // [-96.0,0.0]            default: 0.0 db
	float   fReverbTime;            // [0.001,3000.0]         default: 1000.0 ms
	float   fHighFreqRTRatio;       // [0.001,0.999]          default: 0.001
} BASS_DX8_REVERB;

#define BASS_DX8_PHASE_NEG_180        0
#define BASS_DX8_PHASE_NEG_90         1
#define BASS_DX8_PHASE_ZERO           2
#define BASS_DX8_PHASE_90             3
#define BASS_DX8_PHASE_180            4

typedef struct {
	float fTarget;
	float fCurrent;
	float fTime;
	DWORD lCurve;
} BASS_FX_VOLUME_PARAM;

typedef void (CALLBACK IOSNOTIFYPROC)(DWORD status);
/* iOS notification callback function.
status : The notification (BASS_IOSNOTIFY_xxx) */

#define BASS_IOSNOTIFY_INTERRUPT		1	// interruption started
#define BASS_IOSNOTIFY_INTERRUPT_END	2	// interruption ended

BOOL BASSDEF(BASS_SetConfig)(DWORD option, DWORD value);
DWORD BASSDEF(BASS_GetConfig)(DWORD option);
BOOL BASSDEF(BASS_SetConfigPtr)(DWORD option, const void *value);
const void *BASSDEF(BASS_GetConfigPtr)(DWORD option);
DWORD BASSDEF(BASS_GetVersion)(void);
int BASSDEF(BASS_ErrorGetCode)(void);

BOOL BASSDEF(BASS_GetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info);
#if defined(_WIN32) && !defined(_WIN32_WCE) && !(defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
BOOL BASSDEF(BASS_Init)(int device, DWORD freq, DWORD flags, HWND win, const void *dsguid);
#else
BOOL BASSDEF(BASS_Init)(int device, DWORD freq, DWORD flags, void *win, const void *dsguid);
#endif
BOOL BASSDEF(BASS_Free)(void);
BOOL BASSDEF(BASS_SetDevice)(DWORD device);
DWORD BASSDEF(BASS_GetDevice)(void);
BOOL BASSDEF(BASS_GetInfo)(BASS_INFO *info);
BOOL BASSDEF(BASS_Start)(void);
BOOL BASSDEF(BASS_Stop)(void);
BOOL BASSDEF(BASS_Pause)(void);
DWORD BASSDEF(BASS_IsStarted)(void);
BOOL BASSDEF(BASS_Update)(DWORD length);
float BASSDEF(BASS_GetCPU)(void);
BOOL BASSDEF(BASS_SetVolume)(float volume);
float BASSDEF(BASS_GetVolume)(void);
#if defined(_WIN32) && !defined(_WIN32_WCE) && !(defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
void *BASSDEF(BASS_GetDSoundObject)(DWORD object);
#endif

BOOL BASSDEF(BASS_Set3DFactors)(float distf, float rollf, float doppf);
BOOL BASSDEF(BASS_Get3DFactors)(float *distf, float *rollf, float *doppf);
BOOL BASSDEF(BASS_Set3DPosition)(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top);
BOOL BASSDEF(BASS_Get3DPosition)(BASS_3DVECTOR *pos, BASS_3DVECTOR *vel, BASS_3DVECTOR *front, BASS_3DVECTOR *top);
void BASSDEF(BASS_Apply3D)(void);

HPLUGIN BASSDEF(BASS_PluginLoad)(const char *file, DWORD flags);
BOOL BASSDEF(BASS_PluginFree)(HPLUGIN handle);
BOOL BASSDEF(BASS_PluginEnable)(HPLUGIN handle, BOOL enable);
const BASS_PLUGININFO *BASSDEF(BASS_PluginGetInfo)(HPLUGIN handle);

HSAMPLE BASSDEF(BASS_SampleLoad)(BOOL mem, const void *file, QWORD offset, DWORD length, DWORD max, DWORD flags);
HSAMPLE BASSDEF(BASS_SampleCreate)(DWORD length, DWORD freq, DWORD chans, DWORD max, DWORD flags);
BOOL BASSDEF(BASS_SampleFree)(HSAMPLE handle);
BOOL BASSDEF(BASS_SampleSetData)(HSAMPLE handle, const void *buffer);
BOOL BASSDEF(BASS_SampleGetData)(HSAMPLE handle, void *buffer);
BOOL BASSDEF(BASS_SampleGetInfo)(HSAMPLE handle, BASS_SAMPLE *info);
BOOL BASSDEF(BASS_SampleSetInfo)(HSAMPLE handle, const BASS_SAMPLE *info);
DWORD BASSDEF(BASS_SampleGetChannel)(HSAMPLE handle, DWORD flags);
DWORD BASSDEF(BASS_SampleGetChannels)(HSAMPLE handle, HCHANNEL *channels);
BOOL BASSDEF(BASS_SampleStop)(HSAMPLE handle);

HSTREAM BASSDEF(BASS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, STREAMPROC *proc, void *user);
HSTREAM BASSDEF(BASS_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSDEF(BASS_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
HSTREAM BASSDEF(BASS_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *proc, void *user);
BOOL BASSDEF(BASS_StreamFree)(HSTREAM handle);
QWORD BASSDEF(BASS_StreamGetFilePosition)(HSTREAM handle, DWORD mode);
DWORD BASSDEF(BASS_StreamPutData)(HSTREAM handle, const void *buffer, DWORD length);
DWORD BASSDEF(BASS_StreamPutFileData)(HSTREAM handle, const void *buffer, DWORD length);

HMUSIC BASSDEF(BASS_MusicLoad)(BOOL mem, const void *file, QWORD offset, DWORD length, DWORD flags, DWORD freq);
BOOL BASSDEF(BASS_MusicFree)(HMUSIC handle);

BOOL BASSDEF(BASS_RecordGetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info);
BOOL BASSDEF(BASS_RecordInit)(int device);
BOOL BASSDEF(BASS_RecordFree)(void);
BOOL BASSDEF(BASS_RecordSetDevice)(DWORD device);
DWORD BASSDEF(BASS_RecordGetDevice)(void);
BOOL BASSDEF(BASS_RecordGetInfo)(BASS_RECORDINFO *info);
const char *BASSDEF(BASS_RecordGetInputName)(int input);
BOOL BASSDEF(BASS_RecordSetInput)(int input, DWORD flags, float volume);
DWORD BASSDEF(BASS_RecordGetInput)(int input, float *volume);
HRECORD BASSDEF(BASS_RecordStart)(DWORD freq, DWORD chans, DWORD flags, RECORDPROC *proc, void *user);

double BASSDEF(BASS_ChannelBytes2Seconds)(DWORD handle, QWORD pos);
QWORD BASSDEF(BASS_ChannelSeconds2Bytes)(DWORD handle, double pos);
DWORD BASSDEF(BASS_ChannelGetDevice)(DWORD handle);
BOOL BASSDEF(BASS_ChannelSetDevice)(DWORD handle, DWORD device);
DWORD BASSDEF(BASS_ChannelIsActive)(DWORD handle);
BOOL BASSDEF(BASS_ChannelGetInfo)(DWORD handle, BASS_CHANNELINFO *info);
const char *BASSDEF(BASS_ChannelGetTags)(DWORD handle, DWORD tags);
DWORD BASSDEF(BASS_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask);
BOOL BASSDEF(BASS_ChannelLock)(DWORD handle, BOOL lock);
BOOL BASSDEF(BASS_ChannelFree)(DWORD handle);
BOOL BASSDEF(BASS_ChannelPlay)(DWORD handle, BOOL restart);
BOOL BASSDEF(BASS_ChannelStop)(DWORD handle);
BOOL BASSDEF(BASS_ChannelPause)(DWORD handle);
BOOL BASSDEF(BASS_ChannelUpdate)(DWORD handle, DWORD length);
BOOL BASSDEF(BASS_ChannelSetAttribute)(DWORD handle, DWORD attrib, float value);
BOOL BASSDEF(BASS_ChannelGetAttribute)(DWORD handle, DWORD attrib, float *value);
BOOL BASSDEF(BASS_ChannelSlideAttribute)(DWORD handle, DWORD attrib, float value, DWORD time);
BOOL BASSDEF(BASS_ChannelIsSliding)(DWORD handle, DWORD attrib);
BOOL BASSDEF(BASS_ChannelSetAttributeEx)(DWORD handle, DWORD attrib, void *value, DWORD size);
DWORD BASSDEF(BASS_ChannelGetAttributeEx)(DWORD handle, DWORD attrib, void *value, DWORD size);
BOOL BASSDEF(BASS_ChannelSet3DAttributes)(DWORD handle, int mode, float min, float max, int iangle, int oangle, float outvol);
BOOL BASSDEF(BASS_ChannelGet3DAttributes)(DWORD handle, DWORD *mode, float *min, float *max, DWORD *iangle, DWORD *oangle, float *outvol);
BOOL BASSDEF(BASS_ChannelSet3DPosition)(DWORD handle, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel);
BOOL BASSDEF(BASS_ChannelGet3DPosition)(DWORD handle, BASS_3DVECTOR *pos, BASS_3DVECTOR *orient, BASS_3DVECTOR *vel);
QWORD BASSDEF(BASS_ChannelGetLength)(DWORD handle, DWORD mode);
BOOL BASSDEF(BASS_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode);
QWORD BASSDEF(BASS_ChannelGetPosition)(DWORD handle, DWORD mode);
DWORD BASSDEF(BASS_ChannelGetLevel)(DWORD handle);
BOOL BASSDEF(BASS_ChannelGetLevelEx)(DWORD handle, float *levels, float length, DWORD flags);
DWORD BASSDEF(BASS_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
HSYNC BASSDEF(BASS_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, void *user);
BOOL BASSDEF(BASS_ChannelRemoveSync)(DWORD handle, HSYNC sync);
BOOL BASSDEF(BASS_ChannelSetLink)(DWORD handle, DWORD chan);
BOOL BASSDEF(BASS_ChannelRemoveLink)(DWORD handle, DWORD chan);
HDSP BASSDEF(BASS_ChannelSetDSP)(DWORD handle, DSPPROC *proc, void *user, int priority);
BOOL BASSDEF(BASS_ChannelRemoveDSP)(DWORD handle, HDSP dsp);
HFX BASSDEF(BASS_ChannelSetFX)(DWORD handle, DWORD type, int priority);
BOOL BASSDEF(BASS_ChannelRemoveFX)(DWORD handle, HFX fx);

BOOL BASSDEF(BASS_FXSetParameters)(HFX handle, const void *params);
BOOL BASSDEF(BASS_FXGetParameters)(HFX handle, void *params);
BOOL BASSDEF(BASS_FXSetPriority)(HFX handle, int priority);
BOOL BASSDEF(BASS_FXReset)(DWORD handle);

#ifdef __cplusplus
}

#if defined(_WIN32) && !defined(NOBASSOVERLOADS)
static inline HPLUGIN BASS_PluginLoad(const WCHAR *file, DWORD flags)
{
	return BASS_PluginLoad((const char*)file, flags | BASS_UNICODE);
}

static inline HMUSIC BASS_MusicLoad(BOOL mem, const WCHAR *file, QWORD offset, DWORD length, DWORD flags, DWORD freq)
{
	return BASS_MusicLoad(mem, (const void*)file, offset, length, flags | BASS_UNICODE, freq);
}

static inline HSAMPLE BASS_SampleLoad(BOOL mem, const WCHAR *file, QWORD offset, DWORD length, DWORD max, DWORD flags)
{
	return BASS_SampleLoad(mem, (const void*)file, offset, length, max, flags | BASS_UNICODE);
}

static inline HSTREAM BASS_StreamCreateFile(BOOL mem, const WCHAR *file, QWORD offset, QWORD length, DWORD flags)
{
	return BASS_StreamCreateFile(mem, (const void*)file, offset, length, flags | BASS_UNICODE);
}

static inline HSTREAM BASS_StreamCreateURL(const WCHAR *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user)
{
	return BASS_StreamCreateURL((const char*)url, offset, flags | BASS_UNICODE, proc, user);
}

static inline BOOL BASS_SetConfigPtr(DWORD option, const WCHAR *value)
{
	return BASS_SetConfigPtr(option | BASS_UNICODE, (const void*)value);
}
#endif
#endif

#ifdef __OBJC__
#undef BOOL
#endif

#endif
