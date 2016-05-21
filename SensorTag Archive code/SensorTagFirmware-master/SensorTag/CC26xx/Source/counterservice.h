#ifndef COUNTERSERVICE_H
#define COUNTERSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// MW Base 128-bit UUID: 3E09XXXX-293F-11E4-93BD-AFD0FE6D1DFD
#define MW_UUID_SIZE        ATT_UUID_SIZE
#define MW_UUID(uuid)       MW_BASE_UUID_128(uuid)
 
// Counter Service Parameters
#define COUNTER_VALUE                     0

// Service UUID
#define COUNTER_SERV_UUID              0x9914  // 3E099914-293F-11E4-93BD-AFD0FE6D1DFD
#define COUNTER_DATA_UUID              0x9915
  
// Counter length
#define COUNTER_DATA_LEN                       1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

 // MW Base 128-bit UUID: 3E09XXXX-293F-11E4-93BD-AFD0FE6D1DFD
#define MW_BASE_UUID_128( uuid )  0xFD, 0x1D, 0x6D, 0xFE, 0xD0, 0xAF, 0xBD, 0x93, \
                                  0xE4, 0x11, 0x3F, 0x29, LO_UINT16( uuid ), HI_UINT16( uuid ), 0x09, 0x3E
 
/*********************************************************************
 * Profile Callbacks
 */


/*********************************************************************
 * API FUNCTIONS
 */

/*
 * Counter_AddService- Initializes the Counter service by registering
 *          GATT attributes with the GATT server.
 *
 */

extern bStatus_t Counter_AddService(uint32 services );

/*********************************************************************
 * @fn      Counter_SetParameter
 *
 * @brief   Set a Counter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Counter_SetParameter( uint8 param, uint8 len, void *value );

/*
 * Counter_GetParameter - Get a Counter parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Counter_GetParameter( uint8 param, void *value );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* COUNTERSERVICE_H */
