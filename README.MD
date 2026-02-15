# Anime4K-GUI

This application is designed to enhance the resolution of anime videos using the [Anime4K](https://github.com/bloc97/Anime4K) shaders by bloc97, with the ability to save the upscaled version to disk.
It offers a straightforward and user-friendly graphical interface, eliminating the need for command-line interactions.
Under the hood, the application leverages the power of FFMPEG to handle video processing seamlessly. <br>

**Unlike the original Anime4K project, this application allows you to save the enhanced anime as higher-quality video files rather than real-time preview.**

## Table of Contents

- [Features](#features)
- [System Requirements](#system-requirements)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [Building ang FFMPEG Distribution](#building-and-ffmpeg-distribution)
- [Limitations](#limitations)
- [Special Thanks](#special-thanks)
- [Contribute](#contribute)

## Features

- Anime upscaling using the Anime4K shaders.
- Saving upscaled videos to disk rather than real-time preview.
- User-friendly graphical interface for easy operation; no command-line usage is required.
- Compatibility with Windows, Linux and macOS (ARM)
- Support for various video formats: MP4, AVI, and MKV (both input and output).
- Hardware acceleration with NVIDIA (CUDA + NVENC), AMD (OpenCL + AMF).
- Support for most popular video encoders: H.264, H.265, AV1 and FFV1
- HDR compatibility
- Quick and hassle-free installation.

## Requirements and Compatibility

### Requirements
Application requires relatively powerful GPU such as a GTX 970 or newer models. For older hardware, the application will still function, but upscaling may require significantly more time.

You may need some knowledge about various Anime4K shaders modes and codecs properties:
- [Anime4K documentation](https://github.com/bloc97/Anime4K/blob/master/md/GLSL_Instructions_Advanced.md#modes)

### Codecs compatibility table
|       | NVIDIA | AMD | Intel | CPU | Apple M |
|:------|:------:|:---:|:-----:|:---:|:-------:|
| H.264 |   ✅    |  ✅  |   ❌   |  ✅  |  ✅  |
| H.265 |   ✅    |  ✅  |   ❌   |  ✅  |  ✅  |
| AV1   |   ⚠️    |  ⚠️  |   ❌   |  ✅  |  ❌  |

**Hardware accelerated AV1 for NVIDIA and AMD is supported only on RTX 4000+ and RX 7000+ series**

## Usage

Follow these steps to use the Anime4K-GUI:

1. **Download the Latest Release**
    - Visit the [Releases](https://github.com/mikigal/Anime4K-GUI/releases) section on the GitHub repository.
    - Download the latest release zip file.
    - Unzip the downloaded archive and start the application.

2. **Add Your Anime**
    - Drag and drop your input videos into the application's window.

3. **Select Upscaling Settings**
    - Target settings: Output file resolution - keep in mind to select a resolution with an aspect ratio corresponding with input files. Keep in mind that some 4:3 anime may be saved in a 16:9 ratio with included black bars.
    - Shaders mode: Shaders used for upscaling. Each has some positive and negative effects and should be used in different cases. <br>
      **Please read Anime4K [documentation](https://github.com/bloc97/Anime4K/blob/master/md/GLSL_Instructions_Advanced.md#modes) before use.**
    - Encoder: Encoder used for encoding output file. GPU accelerated encoders are much faster than CPU based in most cases <br>
      **AV1 is best pick as it provides the best quality.** <br>
      **AV1 with GPU acceleration is compatible only RTX 4000/RX 7000 and newer GPUs. For older GPUs you can use CPU based AV1 encoder**
    - Constant Rate Factor (**for CPU based encoders**) and Constant Quality (**for GPU based encoders**)- Methods for controlling the output file's bitrate. The lower the value, the better the quality, but the larger the output file.
    - Output format: Format for the output file. Pick MP4 for best support on most devices, MKV for the best compatibility with more complicated files (e.g. multiple streams or subtitles). <br>
    - CPU threads: How many of CPU threads FFMPEG will use. You may limit it to make your system more responsive wile using CPU based encoder.
    - Debug mode: show more detailed logs, useful for troubleshooting and debugging

4. **Upscale the Video**
    - Click the "Start" button to begin the upscaling process.
    - Be patient. You can check speed and current time progress per file in bottom corner.
    - Output files will be saved in the input file's directory with "_upscaled" suffix in the name.

## Troubleshooting

If you encounter issues while using the application, please consult the [Issues](https://github.com/mikigal/Anime4K-GUI/issues) section on the GitHub repository to check for solutions or report problems. <br>
To get more detailed logs enable `Debug Mode` in settings and restart application. <br>
Firstly try using CPU-based encoding. For some reason GPU acceleration may not work correctly with your graphics card. Due to limited access to hardware, most tests were done with NVIDIA graphics cards; compatibility with other GPUs may be worse. If you have compatibility problems with your GPU, please create an issue with application logs and your computer's specifications. <br>
Issues regarding low output quality will be **instantly** closed. Problems like that are related to the original Anime4K shaders - I can recommend trying other shaders mode settings. <br>

## Building and FFMPEG Distribution

Due to GitHub maximum file limit I can not provide FFMPEG's binaries in this repository.

If you want to compile project by yourself you have to download **GPL** FFMPEG build [here](https://github.com/BtbN/FFmpeg-Builds) and put `ffmpeg.exe` and `ffprobe.exe` into `resources/ffmpeg` directory.
Project can be built using MinGW64 UCRT v12 or newer due to Giu requirements

## Limitations
HDR videos are supported only with AV1 output codec

## Special Thanks

- [bloc97](https://github.com/bloc97/Anime4K) for Anime4K shaders.
- [ocornut](https://github.com/ocornut) for Dear ImGui
- [BtbN](https://github.com/BtbN/FFmpeg-Builds) for automated FFMPEG builds.

## Contribute

Contributions are welcome! If you'd like to improve the application or report issues, please create a Pull Request.
<br>
<br>
![Screenshot](/resources/screenshot.png?raw=png)