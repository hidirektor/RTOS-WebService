#include <zephyr.h>
#include <net/socket.h>
#include <net/http.h>

#define STACKSIZE 2048
#define PRIORITY 7

struct http_response {
    const char *payload;
    size_t payload_len;
};

static char __stack http_stack[STACKSIZE];

static void http_server(void)
{
    struct sockaddr_in server_addr;
    int sock, new_sock;
    int ret;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printk("Error creating socket\n");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printk("Error binding socket\n");
        return;
    }

    ret = listen(sock, 1);
    if (ret < 0) {
        printk("Error listening on socket\n");
        return;
    }

    while (1) {
        new_sock = accept(sock, NULL, NULL);
        if (new_sock < 0) {
            printk("Error accepting connection\n");
            return;
        }

        struct http_response res = {
                .payload = "Hello World!",
                .payload_len = sizeof("Hello World!") - 1
        };

        ret = send(new_sock, &res, sizeof(res), 0);
        if (ret < 0) {
            printk("Error sending response\n");
        }

        close(new_sock);
    }
}

K_THREAD_DEFINE(http_id, STACKSIZE, http_server, NULL, NULL, NULL,
PRIORITY, 0, K_NO_WAIT);
