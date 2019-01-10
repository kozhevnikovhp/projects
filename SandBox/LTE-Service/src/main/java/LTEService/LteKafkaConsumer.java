package LTEService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.stereotype.Service;

@Service
public class LteKafkaConsumer {
    @Autowired
    DatabaseCache databaseCache_;
    @Autowired
    PushNotificator pushNotificator_;

    @KafkaListener(id = "foo", topics = "lte-service")
    public void listen(String message) {
        System.out.println("Received Message: " + message);
        databaseCache_.newData(message);

        //pushNotificator_.send("myTitle", message, "2267EE", "go home, it's too late!");
    }
}
