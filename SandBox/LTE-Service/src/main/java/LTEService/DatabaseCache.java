package LTEService;

import org.json.*;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.util.HashMap;
import java.util.Iterator;

@Component
@Scope("singleton")
public class DatabaseCache {

    @PostConstruct
    public void initialize() {
        newValue("2267EE", "ooma_service_status", "live");
        newValue("2267EE", "connection_type", "lte");
        newValue("2267EE", "signal_quality", "37");
        newValue("2267EE", "lte_dongle", "active");
        newValue("2267EE", "lte_state", "up");

        newValue("39E3F0", "ooma_service_status", "live");
        newValue("39E3F0", "connection_type", "lte");
        newValue("39E3F0", "signal_quality", "38");
        newValue("39E3F0", "lte_dongle", "active");
        newValue("39E3F0", "lte_state", "up");

        newValue("39E35E", "ooma_service_status", "live");
        newValue("39E35E", "connection_type", "LTE");
        newValue("39E35E", "signal_quality", "39");
        newValue("39E35E", "lte_dongle", "active");
        newValue("39E3F0", "lte_state", "up");
    }

    public void newData(String jsonStr)
    {
        final String MYXID_STRING = "myx_id";
        final String VERSION_STRING = "version";
        try {
            JSONObject json = new JSONObject(jsonStr);
            String myxID = json.getString(MYXID_STRING);
            if (myxID == null || myxID.isEmpty())
                return; // do nothing

            Iterator<String> keysItr = json.keys();
            while (keysItr.hasNext())
            {
                String paramName = keysItr.next();
                if (paramName.equalsIgnoreCase(VERSION_STRING))
                    continue;
                if (paramName.equalsIgnoreCase(MYXID_STRING))
                    continue;
                String value = json.get(paramName).toString();
                newValue(myxID, paramName, value);
            }
        } catch (JSONException e) {
            //LOGGER
            System.out.println("Invalid JSON data: " + System.lineSeparator() + jsonStr + e.getMessage());
        }
    }

    public void newValue(String myxID, String paramName, String value) {
        LteModemState state = find(myxID);
        if (state == null) {
            state = new LteModemState();
        }
        state.newValue(paramName, value);

        dbCache_.put(myxID, state);
        // TODO: update MongoDB
    }

    public LteModemState find(String myxID) {
        return dbCache_.get(myxID);
    }

    protected HashMap<String, LteModemState> dbCache_ = new HashMap<String, LteModemState>(); // key is myxID
}


