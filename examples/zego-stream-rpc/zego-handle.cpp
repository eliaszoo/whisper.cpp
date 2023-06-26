#define ZEGO_DISABLE_SWTICH_THREAD

#include "ZegoExpressSDK.h"

namespace zego {

using namespace ZEGO::EXPRESS;

class Broadcaster : public std::enable_shared_from_this<Broadcaster>,
                    public IZegoEventHandler {
  public:
    Broadcaster(std::shared_ptr<Options> options);

    void Start() override;
    void Stop() override;

  private:
    void PrepareAudioDevice();
    void PrepareVideoDevice();

  protected:
    virtual void onRoomStateChanged(const std::string & /*roomID*/,
                                    ZegoRoomStateChangedReason /*reason*/, int /*errorCode*/,
                                    const std::string & /*extendedData*/) override;
    virtual void onPublisherStateUpdate(const std::string & /*streamID*/,
                                        ZegoPublisherState /*state*/, int /*errorCode*/,
                                        const std::string & /*extendedData*/) override;
    virtual void onPublisherQualityUpdate(const std::string & /*streamID*/,
                                          const ZegoPublishStreamQuality & /*quality*/) override;

  private:
    IZegoExpressEngine *engine = nullptr;
    std::shared_ptr<Options> options;
};

} // namespace zego