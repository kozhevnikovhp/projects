package LTEService;

import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringRunner;

import java.util.Date;
import java.util.List;

import static org.junit.Assert.*;

@SpringBootTest
@RunWith(SpringRunner.class)
public class DatabaseUnitTest {
    private static final String UNKNOWN_VALUE = "unknown";

    private static final String LTE_DONGLE = "lte_dongle";
    private static final String LTE_DONGLE_ACTIVE = "active";
    private static final String LTE_DONGLE_INACTIVE = "inactive";

    private static final String LTE_STATE = "lte_state";
    private static final String LTE_STATE_UP = "up";
    private static final String LTE_STATE_DOWN = "down";

    private static final String CARRIER = "carrier";
    private static final String RSSI = "rssi";
    private static final String RSRP = "rsrp";
    private static final String SINR = "sinr";
    private static final String TX_POWER = "tx_power";

    private static final String CONNECTION_TYPE = "connection_type";
    private static final String CONNECTION_TYPE_ETHERNET = "ethernet";
    private static final String CONNECTION_TYPE_WIFI = "wifi";
    private static final String CONNECTION_TYPE_LTE = "lte";

    private static final String OOMA_SERVICE_STATUS = "ooma_service_status";
    private static final String IP_ADDRESS = "ip_address";
    private static final String IP_MASK = "mask";
    private static final String GATEWAY = "gateway";

    private static final String SENT_BYTES_TELO = "telo_ooma_apps_lte_sent_bytes";
    private static final String RECV_BYTES_TELO = "telo_ooma_apps_lte_received_bytes";

    private static final String SIGNAL_QUALITY = "signal_quality";
    private static final String SIGNAL_QUALITY_NO          = "0";
    private static final String SIGNAL_QUALITY_BAD         = "1";
    private static final String SIGNAL_QUALITY_APPROPRIATE = "2";
    private static final String SIGNAL_QUALITY_GOOD        = "3";
    private static final String SIGNAL_QUALITY_PERFECT     = "4";

    private static final String INACTIVE_NOTIFIED = "inactive_notified";
    private static final String NO_SIGNAL_NOTIFIED = "no_signal_notified";
    private static final String CONN_DOWN_NOTIFIED = "connection_down_notified";
    private static final String BAD_SIGNAL_NOTIFIED = "bad_signal_notified";
    private static final String SILENCE_NOTIFIED = "silence_notified";

    private static final String OBSOLETE_VALUE = "obsolete";
    private static final String EMPTY_VALUE = "";

    private static long ONE_SECOND = 1000; // time in msec

    private static final String myxID = "000000";

    @Autowired
    private Database database_;

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void newDataWithoutMyxID() {
        // set definite value
        boolean bSuccess = database_.newData("{\"version\":\"1\", \"sinr\":\"10\", \"myx_id\":\"000000\" }");
        assertTrue(bSuccess);
        checkLastValue(myxID, "sinr", "10");

        // set another value but do not set myxID -> check that old value is the same
        bSuccess = database_.newData("{\"version\":\"1\", \"sinr\":\"11\" }");
        assertFalse(bSuccess);
        checkLastValue(myxID, "sinr", "10");

        // set another value but myxID empty -> check that old value is the same
        bSuccess = database_.newData("{\"version\":\"1\", \"sinr\":\"12\", \"myx_id\":\"\" }");
        assertFalse(bSuccess);
        checkLastValue(myxID, "sinr", "10");
    }

    @Test
    public void newValueTest() {
        final String key = "rssi";
        final String value = "88";

        newValue(myxID, key, value);
        checkLastValue(myxID, key, value);
    }

    @Test
    public void wrongJsonTest() {
        boolean bSuccess = database_.newData("{\"version\":\"1\", \"sinr\":\"10\", \"myx_id\"::\"000000\" }");
        assertFalse(bSuccess);
    }

    @Test
    public void wrongKeyTest() {
        Object obj = database_.getLastValue(myxID, "wrong_key");
        assertNull(obj);
    }

    @Test
    public void resetStatsTest() {
        database_.resetStats(myxID);
        checkLastValue(myxID, SENT_BYTES_TELO, "0");
        checkLastValue(myxID, RECV_BYTES_TELO, "0");
    }

    @Test
    public void getTrafficStatisticsTest() {
        // 1. create data to have what to count and report
        try {
            Thread.sleep(2000); // to avoid strangers from the past
            Date start = new Date();
            newValue(myxID, SENT_BYTES_TELO, "10");
            newValue(myxID, RECV_BYTES_TELO, "20");
            Thread.sleep(2000);
            newValue(myxID, SENT_BYTES_TELO, "10");
            newValue(myxID, RECV_BYTES_TELO, "20");
            Date end = new Date();
            Thread.sleep(2000);
            newValue(myxID, SENT_BYTES_TELO, "10");
            newValue(myxID, RECV_BYTES_TELO, "20");

            // wrong parameter name, expects -1
            int nBytes = database_.getTrafficStatistics(myxID, "wrong_parameter_name", start, end);
            assertEquals(-1, nBytes);

            nBytes = database_.getTrafficStatistics(myxID, SENT_BYTES_TELO, start, end);
            assertEquals(10, nBytes);

            nBytes = database_.getTrafficStatistics(myxID, RECV_BYTES_TELO, start, end);
            assertEquals(20, nBytes);
        } catch(InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    @Test
    public void getHistoricDataTest() {
        // 1. create data to have what to count and report
        try {
            Thread.sleep(2000); // to avoid strangers from the past
            Date start = new Date();
            newValue(myxID, SINR, "10");
            newValue(myxID, TX_POWER, "100");
            Thread.sleep(1000);
            newValue(myxID, SINR, "-11");
            Thread.sleep(1000);
            newValue(myxID, SINR, "-12");
            newValue(myxID, TX_POWER, "101");
            Thread.sleep(1000);
            newValue(myxID, SINR, "-13");
            Thread.sleep(2000);
            Date end = new Date();

            List<Database.TimedValue> sinrs = database_.getHistoricalData(myxID, SINR, start, end);
            assertEquals(4, sinrs.size());
            List<Database.TimedValue> tx_powers = database_.getHistoricalData(myxID, TX_POWER, start, end);
            assertEquals(2, tx_powers.size());
        } catch(InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    @Test
    public void silenceAlarmTest() {
        // 1. ensure that myxID does exist
        newValue(myxID, RECV_BYTES_TELO, "1000");

        // sleep for couple of seconds to be sure that fake myxID is silent
        try {
            Thread.sleep(3000);
        }
        catch(InterruptedException ex) {
            Thread.currentThread().interrupt();
        }

        // check for silent myxIDs and check that necessary values are set
        database_.doSilenceAlarm(ONE_SECOND);

        checkLastValue(myxID, CARRIER, EMPTY_VALUE);
        checkLastValue(myxID, RSSI, EMPTY_VALUE);
        checkLastValue(myxID, RSRP, EMPTY_VALUE);
        checkLastValue(myxID, SINR, EMPTY_VALUE);
        checkLastValue(myxID, IP_ADDRESS, EMPTY_VALUE);
        checkLastValue(myxID, IP_MASK, EMPTY_VALUE);
        checkLastValue(myxID, GATEWAY, EMPTY_VALUE);
        checkLastValue(myxID, CONNECTION_TYPE, OBSOLETE_VALUE);
        checkLastValue(myxID, OOMA_SERVICE_STATUS, OBSOLETE_VALUE);
        checkLastValue(myxID, LTE_DONGLE, LTE_DONGLE_INACTIVE);
        checkLastValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_NO);
        checkLastValue(myxID, LTE_STATE, LTE_STATE_DOWN);

        int nNotified = database_.howManyNotificationsSent(myxID, SILENCE_NOTIFIED);
        assertNotEquals(0, nNotified);
    }

    @Test
    public void resetNotifications() {
        checkResetNotifications(INACTIVE_NOTIFIED);
        checkResetNotifications(BAD_SIGNAL_NOTIFIED);
        checkResetNotifications(NO_SIGNAL_NOTIFIED);
        checkResetNotifications(CONN_DOWN_NOTIFIED);
        checkResetNotifications(SILENCE_NOTIFIED);
    }

    @Test
    public void incrementNotifications() {
        checkIncrementNotifications(INACTIVE_NOTIFIED);
        checkIncrementNotifications(BAD_SIGNAL_NOTIFIED);
        checkIncrementNotifications(NO_SIGNAL_NOTIFIED);
        checkIncrementNotifications(CONN_DOWN_NOTIFIED);
        checkIncrementNotifications(SILENCE_NOTIFIED);
    }

    @Test
    public void dongleActiveInactiveTest() {
        newValue(myxID, LTE_DONGLE, LTE_DONGLE_ACTIVE);
        int nNotified = database_.howManyNotificationsSent(myxID, INACTIVE_NOTIFIED);
        assertEquals(0, nNotified);
        newValue(myxID, LTE_DONGLE, LTE_DONGLE_INACTIVE);
        nNotified = database_.howManyNotificationsSent(myxID, INACTIVE_NOTIFIED);
        assertEquals(1, nNotified);
    }

    @Test
    public void noSignalTest() {
        newValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_PERFECT);
        int nNotified = database_.howManyNotificationsSent(myxID, NO_SIGNAL_NOTIFIED);
        assertEquals(0, nNotified);
        newValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_NO);
        nNotified = database_.howManyNotificationsSent(myxID, NO_SIGNAL_NOTIFIED);
        assertEquals(1, nNotified);
    }

    @Test
    public void badSignalTest() {
        newValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_GOOD);
        int nNotified = database_.howManyNotificationsSent(myxID, BAD_SIGNAL_NOTIFIED);
        assertEquals(0, nNotified);
        newValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_BAD);
        nNotified = database_.howManyNotificationsSent(myxID, BAD_SIGNAL_NOTIFIED);
        assertEquals(1, nNotified);
    }

    @Test
    public void lteState() {
        newValue(myxID, LTE_STATE, LTE_STATE_UP);
        int nNotified = database_.howManyNotificationsSent(myxID, CONN_DOWN_NOTIFIED);
        assertEquals(0, nNotified);
        newValue(myxID, LTE_STATE, LTE_STATE_DOWN);
        nNotified = database_.howManyNotificationsSent(myxID, CONN_DOWN_NOTIFIED);
        assertEquals(1, nNotified);
    }

    @Test
    public void connectionType() {
        newValue(myxID, CONNECTION_TYPE, UNKNOWN_VALUE);
        newValue(myxID, CONNECTION_TYPE, CONNECTION_TYPE_ETHERNET);
        newValue(myxID, CONNECTION_TYPE, CONNECTION_TYPE_WIFI);
        newValue(myxID, CONNECTION_TYPE, CONNECTION_TYPE_LTE);
        checkLastValue(myxID, CONNECTION_TYPE, CONNECTION_TYPE_LTE);
    }

    private void checkLastValue(String myxID, String key, String expected) {
        Object obj = database_.getLastValue(myxID, key);
        assertNotNull("Database does not work", obj);
        String actual = obj.toString();
        assertEquals(expected, actual);
    }

    private void checkResetNotifications(String paramName) {
        database_.resetNotificationCounter(myxID, paramName);
        int nNotifications = database_.howManyNotificationsSent(myxID, paramName);
        assertEquals(0, nNotifications);
    }

    private void checkIncrementNotifications(String paramName) {
        int nBefore = database_.howManyNotificationsSent(myxID, paramName);
        database_.notificationsSent(myxID, paramName);
        int nAfter = database_.howManyNotificationsSent(myxID, paramName);
        assertEquals(1, nAfter-nBefore);
    }

    private void newValue(String myxID, String key, String value) {
        JSONObject json = new JSONObject();
        json.put("myx_id", myxID);
        json.put("version", "1");
        json.put(key, value);
        database_.newData(json.toString());
    }
}
