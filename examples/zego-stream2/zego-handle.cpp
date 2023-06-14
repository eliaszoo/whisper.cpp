#include "zego-handle.h"

using namespace zego;

namespace zego {

Player::Player(std::shared_ptr<Options> options) { this->options = options; }

void Player::Start() {
    printf("Player::Start()\n");

    std::string sdkVersion = ZegoExpressSDK::getVersion();
    printf("[*] RTC SDK Version: %s\n", sdkVersion.c_str());

    if (options->log_path.length() > 0) {
        ZegoLogConfig log_config;
        log_config.logPath = options->log_path;

        ZegoEngineConfig engine_config;
        engine_config.logConfig = &log_config;
        ZegoExpressSDK::setEngineConfig(engine_config);
    }

    ZegoEngineProfile profile;
    profile.appID = options->app_id;
    profile.appSign = options->app_sign;

    // Here we use the broadcast scenario as an example,
    // you should choose the appropriate scenario according to your actual situation,
    // for the differences between scenarios and how to choose a suitable scenario,
    // please refer to https://docs.zegocloud.com/article/14940
    profile.scenario = ZegoScenario::ZEGO_SCENARIO_BROADCAST;

    engine = ZegoExpressSDK::createEngine(profile, shared_from_this());
    if (!engine) {
        printf("[!] Create RTC engine failed!\n");
        return;
    } else {
        printf("[*] RTC Engine created!\n");
    }

    ZegoCustomAudioProcessConfig audio_config;
    audio_config.sampleRate = ZegoAudioSampleRate::ZEGO_AUDIO_SAMPLE_RATE_16K;
    audio_config.channel = ZegoAudioChannel::ZEGO_AUDIO_CHANNEL_UNKNOWN;
    audio_config.samples = 0;
    engine->enableCustomAudioRemoteProcessing(true, &audio_config);

    ZegoRoomConfig room_config;
    room_config.isUserStatusNotify = true;
    engine->loginRoom(options->room_id, ZegoUser(options->user_id, options->user_id), room_config);
    printf("[*] Login room: '%s'\n", options->room_id.c_str());
}

void Player::Stop() {
    printf("Player::Stop()\n");
    if (engine != nullptr) {

        engine->logoutRoom();

        ZegoExpressSDK::destroyEngine(engine, nullptr);
        engine = nullptr;
    }
}

void Player::onRoomStateChanged(const std::string &roomID, ZegoRoomStateChangedReason reason,
                                     int errorCode, const std::string &extendedData) {
    printf("onRoomStateChanged, roomId:%s, reason:%d, errorCode:%d\n", roomID.c_str(), reason,
           errorCode);

    if (errorCode != 0) {
        return;
    }

    if (reason == ZegoRoomStateChangedReason::ZEGO_ROOM_STATE_CHANGED_REASON_LOGINED) {
        std::thread t([this]() { this->engine->startPublishingStream(this->options->stream_id); });
        t.detach();
    }
}

void Player::onPublisherStateUpdate(const std::string &streamID, ZegoPublisherState state,
                                         int errorCode, const std::string &extendedData) {
    printf("onPublisherStateUpdate, streamId:%s, state:%d, errorCcode:%d\n", streamID.c_str(),
           state, errorCode);
}

void Player::onPublisherQualityUpdate(const std::string &streamID,
                                           const ZegoPublishStreamQuality &quality) {
    printf(
        "onPublisherQualityUpdate, streamId:%s, encodeFps:%.2f, sendFps:%.2f, kbps:%.2f, rtt:%d\n",
        streamID.c_str(), quality.videoEncodeFPS, quality.videoSendFPS, quality.videoKBPS,
        quality.rtt);
}

void Player::onProcessRemoteAudioData(unsigned char * data, unsigned int dataLength,
                                          ZegoAudioFrameParam * param,
                                          const std::string & streamID, double timestamp) {
    printf("onProcessRemoteAudioData, streamId:%s, dataLength:%d, param:%p, timestamp:%f\n", 
           streamID.c_str(), dataLength, param, timestamp);

    trans(data, dataLength);
}

void Player::onRoomStreamUpdate(const std::string & roomID,
                                    ZegoUpdateType updateType,
                                    const std::vector<ZegoStream> & streamList, const std::string & extendedData) {
    printf("onRoomStreamUpdate, roomId:%s, updateType:%d, streamList.size:%d\n", 
           roomID.c_str(), updateType, streamList.size());

    if (updateType == ZegoUpdateType::ZEGO_UPDATE_TYPE_ADD) {
        for (auto stream : streamList) {
            engine->startPlayingStream(stream.streamID);
        }
    }

}

} // namespace zego
