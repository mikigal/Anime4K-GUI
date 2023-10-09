package main

import (
	"fmt"
	g "github.com/AllenDang/giu"
	"os/exec"
	"strconv"
	"strings"
	"syscall"
	"time"
)

func readNvidiaTemperature() string {
	return readNvidiaInfo("temperature.gpu") + " C"
}

func readGpuUsage() string {
	return readGenericInfo("GPU Engine(*engtype_3D)\\Utilization Percentage") + "%"
}

func readVramUsage() string {
	raw := readGenericInfo("GPU Process Memory(*)\\Local Usage")
	bytes, err := strconv.ParseInt(raw, 10, 64)
	if err != nil {
		logMessage("Invalid VRAM usage output: "+err.Error(), false)
		return ""
	}

	return fmt.Sprintf("%.2f GB", float64(bytes)/1024/1024/1024)
}

func readNvidiaInfo(query string) string {
	cmd := exec.Command("nvidia-smi", "--query-gpu="+query, "--format=csv,noheader")
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

	rawOut, err := cmd.Output()
	if err != nil {
		logMessage("Could not fetch NVIDIA GPU info: "+err.Error(), false)
		return ""
	}

	return strings.Replace(string(rawOut), "\n", "", -1)
}

func readGenericInfo(query string) string {
	cmd := exec.Command("powershell", "-NoProfile", "(((Get-Counter \"\\"+query+"\").CounterSamples | where CookedValue).CookedValue | measure -sum).sum")
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

	rawOut, err := cmd.Output()
	if err != nil {
		logMessage("Could not fetch generic GPU info: "+err.Error(), false)
		return ""
	}

	return strings.Replace(strings.Split(string(rawOut), ",")[0], "\r\n", "", -1)
}

func updateSensorsData() {
	gpuUsage = readGpuUsage()
	vramUsage = readVramUsage()
	if hwaccelValue == "cuda" {
		gpuTemperature = readNvidiaTemperature()
	}
}

func monitorSensors() {
	for {
		updateSensorsData()
		g.Update()
		time.Sleep(1 * time.Second)
	}
}
