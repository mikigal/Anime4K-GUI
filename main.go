package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"syscall"

	g "github.com/AllenDang/giu"
)

const version = "1.1.2"

var (
	supportedInput = []string{".mp4", ".avi", ".mkv"}

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
		{"Anime4K Mode A+A", "shaders/Anime4K_ModeA+A.glsl"},
		{"Anime4K Mode B", "shaders/Anime4K_ModeB.glsl"},
		{"Anime4K Mode B+B", "shaders/Anime4K_ModeB+B.glsl"},
		{"Anime4K Mode C", "shaders/Anime4K_ModeC.glsl"},
		{"Anime4K Mode C+A", "shaders/Anime4K_ModeC+A.glsl"},
		{"FSRCNNX", "shaders/FSRCNNX_x2_16-0-4-1.glsl"},
	}

	allEncoders = []Encoder{
		{"H.264 (CPU)", "libx264", "yuv420p", "cpu"},
		{"H.264 NVENC (NVIDIA)", "h264_nvenc", "yuv420p", "nvidia"},
		{"H.264 AMF (AMD)", "h264_amf", "yuv420p", "advanced micro devices"},

		{"H.265 (CPU)", "libx265", "yuv420p", "cpu"},
		{"H.265 NVENC (NVIDIA)", "hevc_nvenc", "yuv420p", "nvidia"},
		{"H.265 AMF (AMD)", "hevc_amf", "yuv420p", "advanced micro devices"},

		{"AV1 (CPU)", "libsvtav1", "yuv420p", "cpu"},
		{"AV1 NVENC (NVIDIA)", "av1_nvenc", "yuv420p", "nvidia"},
		{"AV1 AMF (AMD)", "av1_amf", "yuv420p", "advanced micro devices"},
	}

	availableEncoders = make([]Encoder, 0)

	outputFormats = []string{"MP4", "AVI", "MKV"}

	settings = Settings{
		UseSavedPosition:  true,
		Resolution:        5,
		Shaders:           0,
		Encoder:           0,
		Crf:               20,
		OutputFormat:      2,
		CompatibilityMode: false,
		DebugMode:         false,
		Version:           version,
	}

	// UI variables
	currentSpeed  = "Speed:"
	eta           = "ETA:"
	progress      float32
	progressLabel string
	totalProgress string
	buttonLabel   = "Start"
	logs          = "Version: Anime4K-GUI (" + version + ")\n" +
		"Authors: mikigal (whole app + FFMPEG tweaks), Ethan (core FFMPEG stuff)\n" +
		"Special thanks to bloc97 for Anime4K shaders\n" +
		"Drag n' drop your video files into this window (supported extensions: mp4, avi, mkv)\n\n"

	// Internals
	animeList  = make([]Anime, 0)
	processing = false
	cancelled  = false

	// FFMPEG params
	hwaccelParams []string
	videoCodec    string
)

func main() {
	checkDebugParam()
	loaded := loadSettings()

	window := g.NewMasterWindow("Anime4K-GUI", 1600, 950, g.MasterWindowFlagsNotResizable)
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
		}
	}

	progress = 0
	progressLabel = ""
	buttonLabel = "Cancel"
	processing = true
	resetUI()

	logMessage("Started upscaling! Upscaled videos will be saved in original directory, with _upscaled suffix in files name", false)

	logDebug("CV value: "+videoCodec, false)
	g.Update()

	for index, anime := range animeList {
		if animeList[index].Status == Finished {
			continue
		}

		message := fmt.Sprintf("Processing %s (%d / %d)...", anime.Name, index+1, len(animeList))
		logMessage(message, false)
		animeList[index].Status = Processing
		g.Update()

		outputPath := fmt.Sprintf("%s_upscaled.%s", strings.TrimSuffix(anime.Path, filepath.Ext(anime.Path)), strings.ToLower(outputFormat))
		ffmpegParams := buildUpscalingParams(anime, resolution, shader, outputPath)

		workingDirectory, err := os.Getwd()
		if err != nil {
			animeList[index].Status = Error
			buttonLabel = "Start"
			processing = false
			g.Update()
			handleSoftError("Getting working directory error:", err.Error())
			return
		}

		logDebug("Working directory: "+workingDirectory, false)
		logDebug("Input path: "+anime.Path, false)
		logDebug("Output path: "+outputPath, false)
		logDebug("Target resolution: "+resolution.Format(), false)
		logDebug("Shaders: "+shader.Path, false)
		logDebug("Output format: "+outputFormat, false)
		logDebug("FFMPEG command: .\\ffmpeg.exe\\ffmpeg.exe "+strings.Join(ffmpegParams, " "), false)
		g.Update()

		cmd := exec.Command(".\\ffmpeg\\ffmpeg.exe", ffmpegParams...)
		cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

		stderr, err := cmd.StderrPipe()
		if err != nil {
			os.Remove(outputPath)
			animeList[index].Status = Error
			buttonLabel = "Start"
			processing = false
			g.Update()
			handleSoftError("Creating pipe error:", err.Error())
			return
		}

		err = cmd.Start()
		if err != nil {
			os.Remove(outputPath)
			animeList[index].Status = Error
			buttonLabel = "Start"
			processing = false
			g.Update()
			handleSoftError("Starting ffmpeg process error:", err.Error())
			return
		}

		ffmpegLogs := handleUpscalingLogs(stderr, anime)

		err = cmd.Wait()
		if err != nil {
			os.Remove(outputPath)
			if cancelled {
				cancelled = false
				return
			}

			animeList[index].Status = Error
			buttonLabel = "Start"
			processing = false
			g.Update()
			handleSoftError("FFMPEG Error:", err.Error())
			handleSoftError("FFMPEG logs:", ffmpegLogs)
			return
		}

		animeList[index].Status = Finished
		resetUI()
		logMessage(fmt.Sprintf("Finished processing %s", anime.Name), false)
	}

	buttonLabel = "Start"
	processing = false
	resetUI()
	logMessage("Finished upscaling!", false)
	g.Update()
}

func cancelProcessing() {
	cancelled = true
	cmd := exec.Command("taskkill", "/IM", "ffmpeg.exe", "/F")
	err := cmd.Start()
	if err != nil {
		handleSoftError("Starting taskkill error", err.Error())
		return
	}

	err = cmd.Wait()
	if err != nil {
		handleSoftError("Taskkill error", err.Error())
		return
	}

	for i := 0; i < len(animeList); i++ {
		if animeList[i].Status != Finished {
			animeList[i].Status = NotStarted
			g.Update()
		}
	}

	processing = false
	buttonLabel = "Start"
	resetUI()
	logMessage("Cancelled upscaling!", false)
	g.Update()
}
