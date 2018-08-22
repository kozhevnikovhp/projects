/*
 *  messaging-https.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "messaging-https.h"

//static
HttpsMessanger &HttpsMessanger::instance()
{
    static HttpsMessanger s;
    return s;
}

HttpsMessanger::HttpsMessanger()
{
    bVerbose_ = false;
    bVerifyPeer_ = false;
    curl_global_init(CURL_GLOBAL_SSL);
}

//virtual
HttpsMessanger::~HttpsMessanger()
{
    curl_global_cleanup();
}

bool HttpsMessanger::get(const std::string &url)
{
    CURL *pCurl = initCurl();
    if (pCurl)
    {
        curl_easy_setopt(pCurl, CURLOPT_HTTPGET, 1);

        setCommonOptions(pCurl);

        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());

        /* Perform the request, res will get the return code */
        int res = curl_easy_perform(pCurl);
        // Check for errors

        curl_easy_cleanup(pCurl);
        fprintf(stderr, "res = %d\n", res);
    }
}

bool HttpsMessanger::post(const std::string &url, const std::string &data)
{
    CURL *pCurl = initCurl();
    if (pCurl)
    {
        curl_easy_setopt(pCurl, CURLOPT_POST, 1);
        //curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);

        setCommonOptions(pCurl);

        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, data.c_str());

        /* Perform the request, res will get the return code */
        int res = curl_easy_perform(pCurl);
        // Check for errors

        curl_easy_cleanup(pCurl);
        fprintf(stderr, "res = %d\n", res);
    }
}

CURL *HttpsMessanger::initCurl()
{
    CURL *pCurl = curl_easy_init();
    if (!pCurl)
        fprintf(stderr, "couldn't init curl\n");
}

void HttpsMessanger::setCommonOptions(CURL *pCurl)
{
    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, bVerbose_);

    // CERT
    if (!certFile_.empty())
        curl_easy_setopt(pCurl, CURLOPT_SSLCERT, certFile_.c_str());

    // KEY
    if (!keyFile_.empty())
        curl_easy_setopt(pCurl, CURLOPT_SSLKEY, keyFile_.c_str());

    // PassPhrase
    if (!passPhrase_.empty())
        curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD, passPhrase_.c_str());

    // CA path (if enabled)
    //curl_easy_setopt(pCurl, CURLOPT_CAPATH, pPath);
    //curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/certs/cabundle.pem");

    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);
   // curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

    // Header
    if (!header_.empty())
        curl_easy_setopt(pCurl, CURLOPT_HEADER, header_.c_str());

}
