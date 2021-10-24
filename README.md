
# ipsys - Ipsys Particle System Yey letS go

## About

Ipsys is a piece of software that focuces on running and displaying cool
randomly generated particule systems.

<img src="pics/001.gif" width="498" height="298" />

It is written in C and GLSL and uses the SDL2 for window management
and OpenGL 4 for running shaders (either for rendering, or for computing
on the many cores of the GPU).

## Build and run

The current build system assumes being run on Linux, with Python3 and GCC
available.

```sh
python3 _comp.py -l
```

Any argument that comes after the `-l` is instead given to the binary.

### Just build

```sh
python3 _comp.py
```

The produced binary is in `bin`.

### Dependencies

- [SDL2](https://www.libsdl.org/)
- [GLEW](http://glew.sourceforge.net/) but only if building with `--use-glew`

## Controls

The ipsys binary can take one command line argument which would then be the number of particle types.

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
- The W key toggles fading (temporary fix for issue #3).

## TODO

- Read a config file.
  - OpenGL version (disable some features if needed).
  - Set settings.
- Full screen setting.
- Wrapping around the edge as a setting.
- Making particles able to changhe their individual size according to new types of rules.
- Add an history of rules to be able to return to previous rules.
- The `main.c` file should be small, not big ><.
- More UI components to tinker with the settings.
- Text rendering for the UI.
- Built-in console for a cool interface.
- Text-based modular data representation format, then add save/load features.
  - Config, setting set, color theme, rules, rule generators, particle positions, all of that in different blocks.
  - Each rule should be independant from the others, and optional.
  - Understandable and modifiable by humans.
  - One shoule be able to share such data with other users.
- Make the build system usable with clang and on Windows.
- View the "force" fields and motion fields.
- Display the rules.
- Better type changing rules and rule generation (using a changing graph).
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
