#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>

#include "uv.h"
#include "http_parser.h"

using namespace std;

static uv_loop_t* uv_loop;


#define LOGE(...) LOGI(__VA_ARGS__, __LINE__, __PRETTY_FUNCTION__)       // FIXME : to stderr?
void _LOGI(){}
template <typename T, typename ...Args>
void _LOGI(T t, Args && ...args)
{
    if (getenv("SECC_LOG")) {
    try {
      std::ofstream logFile;
      logFile.open(getenv("SECC_LOG"), std::ios::out | std::ios::app);
      logFile << std::forward<T>(t);
      logFile.close();
    } catch(const std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  } else
    std::cout << std::forward<T>(t);

  _LOGI(std::forward<Args>(args)...);
}
template <typename T, typename ...Args>
void LOGI(T t, Args && ...args)
{
  if (!getenv("DEBUG")) return;

  _LOGI("[" + ::to_string(getpid()) + "] ");  // insert pid
  _LOGI(std::forward<T>(t));
  _LOGI(std::forward<Args>(args)...);
  _LOGI('\n');
}


template<class... T>
using Callback = std::map<std::string, std::function<void(T...)>>;

class SimpleHttpRequest {
 public:
  SimpleHttpRequest(map<string, string> &options, map<string, string> &headers, uv_loop_t *loop) {
    uv_loop = loop;

    // FIXME : pointer
    this->options = options;
    this->headers = headers;


    allocCb = [](uv_handle_t* handle, size_t size, uv_buf_t* buf) {
      buf->base = (char*)malloc(size);
      buf->len = size;
    };

    onClose = [](uv_handle_t* handle) {
      LOGI("onClose");
      handle->data = NULL;
    };

    http_parser_init(&parser, HTTP_RESPONSE);
    http_parser_settings_init(&parser_settings);
    parser_settings.on_message_begin = [](http_parser* parser) {
      return 0;
    };
    parser_settings.on_url = [](http_parser *p, const char *buf, size_t len) {
      LOGI("Url: ", string(buf,len));
      return 0;
    };

    parser_settings.on_header_field = [](http_parser *p, const char *buf, size_t len) {
      LOGI("Header field: ", string(buf,len));
      SimpleHttpRequest *client = (SimpleHttpRequest*)p->data;
      client->lastHeaderFieldBuf = (char*)buf;
      client->lastHeaderFieldLenth = (int)len;
      return 0;
    };
    parser_settings.on_header_value = [](http_parser *p, const char* buf, size_t len) {
      LOGI("Header value: ", string(buf,len));

      SimpleHttpRequest *client = (SimpleHttpRequest*)p->data;
      string field = string(client->lastHeaderFieldBuf, client->lastHeaderFieldLenth);
      string value = string(buf, len);

      transform(field.begin(), field.end(), field.begin(), ::tolower);
      client->responseHeaders[field] = value;

      return 0;
    };
    parser_settings.on_headers_complete = [](http_parser *p) {
      LOGI("on_headers_complete");

      SimpleHttpRequest *client = (SimpleHttpRequest*)p->data;
      //LOGI(client->responseHeaders["Content-Type"].c_str());
      // TODO : make fields to lowcase

      return 0;
    };

    parser_settings.on_body = [](http_parser* parser, const char* buf, size_t len) {
      SimpleHttpRequest *client = (SimpleHttpRequest*)parser->data;
      if (buf)
        client->responseBody.rdbuf()->sputn(buf, len);

      //fprintf("Body: %.*s\n", (int)length, at);
      if (http_body_is_final(parser)) {
          LOGI("http_body_is_final");
          //LOGI(client->responseBody.str().c_str());

          client->emit("response");
      } else {
      }

      return 0;
    };

    parser_settings.on_message_complete = [](http_parser* parser) {
      LOGI("on_message_complete");
      SimpleHttpRequest *client = (SimpleHttpRequest*)parser->data;
      ssize_t total_len = client->responseBody.str().size();
      LOGI("total_len: ",total_len);
      if (http_should_keep_alive(parser)) {
          LOGI("http_should_keep_alive");
          uv_stream_t* tcp = (uv_stream_t*)&client->tcp;
          uv_close((uv_handle_t*)tcp, client->onClose);
      }
      LOGI("status code : ", ::to_string(parser->status_code));
      return 0;
    };
  }
  ~SimpleHttpRequest() {}

  //FIXME : ..Args
  SimpleHttpRequest& emit(string name) {
    if (eventListeners.count(name))
      eventListeners[name]();

    return *this;
  }

  template <class... Args>
  SimpleHttpRequest& on(string name, std::function<void()> func) {
    eventListeners[name] = func;

    return *this;
  }


  SimpleHttpRequest& on(string name, std::function<void()> func) {
    eventListeners[name] = func;

    return *this;
  }

  SimpleHttpRequest& write(string data) {
    //FIXME : send directly later when tcp is open
    requestBody << data;

    return *this;
  }

  void end() {
    int r = uv_ip4_addr(options["hostname"].c_str(), stoi(options["port"]), &addr);
    //printf("%d\n",r);

    r = uv_tcp_init(uv_loop, &tcp);
    tcp.data = this;          //FIXME : use one
    connect_req.data = this;
    parser.data = this;

    r = uv_tcp_connect(&connect_req, &tcp, reinterpret_cast<const sockaddr*>(&addr),
      [](uv_connect_t *req, int status) {
        SimpleHttpRequest *client = (SimpleHttpRequest*)req->data;
        LOGI(status);
        if (status != 0) {
            uv_close((uv_handle_t*)req->handle, client->onClose);
            LOGE(uv_err_name(status), uv_strerror(status));
            client->emit("error" );
            return;
        }

        LOGI("TC connection established to ",
          client->options["hostname"].c_str(),
          ":",client->options["port"].c_str());

        uv_buf_t resbuf;
        string res = client->options["method"] + " " + client->options["path"] + " " + "HTTP/1.1\r\n";
        for (const auto &kv : client->headers) {
          res += kv.first + ":" + kv.second + "\r\n";
        }
        if (client->requestBody.str().size() > 0) {
          res += "Content-Length: " + ::to_string(client->requestBody.str().size()) + "\r\n";
          res += "\r\n";
          res += client->requestBody.str();
        } else {
          res += "\r\n";
        }
        resbuf.base = (char *)res.c_str();
        resbuf.len = res.size();

        int r = uv_read_start(req->handle, client->allocCb,
          [](uv_stream_t *tcp, ssize_t nread, const uv_buf_t * buf) {
            ssize_t parsed;
            SimpleHttpRequest* client = (SimpleHttpRequest*)tcp->data;
            LOGI("onRead ", nread);
            LOGI("buf len: ",buf->len);
            if (nread > 0) {
              http_parser *parser = &client->parser;
              parsed = (ssize_t)http_parser_execute(parser, &client->parser_settings, buf->base, nread);

              LOGI("parsed: ", parsed);
              if (parser->upgrade) {
                LOGI("raise upgrade error!!");
              } else if (parsed != nread) {
                LOGI("parsed ", parsed,"/", nread);
                LOGI("%s\n", http_errno_description(HTTP_PARSER_ERRNO(parser)));
              }
            } else {
              if (nread != UV_EOF) {
                LOGE("read error ", uv_err_name(nread));
              }
            }

            //LOGI(buf->base);

            free(buf->base);
          });

        if (r)
          LOGE("uv_write uv_read_start ", uv_err_name(r));

        r = uv_write(&client->write_req, req->handle, &resbuf, 1,
          [](uv_write_t* /*req*/, int status) {
            LOGI("after write");
            if (status)
              LOGE("uv_write_t ", uv_err_name(status));
          });

        if (r) {
          LOGE("uv_write error ", uv_err_name(r));
        }
      });

    if (r) {
      LOGE("uv_tcp_connect error ", uv_err_name(r));
    }
  }

  map<string, string> responseHeaders;
  stringstream responseBody;
 private:
  uv_loop_t* uv_loop;

  sockaddr_in addr;
  uv_tcp_t tcp;
  uv_connect_t connect_req;
  uv_write_t write_req;

  http_parser_settings parser_settings;
  http_parser parser;
  stringstream requestBody;

  uv_alloc_cb allocCb;
  uv_close_cb onClose;

  Callback<> eventListeners;

  map<string, string> options;
  map<string, string> headers;

  char* lastHeaderFieldBuf;
  int lastHeaderFieldLenth;
};