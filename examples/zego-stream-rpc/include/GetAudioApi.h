//
// GetAudioApi.h
//
// Copyright 2019 ZEGO. All rights reserved.
// 

#pragma once

#ifdef WIN32

#ifdef ZegoGetAudioLib_EXPORTS
#define ZEGO_GETAUDIO_API __declspec(dllexport)
#elif defined(ZEGO_GETAUDIO_STATIC)
#define ZEGO_GETAUDIO_API // * nothing
#else
#define ZEGO_GETAUDIO_API __declspec(dllimport)
#endif

#else

#define ZEGO_GETAUDIO_API __attribute__((visibility("default")))

#endif

#include "GetAudioDefines.h"

//////////////////////////////////////////////////////////////////////////
/*   
 * 方式一：通过流id拉单流方式鉴黄，每个进程最多拉取32路流，超过32路流时，请启动新的进程进行拉流
 *   
 *   主要调用接口（详细调用方式查看demo代码）：
 *   zego_api_set_use_test_env -> zego_api_set_audio_data_callback -> zego_api_login_by_token -> zego_api_start_get_audio_data --> 在OnAudioData中获取得到每条流的pcm数据
 *   OnGetAudioStateUpdateFun 收到状态5（单进程最大拉流上限）时，启动新的进程进行拉流；收到状态7（超时无数据异常）时调用zego_api_stop_get_audio_data停止拉流，并回调给业务方鉴黄任务已异常停止；
 *   单进程所有流都停止后，调用zego_api_logout退出当前进程
 *
 * 方式二：以房间为单位的音频混流方式鉴黄，配置要鉴黄的房间id，sdk内部自动拉取房间内存在的音频流，并进行混合成一路pcm数据后，回调出来
 *
 *   主要调用接口（详细调用方式查看demo代码）：
 *   zego_api_set_use_test_env -> zego_api_set_audio_data_callback -> zego_api_init_get_audio_data_by_room_id -> zego_api_login_by_token -> 在OnAudioData中获取得到房间内混合后的pcm数据
 *
 * 方式三：以房间为单位的房间内每条音视频流方式鉴黄，配置要鉴黄的房间id，sdk内部自动拉取房间内存在的音频流，并将音视频数据回调出来
 *
 *   主要调用接口（详细调用方式查看demo代码）：
 *   zego_api_set_use_test_env -> zego_api_set_per_audio_data_callback -> zego_api_init_get_data_by_room_id -> zego_register_custom_video_render_remote_frame_data_callback -> zego_api_login_by_token -> 在OnPerAudioData中获取得到房间内每条音频流的pcm数据
 *
 * 方式四：视频快照方式鉴黄，配置视频数据回调和快照回调，主动调用接口生成视频快照，支持jpg/png格式图片
 * 
 *   主要调用接口（详细调用方式查看demo代码）：
 *   zego_api_set_use_test_env -> zego_api_set_take_snapshot_callback -> zego_register_custom_video_render_remote_frame_data_callback -> zego_api_login_by_token -> zego_api_take_snapshot 在 OnSnapshot 中查看快照返回码
 */
//////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C"
{
#endif

    // 拉流的流信息
    typedef struct ZegoPlayStreamInfo
    {
        char user_id[128];  // 用户id
        char user_name[128];// 用户名字
        char stream_id[512];// 流id

    }ZegoPlayStreamInfo;

/**
    登录回调

    @param error_code 登录错误码，0 - 登录成功；1 - 网络错误；2 - token无效； 3 - 时间戳过期；4 - 其它错误；5 - 开始登录房间
    @param user_data 用户自定义数据，zego_api_login_by_token带的user_data，回调时透传该数据
*/
typedef void(*OnLoginFun)(int error_code, void * user_data);


/**
    音频数据回调

    @param stream_id_or_room_id   音频流的流id或者房间id，以房间为单位获取音频流的模式返回的是房间id
    @param audio_data  音频数据pcm的buffer
    @param data_len    数据长度，字节数
    @param sample_rate 音频采样率
    @param channels    通道数
    @param user_data   用户自定义数据，zego_api_set_audio_data_callback设置的user_data，回调时透传该数据
*/
typedef void(*OnAudioDataFun)(const char * stream_id_or_room_id, unsigned char * audio_data, int data_len, int sample_rate, int channels, void * user_data);

/**
    房间内每条流的音频数据回调

    @param room_id     房间id
    @param stream_id   音频流的流id
    @param audio_data  音频数据pcm的buffer
    @param data_len    数据长度，字节数
    @param sample_rate 音频采样率
    @param channels    通道数
    @param timestamp   音频帧时间戳
    @param user_data   用户自定义数据，zego_api_set_audio_data_callback设置的user_data，回调时透传该数据
*/
typedef void(*OnPerAudioDataFun)(const char * room_id, const char * stream_id, unsigned char * audio_data, int data_len, int sample_rate, int channels, unsigned long long timestamp, void * user_data);


/**
    视频数据回调

    @param stream_id   视频流的流id
    @param video_data  视频数据为BGRA32的buffer
    @param data_len    数据长度，字节数
    @param width       视频分辨率宽度
    @param height      视频分辨率高度
    @param user_data   用户自定义数据，zego_api_set_video_data_callback设置的user_data，回调时透传该数据
*/
typedef void(*OnVideoDataFun)(const char * stream_id, unsigned char * video_data, int data_len, int width, int height, void * user_data);

/**
    房间内每条流的视频帧数据回调
    @param stream_id   视频流的流id
    @param data 视频数据每个面的起始地址，共四个面
    @param data_length 视频数据每个面的长度起始地址
    @param param 视频帧参数
    @param user_data   用户自定义数据，zego_register_custom_video_render_remote_frame_data_callback 设置的 user_context，回调时透传该数据
*/
typedef void(*zego_on_custom_video_render_remote_frame_data)(const char * room_id, const char * stream_id, unsigned char** data, unsigned int* data_length, struct zego_video_frame_param param, void * user_context);

/**
    获取音频数据状态更新
    @param stream_id 音频流的流id
    @param state_code 拉流状态码，0 - 拉流成功；1 - 重试拉流；2 - 重试成功；3 - 拉流临时中断；4 - 拉流失败，其它原因；5 - 拉流失败，已达到sdk最大拉流数，发生该错误时，请启动新的进程调用sdk在进行拉流操作； 6 - 开启以房间为单位的模式下，不支持zego_api_start_get_audio_data接口；7 - 开启以房间为单位的模式下，房间内没有流已超过静默时间。单流鉴黄表示多长时间没收到音频流数据超时；197612 - 拉流失败，流不存在
    @param user_data   用户自定义数据，zego_api_set_play_state_update_callback设置的user_data，回调时透传该数据
*/
typedef void(*OnGetAudioStateUpdateFun)(const char * stream_id, int state_code, void * user_data);

/**
    连接状态更新回调
    @param state_code 状态码，0 - 连接上服务器；1 - 网络临时中断，内部进行重连；2 - 重新连上服务器；3 - 内部重连失败后通知的连接断开，收到该错误码后内部不再进行重连；4 - 被踢掉线
    @param user_data   用户自定义数据，zego_api_set_connection_state_update_callback设置的user_data，回调时透传该数据
*/
typedef void(*OnConnectionUpdateFun)(int state_code, void * user_data);

/**
    截图回调

    @param error_code 截图错误码，0 - 截图成功；1 - 路径不存在；2 图像格式不支持 3 - 截图超时；4 参数错误 5 调用频率太快 6 其它错误
    @param snapshot_file  截图文件全路径
    @param user_data 用户自定义数据，zego_api_login_by_token带的user_data，回调时透传该数据
*/
typedef void(*OnSnapshotFun)(int error_code, const char * snapshot_file, void * user_data);

/**
    收到远端流的 SEI 内容
    @param stream_id 拉流的流 ID。
    @param data SEI 内容，单位字节。
    @param data_length SEI 内容长度。
    @param user_data 用户自定义数据，zego_register_player_recv_sei_callback 带的 user_data，回调时透传该数据
 */
typedef void (*zego_on_player_recv_sei)(const char *stream_id, const unsigned char *data, unsigned int data_length, void *user_data);

/**
    收到远端流的音频次要信息内容
    @param stream_id 拉流的流 ID。
    @param data 音频次要信息内容，单位字节。
    @param data_length 音频次要信息内容长度。
    @param user_data 用户自定义数据，zego_register_player_recv_sei_callback 带的 user_data，回调时透传该数据
 */
typedef void (*zego_on_player_recv_audio_side_info)(const char *stream_id, const unsigned char *data, unsigned int data_length, void *user_data);

/**
    设置拉流状态回调
    @param status_cb  拉流状态回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_play_state_update_callback(OnGetAudioStateUpdateFun status_cb, void * user_data);


/**
    设置音频数据回调，不带时间戳版本，适用于以房间为单位拉音频混流、非房间模式拉单流
    @param audio_data_cb  音频数据回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_audio_data_callback(OnAudioDataFun audio_data_cb, void * user_data);

/**
    设置获取房间内每条流的音频数据回调，适用于以房间为单位拉所有流、以房间为单位拉单流
    @param per_audio_data_cb  房间内每条流的音频数据回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_per_audio_data_callback(OnPerAudioDataFun per_audio_data_cb, void * user_data);


/**
    设置视频数据回调，不带时间戳版本，适用于拉视频流
    @param video_data_cb  视频数据回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_video_data_callback(OnVideoDataFun video_data_cb, void * user_data);

/**
    注册远端拉流视频帧裸数据回调，适用于拉视频流
    @param callback_func  视频数据回调函数
    @param user_context  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_register_custom_video_render_remote_frame_data_callback(zego_on_custom_video_render_remote_frame_data callback_func, void * user_context);

/**
    设置截图状态回调
    @param snapshot_cb  截图结果回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_take_snapshot_callback(OnSnapshotFun snapshot_cb, void * user_data);

/**
    设置连接状态回调函数
    @param connection_cb  连接状态回调函数
    @param user_data  用户自定义数据，回调时回传该数据
*/
ZEGO_GETAUDIO_API void zego_api_set_connection_state_update_callback(OnConnectionUpdateFun connection_cb, void * user_data);

/**
    注册拉流 SEI 数据回调
    @param callback_func SEI 数据回调函数
    @param user_data 用户自定义数据，回调时回传该数据
 */
ZEGO_GETAUDIO_API void zego_register_player_recv_sei_callback(zego_on_player_recv_sei callback_func, void *user_data);

/**
    注册拉音频流次要信息数据回调
    @param callback_func 音频次要信息数据回调函数
    @param user_data 用户自定义数据，回调时回传该数据
 */
ZEGO_GETAUDIO_API void zego_register_player_recv_audio_side_info_callback(zego_on_player_recv_audio_side_info callback_func, void *user_data);

/**
    设置静默时间
    房间鉴黄模式下超过该时间时：sdk内部会停止拉流，自动退出房间，超过该时间房间内都没有流，则OnGetAudioStateUpdateFun回调状态码7，并且自动退出登录
    单流鉴黄模式下超过该时间，通过OnGetAudioStateUpdateFun回调超时状态表示多长时间没有收到音频数据的超时，回调超时异常后，将不在回调OnAudioDataFun出这条流的pcm，收到超时回调时，需要停止拉流zego_api_stop_get_audio_data，并且回调业务方该流鉴黄已经异常停止
    不进行设置时默认是5min
    @param idle_time_ms 单位毫秒
*/
ZEGO_GETAUDIO_API void zego_api_set_idle_time(int idle_time_ms);

/**
    设置以房间为单位获取音频数据,必须要在zego_api_login_by_token前调用
    @param room_id 房间id
    @param user_id  用户id，使用该用户id登录到该房间进行拉流操作，需保证唯一，且要保证与业务方的用户id不冲突；如果业务方不指定，请传入空字符串，默认会生成随机唯一uuid作为该用户id登录到指定房间进行拉流操作
    @param mix_audio_stream 音频数据混流标志，默认房间内返回的是音频混流数据，如果想要返回房间内每条流的音频数据，需要把 mix_audio_stream 设置为false
*/
ZEGO_GETAUDIO_API void zego_api_init_get_audio_data_by_room_id(const char * room_id, const char * user_id, bool mix_audio_stream = true);

/**
    设置以房间为单位获取音视频数据,必须要在zego_api_login_by_token前调用
    @param config 拉流配置
*/
ZEGO_GETAUDIO_API void zego_api_init_get_data_by_room_id(zego_room_player_config config);

/**
    获取当前的拉流播放的流信息列表，在以房间为单位获取音频流的模式下有效
    @param count 当前播放流的个数,输出参数
    @return 流信息数组地址
*/
ZEGO_GETAUDIO_API ZegoPlayStreamInfo* zego_api_get_play_list(int & count);

/**
    登录
    @param token token字符串
    @param user_data  用户自定义数据，回调时回传该数据
    @param login_cb 登录回调函数
*/
ZEGO_GETAUDIO_API void zego_api_login_by_token(const char * token, void * user_data, OnLoginFun login_cb);


/**
    通过流id，开始拉取音频数据
    @param stream_id 要拉取音频流的流id
*/
ZEGO_GETAUDIO_API void zego_api_start_get_audio_data(const char * stream_id);

/**
    停止通过流id获取音频数据
    @param stream_id 要停止拉流的流id
*/
ZEGO_GETAUDIO_API void zego_api_stop_get_audio_data(const char * stream_id);


/**
    通过流id，开始拉取音视频数据
    @param stream_id 要拉取的流id
    @param media_type 要拉取流的媒体类型 1 只拉音频 2 只拉视频 3 拉音视频, 默认值为1
*/
ZEGO_GETAUDIO_API void zego_api_start_get_media_data(const char * stream_id, int media_type);

/**
    停止通过流id获取音视频数据
    @param stream_id 要停止拉流的流id
*/
ZEGO_GETAUDIO_API void zego_api_stop_get_media_data(const char * stream_id);


/**
    退出登录
*/
ZEGO_GETAUDIO_API void zego_api_logout();

/**
    设置使用的zego环境，必须在zego_api_login_by_token之前调用
    @param test_env true-使用测试环境；false-使用正式环境
*/
ZEGO_GETAUDIO_API void zego_api_set_use_test_env(bool test_env);

/**
    设置日志路径和大小
    @param log_dir 日志路径
    @param log_size 日志大小
*/
ZEGO_GETAUDIO_API void zego_api_set_log_dir_and_size(const char * log_dir, unsigned long long log_size);

/**
    是否开启vad检测功能，开启vad检测后，拉到的pcm会经过vad检测，开启后如果是静音数据则不会通过OnAudioDataFun回调出来
    @param enable true - 开启，false - 关闭，默认关闭
    
    @note 以房间为单位的音频单流模式不支持vad检测，即如果调用 zego_api_init_get_audio_data_by_room_id 的 mix_audio_stream为false时，vad检测功能无效
*/
ZEGO_GETAUDIO_API void zego_api_enable_vad(bool enable);

/**
    截图功能
    @param snap_file 截图文件路径，支持只传目录(目录需存在且有写权限)；也支持带文件名 如 "/tmp/test.jpg", "/tmp/test.png"
                     如果只传文件目录，默认截图文件名会按照 streamid_timstamp 规则生成
    @param image_format 截图图片格式，支持 jpg 和png; 1 jpg 2 png ,默认为jpg
    @param time_out  截图超时时间,单位为秒，默认 1秒 

    @note 调用频率需大于截图超时时间，如超时设置1秒，调用频率需要大于1秒
    @note snap_file 支持传空路径，默认会把截图文件保存到程序当前目录
*/
ZEGO_GETAUDIO_API void zego_api_take_snapshot(const char *snapshot_file, int image_format = 1, int idle_time = 1);

/**
    自定义配置
    @param zego_sdk_config SDK 配置，用于自定义配置参数

    @note 调用时机：拉流前调用，即在 zego_api_login_by_token 前调用
*/
ZEGO_GETAUDIO_API void zego_api_set_config(zego_sdk_config config);

#ifdef __cplusplus
}
#endif
