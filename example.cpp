#include <iostream>
#include "SimpleHttpRequest.hpp"

using namespace std;
using namespace request;

const string HOSTNAME = "www.google.com";
const string PORT = "80";
const string PATH = "/";
const string PROTOCOL = "http:";

int main(int argc, char **argv) {

#if 1
  // request.get(url)
  try {
    string url = argc > 1
               ? argv[1]
               : PROTOCOL + "//" + HOSTNAME + ":" + PORT + PATH;

    SimpleHttpRequest request;
    request.timeout = 5000;
    request.get(url)
    .on("error", [](Error&& err){
      cerr << err.name << endl << err.message << endl;
      throw std::runtime_error(err.message);
    }).on("response", [](Response&& res){
      cout << res.str();
    }).end();
  } catch(const std::exception &e) {
    cerr << "exception catched : " << e.what() << endl ;
  }

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
//  { "protocol", "https:" },
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
