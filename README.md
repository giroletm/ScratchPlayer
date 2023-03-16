# ScratchPlayer

A half-broken C++ software to play simple [Scratch](https://scratch.mit.edu/) games offline using [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2).

## Usage

Download the [latest release](https://github.com/giroletm/ScratchPlayer/releases/latest) of the software or [compile](#compiling) it yourself.

Then, acquire the ``.sb3`` file of whatever Scratch project you want to play, and drag-and-drop it onto the software's executable.

(Alternatively, you can do this on your favourite terminal, by passing the path of the ``.sb3`` file as the first argument of the command.)

## Limitations

* Blocks from the "data", "procedures" and "argument" category aren't supported
* Custom blocks aren't supported
* Backgrounds and sprites that have text won't render it, due to [SDL2_image](https://github.com/libsdl-org/SDL_image/tree/SDL2) limitations
* A **lot** of heavy projects won't run properly or will lag

## Compiling

### Prerequisites

To compile this software, you'll need:
* [Visual Studio 2022](https://visualstudio.microsoft.com/fr/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSFeaturesPage&passive=true&tailored=cplus&cid=2031#cplusplus) ([future-proof link to older versions](https://visualstudio.microsoft.com/fr/vs/older-downloads/)), older versions weren't tested but should work with some edits
* Desktop C++ Development tools for Visual Studio 2022 (Check the corresponding checkbox in the Visual Studio Installer)

### Step 1 - Compiling libraries

[SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.24.1), [SDL2_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.6.2) and [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.20.1) are included as compiled binaries in this repo.
However, for obvious safety reasons, it is always recommended to compile these by yourself.
For this, check the instructions from the corresponding repos.

``zlib`` and ``libzip`` need to be compiled by hand, though. For this:
* Open ``zlib``'s solution file (``Libs/zlib-win-build/build-VS2022/zlib.sln``) with Visual Studio 2022
* Build the solution with the targets you're interested in (``Debug``/``Release`` for ``Win32``/``x64``)
* Repeat the process with ``libzip``'s solution file (``Libs/libzip-win-build/build-VS2022/libzip.sln``), with the **same targets** as ``zlib``
* It is required for you to build ``zlib`` first, as ``libzip`` relies on ``zlib`` to build itself.

### Step 2 - Compiling the main program

Once that libraries are compiled, open the solution file located at the root of the repo with Visual Studio 2022 and try to build the solution with the **same targets** as ``zlib`` and ``libzip``.

You should be able to find the output executable at the root of the repo, in folders having names matching your targets.

Add the DLLs from your builds in the same folder and it should run.
If you're struggling with that, I've uncluded a DLLs folder containing all of them, sorted by target.
Do note that again, it is always recommended to use your own DLLs, as using ones from strangers could compromise your computer.

## Notes

This project is widely unfinished!

I've started it a few months ago but eventually lost the motivation to finish it, but I'm publishing it in the hopes of it being useful to someone.

## Licensing

Please refer to the [LICENSE](https://github.com/giroletm/ScratchPlayer/blob/main/LICENSE) file.

## Credits

[libsdl](https://github.com/libsdl-org/) for [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2), [SDL2_image](https://github.com/libsdl-org/SDL_image/tree/SDL2) and [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/tree/SDL2).

[nlohmann](https://github.com/nlohmann) and his contributors for [json library](https://github.com/nlohmann/json)

[Mark Adler](https://madler.net/madler/), [Jean-loup Gailly](http://gailly.net/) and their contributors for [zlib](https://github.com/madler/zlib)

[Dieter Baron](https://github.com/dillof), [Thomas Klausner](https://github.com/0-wiz-0) and their contributors for [libzip](https://github.com/nih-at/libzip/)

[Kelvin Lee](https://github.com/kiyolee) for his [zlib-win-build](https://github.com/kiyolee/zlib-win-build) and [libzip-win-build](https://github.com/kiyolee/libzip-win-build) repositories
