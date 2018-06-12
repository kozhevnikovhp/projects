package LTEService;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;

import java.io.IOException;
import java.util.Iterator;

@RestController
public class LteRestController {
    @Autowired
    DatabaseCache databaseCache_;

    @RequestMapping(value = "/v1/device_info", produces = "application/json")
    public String status(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) {
        int errorCode = 200; // OK
        String errorMessage = "";
        String message = "";
        boolean bSuccess = true;
        LteModemState state = databaseCache_.find(myxID);
        if (state == null) {
            // TODO: fetch in MongoDB
            errorCode = 404;
            errorMessage = "myx_id '" + myxID + "' not found";
        }

        JSONObject json = new JSONObject();
        try {
            ObjectMapper objectMapper = new ObjectMapper();
            byte[] bytes = paramsJSON.getBytes();
            JsonNode rootNode = objectMapper.readTree(bytes);
            if (state != null)
                json.put("parameters", state.getValues(rootNode));
        } catch (IOException e) {
             errorCode = 401;
             bSuccess = false;
             errorMessage = e.getMessage();
             System.out.println("Invalid JSON " + errorMessage);
        }

        json.put("status", errorCode);
        json.put("verified", true);
        json.put("success", bSuccess);
        json.put("error", errorMessage);
        json.put("message", message);

        System.out.println("New request:" + System.lineSeparator());
        System.out.println("myx_id = " + myxID);
        System.out.println("params = " + System.lineSeparator() + paramsJSON);
        System.out.println("output = " + System.lineSeparator() + json.toString());

        return json.toString();
    }
}
