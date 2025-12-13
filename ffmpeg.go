package main

import (
	"bufio"
	"fmt"
	"io"
	"runtime"
	"slices"
	"strconv"
	"strings"

	g "github.com/AllenDang/giu"
)

func handleUpscalingLogs(stderr io.ReadCloser, anime Anime, index int) string {
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
			trim := strings.ReplaceAll(line, "\r", "")
			trim = strings.ReplaceAll(trim, "\n", "")

			guiMutex.Lock()
			logDebug(trim, false)
			guiMutex.Unlock()

			ffmpegLogs += line
			line = ""
			continue
		}

		guiMutex.Lock()

		// It's line with speed and time
		time := strings.Split(readOutputParameter(line, "time", "bitrate"), ".")[0]
		millis := durationToMillis(time)

		var currentProgress float32 = 0.0
		if anime.Length > 0 {
			currentProgress = float32(millis) / float32(anime.Length)
		}

		// FFMPEG may not report time, we must calculate it manually
		if time == "N/A" && len(strings.Split(line, "frame=")) > 1 {
			frame, _ := strconv.ParseFloat(readOutputParameter(line, "frame", "fps"), 32)
			if anime.TotalFrames > 0 {
				currentProgress = float32(frame) / float32(anime.TotalFrames)
				// If time is N/A, estimate current millis from progress
				if anime.Length > 0 {
					millis = int64(currentProgress * float32(anime.Length))
				}
			}

			fps, _ := strconv.ParseFloat(readOutputParameter(line, "fps", "q"), 32)
			if anime.FrameRate > 0 {
				fileSpeed[index] = fps / anime.FrameRate
			}
		}

		fileProgress[index] = currentProgress

		// Speed
		speedRaw := strings.ReplaceAll(readOutputParameter(line, "speed", ""), "x", "")
		if strings.Contains(speedRaw, ".") {
			speedValue, _ := strconv.ParseFloat(speedRaw, 64)
			if speedValue > 0 {
				fileSpeed[index] = speedValue
			}
		}

		// Calculate ETA using the latest speed (from either FPS or Speed tag)
		if fileSpeed[index] > 0 && anime.Length > 0 {
			remainingMillis := anime.Length - millis
			if remainingMillis > 0 {
				etaMillis := int64(float64(remainingMillis) / fileSpeed[index])
				fileEta[index] = formatMillis(etaMillis)
			}
		}

		// We don't calculate global ETA/Progress here anymore, purely per-file.
		// Layout update is handled by main loop or g.Update() calls.

		ffmpegLogs = strings.ReplaceAll(ffmpegLogs, progressLine, line)
		gui.Logs = strings.ReplaceAll(gui.Logs, progressLine, line)
		progressLine = line

		g.Update()
		guiMutex.Unlock()

		line = ""
	}

	return ffmpegLogs
}

func buildUpscalingParams(anime Anime, resolution Resolution, shader Shader, outputPath string) []string {
	encoder := availableEncoders[settings.Encoder]
	videoCodec = encoder.FfmpegValue

	params := []string{
		"-hide_banner", // Hide banner with FFMPEG version
		"-y",           // Override output file
	}

	params = append(params, hwaccelParams...) // Apply selected video encoder and hardware acceleration parameters

	params = append(params,
		"-i", fmt.Sprintf("%s", anime.Path), // Path to input file
		"-init_hw_device", "vulkan",
		"-vf", fmt.Sprintf("format=%s,hwupload,libplacebo=w=%d:h=%d:upscaler=ewa_lanczos:custom_shader_path=%s,format=%s",
			anime.PixelFormat, resolution.Width, resolution.Height, shader.Path, anime.PixelFormat),

		"-c:a", "copy", // Copy all audio streams
		"-c:s", "copy", // Copy all subtitles streams
		"-c:d", "copy", // Copy all data streams
	)

	for _, stream := range anime.Streams {
		if !slices.Contains(codecsBlacklist, stream.CodecName) {
			params = append(params, "-map", fmt.Sprintf("0:%d", stream.Index))
		}
	}

	if encoder.CrfSupported {
		params = append(params,
			"-crf", fmt.Sprintf("%d", settings.Crf),
		)
	} else {
		if encoder.Vendor == "nvidia" {
			params = append(params, "-rc", "vbr")
		} else {
			params = append(params, "-rc", "cq")
		}

		params = append(params,
			"-cq", fmt.Sprintf("%d", settings.Cq),
			"-b:v", "0",
		)
	}

	params = append(params, "-c:v", videoCodec) // Apply selected video codec

	// Preset for encoder, supported only by H264/H265
	if !strings.Contains(videoCodec, "av1") {
		params = append(params, "-preset", "slow")
	}

	// Threads limit for CPU based encoders
	if videoCodec == "libx264" && settings.CpuThreads != int32(runtime.NumCPU()) {
		params = append(params, "-threads", fmt.Sprintf("%d", settings.CpuThreads))
	}
	if videoCodec == "libx265" && settings.CpuThreads != int32(runtime.NumCPU()) {
		params = append(params, "-x265-params", fmt.Sprintf("pools=%d", min(settings.CpuThreads, 16))) // libx265 allow max 16 threads
	}
	if videoCodec == "libsvtav1" && settings.CpuThreads != int32(runtime.NumCPU()) {
		params = append(params, "-svtav1-params", fmt.Sprintf("pin=%d", settings.CpuThreads))
	}

	params = append(params, outputPath)
	return params
}

func readOutputParameter(line string, parameter string, nextParameter string) string {
	if nextParameter == "" {
		return strings.ReplaceAll(strings.Split(line, parameter+"=")[1], " ", "")
	}

	return strings.ReplaceAll(strings.Split(strings.Split(line, parameter+"=")[1], nextParameter)[0], " ", "")
}
