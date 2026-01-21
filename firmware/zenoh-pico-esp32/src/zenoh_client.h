#ifndef ZENOH_CLIENT_H
#define ZENOH_CLIENT_H

void zenoh_client_task(void *arg);
void zenoh_publish_sensor(const char *topic, const char *payload);
void zenoh_subscribe_handler(const char *topic, const char *payload);

#endif // ZENOH_CLIENT_H

