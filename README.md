##Order cache exercise

###Building
- Implmementation doesn't require any dependency
- Catch library is contained in catch2/ folder

Build steps:
```
mkdir build
cd build
cmake -DCMAKE_BUIL_TYPE=Release ..
make -j
```

##Running
- One binary will be created `OrderCacheTest`
- Run all tests including performance ones:
```
./build/OrderCacheTest
```

####Example of the output:
```
10000 orders added in: 17399[µs]
1000 Orders removed in: 17248[µs]
100 Removals above quantity performed in: 182[µs]
===============================================================================
All tests passed (20 assertions in 14 test cases)
```