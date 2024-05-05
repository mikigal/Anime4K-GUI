package main

import (
	"fmt"
	"github.com/jaypipes/ghw"
	"strings"
)

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
