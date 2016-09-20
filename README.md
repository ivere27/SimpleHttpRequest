# SimpleHttpRequest

* c++11 http client based on libuv and http-parser
* a single header file

## Usage
### .get , .post , .put , .del
```cpp
#include <iostream>
#include "SimpleHttpRequest.h"

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



## git clone with submodules.
```bash
git clone --recursive https://github.com/ivere27/SimpleHttpRequest.git
cd SimpleHttpRequest/http-parser && make
cd ../..
cd SimpleHttpRequest/libuv && ./autogen.sh && ./configure && make
```

## example.cpp
```bash
$ g++ example.cpp --std=c++11 \
-I./http-parser/ -I./libuv/include/ \
-lpthread  \
./libuv/.libs/libuv.a \
./http-parser/http_parser.o && DEBUG=1 ./a.out
```

## about
* `This is experimental yet. use at your own purpose!`
* https not supported.
* a project of 'Second Compiler'.

# License

MIT
