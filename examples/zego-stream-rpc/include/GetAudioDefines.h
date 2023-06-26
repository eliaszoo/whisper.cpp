#ifndef __GET_AUDIO_DEFINES_H__
#define __GET_AUDIO_DEFINES_H__

#define ZEGO_EXPRESS_MAX_ROOMID_LEN                         (128)
#define ZEGO_EXPRESS_MAX_USERID_LEN                         (64)

/** Video frame format. */
enum zego_video_frame_format
{
    /** BGRA32(RGBA32 for Android) */
    VIDEO_RENDER_TYPE_RGB = 1,
    /** I420 */
    VIDEO_RENDER_TYPE_YUV = 2,
    /** Unknown format */
    VIDEO_RENDER_TYPE_UNKNOWN
};

/**
 * @brief Play stream mode
 * 
 */
enum zego_play_stream_mode
{
    /** Play stream by stream id */
    PLAY_STREAM_MODE_SINGLE,
    /** Play mix stream of room */
    PLAY_STREAM_MODE_ROOM_MIX,
    /** Play all streams of room */
    PLAY_STREAM_MODE_ROOM_ALL
};

/**
 * @brief Play stream type
 * 
 */
enum zego_play_stream_type
{
    /** Play audio only */
    PLAY_STREAM_TYPE_AUDIO,
    /** Play video only */
    PLAY_STREAM_TYPE_VIDEO,
    /** Play audio and video both */
    PLAY_STREAM_TYPE_BOTH
};

/** audio sample rate. */
enum zego_audio_sample_rate
{
    /** Unknown */
    ZEGO_AUDIO_SAMPLE_RATE_UNKNOWN = 0,

    /** 16K */
    ZEGO_AUDIO_SAMPLE_RATE_16K = 16000,

    /** 32K */
    ZEGO_AUDIO_SAMPLE_RATE_32K = 32000,

    /** 44.1K */
    ZEGO_AUDIO_SAMPLE_RATE_44K = 44100,

    /** 48K */
    ZEGO_AUDIO_SAMPLE_RATE_48K = 48000
};

/**
 * Object for video frame fieldeter.
 *
 * Including video frame format, width and height, etc.
 */
struct zego_video_frame_param
{
    /** Video frame format */
    enum zego_video_frame_format format;

    /** Number of bytes per line (for example: BGRA only needs to consider strides [0], I420 needs to consider strides [0,1,2]) */
    int strides[4];

    /** Video frame width. When use custom video capture, the video data meeting the 32-bit alignment can obtain the maximum performance. Taking BGRA as an example, width * 4 is expected to be multiple of 32. */
    int width;

    /** Video frame height */
    int height;

    /** Video frame timestamp */
    unsigned long long timestamp;
};

/**
 * @brief player config by room id
 * 
 */
struct zego_room_player_config
{
    /** room id */
    char room_id[ZEGO_EXPRESS_MAX_ROOMID_LEN];
    /** user id */
    char user_id[ZEGO_EXPRESS_MAX_USERID_LEN];
    /** play stream type */
    zego_play_stream_type play_stream_type = PLAY_STREAM_TYPE_AUDIO;
    /** if auto mix audio stream */
    bool is_auto_mix_audio_stream = true;
};

/**
 * @brief SDK config
 * 
 */
struct zego_sdk_config
{
    /** Audio sample rate for playing audio stream */
    zego_audio_sample_rate audio_sample_rate = ZEGO_AUDIO_SAMPLE_RATE_44K;
};

#endif