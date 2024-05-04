package main

import (
	"context"
	"fmt"
	"os"
	"path/filepath"
	"slices"
	"strconv"
	"strings"
	"time"

	g "github.com/AllenDang/giu"
	"github.com/AllenDang/imgui-go"
	"gopkg.in/vansante/go-ffprobe.v2"
)

const dragDropLabel = "\n\n\n\n\n\n\n                                              Drag n' Drop your anime here"
const shadersTooltip = "Check the project's GitHub page if you're not sure what to choose"
const encoderTooltip = "Codec for encoding output file. In most cases GPU based are faster, use CPU mainly if you have slow GPU\n" +
	"AV1 is compatible only with RTX 4000+ and RX 6500XT+"
const crfTooltip = "Constant Rate Factor parameter encoder. \nDon't set it too high - file will be very big. " +
	"\n\nCorrect values: 0 - 51 \nIf you don't know what to enter, leave it as 20"
const outputFormatTooltip = "If your input file contains subtitles streams you must use MKV as output format due to other formats limitations"
const compatibilityModeTooltip = "Should be used only for compatibility troubleshooting, disables most of features"
const debugModeTooltip = "Show more detailed logs, useful for troubleshooting and debugging"

var mainPos float32 = 580
var tablePos float32 = 1200
var bottomBarPos float32 = 1310
var bottomProgressPos float32 = 120
var bottomSpeedPos float32 = 110

func loop(window *g.MasterWindow) {
	resolutionsNames := make([]string, len(resolutions))
	for index, res := range resolutions {
		resolutionsNames[index] = res.Format()
	}

	shadersNames := make([]string, len(shaders))
	for index, shader := range shaders {
		shadersNames[index] = shader.Name
	}

	availableEncodersNames := make([]string, len(availableEncoders))
	for index, encoder := range availableEncoders {
		availableEncodersNames[index] = encoder.Name
	}

	g.SingleWindow().Layout(
		g.SplitLayout(g.DirectionHorizontal, &mainPos,
			g.SplitLayout(g.DirectionVertical, &tablePos,
				g.Layout{
					g.Table().Flags(g.TableFlagsResizable).Rows(buildTableRows()...).Columns(buildTableColumns()...),
				},
				g.Layout{
					g.Label("Settings"),
					g.Label(""),

					g.Label("Target resolution"),
					g.Combo("", resolutionsNames[settings.Resolution], resolutionsNames, &settings.Resolution).Size(400),
					g.Label(""),

					g.Label("Shaders"),
					g.Tooltip(shadersTooltip),
					g.Combo("", shaders[settings.Shaders].Name, shadersNames, &settings.Shaders).Size(400),
					g.Tooltip(shadersTooltip),
					g.Label(""),

					g.Label("Encoder"),
					g.Tooltip(encoderTooltip),
					g.Combo("", availableEncoders[settings.Encoder].Name, availableEncodersNames, &settings.Encoder).Size(400),
					g.Tooltip(encoderTooltip),
					g.Label(""),

					g.Label("Constant Rate Factor (CRF)"),
					g.Tooltip(crfTooltip),
					g.InputInt(&settings.Crf).Size(400).OnChange(func() { handleMinMax(&settings.Crf, 0, 0, 51, 51) }),
					g.Tooltip(crfTooltip),
					g.Label(""),

					g.Label("Output format"),
					g.Tooltip(outputFormatTooltip),
					g.Combo("", outputFormats[settings.OutputFormat], outputFormats, &settings.OutputFormat).Size(400),
					g.Tooltip(outputFormatTooltip),
					g.Label(""),

					g.Checkbox("Compatibility mode", &settings.CompatibilityMode),
					g.Tooltip(compatibilityModeTooltip),

					g.Checkbox("Debug mode", &settings.DebugMode),
					g.Tooltip(debugModeTooltip),

					g.Label(""),

					g.Button(buttonLabel).OnClick(handleButton).Size(360, 30),
				},
			),
			g.Layout{
				g.Label("Logs"),
				g.InputTextMultiline(&logs).Flags(g.InputTextFlagsReadOnly).Size(1600, 270),
				g.SplitLayout(g.DirectionVertical, &bottomBarPos,
					g.SplitLayout(g.DirectionVertical, &bottomProgressPos,
						g.Label("Progress: "+totalProgress),
						g.ProgressBar(progress).Overlay(progressLabel).Size(1170, 20),
					),
					g.SplitLayout(g.DirectionVertical, &bottomSpeedPos,
						g.Label(currentSpeed),
						g.Label(eta),
					),
				),
			},
		),
	)

	settings.PositionX, settings.PositionY = window.GetPos()
}

func handleDrop(files []string) {
	if processing {
		return
	}

	ffprobe.SetFFProbeBinPath(".\\ffmpeg\\ffprobe.exe")
	ctx, closeCtx := context.WithTimeout(context.Background(), 5*time.Second)
	defer closeCtx()

LOOP:
	for _, path := range files {
		extension := filepath.Ext(path)
		if !slices.Contains(supportedInput, extension) {
			logMessage(fmt.Sprintf("Invalid input file format (supported: %s)! Path: %s", strings.Join(supportedInput, ", "), path), false)
			continue
		}

		for _, anime := range animeList {
			if anime.Path == path {
				logMessage("File is already added to queue, ignoring it", false)
				continue LOOP
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
			Status:             NotStarted,
		}
		animeList = append(animeList, anime)
		totalProgress = fmt.Sprintf("%d / %d", calcFinished(), len(animeList))
		logMessage("Added file "+path, false)
	}
}

func handleButton() {
	if processing {
		cancelProcessing()
	} else {
		go startProcessing()
	}
}

func resetUI() {
	currentSpeed = "Speed:"
	eta = "ETA:"
	totalProgress = fmt.Sprintf("%d / %d", calcFinished(), len(animeList))
	g.Update()
}

func buildTableRows() []*g.TableRowWidget {
	rows := make([]*g.TableRowWidget, len(animeList))

	for i, anime := range animeList {
		rows[i] = g.TableRow(
			g.Label(fmt.Sprintf("%d", i+1)),
			g.Label(anime.Name),
			g.Label(formatMillis(anime.Length)),
			g.Label(formatMegabytes(anime.Size)),
			g.Label(fmt.Sprintf("%dx%d", anime.Width, anime.Height)),
			g.Label(string(anime.Status)),
			g.Custom(func() { // Workaround for weird UI bug
				g.Button("Remove").Disabled(processing).OnClick(func() { removeAnime(i) }).Build()
			}),
		)
	}

	return rows
}

func buildTableColumns() []*g.TableColumnWidget {
	columns := []*g.TableColumnWidget{
		g.TableColumn("ID").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Title").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(550),
		g.TableColumn("Length").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Size").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Resolution").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Status").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Action").Flags(imgui.TableColumnFlags_WidthFixed).InnerWidthOrWeight(100),
	}

	return columns
}
