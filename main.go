package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"sync"
	"syscall"

	g "github.com/AllenDang/giu"
)

const version = "1.2.3"

var (
	supportedInput = []string{".mp4", ".avi", ".mkv"}

	// Parallel Processing
	guiMutex     sync.Mutex
	fileProgress = make(map[int]float32)
	fileSpeed    = make(map[int]float64)
	fileEta      = make(map[int]string)

	// Available options
	resolutions = []Resolution{
		{1024, 768, false},
		{1440, 1080, false},
		{1920, 1440, false},
		{2880, 2160, false},

		{1280, 720, true},
		{1920, 1080, true},
		{2560, 1440, true},
		{3840, 2160, true},
	}

	shaders = []Shader{
		{"Anime4K Mode A", "shaders/Anime4K_ModeA.glsl"},
		{"Anime4K Mode A+A", "shaders/Anime4K_ModeA_A.glsl"},
		{"Anime4K Mode B", "shaders/Anime4K_ModeB.glsl"},
		{"Anime4K Mode B+B", "shaders/Anime4K_ModeB_B.glsl"},
		{"Anime4K Mode C", "shaders/Anime4K_ModeC.glsl"},
		{"Anime4K Mode C+A", "shaders/Anime4K_ModeC_A.glsl"},
		{"FSRCNNX x2", "shaders/FSRCNNX_x2_16-0-4-1.glsl"},
	}

	allEncoders = []Encoder{
		{"H.264 (CPU)", "libx264", "cpu", true},
		{"H.264 NVENC (NVIDIA)", "h264_nvenc", "nvidia", false},
		{"H.264 AMF (AMD)", "h264_amf", "advanced micro devices", false},

		{"H.265 (CPU)", "libx265", "cpu", true},
		{"H.265 NVENC (NVIDIA)", "hevc_nvenc", "nvidia", false},
		{"H.265 AMF (AMD)", "hevc_amf", "advanced micro devices", false},

		{"AV1 (CPU)", "libsvtav1", "cpu", true},
		{"AV1 NVENC (NVIDIA)", "av1_nvenc", "nvidia", false},
		{"AV1 AMF (AMD)", "av1_amf", "advanced micro devices", false},
	}

	gpuAv1Supported   = false
	availableEncoders = make([]Encoder, 0)
	outputFormats     = []string{"MP4", "AVI", "MKV"}

	// Settings
	settings = Settings{
		UseSavedPosition: true,
		Resolution:       5,
		Shaders:          0,
		Encoder:          0,
		Crf:              18,
		Cq:               18,
		OutputFormat:     2,
		CpuThreads:       int32(runtime.NumCPU()),
		DebugMode:        false,
		Version:          version,
	}

	// GUI pointers
	gui = Gui{
		CurrentSpeed:  "Speed:",
		Eta:           "ETA:",
		Progress:      0,
		ProgressLabel: "",
		TotalProgress: "",
		ButtonLabel:   "Start",
		Logs: "Version: Anime4K-GUI (" + version + ")\n" +
			"Authors: mikigal, Ethan (core libplacebo stuff)\n" +
			"Special thanks to bloc97 for Anime4K shaders\n" +
			"Drag n' drop your video files into this window (supported extensions: mp4, avi, mkv)\n\n",
	}

	// Internals
	animeList       = make([]Anime, 0)
	processing      = false
	cancelled       = false
	codecsBlacklist = []string{"mjpeg", "png"}

	// FFMPEG params
	hwaccelParams []string
	videoCodec    string
)

func main() {
	checkDebugParam()
	loaded := loadSettings()
	preventSleep()

	window := g.NewMasterWindow("Anime4K-GUI", 1800, 950, g.MasterWindowFlagsNotResizable)
	searchHardwareAcceleration()

	if loaded && settings.UseSavedPosition {
		window.SetPos(settings.PositionX, settings.PositionY)
	}

	window.SetDropCallback(handleDrop)
	window.Run(func() { loop(window) })

	saveSettings()
}

func startProcessing() {
	if processing {
		return
	}

	resolution := resolutions[settings.Resolution]
	shader := shaders[settings.Shaders]
	outputFormat := strings.ToLower(outputFormats[settings.OutputFormat])

	if len(animeList) == 0 {
		logMessage("There's no videos on list, can not start. Drag files into this window to add video", false)
		g.Update()
		return
	}

	for i := 0; i < len(animeList); i++ {
		if animeList[i].Status != Finished {
			animeList[i].Status = Waiting
			fileProgress[i] = 0.0 // Reset progress
		} else {
			fileProgress[i] = 1.0
		}
		fileSpeed[i] = 0.0
		delete(fileEta, i)
	}

	gui.Progress = 0
	gui.ProgressLabel = ""
	gui.ButtonLabel = "Cancel"
	processing = true
	cancelled = false // Reset cancelled flag to ensure new jobs can run
	// resetUI() // No need to reset full UI here if we are about to start

	logMessage("Started upscaling! Upscaled videos will be saved in original directory, with _upscaled suffix in files name", false)

	logDebug("CV value: "+videoCodec, false)
	g.Update()

	// Parallel Processing
	var wg sync.WaitGroup
	// Ensure safe default if not set
	if settings.MaxConcurrentUpscales == 0 {
		settings.MaxConcurrentUpscales = 1
	}

	concurrencyLimit := settings.MaxConcurrentUpscales
	// Force concurrency to 1 if CPU is used
	if availableEncoders[settings.Encoder].Vendor == "cpu" {
		concurrencyLimit = 1
		logDebug("CPU encoder selected, enforcing concurrency limit of 1", false)
	}

	sem := make(chan struct{}, concurrencyLimit)

	for index, anime := range animeList {
		idx := index
		a := anime

		if animeList[idx].Status == Finished {
			continue
		}

		if cancelled {
			break
		}

		wg.Add(1)
		sem <- struct{}{}

		go func(i int, anime Anime) {
			defer wg.Done()
			defer func() { <-sem }()

			if cancelled {
				return
			}

			guiMutex.Lock()
			message := fmt.Sprintf("Processing %s (%d / %d)...", anime.Name, i+1, len(animeList))
			gui.Logs += message + "\n"
			animeList[i].Status = Processing
			g.Update()
			guiMutex.Unlock()

			if anime.HasSubtitlesStream && outputFormat != "mkv" {
				handleStartUpscalingError("", i, "File "+anime.Name+" contains subtitles stream, output format must be MKV", errors.New(""))
				return
			}

			outputPath := fmt.Sprintf("%s_upscaled.%s", strings.TrimSuffix(anime.Path, filepath.Ext(anime.Path)), strings.ToLower(outputFormat))
			ffmpegParams := buildUpscalingParams(anime, resolution, shader, outputPath)

			// Debug logs
			workingDirectory, _ := os.Getwd()
			guiMutex.Lock()
			logDebug("Working directory: "+workingDirectory, false)
			logDebug("Input path: "+anime.Path, false)
			logDebug("Output path: "+outputPath, false)
			logDebug("Target resolution: "+resolution.Format(), false)
			logDebug("Shaders: "+shader.Path, false)
			logDebug("Output format: "+outputFormat, false)
			logDebug("FFMPEG command: .\\ffmpeg\\ffmpeg.exe "+strings.Join(ffmpegParams, " "), false)
			logDebug(fmt.Sprintf("[%d] Starting %s", i, anime.Name), false)
			guiMutex.Unlock()

			cmd := exec.Command(".\\ffmpeg\\ffmpeg.exe", ffmpegParams...)
			cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

			stderr, err := cmd.StderrPipe()
			if err != nil {
				handleStartUpscalingError(outputPath, i, "Creating pipe error:", err)
				return
			}

			err = cmd.Start()
			if err != nil {
				handleStartUpscalingError(outputPath, i, "Starting ffmpeg process error:", err)
				return
			}

			// We need handleUpscalingLogs to take 'i' (index) to update specific file progress
			ffmpegLogs := handleUpscalingLogs(stderr, anime, i)

			err = cmd.Wait()
			if err != nil {
				if cancelled {
					return
				}
				handleStartUpscalingError(outputPath, i, "FFMPEG Error:", err)
				handleSoftError("FFMPEG logs:", ffmpegLogs)
				return
			}

			guiMutex.Lock()
			animeList[i].Status = Finished
			fileProgress[i] = 1.0
			fileSpeed[i] = 0.0
			g.Update()
			guiMutex.Unlock()

			logMessage(fmt.Sprintf("Finished processing %s", anime.Name), false)

		}(idx, a)
	}

	wg.Wait()

	gui.ButtonLabel = "Start"
	processing = false
	resetUI()
	logMessage("Finished upscaling!", false)
	g.Update()
}

func cancelProcessing() {
	cancelled = true
	// Update UI immediately to look responsive
	gui.ButtonLabel = "Cancelling..."
	g.Update()

	go func() {
		cmd := exec.Command("taskkill", "/IM", "ffmpeg.exe", "/F")
		cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
		err := cmd.Start()
		if err != nil {
			handleSoftError("Starting taskkill error", err.Error())
			return
		}

		err = cmd.Wait()
		if err != nil {
			// Ignore error if taskkill fails (e.g. no processes found)
		}

		for i := 0; i < len(animeList); i++ {
			if animeList[i].Status != Finished {
				animeList[i].Status = NotStarted
			}
		}

		// Reset parallel tracking maps
		for k := range fileProgress {
			delete(fileProgress, k)
		}
		for k := range fileSpeed {
			delete(fileSpeed, k)
		}
		for k := range fileEta {
			delete(fileEta, k)
		}

		processing = false
		gui.ButtonLabel = "Start"
		resetUI()
		logMessage("Cancelled upscaling!", false)
		g.Update()
	}()
}
