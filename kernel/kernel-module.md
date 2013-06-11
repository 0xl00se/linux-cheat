there are things which are hard to do from regular user programs such as
directly talking to hardware

some operations can be done via system calls, but if you want flexibility and
speed, using the kernel ring is fundamental

however:

- it would be very complicated to recompile the kernel and reboot every time you make some modification
- the kernel would be huge if it were to support all possible hardware

modules overcome those two problems exactly because they can be loaded into the kernel
*while it is running* and use symbols that the kernel chooses to export TODO which

it then runs in the same address space as the kernel and with the same permissions
as the kernel (basically do anything)

compiled modules are special object files that have a `.ko` extension instead of `.o`
they also contain module specific metadata

device drivers (programs that enables the computer to talk to hardware)
are one specific type of kernel modules

two devices can map to the same hardware!

# config files

if file it gets read, if dir, all files in dir get read:

    sudo ls /etc/modprobe.d
    sudo ls /etc/modprobe.conf

modules loaded at boot:

    sudo cat /etc/modules

# module-init-tools

## package version

from any of the commands, --version

    modinfo --version

package that provides utilities

## lsmod

list loaded kernel modules

info is taken from `/proc/modules`

    lsmod

sample output:

    cfg80211              175574  2 rtlwifi,mac80211
    ^^^^^^^^              ^^^^^^  ^ ^^^^^^^,^^^^^^^^
    1                     2       3 4       5

1. name
2. size
3. numer of running instances.

	If negative, TODO

4. depends on 1
5. depends on 2

to get more info:

    cat /proc/modules

also contains two more columns:

- status: Live, Loading or Unloading
- memory offset: 0x129b0000

## modinfo

get info about a module by filename or by module name:

    modinfo ./a.ko
    modinfo a

## insmod

loads the module

does not check for dependencies

    sudo insmod

## modprobe

list available modules relative path to `/lib/modules/VERSION/`:

    sudo modprobe -l

load the module:

    sudo modprobe $m

checks for dependencies

load module under different name to avoid conflicts:

    sudo modprobe vmhgfs -o vm_hgfs

remove module

    sudo modprobe -r $m

check if dependencies are ok:

    sudo depmod -a

get info about given `.ko` module file:

    m=a
    sudo rmmod $m

# includes

the default Makefile which is called by the Makefile in this directory automatically
adds `/usr/src/linux-headers-$(CUR_KERNEL_VERSION)/` TODO check

those come directly from the kernel source tree.

# device drivers

devices map to filesystem under `/dev/`. You can get info on them with:

    ls -l /dev

there are three main types of devices:

- block and char

    crw-rw----  1 root tty       7,   1 Feb 25 09:29 vcs1
    brw-rw----  1 root disk      8,   0 Feb 25 09:30 sda
    ^
    type

- b: block
- c: char

the `b` here is my hd.

each partition also gets a b file

## major minor numbers

using `ls -l`:

    crw-rw----  1 root tty       7,   1 Feb 25 09:29 vcs1
                                 ^    ^
                                 1    2

- 1: major number. tells kernel which driver controls the device represented by this file
- 2: minor number. id of each hardware controlled by a given driver

## mouse

you can have some fun with mouses. Search for the mice or mouse device files and cat them:

	sudo cat /dev/input/mice

and then:

	sudo cat /dev/input/mouse0

now note that when you move the mouse, cat spits something out to the screen!

`mice` is the combination of all mice, and each other `mouseX` is a single mouse device.

## mknod

    sudo mknod /dev/coffee c 12 2

makes a char file, major number 12, minor number 2

# hardware communication

talking to hardware always comes down to writting bytes in specific registers
at a given memory addres

some processors implement a single address space for memory and other hardware devices,
and others do not.

however, since x86 is the most popular and it separates addres spaces,
every architecture must at least mimic this separation.

the memory space for non-memory locations is called I/O ports or I/O space.

to use a port, you must first reserve it. To see who reserved what:

	sudo cat /proc/ioports
