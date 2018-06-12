package LTEService;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.json.JSONObject;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import javax.annotation.PostConstruct;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.IOException;
import java.net.MalformedURLException;

@Component
@Scope("singleton")
public class PushNotificator {
    protected String pushServerURL_;

    @PostConstruct
    public void initialize() {
        // TODO: configure here
        pushServerURL_= "http://10.0.2.15:7000/api/v1/send";
    }

    public boolean sendNotification(String strData) {
        System.out.println("Notification body = " + strData);
        try {
            DefaultHttpClient httpClient = new DefaultHttpClient();
            HttpPost postRequest = new HttpPost(pushServerURL_);

            StringEntity input = new StringEntity(strData);
            input.setContentType("application/json");
            postRequest.setEntity(input);

            HttpResponse response = httpClient.execute(postRequest);

            boolean bSuccess = response.getStatusLine().getStatusCode() != 201;

            httpClient.getConnectionManager().shutdown();
            return true;
        } catch (MalformedURLException e) {
            return false;
        } catch (IOException e) {
             return false;
        }
    }
    public boolean send(String strTitle, String strBody, String myxID, String strText) {
        JSONObject payloadJsonObject = new JSONObject();
        payloadJsonObject.put("title", strTitle);
        payloadJsonObject.put("body", strBody);
        payloadJsonObject.put("my-customer", myxID);
        payloadJsonObject.put("action-loc-key", "loc-key");

        JSONObject jo = new JSONObject();
        jo.put("name", "someid");
        jo.put("type", "LteModemStatus");
        jo.put("app_name", "com.ooma.TeloSetup");
        jo.put("text", strText);
        jo.put("sound", "default");
        jo.put("badge", 0);
        jo.put("payload", payloadJsonObject);

        return sendNotification(jo.toString(1));
    }
}
