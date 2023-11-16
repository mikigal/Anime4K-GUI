package main

import (
	"fmt"
	g "github.com/AllenDang/giu"
	"os"
	"os/exec"
	"strings"
	"syscall"
)

const version = "1.0.4"

var (

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

	shadersModes = []ShadersMode{
		{"Mode A", "shaders/Anime4K_ModeA.glsl"},
		{"Mode A+A", "shaders/Anime4K_ModeA+A.glsl"},
		{"Mode B", "shaders/Anime4K_ModeB.glsl"},
		{"Mode B+B", "shaders/Anime4K_ModeB+B.glsl"},
		{"Mode C", "shaders/Anime4K_ModeC.glsl"},
		{"Mode C+A", "shaders/Anime4K_ModeC+A.glsl"},
	}

	compressionPresets = []CompressionPreset{
		{"High (Worst quality, smaller output file)", "fast"},
		{"Medium", "medium"},
		{"Low (Recommended, best quality, bigger output file)", "slow"},
	}

	outputFormats = []string{"MP4", "AVI", "MKV"}

	// Pointers for UI
	selectedResolution          int32 = 5
	selectedShadersMode         int32
	selectedCompressionPreset   int32 = 2
	selectedOutputFormat        int32
	disableHardwareAcceleration bool
	debug                       bool

	// UI variables
	currentSpeed  = "Speed:"
	currentTime   = "Time:"
	progress      float32
	progressLabel string
	buttonLabel   = "Start"
	logs          = "Version: Anime4K-GUI (" + version + ")\n" +
		"Authors: mikigal (whole app), Ethan (FFMPEG stuff)\n" +
		"Special thanks to bloc97 for Anime4K shaders\n" +
		"Drag n' drop your video files into this window (supported extensions: mp4, avi, mkv)\n\n"
	gpuTemperature string
	gpuUsage       string
	vramUsage      string

	// Internals
	animeList  = make([]Anime, 0)
	processing = false
	cancelled  = false

	// FFMPEG params
	hwaccelParam string
	hwaccelValue string
	cvValue      string
)

func main() {
	checkDebugParam()
	searchHardwareAcceleration()
	go monitorSensors()

	window := g.NewMasterWindow("Anime4K-GUI", 1600, 950, g.MasterWindowFlagsNotResizable)
	window.SetDropCallback(handleDrop)
	window.Run(loop)
}

func startProcessing() {
	if processing {
		return
	}

	resolution := resolutions[selectedResolution]
	shadersMode := shadersModes[selectedShadersMode]
	compressionPreset := compressionPresets[selectedCompressionPreset]
	outputFormat := strings.ToLower(outputFormats[selectedOutputFormat])

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

	buttonLabel = "Cancel"
	processing = true
	updateUI()

	logMessage("Started upscaling! Upscaled videos will be saved in original directory, with _upscaled suffix in files name", false)

	logDebug("Hardware acceleration param: "+hwaccelParam, false)
	logDebug("Hardware acceleration value: "+hwaccelValue, false)
	logDebug("CV value: "+cvValue, false)
	g.Update()

	for index, anime := range animeList {
		if animeList[index].Status == Finished {
			continue
		}

		message := fmt.Sprintf("Processing %s (%d / %d)...", anime.Name, index+1, len(animeList))
		logMessage(message, false)
		animeList[index].Status = Processing
		g.Update()

		outputPath := buildOutputPath(anime, outputFormat)
		ffmpegParams := buildUpscalingParams(anime, resolution, shadersMode, compressionPreset, outputPath)

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
		logDebug("Shaders: "+shadersMode.Path, false)
		logDebug("Compression preset (ffmpeg name): "+compressionPreset.FfmpegName, false)
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

		ffmpegLogs := handleUpscalingLogs(stderr)

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
		updateUI()
		logMessage(fmt.Sprintf("Finished processing %s", anime.Name), false)
	}

	buttonLabel = "Start"
	processing = false
	updateUI()
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
	updateUI()
	logMessage("Cancelled upscaling!", false)
	g.Update()
}
