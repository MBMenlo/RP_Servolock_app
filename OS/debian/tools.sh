################################################################################
# miscelaneous tools
################################################################################

# Added by DM; 2017/10/17 to check ROOT_DIR setting
if [ $ROOT_DIR ]; then 
    echo ROOT_DIR is "$ROOT_DIR"
else
    echo Error: ROOT_DIR is not set
    echo exit with error
    exit
fi

chroot $ROOT_DIR <<- EOF_CHROOT

export DEBIAN_FRONTEND=noninteractive

apt-get -y install dbus udev curl

# Git can be used to share notebook examples
apt-get -y install git

# development tools
apt-get -y install build-essential less vim nano sudo usbutils psmisc lsof
apt-get -y install parted dosfstools

# Python 3

# install PPA
add-apt-repository ppa:deadsnakes/ppa

# update and install
apt update
apt -y install python3.8 python3.8-dev python3.8-venv
update-alternatives --set python3 /usr/bin/python3.8

curl https://bootstrap.pypa.io/get-pip.py | python3.8

#apt-get -y install python3.9 python3.9-pip python3.9-setuptools
pip3 install --upgrade pip

# Meson+ninja build system
pip3 install meson
apt-get -y install ninja-build

# install file system tools
apt-get -y install mtd-utils

# DSP library for C language
# TODO: the package does not exist yet in Ubuntu 16.04
#apt-get -y install libliquid-dev

# Install Midnight commander
apt-get -y install mc 

EOF_CHROOT
