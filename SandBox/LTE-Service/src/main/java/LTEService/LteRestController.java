package LTEService;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.PropertySource;
import org.springframework.web.bind.annotation.*;

import javax.annotation.PostConstruct;
import java.io.IOException;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.TimeZone;

@Configuration
@PropertySource("classpath:build.properties")
@RestController
public class LteRestController {

    private long ONE_SECOND = 1000; // time in msec

    class ReqStatInterval {

        private int nRequests_;
        private long lastTime_;

        public ReqStatInterval() {
            nRequests_ = 0;
            lastTime_ = 0;
        }

        public void newRequest(long time) {
            if ((time - lastTime_) > ONE_SECOND)
                nRequests_ = 0;
            ++nRequests_;
            lastTime_ = time;
        }

        public int getRequests() {
            return nRequests_;
        }

        public long getLastTime() {
            return lastTime_;
        }
    };

    private static final int STATUS_CODE_OK = 200;
    private static final int STATUS_CODE_NOT_OK = 401;

    private static final String STATUS = "status";
    private static final String VERIFIED = "verified";
    private static final String SUCCESS = "success";
    private static final String ERROR = "error";
    private static final String MESSAGE = "message";

    private static final String UNKNOWN_VALUE = "unknown";
    private static final String EMPTY_VALUE = "";

    private static final String TIMESTAMP = "timestamp";

    private static final Logger LOGGER = LogManager.getLogger(LteRestController.class);

    private static final int N_INTERVALS = 60;
    private ReqStatInterval[] processedRequests_ = new ReqStatInterval[N_INTERVALS];

    private DateFormat dateFormat_ = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss z");

    @Value("${build.version}")
    private String version_;

    @Value("${build.revision}")
    private String build_;

    private Date buildTimestamp_;
    @Value("${build.timestamp}")
    public void setBuildTimestamp(String timestamp) {
        buildTimestamp_ = new Date(0);
        try {
            buildTimestamp_ = new Date(Long.parseLong(timestamp));
        } catch (Exception e) {
            e.printStackTrace();
            LOGGER.error(e);
        }
    }

    public LteRestController() {
        dateFormat_.setTimeZone(TimeZone.getTimeZone("UTC"));
    }

    @Autowired
    private Database database_;

    @PostConstruct
    public void initialize() {
        LOGGER.info("Version {}, compiled on {}", version_, buildTimestamp_);
        for (int i = 0; i < N_INTERVALS; ++i)
            processedRequests_[i] = new ReqStatInterval();
    }

    private void requestProcessed() {
        long time = new Date().getTime();
        int second = (int)(time % N_INTERVALS);
        processedRequests_[second].newRequest(time);
    }

    @RequestMapping(value = "version", produces = "application/json", method = RequestMethod.GET)
    public String version() {
        JSONObject json = new JSONObject();

        json.put("version", version_);
        json.put("build", build_);
        json.put("timestamp", dateFormat_.format(buildTimestamp_));

        json.put(STATUS, STATUS_CODE_OK);
        json.put(VERIFIED, true);
        json.put(SUCCESS, true);
        json.put(ERROR, EMPTY_VALUE);
        json.put(MESSAGE, EMPTY_VALUE);

        requestProcessed();

        LOGGER.info("Version reply: {}", json.toString());
        return json.toString();
    }

    @RequestMapping(value = "load", produces = "application/json", method = RequestMethod.GET)
    public String load() {
        long time = new Date().getTime();
        int nReqs = 0;
        for (int i = 0; i < N_INTERVALS; ++i) {
            if ((time - processedRequests_[i].getLastTime()) < N_INTERVALS*ONE_SECOND) {
                nReqs += processedRequests_[i].getRequests();
            }
        }

        JSONObject json = new JSONObject();

        json.put("load", nReqs);

        json.put(STATUS, STATUS_CODE_OK);
        json.put(VERIFIED, true);
        json.put(SUCCESS, true);
        json.put(ERROR, EMPTY_VALUE);
        json.put(MESSAGE, EMPTY_VALUE);

        requestProcessed();

        LOGGER.info("Load reply: {}", json.toString());

        return json.toString();
    }

    private String statusRequest(String myxID, String paramsJSON) {
        LOGGER.info("Status request: myx_id: {}, params: {}", myxID, paramsJSON);

        requestProcessed();

        int statusCode = STATUS_CODE_OK; // OK
        String errorMessage = EMPTY_VALUE;
        String message = EMPTY_VALUE;
        boolean bSuccess = true;
        JSONObject json = new JSONObject();
        Database.TimedJson status = null;
        try {
            ObjectMapper objectMapper = new ObjectMapper();
            byte[] bytes = paramsJSON.getBytes();
            JsonNode rootNode = objectMapper.readTree(bytes);
            JsonNode parametersNode = rootNode.path("parameters");
            if (parametersNode != null) {
                status = database_.getStatus(myxID, parametersNode);
                if (status != null)
                    json.put("parameters", status.getJson());
            }
            else {
                statusCode = STATUS_CODE_NOT_OK;
                bSuccess = false;
                errorMessage = "Absent 'parameters' section in query";
                LOGGER.info("Invalid JSON {}", errorMessage);
            }
        }
        catch (IOException e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.info("Invalid JSON {}", errorMessage);
        }
        catch (Exception e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Internal error: {}", errorMessage);
        }

        // Add timestamp - the latest value for this myxid in human-readable format
        if (status != null) {
            json.put(TIMESTAMP, status.getTime());
            json.put("timestamp_", dateFormat_.format(new Date(status.getTime())));
        }

        json.put(STATUS, statusCode);
        json.put(VERIFIED, true);
        json.put(SUCCESS, bSuccess);
        json.put(ERROR, errorMessage);
        json.put(MESSAGE, message);

        LOGGER.info("Status reply: myx_id: {}, reply: {}", myxID, json.toString());

        return json.toString();
    }

    @RequestMapping(value = "/v1/telo/lteservice", produces = "application/json", method = RequestMethod.POST)
    public String statusOld(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) {
        return statusRequest(myxID, paramsJSON);
    }

    @RequestMapping(value = "/v1/telo/lteservice/status", produces = "application/json", method = RequestMethod.POST)
    public String statusNew(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) {
        return statusRequest(myxID, paramsJSON);
    }

    @RequestMapping(value = "/v1/telo/lteservice/historical_data", produces = "application/json", method = RequestMethod.GET)
    public String historicalData(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) {
        LOGGER.info("Historic data request: myx_id: {}, params: {}", myxID, paramsJSON);

        requestProcessed();

        int statusCode = STATUS_CODE_OK;
        String errorMessage = EMPTY_VALUE;
        String message = EMPTY_VALUE;
        boolean bSuccess = true;
        JSONObject json = new JSONObject();
        try {
            JSONObject resultJson = new JSONObject();

            ObjectMapper objectMapper = new ObjectMapper();
            byte[] bytes = paramsJSON.getBytes();
            JsonNode rootNode = objectMapper.readTree(bytes);

            JsonNode fromTimeNode = rootNode.path("start_time");
            JsonNode toTimeNode = rootNode.path("end_time");
            JsonNode parametersNode = rootNode.path("parameters");
            if (fromTimeNode != null && toTimeNode != null && parametersNode != null) {
                SimpleDateFormat formatter = new SimpleDateFormat("dd-MMM-yyyy HH:mm:ss");

                Date fromTime = formatter.parse(fromTimeNode.asText());
                Date toTime = formatter.parse(toTimeNode.asText());
                json.put("start_time", formatter.format(fromTime));
                json.put("end_time", formatter.format(toTime));

                Iterator<JsonNode> elements = parametersNode.elements();
                while (elements.hasNext()) {
                    JsonNode paramNode = elements.next();
                    String key = paramNode.asText();
                    List<Database.TimedValue> timedValues = database_.getHistoricalData(myxID, key, fromTime, toTime);
                    JSONArray jsonValuesOneKey = new JSONArray(timedValues);
                    int iIndex = 0;
                    for (Database.TimedValue value : timedValues) {
                        jsonValuesOneKey.put(iIndex, value.toJSON());
                        ++iIndex;
                    }
                    resultJson.put(key, jsonValuesOneKey);
                }
            }
            json.put("parameters", resultJson);
        }
        catch (IOException e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Invalid JSON {}", errorMessage);
        }
        catch (ParseException e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Cannot parse date/time {}", errorMessage);
        }
        catch (Exception e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Internal error: {}", errorMessage);
        }

        json.put(STATUS, statusCode);
        json.put(VERIFIED, true);
        json.put(SUCCESS, bSuccess);
        json.put(ERROR, errorMessage);
        json.put(MESSAGE, message);

        String reply = json.toString();
        if (reply.length() > 1000)
            LOGGER.info("Historic data reply: myx_id: {}, reply: { <VERY_BIG_DATA of length {}> omited }", myxID, reply.length()); // not to break down logging system with huge replies
        else
            LOGGER.info("Historic data reply: myx_id: {}, reply: {}", myxID, reply);

        return json.toString();
    }

    @RequestMapping(value = "/v1/telo/lteservice/stats_time_range", produces = "application/json", method = RequestMethod.GET)
    public String queryStatistics(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) {
        LOGGER.info("Traffic statistics request: myx_id: {}, params: {}", myxID, paramsJSON);

        requestProcessed();

        int statusCode = STATUS_CODE_OK;
        String errorMessage = EMPTY_VALUE;
        String message = EMPTY_VALUE;
        boolean bSuccess = true;
        JSONObject json = new JSONObject();
        try {
            JSONObject resultJson = new JSONObject();

            ObjectMapper objectMapper = new ObjectMapper();
            byte[] bytes = paramsJSON.getBytes();
            JsonNode rootNode = objectMapper.readTree(bytes);

            JsonNode fromTimeNode = rootNode.path("start_time");
            JsonNode toTimeNode = rootNode.path("end_time");
            JsonNode parametersNode = rootNode.path("parameters");
            if (fromTimeNode != null && toTimeNode != null && parametersNode != null) {
                SimpleDateFormat formatter = new SimpleDateFormat("dd-MMM-yyyy HH:mm:ss");

                Date fromTime = formatter.parse(fromTimeNode.asText());
                Date toTime = formatter.parse(toTimeNode.asText());
                json.put("start_time", formatter.format(fromTime));
                json.put("end_time", formatter.format(toTime));

                Iterator<JsonNode> elements = parametersNode.elements();
                while (elements.hasNext()) {
                    JsonNode paramNode = elements.next();
                    String key = paramNode.asText();
                    int nBytes = database_.getTrafficStatistics(myxID, key, fromTime, toTime);
                    resultJson.put(key, nBytes);
                }
            }
            json.put("parameters", resultJson);
        }
        catch (IOException e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Invalid JSON {}", errorMessage);
        }
        catch (ParseException e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Cannot parse date/time {}", errorMessage);
        }
        catch (Exception e) {
            statusCode = STATUS_CODE_NOT_OK;
            bSuccess = false;
            errorMessage = e.getMessage();
            LOGGER.error("Internal error: {}", errorMessage);
        }

        json.put(STATUS, statusCode);
        json.put(VERIFIED, true);
        json.put(SUCCESS, bSuccess);
        json.put(ERROR, errorMessage);
        json.put(MESSAGE, message);

        LOGGER.info("Traffic statistics reply: myx_id: {}, reply: {}", myxID, json.toString());

        return json.toString();
    }

    @RequestMapping(value = "/v1/telo/lteservice/countersreset", produces = "application/json", method = RequestMethod.POST)
    public String resetStat(@RequestParam(value="myxID") String myxID) {
        requestProcessed();
        database_.resetStats(myxID);

        JSONObject json = new JSONObject();
        json.put(STATUS, STATUS_CODE_OK);
        json.put(VERIFIED, true);
        json.put(SUCCESS, true);
        json.put(ERROR, EMPTY_VALUE);
        json.put(MESSAGE, EMPTY_VALUE);

        LOGGER.info("Reset stats: {} myx_id: {} output: {}", myxID, json.toString());

        return json.toString();
    }
}
