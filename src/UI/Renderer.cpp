#include "Renderer.h"
#include "pch.h"
#include "Data/Configuration.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"
#include "App.h"
#include "imgui_internal.h"
#include "Utilities/Utilities.h"

#ifdef _WIN32
#include "Utilities/WindowUtilities.h"
#endif

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 950

namespace Upscaler {
    std::string Renderer::Logs;



    std::vector<std::string> Renderer::m_DroppedFiles;

    void Renderer::RenderUI() {
        // ============ Table ============
        ImGui::Begin("Video List");

        ImGui::BeginChild("VideoDropTargetChild", ImVec2(0, 0), true, ImGuiWindowFlags_None);

        if (ImGui::BeginTable("VideoTable##Persistent", 7, ImGuiTableFlags_BordersOuter |
                                                           ImGuiTableFlags_RowBg |
                                                           ImGuiTableFlags_Resizable |
                                                           ImGuiTableFlags_Reorderable)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 700.0f);
            ImGui::TableSetupColumn("Resolution");
            ImGui::TableSetupColumn("Length");
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            for (int i = 0; i < Instance->GetVideoLoader().m_Videos.size(); ++i) {
                Video& video = Instance->GetVideoLoader().m_Videos[i];
                ImGui::TableNextRow();

                std::string idText = std::to_string(i + 1);
                float textWidth = ImGui::CalcTextSize(idText.c_str()).x;
                float columnWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", video.Name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%dx%d", video.Width, video.Height);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", Utilities::FormatTime(video.Duration).c_str());

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d MB", Utilities::ToMegabytes(video.Size));

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", video.Status.c_str());

                ImGui::TableSetColumnIndex(6);
                ImGui::PushID(i);

                float btnWidth = ImGui::CalcTextSize("Remove").x + ImGui::GetStyle().FramePadding.x * 2.0f;
                float colWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (colWidth - btnWidth) * 0.5f);

                float cellHeight = ImGui::GetTextLineHeightWithSpacing();
                float btnHeight = ImGui::GetFrameHeight();
                float offsetY = (cellHeight - btnHeight) * 0.5f;
                if (offsetY > 0.0f) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
                }

                if (ImGui::Button("Remove")) {
                    Instance->GetLogger().Info("Removed file {}", video.Name);
                    Instance->GetVideoLoader().m_Videos.erase(Instance->GetVideoLoader().m_Videos.begin() + i);
                    i--;
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

        for (std::string& path : m_DroppedFiles) {
            Instance->GetVideoLoader().LoadVideo(path);
        }
        m_DroppedFiles.clear();

        // ============ Settings ============
        ImGui::Begin("Settings");
        ImGui::Text("Target resolution");
        ImGui::SetNextItemWidth(300);
        ImGui::Combo("##res", &SelectedResolution, m_ResolutionsNames.data(), m_ResolutionsNames.size());
        ImGui::Spacing();

        ImGui::Text("Shaders");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Shader info here...");
        ImGui::SetNextItemWidth(300);
        ImGui::Combo("##shaders", &SelectedShader, m_ShadersNames.data(), m_ShadersNames.size());
        ImGui::Spacing();

        ImGui::Text("Encoder");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Encoder tooltip here...");
        ImGui::SetNextItemWidth(300);
        ImGui::Combo("##encoders", &SelectedEncoder, m_EncodersNames.data(), m_EncodersNames.size());
        ImGui::Spacing();

        if (SelectedEncoder == 1) {
            ImGui::Text("Constant Rate Factor (CRF)");
            ImGui::SetNextItemWidth(300);
            ImGui::InputInt("##crf", &SelectedCrf);
        }
        else {
            ImGui::Text("Constant Quality (CQ)");
            ImGui::SetNextItemWidth(300);
            ImGui::InputInt("##cq", &SelectedCq);
        }
        ImGui::Spacing();

        ImGui::Text("Output format");
        ImGui::SetNextItemWidth(300);
        static int format = 0;
        ImGui::Combo("##formats", &format, m_OutputFormatsNames.data(), m_OutputFormatsNames.size());
        ImGui::Spacing();

        if (SelectedEncoder == 1) {
            ImGui::Text("CPU threads");
            ImGui::SetNextItemWidth(300);
            ImGui::InputInt("##cpuThreads", &SelectedCpuThreads);
            ImGui::Spacing();
        }


        ImGui::Checkbox("Debug mode", &SelectedDebugMode);
        ImGui::Dummy(ImVec2(0, 10));

        if (ImGui::Button("Start Encoding", ImVec2(300, 30))) {
            // Start/cancel logic
        }
        ImGui::End();

        // ============ Logs ============
        ImGui::Begin("Logs");
        std::string logsDisplayBuffer = Logs + '\0';
        ImGui::InputTextMultiline("##logs", logsDisplayBuffer.data(), logsDisplayBuffer.size(),
                                  ImVec2(-FLT_MIN, 220), ImGuiInputTextFlags_ReadOnly);
        ImGui::End();

        // ============ Progress ============
        ImGui::Begin("Progress");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(120, 100, 255, 255));

        float progressHeight = 20.0f;
        float totalWidth = ImGui::GetContentRegionAvail().x;
        float progressBarWidth = totalWidth - 318.0f;

        ImVec2 startPos = ImGui::GetCursorScreenPos();
        float textLineHeight = ImGui::GetTextLineHeight();
        float centerOffset = (progressHeight - textLineHeight) * 0.5f;

        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + centerOffset));
        ImGui::Text("Progress: 0 / 1");
        ImGui::SameLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
        ImGui::SetCursorScreenPos(ImVec2(startPos.x + 110.0f, startPos.y));
        ImGui::PushItemWidth(progressBarWidth);
        ImGui::ProgressBar(Progress, ImVec2(progressBarWidth, progressHeight), "");
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();

        ImVec2 barPos = ImGui::GetItemRectMin();
        ImVec2 barSize = ImGui::GetItemRectSize();

        std::string percentText = std::to_string(int(Progress * 100)) + "%";
        ImVec2 percentSize = ImGui::CalcTextSize(percentText.c_str());

        float fillEndX = barPos.x + barSize.x * Progress;
        float margin = 4.0f;
        float textX = ImMin(fillEndX + margin, barPos.x + barSize.x - percentSize.x - margin);
        float textY = barPos.y + (barSize.y - percentSize.y) * 0.5f;

        ImGui::GetWindowDrawList()->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text),
                                            percentText.c_str());

        ImVec2 afterBar = ImVec2(barPos.x + barSize.x + 16.0f, startPos.y + centerOffset);
        ImGui::SetCursorScreenPos(afterBar);
        ImGui::Text("Speed: %.1fx", Speed);

        ImGui::SameLine();
        ImVec2 sepPos = ImGui::GetCursorScreenPos();
        sepPos.x += 8.0f;
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(sepPos.x, sepPos.y),
            ImVec2(sepPos.x, sepPos.y + textLineHeight),
            ImGui::GetColorU32(ImGuiCol_Text)
        );
        ImGui::Dummy(ImVec2(16.0f, 0));
        ImGui::SameLine();

        ImGui::SetCursorScreenPos(ImVec2(sepPos.x + 16.0f, startPos.y + centerOffset));
        ImGui::Text("ETA: 00:%02d:%02d", EtaSeconds / 60, EtaSeconds % 60);

        ImGui::PopStyleColor(2);
        ImGui::End();
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

                ImGuiID bottomDock;
                ImGui::DockBuilderSplitNode(mainDockId, ImGuiDir_Down, 0.33f, &bottomDock, &mainDockId);

                ImGuiID logsDock, progressDock;
                ImGui::DockBuilderSplitNode(bottomDock, ImGuiDir_Down, 0.22f, &progressDock, &logsDock);

                ImGuiID settingsDock, videoDock;
                ImGui::DockBuilderSplitNode(mainDockId, ImGuiDir_Right, 0.2f, &settingsDock, &videoDock);

                ImGui::DockBuilderGetNode(videoDock)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                        ImGuiDockNodeFlags_NoResize;
                ImGui::DockBuilderGetNode(settingsDock)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                        ImGuiDockNodeFlags_NoResize;
                ImGui::DockBuilderGetNode(logsDock)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                        ImGuiDockNodeFlags_NoResize;
                ImGui::DockBuilderGetNode(progressDock)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                        ImGuiDockNodeFlags_NoResize;

                ImGui::DockBuilderDockWindow("Video List", videoDock);
                ImGui::DockBuilderDockWindow("Settings", settingsDock);
                ImGui::DockBuilderDockWindow("Logs", logsDock);
                ImGui::DockBuilderDockWindow("Progress", progressDock);

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
