package main

import (
	"fmt"
	g "github.com/AllenDang/giu"
	"log"
	"os"
	"strconv"
	"strings"
	"time"
)

func formatMegabytes(bytes int64) string {
	return fmt.Sprintf("%d MB", bytes/(1024*1024))
}

func durationToMillis(durationStr string) int64 {
	parts := strings.Split(durationStr, ":")

	hours, err := strconv.Atoi(parts[0])
	if err != nil {
		return 0
	}

	minutes, err := strconv.Atoi(parts[1])
	if err != nil {
		return 0
	}

	seconds, err := strconv.Atoi(parts[2])
	if err != nil {
		return 0
	}

	duration := time.Duration(hours)*time.Hour + time.Duration(minutes)*time.Minute + time.Duration(seconds)*time.Second
	milliseconds := duration.Milliseconds()

	return milliseconds
}

func formatMillis(millis int64) string {
	calc := millis / 1000
	seconds := millis / 1000 % 60
	minutes := (calc % 3600) / 60
	hours := calc / 3600

	if hours < 1 {
		return fmt.Sprintf("%02d:%02d", minutes, seconds)
	}

	return fmt.Sprintf("%02d:%02d:%02d", hours, minutes, seconds)
}

func logDebug(message string, additionalLineBefore bool) {
	if settings.DebugMode {
		logMessage("[Debug] "+message, additionalLineBefore)
	}
}

func logMessage(message string, additionalLineBefore bool) {
	if additionalLineBefore {
		gui.Logs += "\n"
	}

	line := fmt.Sprintf("[%s] %s\n", time.Now().Format("15:04:05"), message)
	gui.Logs += line
	fmt.Print(line)
	g.Update()
}

func handleSoftError(message string, logs string) {
	logMessage("Critical error occurred, upscaling has been stopped", true)
	logMessage("Please make issue on GitHub with logs, upscaling settings and your computer specification", false)
	logMessage(message, true)
	logMessage(logs, false)

	for i := 0; i < len(animeList); i++ {
		animeList[i].Status = Error
	}

	gui.CurrentSpeed = "Speed:"
	gui.Eta = "ETA:"
	g.Update()
}

func handleMinMax(value *int32, minLimit int32, minValue int32, maxLimit int32, maxValue int32) {
	if *value > maxLimit {
		*value = maxValue
	}
	if *value < minLimit {
		*value = minValue
	}
}

func calcFinished() int {
	i := 0
	for _, anime := range animeList {
		if anime.Status == Finished {
			i++
		}
	}

	return i
}

func checkDebugParam() {
	if len(os.Args) != 0 {
		for _, arg := range os.Args {
			if arg == "--debug" {
				settings.DebugMode = true
				break
			}
		}
	}
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
