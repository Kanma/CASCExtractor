# CASCExtractor

A command-line tool to extract files from CASC (Content Addressable Storage Container)
storages (used by Blizzard games since 2014).

Works on MacOS X and Linux.


## Dependencies

The following libraries are necessary to build the extractor:

* CascLib (http://www.zezula.net/en/casc/main.html), MIT license
* SimpleOpt 3.4 (http://code.jellycan.com/simpleopt/), MIT License - part of the
  distribution

To download the CascLib submodule, do:

    somewhere$ cd <path/to/the/source/of/CASCExtractor>
    CASCExtractor$ git submodule init
    CASCExtractor$ git submodule update


## Compilation

Requires <a href="http://www.cmake.org/">cmake</a> to build:

    $ mkdir build
    $ cd build
    $ cmake <path/to/the/source/of/CASCExtractor>
    $ make

The executable will be put in build/bin/


## Usage

See `CASCExtractor --help` for more details. All the examples below are shown using the
World of Warcraft: Warlords of Draenor Beta files, as of July 2014.

Note that the CASC format doesn't store the name of the files in plain text, but only in a
hashed version. Thus, a *listfile* is needed if you don't know the exact name of the file
you want to extract. One such *listfile* for WoW:WoD is provided with CascLib in
*CascLib/listfile/listfile-wow6.txt*.


**Extract all the *.M2 files from a CASC storage:**

    $ CASCExtractor -l /path/to/listfile-wow6.txt "/Applications/World of Warcraft Beta/Data/" *.M2
    Opening '/Applications/World of Warcraft Beta/Data'...

    Searching for '*.M2'...

    Found files:
      - Cameras\Abyssal_Maw_CameraFly_01.M2
      - Cameras\Firelands_Bridge_Camera_01.M2
      - Cameras\FlyBy_Maelstrom.M2
      - Cameras\FlyBy_MoguBridge_Collapse.M2
      - Cameras\FlyBy_MoguBridge_Collapse02.M2
      - Cameras\FlyBy_MoguSecret_Door.M2
      - Cameras\FlyBy_MoguSecret_Door02.M2
      - Cameras\FlyByBloodElf.m2
      - Cameras\FlyByDeathKnight.m2
    ....


**Extract a specific file from a CASC storage:**

**IMPORTANT:** Note that the file name is enclosed in "". This is to prevent the shell
to try to interpret the backslashes (\\) as the start of an escape sequence, which would
result in invalid file names.

    $ mkdir out
    $ CASCExtractor -o out "/Applications/World of Warcraft Beta/Data/" "Cameras\Abyssal_Maw_CameraFly_01.M2"
    Opening '/Applications/World of Warcraft Beta/Data'...

    Extracting files...

    $ ls out/
    Abyssal_Maw_CameraFly_01.M2


**Extract some specific files from a CASC storage, preserving the path hierarchy:**

    $ mkdir out
    $ CASCExtractor -f -o out "/Applications/World of Warcraft Beta/Data/" "Cameras\FlyBy_*.M2"
    Opening '/Applications/World of Warcraft Beta/Data'...

    Searching for 'Cameras\FlyBy_*.M2'...

    Found files:
      - Cameras\FlyBy_Maelstrom.M2
      - Cameras\FlyBy_MoguBridge_Collapse.M2
      - Cameras\FlyBy_MoguBridge_Collapse02.M2
      - Cameras\FlyBy_MoguSecret_Door.M2
      - Cameras\FlyBy_MoguSecret_Door02.M2
      - Cameras\FlyByBloodElf.m2
    ...

    Extracting files...

    $ ls out/Cameras/
    FlyBy_Maelstrom.M2
    FlyBy_MoguBridge_Collapse.M2
    FlyBy_MoguBridge_Collapse02.M2
    FlyBy_MoguSecret_Door.M2
    FlyBy_MoguSecret_Door02.M2
    FlyByBloodElf.m2
    ...


## License

CASCExtractor is is made available under the MIT License. The text of the license is in
the file 'LICENSE'.

Under the MIT License you may use CASCExtractor for any purpose you wish, without warranty,
and modify it if you require, subject to one condition:

>   "The above copyright notice and this permission notice shall be included in
>   all copies or substantial portions of the Software."

In practice this means that whenever you distribute your application, whether as binary or
as source code, you must include somewhere in your distribution the text in the file
'LICENSE'. This might be in the printed documentation, as a file on delivered media, or
even on the credits / acknowledgements of the runtime application itself; any of those
would satisfy the requirement.

Even if the license doesn't require it, please consider to contribute your modifications
back to the community.
