
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

## TODO

- Read a config file.
  - OpenGL version (disable some features if needed).
  - Support mesa drivers that seem to not support the current `GL_BLEND` use.
  - Set settings.
- Full screen setting.
- Particle size setting.
- Simulation speed setting.
- Add an history of rules to be able to return to previous rules.
- The `main.c` file should be small, not big ><.
- More UI components to tinker with the settings.
- Text rendering for the UI.
- Built-in console for a cool interface.
- Text-based modular data representation format, then add save/load features.
- Make the build system usable with clang and on Windows.
- View the "force" fields.
- Display the rules.
