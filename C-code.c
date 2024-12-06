int StartDagVerbruik;
int StartNachtVerbruik;
int StartDagOpbrengst;
int StartNachtOpbrengst;
char StartTijd;
int StartGas;
int dag;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "MQTTClient.h"


#define ADDRESS           "tcp://192.168.0.108:1883"
#define QOS               0
#define CLIENTID          "Flandrien"
#define SUB_TOPIC         "P1/MD8"
#define PUB_TOPIC         "VIZO/ERROR_SEND"
#define TOPIC_LEN         1200
#define TIMEOUT           100L
#define ERR_OUT_LEN       2000


  


volatile MQTTClient_deliveryToken deliveredtoken;
volatile int message_count = 0;
volatile long double sum_data = 0;
volatile long double max_data = -__LDBL_MAX__;
volatile long double min_data = __LDBL_MAX__;

void log_to_file(const char *message);
void get_current_time_str(char* buffer, size_t buffer_size);

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    printf("-----------------------------------------------\n");
    deliveredtoken = dt;
}



int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {


    char *payload = message->payload;
    char *token_str;
    char time_buffer[20];
    long double totaal_gasverbruik;

    token_str = strtok(payload, ";");
    char *datum_tijd_stroom = token_str;
    token_str = strtok(NULL, ";");
    char *tarief_indicator = token_str;
    token_str = strtok(NULL, ";");
    char *actueel_stroomverbruik = token_str;
    token_str = strtok(NULL, ";");
    char *actueel_spanning = token_str;
    token_str = strtok(NULL, ";");
    char *totaal_dagverbruik = token_str;
    token_str = strtok(NULL, ";");
    char *totaal_nachtverbruik = token_str;
    token_str = strtok(NULL, ";");
    char *totaal_dagopbrengst = token_str;
    token_str = strtok(NULL, ";");
    char *totaal_nachtopbrengst = token_str;
    token_str = strtok(NULL, ";");
    char *datum_tijd_gas = token_str;
    token_str = strtok(NULL, ";");
    totaal_gasverbruik = strtold(token_str, NULL);
    


    // Get the current time and format it
    get_current_time_str(time_buffer, sizeof(time_buffer));
    //Neerschrijven in file
    char loging[ERR_OUT_LEN];
    sprintf(loging, "Ontvangen om %s - Totaal dagverbruik: %s, Totaal nachtverbruik: %s, Totale dagopbrengst: %s, Totale nachtopbrengst: %s, Totaal gasverbruik: %Lf\n", datum_tijd_stroom, totaal_dagverbruik, totaal_nachtverbruik, totaal_dagopbrengst, totaal_nachtopbrengst, totaal_gasverbruik);

    log_to_file(loging);

    message_count++;
    
    if (message_count == 1)
    {
        
        StartTijd = atoi(datum_tijd_gas);
        StartDagVerbruik = atoi(totaal_dagverbruik);
        StartDagOpbrengst = atoi(totaal_dagopbrengst);
        StartNachtVerbruik = atoi(totaal_nachtverbruik);
        StartNachtOpbrengst = atoi(totaal_nachtopbrengst);
        //StartGas = atoi(totaal_gasverbruik);
        return(StartTijd, StartDagVerbruik);
        dag = atoi(datum_tijd_gas);
    }

     int stop = atoi(tarief_indicator);
     // printf("stop: %d\n",stop);

       if (dag == dag1
    {
        
        StartTijd = atoi(datum_tijd_gas);
        StartDagVerbruik = atoi(totaal_dagverbruik);
        StartDagOpbrengst = atoi(totaal_dagopbrengst);
        StartNachtVerbruik = atoi(totaal_nachtverbruik);
        StartNachtOpbrengst = atoi(totaal_nachtopbrengst);
        //StartGas = atoi(totaal_gasverbruik);
        return(StartTijd, StartDagVerbruik);
        dag = atoi(datum_tijd_gas);
    }
    
     

    


    if (stop == 0) {
        printf("=====================================================\n");
        printf("+ Electriciteit- en gas verbruik - totalen per dag  +\n");
        printf("=====================================================\n");
        printf("Startwaarden:\n\n");
        printf("Datum - Tijd: %d\n", StartTijd);
        printf("Dag     Totaal verbruik     = %d kWh\n", StartDagVerbruik);
        printf("Dag     Totaal opbrengst    = %d kWh\n", StartDagOpbrengst);
        printf("Nacht   Totaal verbruik     = %d kWh\n", StartNachtVerbruik);
        printf("Nacht   Totaal opbrengst    = %d kWh\n", StartNachtOpbrengst);
        //printf("Gas     Totaal verbruik     = %d m3",StartGas)
        printf("-------------------------------------------------------------\n");
        printf("Totalen\n");
        printf("-------------------------------------------------------------\n");
        //printf("%d\n",daglog);
        printf("=====================================================\n");
        printf("+++++++++++++++Einde van dit rappot++++++++++++++++++\n");
        printf("=====================================================\n");
        fflush(stdout);
        return(-1);
    }

    //MQTTClient_freeMessage(&message);
    //MQTTClient_free(topicName);

    return 1;
}

void log_to_file(const char *message) {
    FILE *file = fopen("logs.txt", "a");
    if (file == NULL) {
        perror("Error opening log file");
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

void get_current_time_str(char* buffer, size_t buffer_size) {
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_info);
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main() {
    // Open MQTT client for listening
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Define the correct call back functions when messages arrive
    MQTTClient_setCallbacks(client, client, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    printf("Subscribing to topic %s for client %s using QoS%d\n\n", SUB_TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, SUB_TOPIC, QOS);

    // Keep the program running to continue receiving and publishing messages
    for(;;) {
        ;
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
