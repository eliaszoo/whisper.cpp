// Real-time speech recognition of input from a microphone
//
// A very quick-n-dirty implementation serving mainly as a proof of concept.
//

#include "GetAudioApi.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <cstring>
#include <mutex>
#include <grpcpp/grpcpp.h>

#include "transcribe.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using transcribe::AudioData;
using transcribe::Text;
using transcribe::Transcriber;

//static std::string token_ = "sEub2VayRtw0TXbDS7Ev/WLmC9hx8ay+R62Fiak/I6jHPrJaQs4kv7U5RQVXtZ+VgecNELHvUrH6q+uVF4TV3k8FvTTQgBVar6dXnHWr+pLitO1WLqbQ9f38XgS0Q2MuHLI/+azd8FDkZTPg/Yf3C+rtK4wk5+rujCyM203iowUKqT7Oe/HURcT369NqOvJ3QkmB+vk/Z3gw/9ACfXU0JtcR+Ssehp6K5Y3tp8an+LMIx1ENcF6s5xG9+6KpNS3ectGKr1H1BC4Vi8hJaFiclkG/W4JhXvlB+LAW0WU3w4WjXqmfqV8AFpDuA+J4/sd3+tY1G+DS90BVqbHv8uh2aJlUXxECDDfexSAsNBoRB+Ug2/lxfIW1BfRG58tWV+HL/8H6mBBRNG3A3jYNYJiydk9KwfaYyV0hmoqdjULKv6iGHMiwC/5TVSvKNwqplDM+ep20hEg8MahJjntp/pSAd7OfnPvkkm1VK0XsrtsA9NbboWIMrR50gpP5meqIv9I4";                         // 用于测试的 token
static std::string token_ = "ZJMLarU9zhcgCpLw+J/efbCfD4KSdKRmOumBMw3JJqP4twaNcUjHql7ZHvKLO/tgKysE8xWnApute+ry9h55KcOxLUAUIkZU2MEwE2hesgL90yb/k3ytyfiR6ttguQtHkTX22K3cVB+Xe68zpoK1aDCFb08hAp7vwFgDQvNZOG1zX5V/DfLSlWCTS7TZf7tnU5TPQ9xtRdD7Sys1QQRycj7CPqXZMGsXrPHREA+vtv7mMMZG0VJBTm+mx6HZ4aJSBc/UPaxC2E0fcKwJzzmd3P0SrJXBdRH9OoCgXuiuiz/tk4bbMYEqpNMellJVK6ODbPH0I+fCn+2Udeje9YcVQqa07XVU27WcXnCMOClq5UVrRKewWHOr4ajv8YJ5heDDtgc1rr4nuPvna3TZerUUQgLL6NNUfVNRbTbUrjZivZJxRB3vj2FCFSJi49deFV+wDeIxra7aY5C8lrkG2a2DOkwcm8Hd4ACYnlDla8wialiU8hzdaxPMxIwzKz7VuL44";
static int g_play_mode_ = -1;                           // 测试拉流模式，不用修改
static std::string g_user_id_ = "user_1";               // 用户id，必须全局唯一
static std::string g_room_id_ = "room_1";               // 房间id，适用于以房间为单位拉流场景
static std::string g_play_stream_id_1_ = "stream_1";    // 拉流id，拉单流模式下设置第一路拉流id
static std::string g_play_stream_id_2_ = "stream_2";    // 拉流id，拉单流模式下设置第二路拉流id
// FILE * video_file = nullptr;                            // 不用修改
// std::string video_file_path = "/tmp/test_video.rgb";    // 测试视频数据保存到本地的路径
int idle_time = 20000;                                  // 设置静默时间 20s，适用于以房间为单位拉流场景，超过该时间，回调提示错误
std::string snap_file_1_ = "/tmp/test.jpg";             // 设置jpg格式截图存储路径1
std::string snap_file_2_ = "/tmp/test.png";             // 设置png格式截图存储路径2

class TranscriberClient {
public:
    TranscriberClient(std::shared_ptr<Channel> channel)
        : stub_(Transcriber::NewStub(channel)) {}

    TranscriberClient(){}

    std::string Transcribe(unsigned char * audio_data, int data_len) {
        AudioData audioData;
        // 将音频数据读取并填充到audioData中，这里只是简单示例，你需要根据实际情况进行实现
        int n_samples = data_len/sizeof(int16_t);
        std::vector<int16_t> intArr(n_samples);
        memcpy(intArr.data(), audio_data, data_len);

        std::vector<float> floatArr(n_samples);
        for (uint64_t i = 0; i < n_samples; i++) {
            floatArr[i] = float(intArr[i])/32768.0f;
            audioData.add_data(floatArr[i]);
        }

        ClientContext context;
        Text text;
        stub_->Trans(&context, audioData, &text);

        /*std::shared_ptr<grpc::ClientReaderWriter<AudioData, Text>> stream(
            stub_->Trans(&context));

        // 发送音频数据
        stream->Write(audioData);
        /*
        while (true) {
            // 将音频数据写入到流中，这里只是简单示例，你需要根据实际情况进行实现
            if (!stream->Write(audioData)) {
                break;
            }
        }*/
        //stream->WritesDone();

        //Text text;
        //std::string transcript;

        // 接收转录文本
        /*while (stream->Read(&text)) {
            transcript += text.content();
        }

        Status status = stream->Finish();
        if (status.ok()) {
            return transcript;
        } else {
            std::cout << "Transcription RPC failed: " << status.error_message()
                      << std::endl;
            return "";
        }*/
        return "";
    }

private:
    std::unique_ptr<Transcriber::Stub> stub_;
};

std::string server_address = "localhost:50051";
TranscriberClient client;

void OnAudioData(const char * stream_id_or_room_id, unsigned char * audio_data, int data_len, int sample_rate, int channels, void * user_data)
{
    printf("OnAudioData, stream id = %s, len = %d, user data = %s, sample_rate = %d \n", stream_id_or_room_id, data_len, (char*)user_data, sample_rate);
    //trans(audio_data, data_len);

    client.Transcribe(audio_data, data_len);
}

struct AudioFileInfo
{
    /* data */
    FILE *audio_file = nullptr;
    char audio_file_path[256] = {0};
    std::string stream_id;
};

std::vector<AudioFileInfo> audioFileInfos; //测试存储pcm文件
FILE* pcm = 0;
void OnPerAudioData(const char * room_id, const char * stream_id, unsigned char * audio_data, int data_len, int sample_rate, int channels, unsigned long long timestamp, void * user_data)
{
    printf("OnPerAudioData, room id = %s, streamid = %s, len = %d, sample_rate = %d, timstamp = %lld, channel = %d, user data = %s \n", room_id, stream_id, data_len, sample_rate, timestamp, channels, (char*)user_data);
    /*if (pcm == nullptr) {
        pcm = fopen("test.wav", "wb+");
    } else {
        fwrite(audio_data, 1, data_len, pcm);
    }*/

    client.Transcribe(audio_data, data_len);

    /*printf("OnPerAudioData, room id = %s, streamid = %s, len = %d, sample_rate = %d, timstamp = %lld, user data = %s \n", room_id, stream_id, data_len, sample_rate, timestamp, (char*)user_data);
#if 0
    bool is_new_add = true;
    for(int i = 0; i < audioFileInfos.size(); i++){
        AudioFileInfo info = audioFileInfos.at(i);
        if(stream_id == info.stream_id){
            is_new_add = false;
            if(info.audio_file != nullptr){
                fwrite(audio_data, 1, data_len, info.audio_file);
            }
        }
    }

    if(is_new_add){
        AudioFileInfo info;
        info.stream_id = stream_id;
        sprintf(info.audio_file_path, "/tmp/%s_%s.pcm", room_id, stream_id);
        info.audio_file = fopen(info.audio_file_path, "wb+");
        audioFileInfos.push_back(info);
    }

#endif */
}

void OnVideoData(const char * stream_id, unsigned char * video_data, int data_len, int width, int height, void * user_data)
{
    // 视频流数据回调
    printf("OnVideoData, room id = %s, len = %d, width = %d height = %d \n", stream_id, data_len, width, height);
#if 0

    if (video_file == nullptr)
    {
        video_file = fopen(video_file_path.c_str(), "wb+");
        //PrintLogToView("write pcm file = %d, path = %s", audio_file, audio_file_path.c_str());
    }

    if (video_file != nullptr)
    {
        fwrite(video_data, 1, data_len, video_file);
    }
#endif
}

void OnSnapshot(int error_code, const char* snapshot_file, void * user_data)
{
      printf("OnSnapshot,error_code = %d, snapshot_file = %s\n", error_code, snapshot_file);
}

void OnGetAudioStateUpdate(const char * stream_id, int state_code, void * user_data)
{
    printf("OnGetAudioStateUpdate,stream_id = %s, state_code = %d, user data : %s\n", stream_id, state_code, (char*)user_data);

    // 超时停止
    if(state_code == 7)
    {
        printf("timeout: %s\n", stream_id);
        // 异常超时后停止拉流，此时应该通知业务方鉴黄任务异常停止
        zego_api_stop_get_audio_data(stream_id);

        // 因为一个进程停掉流一个鉴黄任务，此时可以在启动其它鉴黄任务，一个进程鉴黄任务（拉流总数）目前是32个，停掉一个后就还可以起一个
        zego_api_start_get_audio_data(g_play_stream_id_2_.c_str());
    }
}

void OnConnectionUpdate(int state_code, void * user_data)
{
    printf("OnConnectionUpdate,state_code = %d, user data : %s\n", state_code, (char*)user_data);
}

void OnLogin(int error_code, void * user_data)
{

    printf("OnLogin, error_code = %d, user data : %s\n", error_code, (char*)user_data);

    if (error_code == 0)
    {
        if(g_play_mode_ == 1 || g_play_mode_ == 4)
        {
            // 拉单流，1 音频，2 视频，3 音视频
            zego_api_start_get_media_data(g_play_stream_id_1_.c_str(), 3);
        }
    }
}

void OnRemoteVideoRenderData(const char *room_id, const char * stream_id, unsigned char** data, unsigned int* data_length, struct zego_video_frame_param param, void * user_context)
{
    //printf("OnRemoteVideoRenderData, room_id = %s, stream_id = %s, format:%d, width:%d, height:%d, timestamp:%lld\n", room_id, stream_id, param.format, param.width, param.height, param.timestamp);
}

int main(int argc, char ** argv) {
    client = TranscriberClient(grpc::CreateChannel(
        server_address, grpc::InsecureChannelCredentials()));
    // init sdk
    // 设置使用测试环境,true-表示使用zego测试环境，false-表示使用zego正式环境
    zego_api_set_use_test_env(false);

    // 设置获取音频数据状态回调
    zego_api_set_play_state_update_callback(OnGetAudioStateUpdate, nullptr);

    // 设置连接状态回调
    zego_api_set_connection_state_update_callback(OnConnectionUpdate, nullptr);

    // 开启拉流vad检测,拉流检测到静音数据时不会在通过OnAudioData回调出来
    zego_api_enable_vad(true);

    // 设置 SDK 参数
    zego_sdk_config config;
    config.audio_sample_rate = ZEGO_AUDIO_SAMPLE_RATE_16K;  //设置拉流音频采样率
    zego_api_set_config(config);

    zego_api_set_audio_data_callback(OnAudioData, nullptr);

    // 设置获取房间内每条流的音频数据回调
    zego_api_set_per_audio_data_callback(OnPerAudioData, nullptr);

    // 设置获取房间内每条流的视频数据回调
    zego_register_custom_video_render_remote_frame_data_callback(OnRemoteVideoRenderData, nullptr);

    // 设置以房间为单位获取音频流，必须在zego_api_login_by_token之前设置
    // 以房间为单位，超过 idle_time 时会自动停止拉流，并退出房间
    zego_room_player_config playerConfig;
    strncpy(playerConfig.room_id, g_room_id_.c_str(), ZEGO_EXPRESS_MAX_ROOMID_LEN);
    strncpy(playerConfig.user_id, g_user_id_.c_str(), ZEGO_EXPRESS_MAX_USERID_LEN);
    playerConfig.play_stream_type = zego_play_stream_type::PLAY_STREAM_TYPE_AUDIO;
    playerConfig.is_auto_mix_audio_stream = false;
    zego_api_init_get_data_by_room_id(playerConfig);

    zego_api_init_get_audio_data_by_room_id(g_room_id_.c_str(), g_user_id_.c_str());

    //zego_api_start_get_audio_data("stream1");

    // 设置静默时间 20s
    int idle_time = 20000;
    zego_api_set_idle_time(idle_time);

    zego_api_login_by_token(token_.c_str(), nullptr, OnLogin);

    bool is_running = true;

    printf("[Start speaking]");
    fflush(stdout);

    // main loop
    while (is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}