#ifndef ANIME_H
#define ANIME_H
#include <pch.h>

class Anime {
public:
    std::string m_Name;
    long m_Length;
    long m_Size;
    int m_Width;
    int m_Height;
    int m_FrameRate;
    int m_TotalFrames;
    bool m_HasSubtitlesStream;
    std::string m_PixelFormat;
    std::string m_Path;

    Anime(const std::string& m_name, long m_length, long m_size, int m_width, int m_height, int m_frame_rate,
        int m_total_frames, bool m_has_subtitles_stream, const std::string& m_pixel_format, const std::string& m_path)
        : m_Name(m_name),
          m_Length(m_length),
          m_Size(m_size),
          m_Width(m_width),
          m_Height(m_height),
          m_FrameRate(m_frame_rate),
          m_TotalFrames(m_total_frames),
          m_HasSubtitlesStream(m_has_subtitles_stream),
          m_PixelFormat(m_pixel_format),
          m_Path(m_path) {
    }
};

enum ProcessingStatus {
    WAITING,
    PROCESSING,
    FINISHED,
    ERROR,
    NOT_STARTED
};

#endif //ANIME_H
