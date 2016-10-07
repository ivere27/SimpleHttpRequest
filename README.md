# SimpleHttpRequest

* c++11 http client based on libuv, http-parser and openssl
* a single header file
* http/https supports

## Usage
### .get , .post , .put , .del
```cpp
#include <iostream>
#include "SimpleHttpRequest.hpp"

using namespace std;
using namespace request;

int main() {
  SimpleHttpRequest request;
  request.get("http://www.google.com")
  .on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  }).on("response", [](Response&& res){
    cout << res.str();
  }).end();

  return 0;
}
```
```cpp
string body = "{\"hello\": \"world\"}";
SimpleHttpRequest request;
request.setHeader("content-type","application/json")
.post("http://example.org:8080/", body)
.on("error", [](Error&& err){
  cerr << err.name << endl << err.message << endl;
}).on("response", [](Response&& res){
  cout << endl << res.statusCode << endl;
  cout << res.str() << endl;
})
.end();
```

## with options, setHeader
```cpp
uv_loop = uv_default_loop();

map<string, string> options = {
  { "hostname", "google.com" },
  { "port"    , "80"         },
//{ "protocol", "https:"     },
  { "path"    , "/"          },
  { "method"  , "GET"        }
};

SimpleHttpRequest request(options, uv_loop);
request.setHeader("content-type","application/json")
.on("error", [](Error&& err){
  cerr << err.name << endl << err.message << endl;
}).on("response", [](Response&& res){
  for (const auto &kv : res.headers)
    cout << kv.first << " : " << kv.second << endl;

  cout << res.str();
}).end();

return uv_run(uv_loop, UV_RUN_DEFAULT);
```

## with options, headers, timeout
```cpp
uv_loop = uv_default_loop();

map<string, string> options;
map<string, string> headers;
options["hostname"] = "example.org";
options["port"] = "80";
//options["protocol"] = "https:";
options["path"] = "/";
options["method"] = "POST";
headers["content-type"] = "application/json";

SimpleHttpRequest request(options, headers, uv_loop);
request.timeout = 1000;
request.on("error", [](Error&& err){
  cerr << err.name << endl << err.message << endl;
});
request.on("response", [](Response&& res){
  cout << endl << res.statusCode << endl;
  cout << res.str() << endl;
});
request.write("{\"hello\":42}");
request.end();

return uv_run(uv_loop, UV_RUN_DEFAULT);
```


## build & test
### git clone with submodules.
```bash
git clone --recursive https://github.com/ivere27/SimpleHttpRequest.git
cd SimpleHttpRequest
cd http-parser && make
cd ..
cd libuv && ./autogen.sh && ./configure && make
cd ..
# cd openssl && ./config && make
```

### example.cpp - http
```bash
$ make
```
### example.cpp - https
ENABLE_SSL macro (about 2.5MB will be added or $ strip a.out )
```bash
$ g++ example.cpp --std=c++11 \
-I./http-parser/ -I./libuv/include/ -I./openssl/include/ \
./libuv/.libs/libuv.a \
./http-parser/http_parser.o \
./openssl/libssl.a ./openssl/libcrypto.a \
-lpthread -ldl \
-DENABLE_SSL \
&& DEBUG=* ./a.out https://www.google.com
```

## about
* `This is experimental yet. use at your own purpose!`
* binary test
```bash
$ ./a.out http://www.google.com/images/nav_logo242.png > a.png
$ file a.png
a.png: PNG image data, 167 x 410, 8-bit/color RGBA, non-interlaced
```
* a project of 'Second Compiler'.

# License

MIT
