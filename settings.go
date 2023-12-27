package main

import (
	"encoding/json"
	"os"
)

type Settings struct {
	PositionX         int    `json:"position_x"`
	PositionY         int    `json:"position_y"`
	Resolution        int32  `json:"resolution"`
	ShadersMode       int32  `json:"shaders_mode"`
	CompressionPreset int32  `json:"compression_preset"`
	Encoder           int32  `json:"encoder"`
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
