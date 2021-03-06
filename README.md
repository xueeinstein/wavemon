![wavemon screenshot](https://cloud.githubusercontent.com/assets/5132989/8640926/1f8436a0-28c6-11e5-9336-a79fd002c324.png)

## Synopsis

wavemon is a wireless device monitoring application that allows you to watch
signal and noise levels, packet statistics, device configuration and network
parameters of your wireless network hardware. It should work (though with
varying features) with all devices supported by the Linux kernel.

See the man page for an in-depth description of operation and configuration.

This version is forked and developed by Bill Xue ([shadowthink.com](https://shadowthink.com)). Main update is that it starts a TCP server (binding to 0.0.0.0:8888) to provide scan data for other applications to access real time APs information.


### Dependencies

wavemon requires a Linux Kernel with wireless extensions enabled. If your Kernel setup uses `CONFIG_CFG80211`, make sure that the config option `CONFIG_CFG80211_WEXT` is set.

In addition, minimally the following are required:
* Netlink `libnl` at least version 3.2,
* including the Generic Netlink support (`libnl-genl`),
* ncurses development files (`libncurses5-dev`),
* the `pkg-config` package.

On Debian/Ubuntu, this can be done using
```bash
	apt-get -y install pkg-config libncurses5-dev libnl-3-dev libnl-genl-3-dev
```

## How to build

wavemon uses `autoconf`, so that in most cases you can simply run
```
	./configure
	make
	sudo make install
```
to build and install the package. Type 'make uninstall' if not happy.
Refer to the file `INSTALL` for generic installation instructions.

To grant users access to restricted networking operations (scan operations), use additionally
```
	sudo make install-suid-root
```
If you have changed some of the autoconf files or use a git version, run
```
	./config/bootstrap
```
(This requires a recent installation of `autotools`.)

## Bugs?

Send bug reports, comments, and suggestions by opening an issue on [github](https://github.com/xueeinstein/wavemon/issues).
