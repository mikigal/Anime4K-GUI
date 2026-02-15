#include "Renderer.h"
#include "pch.h"
#include "Data/Configuration.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"
#include "App.h"
#include "imgui_internal.h"
#include "Utilities/Utilities.h"
#include "RendererUtilities.h"

#ifdef _WIN32
#include "Utilities/WindowsUtilities.h"
#endif

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 740

namespace Upscaler {
    std::string Renderer::Logs;
    std::vector<std::string> Renderer::m_DroppedFiles;

    void Renderer::RenderUI() {
        for (std::string& path: m_DroppedFiles) {
            Instance->GetVideoLoader().LoadVideo(path);
        }
        m_DroppedFiles.clear();

        RenderVideoTable();
        RenderSettings();
        RenderLogs();
    }

    void Renderer::RenderVideoTable() {
        ImGui::Begin("Video List");
        ImGui::BeginChild("VideoDropTargetChild", ImVec2(0, 0), true, ImGuiWindowFlags_None);

        if (ImGui::BeginTable("VideoTable##Persistent", 10, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg |
                                                           ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 350.0f);
            ImGui::TableSetupColumn("Resolution");
            ImGui::TableSetupColumn("Length");
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Progress");
            ImGui::TableSetupColumn("ETA");
            ImGui::TableSetupColumn("Speed");
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            for (int i = 0; i < Instance->GetVideoLoader().m_Videos.size(); ++i) {
                Video& video = Instance->GetVideoLoader().m_Videos[i];
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%d", i + 1);
                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%s", video.Name.c_str());
                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%dx%d", video.Width, video.Height);
                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%s", Utilities::FormatTime(video.Duration).c_str());
                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%s", Utilities::ToMegabytes(video.Size).c_str());
                ImGui::TableNextColumn();
                RenderProgress(video.Progress.load());
                ImGui::TableNextColumn();
                int eta = video.Eta.load();
                RendererUtilities::CenteredText(eta == -1 ? "-" : Utilities::FormatTime(eta).c_str());
                ImGui::TableNextColumn();
                float speed = video.Speed.load();
                RendererUtilities::CenteredText(speed == -1.0f ? "-" : "%.2fx", speed);
                ImGui::TableNextColumn();
                RendererUtilities::CenteredText("%s", Utilities::FormatStatus(video.Status.load()).c_str());

                ImGui::TableNextColumn();
                ImGui::PushID(i);

                float btnWidth = ImGui::CalcTextSize("Remove").x + ImGui::GetStyle().FramePadding.x * 2.0f;
                ImVec2 cursorPos = ImGui::GetCursorPos();
                ImGui::SetCursorPosX(cursorPos.x + (ImGui::GetColumnWidth() - btnWidth) * 0.5f);
                ImGui::SetCursorPosY(
                    cursorPos.y + (ImGui::GetTextLineHeightWithSpacing() - ImGui::GetFrameHeight()) * 0.5f);

                if (video.Status == STATUS_PROCESSING) {
                    ImGui::BeginDisabled();
                }

                if (ImGui::Button("Remove")) {
                    Instance->GetLogger().Info("Removed file {}", video.Name);
                    Instance->GetVideoLoader().m_Videos.erase(Instance->GetVideoLoader().m_Videos.begin() + i);
                    i--;
                }

                if (video.Status == STATUS_PROCESSING) {
                    ImGui::EndDisabled();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        if (ImGui::BeginDragDropTarget()) {
            ImGui::EndDragDropTarget();
        }

        ImGui::EndChild();
        ImGui::End();
    }
    void Renderer::RenderSettings() {
        ImGui::Begin("Settings");
        RendererUtilities::ComboWithLabel("Target resolution", nullptr, "##resolution", &SelectedResolution, m_ResolutionsNames);
        RendererUtilities::ComboWithLabel("Shaders", ShadersTooltip, "##shaders", &SelectedShader, m_ShadersNames);
        RendererUtilities::ComboWithLabel("Encoder", EncoderTooltip, "##encoders", &SelectedEncoder, m_EncodersNames);

        Encoder selectedEncoder = GetSelectedEncoder();
        if (selectedEncoder.CrfSupport) {
            RendererUtilities::NumberInput("Constant Rate Factor (CRF)", CrfTooltip, "##crf", &SelectedCrf, 1, 51);
        }
        if (selectedEncoder.CqSupport) {
            RendererUtilities::NumberInput("Constant Quality (CQ)", CqTooltip, "##cq", &SelectedCq, 1, 51);
        }
        if (selectedEncoder.VideoToolboxCqSupport) {
            RendererUtilities::NumberInput("Constant Quality (CQ)", nullptr, "##cq", &SelectedCq, 1, 100);
        }
        if (selectedEncoder.ThreadsLimitSupported) {
            RendererUtilities::NumberInput("CPU threads", CpuThreadsTooltip, "##cpuThreads", &SelectedCpuThreads, 1, std::thread::hardware_concurrency());
        }

        // Not implemented yet
        // if (selectedEncoder.Vendor != "cpu") {
        //     RendererUtilities::NumberInput("Concurrent jobs", ConcurrentJobsTooltip, "##concurrentJobs", &SelectedConcurrentJobs, 1, 4);
        // }

        RendererUtilities::ComboWithLabel("Output formats", OutputFormatTooltip, "##output_formats", &SelectedOutputFormat, m_OutputFormatsNames);

        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", DebugModeTooltip);
        ImGui::Checkbox("Debug mode", &SelectedDebugMode);
        ImGui::Dummy(ImVec2(0, 10));

        if (CriticalError) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button(Instance->GetVideoProcessor().IsProcessing() ? "Cancel" : "Start", ImVec2(300, 30))) {
            Instance->GetVideoProcessor().HandleButton();
        }
        if (CriticalError) {
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("%s", "Can not start upscaling due to critical error. \nCcheck logs for more details");
            }
            ImGui::EndDisabled();
        }

        ImGui::End();
    }
    void Renderer::RenderLogs() {
        ImGui::Begin("Logs");
        std::string logsDisplayBuffer = Logs + '\0';
        ImGui::InputTextMultiline("##logs", logsDisplayBuffer.data(), logsDisplayBuffer.size(),
                                  ImVec2(-FLT_MIN, 220), ImGuiInputTextFlags_ReadOnly);
        ImGui::End();
    }
    void Renderer::RenderProgress(float progress) {
        float cellWidth = ImGui::GetContentRegionAvail().x;
        float height = 18.0f;

        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(120, 100, 255, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
        ImGui::ProgressBar(progress, ImVec2(cellWidth, height), "");

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        ImVec2 barMin = ImGui::GetItemRectMin();
        ImVec2 barMax = ImGui::GetItemRectMax();
        ImVec2 barSize(
            barMax.x - barMin.x,
            barMax.y - barMin.y
        );

        std::string percentText = progress == 1 ? "Done" : std::to_string((int)(progress * 100.0f)) + "%";
        ImVec2 textSize = ImGui::CalcTextSize(percentText.c_str());

        ImVec2 textPos;
        textPos.x = barMin.x + (barSize.x - textSize.x) * 0.5f;
        textPos.y = barMin.y + (barSize.y - textSize.y) * 0.5f;

        ImGui::GetWindowDrawList()->AddText(
            textPos,
            ImGui::GetColorU32(ImGuiCol_Text),
            percentText.c_str()
        );
    }

    bool Renderer::Init() {
        for (Shader& shader: Instance->GetConfiguration().Shaders) {
            m_ShadersNames.push_back(shader.Name.c_str());
        }

        for (Resolution& resolution: Instance->GetConfiguration().Resolutions) {
            m_ResolutionsNames.push_back(resolution.VisibleName.c_str());
        }

        for (Encoder& encoder: Instance->GetConfiguration().Encoders) {
            if (!encoder.Available) {
                continue;
            }

            m_EncodersNames.push_back(encoder.Name.c_str());
        }

        for (std::string& outputFormat: Instance->GetConfiguration().OutputFormats) {
            m_OutputFormatsNames.push_back(outputFormat.c_str());
        }

        InitializeWindow();

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "imgui.ini";
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);

        ApplyStyle();

        bool firstFrame = true;
        while (!glfwWindowShouldClose(m_Window)) {
            glfwPollEvents();

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
            if (firstFrame) {
                ImGuiID dockspace_id = ImGui::GetMainViewport()->ID;
                ImGui::DockBuilderRemoveNode(dockspace_id);
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

                ImGuiID mainDockId = dockspace_id;

                // Bottom dock (logs only now)
                ImGuiID logsDock;
                ImGui::DockBuilderSplitNode(mainDockId, ImGuiDir_Down, 0.33f, &logsDock, &mainDockId);

                // Right split (settings / video)
                ImGuiID settingsDock, videoDock;
                ImGui::DockBuilderSplitNode(mainDockId, ImGuiDir_Right, 0.2f, &settingsDock, &videoDock);

                // Flags
                ImGui::DockBuilderGetNode(videoDock)->LocalFlags |=
                    ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
                ImGui::DockBuilderGetNode(settingsDock)->LocalFlags |=
                    ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
                ImGui::DockBuilderGetNode(logsDock)->LocalFlags |=
                    ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;

                // Dock windows
                ImGui::DockBuilderDockWindow("Video List", videoDock);
                ImGui::DockBuilderDockWindow("Settings", settingsDock);
                ImGui::DockBuilderDockWindow("Logs", logsDock);

                ImGui::DockBuilderFinish(dockspace_id);
                firstFrame = false;
            }

            RenderUI();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(m_Window);
        }

        return true;
    }

    void Renderer::InitializeWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Anime4K-GUI",
                                              nullptr, nullptr);
        glfwSetDropCallback(window, DropCallback);

        if (window == nullptr) {
            Instance->GetLogger().Critical("Could not create GLFW window");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            Instance->GetLogger().Critical("Could not initialize GLAD");
            exit(EXIT_FAILURE);
        }

#ifdef _WIN32
        Windows::SetIcon(window);
        Windows::EnableWindowDarkMode(window);
#endif

        this->m_Window = window;
    }

    void Renderer::Terminate() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Renderer::DropCallback(GLFWwindow* window, int count, const char** paths) {
        for (int i = 0; i < count; ++i) {
            std::string path(paths[i]);
            std::string lower = path;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            if (lower.ends_with(".mp4") || lower.ends_with(".avi") || lower.ends_with(".mkv")) {
                m_DroppedFiles.push_back(path);
            }
        }
    }

    // Look by names as SelectedEncoder is index of currently available encoders
    Encoder& Renderer::GetSelectedEncoder() {
        for (Encoder& encoder : Instance->GetConfiguration().Encoders) {
            if (encoder.Name == m_EncodersNames[SelectedEncoder]) {
                return encoder;
            }
        }

        throw std::runtime_error("Encoder " + std::string(m_EncodersNames[SelectedEncoder]) + " does not exist");
    }

    Resolution& Renderer::GetSelectedResolution() {
        return Instance->GetConfiguration().Resolutions[SelectedResolution];
    }

    Shader& Renderer::GetSelectedShader() {
        return Instance->GetConfiguration().Shaders[SelectedShader];
    }

    std::string& Renderer::GetSelectedOutputFormat() {
        return Instance->GetConfiguration().OutputFormats[SelectedOutputFormat];
    }

    void Renderer::ApplyStyle() {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig config;
        config.OversampleH = 1;
        config.OversampleV = 1;
        config.FontDataOwnedByAtlas = false;
        config.PixelSnapH = true;

        AssetLoader::AssetData font = Instance->GetAssetLoader().GetFileData("OpenSans.ttf");
        m_Font = io.Fonts->AddFontFromMemoryTTF(font.data(), font.size(), 18, &config, nullptr);
        io.FontDefault = m_Font;
        io.Fonts->Build();

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);

        colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

        colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);

        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.38f, 0.50f, 1.00f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

        colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);

        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.25f, 0.20f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.20f, 0.25f, 0.30f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);

        style.WindowRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.GrabRounding = 5.0f;
        style.TabRounding = 5.0f;
        style.PopupRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(6, 4);
        style.ItemSpacing = ImVec2(8, 6);
        style.PopupBorderSize = 0.f;
        style.CellPadding = ImVec2(10, 4);
    }
} // Upscaler
