
# ipsys - Ipsys Particle System Yey letS go

## About

Ipsys is a piece of software that focuces on running and displaying cool
randomly generated particule systems.

<img src="pics/001.gif" width="498" height="298" />

It is written in C and GLSL and uses the SDL2 for window management
and OpenGL 4 for running shaders (either for rendering, or for computing
on the many cores of the GPU).

## Build and run

The current build system assumes being run on Linux with Python 3.

```sh
python3 _comp.py
```

Supported compilers are GCC (`--compiler=gcc` which is the default) and Clang (`--compiler=clang`).

Add the `-l` option *at the end* means to execute the binary if compilation succeeded. Any argument that comes after the `-l` is instead given to the binary as its command line arguments.

Use the `--help` option to get a list of options.

### Dependencies

- [Python 3](https://www.python.org/) for the build system
- [SDL2](https://www.libsdl.org/) (maybe read [this](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php))
- [GLEW](http://glew.sourceforge.net/) but only if building with `--use-glew`

## Controls

At runtime, the ways to interact with the simulation and rendering are listed here:

- The first bar in the ui panel controls the intensity of fading.
- The second bar in the ui panel controls the number of physical iterations performed between each frames.
- Left click repels particles.
- Right click attracts particles.
- The R key randomizes the interaction rules.
- The F key slightly and randomly mutates the interaction rules.
- The C key randomizes the particle types colors.
- The P key randomizes the particle positions.
- The escape key terminates execution (same as closing the window).
- The T key switches between the rendering of each iteration and each frame.
- The W key toggles fading.
- The S key saves the current rule set to a file.

## Command line interface

- `-t` followed by a number sets the number of particle types.
- `-f` followed by a file path load the rule set described in the given file.

## TODO

- Text rendering for the UI.
- More UI components to tinker with the settings.
- Make the text-based modular data representation format even better.
  - Config, setting set, color theme, rules, rule generators, particle positions, all of that in different blocks.
  - Each rule should be independant from the others, and optional.
  - Understandable and modifiable by humans.
  - One should be able to share such data with other users.
  - Better code (with error report, and without vulnerabilities).
- Changelog and version numbers.
- Read a config file.
  - OpenGL version (disable some features if needed) (support Apple).
  - Set settings.
- Full screen setting.
- Wrapping around the edge as a setting.
- Making particles able to changhe their individual size according to new types of rules.
- Add an history of rules to be able to return to previous rules.
- The `main.c` file should be small, not big ><.
- Built-in console for a cool interface.
- Configurable interface and scripting.
  - Script the action of the cursor on particles with a custom scripting language.
  - Script the action of user-created UI witgets on the rules, rule generators, patricles, colors, etc.
  - Create UI witgets with scripting.
  - A console line can be a witget.
- Make the build system usable on Windows.
- View the "force" fields and motion fields.
- Display the rules.
- Better type changing rules and rule generation (using a changing graph).
- Rule generation and modification UI.
  - A rule should be the result (at the root) of a tree of modifications (add some interval constant, multiply with some simple function, add these two sub-rules, etc.).
  - Each of these modifications should have a UI component allowing for visualization and tinkering.
- Fix memory leaks when hitting Q.
- Better interaction abilities with the particles.
- Adding walls, wind zones, pipes, etc.
- Adding links and linking rules.
  - Two particles may be linked, which means they will stay close but not too close.
  - The exact behavior of links and how they appear/disappear are the rules called linking rules.
- Dynamic number of particles.
- Dynamic number of particle types.
- Better rule set mutation.
  - Add a slider (or multiple sliders) that, in a continuous manner, changes some parameter that in turns changes some rules.
  - For example, the curve describing the attraction rule between some pair of particle types gets added the values of a curve that is null everywere except in an interval where its value and edges are continuous functions of a slider.
- Built-in video/gif recorder.
- Website/server to upload/download shared rules, configs and scripts.
- Put some features in separated libraries with some clean APIs.
  - And maybe make binding to C++/Python/Rust/whatever.
- Music visualization mode.
  - Having the beat/melodies of a music influence the particles/colors/rules.
- Accept shaders from outside, provided by the user.
- Make it possible to have some dynamic library be used in place of scripts (to make it faster or to allow to call some neat C functions directly or something).
- Multiple particle rendering shaders (square, circle, something else).
- Bloom effect.
- Background cool color effects maybe.
- Optimize even more.
- Support for MacOS/Windows.
- Run in web browser (compile to wasm, use WebGL, etc.).
- Better build system command line interface.
- Better command line interface in general.
- Make a cooler gif or even a video to promote the project.
- Make adding a shader easier (with even more code generation).
- Add debugging features like function calls tracing, verbose option, etc.
- Precomile the shaders to SPIR-V or something.
- Expand the TODO list, it is not long enough xd.
- Be able to compile without the SDL2 library (use Xorg, Wayland, WinAPI, whatever, but keep SDL2 as an optional dependancy).
- More types of interaction laws.
  - Boids-like laws.
  - Friction laws.

### Rerwite it in Rust ?

Actually why not one day~
