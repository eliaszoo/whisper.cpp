#define ZEGO_DISABLE_SWTICH_THREAD

#include "ZegoExpressSDK.h"
#include "ZegoExpressEventHandler.h"
#include "params.h"

namespace zego {

using namespace ZEGO::EXPRESS;

class Player : public std::enable_shared_from_this<Player>,
                    public IZegoEventHandler, public IZegoCustomAudioProcessHandler {
  public:
    Player(std::shared_ptr<Options> options);

    void Start();
    void Stop();

  private:

  protected:
    virtual void onRoomStateChanged(const std::string & /*roomID*/,
                                    ZegoRoomStateChangedReason /*reason*/, int /*errorCode*/,
                                    const std::string & /*extendedData*/) override;
    virtual void onPublisherStateUpdate(const std::string & /*streamID*/,
                                        ZegoPublisherState /*state*/, int /*errorCode*/,
                                        const std::string & /*extendedData*/) override;
    virtual void onPublisherQualityUpdate(const std::string & /*streamID*/,
                                          const ZegoPublishStreamQuality & /*quality*/) override;
    virtual void onProcessRemoteAudioData(unsigned char * /*data*/, unsigned int /*dataLength*/,
                                          ZegoAudioFrameParam * /*param*/,
                                          const std::string & /*streamID*/, double /*timestamp*/) override;
    virtual void onRoomStreamUpdate(const std::string & /*roomID*/, ZegoUpdateType /*updateType*/,
                                    const std::vector<ZegoStream> & /*streamList*/,
                                    const std::string & /*extendedData*/) override;

  private:
    IZegoExpressEngine *engine = nullptr;
    std::shared_ptr<Options> options;
};

} // namespace zego