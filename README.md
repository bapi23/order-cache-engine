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

- Run only performance tests:
```
./OrderCacheTest [performance]
```

####Example of the output:
```
10000 orders added in: 17399[µs]
1000 Orders removed in: 17248[µs]
100 Removals above quantity performed in: 182[µs]
===============================================================================
All tests passed (20 assertions in 14 test cases)
```


####Callgrind output:
```
==25192== Memcheck, a memory error detector
==25192== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==25192== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==25192== Command: ./OrderCacheTest
==25192== 
10000 orders added in: 338654[µs]
1000 Orders removed in: 258553[µs]
1000 Removals above quantity performed in: 357424[µs]
===============================================================================
All tests passed (20 assertions in 14 test cases)

==25192== 
==25192== HEAP SUMMARY:
==25192==     in use at exit: 0 bytes in 0 blocks
==25192==   total heap usage: 105,974 allocs, 105,974 frees, 255,456,549 bytes allocated
==25192== 
==25192== All heap blocks were freed -- no leaks are possible
==25192== 
==25192== For counts of detected and suppressed errors, rerun with: -v
==25192== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```