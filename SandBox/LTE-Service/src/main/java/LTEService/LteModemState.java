package LTEService;

import java.util.HashMap;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;
import java.io.IOException;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.json.JSONObject;


public class LteModemState {
    public LteModemState() {  }

    public void newValue(String paramName, String value)
    {
        if (paramName.equalsIgnoreCase("sent_bytes_telo" ) ||
            paramName.equalsIgnoreCase("received_bytes_telo") ||
            paramName.equalsIgnoreCase("sent_bytes_user" ) ||
            paramName.equalsIgnoreCase("received_bytes_user")
            )
        { // special processing! Bytes are being sent incrementally, we cannot replace, but have to increment
            String oldValue = stateHolder_.get(paramName);
            if (oldValue == null)
            { // not found, just put it there
                stateHolder_.put(paramName, value);
            }
            else
            { // convert to integer, increment, and put it back
                int valueInDB = Integer.parseInt(oldValue);
                int increment = Integer.parseInt(value);
                valueInDB += increment;
                stateHolder_.put(paramName, Integer.toString(valueInDB));
           }
        }
        else
            stateHolder_.put(paramName, value); //easy star all star
    }

    public String getValue(String paramName)
    {
        String value = stateHolder_.get(paramName);
        if (value == null)
            return "not found";
        return value;
    }

    // paramNames is comma-separated set of parameter names
    public JSONObject getValues(JsonNode rootNode)
    {
        List<String> params = new ArrayList<String>();
        JsonNode parametersNode = rootNode.path("parameters");
        if (parametersNode != null)
        {
            Iterator<JsonNode> elements = parametersNode.elements();
            while (elements.hasNext()) {
                JsonNode paramNode = elements.next();
                params.add(paramNode.asText());
            }
        }

        return getValues(params);
    }

    public JSONObject getValues(List<String> params) {
        boolean bFirst = true;
        JSONObject json = new JSONObject();
        for (String param : params) {
            json.put(param, getValue(param));
        }
        return json;
    }

    protected HashMap<String,String> stateHolder_ = new HashMap<String,String>();
}
