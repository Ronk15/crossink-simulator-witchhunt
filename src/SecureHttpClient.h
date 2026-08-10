#pragma once
// Shim de crosspoint::SecureHttpClient para firmware derivado de Witchhunt.
// En el dispositivo esto corre sobre wolfSSL con raices curadas. En escritorio
// se apoya en el HTTPClient del simulador, que a su vez usa curl del anfitrion
// con el almacen de certificados del sistema.
#include <HTTPClient.h>
#include <NetworkClient.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace crosspoint {

class SecureHttpClient {
public:
  using BodySink = std::function<bool(const uint8_t *data, size_t len)>;
  using ProgressFn = std::function<bool(size_t downloaded, size_t total)>;

  SecureHttpClient() = default;
  ~SecureHttpClient() { close(); }
  SecureHttpClient(const SecureHttpClient &) = delete;
  SecureHttpClient &operator=(const SecureHttpClient &) = delete;

  void setCACert(const char *rootCA) { _rootCA = rootCA; }
  void setAllowInsecureFallback(bool allow) { _allowInsecureFallback = allow; }
  void setTimeout(uint32_t ms) { _timeoutMs = ms; }
  void setUserAgent(const std::string &ua) { _userAgent = ua; }
  void setBasicAuth(const std::string &user, const std::string &pass) {
    _user = user;
    _pass = pass;
  }
  void addHeader(const std::string &name, const std::string &value) {
    _headers.push_back(name + ": " + value);
  }
  void clearHeaders() { _headers.clear(); }
  void setMaxRedirects(int n) { _maxRedirects = n; }
  void setAllowRedirectDowngrade(bool allow) { _allowRedirectDowngrade = allow; }

  int request(const char *method, const std::string &url,
              const std::string &body = "") {
    _body.clear();
    HTTPClient http;
    NetworkClient client;
    http.begin(client, url.c_str());
    http.setTimeout((uint16_t)_timeoutMs);
    for (const auto &h : _headers) {
      const size_t colon = h.find(": ");
      if (colon != std::string::npos)
        http.addHeader(h.substr(0, colon).c_str(), h.substr(colon + 2).c_str());
    }
    const int rc = http.sendRequest(method, body.c_str());
    _status = rc;
    _lastInsecure = url.rfind("https://", 0) != 0;
    if (rc > 0) _body = http.getString().s;
    http.end();
    return rc;
  }

  int GET(const std::string &url) { return request("GET", url); }
  int POST(const std::string &url, const std::string &b) {
    return request("POST", url, b);
  }
  int PUT(const std::string &url, const std::string &b) {
    return request("PUT", url, b);
  }

  int get(const std::string &url, const BodySink &sink,
          const ProgressFn &progress = nullptr) {
    const int rc = request("GET", url);
    if (rc > 0 && !_body.empty()) {
      if (progress && !progress(0, _body.size())) return ERR_ABORTED;
      if (sink && !sink(reinterpret_cast<const uint8_t *>(_body.data()),
                        _body.size()))
        return ERR_ABORTED;
      if (progress && !progress(_body.size(), _body.size())) return ERR_ABORTED;
    }
    return rc;
  }

  const std::string &getBody() const { return _body; }
  int lastStatus() const { return _status; }
  bool lastConnectionWasInsecure() const { return _lastInsecure; }
  size_t lastHandshakeMinFree() const { return (size_t)-1; }
  size_t lastHandshakeMinLargest() const { return (size_t)-1; }
  void close() { _body.clear(); }

  enum SecureHttpError {
    ERR_BAD_URL = -1,
    ERR_CONNECT = -2,
    ERR_SEND = -3,
    ERR_TIMEOUT = -4,
    ERR_TOO_MANY_REDIRECTS = -5,
    ERR_ABORTED = -6,
    ERR_TRUNCATED = -7,
  };

private:
  const char *_rootCA = nullptr;
  bool _allowInsecureFallback = false;
  bool _allowRedirectDowngrade = false;
  uint32_t _timeoutMs = 10000;
  int _maxRedirects = 5;
  int _status = 0;
  bool _lastInsecure = false;
  std::string _userAgent, _user, _pass, _body;
  std::vector<std::string> _headers;
};

} // namespace crosspoint
