#!/bin/bash

APP_DIR="/lte-service"

KAFKA_BOOTSTRAP_SERVERS="${KAFKA_BOOTSTRAP_SERVERS:-localhost:9092}"
SPRING_KAFKA_TOPIC="${SPRING_KAFKA_TOPIC:-lte-service}"
SPRING_PUSH_URL="${SPRING_PUSH_URL:-http://push1-mera.mera.ooma.com:8000/api/v1/send}"
MONGO_URI="${MONGO_URI:-localhost:27017}"
MONGO_DATABASE="${MONGO_DATABASE:-lte-service}"

cat $APP_DIR/application.properties.tmpl \
    | sed "s|KAFKA_BOOTSTRAP_SERVERS|$KAFKA_BOOTSTRAP_SERVERS|g" \
    | sed "s|SPRING_KAFKA_TOPIC|$SPRING_KAFKA_TOPIC|g" \
    | sed "s|SPRING_PUSH_URL|$SPRING_PUSH_URL|g" \
    | sed "s|MONGO_URI|$MONGO_URI|g" \
    | sed "s|MONGO_DATABASE|$MONGO_DATABASE|g" \
    > $APP_DIR/application.properties

exec java -Dcom.sun.management.jmxremote -Dcom.sun.management.jmxremote.port=9090 -Dcom.sun.management.jmxremote.rmi.port=9090 -Djava.rmi.server.hostname=172.17.0.1 -Dcom.sun.management.jmxremote.ssl=false -Dcom.sun.management.jmxremote.authenticate=false -jar $APP_DIR/target/LTEService-0.1.0.jar
