#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "uv.h"
#include "http_parser.h"

using namespace std;

static uv_loop_t* uv_loop;

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__);
#else
#define LOG(...)
#endif

template<class... T>
using Callback = std::map<std::string, std::function<void(T...)>>;

class Client {
 public:
  Client(map<string, string> &options, map<string, string> &headers, uv_loop_t *loop) {
    uv_loop = loop;

    // FIXME : pointer
    this->options = options;
    this->headers = headers;


    allocCb = [](uv_handle_t* handle, size_t size, uv_buf_t* buf) {
      buf->base = (char*)malloc(size);
      buf->len = size;
    };

    onClose = [](uv_handle_t* handle) {
      LOG("onClose");
      handle->data = NULL;
    };

    http_parser_init(&parser, HTTP_RESPONSE);
    http_parser_settings_init(&parser_settings);
    parser_settings.on_message_begin = [](http_parser* parser) {
      return 0;
    };
    parser_settings.on_url = [](http_parser *p, const char *buf, size_t len) {
      LOG("Url: %.*s\n", (int)len, buf);
      return 0;
    };

    parser_settings.on_header_field = [](http_parser *p, const char *buf, size_t len) {
      LOG("Header field: %.*s\n", (int)len, buf);
      Client *client = (Client*)p->data;
      client->lastHeaderFieldBuf = (char*)buf;
      client->lastHeaderFieldLenth = (int)len;
      return 0;
    };
    parser_settings.on_header_value = [](http_parser *p, const char* buf, size_t len) {
      LOG("Header value: %.*s\n", (int)len, buf);

      Client *client = (Client*)p->data;
      string field = string(client->lastHeaderFieldBuf, client->lastHeaderFieldLenth);
      string value = string(buf, len);

      transform(field.begin(), field.end(), field.begin(), ::tolower);
      client->responseHeaders[field] = value;

      return 0;
    };
    parser_settings.on_headers_complete = [](http_parser *p) {
      LOG("on_headers_complete\n");

      Client *client = (Client*)p->data;
      LOG("%s",client->responseHeaders["Content-Type"].c_str());
      // TODO : make fields to lowcase

      return 0;
    };

    parser_settings.on_body = [](http_parser* parser, const char* buf, size_t len) {
      Client *client = (Client*)parser->data;
      if (buf)
        client->responseBody << string(buf, len);

      //fprintf("Body: %.*s\n", (int)length, at);
      if (http_body_is_final(parser)) {
          LOG("http_body_is_final")
          LOG("%s",client->responseBody.str().c_str());

          client->emit("response");
      } else {
      }

      return 0;
    };

    parser_settings.on_message_complete = [](http_parser* parser) {
      LOG("on_message_complete\n");
      Client *client = (Client*)parser->data;
      ssize_t total_len = client->responseBody.str().size();
      LOG("total_len: %ld\n",total_len)
      if (http_should_keep_alive(parser)) {
          LOG("http_should_keep_alive\n");
          uv_stream_t* tcp = (uv_stream_t*)&client->tcp;
          uv_close((uv_handle_t*)tcp, client->onClose);
      }
      LOG("%d\n",parser->status_code);
      return 0;
    };
  }
  ~Client() {}

  //FIXME : ..Args
  Client& emit(string name) {
    if (eventListeners.count(name))
      eventListeners[name]();

    return *this;
  }

  template <class... Args>
  Client& on(string name, std::function<void()> func) {
    eventListeners[name] = func;

    return *this;
  }


  Client& on(string name, std::function<void()> func) {
    eventListeners[name] = func;

    return *this;
  }

  Client& write(string data) {
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
        Client *client = (Client*)req->data;
        if (status == -1) {
            cerr <<  "connect failed error " << uv_err_name(status);
            uv_close((uv_handle_t*)req->handle, client->onClose);
            return;
        }

        LOG("on_connected");

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
            Client* client = (Client*)tcp->data;
            LOG("onRead %ld\n",nread);
            LOG("buf len: %ld\n",buf->len);
            if (nread > 0) {
              http_parser *parser = &client->parser;
              parsed = (ssize_t)http_parser_execute(parser, &client->parser_settings, buf->base, nread);

              LOG("%ld\n", parsed);
              if (parser->upgrade) {
                LOG("raise upgrade error!!");
              } else if (parsed != nread) {
                LOG("parsed %ld/%ld\n", parsed, nread);
                LOG("%s\n", http_errno_description(HTTP_PARSER_ERRNO(parser)));
              }
            } else {
              if (nread != UV_EOF) {
                cerr <<  "read error " << uv_err_name(nread);
              }
            }

            //LOG(buf->base);

            free(buf->base);
          });

        if (r)
          cerr <<  "uv_write uv_read_start " << uv_err_name(r);

        r = uv_write(&client->write_req, req->handle, &resbuf, 1,
          [](uv_write_t* /*req*/, int status) {
            LOG("after write")
            if (status)
              cerr << "uv_write_t" << uv_err_name(status);
          });

        if (r) {
          cerr <<  "uv_write error " << uv_err_name(r);
        }
      });

    if (r) {
      cerr <<  "uv_tcp_connect error " << uv_err_name(r);
    }
  }

  map<string, string> responseHeaders;
 private:
  uv_loop_t* uv_loop;

  sockaddr_in addr;
  uv_tcp_t tcp;
  uv_connect_t connect_req;
  uv_write_t write_req;

  http_parser_settings parser_settings;
  http_parser parser;
  stringstream responseBody;
  stringstream requestBody;

  uv_alloc_cb allocCb;
  uv_close_cb onClose;

  Callback<> eventListeners;

  map<string, string> options;
  map<string, string> headers;

  char* lastHeaderFieldBuf;
  int lastHeaderFieldLenth;
};