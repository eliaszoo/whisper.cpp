// Real-time speech recognition of input from a microphone
//
// A very quick-n-dirty implementation serving mainly as a proof of concept.
//

#include "common.h"
#include "whisper.h"
#include "GetAudioApi.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <cstring>

static std::string token_ = "sEub2VayRtw0TXbDS7Ev/WLmC9hx8ay+R62Fiak/I6jHPrJaQs4kv7U5RQVXtZ+VgecNELHvUrH6q+uVF4TV3k8FvTTQgBVar6dXnHWr+pLitO1WLqbQ9f38XgS0Q2MuHLI/+azd8FDkZTPg/Yf3C+rtK4wk5+rujCyM203iowUKqT7Oe/HURcT369NqOvJ3QkmB+vk/Z3gw/9ACfXU0JtcR+Ssehp6K5Y3tp8an+LMIx1ENcF6s5xG9+6KpNS3ectGKr1H1BC4Vi8hJaFiclkG/W4JhXvlB+LAW0WU3w4WjXqmfqV8AFpDuA+J4/sd3+tY1G+DS90BVqbHv8uh2aJlUXxECDDfexSAsNBoRB+Ug2/lxfIW1BfRG58tWV+HL/8H6mBBRNG3A3jYNYJiydk9KwfaYyV0hmoqdjULKv6iGHMiwC/5TVSvKNwqplDM+ep20hEg8MahJjntp/pSAd7OfnPvkkm1VK0XsrtsA9NbboWIMrR50gpP5meqIv9I4";                         // 用于测试的 token
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

// command-line parameters
struct whisper_params {
    int32_t n_threads  = std::min(4, (int32_t) std::thread::hardware_concurrency());
    int32_t step_ms    = 3000;
    int32_t length_ms  = 10000;
    int32_t keep_ms    = 200;
    int32_t capture_id = -1;
    int32_t max_tokens = 32;
    int32_t audio_ctx  = 0;

    float vad_thold    = 0.6f;
    float freq_thold   = 100.0f;

    bool speed_up      = false;
    bool translate     = false;
    bool no_fallback   = false;
    bool print_special = false;
    bool no_context    = true;
    bool no_timestamps = false;

    std::string language  = "en";
    std::string model     = "models/ggml-base.en.bin";
    std::string fname_out;
};

int n_samples_step;
int n_samples_len;
int n_samples_keep;
int n_samples_30s;
bool use_vad;
int n_new_line;
int n_iter = 0;

std::vector<float> pcmf32    (n_samples_30s, 0.0f);
std::vector<float> pcmf32_old;
std::vector<float> pcmf32_new(n_samples_30s, 0.0f);
std::vector<whisper_token> prompt_tokens;
struct whisper_context * ctx;
struct whisper_params params;
std::ofstream fout;
int count = 0;

//  500 -> 00:05.000
// 6000 -> 01:00.000
std::string to_timestamp(int64_t t) {
    int64_t sec = t/100;
    int64_t msec = t - sec*100;
    int64_t min = sec/60;
    sec = sec - min*60;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d.%03d", (int) min, (int) sec, (int) msec);

    return std::string(buf);
}

std::vector<float> audio;
int audio_pos = 0;
int audio_len = 0;
std::mutex       m_mutex;

void get(int ms, std::vector<float>& result) {
    result.clear();

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        size_t n_samples = (16000 * ms) / 1000;
        if (n_samples > audio_len) {
            n_samples = audio_len;
        }

        result.resize(n_samples);

        int s0 = audio_pos - n_samples;
        if (s0 < 0) {
            s0 += audio.size();
        }

        if (s0 + n_samples > audio.size()) {
            const size_t n0 = audio.size() - s0;

            memcpy(result.data(), &audio[s0], n0 * sizeof(float));
            memcpy(&result[n0], &audio[0], (n_samples - n0) * sizeof(float));
        } else {
            memcpy(result.data(), &audio[s0], n_samples * sizeof(float));
        }
    }
}

void trans(std::vector<float>& floatVec) {
    pcmf32_new.insert(pcmf32_new.end(), floatVec.begin(), floatVec.end());

    //printf("pcm size: %d, step:%d\n", (int) pcmf32_new.size(), n_samples_step);
    if (pcmf32_new.size() < n_samples_step) {
        return;
    }

    const int n_samples_new = pcmf32_new.size();

    // take up to params.length_ms audio from previous iteration
    const int n_samples_take = std::min((int) pcmf32_old.size(), std::max(0, n_samples_keep + n_samples_len - n_samples_new));

    //printf("processing: take = %d, new = %d, old = %d, keep = %d, len = %d\n", n_samples_take, n_samples_new, (int) pcmf32_old.size(), n_samples_keep, n_samples_len);

    pcmf32.resize(n_samples_new + n_samples_take);

    for (int i = 0; i < n_samples_take; i++) {
        pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
    }

    memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new*sizeof(float));

    //printf("processing: pcm size = %d, old = %d\n",(int) pcmf32.size(), (int) pcmf32_old.size());

    pcmf32_old = pcmf32;
    pcmf32_new.clear();

    /*char filename[32];
    sprintf(filename, "test%d.pcm", count++);
    FILE* p = fopen(filename, "wb+");
    fwrite(pcmf32.data(), sizeof(float), pcmf32.size(), p);
    fflush(p);
    fclose(p);*/

    // run the inference
    {
        whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

        wparams.print_progress   = false;
        wparams.print_special    = params.print_special;
        wparams.print_realtime   = false;
        wparams.print_timestamps = !params.no_timestamps;
        wparams.translate        = params.translate;
        wparams.single_segment   = !use_vad;
        wparams.max_tokens       = params.max_tokens;
        wparams.language         = params.language.c_str();
        wparams.n_threads        = params.n_threads;
        wparams.suppress_blank   = false;

        wparams.audio_ctx        = params.audio_ctx;
        wparams.speed_up         = params.speed_up;

        // disable temperature fallback
        //wparams.temperature_inc  = -1.0f;
        wparams.temperature_inc  = params.no_fallback ? 0.0f : wparams.temperature_inc;

        wparams.prompt_tokens    = params.no_context ? nullptr : prompt_tokens.data();
        wparams.prompt_n_tokens  = params.no_context ? 0       : prompt_tokens.size();

        if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0) {
            fprintf(stderr, "failed to process audio\n");
            return;
        }

        // print result;
        {
            printf("\33[2K\r");

            // print long empty line to clear the previous line
            printf("%s", std::string(100, ' ').c_str());

            printf("\33[2K\r");

            const int n_segments = whisper_full_n_segments(ctx);
            printf("once done segment: %d\n", n_segments);
            for (int i = 0; i < n_segments; ++i) {
                const char * text = whisper_full_get_segment_text(ctx, i);

                if (params.no_timestamps) {
                    printf("%s", text);
                    fflush(stdout);

                    if (params.fname_out.length() > 0) {
                        fout << text;
                    }
                } else {
                    const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
                    const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

                    printf ("[%s --> %s]  %s\n", to_timestamp(t0).c_str(), to_timestamp(t1).c_str(), text);

                    if (params.fname_out.length() > 0) {
                        fout << "[" << to_timestamp(t0) << " --> " << to_timestamp(t1) << "]  " << text << std::endl;
                    }
                }
            }

            if (params.fname_out.length() > 0) {
                fout << std::endl;
            }
        }

        ++n_iter;

        if (!use_vad && (n_iter % n_new_line) == 0) {
            printf("\n");

            // keep part of the audio for next iteration to try to mitigate word boundary issues
            pcmf32_old = std::vector<float>(pcmf32.end() - n_samples_keep, pcmf32.end());

            // Add tokens of the last full length segment as the prompt
            if (!params.no_context) {
                prompt_tokens.clear();

                const int n_segments = whisper_full_n_segments(ctx);
                for (int i = 0; i < n_segments; ++i) {
                    const int token_count = whisper_full_n_tokens(ctx, i);
                    for (int j = 0; j < token_count; ++j) {
                        prompt_tokens.push_back(whisper_full_get_token_id(ctx, i, j));
                    }
                }
            }
        }
        fflush(stdout);
    }
}

void transInOtherThread() {
    while (true)
    {
        std::vector<float> floatVec;
        while (true) {
            get(params.step_ms, floatVec);

            if ((int) pcmf32_new.size() > 2*n_samples_step) {
                fprintf(stderr, "\n\n%s: WARNING: cannot process audio fast enough, dropping audio ...\n\n", __func__);
                continue;
            }

            if ((int) pcmf32_new.size() >= n_samples_step) {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    audio_pos = 0;
                    audio_len = 0;
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        trans(floatVec);
    }
    
}

void trans(unsigned char * audio_data, int data_len) {
    //printf("data len: %d, step:%d\n", (int) data_len, n_samples_step);
    if (data_len > 2*n_samples_step) {
        fprintf(stderr, "\n\n%s: WARNING: cannot process audio fast enough, dropping audio ...\n\n", __func__);
        return;
    }

    int n_samples = data_len/sizeof(int16_t);
    std::vector<int16_t> intArr(n_samples);
    memcpy(intArr.data(), audio_data, data_len);

    std::vector<float> floatArr(n_samples);
    for (uint64_t i = 0; i < n_samples; i++) {
        floatArr[i] = float(intArr[i])/32768.0f;
    }

    //trans(floatArr);


    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (audio_pos + n_samples > audio.size()) {
            const size_t n0 = audio.size() - audio_pos;

            memcpy(&audio[audio_pos], floatArr.data(), n0 * sizeof(float));
            memcpy(&audio[0], &floatArr[n0], (n_samples - n0) * sizeof(float));

            audio_pos = (audio_pos + n_samples) % audio.size();
            audio_len = audio.size();
        } else {
            memcpy(&audio[audio_pos], floatArr.data(), n_samples * sizeof(float));

            audio_pos = (audio_pos + n_samples) % audio.size();
            audio_len = std::min(size_t(audio_len + n_samples), audio.size());
        }
    }
}


void OnAudioData(const char * stream_id_or_room_id, unsigned char * audio_data, int data_len, int sample_rate, int channels, void * user_data)
{
    printf("OnAudioData, stream id = %s, len = %d, user data = %s, sample_rate = %d \n", stream_id_or_room_id, data_len, (char*)user_data, sample_rate);
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
    //printf("OnPerAudioData, room id = %s, streamid = %s, len = %d, sample_rate = %d, timstamp = %lld, channel = %d, user data = %s \n", room_id, stream_id, data_len, sample_rate, timestamp, channels, (char*)user_data);
    /*if (pcm == nullptr) {
        pcm = fopen("test.wav", "wb+");
    } else {
        fwrite(audio_data, 1, data_len, pcm);
    }*/

    trans(audio_data, data_len);

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

void whisper_print_usage(int argc, char ** argv, const whisper_params & params);

bool whisper_params_parse(int argc, char ** argv, whisper_params & params) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            whisper_print_usage(argc, argv, params);
            exit(0);
        }
        else if (arg == "-t"   || arg == "--threads")       { params.n_threads     = std::stoi(argv[++i]); }
        else if (                 arg == "--step")          { params.step_ms       = std::stoi(argv[++i]); }
        else if (                 arg == "--length")        { params.length_ms     = std::stoi(argv[++i]); }
        else if (                 arg == "--keep")          { params.keep_ms       = std::stoi(argv[++i]); }
        else if (arg == "-c"   || arg == "--capture")       { params.capture_id    = std::stoi(argv[++i]); }
        else if (arg == "-mt"  || arg == "--max-tokens")    { params.max_tokens    = std::stoi(argv[++i]); }
        else if (arg == "-ac"  || arg == "--audio-ctx")     { params.audio_ctx     = std::stoi(argv[++i]); }
        else if (arg == "-vth" || arg == "--vad-thold")     { params.vad_thold     = std::stof(argv[++i]); }
        else if (arg == "-fth" || arg == "--freq-thold")    { params.freq_thold    = std::stof(argv[++i]); }
        else if (arg == "-su"  || arg == "--speed-up")      { params.speed_up      = true; }
        else if (arg == "-tr"  || arg == "--translate")     { params.translate     = true; }
        else if (arg == "-nf"  || arg == "--no-fallback")   { params.no_fallback   = true; }
        else if (arg == "-ps"  || arg == "--print-special") { params.print_special = true; }
        else if (arg == "-kc"  || arg == "--keep-context")  { params.no_context    = false; }
        else if (arg == "-l"   || arg == "--language")      { params.language      = argv[++i]; }
        else if (arg == "-m"   || arg == "--model")         { params.model         = argv[++i]; }
        else if (arg == "-f"   || arg == "--file")          { params.fname_out     = argv[++i]; }
        else {
            fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
            whisper_print_usage(argc, argv, params);
            exit(0);
        }
    }

    return true;
}

void whisper_print_usage(int /*argc*/, char ** argv, const whisper_params & params) {
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [options]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "  -h,       --help          [default] show this help message and exit\n");
    fprintf(stderr, "  -t N,     --threads N     [%-7d] number of threads to use during computation\n",    params.n_threads);
    fprintf(stderr, "            --step N        [%-7d] audio step size in milliseconds\n",                params.step_ms);
    fprintf(stderr, "            --length N      [%-7d] audio length in milliseconds\n",                   params.length_ms);
    fprintf(stderr, "            --keep N        [%-7d] audio to keep from previous step in ms\n",         params.keep_ms);
    fprintf(stderr, "  -c ID,    --capture ID    [%-7d] capture device ID\n",                              params.capture_id);
    fprintf(stderr, "  -mt N,    --max-tokens N  [%-7d] maximum number of tokens per audio chunk\n",       params.max_tokens);
    fprintf(stderr, "  -ac N,    --audio-ctx N   [%-7d] audio context size (0 - all)\n",                   params.audio_ctx);
    fprintf(stderr, "  -vth N,   --vad-thold N   [%-7.2f] voice activity detection threshold\n",           params.vad_thold);
    fprintf(stderr, "  -fth N,   --freq-thold N  [%-7.2f] high-pass frequency cutoff\n",                   params.freq_thold);
    fprintf(stderr, "  -su,      --speed-up      [%-7s] speed up audio by x2 (reduced accuracy)\n",        params.speed_up ? "true" : "false");
    fprintf(stderr, "  -tr,      --translate     [%-7s] translate from source language to english\n",      params.translate ? "true" : "false");
    fprintf(stderr, "  -nf,      --no-fallback   [%-7s] do not use temperature fallback while decoding\n", params.no_fallback ? "true" : "false");
    fprintf(stderr, "  -ps,      --print-special [%-7s] print special tokens\n",                           params.print_special ? "true" : "false");
    fprintf(stderr, "  -kc,      --keep-context  [%-7s] keep context between audio chunks\n",              params.no_context ? "false" : "true");
    fprintf(stderr, "  -l LANG,  --language LANG [%-7s] spoken language\n",                                params.language.c_str());
    fprintf(stderr, "  -m FNAME, --model FNAME   [%-7s] model path\n",                                     params.model.c_str());
    fprintf(stderr, "  -f FNAME, --file FNAME    [%-7s] text output file name\n",                          params.fname_out.c_str());
    fprintf(stderr, "\n");
}

int main(int argc, char ** argv) {
    if (whisper_params_parse(argc, argv, params) == false) {
        return 1;
    }

    params.keep_ms   = std::min(params.keep_ms,   params.step_ms);
    params.length_ms = std::max(params.length_ms, params.step_ms);

    n_samples_step = (1e-3*params.step_ms  )*WHISPER_SAMPLE_RATE;
    n_samples_len  = (1e-3*params.length_ms)*WHISPER_SAMPLE_RATE;
    n_samples_keep = (1e-3*params.keep_ms  )*WHISPER_SAMPLE_RATE;
    n_samples_30s  = (1e-3*30000.0         )*WHISPER_SAMPLE_RATE;

    use_vad = n_samples_step <= 0; // sliding window mode uses VAD

    n_new_line = !use_vad ? std::max(1, params.length_ms / params.step_ms - 1) : 1; // number of steps to print new line

    params.no_timestamps  = !use_vad;
    params.no_context    |= use_vad;
    params.max_tokens     = 0;

    // init sdk
    // 设置使用测试环境,true-表示使用zego测试环境，false-表示使用zego正式环境
    zego_api_set_use_test_env(false);

    // 设置获取音频数据状态回调
    zego_api_set_play_state_update_callback(OnGetAudioStateUpdate, nullptr);

    // 设置连接状态回调
    zego_api_set_connection_state_update_callback(OnConnectionUpdate, nullptr);

    // 开启拉流vad检测,拉流检测到静音数据时不会在通过OnAudioData回调出来
    zego_api_enable_vad(false); 

    // 设置 SDK 参数
    zego_sdk_config config;
    config.audio_sample_rate = ZEGO_AUDIO_SAMPLE_RATE_16K;  //设置拉流音频采样率
    zego_api_set_config(config);
    

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

    // 设置静默时间 20s
    int idle_time = 20000;
    zego_api_set_idle_time(idle_time);

    zego_api_login_by_token(token_.c_str(), nullptr, OnLogin);

    audio.resize((16000*params.length_ms)/1000);
    std::thread threadTrans(transInOtherThread);

    // whisper init

    if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1){
        fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
        whisper_print_usage(argc, argv, params);
        exit(0);
    }

    ctx = whisper_init_from_file(params.model.c_str());

    // print some info about the processing
    {
        fprintf(stderr, "\n");
        if (!whisper_is_multilingual(ctx)) {
            if (params.language != "en" || params.translate) {
                params.language = "en";
                params.translate = false;
                fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n", __func__);
            }
        }
        fprintf(stderr, "%s: processing %d samples (step = %.1f sec / len = %.1f sec / keep = %.1f sec), %d threads, lang = %s, task = %s, timestamps = %d ...\n",
                __func__,
                n_samples_step,
                float(n_samples_step)/WHISPER_SAMPLE_RATE,
                float(n_samples_len )/WHISPER_SAMPLE_RATE,
                float(n_samples_keep)/WHISPER_SAMPLE_RATE,
                params.n_threads,
                params.language.c_str(),
                params.translate ? "translate" : "transcribe",
                params.no_timestamps ? 0 : 1);

        if (!use_vad) {
            fprintf(stderr, "%s: n_new_line = %d, no_context = %d\n", __func__, n_new_line, params.no_context);
        } else {
            fprintf(stderr, "%s: using VAD, will transcribe on speech activity\n", __func__);
        }

        fprintf(stderr, "\n");
    }

    bool is_running = true;

    if (params.fname_out.length() > 0) {
        fout.open(params.fname_out);
        if (!fout.is_open()) {
            fprintf(stderr, "%s: failed to open output file '%s'!\n", __func__, params.fname_out.c_str());
            return 1;
        }
    }

    printf("[Start speaking]");
    fflush(stdout);

    auto t_last  = std::chrono::high_resolution_clock::now();
    const auto t_start = t_last;


    /*std::vector<float> tpcmf32;               // mono-channel F32 PCM
    std::vector<std::vector<float>> tpcmf32s; // stereo-channel F32 PCM

    if (!::read_wav("./samples/jfk.wav", tpcmf32, tpcmf32s, false)) {
        fprintf(stderr, "error: failed to read WAV file");
        return -1;
    }

    trans(tpcmf32);
    pcmf32_new.clear();*/

    // main loop
    while (is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    threadTrans.join();
    whisper_print_timings(ctx);
    whisper_free(ctx);

    return 0;
}
