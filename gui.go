package main

import (
	"context"
	"fmt"
	"os"
	"path/filepath"
	"slices"
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
const bitrateTooltip = "Bitrate of output file. \nThe higher the bitrate, the better the quality and the larger the file size. " +
	"\nDon't set it too high - file will be very big. \n\nCorrect values: 0 (same as input file) or 1000-10000Kb/s \n" +
	"If you don't know what to enter, leave it as 0. Invalid value may make quality worse"
const crfTooltip = "Constant Rate Factor parameter encoder. \nDon't set it too high - file will be very big. " +
	"\n\nCorrect values: 0 - 51 \nIf you don't know what to enter, leave it as 20"
const compatibilityModeTooltip = "Should be used only for compatibility troubleshooting, disables most of features"
const debugModeTooltip = "Show more detailed logs, useful for troubleshooting and debugging"

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
		g.SplitLayout(g.DirectionVertical, 580,
			g.SplitLayout(g.DirectionHorizontal, 1200,
				g.Layout{
					g.Table().Flags(g.TableFlagsResizable).Rows(buildTableRows()...).Columns(buildTableColumns()...),
					g.Custom(func() {
						if len(animeList) == 0 {
							g.Label(dragDropLabel).Font(g.AddFont("Calibri", 35)).Build()
						}
					}),
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

					g.Label("Bitrate (Kb/s)"),
					g.Tooltip(bitrateTooltip),
					g.InputInt(&settings.Bitrate).Size(400).OnChange(func() { handleMinMax(&settings.Bitrate, 1000, 0, 20000, 20000) }),
					g.Tooltip(bitrateTooltip),
					g.Label(""),

					g.Label("Constant Rate Factor (CRF)"),
					g.Tooltip(crfTooltip),
					g.InputInt(&settings.Crf).Size(400).OnChange(func() { handleMinMax(&settings.Crf, 0, 0, 51, 51) }),
					g.Tooltip(crfTooltip),
					g.Label(""),

					g.Label("Output format"),
					g.Combo("", outputFormats[settings.OutputFormat], outputFormats, &settings.OutputFormat).Size(400),
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
				g.SplitLayout(g.DirectionHorizontal, 1310,
					g.SplitLayout(g.DirectionHorizontal, 120,
						g.Label("Progress: "+totalProgress),
						g.ProgressBar(progress).Overlay(progressLabel).Size(1170, 20),
					),
					g.SplitLayout(g.DirectionHorizontal, 110,
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

		split := strings.Split(path, string(os.PathSeparator))
		anime := Anime{
			Name:   split[len(split)-1],
			Length: int64(data.Format.DurationSeconds * 1000),
			Size:   file.Size(),
			Width:  data.FirstVideoStream().Width,
			Height: data.FirstVideoStream().Height,
			Path:   path,
			Status: NotStarted,
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
