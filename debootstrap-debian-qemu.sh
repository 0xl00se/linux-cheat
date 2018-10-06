#!/usr/bin/env bash

# https://unix.stackexchange.com/questions/275429/creating-bootable-debian-image-with-debootstrap/473256#473256

set -eux

debootstrap_dir=debootstrap
root_filesystem=img.ext2.qcow2

sudo apt-get install \
  debootstrap \
  libguestfs-tools \
  qemu-system-x86 \
;

if [ ! -d "$debootstrap_dir" ]; then
  # Create debootstrap directory.
  # - linux-image-amd64: downloads the kernel image
  sudo debootstrap \
    --include linux-image-amd64 \
    stretch \
    "$debootstrap_dir" \
    http://deb.debian.org/debian/ \
  ;
fi

linux_image="$(printf "${debootstrap_dir}/boot/vmlinuz-"*)"

if [ ! -f "$root_filesystem" ]; then

  # Set root password.
  echo 'root:root' | sudo chroot "$debootstrap_dir" chpasswd

  # Remount root filesystem as rw.
  # Otherwise, systemd shows:
  #     [FAILED] Failed to start Create Volatile Files and Directories.
  # and then this leads to further failures in the network setup.
  cat << EOF | sudo tee "${debootstrap_dir}/etc/fstab"
/dev/sda / ext4 errors=remount-ro,acl 0 1
EOF

  # Network.
  # We use enp0s3 because the kernel boot prints:
  #     8139cp 0000:00:03.0 enp0s3: renamed from eth0
  # This can also be observed with:
  #     ip link show
  # Without this, systemd shows many network errors, the first of which is:
  #     [FAILED] Failed to start Network Time Synchronization.
  cat << EOF | sudo tee "${debootstrap_dir}/etc/network/interfaces.d/00mytest"
auto lo
iface lo inet loopback
auto enp0s3
iface enp0s3 inet dhcp
EOF

  # Generate image file from debootstrap directory.
  # Leave 1Gb extra empty space in the image.
  sudo virt-make-fs \
    --format qcow2 \
    --size +1G \
    --type ext2 \
    "$debootstrap_dir" \
    "$root_filesystem" \
  ;
  sudo chmod 666 "$root_filesystem"
fi

linux_img="${debootstrap_dir}/boot/vmlinuz-"*

# Build the Linux kernel.
git clone --depth 1 --branch v4.18 git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
cd linux
wget https://gist.githubusercontent.com/cirosantilli/6e2f4975c1929162a86be09f839874ca/raw/6d151d231a233408a6e1b541bf4a92fd55bf5338/.config
make olddefconfig
make -j`nproc`
cd -
linux_img="linux/arch/x86_64/boot/bzImage"

qemu-system-x86_64 \
  -append 'console=ttyS0 root=/dev/sda' \
  -drive "file=img.ext2.qcow2,format=qcow2" \
  -enable-kvm \
  -serial mon:stdio \
  -m 2G \
  -kernel="$linux_img"
  -device rtl8139,netdev=net0 \
  -netdev user,id=net0 \
;
