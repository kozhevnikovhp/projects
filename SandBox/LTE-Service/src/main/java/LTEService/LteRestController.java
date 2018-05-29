package LTEService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;

import java.io.IOException;

@RestController
public class LteRestController {
    @Autowired
    DatabaseCache databaseCache_;

    @RequestMapping("/v1/device_info")
    public String status(@RequestParam(value="myxID") String myxID, @RequestBody String paramsJSON) throws IOException {
        LteModemState state = databaseCache_.find(myxID);
        if (state == null) {
            // TODO: fetch in MongoDB
            return "myxID '" + myxID + "' not found";
        }
        System.out.println("myxID = " + myxID);
        System.out.println("params = " + paramsJSON);
        String outputJSON = "{" + System.lineSeparator();
        outputJSON += state.getValues(paramsJSON);
        outputJSON += "    \"status\": 200," + System.lineSeparator();
        outputJSON += "    \"verified\": true," + System.lineSeparator();
        outputJSON += "    \"success\": true," + System.lineSeparator();
        outputJSON += "    \"error\": \"no_error\"," + System.lineSeparator();
        outputJSON += "    \"message\": \"no_message\"" + System.lineSeparator();
        outputJSON += "}";
        System.out.println("output = " + outputJSON);
        return outputJSON;
    }
}
