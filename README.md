
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

### Just build

```sh
python3 _comp.py
```

The produced binary is in `bin`.

## Controls

- The first bar in the ui panel controls the intensity of fading.
- The second bar in the ui panel controls the number of physical iterations performed between each frames.
- Left click repels particles.
- Right click attracts particles.
- The R key randomizes the interaction rules.
- The F key slightly and randomly mutates the interaction rules.
- The C key randomizes the particle types colors.
- The P key randomizes the particle positions.
- The escape key terminates execution (same as closing the window).

## TODO

- Read a config file.
  - OpenGL version (disable some features if needed).
  - Support mesa drivers that seem to not support the current `GL_BLEND` use.
  - Set settings.
- Full screen setting.
- Particle size setting.
- Simulation speed setting.
  - Instead of having n iterations per frame, it could be nice to have n sub-frames per frames (each sub-frame is painted on top of the rendering area).
- Add an history of rules to be able to return to previous rules.
- The `main.c` file should be small, not big ><.
- More UI components to tinker with the settings.
- Text rendering for the UI.
- Built-in console for a cool interface.
- Text-based modular data representation format, then add save/load features.
- Make the build system usable with clang and on Windows.
- View the "force" fields.
- Display the rules.
