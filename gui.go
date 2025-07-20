package main

import (
	"context"
	"fmt"
	"runtime"
	"time"

	g "github.com/AllenDang/giu"
	"gopkg.in/vansante/go-ffprobe.v2"
)

const shadersTooltip = "Check the project's GitHub page if you're not sure what to choose"
const encoderTooltip = "Codec for encoding output file. In most cases GPU based are faster, use CPU mainly if you have slow GPU\n" +
	"GPU based AV1 is compatible only with RTX 4000+ and RX 7000+\n" +
	"HDR videos are supported only by AV1 codec"
const crfTooltip = "Constant Rate Factor (CRF) for CPU based encoders. \nLower value = better image quality, but larger files." +
	"\n\nValid range: 0 - 51 \n0 = lossless, 18–23 = good quality, 28+ = lower quality." +
	"\n\nIf you're not sure what to enter, try 18 (default)."

const cqTooltip = "Constant Quality (CQ) parameter for NVIDIA NVENC encoder. \nLower values mean better quality but larger files." +
	"\n\nValid range: 0 - 51 \n0 = lossless, 18 = visually near-lossless, 28+ = noticeable compression." +
	"\n\nIf you're not sure what to enter, try 18 (default)"
const outputFormatTooltip = "If your input file contains subtitles streams you must use MKV as output format due to other formats limitations"
const cpuThreadsTooltip = "How many of CPU threads FFMPEG will use \nYou may limit it to make your system more responsive wile using CPU based encoder"
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
					g.Combo("##", resolutionsNames[settings.Resolution], resolutionsNames, &settings.Resolution).Size(400),
					g.Label(""),

					g.Label("Shaders"),
					g.Tooltip(shadersTooltip),
					g.Combo("##", shaders[settings.Shaders].Name, shadersNames, &settings.Shaders).Size(400),
					g.Tooltip(shadersTooltip),
					g.Label(""),

					g.Label("Encoder"),
					g.Tooltip(encoderTooltip),
					g.Combo("##", availableEncoders[settings.Encoder].Name, availableEncodersNames, &settings.Encoder).Size(400),
					g.Tooltip(encoderTooltip),
					g.Label(""),

					crfCqWidget(),

					g.Label("Output format"),
					g.Tooltip(outputFormatTooltip),
					g.Combo("##", outputFormats[settings.OutputFormat], outputFormats, &settings.OutputFormat).Size(400),
					g.Tooltip(outputFormatTooltip),
					g.Label(""),

					threadWidget(),

					g.Checkbox("Debug mode", &settings.DebugMode),
					g.Tooltip(debugModeTooltip),

					g.Label(""),

					g.Button(gui.ButtonLabel).OnClick(handleButton).Size(360, 30),
				},
			),
			g.Layout{
				g.Label("Logs"),
				g.InputTextMultiline(&gui.Logs).Flags(g.InputTextFlagsReadOnly).Size(1600, 270),
				g.SplitLayout(g.DirectionVertical, &bottomBarPos,
					g.SplitLayout(g.DirectionVertical, &bottomProgressPos,
						g.Label("Progress: "+gui.TotalProgress),
						g.ProgressBar(gui.Progress).Overlay(gui.ProgressLabel).Size(1170, 20),
					),
					g.SplitLayout(g.DirectionVertical, &bottomSpeedPos,
						g.Label(gui.CurrentSpeed),
						g.Label(gui.Eta),
					),
				),
			},
		),
	)

	settings.PositionX, settings.PositionY = window.GetPos()
}

func crfCqWidget() g.Layout {
	selectedEncoder := availableEncoders[settings.Encoder]

	if selectedEncoder.CrfSupported {
		return g.Layout{
			g.Label("Constant Rate Factor (CRF)"),
			g.Tooltip(crfTooltip),
			g.InputInt(&settings.Crf).Size(400).OnChange(func() { handleMinMax(&settings.Crf, 0, 0, 51, 51) }),
			g.Tooltip(crfTooltip),
			g.Label(""),
		}
	}

	return g.Layout{
		g.Label("Constant Quality (CQ)"),
		g.Tooltip(cqTooltip),
		g.InputInt(&settings.Cq).Size(400).OnChange(func() { handleMinMax(&settings.Cq, 0, 0, 51, 51) }),
		g.Tooltip(cqTooltip),
		g.Label(""),
	}
}

func threadWidget() g.Layout {
	selectedEncoder := availableEncoders[settings.Encoder]
	if selectedEncoder.Vendor != "cpu" {
		return g.Layout{}
	}

	return g.Layout{
		g.Label("CPU threads"),
		g.Tooltip(cpuThreadsTooltip),
		g.InputInt(&settings.CpuThreads).Size(400).OnChange(func() { handleMinMax(&settings.CpuThreads, 1, 1, int32(runtime.NumCPU()), int32(runtime.NumCPU())) }),
		g.Tooltip(cpuThreadsTooltip),
		g.Label(""),
	}
}

func handleDrop(files []string) {
	if processing {
		return
	}

	ffprobe.SetFFProbeBinPath(".\\ffmpeg\\ffprobe.exe")
	ctx, closeCtx := context.WithTimeout(context.Background(), 5*time.Second)
	defer closeCtx()

	for _, path := range files {
		handleFfprobe(path, ctx)
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
	gui.CurrentSpeed = "Speed:"
	gui.Eta = "ETA:"
	gui.TotalProgress = fmt.Sprintf("%d / %d", calcFinished(), len(animeList))
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
		g.TableColumn("ID").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Title").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(550),
		g.TableColumn("Length").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Size").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Resolution").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Status").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
		g.TableColumn("Action").Flags(g.TableColumnFlagsWidthFixed).InnerWidthOrWeight(100),
	}

	return columns
}
