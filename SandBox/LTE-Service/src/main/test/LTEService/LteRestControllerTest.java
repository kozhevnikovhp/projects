package LTEService;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;
import org.springframework.test.context.junit4.SpringRunner;
import org.springframework.test.context.web.WebAppConfiguration;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.result.MockMvcResultMatchers;
import org.springframework.test.web.servlet.setup.MockMvcBuilders;
import org.springframework.web.context.WebApplicationContext;

import static org.hamcrest.Matchers.containsString;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;

@SpringBootTest
@RunWith(SpringRunner.class)
public class LteRestControllerTest {

    private static final long ONE_SECOND = 1000; // time in msec
    private static final long ONE_MINUTE = 60*ONE_SECOND; // time in msec

    private static final String TRUE = "true";
    private static final String FALSE = "false";
    private static final int STATUS_CODE_OK = 200;
    private static final int STATUS_CODE_NOT_OK = 401;

    private static final String SUCCESS = "$.success";
    private static final String STATUS = "$.status";
    private static final String VERIFIED = "$.verified";

    @Autowired
    private WebApplicationContext webApplicationContext_;

    @InjectMocks
    private LteRestController controller_;

    private MockMvc mockMvc_;

    @Before
    public void setup() {

        // Process mock annotations
        MockitoAnnotations.initMocks(this);
        this.mockMvc_ = MockMvcBuilders.webAppContextSetup(webApplicationContext_).build();
    }

    @Test
    public void version() {
        try {
            mockMvc_.perform(get("/version")
                    .contentType(MediaType.APPLICATION_JSON))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath("$.version").value("0.1.0"))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void load() {
        try {
            Thread.sleep(ONE_MINUTE); // to avoid strangers from the past, load is calculated for last 1 minute
            Thread.sleep(ONE_SECOND); // and a little bit more
        } catch(InterruptedException ex) {
            Thread.currentThread().interrupt();
        }

        try {
            mockMvc_.perform(get("/load")
                    .contentType(MediaType.APPLICATION_JSON))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath("$.load").value(0))
            ;
            Thread.sleep(2*ONE_SECOND);
            mockMvc_.perform(get("/load")
                    .contentType(MediaType.APPLICATION_JSON))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath("$.load").value(1))
            ;
        } catch(InterruptedException ex) {
            Thread.currentThread().interrupt();
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void status() {
        try {
            mockMvc_.perform(post("/v1/telo/lteservice")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{\"parameters\": [\"sinr\", \"signal_quality\", \"rsrp\" ] }"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("sinr")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("signal_quality")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("rsrp")))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void historicalData() {
        try {
            mockMvc_.perform(get("/v1/telo/lteservice/historical_data")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{ \"parameters\": [\"sinr\", \"signal_quality\", \"tx_power\", \"rsrp\", \"rsrq\" ], " +
                            "\"start_time\":\"3-JuL-2018 11:35:00\", " +
                            "\"end_time\":\"4-jul-2022 18:20:00\" }"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("sinr")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("signal_quality")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("tx_power")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("rsrp")))
                    .andExpect(MockMvcResultMatchers.content().string(containsString("rsrq")))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void status_InvalidJson() {
        // extra opening square bracket
        try {
            mockMvc_.perform(post("/v1/telo/lteservice")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{\"parameters\": [[\"sinr\", \"signal_quality\", \"rsrp\" ] }"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(FALSE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_NOT_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void queryStatistics() {
        try {
            mockMvc_.perform(get("/v1/telo/lteservice/stats_time_range")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{ \"parameters\": [\"telo_ooma_apps_lte_sent_bytes\", \"telo_ooma_apps_lte_received_bytes\" ], " +
                            "\"start_time\":\"3-JuL-2018 11:35:00\", " +
                            "\"end_time\":\"4-jul-2018 18:20:00\" }"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void queryStatistics_InvalidJSON() {
        // Closing curly bracket } is absent
        try {
            mockMvc_.perform(get("/v1/telo/lteservice/stats_time_range")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{ \"parameters\": [\"telo_ooma_apps_lte_sent_bytes\", \"telo_ooma_apps_lte_received_bytes\" ], " +
                            "\"start_time\":\"3-JuL-2018 11:35:00\", " +
                            "\"end_time\":\"4-jul-2018 18:20:00\" "))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(FALSE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_NOT_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void queryStatistics_InvalidDataFormat() {
        // data and time are invalid
        try {
            mockMvc_.perform(get("/v1/telo/lteservice/stats_time_range")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000")
                    .content("{ \"parameters\": [\"telo_ooma_apps_lte_sent_bytes\", \"telo_ooma_apps_lte_received_bytes\" ], " +
                            "\"start_time\":\"3-JJJ-2018 11:35:00\", " +
                            "\"end_time\":\"4-jul-2018 18:20:00\" }"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(FALSE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_NOT_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    @Test
    public void resetStat() {
         try {
            mockMvc_.perform(post("/v1/telo/lteservice/countersreset")
                    .contentType(MediaType.APPLICATION_JSON)
                    .param("myxID", "000000"))
                    .andExpect(MockMvcResultMatchers.status().isOk())
                    .andExpect(MockMvcResultMatchers.jsonPath(SUCCESS).value(TRUE))
                    .andExpect(MockMvcResultMatchers.jsonPath(STATUS).value(STATUS_CODE_OK))
                    .andExpect(MockMvcResultMatchers.jsonPath(VERIFIED).value(TRUE))
            ;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }
}