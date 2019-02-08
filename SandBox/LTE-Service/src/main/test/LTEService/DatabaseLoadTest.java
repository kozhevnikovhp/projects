package LTEService;

import org.junit.Test;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringRunner;

import java.util.Date;

import static org.junit.Assert.*;

@SpringBootTest
@RunWith(SpringRunner.class)
public class DatabaseLoadTest {
    private static final String myxID = "000000";

    @Autowired
    private Database database_;

    @Test
    public void newData() {
        long startTime = new Date().getTime();
        int n = 0;
        while ((new Date().getTime() - startTime) < 100*1000) {
            if (database_.newData(
                    "{\"version\":\"1\"," +
                            " \"sinr\":\"10\"," +
                            " \"rssi\":\"84\"," +
                            " \"imei\":\"1234567890\"," +
                            " \"iccid\":\"0987654321\"," +
                            " \"ooma_service_status\":\"live\", " +
                            " \"band\":\"B24\"," +
                            " \"bandwidth\":\"20MHz\"," +
                            " \"signal_quality\":\"3\"," +
                            " \"carrier\":\"Sprint\"," +
                            " \"telo_version\":\"1234\"," +
                            " \"fw_version\":\"1.2.3.4\"," +
                            " \"sw_version\":\"1.2.3.4.5.6.7.8.9.0\"," +
                            " \"lte_manager_version\":\"1.1.234\"," +
                            " \"myx_id\":\"000000\"," +
                            " \"lte_dongle\":\"active\"," +
                            " \"connection_type\" : \"ethernet\" }"))
                ++n;
            if (database_.newData(
                    "{\"version\":\"1\"," +

                            " \"sinr\":\"11\"," +
                            " \"rssi\":\"87\"," +
                            " \"imei\":\"1234567890\"," +
                            " \"iccid\":\"0987654321\"," +
                            " \"band\":\"B24\"," +
                            " \"bandwidth\":\"20MHz\"," +
                            " \"signal_quality\":\"5\"," +
                            " \"carrier\":\"Beeline\"," +
                            " \"ooma_service_status\":\"live\", " +
                            " \"telo_version\":\"1234\"," +
                            " \"fw_version\":\"1.2.3.4\"," +
                            " \"sw_version\":\"1.2.3.4.5.6.7.8.9.0\"," +
                            " \"lte_manager_version\":\"1.1.234\"," +
                            " \"myx_id\":\"000000\"," +
                            " \"lte_dongle\":\"active\"," +
                            " \"connection_type\" : \"ethernet\" }"))
                ++n;

            if (database_.newData(
                    "{\"version\":\"1\"," +
                            " \"sinr\":\"10\"," +
                            " \"rssi\":\"84\"," +
                            " \"imei\":\"123467890\"," +
                            " \"iccid\":\"09sdgsdggdgsfgs87654321\"," +
                            " \"band\":\"B24\"," +
                            " \"bandwidth\":\"20MHz\"," +
                            " \"signal_quality\":\"3\"," +
                            " \"carrier\":\"MTS\"," +
                            " \"ooma_service_status\":\"live\", " +
                            " \"telo_version\":\"1234\"," +
                            " \"fw_version\":\"1.2.3.4\"," +
                            " \"sw_version\":\"1.2.3.4.5.6.7.8.9.0\"," +
                            " \"lte_manager_version\":\"1.1.234\"," +
                            " \"myx_id\":\"000002\"," +
                            " \"lte_dongle\":\"active\"," +
                            " \"connection_type\" : \"ethernet\" }"))
                ++n;
            if (database_.newData(
                    "{\"version\":\"1\"," +
                            " \"sinr\":\"11\"," +
                            " \"rssi\":\"87\"," +
                            " \"imei\":\"1234567890\"," +
                            " \"iccid\":\"0987654321\"," +
                            " \"band\":\"B24\"," +
                            " \"bandwidth\":\"20MHz\"," +
                            " \"signal_quality\":\"5\"," +
                            " \"carrier\":\"Beeline\"," +
                            " \"ooma_service_status\":\"live\", " +
                            " \"telo_version\":\"1234\"," +
                            " \"fw_version\":\"1.2.3.4\"," +
                            " \"sw_version\":\"1.2.3.A.B.C.D.9.0\"," +
                            " \"lte_manager_version\":\"1.1.234\"," +
                            " \"myx_id\":\"000001\"," +
                            " \"lte_dongle\":\"active\"," +
                            " \"connection_type\" : \"ethernet\" }"))
                ++n;
       }
        double timeElapsed = (new Date().getTime() - startTime)/1000.; // seconds
        System.out.println(n/timeElapsed + " newData per second");
    }

    @Test
    public void newValue() {
        long startTime = new Date().getTime();
        int n = 0;
        while ((new Date().getTime() - startTime) < 10*1000) {
            database_.newValue(myxID, "sinr", "15", false);
            ++n;
        }
        double timeElapsed = (new Date().getTime() - startTime)/1000.; // seconds
        System.out.println(n/timeElapsed + " newValue per second");
    }
}