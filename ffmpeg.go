package main

import (
	"bufio"
	"fmt"
	"io"
	"strconv"
	"strings"

	g "github.com/AllenDang/giu"
	"github.com/jaypipes/ghw"
)

func handleUpscalingLogs(stderr io.ReadCloser, anime Anime) string {
	scanner := bufio.NewScanner(stderr)
	scanner.Split(bufio.ScanRunes)

	progressLine := "frame=    0 fps=0.0 q=0.0 size=       0kB time=N/A bitrate=N/A speed=N/A"
	ffmpegLogs := ""
	line := ""
	for scanner.Scan() {
		char := scanner.Text()
		if char != "\r" {
			// It's still the same line
			line += char
			continue
		}

		if !strings.HasPrefix(line, "frame=") {
			trim := strings.Replace(line, "\r", "", -1)
			trim = strings.Replace(trim, "\n", "", -1)
			logDebug(trim, false)
			ffmpegLogs += line
			line = ""
			continue
		}

		// It's line with speed and time
		speedRaw := strings.Split(strings.Split(line, "speed=")[1], " ")[0]
		time := strings.Split(strings.Split(strings.Split(line, "time=")[1], " ")[0], ".")[0]
		millis := durationToMillis(time)
		progress = float32(millis) / float32(anime.Length)

		rounded := int(progress * 100)
		if rounded == 99 {
			progress = 1
			progressLabel = "100%"
		} else {
			progressLabel = fmt.Sprintf("%d%%", rounded)
		}

		// Workaround for disappearing speed
		if strings.Contains(speedRaw, ".") {
			speedValue, _ := strconv.ParseFloat(strings.Replace(speedRaw, "x", "", -1), 32)

			currentSpeed = fmt.Sprintf("Speed: %s", speedRaw)

			// Just for safety
			if speedValue != 0 {
				etaMillis := float64(anime.Length-millis) / speedValue
				eta = fmt.Sprintf("ETA: %s", formatMillis(int64(etaMillis)))
			}
		}

		ffmpegLogs = strings.Replace(ffmpegLogs, progressLine, line, -1)
		logs = strings.Replace(logs, progressLine, line, -1)
		progressLine = line

		line = ""
		g.Update()

	}

	return ffmpegLogs
}

func buildUpscalingParams(anime Anime, resolution Resolution, shader Shader, outputPath string) []string {
	videoCodec = availableEncoders[settings.Encoder].FfmpegValue

	params := []string{
		"-hide_banner", // Hide banner with FFMPEG version
		"-y",           // Override output file
	}

	if !settings.CompatibilityMode {
		params = append(params, hwaccelParams...) // Apply selected video encoder and hardware acceleration parameters
	}

	params = append(params,
		"-i", fmt.Sprintf("%s", anime.Path), // Path to input file
		"-init_hw_device", "vulkan",
		"-vf", fmt.Sprintf("format=%s,hwupload,libplacebo=w=%d:h=%d:upscaler=ewa_lanczos:custom_shader_path=%s,hwdownload,format=yuv420p",
			availableEncoders[settings.Encoder].Format, resolution.Width, resolution.Height, shader.Path),

		"-c:a", "copy", // Copy all audio streams
		"-c:s", "copy", // Copy all subtitles streams
		"-c:d", "copy", // Copy all data streams
		"-map", "0", // Map all streams,
		"-crf", fmt.Sprintf("%d", settings.Crf), // Constant Rate Factor (CRF) for encoder
	)

	if !settings.CompatibilityMode {
		params = append(params, "-c:v", videoCodec) // Apply selected video codec

		// Preset for encoder, supported only by H264/H265
		if videoCodec != "libsvtav1" {
			params = append(params, "-preset", "slow")
		}
	}

	params = append(params, outputPath)
	return params
}

func searchHardwareAcceleration() {
	nvidia := false
	amd := false
	intel := false

	gpus, err := ghw.GPU()
	if err != nil {
		handleSoftError("Getting GPU info error", err.Error())
		return
	}

	logMessage(fmt.Sprintf("Detected GPUs (%d): ", len(gpus.GraphicsCards)), false)

	for index, gpu := range gpus.GraphicsCards {
		vendor := strings.ToLower(gpu.DeviceInfo.Vendor.Name)

		logDebug(fmt.Sprintf("GPU ID: %d, Vendor: %s", index, vendor), false)

		if strings.Contains(vendor, "nvidia") {
			nvidia = true
		} else if strings.Contains(vendor, "amd") || strings.Contains(vendor, "advanced micro devices") {
			amd = true
		} else if strings.Contains(vendor, "intel") {
			intel = true
		}

		logMessage(fmt.Sprintf("  %d. %s", index+1, gpu.DeviceInfo.Product.Name), false)
	}

	if (nvidia && intel) || (amd && intel) {
		intel = false
		logDebug("Ignoring Intel iGPU, detected NVIDIA/AMD dGPU)", false)
	}

	if nvidia && amd { // AMD is iGPU
		amd = false
		logDebug("Ignoring AMD iGPU, detected NVIDIA dGPU", false)
	}

	if nvidia {
		hwaccelParams = append(hwaccelParams, "-hwaccel_device", "cuda", "-hwaccel_output_format", "cuda")
		addEncoders("nvidia")

		logMessage("Available GPU acceleration: CUDA + NVENC", false)
	} else if amd {
		hwaccelParams = append(hwaccelParams, "-hwaccel_device", "opencl")
		addEncoders("advanced micro devices")

		logMessage("Available GPU acceleration: AMF", false)
	} else if intel {
		settings.CompatibilityMode = true
		addEncoders("cpu")

		logMessage("Intel GPUs are not supported - application may not work correctly", false)
	} else {
		settings.CompatibilityMode = true
		addEncoders("cpu")

		logMessage("There's no available GPU acceleration, application may not work correctly! Please verify your GPU drivers or report bug on GitHub", false)
	}

	for index, encoder := range availableEncoders {
		if encoder.Vendor != "cpu" {
			settings.Encoder = int32(index)
			break
		}
	}
}
