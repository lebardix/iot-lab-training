/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Wakaama LwM2M Client CLI support
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "thread.h"
#include "xtimer.h"
#include "kernel_defines.h"

#include "lwm2m_client.h"
#include "lwm2m_client_objects.h"
#include "lwm2m_platform.h"

/* Include lpsxxx headers */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Include temperature object headers */
#include "temperature_object.h"

/* Declare lpsxxx_t sensor variable (globally) */
static lpsxxx_t sensor;

/* Define number of LwM2M objects */
#define OBJ_COUNT (4)

/* Declare LwM2M temperature object instance */
lwm2m_temp_instance_t *temp_instance;

/* Define temperature sensor read interval (seconds) */
#define TEMP_READ_INTERVAL (5)

/* Define temperature sensor reader thread priority.
 * main = 7 < LwM2M client = 6 < radio at86rf2xx = 2 
 */
#define TEMP_PRIO  (THREAD_PRIORITY_MAIN - 6)

/* Declare thread stack size. */
char thread_stack[THREAD_STACKSIZE_MAIN];


uint8_t connected = 0;
lwm2m_object_t *obj_list[OBJ_COUNT];
static lwm2m_client_data_t client_data;


/* 
 * Read temperature sensor method.
 * Update LwM2M temperature object instance and notify observers.
 */
static void *_temp_read(void *arg)
{
    (void)arg;
    int16_t temp;
    lwm2m_uri_t uri;
    uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID |
               LWM2M_URI_FLAG_RESOURCE_ID;
    uri.objectId = LWM2M_TEMP_OBJECT_ID;
    uri.instanceId = temp_instance->shortID;
    uri.resourceId = LWM2M_TEMP_RES_SENSOR_VALUE;
    while (1) {
        lpsxxx_read_temp(&sensor, &temp);
        temp_instance->sensor_value = temp / 100.0;
        /* mark changed for observers */
        lwm2m_resource_value_changed(client_data.lwm2m_ctx, &uri);
        xtimer_sleep(TEMP_READ_INTERVAL);
    }
    return 0;
}

void lwm2m_cli_init(void)
{
    /* this call is needed before creating any objects */
    lwm2m_client_init(&client_data);

    /* add objects that will be registered */
    obj_list[0] = lwm2m_client_get_security_object(&client_data);
    obj_list[1] = lwm2m_client_get_server_object(&client_data);
    obj_list[2] = lwm2m_client_get_device_object(&client_data);
    obj_list[3] = lwm2m_client_get_temperature_object();
    /* add temperature object */


    if (!obj_list[0] || !obj_list[1] || !obj_list[2]) {
        puts("Could not create mandatory objects");
    }
}

int lwm2m_cli_cmd(int argc, char **argv)
{
    if (argc == 1) {
        goto help_error;
    }

    if (!strcmp(argv[1], "start")) {
        /* run the LwM2M client */
        if (!connected && lwm2m_client_run(&client_data, obj_list, OBJ_COUNT)) {
            connected = 1;

            /* Initialize and enable the lps331ap device */
            if (lpsxxx_init(&sensor, &lpsxxx_params[0]) != LPSXXX_OK) {
                puts("LPS331AP initialization failed");
                return 1;
            }
            lpsxxx_enable(&sensor);

            /* Get temperature object instance */
            temp_instance = (lwm2m_temp_instance_t *)
                 lwm2m_list_find(obj_list[3]->instanceList, 0);

            /* Create temperature sensor reader thread */
            thread_create(thread_stack, sizeof(thread_stack),
                TEMP_PRIO, 0,
                _temp_read, NULL, "temp_reader");

        }
        return 0;
    }

    if (IS_ACTIVE(DEVELHELP) && !strcmp(argv[1],"mem")) {
        lwm2m_tlsf_status();
        return 0;
    }

help_error:
    if (IS_ACTIVE(DEVELHELP)) {
        printf("usage: %s <start|mem>\n", argv[0]);
    }
    else {
        printf("usage: %s <start>\n", argv[0]);
    }

    return 1;
}
