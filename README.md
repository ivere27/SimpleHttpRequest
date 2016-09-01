# SimpleHttpRequest

a project of 'Second Compiler'.

c++11 http client based on libuv and http-parser

This is experimental yet.


* make https://github.com/libuv/libuv
* make https://github.com/nodejs/http-parser
* then (replace the paths with your ones),
```bash
$ clang++ example.cpp --std=c++11 \
-I../http-parser/ -I../libuv/include/ \
-lpthread  \
../libuv/.libs/libuv.a \
../http-parser/http_parser.o && DEBUG=1 ./a.out
```

# License

MIT