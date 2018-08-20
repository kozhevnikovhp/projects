/*
 *  messaging-https.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <curl/curl.h>
#include "messaging-https.h"

//static
HttpsMessanger &HttpsMessanger::instance()
{
    static HttpsMessanger s;
    return s;
}

HttpsMessanger::HttpsMessanger()
{
    curl_global_init(CURL_GLOBAL_SSL);
}

//virtual
HttpsMessanger::~HttpsMessanger()
{
    curl_global_cleanup();
}

bool HttpsMessanger::post(const std::string &url)
{
    CURL *pCurl = curl_easy_init();
    if (pCurl)
    {
        curl_easy_setopt(pCurl, CURLOPT_POST, 1);
        curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());

        /* Perform the request, res will get the return code */
        int res = curl_easy_perform(pCurl);
        // Check for errors

        curl_easy_cleanup(pCurl);
        fprintf(stderr, "res = %d\n");
    }
    else
    {
        fprintf(stderr, "couldn't init curl\n");
    }
}
