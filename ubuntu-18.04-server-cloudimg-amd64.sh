#!/usr/bin/env bash

set -eux

# Install dependencies.
pkgs='cloud-image-utils qemu'
if ! dpkg -s $pkgs >/dev/null 2>&1; then
  sudo apt-get install $pkgs
fi

# Get the image.
# This is already in qcow2 format.
img=ubuntu-18.04-server-cloudimg-amd64.img
if [ ! -f "$img" ]; then
  wget "https://cloud-images.ubuntu.com/releases/18.04/release/${img}"

  # sparse resize: does not use any extra space, just allows the resize to happen later on.
  # https://superuser.com/questions/1022019/how-to-increase-size-of-an-ubuntu-cloud-image
  qemu-img resize "$img" +128G
fi

# Set the password.
# https://serverfault.com/questions/920117/how-do-i-set-a-password-on-an-ubuntu-cloud-image
# https://askubuntu.com/questions/507345/how-to-set-a-password-for-ubuntu-cloud-images-ie-not-use-ssh
user_data=user-data.img
if [ ! -f "$user_data" ]; then
  cat >user-data <<EOF
#cloud-config
password: asdfqwer
chpasswd: { expire: False }
ssh_pwauth: True
EOF
  cloud-localds "$user_data" user-data
fi

qemu-system-x86_64 \
  -drive "file=${img},format=qcow2" \
  -drive "file=${user_data},format=raw" \
  -device rtl8139,netdev=net0 \
  -enable-kvm \
  -m 2G \
  -netdev user,id=net0 \
  -serial mon:stdio \
  -smp 2 \
  -vga virtio \
;
