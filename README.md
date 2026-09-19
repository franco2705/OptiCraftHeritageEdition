# OptiCraft Heritage — Nintendo Switch Homebrew

OptiCraft Heritage is a clean-room C++ implementation of classic Minecraft-era
gameplay. This repository's primary documented target is a **native Nintendo
Switch Homebrew NRO**, built with devkitA64 and libnx. The port uses native
Switch input, SD-card paths, applet lifecycle handling, and a native framebuffer
backend; it does not run through an emulator.

The project is independently implemented and is not affiliated with, endorsed
by, or sponsored by Mojang Studios or Microsoft.

> **Console safety:** use this software only on a Switch that is already
> Homebrew-enabled. This repository does not provide instructions for modifying
> a console or bypassing platform security.

## What is built

Two Switch presets are intentionally available:

| Preset | Purpose | Output |
| --- | --- | --- |
| `switch-bringup` | Small libnx hardware diagnostic: controller, framebuffer, SD mount, and toolchain smoke test. It does not start the game. | `bin/switch/OptiCraft-bringup.nro` |
| `switch-release` | The full native game target, including the Switch platform backends. | `bin/switch/OptiCraft.nro` |

Both builds produce a Homebrew NRO with distinct artifact names, so building
the diagnostic cannot overwrite the playable NRO. CMake creates its NACP
metadata with `nacptool` and packages the linked ELF with `elf2nro`. The full
target additionally requires Switch zlib for the region/minizip code.

## Requirements

Install these before configuring the project:

- CMake **3.21+** and Ninja.
- Git, including the repository submodules.
- devkitPro's Switch development environment: `devkitA64`, `libnx`, and the
  Switch tools that provide `elf2nro` and `nacptool`.
- `switch-zlib` for `switch-release`. It is not required for `switch-bringup`.

In the devkitPro shell, verify the installation before building:

```bash
echo "$DEVKITPRO"
test -x "$DEVKITPRO/devkitA64/bin/aarch64-none-elf-g++" && echo "devkitA64: OK"
test -f "$DEVKITPRO/libnx/include/switch.h" && echo "libnx: OK"
test -x "$DEVKITPRO/tools/bin/elf2nro" && echo "elf2nro: OK"
test -x "$DEVKITPRO/tools/bin/nacptool" && echo "nacptool: OK"
test -f "$DEVKITPRO/portlibs/switch/lib/libz.a" && echo "switch-zlib: OK"
```

The expected devkitPro root is `/opt/devkitpro` on Linux or `C:/devkitPro` on
Windows. If you installed it elsewhere, set `DEVKITPRO` to its root before
running CMake. The toolchain requires `devkitA64`, `libnx`, and the AArch64 C++
compiler at that location.

### Windows

Use the **devkitPro MSYS** shell, not a generic MSYS2 `CLANG64` shell. In that
shell, `dkp-pacman -Q switch-dev libnx switch-zlib` should list the packages
after installation. If your devkitPro directory is `C:\devkitPro`, the MSYS
shell value is normally:

```bash
export DEVKITPRO=/c/devkitPro
```

## Get the source

Clone the repository and initialise every required third-party dependency:

```bash
git clone --recurse-submodules <repository-url> OptiCraftHeritageEdition
cd OptiCraftHeritageEdition
```

For an existing checkout, use:

```bash
git submodule update --init --recursive
```

## Build the Switch port

### 1. Build the game

Build `switch-release` to start OptiCraft itself. This target contains
`main_switch.cpp`, initializes the native Switch graphics context, and hands
control to `Minecraft::start()`; it is the NRO to use when you want to enter the
game rather than run a hardware test.

```bash
cmake --preset switch-release
cmake --build --preset switch-release
```

Copy the resulting NRO to the SD card:

```text
sdmc:/switch/OptiCraft/OptiCraft.nro
```

### 2. Stage runtime data

The NRO does **not** embed the game's loose runtime data. For a playable full
target, provide the following source directories yourself:

```text
data/assets/
data/resources/
```

Then stage them next to the game NRO:

```bash
cmake --build build/switch-release --target switch-data
```

This creates the host-side deployment layout:

```text
bin/switch/
├── OptiCraft.nro
└── data/
    ├── assets/
    └── resources/
```

Copy its contents to the SD card so the final layout is:

```text
sdmc:/switch/OptiCraft/
├── OptiCraft.nro
├── data/
│   ├── assets/
│   └── resources/
└── .minecraft/              # created for saves and options
```

The Switch resource backend reads `sdmc:/switch/OptiCraft/data`, and the
client-policy backend stores saves/options at
`sdmc:/switch/OptiCraft/.minecraft`.

### Optional: hardware bring-up diagnostic

Use this only to diagnose the toolchain, controller, framebuffer, or SD card;
it intentionally does not enter the game:

```bash
cmake --preset switch-bringup
cmake --build --preset switch-bringup
```

It produces `bin/switch/OptiCraft-bringup.nro`, which may safely coexist with
the playable `OptiCraft.nro` on the SD card.

The diagnostic reports both Horizon's native SD-filesystem result and whether
the libnx `sdmc:` devoptab can be opened. It deliberately does **not** call
`fsdevMountSdmc()` itself: libnx's default runtime has already initialized FS
and mounted `sdmc:` before `main()` starts, and a second mount can produce a
spurious libnx error. If native SD access succeeds but devoptab access fails,
record the displayed `errno` and check the Switch/libnx environment rather
than treating a repeated-mount result as an SD-card failure.

> The `data/assets` and `data/resources` directories are not present in every
> source checkout. `switch-data` can only copy data you have supplied; it cannot
> generate missing game assets.

## Customise the Homebrew Menu entry

Set metadata at configure time. `SWITCH_ICON` must point to an existing
256×256 JPEG:

```bash
cmake --preset switch-release \
  -DSWITCH_TITLE="OptiCraft Heritage" \
  -DSWITCH_AUTHOR="Your name" \
  -DSWITCH_VERSION="1.0.0" \
  -DSWITCH_ICON="/absolute/path/to/icon.jpg"
cmake --build --preset switch-release
```

The packaging target validates the icon path and passes these values to
`nacptool`/`elf2nro`.

## Optional network deployment

If `nxlink` is installed and the console is listening, send the packaged NRO
directly with:

```bash
cmake --build --preset switch-bringup --target switch-nxlink
```

`switch-nxlink` is created only when the tool is available.

## Troubleshooting

| Error | Fix |
| --- | --- |
| `devkitPro Switch SDK not found` | Set `DEVKITPRO` to the directory containing `devkitA64` and `libnx`, then configure again. |
| `aarch64-none-elf-g++ not found` | Install the devkitA64 component of devkitPro. |
| `elf2nro and nacptool are required` | Install the devkitPro Switch tools. |
| `Switch full game requires switch-zlib` | Install `switch-zlib`, then rerun the `switch-release` configure command. |
| `switch-data` fails | Add the missing `data/assets` and `data/resources` directories before staging. |

These checks are enforced by the Switch toolchain and build configuration.

## Other targets

The repository also retains PC, PlayStation 2, and Nintendo Wii code paths.
Their presets are listed in `CMakePresets.json`; Switch instructions above are
the supported primary workflow in this README.

## Third-party software

Third-party libraries are located under `external/` and retain their respective
licenses and notices. Review those licences before redistributing binaries.
