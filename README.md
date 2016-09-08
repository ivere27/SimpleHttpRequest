# SimpleHttpRequest

a project of 'Second Compiler'.

* c++11 http client based on libuv and http-parser only
* one header file

This is experimental yet.


* git clone with submodules.
```bash
git clone --recursive https://github.com/ivere27/SimpleHttpRequest.git
cd SimpleHttpRequest/http-parser && make
cd ../..
cd SimpleHttpRequest/libuv && ./autogen.sh && ./configure && make
```

* test the example.cpp
```bash
$ g++ example.cpp --std=c++11 \
-I./http-parser/ -I./libuv/include/ \
-lpthread  \
./libuv/.libs/libuv.a \
./http-parser/http_parser.o && DEBUG=1 ./a.out
```

# License

MIT
