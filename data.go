package main

import (
	"fmt"
	"gopkg.in/vansante/go-ffprobe.v2"
)

type Anime struct {
	Name               string
	Length             int64
	Size               int64
	Width              int
	Height             int
	FrameRate          float64
	TotalFrames        int
	HasSubtitlesStream bool
	Streams            []*ffprobe.Stream
	PixelFormat        string
	Path               string
	Status             AnimeStatus
}

type Gui struct {
	CurrentSpeed  string
	Eta           string
	Progress      float32
	ProgressLabel string
	TotalProgress string
	ButtonLabel   string
	Logs          string
}

type Shader struct {
	Name string
	Path string
}

type Encoder struct {
	Name         string
	FfmpegValue  string
	Vendor       string
	CrfSupported bool
}

type Resolution struct {
	Width     int
	Height    int
	Panoramic bool
}

type AnimeStatus string

const (
	Waiting    AnimeStatus = "Waiting"
	Processing AnimeStatus = "Processing"
	Finished   AnimeStatus = "Finished"
	Error      AnimeStatus = "Error"
	NotStarted AnimeStatus = "Not started"
)

func (res *Resolution) Format() string {
	if !res.Panoramic {
		return fmt.Sprintf("%dx%d (4:3)", res.Width, res.Height)
	}

	return fmt.Sprintf("%dx%d", res.Width, res.Height)
}

func removeAnime(index int) {
	anime := animeList[index]
	animeList = append(animeList[:index], animeList[index+1:]...)
	resetUI()
	logMessage(fmt.Sprintf("Removed %s from queue", anime.Name), false)
}

func addEncoders(vendor string) {
	for _, encoder := range allEncoders {
		if encoder.FfmpegValue == "av1_nvenc" && !gpuAv1Supported {
			continue
		}

		if encoder.Vendor == vendor || encoder.Vendor == "cpu" {
			availableEncoders = append(availableEncoders, encoder)
		}
	}
}
