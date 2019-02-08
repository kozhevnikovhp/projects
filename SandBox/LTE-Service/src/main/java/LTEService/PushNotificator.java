package LTEService;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.util.EntityUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import javax.annotation.PostConstruct;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.IOException;
import java.net.MalformedURLException;

@Component
@Scope("singleton")
public class PushNotificator {
    private static final Logger LOGGER = LogManager.getLogger(PushNotificator.class);

    @Value("${spring.push.url}")
    private String pushServerURL_;

    @PostConstruct
    public void initialize() {
    }

    public boolean sendNotification(String strData) {
        LOGGER.info("Push notification: {}", strData);

        HttpResponse response = null;
        try {
            DefaultHttpClient httpClient = new DefaultHttpClient();
            HttpPost postRequest = new HttpPost(pushServerURL_);
            StringEntity input = new StringEntity(strData);
            input.setContentType("application/json");
            postRequest.setEntity(input);

            response = httpClient.execute(postRequest);

            boolean bSuccess = response.getStatusLine().getStatusCode() != 201;
            LOGGER.info("Status Code: {}", response.getStatusLine().getStatusCode());

            httpClient.getConnectionManager().shutdown();
            return bSuccess;
        } catch (MalformedURLException e) {
            return false;
        } catch (IOException e) {
            return false;
        } finally {
            if (response != null) {
                try {
                    EntityUtils.consume(response.getEntity());
                } catch (Exception e) {
                    LOGGER.error(e.getMessage());
                }
            }
        }
    }

    public boolean send(String myxID, String strText) {
        JSONObject payloadJsonObject = new JSONObject();
        payloadJsonObject.put("title", strText);

        JSONObject jo = new JSONObject();
        jo.put("name", myxID);
        jo.put("type", "LteModemStatus");
        jo.put("app_name", "com.ooma.TeloSetup");
        jo.put("text", strText);
        jo.put("sound", "default");
        jo.put("badge", 0);
        jo.put("payload", payloadJsonObject);

        return sendNotification(jo.toString(1));
    }
}
