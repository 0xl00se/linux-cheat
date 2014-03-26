Info about the init process and related concepts such as system V init system.

First user-space process and parent of all processes!

It has PID 1.

It is the first child of the first kernel process, which has PID 0.

Last thing that is run at boot process.

Determines runlevel.

A great way to understand what happens after `init` is to use `pstree`.

This is distribution dependent.

#rc.local

See System V init system.

#run levels

See System V init system.

#init.d

`/etc/init.d` holds System V scripts, which are then symlinked to `/etc/rcX/[SK]XXname` files.

`/etc/init` is Upstart specific.

#System V init system

Ubuntu currently uses both System V init and the newer (backwards compatible) Upstart alternative.

The exact meaning of levels varies based the exact UNIX variant. The most common across distros are:

- 0. Halt
- 1. Single user mode (aka. S or s)
- 6. Reboot

In addition, every distro has a "normal" runlevel for mos of its operation which is either 2 (Ubuntu) or 3 (SUSE, Mac OS X) in most distributions.

Each runlevel is a directory under `/etc` of the form `/etc/rcX.d` containing symlinks to bash scripts with an specific format, all of which should be under `/etc/init.d/`.

Those files are kept under `init`, because all of those scrips are run by the `init` process by forking. This can be observed through commands like `pstree`.

TODO: must all scripts have filenames of type: `[SK]NN`? I have seen that `S` scripts are run before `K`, and then within `SK`, scripts are run by increasing `NN`.

Therefore this is a POSIX 7 supported concept.

The LSB goes further and specifies:

- 0: Halt: Shuts down the system.
- 1: Single-user Mode: Mode for administrative tasks.[2][2]
- 2: Multi-user Mode: Does not configure network interfaces and does not export networks services.[3]
- 3: Multi-user Mode with Networking: Starts the system normally.[4]
- 4: Not used/User-definable: For special purposes.
- 5: Start the system normally with appropriate display manager. ( with GUI ): Same as runlevel 3 + display manager.
- 6: Reboot: Reboots the system.

Get current runlevel (POSIX 7):

    who -r

##init command

Set runlevel to 6, causing the system to reboot:

    #sudo init 6

##update-rc.d

CLI tool that manages symlinks between `/etc/init.d` and `/etc/rcX`.

Prevent a service from starting at startup (remove the symlink):

    sudo update-rc.d apache2 disable

Enable a service:

    sudo update-rc.d apache2 defaults

Set the `<start><stop>` levels of the script script:

    sudo update-rc.d apache2 defaults 21

The above sets start to `2` and stop to `1`.

##checkconfig

Ubuntu install:

    sudo aptitude install -y chkconfig

Show table containing status of each service based on the runlevel:

    chkconfig --list

Allows you to see is a service automatically starts at startup or not.

Sample output:

    acpi-support              0:off  1:off  2:on   3:on   4:on   5:on   6:off
    acpid                     0:off  1:off  2:off  3:off  4:off  5:off  6:off

Show only for a single service:

    chkconfig --list apache2

##initctl

Same as `sudo service --status-all`:

    initctl list

##Ubuntu specific

`/etc/defaults/name` are shell scripts that contain environment variables which can be used by the corresponding init script. Rationale: scripts can be updated without destroying parameters. File in `defaults` are never changed by the package manager.

#init directory

`/etc/init` is Upstart specific. It contains scripts that correspond to Upstart services.

#upstart

Good sources:

- <http://upstart.ubuntu.com/getting-started.html>

    Good intro, but few options commented.

- <http://upstart.ubuntu.com/cookbook/>

    Very good manual.

##service

Upstart interface to init scripts.

Get status of all services:

    sudo service --status-all

Legend:

- `+`: started
- `-`: stopped
- `?`: unknown

Examples:

    sudo service apache2 status
    sudo status apache2

    sudo service apache2 start
    sudo start apache2

    sudo service apache2 stop
    sudo stop apache2

    sudo service apache2 restart
    sudo restart apache2

##script location

Some systems such as Ubuntu use upstart, newer replacement to the `system v` init system

- `/etc/init`: upstart configuration files

	Named services that can be used via:

	    sudo service XXX start

	correspond to:

	    /etc/init/XXX.conf

	files in this directory.

- `/etc/init.d`: classic System V rc dirs

	Upstart is backwards compatible and can also understand:

	    /etc/init.d/XXX

	as service:

	    sudo service XXX start

##start on

Determines event when the startup script will start.

Required to start script automatically at startup: if missing it won't start.

The service must start only once all of its requirements have already done so.

To start after the very basic facilities are set up (e.g. filesystems and networking) the manual recommends either:

    start on runlevel [2345]

    start on (local-filesystems and net-device-up IFACE!=lo)

To start after `/etc/fstab` mountings have been done on Ubuntu 12.04, use:

    start on stopped mountall
