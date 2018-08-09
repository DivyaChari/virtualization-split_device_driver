#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kv.h"
#include "kvparam.pb-c.h"

int host_get(char *key, void *value, size_t *value_len) 
{
    struct sockaddr_in addr;
  
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(5555);

    Kvdata__Kvparam kv = KVDATA__KVPARAM__INIT; 
    void *send_buf;
    unsigned send_len;

    kv.operation = 1;
    kv.key = key;
    send_len = kvdata__kvparam__get_packed_size(&kv);
  
    send_buf = malloc(send_len);
    kvdata__kvparam__pack(&kv,send_buf);

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    int socklen = sizeof(addr);
    sendto(sock, send_buf, send_len, 0, (struct sockaddr *)&addr, socklen);

    Kvdata__Kvparam *kv_reply;
    uint8_t rcv_buf[MAX_MSG_SIZE];
    struct sockaddr_in sendersize;

    int res = recvfrom(sock, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr*) &addr, &socklen);
    kv_reply = kvdata__kvparam__unpack(NULL, res, rcv_buf);
	
    snprintf((char *)value, 4096, "%s", kv_reply->value);
    *value_len = kv_reply->length;   
    
    close(sock);
    return kv_reply->ret;
}

int host_put(char *key, void *value, size_t value_len) 
{
    struct sockaddr_in addr;
  
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(5555);

    Kvdata__Kvparam kv = KVDATA__KVPARAM__INIT; 
    void *send_buf;
    unsigned send_len;

    kv.operation = 2;
    kv.key = key;
    kv.value = value;
    send_len = kvdata__kvparam__get_packed_size(&kv);
  
    send_buf = malloc(send_len);
    kvdata__kvparam__pack(&kv,send_buf);

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    int socklen = sizeof(addr);
    sendto(sock, send_buf, send_len, 0, (struct sockaddr *)&addr, socklen);

    Kvdata__Kvparam *kv_reply;
    uint8_t rcv_buf[MAX_MSG_SIZE];
    struct sockaddr_in sendersize;

    int res = recvfrom(sock, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr*) &addr, &socklen);
    kv_reply = kvdata__kvparam__unpack(NULL, res, rcv_buf);
    
    close(sock);
    return kv_reply->ret;
}
