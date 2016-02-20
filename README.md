# Windy
## TMX toolset.

The usage is pretty simple, compile with Visual Studio 2015, run and choose among the two options, resize and tmx.

### For splitting:
Specify the matrix that you like for your image to be split and choose a folder.

### For resize:
You must specify the design resolution width that was used to draw the texture you're planning to resize, the resize will be done in a design resolution - 1080p ratio.

1920 / design resolution width.

### For tmx:
Specify the tile size and the texture you're planning to convert into a TMX map.

Clean your alpha channels so that they are rgb(0, 0, 0), otherwise the tool will create
an empty tile for every rgb tile that is completely transparent.

You need to specify a folder because it will split in many tmx maps (this is to help the renderers that have limitations over the amount of tiles that can be drawn, usually it is 128x128 tiles).

### In general:
Windy creates uncompressed files from your texture in your temp directory and file mappings for processing the data, take in consideration that you have enough hard drive space to perform the operations, a 70k x 10k texture will use about 2 GB of hard disk drive for the uncompressed texture and about 1 GB for the tile data operations. Processing of a 20k resolution texture as the former will take about 1 minute to be converted into a tmx map.

About the speed, working with very large textures; as previously specified, is pretty fast, about 1 minute of processing with a core i3 with textures smaller than 20k resolutions.

### Notes:
* This tool uses the Adobe numeric extension for Boost. Included.
* x64 editing tools are recommended for the generated tmx maps.
* You need to compile and run this with an x64 operating system because large file support is really good in x64.
* Avoid indexed textures and always export to RGBA png files.

### Contributions
Any contribution is appreciated, the toolset is completely native, it is free from any API dependency.
If you're willing to port the toolset to another operating system take in consideration the following:

* Boost needs to be implemented with some sort of automated script; by the moment we use precompiled libraries.  
* The numeric extension for Boost was modified so that it could accept large texture dimensions, stick to it, make a patch for the sampler.
* From Boost we use: system, filesystem, and iostreams as compiled libraries, everything else is header-only, like lexical_cast, uuid and gil.
* File dialogs will work with any platform as long as we are able to improve the library.
* C++14 is required.
* x64 is a must. Or have the solution for x86 and memory mapped files.

### TODO:
* Support conversion from indexed and RGB color spaces.

### 3rd parties
* [boost](http://www.boost.org/)
* [nana](http://www.nanapro.org)
* [libpng](http://libpng.org/pub/png/libpng.html)
* [zlib](http://zlib.net/)
* [nfd](https://github.com/mlabbe/nativefiledialog)
* [XmlWriter](http://www.codeproject.com/Articles/5588/Simple-C-class-for-XML-writing)
