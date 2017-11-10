# libpartikel
Libpartikel is a header only particle system written in C99 with and for [raylib](https://github.com/raysan5/raylib). It is currently in a very early stage (alpha) and thus:

* There will be bugs.
* The API will most certainly change.
* Feature requests and bug tickets are welcome.

**LICENSE: zlib/libpng** (see partikel.h)

## GIFs
Some examples of particle systems made with libpartikel recorded as 60fps GIFs.

Large files.. avoid to watch on mobile :)

* [fountain](https://github.com/dbriemann/expo/blob/master/partikel-fountain.gif)
* [swirl](https://github.com/dbriemann/expo/blob/master/partikel-swirl.gif)
* [flame](https://github.com/dbriemann/expo/blob/master/partikel-flame.gif)

## Dependencies
* [raylib](https://github.com/raysan5/raylib)

## Usage
Just have raylib installed on your system and copy partikel.h to your project and include it.

## Run demo
Note: the cmake is currently only configured for Linux. If you can help with Mac or Windows just submit a pull request.

#### Linux
1. `git clone git@github.com:dbriemann/libpartikel.git`
2. `cd libpartikel && mkdir build && cd build`
3. `cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`
4. `make`
5. `./demo`

#### Windows
You are on your own at the moment, sorry.

#### Mac
You are on your own at the moment, sorry.

## Documentation
Currently the only documentation are the comments in the header file. Also demo.c can be used as inspiration. 

Real documentation will follow as soon as I'm sure that the library will not have further API changes.

## Plans for the future
* Particle system editor built with raygui.
