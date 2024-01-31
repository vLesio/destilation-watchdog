#include <stdio.h> //for basic printf commands
#include <string.h> //for handling strings
#include "freertos/FreeRTOS.h" //for delay,mutexs,semphrs rtos operations
#include <freertos/task.h>
#include "esp_system.h" //esp_init funtions esp_err_t 
#include "esp_wifi.h" //esp_wifi_init functions and wifi operations
#include "esp_log.h" //for showing logs
#include "esp_event.h" //for wifi event
#include "nvs_flash.h" //non volatile storage
#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_http_client.h"

#include "d_mqtt.h"
#include "mqtt_client.h"
#include "wifi.h"

esp_mqtt_client_handle_t client;
char Topic[22];

char IsMqttConnected = 0;

const char* server_common_name = "20.82.177.27";

const char ca_authority[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDizCCAnOgAwIBAgIUOSDcjaAAo4w06cIsisv40ItZe2YwDQYJKoZIhvcNAQEL\n" \
"BQAwVTELMAkGA1UEBhMCUEwxDzANBgNVBAgMBlBvbGFuZDEPMA0GA1UEBwwGQ3Jh\n" \
"Y293MRIwEAYDVQQKDAlQcnplQ2h1amUxEDAOBgNVBAMMB2NhX2NlcnQwHhcNMjQw\n" \
"MTIyMDMyNDMwWhcNMjkwMTIxMDMyNDMwWjBVMQswCQYDVQQGEwJQTDEPMA0GA1UE\n" \
"CAwGUG9sYW5kMQ8wDQYDVQQHDAZDcmFjb3cxEjAQBgNVBAoMCVByemVDaHVqZTEQ\n" \
"MA4GA1UEAwwHY2FfY2VydDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AJs+eHVym8njwS82aC3FiGceiNF5ywTVmd4tgH8kUOR70vo0HcLMA8NSJf64grcv\n" \
"8L+lqmdKUI7bMrJQWTlkJy5pBv7uBs05V5YfYeIR1R1zBhxYd80qzIQjDxH+IBlW\n" \
"r2vWeF7gJZ1RLG0SCI3E+HcS/agPKDbDD3XSVxXetE6osp120E4K7O0u3IB7AEMa\n" \
"vocW5WZcUjKp+CEzdT9jpRdEOFCGcXafcZ/GoO4XZRVj8ojiIp7njZmbypdoXh5e\n" \
"2AgrdgE1QBMe03JiuE3a5xElzKmMw5VFfzTpumm0mEfSkGgqJ9h0YVuZsCTsxGc8\n" \
"N6RD0hlc96hhV4d8u+CRPE0CAwEAAaNTMFEwHQYDVR0OBBYEFHOmt4B3v6Gp4mFl\n" \
"Xy2GIyAS8qpbMB8GA1UdIwQYMBaAFHOmt4B3v6Gp4mFlXy2GIyAS8qpbMA8GA1Ud\n" \
"EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBACCPVjWZ6CL93VUOSCX63rYF\n" \
"jCrP6uccL7p3qoZ3RCWNDusBW/mBHKS2xyMv+2/AjTNqFw0D2ieP+QqDg9cNYrBm\n" \
"5F+MjtmgCAwzWJdZBB+HTuqAO/n0MvBpUxyILIovTlpxfiwH7AT/p5iol0Ui2agd\n" \
"OPMgXVLu3YXaR2ju2xMwU16vcg+CffV+Yvi1li8m6BOPAzAoUiH6KXsXcQXV6Dzg\n" \
"8FZT8F8npRLtcPWSM/IX8hZ8JvnwYDm4uwkdRmSSSKRPLDnAoiIYyPD0NkDcJYuq\n" \
"BM0BAuGZewb2JvFV0KErclHjbfCkssqRN4LfEuhD5vEfWgdHT306T6PpI+Nm4rE=\n" \
"-----END CERTIFICATE-----";

const char client_crt[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDPDCCAiQCFErqoiO3tHIIxgqy2/SU+xXz28adMA0GCSqGSIb3DQEBCwUAMFUx\n" \
"CzAJBgNVBAYTAlBMMQ8wDQYDVQQIDAZQb2xhbmQxDzANBgNVBAcMBkNyYWNvdzES\n" \
"MBAGA1UECgwJUHJ6ZUNodWplMRAwDgYDVQQDDAdjYV9jZXJ0MB4XDTI0MDEyMjAz\n" \
"NTMzNVoXDTM0MDExOTAzNTMzNVowYDELMAkGA1UEBhMCUEwxDzANBgNVBAgMBlBv\n" \
"bGFuZDEPMA0GA1UEBwwGQ3JhY293MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRz\n" \
"IFB0eSBMdGQxDDAKBgNVBAMMA2VzcDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n" \
"AQoCggEBAKnCVHTb9jaKojtDaLqQPH+L/3SVbF031RcsMzHCUV6PQqgiYfg7E8Lw\n" \
"axNckWxkosl0S5oCo9eebvUSUtFwFsQfBB5ihFNF9ftRbj7aRhtOCOHA2bGw7XFn\n" \
"N5Q6d7ig/QKllgyX2MBqfb8CjYQz9OFqPtbjft0XmpOk3q5ZOqFNcTMdHP/KJr+h\n" \
"4jXfyUYvtevv3MasHkC+M1ppjGGSBR/IrombnHBPePBRn9saCjS8TsxwLp8UxblB\n" \
"me/nojFiWQZR79jrYciabeUgdz1qK2E2riCrCASPLc1V+xzZY0tWDJ8lRGi2jAXS\n" \
"xlTuIV8mYZS7j818Cu/qbXMcckR4idkCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEA\n" \
"V3k8pF6QsjacdSdEZz/eztPhEU8GfkE6XaRPwYXgVLXy6WzPzaR6vSqH4PrsZXo6\n" \
"I5wmbx8IwdjdGS+skoAUvgObYfNjdVrmGzwwZR/MnKCQ/dn4lvixvXDcXpcu872D\n" \
"CCQc7woz32nz+F+QpRfoX268svdPdBs8i4p3eFLMr67C/nxpTvjiOqMryCrcfDdK\n" \
"6Lod2O3NyhHXifTR3rdQ5OtnQ+s9NQoj6CK1KjtkQHzf87ka4TRylzJQjdaNve6P\n" \
"u8o3TUBhh6AAwn0VMDcmGz/DOpyl+Tdgm3vmCtoUuY15+GYW5+mUkarkkL/nvnbd\n" \
"m/mGrU/+U8m5v8V3etMZ6w==\n" \
"-----END CERTIFICATE-----";

const char client_key[] = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEowIBAAKCAQEAqcJUdNv2NoqiO0NoupA8f4v/dJVsXTfVFywzMcJRXo9CqCJh\n" \
"+DsTwvBrE1yRbGSiyXRLmgKj155u9RJS0XAWxB8EHmKEU0X1+1FuPtpGG04I4cDZ\n" \
"sbDtcWc3lDp3uKD9AqWWDJfYwGp9vwKNhDP04Wo+1uN+3Reak6Terlk6oU1xMx0c\n" \
"/8omv6HiNd/JRi+16+/cxqweQL4zWmmMYZIFH8iuiZuccE948FGf2xoKNLxOzHAu\n" \
"nxTFuUGZ7+eiMWJZBlHv2OthyJpt5SB3PWorYTauIKsIBI8tzVX7HNljS1YMnyVE\n" \
"aLaMBdLGVO4hXyZhlLuPzXwK7+ptcxxyRHiJ2QIDAQABAoIBAEAhT7KN3iegm8Tz\n" \
"pD7GenRcuPab9EIK8487fWOQ2xhz2mSWCJi0CLUeCfVYhP+DzRN5wY2+7ZJpIdWn\n" \
"RG4mgBM6/F1WLlMfHNwCWTmZT97EzgoqxpKgGjM7IP34UH4JI0u6rkK3ooSeNj7Q\n" \
"ghGtZxc5AeBANQ7k+dpI8G7A+qswbpPK4YDV96xCyHJEgvNIObVICSFGyxFeHZi7\n" \
"bp65ROR/5yk8Zg84kU0F6YCUAhSaCTH4kt0du8KtXWrfN0/TQmbpkAXRQejB9tgQ\n" \
"IXrLkCt4ciLYuV28r2X+ilD4DEgj2cF9cOODKpua2P8jQUCZKd1HgNuFG6ImDvbw\n" \
"Phe/nPkCgYEA3BhaVnvRSFTmtaGrjmgzYoLvXn4LwQhsvNu/sprMgcG1x05YcMTo\n" \
"xtFB9bW1OWQORPhxTtUGw23uMHTSbJdNu97FDVLDrz80KLbWAFp3GDeTaJTYNHgZ\n" \
"bU/CcZbp25T8O+EK6jJ/8NTe/tBAUmnkMC6cCn7M9CvRPmRymy3K/s8CgYEAxXPW\n" \
"A+r8UTCISydv4TMyT8E/NkhL3IE+t3aRH7cY27cyfEMFojZYRru+DiEzZ2bmRRjy\n" \
"Gxpus0/3nJmJyBw3C0ZSo8lqGrPgvg5kVwzWN00JMAvN5vu4BSmS9HcqjyJx3haP\n" \
"ddnwEJr26qIlRhkdG83h1JBdVAdFUxJk4b0tVtcCgYEAzQ53ZCn5w10ukbytfIFC\n" \
"lp6M68mdo9v4i0Blar7kmGPku2sX5QDgKxQdUxJXZL/+t22NC7hXk86g406EGh4h\n" \
"k06q4o3guS62ptQpr6yZVtu4n/BFOyVF810jOPc0rJszIYq6w5YHM1bw6BEYTHBR\n" \
"5D4YrYKT603DplCpokBY52sCgYBMvXN9PFPqPiGKi/5ImTxBkQq/TyPkxHqalUNb\n" \
"/y5zHmVJtEa5P8PZYdhA55hx5KU0T7iw3foOSNMkkuvg0U53XFJ8A6jwTd7YHwHS\n" \
"b1PGVHNwr/XLgyOHJhxOVQ4+aRW2SjBhjl1mJsQ2duRjtRkCGGdf1Ofg456G5cw9\n" \
"i+fWKwKBgGt5ALCjQgnLzK9z9qx0k4gpaAWtzavpcPqT14Jh5by/Cp7FpsN2VpAq\n" \
"TeQXewg+KLFoKVlUax701u7UMrDIp2Tykqpe44SiDarz8cK7PTfEKa2nYFpbvvmq\n" \
"fCl5KDl4gIlDZN7zbXMhsh9SkRPbkJtkKAHFpV4AUJtce+1X8RDl\n" \
"-----END RSA PRIVATE KEY-----";

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_CONNECTED");
        IsMqttConnected = 1;
        // esp_mqtt_client_subscribe(client, "my_topic", 0);
        // esp_mqtt_client_publish(client, "my_topic", "Hi to all from ESP32 .........", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_DISCONNECTED");
        IsMqttConnected = 0;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_DATA");
        printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG_MQGTT, "Unknow MQQT event/");
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG_MQGTT, "Event dispatched from event loop base=");
    mqtt_event_handler_cb(event_data);
}

void initialize_mqtt(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://20.82.177.27",
        .broker.address.port = 8883,
        .broker.verification.certificate = ca_authority,
        .broker.verification.certificate_len = strlen(ca_authority)+1,
        .broker.verification.use_global_ca_store = false,
        .credentials.authentication.certificate = client_crt,
        .credentials.authentication.certificate_len = strlen(client_crt)+1,
        .credentials.authentication.key = client_key,
        .credentials.authentication.key_len = strlen(client_key)+1,
        .credentials.authentication.key_password = "espesp",
        .credentials.authentication.key_password_len = 6,
  };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    snprintf(Topic, sizeof(Topic), "watchdog/%s", deviceId);
}

void mqtt_publish(char topic[], char message[]) {
    esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
}

void mqtt_publish_temp(TemperatureReading* tempKeg, TemperatureReading* tempCol) {
    char buffer[100];
    sprintf(&buffer, "temperatures,device=%s temperatureKeg=%f,temperatureColumn=%f", deviceId, tempKeg->temperature, tempCol->temperature);
    mqtt_publish(Topic, buffer);
}