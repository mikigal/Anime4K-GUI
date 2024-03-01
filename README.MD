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
- [FFMPEG Distribution](#ffmpeg-distribution)
- [Known issues](#known-issues)
- [Special Thanks](#special-thanks)
- [Contribute](#contribute)

## Features

- Anime upscaling using the Anime4K shaders.
- Saving upscaled videos to disk rather than real-time preview.
- User-friendly graphical interface for easy operation; no command-line usage is required.
- Support for most popular video encoders: H.264, H.265, AV1 (CPU based or GPU accelerated)
- Hardware acceleration with NVIDIA (CUDA + NVENC), AMD (OpenCL + AMF), and Intel (QSV) graphics cards.
- Compatibility with multiple audio and subtitles streams
- Support for various video formats: MP4, AVI, and MKV (both input and output).
- Quick and hassle-free installation.

## System Requirements

Before installing and running the application, ensure that your system meets the following requirements:

- Windows 10 or later, 64-bit (While the application may function on older versions, it is neither tested nor officially supported).
- A powerful GPU such as a GTX 970/AMD equivalent, or newer models. For older hardware, the application will still function, but upscaling may require significantly more time.
- Knowledge about various Anime4K shaders modes - please read [documentation](https://github.com/bloc97/Anime4K/blob/master/md/GLSL_Instructions_Advanced.md#modes). This will allow you to get better results of upscaling.

Currently, NVIDIA graphics cards should be most stable as most tests were did with it. <br>
Intel Arc in theory should work but can not be tested due to limited access to hardware. <br>
If you have compatibility problems please create an issue with application logs and your computer's specification. <br>
<br>
In the future we plan to add support for Linux and macOS (Intel and Apple Silicon).

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
   - Compression level: Compression level of the output file. If you don't care about the output file size, always keep this setting to Low. <br>
     **Higher level will cause worse quality**
   - Encoder: Encoder used for encoding output file. If you have low-end graphics card CPU-based encoders may be faster. <br>
     **AV1 is best pick as it provides the best quality.** <br>
     **AV1 with GPU acceleration is compatible only RTX4000/RX 6500XT and newer GPUs Even CPU-based is as fast as H264 with GPU acceleration**
   - Output format: Format for the output file - MP4 has best support on most devices.
   - Compatibility mode: check it only if you have  compatibility issues.
   - Debug mode: show more detailed logs, useful for troubleshooting and debugging

4. **Upscale the Video**
   - Click the "Start" button to begin the upscaling process.
   - Be patient. You can check speed and current time progress per file in bottom corner.
   - Output files will be saved in the input file's directory with "_upscaled" suffix in the name.

## Troubleshooting

If you encounter issues while using the application, please consult the [Issues](https://github.com/mikigal/Anime4K-GUI/issues) section on the GitHub repository to check for solutions or report problems. <br>
To get more detailed logs run application with `--debug` parameter or enable `Debug Mode` in settings. <br>
Firstly try using CPU-based encoding. For some reason GPU acceleration may not work correctly with your graphics card. Due to limited access to hardware, most tests were done with NVIDIA graphics cards; compatibility with other GPUs may be worse. If you have compatibility problems with your GPU, please create an issue with application logs and your computer's specifications. <br>
Issues regarding low output quality will be **instantly** closed. Problems like that are related to the original Anime4K shaders - I can recommend trying other shaders mode settings. <br>


## FFMPEG Distribution

Due to GitHub maximum file limit I can not provide FFMPEG's binaries in this repository. If you want to compile project by yourself you have to download FFMPEG build [here](https://github.com/BtbN/FFmpeg-Builds) and put `ffmpeg.exe` and `ffprobe.exe` into `resources/ffmpeg` directory

## Known issues
1. If your input file contains subtitles streams you must use MKV as output format due to other formats limitations
2. For certain files (primarily MKV files with subtitles), the ffmpeg fails to display progress, ETA, and speed, though upscaling functionality remains operational.

## Special Thanks

- [Ethan](https://github.com/2u75) for most of the required FFMPEG knowledge and research.
- [bloc97](https://github.com/bloc97/Anime4K) for Anime4K shaders.
- [AllenDang](https://github.com/AllenDang/giu) for Giu framework.
- [BtbN](https://github.com/BtbN/FFmpeg-Builds) for automated FFMPEG builds.
- [OpenAI](https://openai.com/) for ChatGPT, which was helpful in creating this README :)

## Contribute

Contributions are welcome! If you'd like to improve the application or report issues, please create a Pull Request.
<br>
<br>
![Screenshot](/resources/screenshot.png?raw=png)