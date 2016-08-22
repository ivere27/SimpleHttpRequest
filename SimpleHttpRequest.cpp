#include "SimpleHttpRequest.h"

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

  Client c(options, headers, uv_loop);
  c.on("error", [](){
    cout << endl << "on error" << endl;
  });
  c.on("response", [](){
    cout << endl << "on response" << endl;
  });
  c.write("");
  c.end();

  // options["method"] = "POST";
  // headers["content-type"] = "application/json";

  // Client c(options, headers, uv_loop);
  // c.on("error", [](){
  //   cout << endl << "on error" << endl;
  // });
  // c.on("response", [](){
  //   cout << endl << "on response" << endl;
  // });
  // c.write("{\"archive\":3}");
  // c.end();


  return uv_run(uv_loop, UV_RUN_DEFAULT);
}
