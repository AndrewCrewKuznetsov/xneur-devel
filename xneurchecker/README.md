# xneurchecker 

Command line utility for demonstrating how xneur works. It can be used in pipe with other utilities.

## Synopsis
```
xneurchecker - manual text checker based on xneur library (version ) 
usage: xneurchecker [options] "<word_1> <...word_n>"
  where options are:

  -h, --help            This help!
  -l, --layout          Get probable layouts.
  -w, --word            Get probable translate.
```

## Example of usage
    $ xneurchecker Ghbdtn! Вот ghbvth работы чтугксруслук
    Привет! Вот пример работы xneurchecker

## Compiling and installing
```
$ mkdir xneur-devel/xneurchecker/build
$ cd xneur-devel/xneurchecker/build
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr
$ make
$ sudo make install
```
