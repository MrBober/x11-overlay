# x11-overlay #
Program to display simple shapes on the screen.

## Installation ##
Download package from releases section and install it with your package manager.

### Debian ###
```
apt install /path/to/package.deb
```

### Arch ###
```
pacman -U /path/to/package.tar.zst
```

### Other distros
For other distributions you need to [build](#building) a package from source.

## Building ##

1. Install dependencies.
For Debian you need to install packages `libx11-6`, `libxcomposite1`, `libxfixes3`, `libxcairo2`, build dependencies `libx11-dev`, `libxcomposite-dev`, `libxfixes-dev` and `libcairo-2`.
For Arch you need to install packages `libx11`, `cairo`, `libxcomposite` and `libxfixes`.
For other distros you need to install packages containing libraries `libX11`, `libXcomposite`, `libXfixes` and `Cairo`.

2. Clone repository and enter directory.
```
git clone https://github.com/MrBober/x11-overlay.git
cd x11-overlay
```

3. Build the program.
```
make
```

4. Install
You may need to run it with elevated priveleges.
```
make install
```

5. Done
The program should be installed.
You can use it by running `overlay`.
