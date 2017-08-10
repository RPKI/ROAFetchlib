Introduction
------------
The ROAFetchlib is an Open-source C Implementation for Live and Historical 
Prefix Origin Validation of BGP Announcements.

It enables BGP measurement analysis tools to support BGP Prefix Origin 
Validation (RFC 6811). For the live validation the RPKI-RTR protocol (RFC 6810) 
is used. The general validation process is impemented by the RTRLib 
(http://rtrlib.realmv6.org/)


Requirements
------------

 - RTRLib: http://rtrlib.realmv6.org
  
 - Wandio: https://research.wand.net.nz/software/libwandio.php
 
 - Doxygen (optional)
  
 Compilation
-----------

* Install the RTRLib (with SSH option) / Wandio

* If a Lib was not installed in the default path -> Add to LD_LIBRARY_PATH and CPPFLAGS:

  ```
  $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"<path_to_librtr.so>"
  $ export CPPFLAGS=$CPPFLAGS"<path_to_rtrlib_includes>"
  ```
  
* Build and install the ROAFetchlib:

  ```
  $ ./autobuild.sh
  $ ./configure
  $ make
  $ (sudo) make install
  ```

* If a Lib and/or the Includes were not found -> add -L -I flags:

  ```
  $ ./configure CFLAGS='-L<path_to_librtr.so> -I<path_to_rtrlib_includes>'
  $ make
  ```

* To change the install path (optional):

  ```
  $ ./configure --prefix=<install_path>
  ```

* To activate debug mode (optional):
  ```
  $ ./configure --enable-debug
  ```


Linking to ROAFetchlib
----------------------
The name of the shared library is roafetch. To link programs to the 
ROAFetchlib, pass the following parameter to gcc:

  ```
  -lroafetch
  ```

In case an error such as

  ```
  -/usr/bin/ld: cannot find -lrtr
  -collect2: error: ld returned 1 exit status
  ```

occurs, the location of the library can be passed explicitly as a parameter

  ```
  -L<path_to_roafetchlib.so>
  ```

e.g.,

  ```
  -L/usr/local/lib/
  ```


API Documentation
-----------------
The ROAFetchlib includes a man file documentation of the API. 

  ```
  $ man roafetchlib
  ```
  
Also a doxygen version of the API is available at: doc/doxygen

Contact
-------

Mail: s.al-sheikh@fu-berlin.de

/* vim: set tw=80 sts=2 sw=2 ts=2 expandtab: */
