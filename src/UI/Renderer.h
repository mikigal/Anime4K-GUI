#ifndef RENDERER_H
#define RENDERER_H
#include "Data/Encoder.h"
#include "Data/Resolution.h"
#include "Data/Shader.h"

namespace Upscaler {
    class App;

    inline constexpr const char* ShadersTooltip =
        "Check the project's GitHub page if you're not sure what to choose";

    inline constexpr const char* EncoderTooltip =
        "Codec for encoding output file. In most cases GPU based are faster, use CPU mainly if you have slow GPU\n"
        "GPU based AV1 is compatible only with RTX 4000+ and RX 7000+\n"
        "HDR videos are supported only by AV1 codec\n"
        "FFV1 is lossless codec and provides best output quality, but output files are pretty big";

    inline constexpr const char* CrfTooltip =
        "Constant Rate Factor (CRF) for CPU based encoders. \nLower value = better image quality, but larger files."
        "\n\nValid range: 0 - 51 \n0 = lossless, 18–23 = good quality, 28+ = lower quality."
        "\n\nIf you're not sure what to enter, try 18 (default).";

    inline constexpr const char* CqTooltip =
        "Constant Quality (CQ) parameter for encoder. \nLower values mean better quality but larger files."
        "\n\nValid range: 0 - 51 \n0 = lossless, 18 = visually near-lossless, 28+ = noticeable compression."
        "\n\nIf you're not sure what to enter, try 18 (default)";

    inline constexpr const char* OutputFormatTooltip =
        "If your input file contains subtitles streams you must use MKV as output format due to other formats limitations";

    inline constexpr const char* CpuThreadsTooltip =
        "How many of CPU threads FFMPEG will use \nYou may limit it to make your system more responsive wile using CPU based encoder";

    inline constexpr const char* DebugModeTooltip =
        "Show more detailed logs, useful for troubleshooting and debugging";

    inline constexpr const char* ConcurrentJobsTooltip =
        "Number of simultaneous video upscaling jobs to run.\n"
        "1 - Default/Safe\n"
        "2 - Recommended for modern GPUs (e.g. RTX 3080/4080/5080)\n"
        "3/4 - High-end Only (RTX 4090/5090)\n"
        "Note: Anime4K is compute-heavy. Increasing concurrent jobs beyond 2 may reduce total speed on most cards.";

    class Renderer {
    public:
        ImFont* m_Font;
        App* Instance;

        // UI pointers
        int SelectedResolution = 0;
        int SelectedShader = 0;
        int SelectedEncoder = 0;
        int SelectedCrf = 18;
        int SelectedCq = 18;
        int SelectedCpuThreads = std::thread::hardware_concurrency();
        int SelectedOutputFormat = 0;
        int SelectedConcurrentJobs = 1;
        bool SelectedDebugMode = false;

        static std::string Logs;

        bool CriticalError = false;

        bool Init();
        void Terminate();
        Resolution& GetSelectedResolution();
        Encoder& GetSelectedEncoder();
        Shader& GetSelectedShader();
        std::string& GetSelectedOutputFormat();

        Renderer(App* instance)
            : Instance(instance) {
        }

    private:
        GLFWwindow* m_Window = nullptr;
        std::vector<const char*> m_ShadersNames;
        std::vector<const char*> m_ResolutionsNames;
        std::vector<const char*> m_EncodersNames;
        std::vector<const char*> m_OutputFormatsNames;

        static std::vector<std::string> m_DroppedFiles;

        void RenderUI();
        void RenderVideoTable();
        void RenderSettings();
        void RenderLogs();
        void RenderProgress(float);

        void InitializeWindow();
        void ApplyStyle();
        static void DropCallback(GLFWwindow*, int, const char**);
    };
} // Upscaler

#endif //RENDERER_H
