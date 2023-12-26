package main

import (
	"bufio"
	"fmt"
	"io"
	"strings"

	g "github.com/AllenDang/giu"
	"github.com/jaypipes/ghw"
)

func handleUpscalingLogs(stderr io.ReadCloser) string {
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
			currentTime = fmt.Sprintf("Time: %s", value)
			g.Update()
		}

		line = ""
	}

	return ffmpegLogs
}

func buildUpscalingParams(anime Anime, resolution Resolution, shadersMode ShadersMode, compressionPreset CompressionPreset, outputPath string) []string {
	params := []string{
		"-hide_banner",
		"-y",
	}

	if hwaccelValue != "" && !disableHardwareAcceleration {
		params = append(params, hwaccelParam, hwaccelValue)

		// Additional NVIDIA stuff
		if hwaccelValue == "cuda" {
			params = append(params, "-hwaccel_output_format", "cuda")
		}
	}

	params = append(params,
		"-i", fmt.Sprintf("%s", anime.Path),
		"-init_hw_device", "vulkan",
		"-vf", fmt.Sprintf("format=yuv420p,hwupload,libplacebo=w=%d:h=%d:upscaler=ewa_lanczos:custom_shader_path=%s,hwdownload,format=yuv420p", resolution.Width, resolution.Height, shadersMode.Path),
	)

	if cvValue != "" && !disableHardwareAcceleration && !disableHardwareAccelerationEncoding {
		params = append(params, "-c:v", cvValue)
	}

	params = append(params,
		"-preset", compressionPreset.FfmpegName,
		outputPath,
	)

	return params
}

func buildOutputPath(anime Anime, outputFormat string) string {
	dotSplit := strings.Split(anime.Path, ".")
	extension := dotSplit[len(dotSplit)-1]
	return strings.Replace(anime.Path, "."+extension, "_upscaled."+outputFormat, -1)
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
		hwaccelParam = "-hwaccel_device"
		hwaccelValue = "cuda"
		cvValue = "h264_nvenc"
		logMessage("Available GPU acceleration: CUDA + NVENC", false)
	} else if amd {
		hwaccelParam = "-hwaccel_device"
		hwaccelValue = "opencl"
		cvValue = "h264_amf"
		logMessage("Available GPU acceleration: AMF", false)
	} else if intel {
		hwaccelParam = "-hwaccel"
		hwaccelValue = "vulkan"
		cvValue = "h264_qsv"
		logMessage("Available GPU acceleration: QSV", false)
	} else {
		// Something weird happened
		cvValue = ""
		logMessage("There's no available GPU acceleration, application may not work correctly! Please verify your GPU drivers or report bug on GitHub", false)
	}
	if disableHardwareAccelerationEncoding {
		logMessage("You have disabled GPU encoding, so we're running with H.264 on your CPU", false)
	}
}
