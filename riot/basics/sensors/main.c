#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

/* Add lps331ap related include here */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Add lsm303dlhc related include here */
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

/*light value sensor*/
#include "isl29020.h"
#include "isl29020_params.h"

/*gyroscope values measured*/
#include "l3g4200d.h"
#include "l3g4200d_params.h"

/* light value variable */

/* gyroscope values variables */


/* Declare the lps331ap device variable here */
static lpsxxx_t lpsxxx;

/* Declare the lsm303dlhc device variable here */
static lsm303dlhc_t lsm303dlhc;

/* Declare the isl29020 light device variable here */
static isl29020_t isl29020;

/* Declare the l3g4200d gyroscope device variable here */
static l3g4200d_t l3g4200d;
static l3g4200d_data_t l3g4200d_data;


static char pres_stack[THREAD_STACKSIZE_MAIN];

static void *thread_handler_pres(void *arg)
{
    (void)arg;

    /* Add the lsm303dlhc sensor polling endless loop here */
    while (1) {
        lsm303dlhc_3d_data_t mag_value;
        lsm303dlhc_3d_data_t acc_value;
        lsm303dlhc_read_acc(&lsm303dlhc, &acc_value);
        printf("Accelerometer x: %i y: %i z: %i\n",
               acc_value.x_axis, acc_value.y_axis, acc_value.z_axis);
        lsm303dlhc_read_mag(&lsm303dlhc, &mag_value);
        printf("Magnetometer x: %i y: %i z: %i\n",
               mag_value.x_axis, mag_value.y_axis, mag_value.z_axis);
//        xtimer_usleep(500 * US_PER_MS);
        xtimer_sleep(5);
    }
    return 0;
}

/****************** thread gyroscope ****************/
static char gyr_thread_stack[THREAD_STACKSIZE_MAIN];

static void *gyr_thread(void *arg)
{
    (void)arg;
//    l3g4200d_t l3g4200d;
//    l3g4200d_data_t l3g4200d_data;

    
    while (1) {
        l3g4200d_read(&l3g4200d, &l3g4200d_data);

        printf("Gyro data [dps] - X: %6i   Y: %6i   Z: %6i\n",
               l3g4200d_data.acc_x, l3g4200d_data.acc_y, l3g4200d_data.acc_z);

//        xtimer_usleep(1500 * US_PER_MS);
        xtimer_sleep(5);
    }
    return 0;    
}

/************** thread Light ****************/
static char light_thread_stack[THREAD_STACKSIZE_MAIN];

static void *light_thread(void *arg)
{
    (void)arg;
    
    while (1) {
        
        printf("Light value: %5i LUX\n", isl29020_read(&isl29020));
        xtimer_sleep(1);
    }
    return 0;
}


int main(void)
{
    /* Initialize the lps331ap sensor here */
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]); 
    
    /* Initialize the lsm303dlhc sensor here */
    lsm303dlhc_init(&lsm303dlhc, lsm303dlhc_params);
    
    /* initialise light sensor */
    if (isl29020_init(&isl29020, &isl29020_params[0]) == 0) {
        puts("[light OK]\n");
    }
    else {
        puts("[light Failed]");
        return 1;
    }

    /* initialise gyrocope sensor */
    if (l3g4200d_init(&l3g4200d, &l3g4200d_params[0]) == 0) {
        puts("[gyr OK]\n");
    }
    else {
        puts("[gyr Failed]");
        return 1;
    }

    thread_create(pres_stack, sizeof(pres_stack), THREAD_PRIORITY_MAIN - 1,
               0, thread_handler_pres, NULL, "lsm303dlhc");

    thread_create(light_thread_stack, sizeof(light_thread_stack), THREAD_PRIORITY_MAIN - 2,
               0, light_thread, NULL, "isl29020");
    thread_create(gyr_thread_stack, sizeof(gyr_thread_stack), THREAD_PRIORITY_MAIN - 3,
               0, gyr_thread, NULL, "l3g4200d");
   
    /* Add the lps331ap sensor polling endless loop here */
    while (1) {
        uint16_t pres = 0;
        int16_t temp = 0;
        lpsxxx_read_temp(&lpsxxx, &temp);
        lpsxxx_read_pres(&lpsxxx, &pres);
        printf("Pressure: %uhPa, Temperature: %u.%u°C\n",
               pres, (temp / 100), (temp % 100));
        xtimer_sleep(10);
    }
    

    
    return 0;
}
