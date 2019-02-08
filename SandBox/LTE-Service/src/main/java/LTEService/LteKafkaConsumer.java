package LTEService;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.stereotype.Service;


@Service
public class LteKafkaConsumer
{
    private static final Logger LOGGER = LogManager.getLogger(LteKafkaConsumer.class);

    @Autowired
    private Database database_;

    @KafkaListener(groupId = "${spring.kafka.groupId}", topics = "${spring.kafka.topic}")
    public void listen(String message)
    {
        LOGGER.info("Received Message: {}", message);
        try {
            database_.newData(message);
        }
        catch (Exception e){
            LOGGER.error(e);
        }
    }
}
