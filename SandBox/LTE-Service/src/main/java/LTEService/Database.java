package LTEService;

import com.mongodb.*;
import com.mongodb.client.FindIterable;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;
import com.mongodb.client.model.Filters;
import com.mongodb.client.model.UpdateOptions;
import com.mongodb.client.model.Updates;

import com.mongodb.client.result.DeleteResult;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import org.bson.Document;
import org.bson.conversions.Bson;
import org.json.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;

import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import static com.mongodb.client.model.Filters.*;
import static com.mongodb.client.model.Sorts.ascending;
import static com.mongodb.client.model.Sorts.descending;
import static com.mongodb.client.model.Updates.combine;

@Component
@Scope("singleton")
@EnableScheduling
public class Database
{
    public class TimedValue {
        public TimedValue(long time, Object value) {
            time_ = time;
            value_ = value;
        }

        public JSONObject toJSON() {
            JSONObject json = new JSONObject();
            json.put("time", time_);
            json.put("value", value_);
            return json;
        }

        private long time_;
        private Object value_;
    };

    private static final String MYXID = "myx_id";
    private static final String VERSION = "version";

    private static final String LTE_DONGLE = "lte_dongle";
    private static final String LTE_DONGLE_ACTIVE = "active";
    private static final String LTE_DONGLE_INACTIVE = "inactive";

    private static final String LTE_STATE = "lte_state";
    private static final String LTE_STATE_UP = "up";
    private static final String LTE_STATE_DOWN = "down";

    private static final String SIGNAL_QUALITY = "signal_quality";
    private static final String SIGNAL_QUALITY_NO          = "0";
    private static final String SIGNAL_QUALITY_BAD         = "1";
    private static final String SIGNAL_QUALITY_APPROPRIATE = "2";
    private static final String SIGNAL_QUALITY_GOOD        = "3";
    private static final String SIGNAL_QUALITY_PERFECT     = "4";

    private static final String CARRIER = "carrier";
    private static final String BAND = "band";
    private static final String CELL_ID = "cell_id";
    private static final String BANDWIDTH = "bandwidth";
    private static final String RX_CHANNEL = "rx_channel";
    private static final String TX_CHANNEL = "tx_channel";
    private static final String TX_POWER = "tx_power";
    private static final String RSSI = "rssi";
    private static final String RSRP = "rsrp";
    private static final String RSRQ = "rsrq";
    private static final String SINR = "sinr";

    private static final String CONNECTION_TYPE = "connection_type";
    private static final String CONNECTION_TYPE_ETHERNET = "ethernet";
    private static final String CONNECTION_TYPE_WIFI = "wifi";
    private static final String CONNECTION_TYPE_LTE = "lte";

    private static final String OOMA_SERVICE_STATUS = "ooma_service_status";
    private static final String IP_ADDRESS = "ip_address";
    private static final String IP_MASK = "mask";
    private static final String GATEWAY = "gateway";

    private static final String TIME = "time";
    private static final String LAST_INFO = "last_info";
    private static final String KEY = "key";
    private static final String VALUE = "value";
    private static final String PSEUDO = "pseudo";

    private static final String SENT_BYTES_TELO = "telo_ooma_apps_lte_sent_bytes";
    private static final String RECV_BYTES_TELO = "telo_ooma_apps_lte_received_bytes";

    private static final String INACTIVE_NOTIFIED = "inactive_notified";
    private static final String NO_SIGNAL_NOTIFIED = "no_signal_notified";
    private static final String CONN_DOWN_NOTIFIED = "connection_down_notified";
    private static final String BAD_SIGNAL_NOTIFIED = "bad_signal_notified";
    private static final String SILENCE_NOTIFIED = "silence_notified";

    private static final String OBSOLETE_VALUE = "obsolete";
    private static final String EMPTY_VALUE = "";

    private static long ONE_SECOND = 1000; // time in msec
    private static long ONE_MINUTE = 60*ONE_SECOND;
    private static long ONE_HOUR = 60*ONE_MINUTE;
    private static long ONE_DAY = 24*ONE_HOUR;

    private static final Logger LOGGER = LogManager.getLogger(Database.class);

    @Autowired
    private PushNotificator pushNotificator_;

    @Value("${spring.mongo.URI}")
    private String mongoHostURI_;
    @Value("${spring.mongo.database}")
    private String mongoDatabase_;
    @Value("${spring.mongo.cleanup}")
    private int mongoCleanupTime_;

    @Value("${spring.push.inactive_notifications}")
    private int inactiveMaxNotifications_;
    @Value("${spring.push.no_signal_notifications}")
    private int noSignalMaxNotifications_;
    @Value("${spring.push.bad_signal_notifications}")
    private int badSignalMaxNotifications_;
    @Value("${spring.push.silence_tolerance}")
    private int silenceTolerance_;
    @Value("${spring.push.silence_notifications}")
    private int silenceMaxNotifications_;
    @Value("${spring.push.connection_down_notifications}")
    private int connectionDownMaxNotifications_;

    @PostConstruct
    public void initialize() {
        try {
            // TODO: process errors!
            mongoClient_ = new MongoClient(new MongoClientURI(mongoHostURI_));
            db_ = mongoClient_.getDatabase(mongoDatabase_);

            notificationCollection_ = db_.getCollection("notifications");
            if (notificationCollection_ != null) {
                notificationCollection_.createIndex(new BasicDBObject(MYXID, 1));
            }

            parametersCollection_ = db_.getCollection("parameters");
            if (parametersCollection_ != null) {
                // TODO: use compound index, it is faster
                parametersCollection_.createIndex(new BasicDBObject(MYXID, 1));
                parametersCollection_.createIndex(new BasicDBObject(KEY, 1));
                parametersCollection_.createIndex(new BasicDBObject(TIME, 1));
                parametersCollection_.createIndex(new BasicDBObject(PSEUDO, 1));
            }
        }
        catch (MongoException e) {
            e.printStackTrace();
            LOGGER.error(e.getMessage());
        }
    }

    public int howManyNotificationsSent(String myxID, String paramName) {
        int nSent = 0;
        if (notificationCollection_ != null) {
            Document doc = notificationCollection_.find(and(eq(MYXID, myxID))).first();
            if (doc != null) {
                nSent = doc.getInteger(paramName, 0);
            }
        }
        return nSent;
    }

    public void notificationsSent(String myxID, String paramName) {
        // update "notifications" collection
        // increment counter of pushed notifications
        Bson query = Filters.eq(MYXID, myxID);
        Bson upsert = Updates.inc(paramName, 1);
        notificationCollection_.updateOne(query, upsert, new UpdateOptions().upsert(true));
    }

    public void resetNotificationCounter(String myxID, String paramName) {
        // update "notifications" collection
        // increment counter of pushed notifications
        Bson query = Filters.eq(MYXID, myxID);
        Bson upsert = Updates.set(paramName, 0);
        notificationCollection_.updateOne(query, upsert, new UpdateOptions().upsert(true));
    }

    public boolean newData(String jsonStr) {
        boolean bSuccess = true;
        try {
            JSONObject json = new JSONObject(jsonStr);
            if (!json.has(MYXID))
                return false; // just to avoid exception in the next line
            String myxID = json.getString(MYXID);
            if (myxID.isEmpty())
                return false; // do nothing

            Iterator<String> keysItr = json.keys();
            while (keysItr.hasNext()) {
                String paramName = keysItr.next();
                if (paramName.equalsIgnoreCase(VERSION))
                    continue;
                if (paramName.equalsIgnoreCase(MYXID))
                    continue;
                String value = json.get(paramName).toString();
                newValue(myxID, paramName, value, true);
            }

            // update "notifications" collection with last telemetric info time (for silence detection)
            // set current time and clear the counter of pushed notifications
            Bson query = Filters.eq(MYXID, myxID);
            Bson upsert = combine(Updates.set(LAST_INFO, new Date().getTime()), Updates.set(SILENCE_NOTIFIED, 0));
            notificationCollection_.updateOne(query, upsert, new UpdateOptions().upsert(true));
        } catch (JSONException e) {
            e.printStackTrace();
            LOGGER.error("Invalid JSON data: {}, {}", jsonStr, e.getMessage());
            bSuccess = false;
        } catch (MongoException e) {
            e.printStackTrace();
            LOGGER.error("MongoDB error: {}", e.getMessage());
            bSuccess = false;
        }
        return  bSuccess;
    }

    public void newValue(String myxID, String key, String value, boolean bSpecialProcessing) {
        if (parametersCollection_ != null) {
            // Update MongoDB
            Document document = new Document();
            document.put(MYXID, myxID);
            document.put(KEY, key);

            if (key.equalsIgnoreCase(LTE_DONGLE)) {
                if (value.equalsIgnoreCase(LTE_DONGLE_INACTIVE)) {
                    if (howManyNotificationsSent(myxID, INACTIVE_NOTIFIED) < inactiveMaxNotifications_) {
                        if (pushNotificator_.send(myxID, "Dongle disconnected"))
                            notificationsSent(myxID, INACTIVE_NOTIFIED);
                    }
                }
                else if (value.equalsIgnoreCase(LTE_DONGLE_ACTIVE)) {
                    resetNotificationCounter(myxID, INACTIVE_NOTIFIED);
                }
            }
            else if (key.equalsIgnoreCase(CONNECTION_TYPE)) {
                Object oldValue = getLastValue(myxID, CONNECTION_TYPE);
                if (oldValue != null) {
                    String oldType = oldValue.toString();
                    if (!oldType.equals(value)) {
                        String notificationText;
                        if (value.equalsIgnoreCase(CONNECTION_TYPE_ETHERNET) || value.equalsIgnoreCase(CONNECTION_TYPE_WIFI))
                            notificationText = "Your Telo's wired connection has been restored";
                        else if (value.equalsIgnoreCase(CONNECTION_TYPE_LTE))
                            notificationText = "Your Telo is now using LTE";
                        else
                            notificationText = "Connection type changed";
                        pushNotificator_.send(myxID, notificationText);
                    }
                }
            }
            else if (key.equalsIgnoreCase(LTE_STATE)) {
                if (value.equalsIgnoreCase(LTE_STATE_DOWN)) {
                    if (howManyNotificationsSent(myxID, CONN_DOWN_NOTIFIED) < connectionDownMaxNotifications_) {
                        if (pushNotificator_.send(myxID, "LTE connection down"))
                            notificationsSent(myxID, CONN_DOWN_NOTIFIED);
                    }
                }
                else if (value.equalsIgnoreCase(LTE_STATE_UP)) {
                    resetNotificationCounter(myxID, CONN_DOWN_NOTIFIED);
                }
            }
            else if (key.equalsIgnoreCase(SIGNAL_QUALITY) &&
                    (value.equalsIgnoreCase(SIGNAL_QUALITY_NO) || value.equalsIgnoreCase(SIGNAL_QUALITY_BAD))) {
                // check connection type, and if it is "LTE" - send push notification
                // if it is "ethernet", it might be OK
                boolean bSend = true;
                Object connectionType = getLastValue(myxID, CONNECTION_TYPE);
                if (connectionType != null) {
                    if (connectionType.toString() != CONNECTION_TYPE_LTE) {
                        bSend = false;
                        //System.out.println("Signal is inappropriate, but as connection type != LTE, is is OK");
                    }
                }
                if (bSend) {
                    if (value.equalsIgnoreCase(SIGNAL_QUALITY_NO)) {
                        if (howManyNotificationsSent(myxID, NO_SIGNAL_NOTIFIED) < noSignalMaxNotifications_) {
                            if (pushNotificator_.send(myxID, "No signal"))
                                notificationsSent(myxID, NO_SIGNAL_NOTIFIED);
                        }
                    }
                    // temporarily removed (Dean's request)
                    else if (value.equalsIgnoreCase(SIGNAL_QUALITY_BAD)) {
                        if (howManyNotificationsSent(myxID, BAD_SIGNAL_NOTIFIED) < badSignalMaxNotifications_) {
                            //if (pushNotificator_.send(myxID, "Bad signal")) TEMPORARILY REMOVED (Dean's request)
                                notificationsSent(myxID, BAD_SIGNAL_NOTIFIED);
                        }
                    }
                    else {
                        resetNotificationCounter(myxID, BAD_SIGNAL_NOTIFIED);
                        resetNotificationCounter(myxID, NO_SIGNAL_NOTIFIED);
                    }
                }
            }

            if (bSpecialProcessing) {
                if (key.equalsIgnoreCase(SENT_BYTES_TELO) ||
                        key.equalsIgnoreCase(RECV_BYTES_TELO)) {
                    // special processing for statistics! Bytes are being sent incrementally, we cannot replace, but have to increment
                    Object oldValue = getLastValue(myxID, key);
                    if (oldValue != null) {
                        // convert to integer, increment, and put it back
                        int valueInDB = Integer.parseInt(oldValue.toString());
                        int increment = Integer.parseInt(value.toString());
                        valueInDB += increment;
                        value = Integer.toString(valueInDB);
                    }
                }
            }

            document.put(VALUE, value);
            document.put(TIME, new Date().getTime());
            parametersCollection_.insertOne(document);
        }
    }

    public void newPseudoValue(String myxID, String key, String value) {
        if (parametersCollection_ != null) {
            // Update MongoDB
            Document document = new Document();
            document.put(MYXID, myxID);
            document.put(KEY, key);
            document.put(VALUE, value);
            document.put(PSEUDO, true); // flag, indicating that it is pseudo-value, has not come from real TELO, but was generated by LMS itself (when TELO is silent, for example)
            document.put(TIME, new Date().getTime());
            parametersCollection_.insertOne(document);
        }
    }

    public Object getLastValue(String myxID, String key) {
        if (parametersCollection_ != null) {
            Document doc = parametersCollection_.find(and(eq(MYXID, myxID), eq(KEY, key))).sort(descending(TIME)).first();
            if (doc != null)
                return doc.get(VALUE);
        }

        return null;
    }

    // returns the timestamp of the latest non-pseudo value for specified myxID (pseudo-values can be created by LMS itself in case when Telo is silent for a while)
    public Object getLastRealValueTimestamp(String myxID) {
        //if (parametersCollection_ != null) {
            //Document doc = parametersCollection_.find(and(eq(MYXID, myxID), ne(PSEUDO, true))).sort(descending(TIME)).first();
            //if (doc != null)
            //    return doc.get(TIME);

        //}
        if (notificationCollection_ != null) {
            Document doc = notificationCollection_.find(eq(MYXID, myxID)).first();
            if (doc != null) {
                return doc.getLong(LAST_INFO);
            }
        }

        return null;
    }

    public void resetStats(String myxID) {
        newValue(myxID, SENT_BYTES_TELO, "0", false);
        newValue(myxID, RECV_BYTES_TELO, "0", false);
    }

    public int getTrafficStatistics(String myxID, String key, Date fromTime, Date toTime) {
        if (!key.equalsIgnoreCase(SENT_BYTES_TELO) && !key.equalsIgnoreCase(RECV_BYTES_TELO))
            return -1;
        int nBytes = 0;
        int nPrevBytes = 0;
        long prevTime = -1; // not set yet
        boolean bFirstTimestampInRange = true;
        if (parametersCollection_ != null) {
            Document lastDoc = parametersCollection_.find(and(eq(MYXID, myxID), eq(KEY, key), lt(TIME, fromTime.getTime()))).sort(descending(TIME)).first();
            if (lastDoc != null) {
                nPrevBytes = Integer.parseInt(lastDoc.get(VALUE).toString());
                prevTime = lastDoc.getLong(TIME);
            }

            FindIterable<Document> docs = parametersCollection_.find(and(eq(MYXID, myxID), eq(KEY, key), gte(TIME, fromTime.getTime()), lte(TIME, toTime.getTime()))).sort(ascending(TIME));
            if (docs != null) {
                for (Document doc : docs) {
                    Object value = doc.get(VALUE);
                    //if (value != null) // value must exist, let it crash
                    int n = Integer.parseInt(value.toString());
                    long time = doc.getLong(TIME);
                    if (n != 0) {
                        if (bFirstTimestampInRange) {
                            // add proportional value to the last timestamp before the range
                            if (prevTime > 0) // only if something exists over there
                                nBytes += (int) ((n - nPrevBytes) * (double) (time - fromTime.getTime()) / (double) (time - prevTime));
                        } else
                            nBytes += (n - nPrevBytes);
                    }
                    nPrevBytes = n;
                    prevTime = time;
                    bFirstTimestampInRange = false;
                }
            }

            Document nextDoc = parametersCollection_.find(and(eq(MYXID, myxID), eq(KEY, key), gt(TIME, toTime.getTime()))).sort(ascending(TIME)).first();
            if (nextDoc != null) {
                int n = Integer.parseInt(nextDoc.get(VALUE).toString());
                if (n != 0) {
                    if (prevTime > 0) {
                        long time = nextDoc.getLong(TIME);
                        nBytes += (int) ((n - nPrevBytes) * (double) (toTime.getTime() - prevTime) / (double) (time - prevTime));
                    }
                }
            }
        }
        return nBytes;
    }

    public List<TimedValue> getHistoricalData(String myxID, String key, Date fromTime, Date toTime) {
        List<TimedValue> timedValues = new ArrayList<TimedValue>();
        if (parametersCollection_ != null) {
            FindIterable<Document> docs = parametersCollection_.find(and(eq(MYXID, myxID), eq(KEY, key), gte(TIME, fromTime.getTime()), lte(TIME, toTime.getTime()))).sort(ascending(TIME));
            if (docs != null) {
                for (Document doc : docs) {
                    long time = doc.getLong(TIME);
                    Object value = doc.get(VALUE);
                    timedValues.add(new TimedValue(time, value));
                }
            }
        }

        return timedValues;
    }

    public void doSilenceAlarm(long silenceTolerance) {
        FindIterable<Document> docs = notificationCollection_.find(lte(LAST_INFO, new Date().getTime()-silenceTolerance));
        if (docs != null) {
            for (Document doc : docs) {
                String myxID = doc.getString(MYXID);
                int nNotified = doc.getInteger(SILENCE_NOTIFIED, 0);
                if (nNotified < silenceMaxNotifications_) {
                    if (pushNotificator_.send(myxID, "Telo is silent"))
                        notificationsSent(myxID, SILENCE_NOTIFIED);

                    // add "obsolete" or "empty" values to the DB
                    newPseudoValue(myxID, CARRIER, EMPTY_VALUE);
                    newPseudoValue(myxID, CELL_ID, EMPTY_VALUE);
                    newPseudoValue(myxID, BAND, EMPTY_VALUE);
                    newPseudoValue(myxID, BANDWIDTH, EMPTY_VALUE);
                    newPseudoValue(myxID, RX_CHANNEL, EMPTY_VALUE);
                    newPseudoValue(myxID, TX_CHANNEL, EMPTY_VALUE);
                    newPseudoValue(myxID, TX_POWER, EMPTY_VALUE);
                    newPseudoValue(myxID, RSSI, EMPTY_VALUE);
                    newPseudoValue(myxID, RSRP, EMPTY_VALUE);
                    newPseudoValue(myxID, RSRQ, EMPTY_VALUE);
                    newPseudoValue(myxID, SINR, EMPTY_VALUE);
                    newPseudoValue(myxID, IP_ADDRESS, EMPTY_VALUE);
                    newPseudoValue(myxID, IP_MASK, EMPTY_VALUE);
                    newPseudoValue(myxID, GATEWAY, EMPTY_VALUE);
                    newPseudoValue(myxID, CONNECTION_TYPE, OBSOLETE_VALUE);
                    newPseudoValue(myxID, OOMA_SERVICE_STATUS, OBSOLETE_VALUE);
                    newPseudoValue(myxID, LTE_DONGLE, LTE_DONGLE_INACTIVE);
                    newPseudoValue(myxID, SIGNAL_QUALITY, SIGNAL_QUALITY_NO);
                    newPseudoValue(myxID, LTE_STATE, LTE_STATE_DOWN);
                }
            }
        }
    }

    // once a minute after 10 minutes, to give them chance to put some info about them and suppress "silence" pushes
    @Scheduled(initialDelay = 10*60*1000, fixedRate = 60*1000)
    public void silenceAlarm() {
        if (notificationCollection_ != null) {
            doSilenceAlarm(silenceTolerance_*ONE_MINUTE);
        }
    }

    // once a day
    @Scheduled(fixedRate = 60*60*1000) // once an hour
    public void cleanupDB() {
        LOGGER.info("DB cleanup");
        if (parametersCollection_ != null) {
            // remove obsolete data
            try {
                DeleteResult result = parametersCollection_.deleteMany(lte(TIME, new Date().getTime() - mongoCleanupTime_*ONE_DAY));
                LOGGER.info(result.getDeletedCount() + " records deleted");
            } catch (MongoException e) {
                e.printStackTrace();
                LOGGER.error(e.getMessage());
            }
        }
    }

    private MongoClient mongoClient_ = null;
    private MongoDatabase db_ = null;
    private MongoCollection<Document> notificationCollection_ = null; // myxID, the time of latest telemetric info arrival, and how many times user was notified that some event happened
    private MongoCollection<Document> parametersCollection_ = null; // the collection of parameters with theit values and timestamps
}

