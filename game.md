#Console

##nethack

Most famous dungeon crawler.

Save is only pause: no return from death.

Many things kill you for totally unpredictable reasons.

##fortune

Tells you fortune to stdout!

    fortune

##cowsay

An ASCII art cow echoes stdin:

    echo a | cowsay
    echo b | cowsay

Combo with fortune:

    fortune | cowsay
    fortune | cowsay

##moon-buggy

Jump over craters and shoot forward:

    moon-buggy

##robotfindskitten

Cute!

    robotfindskitten

##bsdgames

Lots of console games/cute apps.

Highly recommended.

List games on Ubuntu:

    apt-file show bsdgames | grep /usr/games/

Review top 10: <http://techtinkering.com/2009/08/11/my-top-10-classic-text-mode-bsd-games/>

###adventure games

Computer RPGs.

Read the description of places and things.

Type what to do and where to go.

####battlestar

####adventure

####phantasia

####battlestar

###backgammon

###number

Convert number in numerals to number in English:

    assert [ `echo 1 | number` = "one." ]

###pom

Displays the phase of the moon.

###primes

Print primes between 1 to 100:

    primes 1 100

Count primes:

    primes 1 100 | wc -l

`factor` is not included here as it is part of coreutils.

###robots

Very simple, fun, but too much luck.

Play:

    robots

Play with better settings:

    alias robots="robots -ta`for i in {1..10000}; do echo -n n; done`"

###atc

Nice timing memory.

`E1 A0` means: plane E1, will land at airport 0

    atc

List scenarios and leave:

    atc -l

Play a scenario:

    atc -g crossover

Cannot pause...

###hack

Nethack predecessor.

    hack

###hunt

Multiplayer shooter.

Looks *very* promising, but multiplayer only, and no online mode.

##gnugo

CLI go.

#netreck

2D non terminal spaceship mouse clicking online multiplayer classic.

#gnome

##golly

Conway's game of life simulator

Because the global menu does not work, deactivate it on ubuntu:

    env UBUNTU_MENUPROXY=0 golly

##gnomine

Minesweeper clone

    gnomine

##gnotski

Knotski clone.

    gnotski

No, Knotski has nothing to do with KDE =)

##four-in-a-row

Also known as connect four.

Warning: this games has been solved, so the program might do perfect play =)

#kde

##kreversi

Reversi for KDE.

#fps

##urban terror

CS like, but mostly capture the flag.

Good inertia, not too fast.

Free but closed source.

##world of padman

FPS, very large scenarios, cool weapons.

Too fast for my taste.

#clones

##pingus

Great lemmings clone.

Includes map designer.

##hedgewars

Great worms clone, has same feeling. Includes map builder.

##super maryo chronicles

Super mario bros clone.

##Tux Kart

Mario Kart 64 clone.

#strategy

##battle for wesnoth

Turn based unit control on 2d hexagonal map.

Very addictive and time consuming.

Open source, but could be a professionally made game: graphics, music and gameplay.

Includes a map designer.

#emulation

##NES

###FCEUX

##SNES

###zsnes

Features:

- fast forward
- states
- good ROM support

##Nintendo 64

###mupen64plus

Features:

- fast forward
- states

Config only via text file: `~/.config/mupen64plus/`.

Defaults emulator:

    0-9       Select virtual 'slot' for save/load state (F5 and F7) commands
    F5        Save emulator state
    F7        Load emulator state
    F9        Reset emulator
    F10       slow down emulator by 5%
    F11       speed up emulator by 5%
    F12       take screenshot
    Alt-Enter Toggle between windowed and fullscreen
    p or P    Pause on/off
    m or M    Mute/unmute sound
    g or G    Press "Game Shark" button (only if cheats are enabled)
    / or ?    single frame advance while paused
    F         Fast Forward (playback at 250% normal speed while F key is pressed)
    [         Decrease volume
    ]         Increase volume

Defaults controller:

    Analog Pad              Arrow Keys (left, right, down, up)
    C Up/Left/Down/Right    "I", "J", "K", "L"
    DPad Up/Left/Down/Right "W", "A", "S", "D"
    Z trigger               "z"
    Left trigger            "x"
    Right trigger           "c"
    Start                   "Enter" ("Return")
    A button                "left shift"
    B button                "left control"
    Select Mempack          ","
    Select Rumblepack       "."

Keyboard: <http://code.google.com/p/mupen64plus/wiki/KeyboardSetup>

Usage:

    mupen64plus rom.v64

Full screen:

    alias mupen="mupen64plus --fullscreen"
    mupen rom.v64

##ps1

###pcsxr

##dosbox

Some good games there

    cd
    mkdir dos
    dosbox game.exe

there are also `.bat` and `.com` executables

Inside the emulator

    mount c /home/$USER/dos
    c:
    dir
        #ls
    cd game
    game.exe

To avoid mounting every time, put under the `[autoexec]` section:

    echo -e "mount c /home/$USER/dos\nc:" >> ~/.dosbox/dosbox-*.conf
