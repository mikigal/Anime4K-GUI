#ifndef UPSCALING_STATUS_H
#define UPSCALING_STATUS_H

namespace Upscaler {
    enum UpscalingStatus {
        STATUS_NOT_STARTED,
        STATUS_WAITING,
        STATUS_PROCESSING,
        STATUS_FAILED,
        STATUS_FINISHED,
        STATUS_CANCELLED
    };
}



#endif // UPSCALING_STATUS_H