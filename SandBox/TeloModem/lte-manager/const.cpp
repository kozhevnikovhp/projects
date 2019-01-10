/*
 *  const.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "const.h"

const char *PSZ_CFG_FILE_PATH = "/etc/ooma/mdm.cfg";
const char *PSZ_SYSLOG_VERBOSE_LEVEL = "lte_manager_verbose_level";

const char *PSZ_LTE_NETWORK_INTERFACE = "usbnet0";

const char *PSZ_FW_UPGRADE_MARKER_PATH = "/opt/lte/";
const char *PSZ_FW_UPGRADE_MARKER_FILE = "firmwareUpdated";
const char *PSZ_FW_UPGRADE_PATH = "/update/firmware/lte-dongle/";

const char *PSZ_DEVICE_NAME = "device_name";

const char *PSZ_JSON_VERSION = "1";

const char *PSZ_KAFKA_ENABLED = "kafka_enabled";
const char *PSZ_KAFKA_BROKERS = "kafka_brokers";
const char *PSZ_KAFKA_BROKERS_DEFAULT = "52.53.80.222:9092";
const char *PSZ_KAFKA_REST_PROXY_ENABLED = "kafka_rest_proxy_enabled";
const char *PSZ_KAFKA_REST_PROXY_URL = "kafka_rest_proxy_url";
const char *PSZ_KAFKA_REST_PROXY_URL_DEFAULT = "https://ec2-52-53-95-125.us-west-1.compute.amazonaws.com";
const char *PSZ_KAFKA_REST_PROXY_CERT = "kafka_rest_proxy_cert";
const char *PSZ_KAFKA_REST_PROXY_KEY = "kafka_rest_proxy_key";
const char *PSZ_KAFKA_REST_PROXY_PASSWORD = "kafka_rest_proxy_password";
const char *PSZ_KAFKA_REST_PROXY_VERIFY_PEER = "kafka_rest_proxy_verify_peer";
const char *PSZ_KAFKA_REST_PROXY_CACERT = "kafka_rest_proxy_ca_cert";
const char *PSZ_KAFKA_REST_PROXY_CAKEY = "kafka_rest_proxy_ca_key";
const char *PSZ_KAFKA_REST_PROXY_VERBOSE = "kafka_rest_proxy_verbose";

const char *PSZ_KAFKA_TOPIC = "kafka_topic";
const char *PSZ_KAFKA_TOPIC_DEFAULT = "lte-service";

const char *PSZ_BASIC_QUERY_DELAY = "basic_query_delay";

