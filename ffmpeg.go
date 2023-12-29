package main

import (
	"bufio"
	"fmt"
	"io"
	"strings"

	g "github.com/AllenDang/giu"
	"github.com/jaypipes/ghw"
)

func handleUpscalingLogs(stderr io.ReadCloser, anime Anime) string {
	scanner := bufio.NewScanner(stderr)
	scanner.Split(bufio.ScanRunes)

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
		}

		if strings.Contains(line, "speed=") {
			value := strings.Split(strings.Split(line, "speed=")[1], " ")[0]

			// Workaround for disappearing speed
			if strings.Contains(value, ".") {
				currentSpeed = fmt.Sprintf("Speed: %s", value)
				g.Update()
			}
		}

		if strings.Contains(line, "time=") {
			value := strings.Split(strings.Split(strings.Split(line, "time=")[1], " ")[0], ".")[0]
			millis := durationToMillis(value)
			progress = float32(millis) / float32(anime.Length)

			rounded := int(progress * 100)
			if rounded == 99 {
				progress = 1
				progressLabel = "100%"
			} else {
				progressLabel = fmt.Sprintf("%d%%", rounded)
			}

			g.Update()
		}

		line = ""
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
		"-vf", fmt.Sprintf("format=yuv420p,hwupload,libplacebo=w=%d:h=%d:upscaler=ewa_lanczos:custom_shader_path=%s,hwdownload,format=yuv420p", resolution.Width, resolution.Height, shader.Path),

		"-c:a", "copy", // Copy all audio streams without re-encoding
		"-c:s", "mov_text", // Force re-encoding subtitles with mov_text codec for compatibility reasons
		"-map", "0", // Map all streams
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
		hwaccelParams = append(hwaccelParams, "-hwaccel", "vulkan")
		addEncoders("intel")

		logMessage("Available GPU acceleration: QSV", false)
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
