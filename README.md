Introduction
------------
The LibhistoryRPKI is an Open-source C Implementation for Live and Historical 
Prefix Origin Validation of BGP Announcements.

It enables BGP measurement analysis tools to support BGP Prefix Origin 
Validation (RFC 6811). For the live validation the RPKI-RTR protocol (RFC 6810) 
is used. The general validation process is impemented by the RTRLib 
(http://rtrlib.realmv6.org/)


Requirements
------------

 - The RTRLib must be installed: https://github.com/rtrlib/rtrlib


Compilation
-----------

* Build and install the LibhistoryRPKI:

  ```
  $ ./autobuild.sh
  $ ./configure
  $ make
  $ (sudo) make install
  ```

* If RTRLib and/or Includes were not found -> export (default path):

  ```
  $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/x86_64-linux-gnu/
  ```

* If RTRLib and/or Includes were not found -> add -L-I flags:

  ```
  $ ./configure CFLAGS='-L<path_to_librtr.so> -I<path_to_rtrlib_includes>'
  $ make CFLAGS='-L<path_to_librtr.so> -I<path_to_rtrlib_includes>'
  ```

* To change the install path (optional):

  ```
  $ ./configure --prefix=<install_path>
  ```

* To activate debug mode (optional):
  ```
  $ ./configure --enable-debug
  ```


Linking to LibhistoryRPKI
----------------------
The name of the shared library is historyrpki. To link programs to the 
LibhistoryRPKI, pass the following parameter to gcc:

  ```
  -lhistoryrpki
  ```

In case an error such as

  ```
  -/usr/bin/ld: cannot find -lrtr
  -collect2: error: ld returned 1 exit status
  ```

occurs, the location of the library can be passed explicitly as a parameter

  ```
  -L<path_to_libhistoryrpki.so>
  ```

e.g.,

  ```
  -L/usr/local/lib/
  ```


API Documentation
-----------------
The LibhistoryRPKI includes a man file documentation of the API. 

  ```
  $ man libhistoryrpki
  ```
  
Also a doxygen version of the API is available at: doc/doxygen

Contact
-------

Mail: s.al-sheikh@fu-berlin.de

/* vim: set tw=80 sts=2 sw=2 ts=2 expandtab: */
