# C Hashtable

To use in your project, include src/hashtable.h in your code.

## Prerequisites for running tests/docs:

* Doxygen
* cmock
* gcov

## To run the tests:

```
make test
```

Coverage metrics are placed in coverage/.

```
open coverage/html/index.html
```

## To build documentation:

```
make doc
```

Documentation is built using Doxygen and placed in doc/. To view the HTML based doc, point your browser to doc/html/index.html:

```
open doc/html/index.html
```

## To build and link against a stub main:

```
make
```

Output binary is placed in bin/.

```
./bin/main
```
