package main

import (
	"context"
	"fmt"
	"gopkg.in/vansante/go-ffprobe.v2"
	"os"
	"path/filepath"
	"slices"
	"strconv"
	"strings"
)

func handleFfprobe(path string, ctx context.Context) {
	if processing {
		return
	}

	extension := filepath.Ext(path)
	if !slices.Contains(supportedInput, extension) {
		logMessage(fmt.Sprintf("Invalid input file format (supported: %s)! Path: %s", strings.Join(supportedInput, ", "), path), false)
		return
	}

	for _, anime := range animeList {
		if anime.Path == path {
			logMessage("File is already added to queue, ignoring it", false)
			return
		}
	}

	file, err := os.Stat(path)
	if err != nil {
		handleSoftError("Reading file stats error", err.Error())
		return
	}

	data, err := ffprobe.ProbeURL(ctx, path)
	if err != nil {
		handleSoftError("FFPROBE error", err.Error())
		return
	}

	videoStream := data.FirstVideoStream()
	pathSplit := strings.Split(path, string(os.PathSeparator))

	frameRateSplit := strings.Split(videoStream.AvgFrameRate, "/")
	base, _ := strconv.ParseFloat(frameRateSplit[0], 64)
	divider, _ := strconv.ParseFloat(frameRateSplit[1], 64)

	anime := Anime{
		Name:               pathSplit[len(pathSplit)-1],
		Length:             int64(data.Format.DurationSeconds * 1000),
		Size:               file.Size(),
		Width:              videoStream.Width,
		Height:             videoStream.Height,
		FrameRate:          base / divider,
		TotalFrames:        int((base / divider) * data.Format.DurationSeconds),
		HasSubtitlesStream: data.FirstSubtitleStream() != nil,
		Path:               path,
		Streams:            data.Streams,
		PixelFormat:        videoStream.PixFmt,
		Status:             NotStarted,
	}
	animeList = append(animeList, anime)
	gui.TotalProgress = fmt.Sprintf("%d / %d", calcFinished(), len(animeList))
	logMessage("Added file "+path, false)
}
