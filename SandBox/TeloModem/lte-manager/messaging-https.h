/*
 *  https-messenger.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <curl/curl.h>

// singleton
class HttpsMessanger
{
  public:
    static HttpsMessanger &instance();

    bool get(const std::string &url);
    bool post(const std::string &url, const std::string &data);
    void setCertKeyFiles(const std::string &cert, const std::string &key) { certFile_ = cert; keyFile_ = key; }
    void setPassPhrase(const std::string &passPhrase) { passPhrase_ = passPhrase; }
    void setVerbose(bool b) { bVerbose_ = b; }
    void setVerifyPeer(bool b) { bVerifyPeer_ = b; }
    void setHeader(const std::string &header) { header_ = header; }

  private:
    HttpsMessanger();
    ~HttpsMessanger();

    // disable copy constructor...
    HttpsMessanger(HttpsMessanger const&) = delete;
    // ... and operator =
    HttpsMessanger & operator= (HttpsMessanger const&) = delete;

protected:
    CURL *initCurl();
    void setCommonOptions(CURL *pCurl);

    std::string certFile_;
    std::string keyFile_;
    std::string passPhrase_;
    std::string CAPath_;
    bool bVerifyPeer_;
    bool bVerbose_;

    std::string header_;
};

