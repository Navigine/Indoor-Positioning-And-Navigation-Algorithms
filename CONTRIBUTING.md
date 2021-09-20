# <p align="center"> Contributing to Navigine Routing Library </p>

The easiest way to help out is to submit bug reports and feature requests on our [issues](https://github.com/Navigine/Indoor-Routing-Library/issues) page.

When submitting a bug report, please include:

 - The device and operating system version that produced the bug
 - The version or commit of Routing Library that produced the bug
 - Steps required to recreate the issue
 - What happened
 - What you expected to happen

Thanks!

## Development

To develop for the library, it is usually easiest to build and test your changes using either the Mac OS X or Ubuntu desktop targets.

## Code Style

In general, code changes should follow the style of the surrounding code.

When in doubt, you can use the provided clang-format style file for automatic styling.

Install clang-format (available through brew or apt-get):
```
brew install clang-format
```
or
```
sudo apt-get install clang-format
```

Run clang-format with specified style (use -i to modify the contents of the specified file):
```
clang-format -i -style=file [file]
```
