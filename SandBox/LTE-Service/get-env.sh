#!/bin/bash

CONTAINER=$1

docker inspect -f '{{range $index, $value := .Config.Env}}{{println $value}}{{end}}' ${CONTAINER}
