/*
 *  curl-lib.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <curl/curl.h>

// forward declarations
class Configuration;

// singleton
class CurlLib
{
  public:
    static CurlLib &instance();

    bool post(const std::string &data);
    bool putFileTFTP(const std::string &fileFullPath, const std::string &tftpServer);
    bool isPostEnabled() const { return bPostEnabled_; }
    void configure(const Configuration &config, bool bDaemon);

  private:
    CurlLib();
    ~CurlLib();

    // disable copy constructor...
    CurlLib(CurlLib const &) = delete;
    // ... and operator =
    CurlLib & operator= (CurlLib const &) = delete;

protected:
    CURL *initCurl();
    void setSecurityOptions(CURL *pCurl);

    std::string RestProxyURL_;
    std::string certFile_;
    std::string keyFile_;
    std::string passPhrase_;
    std::string CAcertFile_;
    bool bPostEnabled_;
    bool bVerifyPeer_;
    int postTimeout_;
    bool bPostVerbose_, bTftpVerbose_;
};

