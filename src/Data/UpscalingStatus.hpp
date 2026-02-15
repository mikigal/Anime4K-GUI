#ifndef ANIME4K_GUI_UPSCALINGSTATUS_H
#define ANIME4K_GUI_UPSCALINGSTATUS_H

namespace Upscaler {
    enum UpscalingStatus {
        STATUS_NOT_STARTED,
        STATUS_WAITING,
        STATUS_PROCESSING,
        STATUS_FAILED,
        STATUS_FINISHED
    };
}



#endif