package main

import (
	"encoding/json"
	"os"
)

type Settings struct {
	UseSavedPosition  bool   `json:"use_saved_position"`
	PositionX         int    `json:"position_x"`
	PositionY         int    `json:"position_y"`
	Resolution        int32  `json:"resolution"`
	Shaders           int32  `json:"shaders"`
	Encoder           int32  `json:"encoder"`
	Bitrate           int32  `json:"bitrate"`
	OutputFormat      int32  `json:"output_format"`
	CompatibilityMode bool   `json:"compatibility_mode"`
	DebugMode         bool   `json:"debug_mode"`
	Version           string `json:"version"`
}

func saveSettings() {
	os.Remove("settings.json") // Ignore error as file may not exist

	file, err := os.Create("settings.json")
	check(err)

	marshalled, err := json.Marshal(settings)
	if err != nil {
		return
	}

	_, err = file.Write(marshalled)
	check(err)
}

func loadSettings() bool {
	bytes, err := os.ReadFile("settings.json")
	if err != nil {
		return false
	}

	loadedSettings := Settings{}
	err = json.Unmarshal(bytes, &loadedSettings)
	if err != nil || loadedSettings.Version != version {
		os.Remove("settings.json")
		return false
	}

	settings = loadedSettings
	return true
}
