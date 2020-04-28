[![Build Status](https://travis-ci.org/bauman/libgeohash.svg?branch=master)](https://travis-ci.org/bauman/libgeohash)
![C/C++ CI](https://github.com/bauman/libgeohash/workflows/C/C++%20CI/badge.svg?branch=master)
![Python package](https://github.com/bauman/libgeohash/workflows/Python%20package/badge.svg?branch=master)

libgeohash
==========

-------------------

A library used for encoding/decoding geohashes.

libgeohash was originally developed by Derek Smith @ simplegeo as a BSD licensed reference implementation for the Public Domain [Geohash algorithm](https://en.wikipedia.org/wiki/Geohash) invented in 2008 by Gustavo Niemeyer 

pylibgeohash has been added under the same license as the original 

To use libgeohash just run 
```
cmake .
make
```

to build and test the python wrapper
```
   python3 setup.py bdist_wheel
   pip3 install dist/pylibgeohash*
```


###Installation

Linux and Mac users can install the package via pip.

```
pip install libgeohash
```


### Encode

c
```
char* hash =  geohash_encode(double lat, double lng, int precision);
<do stuff>
free(hash);
```

python 
```python
import pylibgeohash as geohash
hash = geohash.geohash_encode(lat, long, precision)

```

Takes in latitude and longitude with a desired precision and returns the correct hash value. If
precision < 1 or precision > 12, a default value of 6 will be used.

### Decode
c
```
GeoCoord coord = geohash_decode(char* hash);
coord.latitude;
coord.longitude;
```
python 
```
import pylibgeohashas as geohash
coord = geohash.geohash_decode("ezs42")
coord["latitude"] 
coord["longitude"]
```
Produces an allocated GeoCoord structure which contains the latitude and longitude that was decoded from
the geohash. A GeoCoord also provides the bounding box for the geohash (north, east, south, west).

### Neighbors
c
```
char** geohash_neighbors(char* hash);
```

Uses the bounding box declared at hash and calculates the 8 neighboring boxes. An example is show below.

+ ezefx ezs48 ezs49
+ ezefr ezs42 ezs43
+ ezefp ezs40 ezs41

The value returned is an array of char* with length of 8. The neighboring positions of values are shown 
below with each box representing the index of the array.

+ 7 0 1
+ 6 * 2
+ 5 4 3


### Similar works 
reponame | license | codebase  | install | link
--- | --- | --- | --- | --- 
pygeohash | AGPL | pure python | `pip install pygeohash` |  https://github.com/wdm0006/pygeohash
Geohash | AGPL | pure python | `pip install geohash` | https://github.com/vinsci/geohash/
geohash2 | AGPL | pure python | `pip install geohash2` | https://github.com/DBarthe/geohash
python-geohash | Apache2 & MIT & BSD | python & c++ | `pip install python-geohash` | https://github.com/hkwi/python-geohash
libgeohash | MIT | pure python | `pip install libgeohash | https://github.com/bashhike/libgeohash
geohashcx | GPLv3 | python + ffi to static bin | `pip install geohashcx` | https://github.com/aldnav/geohash   
pylibgeohash | BSD | c | `pip install pylibgeohash` | https://github.com/bauman/libgeohash
