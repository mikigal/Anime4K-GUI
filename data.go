package main

import "fmt"

type Anime struct {
	Name   string
	Length int64
	Size   int64
	Width  int
	Height int
	Path   string
	Status AnimeStatus
}

type Shader struct {
	Name string
	Path string
}

type Encoder struct {
	Name        string
	FfmpegValue string
	Vendor      string
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
	updateUI()
	logMessage(fmt.Sprintf("Removed %s from queue", anime.Name), false)
}

func addEncoders(vendor string) {
	for _, encoder := range allEncoders {
		if encoder.Vendor == vendor || encoder.Vendor == "cpu" {
			availableEncoders = append(availableEncoders, encoder)
		}
	}
}
