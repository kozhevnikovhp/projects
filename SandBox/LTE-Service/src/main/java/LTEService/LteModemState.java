package LTEService;

import java.util.HashMap;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;
import java.io.IOException;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;


public class LteModemState {
    public LteModemState() {  }

    public String getParamValue(String paramName) {
        return paramName+" "+paramName;
    }

    public void newValue(String paramName, String value) {
        stateHolder_.put(paramName, value);
    }

    public String getValue(String paramName) {
        String value = stateHolder_.get(paramName);
        if (value == null)
            return "NotFound";
        return value;
    }

    // paramNames is comma-separated set of parameter names
    public String getValues(String paramNamesJSON) throws IOException {

        ObjectMapper objectMapper = new ObjectMapper();
        byte[] bytes = paramNamesJSON.getBytes();
        JsonNode rootNode = objectMapper.readTree(bytes);
        List<String> params = new ArrayList<String>();
        JsonNode parametersNode = rootNode.path("parameters");
        Iterator<JsonNode> elements = parametersNode.elements();
        while (elements.hasNext()) {
            JsonNode paramNode = elements.next();
            System.out.println(" param = " + paramNode.asText());
            params.add(paramNode.asText());
        }

        return getValues(params);
    }

    public String getValues(List<String> params) {
        boolean bFirst = true;
        String retValue = "    parameters:" + System.lineSeparator() + "    {";
        for (String param : params) {
            if (!bFirst)
                retValue += ", ";
            bFirst = false;
            retValue += System.lineSeparator();
            retValue += "        " + param + ":";
            retValue += getValue(param);
        }
        retValue += System.lineSeparator() + "    }" + System.lineSeparator();
        return retValue;
    }

    protected HashMap<String,String> stateHolder_ = new HashMap<String,String>();
}
