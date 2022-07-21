# Simple MQTT
Basic project containing a simple MQTT publisher with limited MQTT features.
#### Compiling
    $ gcc -Werror main.c mqtt_prot.c network.c -o simple_mqtt
#### How to use
    $ ./simple_mqtt <broker url> <port> <topic>
main.c is just an example, feel free to adapt as you need.
mqtt.h and mqtt_prot.h are fully commented on how to implement.