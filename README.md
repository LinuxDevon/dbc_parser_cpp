# C++ DBC Parser

This is to provide a library header only file to read in DBC files. I was looking around and couldn't
find a simple library that didn't have dependencies. So here we are making one. I got some inspiration
from the python dbc library here: https://pypi.org/project/cantools/

## Testing

I am trying to always make sure that this is very well tested code. I am using Catch2 to do this
testing and if you aren't familiar here is the documentation: https://github.com/catchorg/Catch2/blob/master/docs/Readme.md#top

To run the tests locally you can use the following:
```bash
mkdir build
cd build
cmake ..
make test -j
```

## Building

I am using Cmake to be able to build the tests and the lib. I plan on doing more with it but this is what it
is for now. I am doing developement on the WSL Ubuntu 18.04 kernel. This doesn't mean that IDEs aren't
welcome but the build process might not be suited for this. You will need to modify it for your
needs. Feel free to submit changes so the building process will be more robust.

Here are the steps to get started:
```bash
mkdir build
cd build
cmake ..
make
```

## Scripts

To use the scripts in `scripts/` you will need to install the requirements
from the top level directory.
```bash
pip install -r script-requirements.txt
```

## Contributing

I welcome all help! Please feel free to fork and start some pull requests!
You can see the issues sections for some ideas what might need to be done.