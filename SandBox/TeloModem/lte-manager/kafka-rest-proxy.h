/*
 *  https-messenger.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <curl/curl.h>
#include "config.h"

// singleton
class KafkaRestProxy
{
  public:
    static KafkaRestProxy &instance();

    bool post(const std::string &data);
    bool isEnabled() const { return bEnabled_; }
    void configure(const Configuration &config);

  private:
    KafkaRestProxy();
    ~KafkaRestProxy();

    // disable copy constructor...
    KafkaRestProxy(KafkaRestProxy const&) = delete;
    // ... and operator =
    KafkaRestProxy & operator= (KafkaRestProxy const&) = delete;

protected:
    CURL *initCurl();
    void setCommonOptions(CURL *pCurl);

    std::string URL_;
    std::string certFile_;
    std::string keyFile_;
    std::string passPhrase_;
    std::string CAcertFile_;
    bool bEnabled_;
    bool bVerifyPeer_;
    bool bVerbose_;
};

