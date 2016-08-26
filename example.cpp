#include <iostream>
#include "SimpleHttpRequest.h"

using namespace std;

int main() {
  int r;

  uv_loop = uv_default_loop();

  map<string, string> options;
  map<string, string> headers;
  options["hostname"] = "192.168.10.9";
  options["port"] = "10509";
  options["path"] = "/archive";
  options["method"] = "GET";
  headers["content-type"] = "application/json";

  SimpleHttpRequest request(options, headers, uv_loop);
  request.on("error", [](){
    cerr << endl << "on error" << endl;
  }).on("response", [&request](){
    for (const auto &kv : request.responseHeaders)
      cout << kv.first << " : " << kv.second << endl;

    cout << request.responseBody.str().c_str();
  }).end();

  // options["method"] = "POST";
  // headers["content-type"] = "application/json";

  // SimpleHttpRequest request(options, headers, uv_loop);
  // request.on("error", [](){
  //   cout << endl << "on error" << endl;
  // });
  // request.on("response", [](){
  //   cout << endl << "on response" << endl;
  // });
  // request.write("{\"archive\":3}");
  // request.end();


  return uv_run(uv_loop, UV_RUN_DEFAULT);
}
