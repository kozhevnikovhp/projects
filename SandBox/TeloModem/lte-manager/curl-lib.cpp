/*
 *  curl-lib.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "curl-lib.h"
#include "const.h"
#include "config.h"
#include "log.h"

//static
CurlLib &CurlLib::instance()
{
    static CurlLib s;
    return s;
}

CurlLib::CurlLib()
{
    bPostEnabled_ = true;
    bPostVerbose_ = bTftpVerbose_ = false;
    bVerifyPeer_ = false;
    postTimeout_ = 10; // 10 seconds by default but configuarable
    curl_global_init(CURL_GLOBAL_ALL);
}

//virtual
CurlLib::~CurlLib()
{
    curl_global_cleanup();
}

bool CurlLib::post(const std::string &data)
{
    bool bSuccess = false;
    CURL *pCurl = initCurl();
    if (pCurl)
    {
        setSecurityOptions(pCurl);

        curl_easy_setopt(pCurl, CURLOPT_VERBOSE, bPostVerbose_ ? 1L : 0L);

        curl_easy_setopt(pCurl, CURLOPT_POST, 1L);

        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, postTimeout_);

        curl_easy_setopt(pCurl, CURLOPT_URL, RestProxyURL_.c_str());

        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, data.c_str());

        // header
        struct curl_slist *pCurlHeader = nullptr;
        pCurlHeader = curl_slist_append(pCurlHeader, "Content-Type: application/vnd.kafka.json.v1+json");
        curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pCurlHeader);

        // Perform the request, res will get the return code
        CURLcode res = curl_easy_perform(pCurl);
        bSuccess = (res == CURLE_OK);
        // Check for errors
        if (!bSuccess)
            log_error("REST proxy failed to POST %s (%s)", RestProxyURL_.c_str(), curl_easy_strerror(res));

        curl_easy_cleanup(pCurl);
        curl_slist_free_all(pCurlHeader);
        //fprintf(stderr, "res = %d\n", res);
    }
    return bSuccess;
}

bool CurlLib::putFileTFTP(const std::string &fileFullPath, const std::string &tftpServer)
{
    bool bSuccess = false;
    CURL *pCurl = initCurl();
    if (pCurl)
    {
        FILE *fd = fopen(fileFullPath.c_str(), "rb"); // open file to upload
        if (!fd)
        {
            log_error("Cannot open file %s\n", fileFullPath.c_str());
            return false; // can't continue
        }

        curl_easy_setopt(pCurl, CURLOPT_VERBOSE, bTftpVerbose_ ? 1L : 0L);
        curl_easy_setopt(pCurl, CURLOPT_READDATA, fd);

        // set timeout not to wait too long
        //curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10L);

        // binary transfer mode
        curl_easy_setopt(pCurl, CURLOPT_TRANSFERTEXT, 0L);
        // enable uploading
        curl_easy_setopt(pCurl, CURLOPT_UPLOAD, 1L);

        // specify target
        std::string remoteURL = "tftp://";
        remoteURL += tftpServer;
        remoteURL += "/";
        std::size_t pos = fileFullPath.find_last_of("/");
        if (pos == std::string::npos)
            remoteURL += fileFullPath;
        else
            remoteURL += fileFullPath.substr(pos+1);
        //printf("RemoteURL = %s\n", remoteURL.c_str());
        curl_easy_setopt(pCurl, CURLOPT_URL, remoteURL.c_str());

        // Perform the request, res will get the return code
        CURLcode res = curl_easy_perform(pCurl);
        bSuccess = (res == CURLE_OK);
        // Check for errors
        if (bSuccess)
            log_info("TFTP transfer to %s succeded", remoteURL.c_str());
        else
            log_error("TFTP transfer to %s failed (%s)", remoteURL.c_str(), curl_easy_strerror(res));

        curl_easy_cleanup(pCurl);
        fclose(fd);
    }

    return bSuccess;
}

CURL *CurlLib::initCurl()
{
    CURL *pCurl = curl_easy_init();
    if (!pCurl)
        log_error("Could not init curl library");
    return pCurl;
}

void CurlLib::setSecurityOptions(CURL *pCurl)
{
    // CERT
    //printf("CERT = %s\n", certFile_.c_str());
    if (!certFile_.empty())
        curl_easy_setopt(pCurl, CURLOPT_SSLCERT, certFile_.c_str());

    // KEY
    //printf("KEY = %s\n", keyFile_.c_str());
    if (!keyFile_.empty())
        curl_easy_setopt(pCurl, CURLOPT_SSLKEY, keyFile_.c_str());

    // PassPhrase
    //printf("PASS = %s\n", passPhrase_.c_str());
    if (!passPhrase_.empty())
        curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD, passPhrase_.c_str());

    // CA path (if enabled)
    //curl_easy_setopt(pCurl, CURLOPT_CAPATH, pPath);
    if (CAcertFile_.empty())
        curl_easy_setopt(pCurl, CURLOPT_CAINFO, CAcertFile_.c_str());

    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, bVerifyPeer_);

    curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
}

void CurlLib::configure(const Configuration &cfg, bool bDaemon)
{
    bPostEnabled_ = cfg.getBoolean(PSZ_KAFKA_REST_PROXY_ENABLED, true);
    log_info("REST proxy enabled: %s", bPostEnabled_ ? "true" : "false");

    RestProxyURL_ = cfg.get(PSZ_KAFKA_REST_PROXY_URL, PSZ_KAFKA_REST_PROXY_URL_DEFAULT);
    log_info("REST proxy URL: %s", RestProxyURL_.c_str());
    RestProxyURL_ += "/topics/";
    RestProxyURL_ += cfg.get(PSZ_KAFKA_TOPIC, PSZ_KAFKA_TOPIC_DEFAULT);

    postTimeout_ = cfg.getInteger(PSZ_KAFKA_REST_PROXY_TIMEOUT, postTimeout_);
    if (postTimeout_ < 5)
        postTimeout_ = 5;
    log_info("REST proxy timeout: %d seconds", postTimeout_);

    certFile_ = cfg.get(PSZ_KAFKA_REST_PROXY_CERT, "");
    keyFile_ = cfg.get(PSZ_KAFKA_REST_PROXY_KEY, "");
    passPhrase_ = cfg.get(PSZ_KAFKA_REST_PROXY_PASSWORD, "");
    CAcertFile_ = cfg.get(PSZ_KAFKA_REST_PROXY_CACERT, "");
    bVerifyPeer_ = cfg.getBoolean(PSZ_KAFKA_REST_PROXY_VERIFY_PEER, false);

    // for performance in daemon mode, just to not make libcurl create its verbose output and nullify it then
    if (bDaemon)
        bPostVerbose_ = bTftpVerbose_ = false;
    else
        bPostVerbose_ = cfg.getBoolean(PSZ_KAFKA_REST_PROXY_VERBOSE, false);
}

