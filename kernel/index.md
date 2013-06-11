the kernel does the most fundamental operations such as:

- **user permission control**

    the kernel determines what programs can do or not,
    enforcing for example file permissions

- **virtual address space**

     all programs see is a virtual address space from 0 to a max size,
     even if the physical memory may be split in a complex way in the physical RAM

     if they try write out of this space, the kernel termiates them,
     so that they don't mess up with other process memory

     this also increases portability across different memory devices and architectures.

- **filesystem**

     the kernel abstracts individual filesystems into a simple directory file tree
     easily usable by any program, without considering the filesystem type or
     the hardware type (hd, flash device, floppy disk, etc)

- **concurrence**

     the kernel schedules programs one after another quite quickly and in a smart way,
     so that even users with a single processor can have the impression that they are
     running multiple applications at the same time, while in reality all they are doing
     is switching very quicly between applications.

therefore it reaches general goals such as:

- increasing code portability across different hardware and architectures
- creating useful and simple abstractions which programs can rely on
	(contiguous RAM memory, files, processes, user permissions, etc)

the linux kernel is written on mainly on c c99 standard,
with **gasp** gcc extensions. Therefore the linux kernel is married to gcc.
Just guessing here, but an important reason for that is to use inline assembly TODO check

also note that besides the linux kernel, what most people call a linux system, or more precisely
a linux distribution, must also contain many more user level basic services such as the python
interpreter, the X server, etc. The extra user space services are specified by the lsb, and are not a part
of the linux kernel.

You cannot use user space libs such as libc to program the kernel,
since the kernel itself itself if need to be working for user space to work.

# user programs

User programs such as a simple hello world run inside an abstraction called *process* defined by the kernel.

The kernel restricts what user programs can do directly basically to basic processor operations (adding)
and memory operations (setting or getting RAM memory)

User programs can, however, ask the kernel to do certain operations for them via *system calls*.

A simple example is the c `printf` function, which must at some point ask the kernel to print to `stdout`,
probably via the `write` system call.

Another simple example is file io.

## floating point

you cannto use floating point operations on kernel code because that would incur too much overhead
of saving floating point registers on some architectures, so don't do it.

## memory

memory is much more restrictive than in user applictions, so be extra cautious to use little memory

# rings

x86 implemented concept

programs can run in different rings

4 rings exist

linux uses 2:

- 0: kernel mode
- 3: user mode

this is used to separate who can do what

# version number

- rc = Release Candidate

# test a new kernel

## compile

this is a very slow test mechanism since you need to reboot everytime.

get the source:

	git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

clean everything:

	make mrproper

generate the `.config` file:

	make menuconfig

this opens up a ncurses interface which allows you to choose amongst tons of options
which determine which features your kernel will include or not.

then go on to `save` to save to the `.config` file and then exit

build:

	make -j5

`-j` tells make to spawn several process, which is useful if you have a multicore processor.
it is recommend to use:

	n = number of processors + 1

this may take more than one hour.

## install

tested on Ubuntu `13.04` with kernel dev version `3.10.0-rc5+`

	sudo make modules_install -j5
	sudo make install -j5

this will place:

- the compiled kernel under `/boot/vmlinuz-<version>`
- config file `.config` as `/boot/config-<version>`
- `System.map` under `/boot/System.map-<version>`.

	This contains symbolic debug information.

- `/lib/modules/<version>/` for the modules

now reboot, from the GRUB menu choose "Advanced Ubuntu options",
and then choose the newly installed kernel.

TODO how to go back to the old kernel image by default at startup?
	going again into advance options and clicking on it works,
	but the default is still the newer version which was installed.

## kernel module

can be inserted and removed while the kernel runs.

However, if you make an error at startup (dereference null pointer for example),
the kernel module may become impossible to reinsert without a reboot.
<http://unix.stackexchange.com/questions/78858/cannot-remove-or-reinsert-kernel-module-after-error-while-inserting-it-without-r/>

## kernel virtual machine

# get kernel version

    uname -r

    cat /proc/version

# sysctl

view/config kernel parameters at runtime

    sudo sysctl –a

# kernel coding conventions

- tabs instead of spaces. Configure editors to view tabs as 8 spcaes. In `vim` you could source:

               if expand('%:e') =~ '\(c\|cpp\|f\)'
                    set noexpandtab
                    set tabstop=8
                    set shiftwidth=8
               endif

     the 8 space rule is needed when we want to make ascii tables and align each column at a multiple of the tab width
     so that it is easier to write the table.

     For example, if a tab has 8 spaces then only one tab is need for:

          123456     c2
          c1          c2
          c1          c2

     but two tabs would be needed for:

          123456789     c2
          c1               c2
          c1               c2

     if the tab was sees as say, 4 spaces, the first example would look ugly:

          123456     c2
          c1     c2
          c1     c2

# kernel source tree

- `arch`: architecture specific code. Ex: `x86`, `sparc`, `arm`
- `include`: headers which may be useful for using kernel parameters and functions on user programs or kernel modules TODO confirm

	for kernel modules, those are automatically appended to the `cpp` include search path by the default makefile
