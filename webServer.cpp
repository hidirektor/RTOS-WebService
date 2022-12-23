#include <zephyr.h>
#include <net/socket.h>
#include <net/http.h>

#define STACKSIZE 2048
#define PRIORITY 7

class WebServer {
 public:
  WebServer() {
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ < 0) {
      printk("Error creating socket\n");
      return;
    }

    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(80);
    server_addr_.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(sock_, (struct sockaddr *)&server_addr_, sizeof(server_addr_));
    if (ret < 0) {
      printk("Error binding socket\n");
      return;
    }

    ret = listen(sock_, 1);
    if (ret < 0) {
      printk("Error listening on socket\n");
      return;
    }
  }

  void run() {
    while (1) {
      int new_sock = accept(sock_, NULL, NULL);
      if (new_sock < 0) {
        printk("Error accepting connection\n");
        return;
      }

      struct http_response res = {
        .payload = "Hello World!",
        .payload_len = sizeof("Hello World!") - 1
      };

      int ret = send(new_sock, &res, sizeof(res), 0);
      if (ret < 0) {
        printk("Error sending response\n");
      }

      close(new_sock);
    }
  }

 private:
  int sock_;
  struct sockaddr_in server_addr_;
};

K_THREAD_DEFINE(http_id, STACKSIZE, WebServer::run, NULL, NULL, NULL,
                PRIORITY, 0, K_NO_WAIT);

int main() {
  WebServer server;
  server.run();

  return 0;
}
