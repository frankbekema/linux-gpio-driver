# linux-gpio-driver
A GPIO driver written in C for linux, works with Raspberry PI and Arch Linux. 
This driver makes use of the sysfs interface to use GPIO on Arch Linux with a Raspberry PI.
The driver could work on other systems but it hasn't been tested.

## Requirements
- Linux
- GPIO sysfs interface


### Packages
- make

## Building
All commands should be executed in the ``src`` directory.

For shared and static libraries you can execute the make command:
```
make
```

To only build the static library execute:
```
make static
```

To only build the shared library execute:
```
make shared
```

Besides this you can always include the source files in your C application.

## References
The library is based on the reference from kernel.org
- [https://www.kernel.org/doc/Documentation/gpio/sysfs.txt](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt)

## License
This project is licensed under the [MIT license](https://opensource.org/licenses/MIT)

## Questions?
If you have any questions feel free to contact Frank Bekema on frankbekema10@gmail.com or open up an issue.
