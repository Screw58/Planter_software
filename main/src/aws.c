/*!
******************************************************************
* \file: aws.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "aws.h"
#include "esp_wifi.h"
#include "secret_data.h"

//==================================[EXTERN VARIABLES]==================================//
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_Plant_sensor01_cert_pem_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_Plant_sensor01_cert_pem__end");
extern const uint8_t private_pem_key_start[] asm("_binary_Plant_sensor01_private_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_Plant_sensor01_private_key_end");


//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG_AWS    "aws_file"

#define SENSOR_NO  "1"
#define TOPIC      "home/" SENSOR_NO "/enable"
#define test_topic "iot/topic"

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static char endpoint_adress[] = AWS_ENDPOINT;
static char client_id[] = "Plant_sensor01";
static AWS_IoT_Client aws_client;

//==================================[GLOBAL VARIABLES]==================================//
bool enabled = false;
bool aws_connected = false;

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//

void disconnected_handler(AWS_IoT_Client *pClient, void *data)
{
   ESP_LOGW(TAG_AWS, "Reconnecting ...");
   aws_iot_mqtt_attempt_reconnect(pClient);
}


void ConnectAwsMqtt(void)
{
   memset((void *)&aws_client, 0, sizeof(aws_client));
   IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
   mqttInitParams.enableAutoReconnect = true;
   mqttInitParams.pHostURL = endpoint_adress;
   mqttInitParams.port = AWS_IOT_MQTT_PORT;
   mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
   mqttInitParams.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
   mqttInitParams.pDevicePrivateKeyLocation = (const char *)private_pem_key_start;
   mqttInitParams.disconnectHandler = disconnected_handler;
   aws_iot_mqtt_init(&aws_client, &mqttInitParams);

   IoT_Client_Connect_Params mqttConnectParams = iotClientConnectParamsDefault;
   mqttConnectParams.keepAliveIntervalInSec = 10;
   mqttConnectParams.pClientID = client_id;
   mqttConnectParams.clientIDLen = (uint16_t)strlen(client_id);
   while(aws_iot_mqtt_connect(&aws_client, &mqttConnectParams) != SUCCESS)
   {
      vTaskDelay(500 / portTICK_RATE_MS);
   }
   ESP_LOGI(TAG_AWS, "aws client connected");
   aws_connected = true;
}


void PublishReadingsToAws(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id)
{
   char buffer[150];
   IoT_Error_t result;
   IoT_Publish_Message_Params message;
   ESP_LOGI("debug_flag", "before aws-mqtt-yield");
   result = aws_iot_mqtt_yield(&aws_client, 500);
   if(result != SUCCESS && result != NETWORK_RECONNECTED)
   {
      ESP_LOGI("debug_flag", "in if condition ERROR!");
      result = aws_iot_mqtt_yield(&aws_client, 500);
      ESP_LOGI("debug_flag", "result of the second connection: %d", result);
      // return;
   }

   memset((void *)&message, 0, sizeof(message));
   // itoa(temp, buffer, 10);
   sprintf(buffer,
           "{ \"soilhumidity\" : %d , \"temperature\" : %.2f , \"humidity\" : %.2f , \"illuminance\" : %d , \"errors\" : %d, \"device_id\" "
           ": \"sensor_3\"}",
           AllSensorsReadings->soil_moisture,
           AllSensorsReadings->temperature,
           AllSensorsReadings->humidity,
           (uint16_t)AllSensorsReadings->illuminance,
           err_id);

   message.qos = QOS0;
   message.payload = (void *)buffer;
   message.payloadLen = strlen(buffer);
   // aws_iot_mqtt_publish(&aws_client, TEMP_TOPIC, strlen(TEMP_TOPIC), &message);
   ESP_LOGI("debug_flag", "before mqtt publish");
   aws_iot_mqtt_publish(&aws_client, test_topic, strlen(test_topic), &message);
}

void AwsDisconnect(void)
{
   while(aws_iot_mqtt_disconnect(&aws_client) != SUCCESS)
   {
      vTaskDelay(500 / portTICK_RATE_MS);
   }
}
