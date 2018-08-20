/*
 *  lte-manager-unit-tests.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <gtest/gtest.h>
#include "../modem-gtc.h"

// Template? Good idea. One day...
/*template<void (*T)()> check_one(const char *pszKey, const char *pszValue)
{
    ModemGTC modem("");
    JsonContent content;
    modem.iccid(content);
    EXPECT_EQ(content.size(), 1);
    EXPECT_STREQ(pszKey, content[0].first.c_str());
    EXPECT_STREQ(pszValue, content[0].second.c_str());
}*/

void check_key_value_one(const JsonContent &content, const char *pszKey, const char *pszValue)
{
    EXPECT_EQ(content.size(), 1);
    EXPECT_STREQ(pszKey, content[0].first.c_str());
    EXPECT_STREQ(pszValue, content[0].second.c_str());
}

void check_key_value_many(const JsonContent &content, const char *pszKey, const char *pszValue, int index)
{
    EXPECT_STREQ(pszKey, content[index].first.c_str());
    EXPECT_STREQ(pszValue, content[index].second.c_str());
}

TEST(ModemTests, ManufactureInfo)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getManufacturerInfo(content);
    EXPECT_EQ(content.size(), 5);
    check_key_value_many(content, "board_version", "V1.0", 0);
    check_key_value_many(content, "board_sn", "-", 1);
    check_key_value_many(content, "board_name", "GDM7243S-MOD-U-B25_26_41", 2);
    check_key_value_many(content, "device_type", "Global-7243S", 3);
    check_key_value_many(content, "sw_version", "V1.0.0 PACK 0 (Ver.1379) Build on: May 31 2018", 4);
}

TEST(ModemTests, Firmware)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getFirmwareVersionInfo(content);
    check_key_value_one(content, "fw_version", "0.3.2.4");
}

TEST(ModemTests, Imei)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getImei(content);
    check_key_value_one(content, "imei", "8623430390034200");
}

TEST(ModemTests, IccID)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getIccId(content);
    check_key_value_one(content, "iccid", "89011202000218997994");
}

TEST(ModemTests, Carrier)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getCarrier(content);
    check_key_value_one(content, "carrier", "Sprint");
}

TEST(ModemTests, Status)
{
    ModemGTC modem("");
    JsonContent content;
    modem.getStatus(content);
    EXPECT_EQ(content.size(), 6);
    check_key_value_many(content, "mode", "online", 0);
    check_key_value_many(content, "ps_state", "attached", 1);
    check_key_value_many(content, "rssi", "-78", 2);
    check_key_value_many(content, "sinr", "15", 3);
    check_key_value_many(content, "signal_quality", "3", 4);
    check_key_value_many(content, "lte_status", "up", 5);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


