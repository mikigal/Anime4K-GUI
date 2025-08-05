#ifndef ANIME_H
#define ANIME_H
#include <pch.h>

namespace Upscaler {
    enum class ProcessingStatus {
        WAITING,
        PROCESSING,
        FINISHED,
        ERROR,
        NOT_STARTED
    };

    class Video {
    public:
        std::string m_Name;
        int m_Duration;
        long m_Size;
        int m_Width;
        int m_Height;
        int m_FrameRate;
        int m_TotalFrames;
        bool m_HasSubtitlesStream;
        std::vector<std::string> m_StreamCodecs;
        std::string m_PixelFormat;
        std::string m_Path;
        ProcessingStatus m_Status;

        Video(const std::string &m_name, int m_duration, long m_size, int m_width, int m_height, int m_frame_rate,
              int m_total_frames, bool m_has_subtitles_stream, std::vector<std::string> &m_streams_codecs,
              const std::string &m_pixel_format, const std::string& m_path, ProcessingStatus m_status)
            : m_Name(m_name),
              m_Duration(m_duration),
              m_Size(m_size),
              m_Width(m_width),
              m_Height(m_height),
              m_FrameRate(m_frame_rate),
              m_TotalFrames(m_total_frames),
              m_HasSubtitlesStream(m_has_subtitles_stream),
              m_StreamCodecs(m_streams_codecs),
              m_PixelFormat(m_pixel_format),
              m_Path(m_path),
              m_Status(m_status) {
        }

        [[nodiscard]] std::string ToString() const {
            std::ostringstream oss;
            oss << "Name: " << m_Name << "\n";
            oss << "Duration: " << m_Duration << " sec\n";
            oss << "Size: " << m_Size << " bytes\n";
            oss << "Resolution: " << m_Width << "x" << m_Height << "\n";
            oss << "FrameRate: " << m_FrameRate << " fps\n";
            oss << "TotalFrames: " << m_TotalFrames << "\n";
            oss << "HasSubtitlesStream: " << (m_HasSubtitlesStream ? "Yes" : "No") << "\n";
            oss << "StreamCodecs: ";
            for (const auto& codec : m_StreamCodecs) {
                oss << codec << " ";
            }
            oss << "\n";
            oss << "PixelFormat: " << m_PixelFormat << "\n";
            oss << "Path: " << m_Path << "\n";
            return oss.str();
        }
    };


}

#endif //ANIME_H
