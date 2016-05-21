#define YOURSERVICE_DATA_UUID              0x9915

//you can pretty much define boat loads of things that you want.

/*
 * For example.
 * / Service UUID (base F000****-0451-4000-B000-00000000-0000)
#define YOURSERVICE_SERV_UUID             0xAA40
#define YOURSERVICE_DATA_UUID             0xAA41
#define YOURSERVICE_CONF_UUID             0xAA42
#define YOURSERVICE_CAL_UUID              0xAA43 // Not used on SensorTag2
#define YOURSERVICE_PERI_UUID             0xAA44

// Length of sensor data in bytes
#define YOURSERVICE_DATA_LEN
 *
 */

#define MW_UUID(uuid)                  MW_BASE_UUID_128(uuid)

// MW Base 128-bit UUID: 3E09XXXX-293F-11E4-93BD-AFD0FE6D1DFD
#define MW_BASE_UUID_128( uuid )  0xFD, 0x1D, 0x6D, 0xFE, 0xD0, 0xAF, 0xBD, 0x93, \
                                  0xE4, 0x11, 0x3F, 0x29, LO_UINT16( uuid ), HI_UINT16( uuid ), 0x09, 0x3E

//it could very well be  : extern bStatus_t YOURSERVICE_AddService(void);
extern bStatus_t YOURSERVICE_AddService(void);
                                  
