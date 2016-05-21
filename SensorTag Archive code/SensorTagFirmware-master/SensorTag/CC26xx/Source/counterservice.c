/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "hal_led.h"

#include "counterservice.h"
#include "st_util.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* Service configuration values */
#define COUNTER_SERVICE_UUID    COUNTER_SERV_UUID

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Service UUID
static CONST uint8 counterServiceUUID[MW_UUID_SIZE] =
{
  MW_UUID(COUNTER_SERVICE_UUID),
};

// Characteristic UUID: data
static CONST uint8 counterDataUUID[MW_UUID_SIZE] =
{
  MW_UUID(COUNTER_DATA_UUID),
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * Profile Attributes - variables
 */

// Profile Service attribute
static CONST gattAttrType_t counterService = { MW_UUID_SIZE, counterServiceUUID };

// Characteristic Value: data
static uint8 counterData[COUNTER_DATA_LEN] = { 0 };

// Characteristic Properties: data
static uint8 counterDataProps = GATT_PROP_READ | GATT_PROP_WRITE;


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t counterAttrTable[] =
{
  {
    // Service declaration
    { ATT_BT_UUID_SIZE, primaryServiceUUID },   /* type */
      GATT_PERMIT_READ,                         /* permissions */
      0,                                        /* handle */
      (uint8 *)&counterService                  /* pValue */
    },

    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &counterDataProps
    },

    // Characteristic Value "Data"
    {
      { MW_UUID_SIZE, counterDataUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        counterData
    }
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t counter_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method);
static bStatus_t counter_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Counter Service Callbacks
CONST gattServiceCBs_t counterCBs =
{
  counter_ReadAttrCB,    // Read callback function pointer
  counter_WriteAttrCB,   // Write callback function pointer
  NULL                   // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Counter_AddService
 *
 * @brief   Initializes the counter service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   
 - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Counter_AddService(uint32 services)
{
  uint8 status = SUCCESS;


  if ( services )
  {
       // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( counterAttrTable,
                                          GATT_NUM_ATTRS( counterAttrTable ),
										  GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &counterCBs );
  }

  return ( status );
}


/*********************************************************************
 * @fn      Counter_SetParameter
 *
 * @brief   Set a parameter. Allows *this application* to set a parameter. Parameters corresponds to the GATT characteristic values in the profile.
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
bStatus_t Counter_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case COUNTER_VALUE:
    if ( len == COUNTER_DATA_LEN )
    {
      VOID memcpy( counterData, value, COUNTER_DATA_LEN );
    }
    else
    {
      ret = bleInvalidRange;
    }
    break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      Counter_GetParameter
 *
 * @brief   Get a parameter. Allows *the application* to set or change parameters in the profile; 
 *          these parameters correspond to the GATT characteristic values in the profile.
 *
 * @param   param - parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Counter_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case COUNTER_VALUE:
      VOID memcpy( value, counterData, COUNTER_DATA_LEN );
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn          counter_ReadAttrCB
 *
 * @brief       Read an attribute. Every time a GATT client device wants to read 
 *              from an attribute in the profile, this function gets called.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 counter_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
        uint8_t *pValue, uint16_t *pLen,
        uint16_t offset, uint16_t maxLen,
        uint8_t method )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    *pLen = 0;
    return ATT_ERR_INVALID_HANDLE;
  }

  switch ( uuid )
  {
    // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
    // gattserverapp handles those reads
    case COUNTER_DATA_UUID:
      *pLen = COUNTER_DATA_LEN;
      // copy current counter value in counterData[0] to the buffer pointed to by pValue
      memcpy( pValue, &counterData[0], COUNTER_DATA_LEN );
      counterData[0]++;
      break;

    default:
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
    }

  return ( status );
}

/*********************************************************************
* @fn      counter_WriteAttrCB
*
* @brief   Validate attribute data prior to a write operation. Every time a GATT 
*          client device wants to write to an attribute in the profile, 
*          this function gets called.
*
* @param   connHandle - connection message was received on
* @param   pAttr - pointer to attribute
* @param   pValue - pointer to data to be written
* @param   len - length of data
* @param   offset - offset of the first octet to be written
*
* @return  Success or Failure
*/
static bStatus_t counter_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
        uint8_t *pValue, uint16_t len,
        uint16_t offset, uint8_t method )
{
  bStatus_t status = SUCCESS;
  uint16 uuid;

  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  switch ( uuid )
  {
    case COUNTER_DATA_UUID:
      // validate
      if ( offset != 0 ) 
      {
         status = ATT_ERR_INVALID_OFFSET;
      }
      if ( offset == 0 )
      {
        if ( len != 1 )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      // Write the value
      if ( status == SUCCESS )
      {
        // Attribute consists of type|permissions|value|handle
        
        // pointer to the attribute's value part
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        
        // set attribute value to new value supplied in write operation
        *pCurValue = pValue[0];

        // save new value in variable        
        counterData[0] = pValue[0];
       // HalLedSet(HAL_LED_1, HAL_LED_MODE_ON );
      }
      break;

    default:
      // Should never get here!
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
  }

  return ( status );
}


/*********************************************************************
*********************************************************************/
