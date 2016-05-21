//My Personal  Service
//SensorTag Firmware Magic
//This took me heaps of time to make and test and run. It's been emotional.
//But Iron Maiden and Megadeth saw me pull through.
//Worked : 8 & 9 October , 2015

//*****************************************************
/*Includes*/

#include "bcomdef.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h" // enables gatt function calls
#include "gatt_uuid.h" // UUID Mgmt
#include "gattservapp.h" //super important. Registers your service
#include "gapbondmgr.h"
#include "string.h"
#include "yourservice.h" //this is the header for our service
#include "st_util.h"

//*****************************************************



/*********************************************************************
 * CONSTANTS
 *
 * some templates for you.

 * /* Service configuration values
#define SENSOR_SERVICE_UUID     YOURSERVICE_SERV_UUID
#define SENSOR_DATA_UUID        YOURSERVICE_DATA_UUID
#define SENSOR_CONFIG_UUID      YOURSERVICER_CONF_UUID
#define SENSOR_PERIOD_UUID      YOURSERVICER_PERI_UUID

#define SENSOR_SERVICE          YOURSERVICE_SERVICE
#define SENSOR_DATA_LEN         YOURSERVICE_DATA_LEN

#define SENSOR_DATA_DESCR       "urServ. Data"
#define SENSOR_CONFIG_DESCR     "urServ. Conf."
#define SENSOR_PERIOD_DESCR     "urServ. Period"
 *
 *
 *
 */
#define yourService_DATA_LEN                       1   //default value

#define SENSOR_SERVICE_UUID   MW_UUID(uuid) //this is the service UUID.

//*******************************************************************


//*GLOBAL VARIABLES**********************************************************

/*
 * Smome templates you may use.
 * // Service UUID
static CONST uint8_t sensorServiceUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_SERVICE_UUID),
};

// Characteristic UUID: data
static CONST uint8_t sensorDataUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_DATA_UUID),
};

// Characteristic UUID: config
static CONST uint8_t sensorCfgUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_CONFIG_UUID),
};

// Characteristic UUID: period
static CONST uint8_t sensorPeriodUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_PERIOD_UUID),
};
 *
 *
 */

static uint8 yourServiceData[yourService_DATA_LEN] = { 0 }; //this is whats holding our data

static CONST uint8 yourServiceDataUUID[TI_UUID_SIZE] =
{ //yourServiceDataUUID is the UUID of the yourServiceData variable.
		//see , you gotta define UUIDs for all aspects of your characteristics.
		//why you ask? Cuz your android app is gonna read off the UUID. Thats why.
  MW_UUID(yourService_DATA_UUID),
};
//I played around with a lot of sizes and stuff kept breaking time and time again.
//that's why I highly suggest you stick to TI_UUID_SIZE

//****************************************************************************


/*********************************************************************
 * Profile Attributes - variables
 *
 * some templates you can use.
 *
 * // Profile Service attribute
static CONST gattAttrType_t sensorService = { TI_UUID_SIZE, sensorServiceUUID };

// Characteristic Value: data
static uint8_t sensorData[SENSOR_DATA_LEN] = { 0, 0, 0, 0};

// Characteristic Properties: data
static uint8_t sensorDataProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic Configuration: data
static gattCharCfg_t *sensorDataConfig;

#ifdef USER_DESCRIPTION
// Characteristic User Description: data
static uint8_t sensorDataUserDescr[] = SENSOR_DATA_DESCR;
#endif

// Characteristic Properties: configuration
static uint8_t sensorCfgProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: configuration
static uint8_t sensorCfg = 0;

#ifdef USER_DESCRIPTION
// Characteristic User Description: configuration
static uint8_t sensorCfgUserDescr[] = SENSOR_CONFIG_DESCR;
#endif

// Characteristic Properties: period
static uint8_t sensorPeriodProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: period
static uint8_t sensorPeriod = SENSOR_MIN_UPDATE_PERIOD / SENSOR_PERIOD_RESOLUTION;

#ifdef USER_DESCRIPTION
// Characteristic User Description: period
static uint8_t sensorPeriodUserDescr[] = SENSOR_PERIOD_DESCR;
#endif
 *
 *
 *
 */


static uint8_t yourServiceDataProps = GATT_PROP_READ | GATT_PROP_WRITE;
//so basically, this defines the properties for characteristic that you've defined.
//which means, you can either read or write to it the data element.

static CONST gattAttrType_t yourServiceService = { TI_UUID_SIZE };
//this is something that really confused me. Apparently, this 'defines' the service and it's length
//and it's used later on in a callback function.

//**************************************************************************




/*********************************************************************
 * Profile Attributes - Table
 * Welcome to the cheesy part of the code. This is where you define your table and use
 * most of the things that you've declared above. Code is pretty self explanatory.
 */

static gattAttribute_t yourServiceAttrTable[] =
{
  {
    // Service declaration
    { ATT_BT_UUID_SIZE, primaryServiceUUID },   /* type */
      GATT_PERMIT_READ,                         /* permissions */
      0,                                        /* handle */
      (uint8 *)&yourServiceService                  /* pValue */
    },

    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID }, //UUID's defined
        GATT_PERMIT_READ, // READ property allotted
        0,
        &yourServiceDataProps //allottment from the variable defined above
    },

    // Characteristic Value "Data"
    {
      { TI_UUID_SIZE, yourServiceDataUUID },
        GATT_PERMIT_READ, // | GATT_PERMIT_WRITE, // This was one tricky cookie
        0,
        yourServiceData //this is where the magic data value sits.
    }
};

//************************************************************************************

/*********************************************************************
 * LOCAL FUNCTIONS
 *
 */
static bStatus_t sensor_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method);
static bStatus_t sensor_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method);
//************************************************************************************


/*********************************************************************
 * PROFILE CALLBACKS
 */

static CONST gattServiceCBs_t sensorCBs =
{
  sensor_ReadAttrCB,  // Read callback function pointer
  sensor_WriteAttrCB, // Write callback function pointer
  NULL                // Authorization callback function pointer
};

//***********************************************************************

//ADD Service function which is called in your main app. Make sure that the
//argument list matches. Else, things will break. Trust me. I spent 3 hours fixing this alone.
bStatus_t yourService_AddService(void)
 {

   // Register GATT attribute list and CBs with GATT Server App
	return GATTServApp_RegisterService(yourServiceAttrTable,
	                                     GATT_NUM_ATTRS (yourServiceAttrTable),
	                                     GATT_MAX_ENCRYPT_KEY_SIZE,
	                                     &sensorCBs);
  }

//************************************************************************



/*
 * GAME TIME :
 * So this is where you implement your functionality. I cannot emphasize how ridiculously hard
 * it was to troubleshoot and debug all of this. Things kept breaking like.. a lot. So, use it
 * wisely. Don't mess around. Like seriously. DONT MESS AROUND. Change what you want and hope to god
 * it does not break. Also, there are no online resources or help websites to find what error it is or why
 * contact me if you got doubts  : shreyas.enug@gmail.com
 *
 */
static bStatus_t sensor_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method)
{
  uint16_t uuid;
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if (gattPermitAuthorRead(pAttr->permissions))
  {
    // Insufficient authorization
    return (ATT_ERR_INSUFFICIENT_AUTHOR);
  }

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    *pLen = 0;
    return ATT_ERR_INVALID_HANDLE;
  }

  switch ( uuid )
        {
          // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
          // gattserverapp handles those reads
  	  	  //ONLY MESS AROUND WITH THIS SECTION. DONT, I REPEAT, DONT CHANGE ANYTHING ELSE.
          case yourService_DATA_UUID:
            *pLen = yourService_DATA_LEN;
            // copy current yourService value in yourServiceData[0] to the buffer pointed to by pValue
            memcpy( pValue, &yourServiceData[0], yourService_DATA_LEN );
            //dont use osal.memcpy .. things break. just use what I've given you.

            // increment the yourService
            //you can pretty much do anything here. Carefully.
            yourServiceData[0]++;
            break;

          default:
            *pLen = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
          }

  return (status);
}


//Boilerplate Write function. Ain't got no functionality yet. Will mess around with it later.
static bStatus_t sensor_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint16_t uuid;

  // If attribute permissions require authorization to write, return error
  if (gattPermitAuthorWrite(pAttr->permissions))
  {
    // Insufficient authorization
    return (ATT_ERR_INSUFFICIENT_AUTHOR);
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }
  return (status);
}


//There isn't a whole lot more to it. Just play around with items and hope that things dont break.

/*post game time template functions for you to mess around with.
 *
 *
 * *********************************************************************
 * @fn      sensor_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 //***************************************************************
static bStatus_t sensor_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8_t notifyApp = 0xFF; // just some notification thing
  uint16_t uuid;

  // If attribute permissions require authorization to write, return error
  if (gattPermitAuthorWrite(pAttr->permissions))
  {
    // Insufficient authorization
    return (ATT_ERR_INSUFFICIENT_AUTHOR);
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  switch (uuid)
  {
    case SENSOR_DATA_UUID:
      // Should not get here
      break;

    case SENSOR_CONFIG_UUID:
      // Validate the value
      // Make sure it's not a blob oper
      if (offset == 0)
      {
        if (len != 1)
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }



      // Write the value
      if (status == SUCCESS)
      {
        uint8_t *pCurValue = (uint8_t *)pAttr->pValue;

        *pCurValue = pValue[0];

        if (pAttr->pValue == &sensorCfg)
        {
          notifyApp = SENSOR_CONF;
        }
      }
      break;

    case SENSOR_PERIOD_UUID:
      // Validate the value
      // Make sure it's not a blob oper
      if (offset == 0)
      {
        if (len != 1)
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      //This is what we're interested in
     //.....................................................
      // Write the value
      if (status == SUCCESS)
      {
        if (pValue[0]>=(SENSOR_MIN_UPDATE_PERIOD/SENSOR_PERIOD_RESOLUTION))
        {

          uint8_t *pCurValue = (uint8_t *)pAttr->pValue;
          *pCurValue = pValue[0];

          if (pAttr->pValue == &sensorPeriod)
          {
            notifyApp = SENSOR_PERI;
          }
        }
        else
        {
           status = ATT_ERR_INVALID_VALUE;
        }
      }
      break;
    //........................................................


    case GATT_CLIENT_CHAR_CFG_UUID:
      status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                              offset, GATT_CLIENT_CFG_NOTIFY);
      break;

    default:
      // Should never get here!
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
  }

  // If a charactersitic value changed then callback function to notify application of change
  if ((notifyApp != 0xFF ) && sensor_AppCBs && sensor_AppCBs->pfnSensorChange)
  {
    sensor_AppCBs->pfnSensorChange(notifyApp);
  }

  return (status);
}
 *
 *
 */





