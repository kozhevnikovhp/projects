package LTEService;

import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.util.HashMap;

@Component
@Scope("singleton")
public class DatabaseCache {

    @PostConstruct
    public void initialize() {
        newValue("2267EE", "OomaServiceStatusEnum", "Live");
        newValue("2267EE", "ConnectionType", "LTE");
        newValue("2267EE", "SignalStrength", "37");
        newValue("2267EE", "LteStatus", "Active");

        newValue("39E3F0", "OomaServiceStatusEnum", "Live");
        newValue("39E3F0", "ConnectionType", "LTE");
        newValue("39E3F0", "SignalStrength", "38");
        newValue("39E3F0", "LteStatus", "Active");

        newValue("39E35E", "OomaServiceStatusEnum", "Live");
        newValue("39E35E", "ConnectionType", "LTE");
        newValue("39E35E", "SignalStrength", "39");
        newValue("39E35E", "LteStatus", "Active");
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


