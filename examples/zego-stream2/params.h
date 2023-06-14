#pragma once

#include "common.h"
#include "whisper.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <cstring>

struct Options {
    std::string example;    // 示例专题名称
    uint32_t app_id = 0;    // app id
    std::string app_sign;   // app sign
    std::string room_id;    // 房间 ID
    std::string user_id;    // 用户 ID
    std::string stream_id;  // 流 ID
    std::string media_path; // 推流用的媒体资源文件
    std::string log_path;   // 日志输出目录
};


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

    std::shared_ptr<Options> options = std::make_shared<Options>();
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

void trans(const unsigned char * audio_data, int data_len) {
    if (data_len > 2*n_samples_step) {
        fprintf(stderr, "\n\n%s: WARNING: cannot process audio fast enough, dropping audio ...\n\n", __func__);
        return;
    }

    float* floatArr = (float*) audio_data;
    std::vector<float> floatVec(floatArr, floatArr + data_len / sizeof(float));
    pcmf32_new.assign(floatVec.begin(), floatVec.end());

    if (pcmf32_new.size() < n_samples_step) {
        return;
    }

    const int n_samples_new = pcmf32_new.size();

    // take up to params.length_ms audio from previous iteration
    const int n_samples_take = std::min((int) pcmf32_old.size(), std::max(0, n_samples_keep + n_samples_len - n_samples_new));

    //printf("processing: take = %d, new = %d, old = %d\n", n_samples_take, n_samples_new, (int) pcmf32_old.size());

    pcmf32.resize(n_samples_new + n_samples_take);

    for (int i = 0; i < n_samples_take; i++) {
        pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
    }

    memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new*sizeof(float));

    pcmf32_old = pcmf32;
    pcmf32_new.clear();

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

