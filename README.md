MinerPP
===========

High performance C++11 cryptographic currency mining library

Dependencies:

boost

To compile run the build script for your platform or run bjam manually.

CPU mining example:

```
./minerpp --work-hosts=stratum+tcp://username:password@71.43.223.221:3032,stratum+tcp://uasername:password@vnl.suprnova.cc:1111 --device-cores=0 --device-type=cpu --work-algorithm=whirlpoolxor
```

Thank you for your support.
