/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "hal_led.h"

#include "securityservice.h"
#include "st_util.h"
#include "math.h"
#include "stdlib.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* Service configuration values */
#define SECURITY_SERVICE_UUID    SECURITY_SERV_UUID

/*********************************************************************
 * TYPEDEFS

 *
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Service UUID
static CONST uint8 securityServiceUUID[MW_UUID_SIZE] =
{
  MW_UUID(SECURITY_SERVICE_UUID),
};


// Data UUID: data
CONST uint8 securityDataUUID[MW_UUID_SIZE] =
{
  MW_UUID(SECURITY_DATA_UUID),
};

 CONST uint8 PublicKeyDataUUID[MW_UUID_SIZE] =
{
  MW_UUID(PUBLIC_DATA_UUID),
};

 CONST uint8 rcvdSecretDataUUID[MW_UUID_SIZE] =
{
  MW_UUID(RCVD_DATA_UUID),
};

 CONST uint8 FlagDataUUID[MW_UUID_SIZE] =
{
  MW_UUID(FLAG_DATA_UUID),
};

 // Profile Service attribute
 static CONST gattAttrType_t securityService = { MW_UUID_SIZE, securityServiceUUID };

//Charactersitc UUID

// uint8 Xa = 6;
//        //uint8 p = 17;
//        //uint8 g = 5;

 uint8 m[] = {20, 40, 60, 80};
 //int count = 0;
 //int seed[SECURITY_DATA_LEN] = {5};
 //int randnum[] = {rand()};
 int seed;

 //Public Key Characteristic
 static uint8 pubKeyProps = GATT_PROP_READ;
 static uint8 pubKeyVal[SECURITY_DATA_LEN+1] = { 0 };

 //Public Key Characteristic
 static uint8 rcvdSecProps = GATT_PROP_READ | GATT_PROP_WRITE ;
 static uint8 rcvdSecVal[SECURITY_DATA_LEN+1] = { 0 };

 //Public Key Characteristic
 static uint8 FlagProps = GATT_PROP_READ;
 static uint8 FlagVal[SECURITY_DATA_LEN+1] = { 0 };

//Security Data Characterstic
 static uint8 securityDataProps = GATT_PROP_READ;
 static securityData[SECURITY_DATA_LEN] = { 0 };

 //static int secret = 8; //sample



/*********************************************************************
 * Profile Attributes - variables


// Characteristic Value: data





/*********************************************************************
*/

static gattAttribute_t securityAttrTable[] =
{
  {
    // Service declaration

    { ATT_BT_UUID_SIZE, primaryServiceUUID },   /* type */
      GATT_PERMIT_READ,                         /* permissions */
      0,                                        /* handle */
      (uint8 *)&securityService                  /* pValue */
    },
//----------------------------------------------
    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &securityDataProps
    },

    // Characteristic Value "Data"
    {
      { MW_UUID_SIZE, securityDataUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        securityData
    },
//-----------------------------------------------

	//Public Key Characteristic Declaration
	{
		{ATT_BT_UUID_SIZE,characterUUID},
		GATT_PERMIT_READ,
		1,
		&pubKeyProps
	},
	
	//Public Key Value (DATA)
	{
		{MW_UUID_SIZE,PublicKeyDataUUID},
		GATT_PERMIT_READ,
		1,
		pubKeyVal

	},
//-------------------------------------------------	
	
	//-----------------------------------------------
	
	//Received Secret Characteristic Declaration
	{
		{ATT_BT_UUID_SIZE,characterUUID},
		GATT_PERMIT_READ,
		1,
		&rcvdSecProps
	},
	
	//Received Secret Value (DATA)
	{
		{MW_UUID_SIZE,rcvdSecretDataUUID},
		 GATT_PERMIT_READ | GATT_PERMIT_WRITE,
		1,
		rcvdSecVal
	},
//-------------------------------------------------	

//-----------------------------------------------
	
	//Flag Characteristic Declaration
	{
		{ATT_BT_UUID_SIZE,characterUUID},
		GATT_PERMIT_READ,
		1,
		&FlagProps
	},
	
	//Public Key Value (DATA)
	{
		{MW_UUID_SIZE,FlagDataUUID},
		GATT_PERMIT_READ,
		1,
		FlagVal
	},
//-------------------------------------------------	
	
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t security_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method);
static bStatus_t security_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// security Service Callbacks
CONST gattServiceCBs_t securityCBs =
{
  security_ReadAttrCB,    // Read callback function pointer
  security_WriteAttrCB,   // Write callback function pointer
  NULL                   // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      security_AddService
 *
 * @brief   Initializes the security service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   
 - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t security_AddService(uint32 services)
{
  uint8 status = SUCCESS;


  if ( services )
  {
       // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( securityAttrTable,
                                          GATT_NUM_ATTRS( securityAttrTable ),
										  GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &securityCBs );
  }

  return ( status );
}


/*********************************************************************
 * @fn      security_SetParameter
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

bStatus_t security_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case security_VALUE:
    if ( len == security_DATA_LEN )
    {
      VOID memcpy( securityData, value, security_DATA_LEN );
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
 * @fn      security_GetParameter
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

bStatus_t security_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case security_VALUE:
      VOID memcpy( value, securityData, security_DATA_LEN );
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}
*/

/*********************************************************************
 * @fn          security_ReadAttrCB
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

 * @return      Success or Failure
 */
static uint8 security_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
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
    case SECURITY_DATA_UUID:
      *pLen = SECURITY_DATA_LEN;
      // copy current security value in securityData[0] to the buffer pointed to by pValue
      memcpy( pValue, &securityData[0], SECURITY_DATA_LEN );
      //securityData[0]++;
      securityData[0] = rand()%99;
      break;
	  
	  case PUBLIC_DATA_UUID:
      *pLen = SECURITY_DATA_LEN;
      // copy current security value in publicKey[0] to the buffer pointed to by pValue
      memcpy( pValue, &pubKeyVal[0], SECURITY_DATA_LEN );


      //uint8 Ya = (uint8)(((uint32)pow(5,Xa))%55);

      //count=count+1;
      //pubKeyVal[0]= (uint8)(((uint32)pow(key,3))%55) ;
      //key = seed[0]+2;

      //pubKeyVal[0]=  (uint8)(((uint32)pow(m[0],3))%55) ;
      //randnum[0] = rand()%99;
      //pubKeyVal[0]=  (uint8)(((uint32)pow(20,3))%55) ;
      //securityData[0] = rand()%99;
      //seed = securityData[0];
      pubKeyVal[0]=  (uint8)(((uint32)pow(securityData[0],3))%55) ;




      break;

	  case RCVD_DATA_UUID:
	       *pLen = SECURITY_DATA_LEN;
	       // copy current security value in publicKey[0] to the buffer pointed to by pValue
	       memcpy( pValue, &rcvdSecVal[0], SECURITY_DATA_LEN );
	       break;

	  
	  case FLAG_DATA_UUID:
      *pLen = SECURITY_DATA_LEN;
      // copy current security value in FlagRx[0] to the buffer pointed to by pValue
      memcpy( pValue, &FlagVal[0], SECURITY_DATA_LEN );
     // uint8 Kb = (uint8)(((uint32)pow(securityData[0],Xa))%17);
     // uint8 sec = (uint8)(((uint32)(pow(((uint32)pow(5,Xa)),Xa)))%17); //this is the secret
//      if((rcvdSecVal[0]==m[0])||(rcvdSecVal[0]==m[1])||(rcvdSecVal[0]==m[2])||(rcvdSecVal[0]==m[3]))
//      {
//      FlagVal[0]=1;
//      }

      if(rcvdSecVal[0]==(uint8)(((uint32)pow(pubKeyVal[0],7))%55)&&(rcvdSecVal[0]!=0))
      {
    	  //FlagVal[0]=1;
    	  FlagVal[0] = (uint8)(((uint32)pow(37,3))%55);
      }
      else
      //FlagVal[0] = securityData[0];
    	  //FlagVal[0]=pubKeyVal[0];
    	  FlagVal[0] = 0;


      //      FlagVal[0] = randnum[0];

      //      if(rcvdSecVal[0]==randnum)
      //      {
      //    	  FlagVal[0] = 1;
      //      }

      //      int kb = rcvdSecVal[0]-(2*count);
      //      if(kb==seed)
      //      {
      //    	  FlagVal[0]=1;
      //      }
      break;

    default:
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
    }

  return ( status );
}

/*********************************************************************
* @fn      security_WriteAttrCB
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
static bStatus_t security_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
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
    case SECURITY_DATA_UUID:
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
        securityData[0] = pValue[0];
       // HalLedSet(HAL_LED_1, HAL_LED_MODE_ON );
      }
      break;
	  
	  case RCVD_DATA_UUID:
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
        rcvdSecVal[0] = (pValue[0]); //not right now //layer 2 security. It can be any number.
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
