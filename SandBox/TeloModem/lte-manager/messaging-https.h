/*
 *  https-messenger.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>

// singleton
class HttpsMessanger
{
  public:
    static HttpsMessanger &instance();

  private:
    HttpsMessanger();
    ~HttpsMessanger();

    // disable copy constructor...
    HttpsMessanger(HttpsMessanger const&) = delete;
    // ... and operator =
    HttpsMessanger & operator= (HttpsMessanger const&) = delete;

public:
    bool post(const std::string &url);
};

