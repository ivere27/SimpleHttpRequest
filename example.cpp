#include <iostream>
#include "SimpleHttpRequest.h"

using namespace std;

int main() {
  int r;

  uv_loop = uv_default_loop();


#if 1
  // request.get(url)
  SimpleHttpRequest request(uv_loop);
  request.get("http://192.168.10.9:10509/archive")
  .on("error", [](){
    cerr << endl << "on error" << endl;
  }).on("response", [&request](){
    cout << request.responseBody.str();
  }).end();
#endif

#if 0
  // request(options) GET
  map<string, string> options;
  options["hostname"] = "192.168.10.9";
  options["port"] = "10509";
  options["path"] = "/archive";
  options["method"] = "GET";

  SimpleHttpRequest request(options, uv_loop);
  request.setHeader("content-type","application/json")
  .on("error", [](){
    cerr << endl << "on error" << endl;
  }).on("response", [&request](){
    for (const auto &kv : request.responseHeaders)
      cout << kv.first << " : " << kv.second << endl;

    cout << request.responseBody.str().c_str();
  }).end();
#endif

#if 0
// request.post(url, body) POST
  string body = "{\"archive\":3}";
  SimpleHttpRequest request(uv_loop);
  request.setHeader("content-type","application/json")
  .post("http://192.168.10.9:10509/archive", body)
  .on("error", [](){
    cout << endl << "on error" << endl;
  })
  .on("response", [&request](){
    cout << endl << request.statusCode << endl;
    cout << request.responseBody.str() << endl;
  })
  .end();
#endif


#if 0
// request(options, headers) POST
  map<string, string> options;
  map<string, string> headers;
  options["hostname"] = "192.168.10.9";
  options["port"] = "10509";
  options["path"] = "/archive";
  options["method"] = "POST";
  headers["content-type"] = "application/json";

  SimpleHttpRequest request(options, headers, uv_loop);
  request.on("error", [](){
    cout << endl << "on error" << endl;
  });
  request.on("response", [&request](){
    cout << endl << request.statusCode << endl;
    cout << request.responseBody.str() << endl;
  });
  request.write("{\"archive\":3}");
  request.end();
#endif

  return uv_run(uv_loop, UV_RUN_DEFAULT);
}
