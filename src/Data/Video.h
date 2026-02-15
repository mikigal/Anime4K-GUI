#ifndef ANIME_H
#define ANIME_H

#include <atomic>
#include <utility>
#include <pch.h>

namespace Upscaler {
    class Video {
    public:
        std::string Name;
        int Duration;
        uintmax_t Size;
        int Width;
        int Height;
        int FrameRate;
        int TotalFrames;
        bool HasSubtitlesStream;
        std::vector<std::string> StreamCodecs;
        std::string PixelFormat;
        std::string Path;
        std::atomic<int> Status;
        std::atomic<float> Progress;
        std::atomic<int> Eta;
        std::atomic<float> Speed;

        Video(std::string name, int duration, uintmax_t size, int width, int height, int frameRate,
              int totalFrames, bool hasSubtitlesStream, std::vector<std::string> streamCodecs,
              std::string pixelFormat, std::string path, int status, float progress, int eta, float speed)
            : Name(std::move(name)), Duration(duration), Size(size), Width(width), Height(height),
              FrameRate(frameRate), TotalFrames(totalFrames), HasSubtitlesStream(hasSubtitlesStream),
              StreamCodecs(std::move(streamCodecs)), PixelFormat(std::move(pixelFormat)),
              Path(std::move(path)), Status(status), Progress(progress), Eta(eta), Speed(speed) {}

        Video(const Video& other)
            : Name(other.Name), Duration(other.Duration), Size(other.Size), Width(other.Width),
              Height(other.Height), FrameRate(other.FrameRate), TotalFrames(other.TotalFrames),
              HasSubtitlesStream(other.HasSubtitlesStream), StreamCodecs(other.StreamCodecs),
              PixelFormat(other.PixelFormat), Path(other.Path), Status(other.Status.load()),
              Progress(other.Progress.load()), Eta(other.Eta.load()), Speed(other.Speed.load()) {}

        Video& operator=(const Video& other) {
            if (this != &other) {
                Name = other.Name;
                Duration = other.Duration;
                Size = other.Size;
                Width = other.Width;
                Height = other.Height;
                FrameRate = other.FrameRate;
                TotalFrames = other.TotalFrames;
                HasSubtitlesStream = other.HasSubtitlesStream;
                StreamCodecs = other.StreamCodecs;
                PixelFormat = other.PixelFormat;
                Path = other.Path;
                Status.store(other.Status.load());
                Progress.store(other.Progress.load());
                Eta.store(other.Eta.load());
                Speed.store(other.Speed.load());
            }
            return *this;
        }

        Video(Video&& other) noexcept
            : Name(std::move(other.Name)), Duration(other.Duration), Size(other.Size), Width(other.Width),
              Height(other.Height), FrameRate(other.FrameRate), TotalFrames(other.TotalFrames),
              HasSubtitlesStream(other.HasSubtitlesStream), StreamCodecs(std::move(other.StreamCodecs)),
              PixelFormat(std::move(other.PixelFormat)), Path(std::move(other.Path)),
              Status(other.Status.load()), Progress(other.Progress.load()), Eta(other.Eta.load()),
              Speed(other.Speed.load()) {}

        Video& operator=(Video&& other) noexcept {
            if (this != &other) {
                Name = std::move(other.Name);
                Duration = other.Duration;
                Size = other.Size;
                Width = other.Width;
                Height = other.Height;
                FrameRate = other.FrameRate;
                TotalFrames = other.TotalFrames;
                HasSubtitlesStream = other.HasSubtitlesStream;
                StreamCodecs = std::move(other.StreamCodecs);
                PixelFormat = std::move(other.PixelFormat);
                Path = std::move(other.Path);
                Status.store(other.Status.load());
                Progress.store(other.Progress.load());
                Eta.store(other.Eta.load());
                Speed.store(other.Speed.load());
            }
            return *this;
        }
    };
}

#endif //ANIME_H
