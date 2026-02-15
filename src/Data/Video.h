#ifndef ANIME_H
#define ANIME_H

#include <atomic>
#include <utility>
#include <pch.h>

namespace Upscaler {
    class Video {
    private:
        std::string m_Name;
        int m_Duration;
        uintmax_t m_Size;
        int m_Width;
        int m_Height;
        int m_FrameRate;
        int m_TotalFrames;
        bool m_HasSubtitlesStream;
        std::vector<std::string> m_StreamCodecs;
        std::string m_PixelFormat;
        std::string m_Path;
        std::atomic<int> m_Status;
        std::atomic<float> m_Progress;
        std::atomic<int> m_Eta;
        std::atomic<float> m_Speed;
        TinyProcessLib::Process* m_UpscalingProcess;

    public:
        Video(std::string name, int duration, uintmax_t size, int width, int height, int frameRate,
              int totalFrames, bool hasSubtitlesStream, std::vector<std::string> streamCodecs,
              std::string pixelFormat, std::string path, int status, float progress, int eta, float speed, TinyProcessLib::Process* upscalingProcess)
            : m_Name(std::move(name)), m_Duration(duration), m_Size(size), m_Width(width), m_Height(height),
              m_FrameRate(frameRate), m_TotalFrames(totalFrames), m_HasSubtitlesStream(hasSubtitlesStream),
              m_StreamCodecs(std::move(streamCodecs)), m_PixelFormat(std::move(pixelFormat)),
              m_Path(std::move(path)), m_Status(status), m_Progress(progress), m_Eta(eta), m_Speed(speed), m_UpscalingProcess(upscalingProcess) {}

        Video(const Video& other)
            : m_Name(other.GetName()), m_Duration(other.GetDuration()), m_Size(other.GetSize()), m_Width(other.GetWidth()),
              m_Height(other.GetHeight()), m_FrameRate(other.GetFrameRate()), m_TotalFrames(other.GetTotalFrames()),
              m_HasSubtitlesStream(other.HasSubtitlesSteam()), m_StreamCodecs(other.GetStreamCodecs()),
              m_PixelFormat(other.GetPixelFormat()), m_Path(other.GetPath()), m_Status(other.GetStatus()),
              m_Progress(other.GetProgress()), m_Eta(other.GetEta()), m_Speed(other.GetSpeed()), m_UpscalingProcess(other.GetUpscalingProcess()) {}

        Video& operator=(const Video& other) {
            if (this != &other) {
                m_Name = other.GetName();
                m_Duration = other.GetDuration();
                m_Size = other.GetSize();
                m_Width = other.GetWidth();
                m_Height = other.GetHeight();
                m_FrameRate = other.GetFrameRate();
                m_TotalFrames = other.GetTotalFrames();
                m_HasSubtitlesStream = other.HasSubtitlesSteam();
                m_StreamCodecs = other.GetStreamCodecs();
                m_PixelFormat = other.GetPixelFormat();
                m_Path = other.GetPath();
                m_Status.store(other.GetStatus());
                m_Progress.store(other.GetProgress());
                m_Eta.store(other.GetEta());
                m_Speed.store(other.GetSpeed());
                m_UpscalingProcess = other.GetUpscalingProcess();
            }
            return *this;
        }

        Video(const Video&& other) noexcept
            : m_Name(std::move(other.GetName())), m_Duration(other.GetDuration()), m_Size(other.GetSize()), m_Width(other.GetWidth()),
              m_Height(other.GetHeight()), m_FrameRate(other.GetFrameRate()), m_TotalFrames(other.GetTotalFrames()),
              m_HasSubtitlesStream(other.HasSubtitlesSteam()), m_StreamCodecs(std::move(other.GetStreamCodecs())),
              m_PixelFormat(std::move(other.GetPixelFormat())), m_Path(std::move(other.GetPath())),
              m_Status(other.GetStatus()), m_Progress(other.GetProgress()), m_Eta(other.GetEta()),
              m_Speed(other.GetSpeed()), m_UpscalingProcess(other.GetUpscalingProcess()) {}

        Video& operator=(const Video&& other) noexcept {
            if (this != &other) {
                m_Name = std::move(other.GetName());
                m_Duration = other.GetDuration();
                m_Size = other.GetSize();
                m_Width = other.GetWidth();
                m_Height = other.GetHeight();
                m_FrameRate = other.GetFrameRate();
                m_TotalFrames = other.GetTotalFrames();
                m_HasSubtitlesStream = other.HasSubtitlesSteam();
                m_StreamCodecs = std::move(other.GetStreamCodecs());
                m_PixelFormat = std::move(other.GetPixelFormat());
                m_Path = std::move(other.GetPath());
                m_Status.store(other.GetStatus());
                m_Progress.store(other.GetProgress());
                m_Eta.store(other.GetEta());
                m_Speed.store(other.GetSpeed());
                m_UpscalingProcess = other.GetUpscalingProcess();
            }
            return *this;
        }

        [[nodiscard]] std::string GetName() const {
            return m_Name;
        }

        [[nodiscard]] int GetDuration() const {
            return m_Duration;
        }

        [[nodiscard]] uintmax_t GetSize() const {
            return m_Size;
        }

        [[nodiscard]] int GetWidth() const {
            return m_Width;
        }

        [[nodiscard]] int GetHeight() const {
            return m_Height;
        }

        [[nodiscard]] int GetFrameRate() const {
            return m_FrameRate;
        }

        [[nodiscard]] int GetTotalFrames() const {
            return m_TotalFrames;
        }

        [[nodiscard]] bool HasSubtitlesSteam() const {
            return m_HasSubtitlesStream;
        }

        [[nodiscard]] std::vector<std::string>& GetStreamCodecs() {
            return m_StreamCodecs;
        }

        [[nodiscard]] const std::vector<std::string>& GetStreamCodecs() const {
            return m_StreamCodecs;
        }

        [[nodiscard]] std::string GetPixelFormat() const {
            return m_PixelFormat;
        }

        [[nodiscard]] std::string GetPath() const {
            return m_Path;
        }

        [[nodiscard]] int GetStatus() const {
            return m_Status.load();
        }

        [[nodiscard]] float GetProgress() const {
            return m_Progress.load();
        }

        [[nodiscard]] int GetEta() const {
            return m_Eta.load();
        }

        [[nodiscard]] float GetSpeed() const {
            return m_Speed.load();
        }

        [[nodiscard]] TinyProcessLib::Process* GetUpscalingProcess() const {
            return m_UpscalingProcess;
        }

        void SetStatus(int status) {
            m_Status.store(status);
        }

        void SetProgress(float progress) {
            m_Progress.store(progress);
        }

        void SetEta(int eta) {
            m_Eta.store(eta);
        }

        void SetSpeed(float speed) {
            m_Speed.store(speed);
        }

        void SetUpscalingProcess(TinyProcessLib::Process* upscalingProcess) {
            m_UpscalingProcess = upscalingProcess;
        }
    };
}

#endif //ANIME_H
