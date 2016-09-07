#include <iostream>
#include "SimpleHttpRequest.h"

const string HOSTNAME = "127.0.0.1";
const string PORT = "10509";
const string PATH = "/archive";

using namespace std;
using namespace request;

int main() {

#if 1
  // request.get(url)
  SimpleHttpRequest request;
  request.timeout = 1000;
  request.get("http://" + HOSTNAME + ":" + PORT + PATH)
  .on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  }).on("response", [](Response&& res){
    cout << res.str();
  }).end();

  return 0;
#endif

#if 0
  uv_loop = uv_default_loop();

  // request.get(url)
  SimpleHttpRequest request(uv_loop);
  request.timeout = 1000;
  request.get("http://" + HOSTNAME + ":" + PORT + PATH)
  .on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  }).on("response", [](Response&& res){
    cout << res.str();
  }).end();

  return uv_run(uv_loop, UV_RUN_DEFAULT);
#endif

#if 0
  uv_loop = uv_default_loop();

  // request(options) GET
  map<string, string> options = {
    { "hostname", HOSTNAME },
    { "port"    , PORT     },
    { "path"    , PATH     },
    { "method"  , "GET"    }
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
#endif

#if 0
  uv_loop = uv_default_loop();

// request.post(url, body) POST
  string body = "{\"archive\":3}";
  SimpleHttpRequest request(uv_loop);
  request.setHeader("content-type","application/json")
  .post("http://" + HOSTNAME + ":" + PORT + PATH, body)
  .on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  }).on("response", [](Response&& res){
    cout << endl << res.statusCode << endl;
    cout << res.str() << endl;
  })
  .end();

  return uv_run(uv_loop, UV_RUN_DEFAULT);
#endif


#if 0
  uv_loop = uv_default_loop();

// request(options, headers) POST write(string)
  map<string, string> options;
  map<string, string> headers;
  options["hostname"] = HOSTNAME;
  options["port"] = PORT;
  options["path"] = PATH;
  options["method"] = "POST";
  headers["content-type"] = "application/json";

  SimpleHttpRequest request(options, headers, uv_loop);
  request.on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  });
  request.on("response", [](Response&& res){
    cout << endl << res.statusCode << endl;
    cout << res.str() << endl;
  });
  request.write("{\"archive\":3}");
  request.end();

  return uv_run(uv_loop, UV_RUN_DEFAULT);
#endif

#if 0
  uv_loop = uv_default_loop();

// request(options, headers) POST write(stream)
  map<string, string> options;
  map<string, string> headers;
  options["hostname"] = HOSTNAME;
  options["port"] = PORT;
  options["path"] = PATH;
  options["method"] = "POST";
  headers["content-type"] = "application/json"; // ignored!!

  stringstream body;
  body << "{\"archive\":3}";
  SimpleHttpRequest request(options, headers, uv_loop);
  request.on("error", [](Error&& err){
    cerr << err.name << endl << err.message << endl;
  });
  request.on("response", [](Response&& res){
    cout << endl << res.statusCode << endl;
    cout << res.str() << endl;
  });
  request.write(body);  //contnet-type = "application/octet-stream"
  request.end();

  return uv_run(uv_loop, UV_RUN_DEFAULT);
#endif
}
