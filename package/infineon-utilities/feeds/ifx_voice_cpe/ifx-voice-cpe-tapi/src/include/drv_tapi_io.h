#ifndef DRV_TAPI_IO_H
#define DRV_TAPI_IO_H
/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/**
   \file drv_tapi_io.h
   Contains TAPI I/O defines, enums and structures according to LTAPI
   specification.
   TAPI ioctl defines
      The file is divided in sections ioctl commands, constants, enumerations,
      structures for the following groups :
      - TAPI_INTERFACE_INIT
      - TAPI_INTERFACE_OP
      - TAPI_INTERFACE_METER
      - TAPI_INTERFACE_TONE
      - TAPI_INTERFACE_SIGNAL
      - TAPI_INTERFACE_CID
      - TAPI_INTERFACE_CON
      - TAPI_INTERFACE_MISC
      - TAPI_INTERFACE_EVENT
      - TAPI_INTERFACE_RINGING
      - TAPI_INTERFACE_CALIBRATION
      - TAPI_INTERFACE_COMTEL
      - TAPI_INTERFACE_PCM
      - TAPI_INTERFACE_FAX
      - TAPI_INTERFACE_FAX_STACK
      - TAPI_INTERFACE_DECT
      - TAPI_INTERFACE_MWL
      - TAPI_INTERFACE_PEAKD
   \note Changes:
   IFX_TAPI_MAP_DATA_t nPhoneCh changed to nDstCh because it can also be a PCM
   and two more enum entries for chnl type are added for audio channel.
   IFX_TAPI_MAP_DATA_t nDataChType changed to nChType for destination type
   IFX_TAPI_MAP_PCM_ADD and REMOVE added
*/

#include "drv_tapi_if_version.h"
#if 1 /* ctc */
 #include "ifx_types.h"
#else
 #include <ifx_types.h>
#endif

#ifdef WIN32
#define _IOWR(x,y,t)  (IOC_INOUT|(((long)sizeof (t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* WIN32 */


#ifdef TAPI_VERSION3
   #undef TAPI_ONE_DEVNODE
   #ifdef TAPI_DXY_DOC
      #define TAPI3_DXY_DOC
   #endif /* TAPI_DXY_DOC */
#endif /* TAPI_VERSION3 */

#ifdef TAPI_VERSION4
   #ifndef TAPI_ONE_DEVNODE
      #define TAPI_ONE_DEVNODE
   #endif /* TAPI_ONE_DEVNODE */
   #ifdef TAPI_DXY_DOC
      #define TAPI4_DXY_DOC
   #endif /* TAPI_DXY_DOC */
#endif /* TAPI_VERSION4 */

/** This definition is used to enable the old (outdated) capability list.
    The old capability list required as an argument a pointer to the array of
    IFX_TAPI_CAP_t structures.
    The new capability list requires a pointer to the IFX_TAPI_CAP_LIST_t
    structure, which contains the allocated memory size.
    \note: The old capability list is kept for backwards compatibility.
*/
#define USE_OLD_TAPI_CAP_LIST

#if defined (TAPI_VERSION3) && defined (TAPI_VERSION4)
   #error only single version can be specifyed
#endif /* TAPI_VERSION3 */

#if !defined (TAPI_VERSION3) && !defined (TAPI_VERSION4)
   #error Please specify TAPI version
#endif /* !defined (TAPI_VERSION3) && !defined (TAPI_VERSION4) */

/** \defgroup TAPI_INTERFACE TAPI Ioctl and Functions Reference
    This chapter describes all the services that can be used via the TAPI. The ioctl
    commands are described by means of the return values for each function.
    The chapter is organized as follows: */
/*@{*/

/** \defgroup TAPI_INTERFACE_CONTMEASUREMENT Analog Line Continuous Measurement
    Contains services to perform continuous measurement on the analog line.
    These measurements do not influence normal telephone operation. */

/** \defgroup TAPI_INTERFACE_CALIBRATION Calibration Services
      Calibration interfaces. */

/** \defgroup TAPI_INTERFACE_CID CID Features Services
      Contains services for configuring, sending and receiving the caller ID. */

/** \defgroup TAPI_INTERFACE_COMTEL COMTEL Signal Services
      COMTEL signal interfaces. */

/** \defgroup TAPI_INTERFACE_CON Connection Control Services
    Contains all services used for RTP or AAL connections. It also contains
    services for conferencing.
    Applies to data channels unless otherwise stated.  */

#ifndef TAPI4_DXY_DOC
/** \defgroup TAPI_INTERFACE_DECT DECT Services
    Contains services provided by the DECT channels. */
#endif /* #ifndef TAPI4_DXY_DOC */

/** \defgroup TAPI_INTERFACE_EVENT Event Reporting Services
    Contains services for event reporting. This is applicable to device file
    descriptors unless otherwise stated. */

/** \defgroup TAPI_INTERFACE_FAX Fax T.38 Data Pump Services
   The fax services switch the corresponding data channel from voice to a T.38
   data pump. All fax services apply to data channels unless otherwise stated.*/

/** \defgroup TAPI_INTERFACE_FAX_STACK Fax T.38 Stack Services
   Full fax T.38 stack control, making the T.38 data pump services obsolete.
   All fax services apply to data channels unless otherwise stated.*/

#ifndef TAPI4_DXY_DOC
/** \defgroup TAPI_INTERFACE_FXO FXO Services
    All FXO services apply to channel file descriptors unless otherwise stated.*/

/** \defgroup TAPI_INTERFACE_PHONE_DETECTION FXS Phone Detection
    Contains services to detect a connected telephone on an FXS line. */

/** \defgroup TAPI_INTERFACE_GR909 GR-909 Services
   Contains services and mechanisms to perform measurements according
   to the GR-909 standard. */
#endif /* #ifndef TAPI4_DXY_DOC */

/** \defgroup TAPI_INTERFACE_INIT Initialization Services
   These services set the default initialization of the device and hardware.*/

/** \defgroup TAPI_INTERFACE_PEAKD Level Peak Detector Services
    Contains services for the level peak detector. */

/** \defgroup TAPI_INTERFACE_MWL Message Waiting Lamp Services
    Contains services for the message waiting lamp. */

/** \defgroup TAPI_INTERFACE_METER Metering Services
    Contains services for metering.
    All metering services apply to phone channels unless otherwise stated. */

/** \defgroup TAPI_INTERFACE_MISC Miscellaneous Services
    Contains services for status and version information. */

/** \defgroup TAPI_INTERFACE_NLT Network Line Testing Services
    Contains services to perform network line testing. These services are
    used in conjunction with the TAPI NLT library provided by Lantiq. */

/** \defgroup TAPI_INTERFACE_OP Operation Control Services
    Modifies the operation of the device.
    All operation control services apply to phone channels unless otherwise
    stated.  */

/** \defgroup TAPI_INTERFACE_PCM PCM Services
    Contains services for PCM configuration.
    Applies to phone channels unless
    otherwise stated. */

#ifndef TAPI4_DXY_DOC
/** \defgroup TAPI_POLLING_SERVICE Polling Services
    All polling services apply to device file descriptors unless otherwise stated.*/
#endif /* #ifndef TAPI4_DXY_DOC */

/** \defgroup TAPI_INTERFACE_RINGING Power Ringing Services
      Ringing on FXS interfaces. */

/** \defgroup TAPI_INTERFACE_SIGNAL Signal Detection Services
   The application handles the different states of the detection status. */

/** \defgroup TAPI_INTERFACE_TEST Testing Services
    Contains services for system tests such as hook generation and loops. */

/** \defgroup TAPI_INTERFACE_TONE Tone Control Services
    All tone services apply to phone channels unless otherwise stated. */

/*@}*/

/* ========================================================================== */
/*                     TAPI Interface Ioctl Commands                          */
/* ========================================================================== */

/* Magic number for ioctls.*/
#define IFX_TAPI_IOC_MAGIC 'q'

/* include automatically generated TAPI ioctl command indexes */
#include "drv_tapi_io_indexes.h"

/* ======================================================================== */
/* TAPI Initialization Services, ioctl commands (Group TAPI_INTERFACE_INIT) */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** This service sets the default initialization of the device and of the
      specific channel. This command applies to any channel file descriptor.

   \note Deprecated service, use \ref IFX_TAPI_DEV_START instead.

   \param IFX_TAPI_CH_INIT_t* Pointer to an \ref IFX_TAPI_CH_INIT_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_CH_INIT_t param;

   memset (&param, 0, sizeof (param));

   param.nMode = 0;
   param.pProc = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_CH_INIT, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_CH_INIT                    _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CH_INIT_IDX, IFX_TAPI_CH_INIT_t)

/**
   This service starts the TAPI for the device file descriptor on which it is given.
   It reserves all resources needed by the TAPI for this specific device.
   This command applies to the device file descriptor.

   \param IFX_TAPI_DEV_START_CFG_t* Pointer to an \ref IFX_TAPI_DEV_START_CFG_t
          structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_DEV_START_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DEV_START, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_DEV_START                  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEV_START_IDX, IFX_TAPI_DEV_START_CFG_t)

/**
   This service stops the TAPI for the device file descriptor on which it is given.
   It frees all resources allocated by the TAPI for this specific device.
   This command applies to the device file descriptor.

   \param IFX_TAPI_DEV_START_CFG_t* Pointer to an \ref IFX_TAPI_DEV_START_CFG_t
          structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_DEV_START_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DEV_STOP, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   return IFX_SUCCESS;
   \endcode
*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_DEV_STOP                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEV_STOP_IDX, IFX_TAPI_DEV_START_CFG_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_DEV_STOP                _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEV_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ========================================================================= */
/* TAPI Operation Control Services, ioctl commands (Group TAPI_INTERFACE_OP) */
/* ========================================================================= */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** This service sets the line feeding mode.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.


   \param IFX_TAPI_LINE_FEED_t* Pointer to an \ref IFX_TAPI_LINE_FEED_t structure.

   \remarks The hardware must be able to support battery-switching modes,
   for example by programming coefficients.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_LINE_FEED_SET              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_FEED_SET_IDX, IFX_TAPI_LINE_FEED_t)

/** This service sets the line type (FXS, FXO) of an analog channel.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_LINE_TYPE_CFG_t* Pointer to an \ref IFX_TAPI_LINE_TYPE_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_LINE_TYPE_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.lineType = IFX_TAPI_LINE_TYPE_FXS;
   param.nDaaCh = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_LINE_TYPE_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_LINE_TYPE_SET              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_TYPE_SET_IDX, IFX_TAPI_LINE_TYPE_CFG_t)

/** Sets the line echo canceller (LEC) configuration for the analog line
    channel.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_WLEC_CFG_t* Pointer to an \ref IFX_TAPI_WLEC_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_WLEC_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nType = IFX_TAPI_WLEC_TYPE_NFE;
   param.bNlp = IFX_TAPI_WLEC_NLP_DEFAULT;
   param.nNBFEwindow = IFX_TAPI_WLEC_TYPE_NFE;
   param.nNBNEwindow = IFX_TAPI_WLEC_TYPE_NFE;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_WLEC_PHONE_CFG_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_WLEC_PHONE_CFG_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_WLEC_PHONE_CFG_SET_IDX, IFX_TAPI_WLEC_CFG_t)

/** Retrieves the LEC configuration of the analog line channel.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_WLEC_CFG_t* Pointer to an \ref IFX_TAPI_WLEC_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \note The ioctl IFX_TAPI_LEC_PHONE_CFG_GET is obsolete and has been replaced
    by this ioctl.

   \remarks WLEC mode is automatically mapped to NLEC mode when wideband
            is in use. However, this interface still reports the configured
            WLEC mode in this case.
*/
#define  IFX_TAPI_WLEC_PHONE_CFG_GET         _IOR  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_WLEC_PHONE_CFG_GET_IDX, IFX_TAPI_WLEC_CFG_t)

/** Sets the line echo canceller (LEC) configuration for the PCM channel.
    The file descriptor is applicable to phone channel file descriptors.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_WLEC_CFG_t* Pointer to an \ref IFX_TAPI_WLEC_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \note The ioctl IFX_TAPI_LEC_PCM_CFG_SET is obsolete and has been replaced
    by this ioctl.

   \remarks Note: WLEC mode is automatically mapped to NLEC mode when wideband
            is in use.
*/
#define  IFX_TAPI_WLEC_PCM_CFG_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_WLEC_PCM_CFG_SET_IDX, IFX_TAPI_WLEC_CFG_t)

/** Retrieves the line echo canceller (LEC) configuration of the PCM channel.
   This command applies to any channel file descriptor that
   includes a PCM module resource.

   \param IFX_TAPI_WLEC_CFG_t* Pointer to an \ref IFX_TAPI_WLEC_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \note The ioctl IFX_TAPI_LEC_PCM_CFG_GET is obsolete and has been replaced
    by this ioctl.

   \remarks Note: WLEC mode is automatically mapped to NLEC mode when wideband
            is in use. However, this interface still reports the configured
            WLEC mode in this case.
*/
#define  IFX_TAPI_WLEC_PCM_CFG_GET           _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_WLEC_PCM_CFG_GET_IDX, IFX_TAPI_WLEC_CFG_t)

/** Specifies the time for hook, pulse digit and hook flash validation.
   The file descriptor is applicable to phone channel file descriptors
   containing an analog interface.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_LINE_HOOK_VT_t* Pointer to an \ref IFX_TAPI_LINE_HOOK_VT_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks
   The following conditions must be met:
      - IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME
        min. and max. < IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME min. and max.
      - IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME
        min. and max. < IFX_TAPI_LINE_HOOK_VT_HOOKON_TIME min. and max.

   \code
   IFX_TAPI_LINE_HOOK_VT_t param;

   memset (&param, 0, sizeof (param));

   // Set pulse dialing
   param.nType = IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME;
   param.nMinTime = 40;
   param.nMaxTime = 60;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_LINE_HOOK_VT_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_LINE_HOOK_VT_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_HOOK_VT_SET_IDX, IFX_TAPI_LINE_HOOK_VT_t)

/** Sets the voice volume of the analog line module for the incoming and
    outgoing voice path. This is used for speaker phone and microphone
    volume settings. The file descriptor is applicable to phone channel
    file descriptors containing an analog interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_LINE_VOLUME_t* Pointer to an \ref IFX_TAPI_LINE_VOLUME_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_LINE_VOLUME_t param;

   memset (&param, 0, sizeof (param));
   param.nGainTx = 24;
   param.nGainRx = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PHONE_VOLUME_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_PHONE_VOLUME_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PHONE_VOLUME_SET_IDX, IFX_TAPI_LINE_VOLUME_t)

/** Sets the PCM interface volume settings.
   This command applies to any channel file descriptor that
   includes a PCM module resource.

   \param IFX_TAPI_LINE_VOLUME_t* Pointer to an \ref IFX_TAPI_LINE_VOLUME_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_LINE_VOLUME_t param;

   memset (&param, 0, sizeof (param));
   param.nGainTx = 24;
   param.nGainRx = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_VOLUME_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_PCM_VOLUME_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_VOLUME_SET_IDX, IFX_TAPI_LINE_VOLUME_t)


/** Sets the relative gain of the analog line module for the incoming and
    outgoing voice path. This is used for speaker phone and microphone
    volume settings.
    This command applies to ALM modules.

   \param IFX_TAPI_GAIN_t* Pointer to an \ref IFX_TAPI_GAIN_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_GAIN_t param;

   memset (&param, 0, sizeof (param));

   param.nGainTx = 24;
   param.nGainRx = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PHONE_RELATIVE_LEVEL_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_PHONE_RELATIVE_LEVEL_SET   _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PHONE_RELATIVE_LEVEL_SET_IDX, IFX_TAPI_GAIN_t)

/** Retrieves the relative gain configuration of the analog line module.
    This command applies to ALM modules.

   \param IFX_TAPI_GAIN_t* Pointer to an \ref IFX_TAPI_GAIN_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PHONE_RELATIVE_LEVEL_GET   _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PHONE_RELATIVE_LEVEL_GET_IDX, IFX_TAPI_GAIN_t)

/** This service enables or disables a high-level path of a phone channel.
   The high-level path might be required to play howler tones.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_LINE_LEVEL_CFG_t* Pointer to an \ref IFX_TAPI_LINE_LEVEL_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks This service is intended for use with phone channels only and must be used
   in combination with \ref IFX_TAPI_PHONE_VOLUME_SET or
   \ref IFX_TAPI_PCM_VOLUME_SET to set the maximum level
   (\ref IFX_TAPI_LINE_VOLUME_HIGH) or to restore the level.
    Only the order of calls with parameters
    IFX_TAPI_LINE_LEVEL_ENABLE and then IFX_TAPI_LINE_LEVEL_DISABLE
    is supported.

*/
#define  IFX_TAPI_LINE_LEVEL_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_LEVEL_SET_IDX, IFX_TAPI_LINE_LEVEL_CFG_t)

/**  This service reads the hook status from the driver.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_LINE_HOOK_STATUS_GET_t* Pointer to an \ref IFX_TAPI_LINE_HOOK_STATUS_GET_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \note 'Off-hook' corresponds to 'ground start', depending on the line feed mode.

*/
#define  IFX_TAPI_LINE_HOOK_STATUS_GET       _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_HOOK_STATUS_GET_IDX, IFX_TAPI_LINE_HOOK_STATUS_GET_t)

/*@}*/ /* TAPI_INTERFACE_OP */

/* =================================================================== */
/* TAPI Metering Services, ioctl commands (Group TAPI_INTERFACE_METER) */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** This service sends one burst of metering.
    This command applies to ALM modules.

   \param IFX_TAPI_METER_CFG_t* Pointer to an \ref IFX_TAPI_METER_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Bursts are sent out until an \ref IFX_TAPI_METER_STOP ioctl
            is called.

   \code
   IFX_TAPI_METER_CFG_t param;

   memset (&param, 0, sizeof (param));

   // set param characteristic
   // param mode is already set to 0
   // 100 ms pulse length
   param.nPulseLen = 100;
   // pause between two metering pulses
   // 100 ms length
   param.nPauseLen = 100;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_METER_CFG_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_METER_CFG_SET              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_CFG_SET_IDX, IFX_TAPI_METER_CFG_t)

/** This service starts the metering.
    This command applies to ALM modules.

   \param IFX_TAPI_METER_START_t* Pointer to an \ref IFX_TAPI_METER_START_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks Before this service can be used, the metering characteristic must
    be set (\ref IFX_TAPI_METER_CFG_SET) and the line mode must be set
    to normal (\ref IFX_TAPI_LINE_FEED_SET).

   \remarks To use transparent activation, set nPulseDist and nPulses to ZERO.
    For this mode, it is not necessary to set \ref IFX_TAPI_METER_CFG_SET.

   \code
   IFX_TAPI_METER_START_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_METER_START, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_METER_START                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_START_IDX, IFX_TAPI_METER_START_t)

/** This service stops the metering.
    This command applies to ALM modules.

   \param IFX_TAPI_METER_STOP_t* Pointer to an \ref IFX_TAPI_METER_STOP_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks If the metering has not been previously started, this service returns
    an error.

   \code
   IFX_TAPI_METER_STOP_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_METER_STOP, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_METER_STOP              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_STOP_IDX, IFX_TAPI_METER_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_METER_STOP              _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service sends one burst of metering.
    This command applies to ALM modules.

   \param IFX_TAPI_METER_BURST_t* Pointer to an \ref IFX_TAPI_METER_BURST_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks If the metering has not been previously started, this service returns
    an error.

   \code
   IFX_TAPI_METER_BURST_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_METER_BURST, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_METER_BURST                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_BURST_IDX, IFX_TAPI_METER_BURST_t)

/** This service reads the metering statistic.
    This command applies to ALM modules.

   \param IFX_TAPI_METER_STATISTICS_t* Pointer to an \ref IFX_TAPI_METER_STATISTICS_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks If the metering has not been previously started, this service returns
    an error.

   \code
   IFX_TAPI_METER_STATISTICS_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_METER_STATISTICS_GET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_METER_STATISTICS_GET       _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_METER_STATISTICS_GET_IDX, IFX_TAPI_METER_STATISTICS_t)

/*@}*/ /* TAPI_INTERFACE_METER */

/* ======================================================================= */
/* TAPI Tone Control Services, ioctl commands (Group TAPI_INTERFACE_TONE)  */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/**
   This service sets the tone level of the tone currently being played.
   \param IFX_TAPI_PREDEF_TONE_LEVEL_t* Pointer to an
   \ref IFX_TAPI_PREDEF_TONE_LEVEL_t structure.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks

   \code
   IFX_TAPI_PREDEF_TONE_LEVEL_t param;

   memset (&param, 0, sizeof (param));

   // all param generators
   param.nGenerator = 0xFF;
   param.nLevel = 0x100;
   // -38.97 dB level
   // playlevel in dB can be calculated via formula:
   // playlevel_in_dB = +3.17 + 20 log10 (nPlayLevel/32767)

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_LEVEL_SET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_TONE_LEVEL_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_LEVEL_SET_IDX, IFX_TAPI_PREDEF_TONE_LEVEL_t)

/** Configures a tone based on simple or composed tones. The tone is also added
    to the tone table.
    This command applies to the device file descriptor.

   \param IFX_TAPI_TONE_t* Pointer to an \ref IFX_TAPI_TONE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   A simple tone specifies a tone sequence composed of several single frequency
   tones or dual frequency tones. The sequence can be transmitted only once,
   several times or until transmission is stopped by the client.
   This interface can add a simple tone to the internal table with a maximum of
   222 entries, starting from IFX_TAPI_TONE_INDEX_MIN (32).
   At least one cadence must be defined, otherwise this interface returns an
   error. The tone table provides all tone frequencies in steps of 5 Hz and with a 5%
   tolerance, as defined in RFC 2833.
   For composed tones, the loop count of each simple tone must have a value other than 0.

   \code

   IFX_TAPI_TONE_t tone;
   IFX_TAPI_TONE_PLAY_t param;

   memset (&tone,0, sizeof (tone));
   tone.simple.format = IFX_TAPI_TONE_TYPE_SIMPLE;
   tone.simple.index = 71;
   tone.simple.freqA = 480;
   tone.simple.freqB = 620;
   tone.simple.levelA = -300;
   tone.simple.cadence[0] = 2000;
   tone.simple.cadence[1] = 2000;
   tone.simple.frequencies[0] = IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB;
   tone.simple.loop = 2;
   tone.simple.pause = 200;
   if (ioctl (fd, IFX_TAPI_TONE_TABLE_CFG_SET, (unsigned long) &tone) != IFX_SUCCESS)
      return IFX_ERROR;

   memset (&tone,0, sizeof (tone));
   tone.composed.format = IFX_TAPI_TONE_TYPE_COMPOSED;
   tone.composed.index = 100;
   tone.composed.count = 2;
   tone.composed.tones[0] = 71;
   tone.composed.tones[1] = 71;
   if (ioctl (fd, IFX_TAPI_TONE_TABLE_CFG_SET, (unsigned long) &tone) != IFX_SUCCESS)
      return IFX_ERROR;

   memset (&param,0, sizeof (param));
   // Play tone on analog line module towards the telephone
   param.module = IFX_TAPI_MODULE_TYPE_ALM;
   // Play tone index 100
   param.index = 100;
   param.external = 1;
   param.internal = 0;

   // Play tone on the third device
   param.dev = 2;
   // Play tone on the second channel
   param.ch = 1;
   // Start to play
   if (ioctl (fd, IFX_TAPI_TONE_PLAY, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   sleep(5);

   //Stop playing tone
   if (ioctl (fd, IFX_TAPI_TONE_STOP, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   \endcode
*/
#define  IFX_TAPI_TONE_TABLE_CFG_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_TABLE_CFG_SET_IDX, IFX_TAPI_TONE_t)

/** Start/stop generation of a tone towards TDM, analog line, conf-bridge or
    RTP. The 'index' parameter of \ref IFX_TAPI_TONE_PLAY_t (if greater than 0)
     gives the tone table index of the tone to be played.
    If the parameter is equal to zero, stop the current tone generation.
    This command applies to ALM, PCM and COD modules.

   \param IFX_TAPI_TONE_PLAY_t* Pointer to an \ref IFX_TAPI_TONE_PLAY_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   This can be a pre-defined simple or a composed tone.
   The tone codes are assigned previously on system start.
   Index 1 - 31 is pre-defined by the driver and covers the
   original TAPI.
   All other indices can be custom-defined by
   \ref IFX_TAPI_TONE_TABLE_CFG_SET.

   \code
   IFX_TAPI_TONE_PLAY_t param;

   memset (&param, 0, sizeof (param));

   // Play tone on analog line module towards the telephone
   param.module = IFX_TAPI_MODULE_TYPE_ALM;
   // Play tone index 34
   param.index = 34;
   param.external = 1;
   param.internal = 0;

   // Play tone on the third device
   param.dev = 2;
   // Play tone on the second channel
   param.ch = 1;
   if (ioctl (fd, IFX_TAPI_TONE_PLAY, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_TONE_PLAY                  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_PLAY_IDX, IFX_TAPI_TONE_PLAY_t)

#ifndef TAPI4_DXY_DOC
   /** Starts/stops generation of a tone towards the local port; the parameter
   (if greater than 0) gives the tone table index of the tone to be played.
   If the parameter is equal to zero, the current tone generation stops.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_IDX_t* Pointer to an \ref IFX_TAPI_TONE_IDX_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   This can be a pre-defined simple or a composed tone. The tone codes are
   assigned previously upon system start.
   Index 1 - 31 is pre-defined by the driver and covers the original TAPI.
   A timing value must be specified with \ref IFX_TAPI_TONE_ON_TIME_SET and
   \ref IFX_TAPI_TONE_OFF_TIME_SET before.

*/
#define  IFX_TAPI_TONE_LOCAL_PLAY            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_LOCAL_PLAY_IDX, IFX_TAPI_TONE_IDX_t)

   /** Starts/stops generation of a tone towards the network; the parameter
   (if greater than 0) gives the tone table index of the tone to be played.
   If the parameter is equal to zero, the current tone generation stops.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_IDX_t* Pointer to an \ref IFX_TAPI_TONE_IDX_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   This can be a pre-defined simple or a composed tone. The tone codes are
   assigned previously upon system start.
   Index 1 - 31 is pre-defined by the driver and covers the original TAPI.
   A timing value must be specified with \ref IFX_TAPI_TONE_ON_TIME_SET and
    \ref IFX_TAPI_TONE_OFF_TIME_SET before.
   All other indices can be custom-defined by \ref IFX_TAPI_TONE_TABLE_CFG_SET.

*/
#define  IFX_TAPI_TONE_NET_PLAY              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_NET_PLAY_IDX, IFX_TAPI_TONE_IDX_t)

/** Pre-defined tone services for busy tone.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param int This interface expects no parameters. It should be set to 0.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for the USA.

   \code
   IFX_TAPI_TONE_BUSY_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_BUSY_PLAY, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_TONE_BUSY_PLAY          _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_BUSY_PLAY_IDX, IFX_TAPI_TONE_BUSY_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_TONE_BUSY_PLAY          _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_BUSY_PLAY_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Pre-defined tone services for the ring back tone.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param int This interface expects no parameters. It should be set to 0.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for the USA.

   \code
   IFX_TAPI_TONE_RINGBACK_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_RINGBACK_PLAY, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_TONE_RINGBACK_PLAY      _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_RINGBACK_PLAY_IDX, IFX_TAPI_TONE_RINGBACK_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_TONE_RINGBACK_PLAY      _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_RINGBACK_PLAY_IDX)
#endif /* TAPI_ONE_DEVNODE */

/**
   Pre-defined tone services for the dial tone.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param int This interface expects no parameters. It should be set to 0.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for the USA.

   \code
   IFX_TAPI_TONE_DIALTONE_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_DIALTONE_PLAY, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_TONE_DIALTONE_PLAY      _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_DIALTONE_PLAY_IDX, IFX_TAPI_TONE_DIALTONE_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_TONE_DIALTONE_PLAY      _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_DIALTONE_PLAY_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Stops playback of a specified tone.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_IDX_t* Pointer to an \ref IFX_TAPI_TONE_IDX_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Stopping of all tones played in this direction can be done with
   index 0. Passing index 0 to \ref IFX_TAPI_TONE_LOCAL_PLAY has the same effect.

*/
#define  IFX_TAPI_TONE_LOCAL_STOP            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_LOCAL_STOP_IDX, IFX_TAPI_TONE_IDX_t)

/** Stops playback of a specified tone.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_IDX_t* Pointer to an \ref IFX_TAPI_TONE_IDX_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Stopping of all tones played in this direction can be done with
   index 0. Passing index 0 to \ref IFX_TAPI_TONE_NET_PLAY has the same effect.

*/
#define  IFX_TAPI_TONE_NET_STOP              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_NET_STOP_IDX, IFX_TAPI_TONE_IDX_t)
#endif /* #ifndef TAPI4_DXY_DOC */

/** Stops tone generation.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_PLAY_t* Pointer to an \ref IFX_TAPI_TONE_PLAY_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Assigning an 'index' of 0 will stop playback of all tones in all
   directions. Tones can also be stopped with \ref IFX_TAPI_TONE_PLAY
   and 'index' set to zero.
   The function returns an error in case there is no tone to stop.

   \code
   IFX_TAPI_TONE_PLAY_t param;

   memset (&param, 0, sizeof (param));

   // Stop tone on analog line module towards the telephone
   param.module = IFX_TAPI_MODULE_TYPE_ALM;
   // Stop to play
   param.external = 1;
   param.internal = 0;
   // Set to zero but the TAPI ignores this parameter
   param.index = 0;

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_STOP, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_TONE_STOP                  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_STOP_IDX, IFX_TAPI_TONE_PLAY_t)

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ======================================================================= */
/* TAPI Signal Detection Services, ioctl commands                          */
/* (Group TAPI_INTERFACE_SIGNAL)                                           */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

#ifndef TAPI4_DXY_DOC
/** Enables signal detection for fax/modem signals.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_SIG_DETECTION_t* Pointer to an \ref IFX_TAPI_SIG_DETECTION_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks For firmware providing ATD/UTD, check the capability list for
    detection services and the number of detectors available.
   Signal events are reported via an exception. The information is queried with
   \ref IFX_TAPI_EVENT_GET. If AM is detected, the driver may automatically switch
   to modem coefficients after CED is ended. Not every combination is possible,
   depending on the underlying device and firmware. For firmware providing MFTD,
   there are no limitations on the combination of signals that can be detected.
   When re-programming the detectors, the driver has to stop the detector for a short time.

   \code
   IFX_TAPI_SIG_DETECTION_t param;

   memset (&param, 0, sizeof (param));

   param.sig_ext = IFX_TAPI_SIG_CEDTX;
   param.sig = IFX_TAPI_SIG_CEDTX | IFX_TAPI_SIG_PHASEREVRX;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_SIG_DETECT_ENABLE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode  */
#define  IFX_TAPI_SIG_DETECT_ENABLE          _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_SIG_DETECT_ENABLE_IDX, IFX_TAPI_SIG_DETECTION_t)

/** Disables signal detection for fax or modem signals.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_SIG_DETECTION_t* Pointer to an \ref IFX_TAPI_SIG_DETECTION_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \note Use of the new event reporting interfaces is strongly recommended.
      This interface will be discontinued in a future TAPI release.
*/

#define  IFX_TAPI_SIG_DETECT_DISABLE         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_SIG_DETECT_DISABLE_IDX, IFX_TAPI_SIG_DETECTION_t)
#endif /* #ifndef TAPI4_DXY_DOC */

/** This service is used to set DTMF receiver coefficients.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DTMF_RX_CFG_t* Pointer to an \ref IFX_TAPI_DTMF_RX_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks If enabled, the DTMF receiver will be temporarily disabled during
    the writing of the coefficients.

   \code
   IFX_TAPI_DTMF_RX_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nLevel   = -56;   // dB
   param.nTwist   = 9;     // dB
   param.nGain    = 0;     // dB

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DTMF_RX_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_DTMF_RX_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DTMF_RX_CFG_SET_IDX, IFX_TAPI_DTMF_RX_CFG_t)

/** Retrieves DTMF receiver coefficients.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DTMF_RX_CFG_t* Pointer to an \ref IFX_TAPI_DTMF_RX_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_DTMF_RX_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DTMF_RX_CFG_GET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_DTMF_RX_CFG_GET            _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DTMF_RX_CFG_GET_IDX, IFX_TAPI_DTMF_RX_CFG_t)

/** Starts the call progress tone detection based on a previously defined
   simple tone.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_CPTD_t* Pointer to an \ref IFX_TAPI_TONE_CPTD_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_TONE_CPTD_t param;

   memset (&param, 0, sizeof (param));
   param.tone   = 74;
   param.external = 1;
   param.internal = 0;
   param.module = IFX_TAPI_MODULE_TYPE_ALM;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TONE_CPTD_START, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_TONE_CPTD_START            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_CPTD_START_IDX, IFX_TAPI_TONE_CPTD_t)

/** Stops call progress tone detection.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_TONE_CPTD_t* Pointer to an \ref IFX_TAPI_TONE_CPTD_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
   */
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_TONE_CPTD_STOP             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_CPTD_STOP_IDX, IFX_TAPI_TONE_CPTD_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_TONE_CPTD_STOP             _IO  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_CPTD_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */


/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ======================================================================= */
/* TAPI CID Features Service, ioctl commands  (Group TAPI_INTERFACE_CID)   */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** Sets up the CID receiver to start receiving CID data.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_CID_RX_CFG_t* Pointer to an \ref IFX_TAPI_CID_RX_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks This command must be sent so that the driver can start collecting
    CID data.

*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_CID_RX_START            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_START_IDX, IFX_TAPI_CID_RX_CFG_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_CID_RX_START            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_START_IDX, IFX_TAPI_CID_HOOK_MODE_t)
#endif /* TAPI_ONE_DEVNODE */

/** Stops the CID receiver. The file descriptor is applicable to data channel
    file descriptors.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_CID_RX_CFG_t* Pointer to an \ref IFX_TAPI_CID_RX_CFG_t structure. The 'hookMode' parameter is ignored by the TAPI.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_CID_RX_STOP             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_STOP_IDX, IFX_TAPI_CID_RX_CFG_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_CID_RX_STOP             _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Retrieves the current status information of the CID receiver.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_CID_RX_STATUS_t* Pointer to an
   \ref IFX_TAPI_CID_RX_STATUS_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Once the CID receiver is activated, it signals the status with an
   event. The event is raised if data has been received completely or
   an error occurred. Afterwards, this interface is used to determine whether data
   has been received. In this case, it can be read with the interface
   \ref IFX_TAPI_CID_RX_DATA_GET. This interface can also be used without
   exception to determine the status of the receiver during reception.

   \code
   IFX_TAPI_CID_RX_STATUS_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;

   // Check if CID information is available for reading
   if (ioctl (fd, IFX_TAPI_CID_RX_STATUS_GET, (unsigned long) &param) ==
      IFX_SUCCESS)
      return IFX_ERROR;

   if ((IFX_TAPI_CID_RX_ERROR_NONE == param.nError)
      && (IFX_TAPI_CID_RX_STATE_DATA_READY == param.nStatus))
   {
   printf ( "Data is ready for reading\n\r" );
      return IFX_SUCCESS;
   }

   return IFX_ERROR;
   \endcode
*/

#define  IFX_TAPI_CID_RX_STATUS_GET          _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_STATUS_GET_IDX, IFX_TAPI_CID_RX_STATUS_t)

/** Reads CID data collected by the caller ID receiver.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_CID_RX_DATA_t* Pointer to an
   \ref IFX_TAPI_CID_RX_DATA_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks This request can be sent after the CID receiver status signals
    that data is ready for reading or that data collection is ongoing. In the
    last case, the amount of data read corresponds to the amount of data
    received since the CID receiver was started. Once the data is read after the
    status signals that data is ready, new data can be read only if the CID
    receiver detects new data.

   \code
   IFX_TAPI_CID_RX_STATUS_t cidStatus;
   IFX_TAPI_CID_RX_DATA_t cidData;

   memset (&cidStatus, 0, sizeof (cidStatus));
   memset (&cidData, 0, sizeof (cidData));

   // First channel on the first device
   cidStatus.ch = 0;
   cidStatus.dev = 0;

   cidData.ch = 0;
   cidData.dev = 0;

   // Check if CID data is available for reading
   if ((IFX_SUCCESS == ioctl (fd, IFX_TAPI_CID_RX_STATUS_GET, (unsigned long) &cidStatus))
   && (IFX_TAPI_CID_RX_ERROR_NONE == cidStatus.nError)
   && (IFX_TAPI_CID_RX_STATE_DATA_READY == cidStatus.nStatus))
   {
      ioctl (fd, IFX_TAPI_CID_RX_DATA_GET, (unsigned long) &cidData);
   }
   else
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/

#define  IFX_TAPI_CID_RX_DATA_GET            _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_RX_DATA_GET_IDX, IFX_TAPI_CID_RX_DATA_t)

/** This interface transmits CID messages.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param  IFX_TAPI_CID_MSG_t*  Pointer to an \ref IFX_TAPI_CID_MSG_t structure,
   containing the CID/MWI information to be transmitted.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Before issuing this service, the CID engine must be configured with
   IFX_TAPI_CID_CFG_SET at least once after boot. This is required to
   configure country-specific settings.

   \code
   IFX_TAPI_CID_MSG_t Cid_Info;
   IFX_TAPI_CID_MSG_ELEMENT_t Msg_El[2];
   IFX_char_t* number = "12345";

   memset(&Cid_Info, 0, sizeof(Cid_Info));
   memset(&Msg_El, 0, sizeof(Msg_El));

   Cid_Info.txMode = IFX_TAPI_CID_HM_ONHOOK;

   // Message Waiting
   Cid_Info.messageType = IFX_TAPI_CID_MT_MWI;
   Cid_Info.nMsgElements = 2;
   Cid_Info.message = Msg_El;

   // Mandatory for Message Waiting: set Visual Indicator on
   Msg_El[0].value.elementType = IFX_TAPI_CID_ST_VISINDIC;
   Msg_El[0].value.element = IFX_TAPI_CID_VMWI_EN;

   // Add optional CLI (number) element
   Msg_El[1].string.elementType = IFX_TAPI_CID_ST_CLI;
   Msg_El[1].string.len = strlen(number);
   strncpy(Msg_El[1].string.element, number, sizeof(Msg_El[1].string.element));

   // First channel on the first device
   Cid_Info.ch = 0;
   Cid_Info.dev = 0;
   // Transmit the caller id
   if (ioctl (fd, IFX_TAPI_CID_TX_INFO_START, (unsigned long) &Cid_Info) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/

#define  IFX_TAPI_CID_TX_INFO_START          _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_TX_INFO_START_IDX, IFX_TAPI_CID_MSG_t)

/** This service stops the ongoing transmission of a CID message that was
   started with \ref IFX_TAPI_CID_TX_INFO_START.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_CID_TX_INFO_STOP_t* Pointer to an \ref IFX_TAPI_CID_TX_INFO_STOP_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_CID_TX_INFO_STOP_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_CID_TX_INFO_STOP, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
  */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_CID_TX_INFO_STOP        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_TX_INFO_STOP_IDX, IFX_TAPI_CID_TX_INFO_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_CID_TX_INFO_STOP        _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_TX_INFO_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service starts a pre-programmed CID sequence driven by the TAPI. This is a
   non-blocking service; the driver will signal the end of the CID sequence with an event.
   Before issuing this service, the CID engine must be configured with
   IFX_TAPI_CID_CFG_SET at least once after boot. This is required to
   configure country-specific settings.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param  IFX_TAPI_CID_MSG_t*  Pointer to an \ref IFX_TAPI_CID_MSG_t structure
   defining the CID/MWI type and containing the information to be transmitted.

   \remarks For FSK transmission, the decision of seizure and mark length is based
    on the configured standard and CID transmission type.
*/
#define  IFX_TAPI_CID_TX_SEQ_START           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_TX_SEQ_START_IDX, IFX_TAPI_CID_MSG_t)

/** Configures the CID transmitter.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param  IFX_TAPI_CID_CFG_t*  Pointer to an \ref IFX_TAPI_CID_CFG_t structure
   containing CID / MWI configuration information.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

  \remarks The delay must be programmed so that the CID data would still fit
   between the ring burst in case of appearance mode 1 and 2, otherwise the
   ioctl \ref IFX_TAPI_RING_START may return an error. CID transmission is stopped when the
   ringing is stopped or the phone goes off-hook.

   \code
      IFX_TAPI_CID_CFG_t param;

   memset (&param, 0, sizeof (param));

      // Set CID standard to Telcordia/Bellcore default values
      param.nStandard = IFX_TAPI_CID_STD_TELCORDIA;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_CID_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/

#define  IFX_TAPI_CID_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CID_CFG_SET_IDX, IFX_TAPI_CID_CFG_t)

/*@}*/ /* TAPI_INTERFACE_CID */

/* ======================================================================= */
/* TAPI Connection Services, ioctl commands (Group TAPI_INTERFACE_CON)     */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

#ifndef TAPI4_DXY_DOC
/** This interface configures AAL fields for a new connection.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PCK_AAL_CFG_t* Pointer to an
   \ref IFX_TAPI_PCK_AAL_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PCK_AAL_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nTimestamp = 0x1234;

   param.nCid=0;
   param.nCpsCid=0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_AAL_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_AAL_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_AAL_CFG_SET_IDX, IFX_TAPI_PCK_AAL_CFG_t)

/** AAL profile configuration.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PCK_AAL_PROFILE_t* Pointer to an
   \ref IFX_TAPI_PCK_AAL_PROFILE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
    Sets each row in a profile. A maximum number of 10 rows can be set up. This
    interface is called on initialization, to program the selected AAL profile.
    The following example programs the ATMF profiles 3, 4 and 5.

   \code
   IFX_TAPI_PCK_AAL_PROFILE_t param;
   IFX_int32_t nProfile;

   memset (&param, 0, sizeof (param));

   switch (nProfile)
   {
      case  3:
      case  4:
      // ATMF profile 3 and 4
      param.rows = 1;
      param.codec[0] = IFX_TAPI_COD_TYPE_MLAW;
      param.len[0] = 43;
      param.nUUI[0] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7;
      break;
      case  5:
      // ATMF profile 5
      param.rows = 2;
      param.codec[0] = IFX_TAPI_COD_TYPE_MLAW;
      param.len[0] = 43;
      param.nUUI[0] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7;
      param.codec[1] = IFX_TAPI_COD_TYPE_G726_32;
      param.len[1] = 43;
      param.nUUI[1] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15;
      break;
   }

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_AAL_PROFILE_SET, (unsigned long) &param)
 != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_PKT_AAL_PROFILE_SET        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_AAL_PROFILE_SET_IDX, IFX_TAPI_PCK_AAL_PROFILE_t)
#endif /* #ifndef TAPI4_DXY_DOC */

/** This interface configures RTP and RTCP fields for a new connection.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_RTP_CFG_t* Pointer to an
   \ref IFX_TAPI_PKT_RTP_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PKT_RTP_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nSeqNr = 0x1234;

   param.nEventPlayPT = 18;
   param.nEventPT = 18;
   param.nEvents = IFX_TAPI_PKT_EV_OOB_ONLY;
   // Play out the signal upon reception of RFC 2833 packets
   param.nPlayEvents = IFX_TAPI_PKT_EV_OOBPLAY_PLAY;
   param.nSsrc = 0;
   param.nTimestamp = 0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_RTP_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_RTP_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTP_CFG_SET_IDX, IFX_TAPI_PKT_RTP_CFG_t)

/** This service controls the DTMF sending mode of out-of-band (OOB)
    information: RFC 2833 packets.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_EV_OOB_DTMF_t* Pointer to an \ref IFX_TAPI_PKT_EV_OOB_DTMF_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PKT_EV_OOB_DTMF_SET        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_EV_OOB_DTMF_SET_IDX, IFX_TAPI_PKT_EV_OOB_DTMF_t)

#define IFX_TAPI_PKT_EV_OOB_SET              IFX_TAPI_PKT_EV_OOB_DTMF_SET // ctc

/** This service controls the MFTD sending mode of out-of-band (OOB)
    information: RFC 2833 packets.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_EV_OOB_MFTD_t* Pointer to an
          \ref IFX_TAPI_PKT_EV_OOB_MFTD_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PKT_EV_OOB_MFTD_SET        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_EV_OOB_MFTD_SET_IDX, IFX_TAPI_PKT_EV_OOB_MFTD_t)

/** Configures the payload type table.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_RTP_PT_CFG_t* Pointer to an
   \ref IFX_TAPI_PKT_RTP_PT_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   The requested payload type should have been negotiated with the peer prior
   to the connection set-up. Event payload types are negotiated during the
   signaling phase, and the driver and device can be programmed accordingly
   at the time of starting the media session. Event payload types are not
   modified when the session is in progress.

   \code
   IFX_TAPI_PKT_RTP_PT_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nPTup[6] = 0x5;
   param.nPTdown[12] = 0x4;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_RTP_PT_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_RTP_PT_CFG_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTP_PT_CFG_SET_IDX, IFX_TAPI_PKT_RTP_PT_CFG_t)

/** Configures the session packet header information. This configuration is
    used in case the device supports direct Ethernet packet exchange for
    RTP/UDP/IP/VLAN/MAC packets with the physical Ethernet hardware. This
    interface configures the protocol header information for sent packets
    and filter parameters for receive packets.
    The VLAN IDs of the VLAN configuration have to be set to zero in case
    VLAN is to be used or if the device does not support VLAN tagging.
    The command applies to COD modules.

   \param IFX_TAPI_PKT_NET_CFG_t* Pointer to an \ref IFX_TAPI_PKT_NET_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   Not all devices support Ethernet data packet exchange. In this case
   the TAPI configuration returns with an error.
   Some devices support Ethernet data packet configuration but not VLAN
   tagging. In this case the TAPI would return with an error if a VLAN
   configuration is given.
*/
#define  IFX_TAPI_PKT_NET_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_NET_CFG_SET_IDX, IFX_TAPI_PKT_NET_CFG_t)

/** Retrieves RTCP statistics.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_RTCP_STATISTICS_t* Pointer to an
   \ref IFX_TAPI_PKT_RTCP_STATISTICS_t structure according to RFC 3550/3551
    for a sender report.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks Not all statistics may be supported by the device.

   \code
   IFX_TAPI_PKT_RTCP_STATISTICS_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_RTCP_STATISTICS_GET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_RTCP_STATISTICS_GET    _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTCP_STATISTICS_GET_IDX, IFX_TAPI_PKT_RTCP_STATISTICS_t)

/** Resets the RTCP statistics.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_RTCP_STATISTICS_RESET_t* Pointer to an \ref IFX_TAPI_PKT_RTCP_STATISTICS_RESET_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PKT_RTCP_STATISTICS_RESET_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_RTCP_STATISTICS_RESET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_PKT_RTCP_STATISTICS_RESET  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTCP_STATISTICS_RESET_IDX, IFX_TAPI_PKT_RTCP_STATISTICS_RESET_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_PKT_RTCP_STATISTICS_RESET  _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTCP_STATISTICS_RESET_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Retrieves and resets the extended RTP statistics.
    This command returns with an error in case no firmware is downloaded.
    The command applies to COD modules.

   \param IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET_t* Pointer to an \ref IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET  _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET_IDX, IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET_t)

/** Configure the extended RTP statistics.
    This command returns with an error in case no firmware is downloaded.
    The command applies to COD modules.

   \param IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG_t* Pointer to an \ref IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG  _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG_IDX, IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG_t)

/** This service is used to generate RFC 2833 and ABCD events
    (RFC 5244 and RFC 4733) from the application software.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_EV_GENERATE_t* Pointer to an \ref IFX_TAPI_PKT_EV_GENERATE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PKT_EV_GENERATE_t param;

   memset (&param, 0, sizeof (param));

   param.action = IFX_TAPI_EV_GEN_START;
   param.event = 64;
   param.duration = 5;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_EV_GENERATE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_EV_GENERATE            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_EV_GENERATE_IDX, IFX_TAPI_PKT_EV_GENERATE_t)

/** This service is used to configure the generation of
    ABCD events (RFC 4733 and RFC 5244) from the application software.
    The command applies to COD modules.

   \param IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_t* Pointer to an \ref IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Enable ABCD event reporting
   param.bABCD_DisableEventReporting = 0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG   _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_IDX, IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_t)

/**
   Flush the upstream packet FIFO on a channel.

   \note This command flushes the transmit and receive packet buffer and should be called
   while switching a call from voice to fax, or vice versa. It avoids race conditions that can occur
   when a fax call is still receiving voice packets, or vice versa.

*/

#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_PKT_FLUSH               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_FLUSH_IDX, IFX_TAPI_PKT_FLUSH_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_PKT_FLUSH               _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PKT_FLUSH_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Configures the jitter buffer.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_JB_CFG_t* Pointer to an \ref IFX_TAPI_JB_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks Selects the fixed or adaptive jitter buffer and sets parameters. Modifies
    the jitter buffer settings during run-time.

   \code
   IFX_TAPI_JB_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Set to adaptive jitter buffer type
   param.nJbType = IFX_TAPI_JB_TYPE_ADAPTIVE;

   param.nLocalAdpt = IFX_TAPI_JB_LOCAL_ADAPT_ON;
   param.nScaling = 8;

   // Reconfigure JB for fax/modem communications
   param.nJbType = IFX_TAPI_JB_TYPE_FIXED;
   param.nPckAdpt = IFX_TAPI_JB_PKT_ADAPT_DATA;
   // The JB size are strictly application dependent
   // Initial JB size 90 ms
   param.nInitialSize = 0x02D0;
   // Minimum JB size 10 ms
   param.nMinSize = 0x50;
   // Maximum JB size 180 ms
   param.nMaxSize = 0x5A0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_JB_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_JB_CFG_SET                 _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_JB_CFG_SET_IDX, IFX_TAPI_JB_CFG_t)

/** Reads out jitter buffer statistics.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_JB_STATISTICS_t* Pointer to an \ref IFX_TAPI_JB_STATISTICS_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_JB_STATISTICS_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_JB_STATISTICS_GET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_JB_STATISTICS_GET          _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_JB_STATISTICS_GET_IDX, IFX_TAPI_JB_STATISTICS_t)

/** Resets the jitter buffer statistics.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_JB_STATISTICS_RESET_t* Pointer to an \ref IFX_TAPI_JB_STATISTICS_RESET_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_JB_STATISTICS_RESET_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_JB_STATISTICS_RESET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_JB_STATISTICS_RESET     _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_JB_STATISTICS_RESET_IDX, IFX_TAPI_JB_STATISTICS_RESET_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_JB_STATISTICS_RESET     _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_JB_STATISTICS_RESET_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This interface connects a phone channel to another phone channel.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_MAP_PHONE_t* Pointer to an
   \ref IFX_TAPI_MAP_PHONE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks The host has to take care of resource management.

   \code
   IFX_TAPI_MAP_PHONE_t param;

   memset (&param, 0, sizeof (param));

   // Add phone 2 (aram.nPhoneCh = 2) to phone 1 (param.ch = 1)
   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nPhoneCh = 2;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_PHONE_ADD, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_MAP_PHONE_ADD              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_PHONE_ADD_IDX, IFX_TAPI_MAP_PHONE_t)

/** This interface removes a phone channel from another phone channel.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_MAP_PHONE_t* Pointer to an
   \ref IFX_TAPI_MAP_PHONE_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   Applies to a phone channel file descriptor.

   \code
   IFX_TAPI_MAP_PHONE_t param;

   memset (&param, 0, sizeof (param));

   // Remove phone 2 (aram.nPhoneCh = 2) to phone 1 (param.ch = 1)
   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nPhoneCh = 2;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_PHONE_REMOVE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_MAP_PHONE_REMOVE           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_PHONE_REMOVE_IDX, IFX_TAPI_MAP_PHONE_t)

/** This interface connects the data channel to a phone channel.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \note It is recommended that the destination is chosen using the
   \ref IFX_TAPI_MAP_TYPE_t enumerator. The TAPI includes the IFX_TAPI_MAP_DATA_TYPE_t
    enumerator for backwards compatibility reasons only.

   \param IFX_TAPI_MAP_DATA_t* Pointer to an
   \ref IFX_TAPI_MAP_DATA_t structure

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks The host has to take care of resource management. It has to
    count the data channel (codec) resource usage and maintain a list of which
    data channel is mapped to which phone channel. The available resources can
    be queried using \ref IFX_TAPI_CAP_CHECK.

   \code
   IFX_TAPI_MAP_DATA_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the device
   param.ch = 1;
   // Second device
   param.dev = 1;

   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nPlayStart = IFX_TAPI_MAP_DATA_UNCHANGED;
   param.nRecStart = IFX_TAPI_MAP_DATA_UNCHANGED;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   // Add phone 0 (param.nDstCh = 0) to data 1 (param.ch = 1)
   param.nDstCh = 0;

   if (ioctl (fd, IFX_TAPI_MAP_DATA_ADD, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   // Add phone 1 (param.nDstCh = 1) to data 1 (param.ch = 1)
   param.nDstCh = 1;

   if (ioctl (fd, IFX_TAPI_MAP_DATA_ADD, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_DATA_ADD               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_DATA_ADD_IDX, IFX_TAPI_MAP_DATA_t)

/** This interface removes a data channel from an analog phone device.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_MAP_DATA_t* Pointer to an
   \ref IFX_TAPI_MAP_DATA_t structure

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks The host has to take care of resource management. It has to
   count the data channel (codec) resource usage and maintain a list of which
   data channel is mapped to which phone channel. The available resources can
   be queried using \ref IFX_TAPI_CAP_CHECK.

   \note It is recommended that the destination is chosen using the IFX_TAPI_MAP_TYPE_t
   enumerator! The TAPI includes the IFX_TAPI_MAP_DATA_TYPE_t enumerator for
   backwards compatibility reasons only.

   \code
   IFX_TAPI_MAP_DATA_t param;

   memset (&param, 0, sizeof (param));

   // Do not modify the status of the encoder or the decoder
   param.nPlayStart = IFX_TAPI_MAP_DATA_UNCHANGED;
   param.nRecStart = IFX_TAPI_MAP_DATA_UNCHANGED;

   // Remove the coder from an analog line module 1
   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nDstCh = 1;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   // Remove connection between phone channel 1 (param.nDstCh = 1) and data channel 1 (param.ch = 1)
   if (ioctl (fd, IFX_TAPI_MAP_DATA_REMOVE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   // Now phone and data resources are unmapped
   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_DATA_REMOVE            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_DATA_REMOVE_IDX, IFX_TAPI_MAP_DATA_t)

/** This interface connects the PCM channel to a phone channel.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_MAP_PCM_t* Pointer to an
   \ref IFX_TAPI_MAP_PCM_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_MAP_PCM_t param;

   memset (&param, 0, sizeof (param));

   // Connect PCM2 (param.ch = 1) to phone channel 1 (param.nDstCh = 1)
   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nDstCh = 1;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_PCM_ADD, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_PCM_ADD                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_PCM_ADD_IDX, IFX_TAPI_MAP_PCM_t)

/** This interface removes the PCM channel from the phone channel.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_MAP_PCM_t* Pointer to an
   \ref IFX_TAPI_MAP_PCM_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_MAP_PCM_t param;

   memset (&param, 0, sizeof (param));

   // PCM channel 1 (param.ch = 1) is removed from phone channel 2 (param.nDstCh = 2)
   param.nChType = IFX_TAPI_MAP_TYPE_PHONE;
   param.nDstCh = 2;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_PCM_REMOVE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_PCM_REMOVE             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_PCM_REMOVE_IDX, IFX_TAPI_MAP_PCM_t)

#ifndef TAPI4_DXY_DOC
/** This interface connects a DECT channel to another ALM, PCM or DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_MAP_DECT_t* Pointer to an
   \ref IFX_TAPI_MAP_DECT_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_MAP_DECT_t param;

   memset (&param, 0, sizeof (param));

   // PCM channel 2 (param.nDstCh = 2) is added to DECT channel 1 (param.ch = 1)
   param.nChType = IFX_TAPI_MAP_TYPE_PCM;
   param.nDstCh = 2;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_DECT_REMOVE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_DECT_ADD               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_DECT_ADD_IDX, IFX_TAPI_MAP_DECT_t)

/** This interface disconnects a DECT channel from another ALM, PCM or
    DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_MAP_DECT_t* Pointer to an
   \ref IFX_TAPI_MAP_DECT_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_MAP_DECT_t param;

   memset (&param, 0, sizeof (param));

   // PCM channel 2 (param.nDstCh = 2) is removed from DECT channel 1 (param.ch = 1)
   param.nChType = IFX_TAPI_MAP_TYPE_PCM;
   param.nDstCh = 2;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_MAP_DECT_REMOVE, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_MAP_DECT_REMOVE            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_DECT_REMOVE_IDX, IFX_TAPI_MAP_DECT_t)
#endif /* #ifndef TAPI4_DXY_DOC */

/** This interface connects a monitoring channel (ALM, PCM, COD)
    to another ALM, PCM or COD channel. It adds an additional participant to
    an existing call for monitoring purposes. This feature can be used for
    testing. The newly added party does not add its voice path to the call,
    instead the party can only listen to the voice path of all other parties.
    This command applies to ALM, PCM and COD modules.

   \param IFX_TAPI_MAP_MON_CFG_t* Pointer to an \ref IFX_TAPI_MAP_MON_CFG_t structure

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_MAP_MON_ADD                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_MON_ADD_IDX, IFX_TAPI_MAP_MON_CFG_t)

/** This interface disconnects a monitoring channel (ALM, PCM, COD)
    from another ALM, PCM or COD channel. It removes the additional participant
    from an existing call, which was added by \ref IFX_TAPI_MAP_MON_ADD.
    This command applies to ALM, PCM and COD modules.

   \param IFX_TAPI_MAP_MON_CFG_t* Pointer to an \ref IFX_TAPI_MAP_MON_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_MAP_MON_REMOVE             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MAP_MON_REMOVE_IDX, IFX_TAPI_MAP_MON_CFG_t)


/** This service is used to configure encoding type and length.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_CFG_t* Pointer to an
          \ref IFX_TAPI_ENC_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_ENC_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_CFG_SET_IDX, IFX_TAPI_ENC_CFG_t)

/** This service is used to configure the decoder. Currently only bit-packing
    according to ITU-T I366.2 bit alignment for G.726 codecs is configurable.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DEC_CFG_t* Pointer to an
          \ref IFX_TAPI_DEC_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_DEC_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_CFG_SET_IDX, IFX_TAPI_DEC_CFG_t)

/** Starts encoding and packetization.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_MODE_t* Pointer to an \ref IFX_TAPI_ENC_MODE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks The voice is serviced with the driver's read and write interface.
   The data format is dependent on the selected setup (coder, chip setup,
    etc.). For example, an RTP setup will receive RTP packets. Read and
    write are always non-blocking. If the codec has not been set before with
    \ref IFX_TAPI_ENC_CFG_SET, the encoder is started with G711.

   \code
   IFX_TAPI_ENC_MODE_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_ENC_START, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_ENC_START               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_START_IDX, IFX_TAPI_ENC_MODE_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_ENC_START               _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_START_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Stops encoding and packetization on this channel.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_MODE_t* Pointer to an \ref IFX_TAPI_ENC_MODE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_ENC_MODE_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_ENC_STOP, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;
   return IFX_SUCCESS;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_ENC_STOP                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_STOP_IDX, IFX_TAPI_ENC_MODE_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_ENC_STOP                _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This ioctl can be used to temporarily stop and restart packet encoding; no
    packets are generated in the upstream direction and all related packet counters
    are frozen (statistics are maintained).
    This ioctl can be used, for example, to hold/unhold the remote VoIP party.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_HOLD_t* Pointer to an \ref IFX_TAPI_ENC_HOLD_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_ENC_HOLD                   _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_HOLD_IDX, IFX_TAPI_ENC_HOLD_t)

/** Selects a codec for the data channel playout.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DEC_TYPE_t* The parameter specifies the codec;
   default is G711.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks This interface is currently not supported, because the codec
   is determined by the payload type of the received packets. */
#define  IFX_TAPI_DEC_TYPE_SET               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_TYPE_SET_IDX, IFX_TAPI_DEC_TYPE_t)

/** Starts the decoding of data.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DEC_t* Pointer to an \ref IFX_TAPI_DEC_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_DEC_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DEC_START, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
  */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_DEC_START               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_START_IDX, IFX_TAPI_DEC_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_DEC_START               _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_START_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Stops the playout of data.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_DEC_t* Pointer to an \ref IFX_TAPI_DEC_t structure.

\remarks Stopping the decoding will lead to a reset of the connection
    statistics.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_DEC_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DEC_STOP, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_DEC_STOP                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_STOP_IDX, IFX_TAPI_DEC_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_DEC_STOP                _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEC_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service switches on/off the high-pass (HP) filters of the decoder path
    in the COD module.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_COD_DEC_HP_t* Pointer to an \ref IFX_TAPI_COD_DEC_HP_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_COD_DEC_HP_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_DEC_HP_SET_IDX, IFX_TAPI_COD_DEC_HP_t)

/** Sets the volume settings of the COD module, both for the receiving
    (downstream) and transmitting (upstream) paths.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_PKT_VOLUME_t* This interface expects a pointer to an
   \ref IFX_TAPI_PKT_VOLUME_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PKT_VOLUME_t param;

   memset (&param, 0, sizeof (param));

   param.nEnc = 6;    // dB
   param.nDec = 9;    // dB

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_COD_VOLUME_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_COD_VOLUME_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_VOLUME_SET_IDX, IFX_TAPI_PKT_VOLUME_t)

/** Configures AGC coefficients for a coder module. This implementation assumes
   that an index of an AGC resource is fixedly assigned to the related index of
   the coder module.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_AGC_CFG_t* Pointer to an \ref IFX_TAPI_ENC_AGC_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

    \code
   IFX_TAPI_ENC_AGC_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.com  = -30;
   param.gain = +10;
   param.att  = -20;
   param.lim  = -25;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_ENC_AGC_CFG, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_ENC_AGC_CFG                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_AGC_CFG_IDX, IFX_TAPI_ENC_AGC_CFG_t)

/** Enables/disables the AGC.
    The AGC may be enabled only while the encoder is active. The AGC must be
    disabled before deactivating the encoder.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_AGC_ENABLE_t* Pointer to an \ref IFX_TAPI_ENC_AGC_ENABLE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_ENC_AGC_ENABLE             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_AGC_ENABLE_IDX, IFX_TAPI_ENC_AGC_ENABLE_t)

/** Configures voice activity detection and silence handling. Voice Activity
    Detection (VAD) is a feature that allows the codec to determine whether to
    send voice data or silence data.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_VAD_CFG_t* Pointer to an \ref IFX_TAPI_ENC_VAD_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   Codecs G.723.1 and G.729A/B have built-in Comfort Noise Generation (CNG).
   Explicitly switching on the CNG is not necessary. Voice activity detection
   may not be available for G.728 and G.729E.

*/
#define  IFX_TAPI_ENC_VAD_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_VAD_CFG_SET_IDX, IFX_TAPI_ENC_VAD_CFG_t)

/** This command downloads an announcement file to the firmware memory.
    The file contains the RTP voice samples encoded with a codec that is
    supported by the firmware. The command \ref IFX_TAPI_CAP_LIST provides a
    list of the supported codecs.
    The codec used in the announcement file is given by the parameter
    'nCodType'.
    The announcement playback on a coder module is started by calling
    \ref IFX_TAPI_COD_ANNOUNCE_START. To stop the playback, call
    \ref IFX_TAPI_COD_ANNOUNCE_STOP.
    The command applies to COD modules.

   \param IFX_TAPI_COD_ANNOUNCE_CFG_t* Pointer to an \ref IFX_TAPI_COD_ANNOUNCE_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   The TAPI returns with an error in case the chosen codec type is not supported, or if
   the file size or 'nAnnIdx' are outside the supported range.
*/
#define  IFX_TAPI_COD_ANNOUNCE_CFG_SET       _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_ANNOUNCE_CFG_SET_IDX, IFX_TAPI_COD_ANNOUNCE_CFG_t)

/** This command starts the announcement file playback on a coder module. The
    announcement file to playback has to be downloaded to the firmware using
    \ref IFX_TAPI_COD_ANNOUNCE_CFG_SET.
    Call \ref IFX_TAPI_COD_ANNOUNCE_STOP to stop the playback.
    The command applies to COD modules.

   \param IFX_TAPI_COD_ANNOUNCE_START_t* Pointer to an \ref IFX_TAPI_COD_ANNOUNCE_START_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_COD_ANNOUNCE_START         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_ANNOUNCE_START_IDX, IFX_TAPI_COD_ANNOUNCE_START_t)

/** This command tells the TAPI to stop an announcement. The
    playback was previously started by the TAPI command
    \ref IFX_TAPI_COD_ANNOUNCE_START.
    The command applies to COD modules.

   \param IFX_TAPI_COD_ANNOUNCE_STOP_t* Pointer to an
   \ref IFX_TAPI_COD_ANNOUNCE_STOP_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks
   This command returns with an error in case no announcement playback was
   previously started by \ref IFX_TAPI_COD_ANNOUNCE_START, or if playback
   was already stopped by \ref IFX_TAPI_COD_ANNOUNCE_STOP.
*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_COD_ANNOUNCE_STOP       _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_ANNOUNCE_STOP_IDX, IFX_TAPI_COD_ANNOUNCE_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_COD_ANNOUNCE_STOP       _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_ANNOUNCE_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/**
   This command tells the TAPI to release the buffer which was allocated for
   an announcement. If the announcement with this ID is currently being played back
   by the firmware, the TAPI stops the announcement and wait for the
   "announcement end" event from the firmware. The TAPI will actually free the buffer
   and mark the announcement ID as being free only after reception of this event
   (for this buffer handling, the TAPI will have to implement a simple
   state machine).

   \param Pointer to an
   \ref IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE   _IOW (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE_IDX,  IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE_t)

#ifndef TAPI4_DXY_DOC
/** Configuration and start of room noise detection.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_ENC_ROOM_NOISE_DETECT_t* Pointer to an \ref IFX_TAPI_ENC_ROOM_NOISE_DETECT_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_ENC_ROOM_NOISE_DETECT_START _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_ROOM_NOISE_DETECT_START_IDX, IFX_TAPI_ENC_ROOM_NOISE_DETECT_t)

/** This function stops room noise detection.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param Pointer to an \ref IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP_IDX, IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP_t)
#else/* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI4_DXY_DOC
/** Configure the mapping of packet streams to KPI channels or the application.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module, a DECT channel module and an LIN resource.

   \param IFX_TAPI_KPI_CH_CFG_t* Pointer to an \ref IFX_TAPI_KPI_CH_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_KPI_CH_CFG_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_KPI_CH_CFG_SET_IDX, IFX_TAPI_KPI_CH_CFG_t)

/** Configure a group of KPIs.

    This function configures the given KPI group. With this, the size of the
    FIFOs in the group can be set to individual values.
    This command can be applied on any device or channel file descriptor.
    The command is global to the TAPI and does not belong to any device or channel,
    but in order to save a global file descriptor it can be done on any of
    the existing file descriptors.

    \remarks
    This function locks the interrupts for a significantly long time and so may
    have a significant effect on system performance. It is recommended that
    this function is used only then data transfer on the KPI group has stopped, for
    example, during startup when nothing else is done in the system.

   \param IFX_TAPI_KPI_GRP_CFG_t* Pointer to an
          \ref IFX_TAPI_KPI_GRP_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_KPI_GRP_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_KPI_GRP_CFG_SET_IDX, IFX_TAPI_KPI_GRP_CFG_t)
#endif /* #ifndef TAPI4_DXY_DOC */

/** Configures the MOS-LQE calculation.
   The coder channel supports continuous quality measurements on the received
   voice samples (RTP stream from the network side).
   These measurement result in an MOS-LQE calculation, according to the
   computational model (E-model) of the ITU-T G.107 standard.
   This command configures the parameters required for the MOS-LQE calculation.
   The current configuration and the latest calculated MOS-LQE value can be
   read out using \ref IFX_TAPI_COD_MOS_RESULT_GET.

   To successfully set values with this command, the encoder and decoder in the
   addressed channel must be stopped.

   \remarks
   The MOS calculation is supported for narrowband and wideband voice samples.

   \param IFX_TAPI_COD_MOS_t*  Pointer to an \ref IFX_TAPI_COD_MOS_t structure.
      The 'mos' field in this struct is also a struct but of type
      \ref IFX_TAPI_COD_MOS_VALUE_t. The 'nMos' field in this struct configures
      a threshold MOS value. The TAPI generates an event \ref IFX_TAPI_EVENT_COD_MOS
      in case the calculated MOS-LQE value falls below the threshold.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error. An error occurs in case the provided
      values are out of range, or if the used firmware version does not support
      this MOS-LQE measurement and calculation feature.
*/
#define  IFX_TAPI_COD_MOS_CFG_SET            _IOW (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_MOS_CFG_SET_IDX, IFX_TAPI_COD_MOS_t)

/** Get the latest calculated MOS-LQE value.
   The MOS-LQE calculation is configured using \ref IFX_TAPI_COD_MOS_CFG_SET.
   This command reads out the latest calculated MOS-LQE value.

   To successfully read values with this command, the decoder in the addressed
   channel must be running.

   \remarks
   This commands return an error in case the MOS measurement returns an error
   if the corresponding coder channel is disabled.

   \param IFX_TAPI_COD_MOS_t*  Pointer to an \ref IFX_TAPI_COD_MOS_t structure.
      The 'mos' field in this struct is also a struct but of type
      \ref IFX_TAPI_COD_MOS_VALUE_t. The 'nMos' field in this struct contains
      the latest calculated MOS-LQE value. The other fields contain the values
      that were configured with \ref IFX_TAPI_COD_MOS_CFG_SET.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error. An error occurs in case the decoder on
      the addressed channel is not running, or if the used firmware version does not
      support this MOS-LQE measurement and calculation feature. In case of an
      error the content of the struct provided as a parameter is undefined.
*/
#define  IFX_TAPI_COD_MOS_RESULT_GET         _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COD_MOS_RESULT_GET_IDX, IFX_TAPI_COD_MOS_t)

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, ioctl commands (Group TAPI_INTERFACE_MISC)  */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Retrieves the TAPI version string.
    This command applies to the device file descriptor.

   \param IFX_char_t* Pointer to version character string.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_char_t Version[IFX_TAPI_VERSION_LEN];

   if (ioctl (fd, IFX_TAPI_VERSION_GET, (unsigned long) &Version[0]) != IFX_SUCCESS)
      return IFX_ERROR;

   printf("Version:%s\n" , Version);
   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_VERSION_GET                _IOR  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_VERSION_GET_IDX, IFX_char_t[IFX_TAPI_VERSION_LEN])

/** Sets the report levels if the driver is compiled with ENABLE_TRACE.
    It is applicable to device file descriptors. The trace
    level can be set to 'low', 'medium' or 'high'. All traces with a lower
    trace level than enabled are not reported. The trace information can vary
    between different driver implementations and driver versions. In general,
    traces of type 'high' indicate problems that could result in a system
    failure and system crash. These problems can be solved during the
    integration phase.
    This command applies to the device file descriptor.

   \param IFX_TAPI_DEBUG_REPORT_t* Pointer to an \ref IFX_TAPI_DEBUG_REPORT_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_DEBUG_REPORT_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DEBUG_REPORT_SET_IDX, IFX_TAPI_DEBUG_REPORT_t)

/** Checks the supported TAPI interface version.
    This command applies to the device file descriptor.

   \param IFX_TAPI_VERSION_t Pointer to an
   \ref IFX_TAPI_VERSION_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   Since an application is always built for one specific TAPI interface
   version, it should check whether this is supported. If not, the application should
   abort. This interface checks if the current TAPI version supports a
   particular version. For example, TAPI version 2.1 will support TAPI 2.0,
   but version 3.0 might not support 2.0.

   \code
   IFX_TAPI_VERSION_t param;

   memset (&param, 0, sizeof (param));

   param.majorNumber = 2;
   param.minorNumber = 1;

   if (ioctl (fd, IFX_TAPI_VERSION_CHECK, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

      printf( "Version 2.1 supported\n");
   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_VERSION_CHECK              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_VERSION_CHECK_IDX, IFX_TAPI_VERSION_t)

/**
   Retrieves the firmware version string of the currently downloaded firmware
   version.
   This command returns with an error in case no firmware is downloaded. The
   returned string format might vary between the device platforms used. This
   means that the version information cannot be compared between different
   device platforms. All returned version information is in the form of zero-terminated
   character strings. This command applies to the whole device.

   \param    pVersionDevEntry Handle to an IFX_TAPI_VERSION_DEV_ENTRY_t data
   type.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_VERSION_DEV_ENTRY_t param;

   memset (&param, 0, sizeof (param));
   // Second device
   param.dev = 1;
   while (1)
   {
      if (ioctl (fd, IFX_TAPI_VERSION_DEV_ENTRY_GET, (unsigned long) &param)
         != IFX_SUCCESS)
      {
         printf( "ERROR: TAPI version request failed!\n");
         return IFX_ERROR;
      }
      if ((strlen(param.versionEntry.name) == 0) ||
         (strlen(param.versionEntry.version) == 0))
         // No more version entries found
         break;
      printf(" %s version: %s ", param.versionEntry.name,
         param.versionEntry.version);
      param.nId++;
   }
   return IFX_SUCCESS;
   \endcode
*/
#define IFX_TAPI_VERSION_DEV_ENTRY_GET       _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_VERSION_DEV_ENTRY_GET_IDX, IFX_TAPI_VERSION_DEV_ENTRY_t)

/**
   Retrieves the version string of the current version.
   This command returns with an error in case no firmware is downloaded. The
   returned string format might vary between the device platforms used. This
   means that the version information cannot be compared between different
   device platforms. All returned version information is in the form of zero-terminated
   character strings. This command applies to ALM modules.

   \param    pVersionChEntry Handle to an IFX_TAPI_VERSION_CH_ENTRY_t data
   type.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_VERSION_CH_ENTRY_t param;

   memset (&param, 0, sizeof (param));

   // First channel on the second device
   param.ch = 0;
   param.dev = 1;
   while (1)
   {
      if (ioctl (fd, IFX_TAPI_VERSION_CH_ENTRY_GET, (unsigned long) &param)
         != IFX_SUCCESS)
      {
         printf( "ERROR: TAPI version request failed!\n");
         return IFX_ERROR;
      }
      if ((strlen(param.versionEntry.name) == 0) ||
      (strlen(param.versionEntry.version) == 0))
         // No more version entries found
         break;
      printf(" %s version: %s ",
      param.versionEntry.name, param.versionEntry.version);
      param.nId++;
   }
   return IFX_SUCCESS;
   \endcode
*/
#define IFX_TAPI_VERSION_CH_ENTRY_GET        _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_VERSION_CH_ENTRY_GET_IDX, IFX_TAPI_VERSION_CH_ENTRY_t)

/** This service returns the number of capabilities. The file descriptor is
    applicable to device file descriptors. These capabilities are of the
    same type as listed by \ref IFX_TAPI_CAP_LIST. They include
    supported features such as coder modules, analog line modules, PCM modules,
    tone generator, and tone detectors.
    This command applies to the device file descriptor.

   \param IFX_TAPI_CAP_NR_t* Pointer to an \ref IFX_TAPI_CAP_NR_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_CAP_NR                     _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CAP_NR_IDX, IFX_TAPI_CAP_NR_t)

#if defined (USE_OLD_TAPI_CAP_LIST)
   /** This service returns the capability lists. The file descriptor is
       applicable to device file descriptors. The capability list contains
       the number of supported features such as coder modules, analog line modules,
       PCM modules, tone generator, and tone detectors.
       This command applies to the device file descriptor.

      \param int Pointer to an \ref IFX_TAPI_CAP_t structure array.
         The number of array entries can be retrieved by \ref IFX_TAPI_CAP_NR.

      \return Returns the following values:
         - IFX_SUCCESS: if successful
         - IFX_ERROR: in case of an error

      \note: Deprecated style

      \code
      IFX_TAPI_CAP_t *pList;
      IFX_int32_t i, nCap;

      // retrieve nCap value via IFX_TAPI_CAP_NR

      pList = malloc (nCap * sizeof (IFX_TAPI_CAP_t));

      // Get the cap list
      if (ioctl (fd, IFX_TAPI_CAP_LIST, (unsigned long) pList) == IFX_SUCCESS)
      {
         for (i = 0; i < nCap; i++)
         {
            IFX_TAPI_CAP_t *pCap = &pList[i];

            switch (pCap->captype)
            {
                 case IFX_TAPI_CAP_TYPE_CODEC:
               printf ("Codec: %s\n\r", pCap->desc);
                     break;
                 case IFX_TAPI_CAP_TYPE_PCM:
               printf ("PCM: %d\n\r", pCap->cap);
                    break;
                 default:
                     break;
            }
         }
      }

      // Free the allocated memory
      free(pList);

      return IFX_SUCCESS;
      \endcode   */
   #define  IFX_TAPI_CAP_LIST                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CAP_LIST_IDX, IFX_TAPI_CAP_t*)
#else /* defined (USE_OLD_TAPI_CAP_LIST) */
   /** This service returns the capability lists. The file descriptor is
       applicable to device file descriptors. The capability list contains
       the number of supported features such as coder modules, analog line modules,
       PCM modules, tone generator, and tone detectors.
       This command applies to the device file descriptor.

      \param int Pointer to an \ref IFX_TAPI_CAP_t structure array.
         The number of array entries can be retrieved by \ref IFX_TAPI_CAP_NR.

      \return Returns the following values:
         - IFX_SUCCESS: if successful
         - IFX_ERROR: in case of an error

      \code
      IFX_TAPI_CAP_LIST_t param;
      IFX_int32_t i, nCap;

      memset (&param, 0, sizeof (param));

      // retrieve nCap value via IFX_TAPI_CAP_NR

      param.nCap = nCap;
      param.pList = malloc (nCap * sizeof (IFX_TAPI_CAP_t));

      // Second device
      param.dev = 1;
      // Get the cap list
      if (ioctl (fd, IFX_TAPI_CAP_LIST, (unsigned long) &param) == IFX_SUCCESS)
      {
         for (i = 0; i < nCap; i++)
         {
            IFX_TAPI_CAP_t *pCap = &param.pList[i];

            switch (pCap->captype)
            {
               case IFX_TAPI_CAP_TYPE_CODEC:
                  printf ("Codec: %s\n\r", pCap->desc);
               break;
               case IFX_TAPI_CAP_TYPE_PCM:
                  printf ("PCM: %d\n\r", pCap->cap);
                  break;
               default:
               break;
            }
         }
      }

      // Free the allocated memory
      free(param.pList);

      return IFX_SUCCESS;
      \endcode   */
   #define  IFX_TAPI_CAP_LIST                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CAP_LIST_IDX, IFX_TAPI_CAP_LIST_t)
#endif /* defined (USE_OLD_TAPI_CAP_LIST) */

/** This service checks whether a specific capability is supported.
   This command applies to the device file descriptor.

   \param IFX_TAPI_CAP_t* Pointer to an
   \ref IFX_TAPI_CAP_t structure.

   \return Returns the following values:
      - 0: Capability not supported
      - 1:  Capability supported
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_CAP_t param;

   memset (&param, 0, sizeof (param));

   // Second device
   param.dev = 1;

   // Check if G726, 16 kbit/s is supported
   param.captype = IFX_TAPI_CAP_TYPE_CODEC;
   param.cap = IFX_TAPI_COD_TYPE_G726_16;

   if (ioctl (fd, IFX_TAPI_CAP_CHECK, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

      printf( "G726_16 supported\n" );

   // Check how many data channels are supported
   param.captype = IFX_TAPI_CAP_TYPE_CODECS;

   if (ioctl (fd, IFX_TAPI_CAP_CHECK, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   printf( "%d data channels supported\n", param.cap);

   // Check if POTS port is available
   param.captype = IFX_TAPI_CAP_TYPE_PORT;
   param.cap = IFX_TAPI_CAP_PORT_POTS;

   if (ioctl (fd, IFX_TAPI_CAP_CHECK, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

      printf("POTS port supported\n");

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_CAP_CHECK                  _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CAP_CHECK_IDX, IFX_TAPI_CAP_t)

/** This service returns the last error code occurring in the
    TAPI driver or the low-level driver. It also contains an error
    stack for tracking down the origin of the error source.
    This command applies to the device file descriptor.
    After calling this service the stack is reset.

   \param IFX_TAPI_Error_t* Pointer to an
   \ref IFX_TAPI_Error_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_VERSION_CH_ENTRY_t param;
   int ret, i;

   // call some ioctl
   // ret = ioctl (fd, <some ioctl> , (unsigned long)&param);
   if (ret != IFX_SUCCESS)
   {
      IFX_TAPI_Error_t error;

      memset (&error, 0, sizeof (error));

      // use the correct device and channel
      error.nDev = param.dev;
      error.nCh = param.ch;

      ioctl (fd, IFX_TAPI_LASTERR, (unsigned long)&error);
      if (error.nCode != -1)
      {
         // we have additional information
         printf ("Error Code 0x%X occurred\n", error.nCode);
         for (i = 0; i < TAPI_ERRNO_CNT; ++i)
         {
            if (TAPI_drvErrnos[i] == (error.nCode & 0xffff))
            {
               printf ("%s\n", TAPI_drvErrStrings[i]);
            }
         }
         for (i = 0; i < error.nCnt; ++i)
         {
            printf ("%s:%d Code 0x%4X%4X\n", error.stack[i].sFile,
            error.stack[i].nLine,
            error.stack[i].nHlCode,
            error.stack[i].nLlCode);
         }
      }
   }
   \endcode
   */
#define  IFX_TAPI_LASTERR                    _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LASTERR_IDX, IFX_TAPI_Error_t)

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ========================================================================== */
/* TAPI Power Ringing Services, ioctl commands (Group TAPI_INTERFACE_RINGING) */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** This service sets the ring configuration for the non-blocking
    power ringing services.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_CFG_t* Pointer to an \ref IFX_TAPI_RING_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks The configuration has to be set before ringing starts, using the
    interface \ref IFX_TAPI_RING_START .

   \code
     IFX_TAPI_RING_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nMode = 0;
   param.nSubmode = 1;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_RING_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode
*/
#define  IFX_TAPI_RING_CFG_SET               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_CFG_SET_IDX, IFX_TAPI_RING_CFG_t)

/** This service sets the high resolution ring cadence for the power ringing
   services. The cadence value has to be set before ringing is started with
   \ref IFX_TAPI_RING_START or \ref IFX_TAPI_RING.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_CADENCE_t* Pointer to an \ref IFX_TAPI_RING_CADENCE_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   The number of ring bursts can be obtained by counting the events
    \ref IFX_TAPI_EVENT_FXS_RING.
   The initial cadence pattern can be of zero length, in which case the initial
   pattern will not be played. If a length for the initial pattern is given,
   the initial pattern may not consist of all-zero bits, but of all bits set to one.
   The periodic pattern must contain at least one bit set to one. This implies
   that the length of the periodic pattern must be at least one. The setting of all
   bits for the defined length to one is allowed, and is a way of starting an infinite
   ringing.

   \code
   IFX_TAPI_RING_CADENCE_t ringCadence;
   IFX_TAPI_RING_t ring;
   IFX_char_t data[15] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00 };

   memset (&ringCadence, 0, sizeof (ringCadence));

   // Program the cadence
   memcpy(&ringCadence.data, data, sizeof (data));
   ringCadence.nr = (int)(sizeof (data) * 8);

   // Ringing should be done on the second analog line of the third device
   ringCadence.dev = 2;
   ringCadence.ch = 1;
   if (ioctl (fd, IFX_TAPI_RING_CADENCE_HR_SET, (unsigned long) &ringCadence)
      != IFX_SUCCESS)
   return IFX_ERROR;

   \endcode
*/
#define  IFX_TAPI_RING_CADENCE_HR_SET        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_CADENCE_HR_SET_IDX, IFX_TAPI_RING_CADENCE_t)

/** This service gets the ring configuration for the non-blocking
    power ringing services.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_CFG_t* Pointer to an \ref IFX_TAPI_RING_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_RING_CFG_GET               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_CFG_GET_IDX, IFX_TAPI_RING_CFG_t)


/** This service sets the maximum number of cadences after which ringing stops
    automatically.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param int The parameter defines the number of cadences to be played.
              A value of 0 means infinity.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_RING_MAX_SET               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_MAX_SET_IDX, IFX_TAPI_RING_MAX_t)

/** Important: The use of this interface is deprecated. Use
   \ref IFX_TAPI_RING_CADENCE_HR_SET for new developments.
   This service sets the ring cadence for the non-blocking power ringing
    services. The cadence value has to be set before ringing starts
   (\ref IFX_TAPI_RING_START). This service is non-blocking.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_CADENCE_CFG_t* Pointer to an \ref IFX_TAPI_RING_CADENCE_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks
   The number of ring bursts can be obtained by counting the events
   \ref IFX_TAPI_EVENT_FXS_RING.

*/
#define  IFX_TAPI_RING_CADENCE_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_CADENCE_SET_IDX, IFX_TAPI_RING_CADENCE_CFG_t)

/** This service starts the non-blocking ringing on the phone line using the
    pre-configured ring cadence.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_t* Pointer to an \ref IFX_TAPI_RING_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks
   This interface does not not provide caller ID services.
   If ringing with caller ID is desired, \ref IFX_TAPI_CID_TX_SEQ_START should be
   used! The ringing must be stopped with \ref IFX_TAPI_RING_STOP. A second call
   to \ref IFX_TAPI_RING_START while the phone is ringing returns an error.
   The ringing can be configured with the interfaces \ref IFX_TAPI_RING_CFG_SET,
   \ref IFX_TAPI_RING_CADENCE_SET and \ref IFX_TAPI_RING_CADENCE_HR_SET, before
   this interface is called.

*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_RING_START              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_START_IDX, IFX_TAPI_RING_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_RING_START              _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_START_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service rings the phone. The service is blocking and will not return
    until the phone is off-hook or the maximum number of ring cadences as
    previously set by \ref IFX_TAPI_RING_MAX_SET have been played.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param int This interface expects no parameters. It should be set to 0.

   \return The execution status can be:
      - 0: number of rings has been reached
      - 1: phone was unhooked
      - -1: in case of an error

   \code
   int nMaxRing = 3, ret;
   // set the maximum rings
   ioctl(fd, IFX_TAPI_RING_MAX_SET, nMaxRing);
   // ring the phone
   ret = ioctl(fd, IFX_TAPI_RING, 0);
   if (ret == 0)
   {
      // no answer, maximum number of rings reached
    }
    else if (ret == 1)
    {
      // phone unhooked
    }
   \endcode   */
#define  IFX_TAPI_RING                       _IO (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_IDX)

/** This service stops non-blocking ringing on the phone line that was previously started
   with the \ref IFX_TAPI_RING_START or \ref IFX_TAPI_CID_TX_SEQ_START services.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_RING_t* Pointer to an \ref IFX_TAPI_RING_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

*/
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_RING_STOP               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_STOP_IDX, IFX_TAPI_RING_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_RING_STOP               _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_RING_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ==================================================================== */
/* TAPI PCM Services, ioctl commands (Group TAPI_INTERFACE_PCM)         */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** This service sets the configuration of the PCM interface; it must be
    called after \ref IFX_TAPI_DEV_START but before activating a PCM channel.
    After activating a PCM channel, the PCM interface settings cannot be
    modified anymore.
    If a PCM channel is activated without calling the PCM interface ioctl,
    the default settings are configured automatically (PCM slave).
    This command applies to any channel file descriptor
    that includes a PCM module resource.

    \param IFX_TAPI_PCM_IF_CFG_t* Pointer to an \ref IFX_TAPI_PCM_IF_CFG_t structure
    \code
   IFX_TAPI_PCM_IF_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nOpMode       = IFX_TAPI_PCM_IF_MODE_SLAVE;
   param.nDCLFreq      = IFX_TAPI_PCM_IF_DCLFREQ_2048;
   param.nDoubleClk    = IFX_DISABLE;
   param.nSlopeTX      = IFX_TAPI_PCM_IF_SLOPE_RISE;
   param.nSlopeRX      = IFX_TAPI_PCM_IF_SLOPE_FALL;
   param.nOffsetTX     = IFX_TAPI_PCM_IF_OFFSET_NONE;
   param.nOffsetRX     = IFX_TAPI_PCM_IF_OFFSET_NONE;
   param.nDrive        = IFX_TAPI_PCM_IF_DRIVE_ENTIRE;
   param.nShift        = IFX_DISABLE;
   param.nMCTS         = 0x00;

   // Configure the second PCM highway of the device
   param.nHighway = 0;

   // Second device
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_IF_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
    \endcode
*/
#define  IFX_TAPI_PCM_IF_CFG_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_IF_CFG_SET_IDX, IFX_TAPI_PCM_IF_CFG_t)

/** This service sets the configuration of a PCM channel.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_PCM_CFG_t* Pointer to an \ref IFX_TAPI_PCM_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks The parameter rate must be set to the PCM rate that is applied to the
   device, otherwise an error is returned.

   \code
   IFX_TAPI_PCM_CFG_t param;

   memset (&param, 0, sizeof (param));

   param.nTimeslotRX = 5;
   param.nTimeslotTX = 5;
   param.nHighway = 1;

   // 16-bit resolution
   param.nResolution = IFX_TAPI_PCM_RES_LINEAR_16BIT;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PCM_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_CFG_SET_IDX, IFX_TAPI_PCM_CFG_t)

/** This service gets the configuration of the PCM channel.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_PCM_CFG_t* Pointer to an \ref IFX_TAPI_PCM_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PCM_CFG_t param;

   memset (&param, 0, sizeof (param));

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_CFG_GET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode   */
#define  IFX_TAPI_PCM_CFG_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_CFG_GET_IDX, IFX_TAPI_PCM_CFG_t)

/** This service activates/deactivates the PCM time slots configured for this
    channel.
    This command applies to any channel file descriptor that includes a
    PCM module resource.

   \param IFX_TAPI_PCM_ACTIVATION_t* Pointer to an \ref IFX_TAPI_PCM_ACTIVATION_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PCM_ACTIVATION_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_ACTIVATION_SET_IDX, IFX_TAPI_PCM_ACTIVATION_t)

/** This service receives the activation status from the PCM time slots configured
   for this channel.
   This command applies to any channel file descriptor that
   includes a PCM module resource.

   \param IFX_TAPI_PCM_ACTIVATION_t* Pointer to an \ref IFX_TAPI_PCM_ACTIVATION_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PCM_ACTIVATION_GET         _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_ACTIVATION_GET_IDX, IFX_TAPI_PCM_ACTIVATION_t)

/**
   This service activates/deactivates and sets the configuration
   of a PCM HDLC (D) channel. This command applies to any channel file
   descriptor that includes a PCM module resource.

   \param IFX_TAPI_PCM_HDLC_CFG_t* Pointer to an \ref IFX_TAPI_PCM_HDLC_CFG_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PCM_HDLC_CFG_t param;

   memset (&param, 0, sizeof (param));

   // configure PCM D-channel
   param.nTimeslot   = 5;
   param.nEnable     = IFX_ENABLE;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_HDLC_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode
*/
#define  IFX_TAPI_PCM_HDLC_CFG_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_HDLC_CFG_SET_IDX, IFX_TAPI_PCM_HDLC_CFG_t)

/**
   This service activates/deactivates a PCM channel loop. This command applies
   to any channel file descriptor that includes a PCM module resource.

   \param IFX_TAPI_PCM_LOOP_CFG_t* Pointer to an \ref IFX_TAPI_PCM_LOOP_CFG_t structure.

   \remarks
   Two PCM channels are used: the PCM channel addressed by the fd and
   the one in the next channel fd. This means that if a shortcut is programmed
   for PCM channel 2, PCM channel 3 is also used for this shortcut.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_PCM_LOOP_CFG_t param;

   memset (&param, 0, sizeof (param));

   // configure PCM D-channel
   param.nTimeslot1  = 3;
   param.nTimeslot2  = 5;
   param.nEnable     = IFX_ENABLE;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_PCM_LOOP_CFG_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode
*/
#define  IFX_TAPI_PCM_LOOP_CFG_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_LOOP_CFG_SET_IDX, IFX_TAPI_PCM_LOOP_CFG_t)

/** This service switches on/off the HP filter of the decoder path in the PCM module.
    This command applies to any channel file descriptor
    that includes a PCM module resource.

   \param IFX_TAPI_PCM_DEC_HP_CFG_t* Pointer to an \ref IFX_TAPI_PCM_DEC_HP_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define  IFX_TAPI_PCM_DEC_HP_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PCM_DEC_HP_SET_IDX, IFX_TAPI_PCM_DEC_HP_CFG_t)

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================= */
/* TAPI Fax T.38 Stack, ioctl commands (Group TAPI_INTERFACE_FAX_STACK)    */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_FAX_STACK */
/*@{*/

/** This command starts a T.38 fax session on the given firmware coder module
    ('ch'/'dev').
   The fax channel implements FoIP (fax over IP) support, including both
   data pump and T.38 functionality. The fax channel is overlaid with the
   coder channel. This means that only a coder channel or a fax channel may
   be enabled with the same channel number.
   This command is used to activate a fax channel in order to start a T.38
   session with parameters that were negotiated by the signaling protocol.
   T.38 parameters may differ from session to session, depending on the
   results of the signaling protocol negotiation. The parameters can be
   passed together with the activation of the channel.
   The command \ref IFX_TAPI_T38_SESS_STOP is used to deactivate
   a fax channel, in order to stop the T.38 session. The fax channel should
   be deactivated when a local on-hook is detected, when an 'end of session'
   is reported from T.38 that indicates a normal session end, or when an error
   is reported from T.38.
   The command applies to COD modules.

   \param IFX_TAPI_T38_SESS_CFG_t* Pointer to an \ref IFX_TAPI_T38_SESS_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks The TAPI returns with an error in case the coder module
   (with the given 'ch' and 'dev' index) is currently enabled, the
   fax T.38 session on this module is already started, or a parameter is wrong.
*/
#define  IFX_TAPI_T38_SESS_START             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_SESS_START_IDX, IFX_TAPI_T38_SESS_CFG_t)

/** This command stops a currently running T.38 fax session on the given
   firmware coder module ('ch'/'dev').
   The command \ref IFX_TAPI_T38_SESS_START is used to activate
   a fax channel.
   The fax channel should be deactivated when a local on-hook is detected, when
   an 'end of session' is reported from T.38, indicating a normal
   session end, or when an error is reported from T.38.
   The command applies to COD modules.

   \param IFX_TAPI_T38_SESS_STOP_t* Pointer to an \ref IFX_TAPI_T38_SESS_STOP_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks TAPI returns with an error in case no fax T.38 session is running.
*/
#define  IFX_TAPI_T38_SESS_STOP              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_SESS_STOP_IDX, IFX_TAPI_T38_SESS_STOP_t)

/** The command is used to read T.38 session statistics. Statistical data is
    only valid while the channel is still active. After deactivation, all session-related
    values are reset to 0.
    The command applies to COD modules.

   \param IFX_TAPI_T38_SESS_STATISTICS_t* Pointer to an \ref IFX_TAPI_T38_SESS_STATISTICS_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks Statistical information is reset after session termination.
   It can only be read out while the fax session is still active.
*/
#define  IFX_TAPI_T38_SESS_STATISTICS_GET    _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_SESS_STATISTICS_GET_IDX, IFX_TAPI_T38_SESS_STATISTICS_t)

/** The command facilitates reading out of supported T.38
    implementation capabilities, which should be used by signaling
    protocols such as H.323, SIP/SDP or MDC/MGCP for the negotiation of T.38
    capabilities during call establishment. Capabilities for TCP and UDP
    may be obtained using this command. Note that only UDP is supported by
    the current implementation. The command may be issued at any time, but
    is intended to be called during system initialization, as T.38
    capabilities are fixed and correspond to implemented features.
    The command applies to COD modules.

   \param IFX_TAPI_T38_CAP_t* Pointer to an \ref IFX_TAPI_T38_CAP_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_CAP_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_CAP_GET_IDX, IFX_TAPI_T38_CAP_t)

/** This command is used to set the parameters of a fax channel. It
    may be applied only to an inactive fax channel.
    The configuration can be read out by \ref IFX_TAPI_T38_CFG_GET.
    The command applies to COD modules.

   \param IFX_TAPI_T38_FAX_CFG_t* Pointer to an \ref IFX_TAPI_T38_FAX_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_CFG_SET_IDX, IFX_TAPI_T38_FAX_CFG_t)

/** This command is used to read out the parameters of a fax channel.
    The configuration is set by \ref IFX_TAPI_T38_CFG_SET.
    The command applies to COD modules.

   \param IFX_TAPI_T38_FAX_CFG_t* Pointer to an \ref IFX_TAPI_T38_FAX_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_CFG_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_CFG_GET_IDX, IFX_TAPI_T38_FAX_CFG_t)

/**
   This command is used to configure fax data pump parameters that are
   related to the buffer interface between the data pump and T.38. The
   default values of these parameters are defined to achieve minimal delay
   and guarantee absence of internal buffer underrun or overflow.
   This command may be applied only to an inactive fax channel.
   The configuration can be read out by \ref IFX_TAPI_T38_FDP_CFG_GET.
   The command applies to COD modules.

   \param IFX_TAPI_T38_FDP_CFG_t* Pointer to an \ref IFX_TAPI_T38_FDP_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_FDP_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_FDP_CFG_SET_IDX, IFX_TAPI_T38_FDP_CFG_t)

/**
   This command is used to read out the current configuration of the
   fax data pump parameters.
   The configuration is set by \ref IFX_TAPI_T38_FDP_CFG_SET.
   The command applies to COD modules.

   \param IFX_TAPI_T38_FDP_CFG_t* Pointer to an \ref IFX_TAPI_T38_FDP_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_FDP_CFG_GET            _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_FDP_CFG_GET_IDX, IFX_TAPI_T38_FDP_CFG_t)

/**
   This command is used to enable/disable a trace of fax events. Fax trace
   data is transported in a proprietary format over UDP and with the IP protocol
   information TYPE set to FAXTR.
   The trace feature is disabled in case the debug mask is set to zero.
   The command applies to COD modules.

   \param IFX_TAPI_T38_TRACE_CFG_t* Pointer to an \ref IFX_TAPI_T38_TRACE_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_T38_TRACE_SET              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_TRACE_SET_IDX, IFX_TAPI_T38_TRACE_CFG_t)

/*@}*/ /* TAPI_INTERFACE_FAX_STACK */

/* ======================================================================= */
/* TAPI Fax T.38 Services, ioctl commands (Group TAPI_INTERFACE_FAX)       */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** This service configures and enables the modulator during a T.38 fax session.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_T38_MOD_DATA_t* Pointer to an \ref IFX_TAPI_T38_MOD_DATA_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks This service deactivates the voice data path and configures the
    channel for a fax session.

   \code
     IFX_TAPI_T38_MOD_DATA_t param;

     memset (&param, 0, sizeof (param));

   //Set V.21 standard
     param.nStandard = 0x01;

   //Configure the second coder channel on the first device
   param.dev = 0;
   param.ch = 1;
   if (ioctl (fd, IFX_TAPI_T38_MOD_START, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_T38_MOD_START              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_MOD_START_IDX, IFX_TAPI_T38_MOD_DATA_t)

/** This service configures and enables the demodulator for a T.38 fax session.
   This command applies to any channel file descriptor that
   includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_T38_DEMOD_DATA_t* Pointer to an \ref IFX_TAPI_T38_DEMOD_DATA_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks This service deactivates the voice path and configures the driver
    for a fax session.

   \code
     IFX_TAPI_T38_DEMOD_DATA_t param;

   memset (&param, 0, sizeof (param));

     // set V.21 standard as standard used for fax
     param.nStandard1 = 0x01;
     // set V.17/14400 as alternative standard
     param.nStandard2 = 0x09;

   //Configure the second coder channel on the first device
   param.dev = 0;
   param.ch = 1;
   if (ioctl (fd, IFX_TAPI_T38_DEMOD_START, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode */
#define  IFX_TAPI_T38_DEMOD_START            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_DEMOD_START_IDX, IFX_TAPI_T38_DEMOD_DATA_t)

/** This service disables the T.38 fax data pump and activates the voice
    path again.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_T38_DATAPUMP_STOP_t* Pointer to an \ref IFX_TAPI_T38_DATAPUMP_STOP_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_T38_DATAPUMP_STOP_t param;

   memset (&param, 0, sizeof (param));

   //Configure the second coder channel on the first device
   param.dev = 0;
   param.ch = 1;
   if (ioctl (fd, IFX_TAPI_T38_STOP, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#ifdef TAPI_ONE_DEVNODE
   #define  IFX_TAPI_T38_STOP                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_STOP_IDX, IFX_TAPI_T38_DATAPUMP_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define  IFX_TAPI_T38_STOP                _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service provides the T.38 fax status on query.
    This command applies to any channel file descriptor
    that includes a data channel (COD+SIG) module resource.

   \param IFX_TAPI_T38_STATUS_t* Pointer to an \ref IFX_TAPI_T38_STATUS_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks This interface must be used when a fax exception has occurred, in order to
   find out the status.

   \code
     IFX_TAPI_T38_STATUS_t param;

   memset (&param, 0, sizeof (param));

   //Configure the second coder channel on the first device
   param.dev = 0;
   param.ch = 1;
     // request status
   if (ioctl (fd, IFX_TAPI_T38_STATUS_GET, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode */
#define  IFX_TAPI_T38_STATUS_GET             _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_T38_STATUS_GET_IDX, IFX_TAPI_T38_STATUS_t)
/*@}*/ /* TAPI_INTERFACE_FAX */

/* ======================================================================= */
/* TAPI Test Services, ioctl commands (Group TAPI_INTERFACE_TEST)          */
/* ======================================================================= */

/** \addtogroup TAPI_INTERFACE_TEST */
/*@{*/
/** Forces generation of on-/off-hook.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_TEST_HOOKGEN_t* Pointer to an \ref IFX_TAPI_TEST_HOOKGEN_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \remarks After switching the hook state, the hook event gets to the hook
    state machine for validation. Depending on the timing of the interface call,
    hook flash and pulse dialing can be verified. The example
   shows the generation of a flash hook with a timing of 100 ms. The flash hook
   can be queried afterwards using the \ref IFX_TAPI_EVENT_GET ioctl.

*/
#define  IFX_TAPI_TEST_HOOKGEN               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TEST_HOOKGEN_IDX, IFX_TAPI_TEST_HOOKGEN_t)

/** Enables a local test loop in the analog part. The digital voice data is
   transparently looped back to the network without affecting downstream
   transmission. Local voice reception (upstream) is disabled,
   This means that voice applied to the local phone is not being processed,
   but data sent to the phone can still be heard.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_TEST_LOOP_t* Pointer to an \ref IFX_TAPI_TEST_LOOP_t structure.

   \return Returns the following values:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_TEST_LOOP_t param;

   memset (&param, 0, sizeof (param));
   param.bAnalog = 1;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_TEST_LOOP, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
   \endcode */
#define  IFX_TAPI_TEST_LOOP                  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TEST_LOOP_IDX, IFX_TAPI_TEST_LOOP_t)

/*@}*/ /* TAPI_INTERFACE_TEST */

/* ===================================================================== */
/* TAPI Event Services, ioctl commands (Group TAPI_INTERFACE_EVENT)      */
/* ===================================================================== */

/** \addtogroup TAPI_INTERFACE_EVENT */
/*@{*/

/** IFX_TAPI_EVENT_GET always returns IFX_SUCCESS as long as the channel
   parameter is not out of range, or if no event was available.
   If the parameter is out of range then IFX_ERROR is returned.
   As the event data is not valid in this case, the value
   \ref IFX_TAPI_EVENT_NONE is returned in the 'id' field of the event.
   This ioctl indicates that additional events are ready to be retrieved.
   The information is provided in the 'more' field of the returned
    \ref IFX_TAPI_EVENT_t structure.
    This command applies to the device file descriptor.

   \param IFX_TAPI_EVENT_t* Pointer to an \ref IFX_TAPI_EVENT_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_EVENT_GET                   _IOR  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_EVENT_GET_IDX, IFX_TAPI_EVENT_t)

/** Enable detection of a single event.
    This command applies to the device file descriptor.

   \param IFX_TAPI_EVENT_t* Pointer to an \ref IFX_TAPI_EVENT_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_EVENT_ENABLE                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_EVENT_ENABLE_IDX, IFX_TAPI_EVENT_t)

/** Disable detection of a single event.
    This command applies to the device file descriptor.

   \param IFX_TAPI_EVENT_t* Pointer to an \ref IFX_TAPI_EVENT_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_EVENT_DISABLE               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_EVENT_DISABLE_IDX, IFX_TAPI_EVENT_t)

/** Enable detection of multiple events. All events are enabled on
    a single target module on one specific device. The number of events
    stored in 'pEvent' for enabling has to be set in 'nCount' of
    \ref IFX_TAPI_EVENT_MULTI_t.
    This command applies to the device file descriptor.

   \param IFX_TAPI_EVENT_MULTI_t* Pointer to an \ref IFX_TAPI_EVENT_MULTI_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks Memory allocated for 'pEvent' should be equal
      to (sizeof (IFX_TAPI_EVENT_ENTRY_t) * nCont)
*/
#define IFX_TAPI_EVENT_MULTI_ENABLE          _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_EVENT_MULTI_ENABLE_IDX, IFX_TAPI_EVENT_MULTI_t)

/** Disable detection of multiple events. All events are disabled on
    a single target module on one specific device. The number of events
    stored in 'pEvent' for disabling has to be set in 'nCount' of
    \ref IFX_TAPI_EVENT_MULTI_t.
    This command applies to the device file descriptor.

   \param IFX_TAPI_EVENT_MULTI_t* Pointer to an \ref IFX_TAPI_EVENT_MULTI_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

   \remarks Memory allocated for 'pEvent' should be equal
      to (sizeof (IFX_TAPI_EVENT_ENTRY_t) * nCont)
*/
#define IFX_TAPI_EVENT_MULTI_DISABLE         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_EVENT_MULTI_DISABLE_IDX, IFX_TAPI_EVENT_MULTI_t)
/*@}*/ /* TAPI_INTERFACE_EVENT */

#ifndef TAPI4_DXY_DOC
/* ======================================================================== */
/* Polling Services, ioctl commands (Group TAPI_POLLING_SERVICE)            */
/* ======================================================================== */
/** \addtogroup TAPI_POLLING_SERVICE */
/*@{*/

/**   This service switches the driver between polling and interrupt
      mode and vice versa. This command applies to any channel file descriptor.

   \param    pDrvCtrl Handle to an IFX_TAPI_POLL_CONFIG_t data type.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \remarks
      This function will be called only once with any device file descriptor
      to configure all adequately registered LL devices into polling/interrupt
      mode according to the configuration provided.
      In case the driver is switched to packets polling mode (bPollPkts = IFX_TRUE)
      all packet-related interrupts are disabled.
      In case the driver is switched to events polling mode (bPollEvts = IFX_TRUE)
      all events-related interrupts are disabled.
      In case of switching both to packets and events polling mode
      then all device interrupts are disabled.
      In case the driver is switched back to interrupt mode, either for packets or
      events, the reverse applies.
      The packets polling/interrupt configuration is only applied to
      those devices that previously had been registered using the
      IFX_TAPI_POLL_PKTS_ADD ioctl, otherwise the default configuration of interrupt mode applies.
      Also, the events polling/interrupt configuration is applied only
      to devices that have previously been registered using the
      IFX_TAPI_POLL_EVTS_ADD ioctl, otherwise the default configuration will
      apply, which is interrupt mode.

   \code
   IFX_TAPI_CH_INIT_t param;

   memset (&param, 0, sizeof (param));

   param.nMode = 0;
   param.nCountry = 2;
   param.pProc = 0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_CH_INIT, (unsigned long)&param) != IFX_SUCCESS)
      return IFX_ERROR;
   return IFX_SUCCESS;
   \endcode
*/
#define IFX_TAPI_POLL_CONFIG                 _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_CONFIG_IDX, IFX_TAPI_POLL_CONFIG_t)

/** Used to register a TAPI device for polling.
    This command applies to any channel file descriptor.

    \param IFX_int32_t The parameter is not required.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_POLL_DEV_ADD             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_DEV_ADD_IDX, IFX_TAPI_POLL_DEV_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_POLL_DEV_ADD             _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_DEV_ADD_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Used to de-register a TAPI device for polling.
    This command applies to any channel file descriptor.

    \param IFX_int32_t The parameter is not required.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_POLL_DEV_REM             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_DEV_REM_IDX, IFX_TAPI_POLL_DEV_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_POLL_DEV_REM             _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_DEV_REM_IDX)
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_POLLING_SERVICE */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ======================================================================= */
/* TAPI FXO Services, ioctl commands (Group TAPI_INTERFACE_FXO  )          */
/* ======================================================================= */
#ifndef TAPI4_DXY_DOC
/** \addtogroup TAPI_INTERFACE_FXO */
/*@{*/

/** Configuration for DTMF dialing. Mainly used on FXO lines but can be also
    used on analog lines.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_DIAL_CFG_t* Pointer to an \ref IFX_TAPI_FXO_DIAL_CFG_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_DIAL_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_DIAL_CFG_SET_IDX, IFX_TAPI_FXO_DIAL_CFG_t)
/** Configuration of the FXO hook.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_FLASH_CFG_t* Pointer to an \ref IFX_TAPI_FXO_FLASH_CFG_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_FLASH_CFG_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_FLASH_CFG_SET_IDX, IFX_TAPI_FXO_FLASH_CFG_t)
/** Configuration of OSI timing.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_OSI_CFG_t* Pointer to an \ref IFX_TAPI_FXO_OSI_CFG_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_OSI_CFG_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_OSI_CFG_SET_IDX, IFX_TAPI_FXO_OSI_CFG_t)
/** Dials DTMF digits.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_DIAL_t* Pointer to an \ref IFX_TAPI_FXO_DIAL_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_DIAL_START              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_DIAL_START_IDX, IFX_TAPI_FXO_DIAL_t)

/** Stops dialing digits on the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_DIAL_t* Pointer to an \ref IFX_TAPI_FXO_DIAL_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_FXO_DIAL_STOP            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_DIAL_STOP_IDX, IFX_TAPI_FXO_DIAL_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_FXO_DIAL_STOP            _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_DIAL_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Issues on-/off-hook on the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_HOOK_t* Pointer to an \ref IFX_TAPI_FXO_HOOK_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_HOOK_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_HOOK_SET_IDX, IFX_TAPI_FXO_HOOK_CFG_t)

/** Issues flash-hook on the FXO interface.
   This command applies to any channel file descriptor that
   includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_FLASH_t* Pointer to an \ref IFX_TAPI_FXO_FLASH_t structure.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_FXO_FLASH_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_FLASH_SET_IDX, IFX_TAPI_FXO_FLASH_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_FXO_FLASH_SET            _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_FLASH_SET_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Receives battery status from the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_BAT_t* Pointer to an \ref IFX_TAPI_FXO_BAT_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_BAT_GET                 _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_BAT_GET_IDX, IFX_TAPI_FXO_BAT_t)

/** This service retrieves the current hook state on an FXO channel (set by the
 application itself).
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_HOOK_t* Pointer to an \ref IFX_TAPI_FXO_HOOK_t type

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_HOOK_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_HOOK_GET_IDX, IFX_TAPI_FXO_HOOK_CFG_t)

/** Retrieves APOH (another phone off-hook) status of the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_APOH_t* Pointer to an \ref IFX_TAPI_FXO_APOH_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define IFX_TAPI_FXO_APOH_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_APOH_GET_IDX, IFX_TAPI_FXO_APOH_t)

/** Receives ring status from the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_RING_STATUS_t* Pointer to an \ref IFX_TAPI_FXO_RING_STATUS_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error

*/
#define IFX_TAPI_FXO_RING_GET                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_RING_GET_IDX, IFX_TAPI_FXO_RING_STATUS_t)

/** Receives the polarity status from the FXO interface.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_FXO_POLARITY_t* Pointer to an \ref IFX_TAPI_FXO_POLARITY_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_FXO_POLARITY_GET            _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_POLARITY_GET_IDX, IFX_TAPI_FXO_POLARITY_t)

#ifndef TAPI_DXY_DOC
/** Used for TAPI POLL testing. */
#define IFX_TAPI_POLL_TEST                   _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_POLL_TEST_IDX)
#endif /* TAPI_DXY_DOC */

/** Maximum number of DTMF digits to be dialed with a single TAPI ioctl
    used in \ref IFX_TAPI_FXO_DIAL_START. */
#define IFX_TAPI_FXO_DIAL_DIGITS             30

/** This service sets the FXO line mode.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param Pointer to \ref IFX_TAPI_FXO_LINE_MODE_t type, indicating FXO line mode.

   \remarks
   Switching from FXS mode to FXO and back is done via \ref IFX_TAPI_LINE_TYPE_SET
   ioctl. The default FXO line mode after TAPI init is IFX_TAPI_FXO_LINE_MODE_ACTIVE.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_FXO_LINE_MODE_t param;

   memset (&param, 0, sizeof (param));

   // set FXO line mode to DISABLED
   param.mode = IFX_TAPI_FXO_LINE_MODE_DISABLED;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_FXO_LINE_MODE_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;

   // set FXO line mode to ACTIVE
   param.mode = IFX_TAPI_FXO_LINE_MODE_ACTIVE;

   if (ioctl (fd, IFX_TAPI_FXO_LINE_MODE_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode
*/
#define IFX_TAPI_FXO_LINE_MODE_SET           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_FXO_LINE_MODE_SET_IDX, IFX_TAPI_FXO_LINE_MODE_t)

/*@}*/ /* TAPI_INTERFACE_FXO */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI4_DXY_DOC
/* ======================================================================= */
/* TAPI DECT Services, ioctl commands (Group TAPI_INTERFACE_DECT)          */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_DECT */
/*@{*/
/** This service activates/deactivates the DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param Pointer to \ref IFX_TAPI_DECT_ACTIVATION_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_DECT_ACTIVATION_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_ACTIVATION_SET_IDX, IFX_TAPI_DECT_ACTIVATION_t)

/** This service configures the DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_DECT_CFG_t* Pointer to an \ref IFX_TAPI_DECT_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_DECT_CFG_SET                _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_CFG_SET_IDX, IFX_TAPI_DECT_CFG_t)
/** This service selects DECT encoding and packetization time.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_DECT_ENC_CFG_t* Pointer to an \ref IFX_TAPI_DECT_ENC_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_DECT_ENC_CFG_SET            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_ENC_CFG_SET_IDX, IFX_TAPI_DECT_ENC_CFG_t)

/** This service sets the DECT channel encoder/decoder volume.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_LINE_VOLUME_t* Pointer to an \ref IFX_TAPI_LINE_VOLUME_t
    structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \code
   IFX_TAPI_LINE_VOLUME_t param;

   memset (&param, 0, sizeof (param));

   param.nGainTx = 3;
   param.nGainRx = 0;

   // Second channel on the second device
   param.ch = 1;
   param.dev = 1;
   if (ioctl (fd, IFX_TAPI_DECT_VOLUME_SET, (unsigned long) &param) != IFX_SUCCESS)
      return IFX_ERROR;
   \endcode
*/
#define IFX_TAPI_DECT_VOLUME_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_VOLUME_SET_IDX, IFX_TAPI_LINE_VOLUME_t)

/** This service retrieves the DECT statistics.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param IFX_TAPI_DECT_STATISTICS_t* Pointer to an \ref IFX_TAPI_DECT_STATISTICS_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_DECT_STATISTICS_GET         _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_STATISTICS_GET_IDX, IFX_TAPI_DECT_STATISTICS_t)

/** This service plays a tone on the DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param Tone table index

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_TONE_DECT_PLAY              _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_DECT_PLAY_IDX, IFX_TAPI_TONE_DECT_PLAY_t)

/** This service stops playing a tone on the DECT channel.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param unused - should be set to 0

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_TONE_DECT_STOP           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_DECT_STOP_IDX, IFX_TAPI_TONE_DECT_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_TONE_DECT_STOP           _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_DECT_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This service activates/deactivates the DECT channel echo canceller.
    This command applies to any channel file descriptor
    that includes a DECT channel module resource.

   \param pointer to \ref IFX_TAPI_DECT_EC_CFG_t structure

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_DECT_EC_CFG_SET             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_DECT_EC_CFG_SET_IDX, IFX_TAPI_DECT_EC_CFG_t)
/*@}*/ /* TAPI_INTERFACE_DECT */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ======================================================================= */
/* TAPI Message Waiting Lamp Services, ioctl commands                      */
/* (Group TAPI_INTERFACE_MWL)                                              */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_MWL */
/*@{*/
/** This service activates/deactivates the message waiting lamp using configurable
    analog signals on the line.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

    \remarks
    Every change of the line mode via \ref IFX_TAPI_LINE_FEED_SET disables
    the MWL service. It is up to the application to restart the MWL service
    (if required), after switching back to standby mode.

   \param IFX_TAPI_MWL_ACTIVATION_t* Pointer to an \ref IFX_TAPI_MWL_ACTIVATION_t structure containing the
   activation status.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_MWL_ACTIVATION_SET          _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MWL_ACTIVATION_SET_IDX, IFX_TAPI_MWL_ACTIVATION_t)

/** This service gets the activation status of the message waiting lamp.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_MWL_ACTIVATION_t* Pointer to an \ref IFX_TAPI_MWL_ACTIVATION_t structure containing the
   activation status.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_MWL_ACTIVATION_GET          _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_MWL_ACTIVATION_GET_IDX, IFX_TAPI_MWL_ACTIVATION_t)
/*@}*/ /* TAPI_INTERFACE_MWL */

/* ======================================================================= */
/* TAPI Level Peak Detector Services, ioctl commands                       */
/* (Group TAPI_INTERFACE_PEAKD)                                            */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_PEAKD */
/*@{*/

/** This service starts a tone level peak detector to measure the maximum
    16-bit value on a module input or output signal.
    This command applies to any channel file descriptor
    that includes an ALM, PCM or COD module resource.

   \param IFX_TAPI_PEAK_DETECT_CFG_t* Pointer to an \ref IFX_TAPI_PEAK_DETECT_CFG_t structure containing the
   basic detector configuration.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_PEAK_DETECT_START           _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PEAK_DETECT_START_IDX, IFX_TAPI_PEAK_DETECT_CFG_t)

/** This service stops a tone level peak detector that was previously
    started using \ref IFX_TAPI_PEAK_DETECT_START.
    This command applies to any channel file descriptor
    that includes an ALM, PCM or COD module resource.

   \param IFX_TAPI_PEAK_DETECT_CFG_t* Pointer to an \ref IFX_TAPI_PEAK_DETECT_CFG_t structure containing the
   basic detector configuration.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_PEAK_DETECT_STOP            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PEAK_DETECT_STOP_IDX, IFX_TAPI_PEAK_DETECT_CFG_t)

/** This service reads out the maximum value of the peak detector.
    It also resets and restarts the peak detector for a new measurement.
    This command applies to any channel file descriptor
    that includes an ALM, PCM or COD module resource.

   \param IFX_TAPI_PEAK_DETECT_RESULT_GET_t* Pointer to an \ref IFX_TAPI_PEAK_DETECT_RESULT_GET_t structure
   containing the result value.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_PEAK_DETECT_RESULT_GET      _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_PEAK_DETECT_RESULT_GET_IDX, IFX_TAPI_PEAK_DETECT_RESULT_GET_t)

/*@}*/ /* TAPI_INTERFACE_PEAKD */

/* ======================================================================= */
/* TAPI Level MF R2 Detector Services, ioctl commands                      */
/* (Group TAPI_INTERFACE_TONE)                                            */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** This service starts an MF R2 detector.
    This command applies to any channel file descriptor
    that includes an ALM, PCM or COD module resource.

   \param IFX_TAPI_TONE_MF_R2_CFG_t* Pointer to an \ref IFX_TAPI_TONE_MF_R2_CFG_t structure containing the
   basic detector configuration.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_TONE_MF_R2_START            _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_MF_R2_START_IDX, IFX_TAPI_TONE_MF_R2_CFG_t)

/** This service stops an MF R2 detector that was previously
    started using \ref IFX_TAPI_TONE_MF_R2_START.
    This command applies to any channel file descriptor
    that includes an ALM, PCM or COD module resource.

   \param IFX_TAPI_TONE_MF_R2_CFG_t* Pointer to an \ref IFX_TAPI_TONE_MF_R2_CFG_t structure containing the
   basic detector configuration.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_TONE_MF_R2_STOP             _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_TONE_MF_R2_STOP_IDX, IFX_TAPI_TONE_MF_R2_CFG_t)

/*@}*/ /* TAPI_INTERFACE_TONE */


/* ========================================================================== */
/* TAPI Calibration Services, ioctl commands                                  */
/* (Group TAPI_INTERFACE_CALIBRATION)                                         */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_CALIBRATION */
/*@{*/

/** This function starts the analog line calibration process. It returns while
    the calibration process is still running (unblocking call). Calibration is
    done on the analog line.
    The calibration process stops automatically when finished or when an error
    occurs. The termination, including the reason, is reported autonomously with
    the event \ref IFX_TAPI_EVENT_CALIBRATION_END.
    The TAPI compares the calibration results, stored in the TAPI low-level driver,
    with pre-defined SLIC value ranges. It overwrites the results with default
    values in case the results are out of range.
    All calibration result parameters can be read by
    \ref IFX_TAPI_CALIBRATION_RESULTS_GET. The currently used analog line
    coefficients can be read by \ref IFX_TAPI_CALIBRATION_CFG_GET.
    \ref IFX_TAPI_CALIBRATION_CFG_SET allows new coefficients to be set
    if required.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_CALIBRATION_t* Pointer to an \ref IFX_TAPI_CALIBRATION_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_CALIBRATION_START        _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_START_IDX, IFX_TAPI_CALIBRATION_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_CALIBRATION_START        _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_START_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This function stops a currently running analog line calibration process.
    A calibration can be started by IFX_TAPI_CALIBRATION_START.
    This function is called by the application in case the calibration did
    not finish successfully after a dedicated time-out (around 2 seconds).
    Stopping calibration means that the analog line coefficients are not
    well configured. The analog line has to be calibrated again using
    IFX_TAPI_CALIBRATION_START, or pre-defined values have to be programmed
    with IFX_TAPI_CALIBRATION_CFG_SET.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_CALIBRATION_t* Pointer to an \ref IFX_TAPI_CALIBRATION_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_CALIBRATION_STOP         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_STOP_IDX, IFX_TAPI_CALIBRATION_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_CALIBRATION_STOP         _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This function reads out the analog line coefficient results that
    are generated by an automatic calibration process.
    These values might differ from the currently used coefficients.
    IFX_TAPI_CALIBRATION_START starts the automatic calibration and
    IFX_TAPI_CALIBRATION_CFG_SET allows the coefficients to be written.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_CALIBRATION_CFG_t* Pointer to an
      \ref IFX_TAPI_CALIBRATION_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_CALIBRATION_RESULTS_GET     _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_RESULTS_GET_IDX, IFX_TAPI_CALIBRATION_CFG_t)

/** This function sets the analog line coefficients.
    This command writes the coefficients for the analog line described in
    \ref IFX_TAPI_CALIBRATION_t.
    These coefficients could also be generated by an automatic calibration
    process (\ref IFX_TAPI_CALIBRATION_START).
    All calibration process result parameters can be read by
    \ref IFX_TAPI_CALIBRATION_RESULTS_GET. The currently used analog line
    coefficients can be read by \ref IFX_TAPI_CALIBRATION_CFG_GET.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_CALIBRATION_CFG_t* Pointer to an
      \ref IFX_TAPI_CALIBRATION_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_CALIBRATION_CFG_SET         _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_CFG_SET_IDX, IFX_TAPI_CALIBRATION_CFG_t)

/** This function reads out the analog line coefficients. It reads the
    currently used coefficients for the analog line described
    in \ref IFX_TAPI_CALIBRATION_t.
    \ref IFX_TAPI_CALIBRATION_CFG_SET allows the coefficients to be modified.
    \ref IFX_TAPI_CALIBRATION_START could be used to start an automatic
    calibration process for these coefficients. All calibration process
    result parameters can be read by \ref IFX_TAPI_CALIBRATION_RESULTS_GET.
    This command applies to any channel file descriptor
    that includes an analog (ALM) module resource.

   \param IFX_TAPI_CALIBRATION_CFG_t* Pointer to an
      \ref IFX_TAPI_CALIBRATION_CFG_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_CALIBRATION_CFG_GET         _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CALIBRATION_CFG_GET_IDX, IFX_TAPI_CALIBRATION_CFG_t)

/*@}*/ /* TAPI_INTERFACE_CALIBRATION */

/* ========================================================================== */
/* TAPI COMTEL Signal Services, ioctl commands                                */
/* (Group TAPI_INTERFACE_COMTEL)                                              */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_COMTEL */
/*@{*/

/** This function starts COMTEL data transmission. It returns while the
    transmission process is still running (unblocking call). The transmission is
    done on the analog line as set in \ref IFX_TAPI_COMTEL_t.
    The data transmission stops automatically and is indicated by a reported
    event \ref IFX_TAPI_EVENT_COMTEL_END. The event reports whether the
    transmission finished successfully or stopped during an off-hook event.
    This command applies to ALM modules.

   \param IFX_TAPI_COMTEL_t* The parameter points to an
      \ref IFX_TAPI_COMTEL_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_COMTEL_START               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_COMTEL_START_IDX, IFX_TAPI_COMTEL_t)

/*@}*/ /* TAPI_INTERFACE_COMTEL */

/* ========================================================================== */
/* TAPI Analog Line Continuous Measurement Services, ioctl commands           */
/* (Group TAPI_INTERFACE_CONTMEASUREMENT)                                               */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_CONTMEASUREMENT */
/*@{*/

/** This function triggers the request to read out the results of the
    continuous analog line measurement process. The TAPI generates an event when
    the requested measurement results are available. The application then
    calls \ref IFX_TAPI_CONTMEASUREMENT_GET to read out the results. The
    \ref IFX_TAPI_CONTMEASUREMENT_RESET function allows the result
    history to be reset in the firmware.
    This command applies to ALM modules.

   \param IFX_TAPI_CONTMEASUREMENT_t* Pointer to an \ref IFX_TAPI_CONTMEASUREMENT_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \note This function is non-blocking and returns immediately.
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_CONTMEASUREMENT_REQ      _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CONTMEASUREMENT_REQ_IDX, IFX_TAPI_CONTMEASUREMENT_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_CONTMEASUREMENT_REQ      _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CONTMEASUREMENT_REQ_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This function reads results of the continuous analog line measurement
    process. \ref IFX_TAPI_CONTMEASUREMENT_REQ is used to request new
    measurement values from the firmware. An event is generated by the TAPI
    when new measurement results are available. The
    \ref IFX_TAPI_CONTMEASUREMENT_RESET function allows the result
    history to be reset in the firmware.
    This command applies to ALM modules.

   \param IFX_TAPI_CONTMEASUREMENT_GET_t* Pointer to an \ref IFX_TAPI_CONTMEASUREMENT_GET_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

   \note This function returns with an error in case there are no results to be
   read out, or if the results have already been read before.
*/
#define IFX_TAPI_CONTMEASUREMENT_GET         _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CONTMEASUREMENT_GET_IDX, IFX_TAPI_CONTMEASUREMENT_GET_t)

/** This function resets the history of the continuous analog line measurement
    process.
    This command applies to ALM modules.

   \param IFX_TAPI_CONTMEASUREMENT_t* Pointer to an \ref IFX_TAPI_CONTMEASUREMENT_t structure.

   \return Returns the following values:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_CONTMEASUREMENT_RESET    _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CONTMEASUREMENT_RESET_IDX, IFX_TAPI_CONTMEASUREMENT_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_CONTMEASUREMENT_RESET    _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_CONTMEASUREMENT_RESET_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Read out the analog line battery voltage configuration. This configuration
    can be set by BBD coefficient download or
    by \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_SET. Note that this command returns a
    given configuration instead of any measured value.
    This command applies to ALM modules.

   \param IFX_TAPI_LINE_BATTERY_VOLTAGE_t* Pointer to an \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_LINE_BATTERY_VOLTAGE_GET    _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_BATTERY_VOLTAGE_GET_IDX, IFX_TAPI_LINE_BATTERY_VOLTAGE_t)

/** Set the current battery voltage of the analog line.
   The voltage can be read out by \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_GET.
   This command applies to ALM modules.

   \param IFX_TAPI_LINE_BATTERY_VOLTAGE_t* Pointer to an \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_LINE_BATTERY_VOLTAGE_SET    _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_BATTERY_VOLTAGE_SET_IDX, IFX_TAPI_LINE_BATTERY_VOLTAGE_t)

/*@}*/ /* TAPI_INTERFACE_CONTMEASUREMENT */

/* ======================================================================= */
/* TAPI Network Line Testing services, ioctl commands                       */
/* (Group TAPI_INTERFACE_NLT)                                              */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_NLT */
/*@{*/

/** This service starts an NLT test.
    This command applies to ALM modules.

   \param IFX_TAPI_NLT_TEST_START_t Value from \ref IFX_TAPI_NLT_TEST_START_t; specifies the target device and channel
   on which the test is to be performed

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_NLT_TEST_START              _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_NLT_TEST_START_IDX, IFX_TAPI_NLT_TEST_START_t)

/** This service reads test results of the specified NLT test.
    This command applies to ALM modules.

   \param IFX_TAPI_NLT_RESULT_GET_t Value from \ref IFX_TAPI_NLT_RESULT_GET_t; specifies the target
   device and channel on which the test was performed, and consequently test
   results are to be read.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define IFX_TAPI_NLT_RESULT_GET              _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_NLT_RESULT_GET_IDX, IFX_TAPI_NLT_RESULT_GET_t)

/*@}*/ /* TAPI_INTERFACE_NLT */

#ifndef TAPI4_DXY_DOC
/* ========================================================================== */
/* TAPI GR909 Services, ioctl commands                                        */
/* (Group TAPI_INTERFACE_GR909)                                               */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_GR909 */
/*@{*/

/** Start a GR909 measurement;
    parameter is a pointer to an \ref IFX_TAPI_GR909_START_t structure.
    \ingroup TAPI_INTERFACE_GR909
*/
#define IFX_TAPI_GR909_START                 _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_GR909_START_IDX, IFX_TAPI_GR909_START_t)

#ifdef TAPI_ONE_DEVNODE
   /** Stop a GR909 measurement.
       \ingroup TAPI_INTERFACE_GR909
   */
   #define IFX_TAPI_GR909_STOP               _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_GR909_STOP_IDX, IFX_TAPI_GR909_STOP_t)
#else /* TAPI_ONE_DEVNODE */
   /** Stop a GR909 measurement.
       \ingroup TAPI_INTERFACE_GR909
   */
   #define IFX_TAPI_GR909_STOP               _IO   (IFX_TAPI_IOC_MAGIC, IFX_TAPI_GR909_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** Read results of a GR909 measurement;
    parameter is a pointer to an \ref IFX_TAPI_GR909_RESULT_t structure.
    \ingroup TAPI_INTERFACE_GR909
*/
#define IFX_TAPI_GR909_RESULT                _IOWR (IFX_TAPI_IOC_MAGIC, IFX_TAPI_GR909_RESULT_IDX, IFX_TAPI_GR909_RESULT_t)

/*@}*/ /* TAPI_INTERFACE_GR909 */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI4_DXY_DOC
/* ========================================================================== */
/* TAPI FXS Phone Detection Services, ioctl commands                          */
/* (Group TAPI_INTERFACE_PHONE_DETECTION)                                     */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_PHONE_DETECTION */
/*@{*/

/** This function starts an analog line capacitance measurement session.
    The TAPI generates an \ref IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_RDY event
    containing the results when the measurement is completed.
    This measurement can be stopped at any time by calling
    \ref IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP or selecting another line
    feeding mode. This command does not take any parameters.

    \param int This interface expects no parameters. It should be set to 0.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_LINE_MEASURE_CAPACITANCE_START _IOW (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_MEASURE_CAPACITANCE_START_IDX, IFX_TAPI_LINE_MEASURE_CAPACITANCE_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_LINE_MEASURE_CAPACITANCE_START _IO  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_MEASURE_CAPACITANCE_START_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This function stops any currently running analog line capacitance measurement session.
    A measurement can be started by calling
    \ref IFX_TAPI_LINE_MEASURE_CAPACITANCE_START.
    This command does not take any parameters.

    \param int This interface expects no parameters. It should be set to 0.

   \return Returns the following values:
    - IFX_SUCCESS: if successful
    - IFX_ERROR: in case of an error
*/
#ifdef TAPI_ONE_DEVNODE
   #define IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP  _IOW (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP_IDX, IFX_TAPI_LINE_MEASURE_CAPACITANCE_t)
#else /* TAPI_ONE_DEVNODE */
   #define IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP  _IO  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP_IDX)
#endif /* TAPI_ONE_DEVNODE */

/** This function reads out the current FXS phone detection state machine
    parameters. These parameters can be modified by calling
    \ref IFX_TAPI_LINE_PHONE_DETECT_CFG_SET.
    This command applies to any channel file descriptor for an analog (ALM)
    module resource.

   \param IFX_TAPI_LINE_PHONE_DETECT_CFG_t* Pointer to an \ref IFX_TAPI_LINE_PHONE_DETECT_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_LINE_PHONE_DETECT_CFG_GET  _IOR  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_PHONE_DETECT_CFG_GET_IDX, IFX_TAPI_LINE_PHONE_DETECT_CFG_t)

/** This function configures the FXS phone detection state machine parameters.
    These parameters specify, among others, time periods for off-hook detection
    and disabled line-state phases. The configured capacitance value specifies
    the threshold that indicates the detection of a telephone.
    \ref IFX_TAPI_LINE_PHONE_DETECT_CFG_GET allows for the current
    configuration to be read out.
    This command applies to any channel file descriptor for an analog (ALM)
    module resource.

   \param IFX_TAPI_LINE_PHONE_DETECT_CFG_t* Pointer to an \ref IFX_TAPI_LINE_PHONE_DETECT_CFG_t structure.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
#define  IFX_TAPI_LINE_PHONE_DETECT_CFG_SET  _IOW  (IFX_TAPI_IOC_MAGIC, IFX_TAPI_LINE_PHONE_DETECT_CFG_SET_IDX, IFX_TAPI_LINE_PHONE_DETECT_CFG_t)

/*@}*/ /* TAPI_INTERFACE_PHONE_DETECTION */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ========================================================================= */
/*                     TAPI Interface Constants                              */
/* ========================================================================= */

/* ======================================================================== */
/* TAPI Initialization Services, constants (Group TAPI_INTERFACE_INIT)      */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================= */
/* TAPI Operation Control Services, constants (Group TAPI_INTERFACE_OP)    */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** TAPI phone volume control */
/** Switches the volume to low: -12 dB. */
#define IFX_TAPI_LINE_VOLUME_LOW                (-12)
/** Switches the volume to medium: -6 dB. */
#define IFX_TAPI_LINE_VOLUME_MEDIUM              (-6)
/** Switches the volume to high: 0 dB */
#define IFX_TAPI_LINE_VOLUME_HIGH                 (0)
/** Switches the volume to minimum gain: -24 dB; note that DTMF detection etc.
    might not work properly with such low signals. */
#define IFX_TAPI_LINE_VOLUME_MIN_GAIN           (-24)
/** Switches the volume to maximum gain: +24 dB. */
#define IFX_TAPI_LINE_VOLUME_MAX_GAIN            (24)

/** TAPI LEC control */

/** LEC delay line maximum length. */
#define IFX_TAPI_LEC_LEN_MAX                      (16)
/** LEC delay line minimum length. */
#define IFX_TAPI_LEC_LEN_MIN                      (4)

/*@}*/ /* TAPI_INTERFACE_OP */

/* ================================================================ */
/* TAPI Metering Services, constants (Group TAPI_INTERFACE_METER)   */
/* ================================================================ */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_METER */

/* ====================================================================== */
/* TAPI Tone Services, constants (Group TAPI_INTERFACE_TONE)              */
/* ====================================================================== */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Maximum number of simple tones that can be played at one go. */
#define IFX_TAPI_TONE_SIMPLE_MAX                  (7)

/** Maximum tone generation steps, also called cadences. */
#define IFX_TAPI_TONE_STEPS_MAX                   (6)

/** Tone minimum index that can be configured by the user. */
#define IFX_TAPI_TONE_INDEX_MIN                  (32)

/** Tone maximum index that can be configured by the user. */
#define IFX_TAPI_TONE_INDEX_MAX                 (255)

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ===================================================================== */
/* TAPI Misc Services, constants (Group TAPI_INTERFACE_MISC)             */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Used to report events from any channel in the device. This constant is also
   used to report events that cannot be associated with a particular channel.*/
#define IFX_TAPI_EVENT_ALL_CHANNELS               0xffff

/** Used to report events from any device. This constant is also
   used to report events that cannot be associated with a particular device. */
#define IFX_TAPI_EVENT_ALL_DEVICES                0xffff
/*@}*/ /* TAPI_INTERFACE_MISC */

/* =================================================================== */
/* TAPI Signal Detection Services, constants                           */
/* (Group TAPI_INTERFACE_SIGNAL)                                       */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ===================================================================== */
/* TAPI CID Features Service, constants (Group TAPI_INTERFACE_CID)         */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** CID RX FIFO size. */
#define IFX_TAPI_CID_RX_FIFO_SIZE                (10)

/** CID RX size of one data buffer.

    \remarks Larger data is automatically split into multiple buffers until
             there is no further data, or till the FIFO defined above is full.
             The data can then be retrieved by multiple calls to
             \ref IFX_TAPI_CID_RX_DATA_GET, and the application should
             concatenate it before interpreting the data. */
#define IFX_TAPI_CID_RX_SIZE_MAX                (128)

/**
   CID TX maximum buffer size.

   \remarks
   -  ETSI  :
      call setup cmd : 2,  cli : 22, date/time : 10, name : 52,
      redir num : 22, checksum : 1 => 109 Bytes max in CID buffer
   -  NTT :
      DLE : 3, SOH : 1, Header : 1, STX : 1, ETX : 1, DATA: 119, CRC : 2
      => 128 Bytes max in CID Buffer
   - ETSI SMS (protocol 2):
      type 1, length 1, data 1-255, crc 1 => 258 byte
*/
#define IFX_TAPI_CID_TX_SIZE_MAX                (258)

/** Maximum allowed length of one CID message element (in characters). */
#define IFX_TAPI_CID_MSG_LEN_MAX                 (50)

/*@}*/ /* TAPI_INTERFACE_CID */

#ifndef TAPI_DXY_DOC
/* =================================================================== */
/* TAPI Connection Services, constants (Group TAPI_INTERFACE_CON)      */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Number of bytes for the Ethernet MAC source and destination address
    field. */
#define IFX_TAPI_MAC_LEN 6

/** Number of bytes for the IPv6 source and destination address field. */
#define IFX_TAPI_IPV6_LEN 16

/** Number of bytes for the IPv4 source and destination address field. */
#define IFX_TAPI_IPV4_LEN 4

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================= */
/* TAPI Miscellaneous Services, constants (Group TAPI_INTERFACE_MISC)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_MISC */

#endif /* TAPI_DXY_DOC */
/* ======================================================================= */
/* TAPI Power Ringing Services, constants (Group TAPI_INTERFACE_RINGING)         */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Maximum number of cadence bytes. */
#define IFX_TAPI_RING_CADENCE_MAX_BYTES                 (40)

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ======================================================================= */
/* TAPI PCM Services, constants (Group TAPI_INTERFACE_PCM)                 */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================= */
/* TAPI Fax T.38 Stack, constants (Group TAPI_INTERFACE_FAX_STACK)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_FAX_STACK */
/*@{*/

/** Size of the data array to configure NSX on the T.38 stack. */
#define  IFX_TAPI_T38_NSXLEN                             7
/** Default value to configure nGainRx = 3.14 dB and nGainRx = -3.14 dB */
#define  IFX_TAPI_T38_CFG_DEFAULT_GAIN                   100

/*@}*/ /* TAPI_INTERFACE_FAX_STACK */

/** Maximum version information string length. */
#define  IFX_TAPI_VERSION_LEN                            64

/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_FAX */

/* ========================================================================= */
/*                      TAPI Interface Enumerations                          */
/* ========================================================================= */

/* ======================================================================== */
/* TAPI Initialization Services, enumerations (Group TAPI_INTERFACE_INIT)   */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** TAPI initialization modes; controls pre-configuration of the driver for
    different target systems. This parameter specifies for which operation
    the driver should be set up. This pre-configures the driver with typical
    settings for the given operation mode. Not all modes are supported by all
    LL drivers, and the meaning of the mode is dependent on the implementation.*/
typedef enum
{
   /** Default initialization. The meaning of the default
       might vary between platforms. */
   IFX_TAPI_INIT_MODE_DEFAULT = 0,
   /** Typical VoIP solution; phone connected to a packet coder (data channel)
    with DSP features for signal detection. */
   IFX_TAPI_INIT_MODE_VOICE_CODER = 1,
   /** Phone to PCM using DSP features for signal detection. */
   IFX_TAPI_INIT_MODE_PCM_DSP = 2,
   /** Phone to PCM not using DSP features for signal detection. */
   IFX_TAPI_INIT_MODE_PCM_PHONE = 3,
   /** Internal test only; direct COD to ALM connection */
   IFX_TAPI_INIT_MODE_TEST_COD2ALM = 0xf0,
   /** Internal test only; no initialization and no modules connected at all */
   IFX_TAPI_INIT_MODE_NONE = 0xff
} IFX_TAPI_INIT_MODE_t;

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================== */
/* TAPI Operation Control Services, enumerations (Group TAPI_INTERFACE_OP)  */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Definitions for line feeding. */
typedef enum
{
   /** Normal feeding mode for phone off-hook. */
   IFX_TAPI_LINE_FEED_ACTIVE = 0,
   /** Normal feeding mode for phone off-hook reversed. */
   IFX_TAPI_LINE_FEED_ACTIVE_REV = 1,
   /** Power-down resistance = on-hook with hook detection. */
   IFX_TAPI_LINE_FEED_STANDBY = 2,
   /** Switches off the line, but the device is able to test the line. */
   IFX_TAPI_LINE_FEED_HIGH_IMPEDANCE = 3,
   /** Switches off the line and the device. */
   IFX_TAPI_LINE_FEED_DISABLED = 4,
   /* Obsolete */
   /* IFX_TAPI_LINE_FEED_GROUND_START = 5, */
   /** Thresholds for automatic battery switch are set via coefficient settings. */
   IFX_TAPI_LINE_FEED_NORMAL_AUTO = 6,
   /** Thresholds for automatic battery switch are set via coefficient
   settings reversed. */
   IFX_TAPI_LINE_FEED_REVERSED_AUTO = 7,
   /** Feeding mode for phone off-hook with low battery to save power. */
   IFX_TAPI_LINE_FEED_NORMAL_LOW = 8,
   /** Feeding mode for phone off-hook with low battery to save power
       and reserved polarity. */
   IFX_TAPI_LINE_FEED_REVERSED_LOW = 9,
   /** Reserved; needed for ring call-back function. */
   IFX_TAPI_LINE_FEED_RING_BURST = 10,
   /** Reserved; needed for ring call-back function. */
   IFX_TAPI_LINE_FEED_RING_PAUSE = 11,
   /** Reserved; needed for internal function.  */
   IFX_TAPI_LINE_FEED_METER = 12,
   /** Reserved; special test line mode.*/
   IFX_TAPI_LINE_FEED_ACTIVE_LOW = 13,
   /** Reserved; special test line mode. */
   IFX_TAPI_LINE_FEED_ACTIVE_BOOSTED = 14,
   /** Reserved; special line mode for GEMINAX-S MAX SLIC. */
   IFX_TAPI_LINE_FEED_ACT_TESTIN = 15,
   /** Reserved; special line mode for GEMINAX-S MAX SLIC. */
   IFX_TAPI_LINE_FEED_DISABLED_RESISTIVE_SWITCH = 16,
   /** Power-down resistance = on-hook with hook detection. */
   IFX_TAPI_LINE_FEED_PARKED_REVERSED = 17,
   /** Active feeding mode with off-hook sensing ability, normal polarity and
       5 kOhm resistors activated. */
   IFX_TAPI_LINE_FEED_ACTIVE_RES_NORMAL = 18,
   /** Active feeding mode with off-hook sensing ability, reversed polarity and
       5 kOhm resistors activated. */
   IFX_TAPI_LINE_FEED_ACTIVE_RES_REVERSED = 19,
   /** Reserved; special line mode for SLIC-LCP. */
   IFX_TAPI_LINE_FEED_ACT_TEST = 20,
   /** Network line testing operating mode. */
   IFX_TAPI_LINE_FEED_NLT = 21,
   /** Power-down resistance = on-hook with hook detection and 5 kOhm
       resistors activated. */
   IFX_TAPI_LINE_FEED_STANDBY_RES = 22,
   /** Active mode with TIP wire in high-impedance state. */
   IFX_TAPI_LINE_FEED_ACTIVE_HIT = 23,
   /** Active mode with RING wire in high-impedance state. */
   IFX_TAPI_LINE_FEED_ACTIVE_HIR = 24,
   /** Starts the FXS Phone Plug Detection state machine. */
   IFX_TAPI_LINE_FEED_PHONE_DETECT = 25
} IFX_TAPI_LINE_MODE_t;

   /** Line feed mode configuration \ref IFX_TAPI_LINE_FEED_SET */
#ifdef TAPI_ONE_DEVNODE
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Apply the line mode of this analog channel. */
      IFX_TAPI_LINE_MODE_t lineMode;
   } IFX_TAPI_LINE_FEED_t;
#else /* TAPI_ONE_DEVNODE */
   typedef IFX_TAPI_LINE_MODE_t IFX_TAPI_LINE_FEED_t;
#endif /* TAPI_ONE_DEVNODE */

/** Definitions for line types. */
typedef enum
{
   /** Wrong line-mode type for analog channel. */
   IFX_TAPI_LINE_TYPE_UNKNOWN = -1,
   /** Line-mode type FXS narrowband sampling for analog channel. */
   IFX_TAPI_LINE_TYPE_FXS_NB = 0,
   /** Line-mode type FXS wideband sampling for analog channel. */
   IFX_TAPI_LINE_TYPE_FXS_WB = 1,
   /** Line-mode type FXS automatic NB/WB switching for analog channel. */
   IFX_TAPI_LINE_TYPE_FXS_AUTO = 2,
   /** Line-mode type FXO narrowband sampling for analog channel. */
   IFX_TAPI_LINE_TYPE_FXO_NB = 3
} IFX_TAPI_LINE_TYPE_t;

/* Map the old names to the NB names */
#define IFX_TAPI_LINE_TYPE_FXS  IFX_TAPI_LINE_TYPE_FXS_NB
#define IFX_TAPI_LINE_TYPE_FXO  IFX_TAPI_LINE_TYPE_FXO_NB

/** Line type configuration \ref IFX_TAPI_LINE_TYPE_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Configures the line type of this analog channel. */
   IFX_TAPI_LINE_TYPE_t    lineType;
   /** Corresponding index of the DAA channel defined in drv_daa
       (board-specific). */
   IFX_uint8_t             nDaaCh;
} IFX_TAPI_LINE_TYPE_CFG_t;

/** LEC type configuration. */
typedef enum
{
   /** LEC and echo suppressor turned off. */
   IFX_TAPI_WLEC_TYPE_OFF = 0x00,
   /** LEC using fixed window; no echo suppressor. */
   IFX_TAPI_WLEC_TYPE_NE  = 0x01,
   /** LEC using fixed and moving window; no echo suppressor. */
   IFX_TAPI_WLEC_TYPE_NFE = 0x02,
   /** LEC using fixed window and echo suppressor. */
   IFX_TAPI_WLEC_TYPE_NE_ES  = 0x03,
   /** LEC using fixed and moving window and echo suppressor. */
   IFX_TAPI_WLEC_TYPE_NFE_ES = 0x04,
   /** Echo suppressor. */
   IFX_TAPI_WLEC_TYPE_ES     = 0x05
} IFX_TAPI_WLEC_TYPE_t;

/** Definition of LEC window size; to be used for
      configuration of fixed and moving window size. */
typedef enum
{
   /** LEC window size 4 ms.*/
   IFX_TAPI_WLEN_WSIZE_4 = 4,
   /** LEC window size 6 ms. */
   IFX_TAPI_WLEN_WSIZE_6 = 6,
   /** LEC window size 8 ms. */
   IFX_TAPI_WLEN_WSIZE_8 = 8,
   /** LEC window size 16 ms. */
   IFX_TAPI_WLEN_WSIZE_16 = 16
} IFX_TAPI_WLEC_WIN_SIZE_t;

/** Validation types used for the \ref IFX_TAPI_LINE_HOOK_VT_t structure.

   \remarks
   The default values are as follows:

   - 80 ms  <= flash time      <= 200 ms
   - 30 ms  <= digit low time  <= 80 ms
   - 30 ms  <= digit high time <= 80 ms
   - interdigit time =     300 ms
   - off hook time   =      40 ms
   - on hook time    =     400 ms
   !!! open: only min time is validated and pre-initialized */
typedef enum
{
   /** Settings for hook validation; if the time lies between nMinTime and
      nMaxTime, an exception is raised. */
   IFX_TAPI_LINE_HOOK_VT_HOOKOFF_TIME     = 0x0,
   /** Settings for hook validation; if the time value lies between nMinTime and
       nMaxTime, an exception is raised. */
   IFX_TAPI_LINE_HOOK_VT_HOOKON_TIME      = 0x1,
   /** Settings for hook flash validation - also known as register recall.
       If the time value lies between the times defined in the fields nMinTime
       and nMaxTime, an exception is raised. */
   IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME   = 0x2,
   /** Settings for pulse digit low, open loop and make validation.
       The time value must lie between the times defined in the fields nMinTime and
       nMaxTime, in order to recognize it as a pulse dialing event. */
   IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME    = 0x4,
   /** Settings for pulse digit high, close loop and break validation.
       The time value must lie between the times defined in the fields nMinTime and
       nMaxTime, in order to recognize it as a pulse dialing event. */
   IFX_TAPI_LINE_HOOK_VT_DIGITHIGH_TIME   = 0x8,
   /** Settings for pulse digit pause; the time must lie between the times defined
       in the fields nMinTime and nMaxTime in order to recognize it as a
       pulse dialing event. */
   IFX_TAPI_LINE_HOOK_VT_INTERDIGIT_TIME  = 0x10
} IFX_TAPI_LINE_HOOK_VALIDATION_TYPE_t;

/** WLEC NLP settings. */
typedef enum
{
   /** Reserved; default NLP on. */
   IFX_TAPI_WLEC_NLP_DEFAULT = 0,
   /** Switches on NLP. */
   IFX_TAPI_WLEC_NLP_ON = 1,
   /** Switches off NLP. */
   IFX_TAPI_WLEC_NLP_OFF = 2
} IFX_TAPI_WLEC_NLP_t;

#if 1 // ctc
typedef enum
{
   /** Reserved. Default NLP on.*/
   IFX_TAPI_LEC_NLP_DEFAULT = IFX_TAPI_WLEC_NLP_DEFAULT,
   /** Switches on NLP. */
   IFX_TAPI_LEC_NLP_ON = IFX_TAPI_WLEC_NLP_ON,
   /** Switches off NLP. */
   IFX_TAPI_LEC_NLP_OFF = IFX_TAPI_WLEC_NLP_OFF
} IFX_TAPI_LEC_NLP_t;
#endif

/** Specifies the enable/disable mode of the high level. */
typedef enum
{
   /** Disables line level (default). */
   IFX_TAPI_LINE_LEVEL_DISABLE = 0x0,
   /** Enables line level. */
   IFX_TAPI_LINE_LEVEL_ENABLE = 0x1
} IFX_TAPI_LINE_LEVEL_t;

/** Specifies the enable/disable mode of the AGC resource.*/
typedef enum
{
   /** Disables AGC. */
   IFX_TAPI_ENC_AGC_MODE_DISABLE = 0x0,
   /** Enables AGC. */
   IFX_TAPI_ENC_AGC_MODE_ENABLE  = 0x1
} IFX_TAPI_ENC_AGC_MODE_t;

/** Structure used for AGC configuration. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** 'Compare Level'; this is the target level.
       Range: -50 dB ... 0 dB */
   IFX_int32_t   com;
   /** 'Maximum Gain'; maximum gain that will be applied to the signal.
       Range: 0 dB ... 48 dB */
   IFX_int32_t   gain;
   /** 'Maximum attenuation for AGC'; maximum attenuation that will be
       applied to the signal.
       Range: -42 dB ... 0 dB */
   IFX_int32_t   att;
   /** 'Minimum Input Level'; signals below this threshold will not be
       processed by AGC.
       Range: -60 dB ... -25 dB */
   IFX_int32_t   lim;
} IFX_TAPI_ENC_AGC_CFG_t;

/** Specifies the current on-hook or off-hook status of the line. */
typedef enum {
   /** Line is in on-hook state. */
   IFX_TAPI_LINE_ONHOOK = 0,
   /** Line is in off-hook state. */
   IFX_TAPI_LINE_OFFHOOK = 1
} IFX_TAPI_LINE_HOOK_t;

/** Echo Canceller type selection. */
typedef enum
{
   /** no EC or echo suppressor. */
   IFX_TAPI_EC_TYPE_OFF = 0x00,
   /** echo suppressor. */
   IFX_TAPI_EC_TYPE_ES  = 0x01
} IFX_TAPI_EC_TYPE_t;

/*@}*/ /* TAPI_INTERFACE_OP */

/* ======================================================================= */
/* TAPI Metering Services, enumerations (Group TAPI_INTERFACE_METER)       */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** Metering modes. */
typedef enum
{
   /** Normal TTX mode. */
   IFX_TAPI_METER_MODE_TTX = 0,
   /** Reverse polarity mode. */
   IFX_TAPI_METER_MODE_REVPOL = 1
} IFX_TAPI_METER_MODE_t;

/*@}*/ /* TAPI_INTERFACE_METER */

/* ======================================================================= */
/* TAPI Tone Control Services, enumerations (Group TAPI_INTERFACE_TONE)    */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Tone sources. */
typedef enum
{
   /** Tone is played out on the default source. */
   IFX_TAPI_TONE_SRC_DEFAULT = 0,
   /** Tone is played out on the DSP for the DECT channel. */
   IFX_TAPI_TONE_SRC_DECT    = 0x2000,
   /** Tone is played out on the DSP; default, if available. */
   IFX_TAPI_TONE_SRC_DSP     = 0x4000,
   /** Tone is played out on the local tone generator in
      the analog part of the device; default if DSP is not available. */
   IFX_TAPI_TONE_SRC_TG      = 0x8000
} IFX_TAPI_TONE_SRC_t;

/** Used for selection of one or more frequencies belonging to a tone cadence. */
typedef enum
{
   /** All frequencies are inactive.*/
   IFX_TAPI_TONE_FREQNONE = 0,
   /** Plays frequency A. */
   IFX_TAPI_TONE_FREQA = 0x1,
   /** Plays frequency B.*/
   IFX_TAPI_TONE_FREQB = 0x2,
   /** Plays frequency C. */
   IFX_TAPI_TONE_FREQC = 0x4,
   /** Plays frequency D. */
   IFX_TAPI_TONE_FREQD = 0x8,
   /** Plays all frequencies. */
   IFX_TAPI_TONE_FREQALL = 0xF
} IFX_TAPI_TONE_FREQ_t;

/** Modulation setting for a cadence step. */
typedef enum
{
   /** Modulation is off for the cadence step.*/
   IFX_TAPI_TONE_MODULATION_OFF   = 0,
   /** Modulation is on for the cadence step. */
   IFX_TAPI_TONE_MODULATION_ON    = 1
} IFX_TAPI_TONE_MODULATION_t;

/** Modulation factor settings */
typedef enum
{
   /* Modulation factor 100% */
   IFX_TAPI_TONE_MODULATION_FACTOR_100 = 0,  /* for backward compatibility */
   /* Modulation factor 10% */
   IFX_TAPI_TONE_MODULATION_FACTOR_10  = 10,
   /* Modulation factor 20% */
   IFX_TAPI_TONE_MODULATION_FACTOR_20  = 20,
   /* Modulation factor 30% */
   IFX_TAPI_TONE_MODULATION_FACTOR_30  = 30,
   /* Modulation factor 40% */
   IFX_TAPI_TONE_MODULATION_FACTOR_40  = 40,
   /* Modulation factor 50% */
   IFX_TAPI_TONE_MODULATION_FACTOR_50  = 50,
   /* Modulation factor 55% */
   IFX_TAPI_TONE_MODULATION_FACTOR_55  = 55,
   /* Modulation factor 60% */
   IFX_TAPI_TONE_MODULATION_FACTOR_60  = 60,
   /* Modulation factor 65% */
   IFX_TAPI_TONE_MODULATION_FACTOR_65  = 65,
   /* Modulation factor 70% */
   IFX_TAPI_TONE_MODULATION_FACTOR_70  = 70,
   /* Modulation factor 75% */
   IFX_TAPI_TONE_MODULATION_FACTOR_75  = 75,
   /* Modulation factor 80% */
   IFX_TAPI_TONE_MODULATION_FACTOR_80  = 80,
   /* Modulation factor 85% */
   IFX_TAPI_TONE_MODULATION_FACTOR_85  = 85,
   /* Modulation factor 90% */
   IFX_TAPI_TONE_MODULATION_FACTOR_90  = 90,
   /* Modulation factor 95% */
   IFX_TAPI_TONE_MODULATION_FACTOR_95  = 95
} IFX_TAPI_TONE_MODULATION_FACTOR_t;

/** Tone types */
typedef enum
{
   /** Simple tone. */
   IFX_TAPI_TONE_TYPE_SIMPLE = 1,
   /** Composed tone. */
   IFX_TAPI_TONE_TYPE_COMPOSED = 2,
#ifndef TAPI_DXY_DOC
   /** Dual tone. */
   IFX_TAPI_TONE_TYPE_DUAL = 3
#endif /* TAPI_DXY_DOC */
} IFX_TAPI_TONE_TYPE_t;

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ==================================================================== */
/* TAPI Signal Detection Services, enumerations                         */
/* (Group TAPI_INTERFACE_SIGNAL)                                        */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/** Lists the tone detection options. Some applications may not be interested
    in whether the signal came from the receive or transmit path. Therefore,
    a mask exists for each signal, including the receive and transmit paths.
    */
typedef enum
{
   /** No signal detected.*/
   IFX_TAPI_SIG_NONE        = 0x0,
   /** V.21 preamble fax tone, digital identification signal (DIS),
      receive path. */
   IFX_TAPI_SIG_DISRX       = 0x1,
   /** V.21 preamble fax tone, digital identification signal (DIS),
      transmit path. */
   IFX_TAPI_SIG_DISTX       = 0x2,
   /** V.21 preamble fax tone in all paths, digital identification
       signal (DIS).  */
   IFX_TAPI_SIG_DIS         = 0x4,
   /** V.25 2100 Hz (CED) modem/fax tone, receive path. */
   IFX_TAPI_SIG_CEDRX       = 0x8,
   /** V.25 2100 Hz (CED) modem/fax tone, transmit path. */
   IFX_TAPI_SIG_CEDTX       = 0x10,
   /** V.25 2100 Hz (CED) modem/fax tone in all paths. */
   IFX_TAPI_SIG_CED         = 0x20,
   /** CNG fax calling tone (1100 Hz) receive path. */
   IFX_TAPI_SIG_CNGFAXRX    = 0x40,
   /** CNG fax calling tone (1100 Hz) transmit path. */
   IFX_TAPI_SIG_CNGFAXTX    = 0x80,
   /** CNG fax calling tone (1100 Hz) in all paths. */
   IFX_TAPI_SIG_CNGFAX      = 0x100,
   /** CNG modem calling tone (1300 Hz) receive path. */
   IFX_TAPI_SIG_CNGMODRX    = 0x200,
   /** CNG modem calling tone (1300 Hz) transmit path.  */
   IFX_TAPI_SIG_CNGMODTX    = 0x400,
   /** CNG modem calling tone (1300 Hz) in all paths. */
   IFX_TAPI_SIG_CNGMOD      = 0x800,
   /** Phase reversal detection receive path.
       \remarks Not supported phase reversal uses the same
       paths as CED detection. The detector for CED must also be configured. */
   IFX_TAPI_SIG_PHASEREVRX  = 0x1000,
   /** Phase reversal detection transmit path.
      \remarks Not supported phase reversal uses the same
      paths as CED detection. The detector for CED must also be configured.  */
   IFX_TAPI_SIG_PHASEREVTX  = 0x2000,
   /** Phase reversal detection in all paths.
       \remarks Phase reversals are detected at the
       end of an CED. If this signal is enabled, CED end detection
       is also automatically enabled and reported if it occurs. */
   IFX_TAPI_SIG_PHASEREV    = 0x4000,
   /** Amplitude modulation receive path.
      \remarks Non-supported amplitude modulation uses the same
      paths as CED detection. The detector for CED must also be configured.  */
   IFX_TAPI_SIG_AMRX         = 0x8000,
   /** Amplitude modulation transmit path.
   \remarks Non-supported amplitude modulation uses the same
   paths as CED detection. The detector for CED must also be configured.  */
   IFX_TAPI_SIG_AMTX                 = 0x10000,
   /** Amplitude modulation.

    \remarks Amplitude modulation is detected at the
    end of an CED. If this signal is enabled, CED end detection
    is also automatically enabled and reported if it occurs.

    \note In case of AM detected, the driver automatically switches
    to modem coefficients after end of CED. */
   IFX_TAPI_SIG_AM                   = 0x20000,
   /** Modem tone holding signal stopped receive path.*/
   IFX_TAPI_SIG_TONEHOLDING_ENDRX    = 0x40000,
   /** Modem tone holding signal stopped transmit path. */
   IFX_TAPI_SIG_TONEHOLDING_ENDTX    = 0x80000,
   /** Modem tone holding signal stopped all paths. */
   IFX_TAPI_SIG_TONEHOLDING_END      = 0x100000,
   /** End of signal CED detection receive path.

      \remarks Not supported; CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well.  */
   IFX_TAPI_SIG_CEDENDRX          = 0x200000,
   /** End of signal CED detection transmit path.

      \remarks Not supported; CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well.  */
   IFX_TAPI_SIG_CEDENDTX          = 0x400000,
   /** End of signal CED detection; this signal also includes
      information about phase reversals and amplitude modulation,
      if enabled. */
   IFX_TAPI_SIG_CEDEND            = 0x800000,
   /** Signals a call progress tone detection. This signal is enabled with
       the interface \ref IFX_TAPI_TONE_CPTD_START and stopped with
       \ref IFX_TAPI_TONE_CPTD_STOP. It cannot be activated with
       \ref IFX_TAPI_SIG_DETECT_ENABLE. */
   IFX_TAPI_SIG_CPTD              = 0x1000000,
   /** Signals V8bis detection on the receive path. */
   IFX_TAPI_SIG_V8BISRX           = 0x2000000,
   /** Signals V8bis detection on the transmit path. */
   IFX_TAPI_SIG_V8BISTX           = 0x4000000,
   /** Signals that the caller ID transmission has finished.
       This event cannot be activated with \ref IFX_TAPI_SIG_DETECT_ENABLE;
       use IFX_TAPI_CID_RX_START and IFX_TAPI_CID_RX_STOP instead. */
   IFX_TAPI_SIG_CIDENDTX          = 0x8000000,
   /** Enables DTMF reception on locally connected analog line. */
   IFX_TAPI_SIG_DTMFTX            = 0x10000000,
   /** Enables DTMF reception on remote connected line. */
   IFX_TAPI_SIG_DTMFRX            = 0x20000000
} IFX_TAPI_SIG_t;

/** This service offers extended tone-detection options. */
typedef enum
{
   /** No signal detected. */
   IFX_TAPI_SIG_EXT_NONE          = 0x0,
   /** 980 Hz single tone (V.21L mark sequence) receive path. */
   IFX_TAPI_SIG_EXT_V21LRX        = 0x1,
   /** 980 Hz single tone (V.21L mark sequence) transmit path. */
   IFX_TAPI_SIG_EXT_V21LTX        = 0x2,
   /** 980 Hz single tone (V.21L mark sequence) all paths. */
   IFX_TAPI_SIG_EXT_V21L          = 0x4,
   /** 1400 Hz single tone (V.18A mark sequence) receive path. */
   IFX_TAPI_SIG_EXT_V18ARX        = 0x8,
   /** 1400 Hz single tone (V.18A mark sequence) transmit path. */
   IFX_TAPI_SIG_EXT_V18ATX        = 0x10,
   /** 1400 Hz single tone (V.18A mark sequence) all paths. */
   IFX_TAPI_SIG_EXT_V18A          = 0x20,
   /** 1800 Hz single tone (V.27, V.32 carrier) receive path. */
   IFX_TAPI_SIG_EXT_V27RX         = 0x40,
   /** 1800 Hz single tone (V.27, V.32 carrier) transmit path. */
   IFX_TAPI_SIG_EXT_V27TX         = 0x80,
   /** 1800 Hz single tone (V.27, V.32 carrier) all paths. */
   IFX_TAPI_SIG_EXT_V27           = 0x100,
   /** 2225 Hz single tone (Bell answering tone) receive path. */
   IFX_TAPI_SIG_EXT_BELLRX        = 0x200,
   /** 2225 Hz single tone (Bell answering tone) transmit path. */
   IFX_TAPI_SIG_EXT_BELLTX        = 0x400,
   /** 2225 Hz single tone (Bell answering tone) all paths. */
   IFX_TAPI_SIG_EXT_BELL          = 0x800,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) receive path. */
   IFX_TAPI_SIG_EXT_V22RX         = 0x1000,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) transmit path. */
   IFX_TAPI_SIG_EXT_V22TX         = 0x2000,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) all paths. */
   IFX_TAPI_SIG_EXT_V22           = 0x4000,
   /** 2225 Hz or 2250 Hz single tone; not possible to distinguish receive path.*/
   IFX_TAPI_SIG_EXT_V22ORBELLRX   = 0x8000,
   /** 2225 Hz or 2250 Hz single tone; not possible to distinguish transmit path.*/
   IFX_TAPI_SIG_EXT_V22ORBELLTX   = 0x10000,
   /** 2225 Hz or 2250 Hz single tone; not possible to distinguish all paths. */
   IFX_TAPI_SIG_EXT_V22ORBELL     = 0x20000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) receive path. */
   IFX_TAPI_SIG_EXT_V32ACRX       = 0x40000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) transmit path. */
   IFX_TAPI_SIG_EXT_V32ACTX       = 0x80000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) all paths. */
   IFX_TAPI_SIG_EXT_V32AC         = 0x100000,
   /** 2130 + 2750 Hz dual tone (Bell caller ID type 2 alert tone) receive path. */
   IFX_TAPI_SIG_EXT_CASBELLRX     = 0x200000,
   /** 2130 + 2750 Hz dual tone (Bell caller ID type 2 alert tone) transmit path. */
   IFX_TAPI_SIG_EXT_CASBELLTX     = 0x400000,
   /** 2130 + 2750 Hz dual tone (Bell caller ID type 2 alert tone) all paths. */
   IFX_TAPI_SIG_EXT_CASBELL       = 0x600000,
   /** 1650 Hz single tone (V.21H mark sequence) receive path. */
   IFX_TAPI_SIG_EXT_V21HRX        = 0x800000,
   /** 1650 Hz single tone (V.21H mark sequence) transmit path. */
   IFX_TAPI_SIG_EXT_V21HTX        = 0x1000000,
   /** 1650 Hz single tone (V.21H mark sequence) all paths. */
   IFX_TAPI_SIG_EXT_V21H          = 0x1800000,
   /** Voice modem discriminator all paths. */
   IFX_TAPI_SIG_EXT_VMD           = 0x2000000
} IFX_TAPI_SIG_EXT_t;

/** Lists the RFC 2833 events and ABCD events (RFC 4733 and RFC 5244);
    defined values should not be changed because these are directly passed
    to the firmware. */
typedef enum
{
   /** RFC 2833 event number for DTMF tone 0. */
   IFX_TAPI_PKT_EV_NUM_DTMF_0     = 0,
   /** RFC 2833 event number for DTMF tone 1. */
   IFX_TAPI_PKT_EV_NUM_DTMF_1     = 1,
   /** RFC 2833 event number for DTMF tone 2. */
   IFX_TAPI_PKT_EV_NUM_DTMF_2     = 2,
   /** RFC 2833 event number for DTMF tone 3. */
   IFX_TAPI_PKT_EV_NUM_DTMF_3     = 3,
   /** RFC 2833 event number for DTMF tone 4. */
   IFX_TAPI_PKT_EV_NUM_DTMF_4     = 4,
   /** RFC 2833 event number for DTMF tone 5. */
   IFX_TAPI_PKT_EV_NUM_DTMF_5     = 5,
   /** RFC 2833 event number for DTMF tone 6. */
   IFX_TAPI_PKT_EV_NUM_DTMF_6     = 6,
   /** RFC 2833 event number for DTMF tone 7. */
   IFX_TAPI_PKT_EV_NUM_DTMF_7     = 7,
   /** RFC 2833 event number for DTMF tone 8. */
   IFX_TAPI_PKT_EV_NUM_DTMF_8     = 8,
   /** RFC 2833 event number for DTMF tone 9. */
   IFX_TAPI_PKT_EV_NUM_DTMF_9     = 9,
   /** RFC 2833 event number for DTMF tone *. */
   IFX_TAPI_PKT_EV_NUM_DTMF_STAR  = 10,
   /** RFC 2833 event number for DTMF tone #. */
   IFX_TAPI_PKT_EV_NUM_DTMF_HASH  = 11,
   /** RFC 2833 event number for DTMF tone A. */
   IFX_TAPI_PKT_EV_NUM_DTMF_A     = 12,
   /** RFC 2833 event number for DTMF tone B. */
   IFX_TAPI_PKT_EV_NUM_DTMF_B     = 13,
   /** RFC 2833 event number for DTMF tone C. */
   IFX_TAPI_PKT_EV_NUM_DTMF_C     = 14,
   /** RFC 2833 event number for DTMF tone D. */
   IFX_TAPI_PKT_EV_NUM_DTMF_D     = 15,
   /** RFC 2833 event number for ANS tone. */
   IFX_TAPI_PKT_EV_NUM_ANS        = 32,
   /** RFC 2833 event number for /ANS tone. */
   IFX_TAPI_PKT_EV_NUM_NANS       = 33,
   /** RFC 2833 event number for ANSam tone. */
   IFX_TAPI_PKT_EV_NUM_ANSAM      = 34,
   /** RFC 2833 event number for /ANSam tone. */
   IFX_TAPI_PKT_EV_NUM_NANSAM     = 35,
   /** RFC 2833 event number for CNG tone. */
   IFX_TAPI_PKT_EV_NUM_CNG        = 36,
   /** RFC 2833 event number for DIS signal. */
   IFX_TAPI_PKT_EV_NUM_DIS        = 54,
   /** ABCD signaling event state '0000' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_0 = 144,
   /** ABCD signaling event state '0001' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_1 = 145,
   /** ABCD signaling event state '0010' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_2 = 146,
   /** ABCD signaling event state '0011' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_3 = 147,
   /** ABCD signaling event state '0100' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_4 = 148,
   /** ABCD signaling event state '0101' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_5 = 149,
   /** ABCD signaling event state '0110' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_6 = 150,
   /** ABCD signaling event state '0111' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_7 = 151,
   /** ABCD signaling event state '1000' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_8 = 152,
   /** ABCD signaling event state '1001' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_9 = 153,
   /** ABCD signaling event state '1010' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_10 = 154,
   /** ABCD signaling event state '1011' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_11 = 155,
   /** ABCD signaling event state '1100' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_12 = 156,
   /** ABCD signaling event state '1101' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_13 = 157,
   /** ABCD signaling event state '1110' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_14 = 158,
   /** ABCD signaling event state '1111' (RFC4733).*/
   IFX_TAPI_PKT_EV_NUM_ABCD_STATE_15 = 159,
   /** ABCD signaling event timeout. */
   IFX_TAPI_PKT_EV_NUM_ABCD_TIMEOUT,
   /** No supported RFC event received, or no event received. */
   IFX_TAPI_PKT_EV_NUM_NO_EVENT   = 0xFFFFFFFF
} IFX_TAPI_PKT_EV_NUM_t;

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ======================================================================= */
/* TAPI CID Features Service, enumerations (Group TAPI_INTERFACE_CID)        */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** List of ETSI alerts. */
typedef enum
{
   /** First ring burst;
     Note: this is defined only for CID transmission associated with ringing.*/
   IFX_TAPI_CID_ALERT_ETSI_FR       = 0x0,
   /** DTAS. */
   IFX_TAPI_CID_ALERT_ETSI_DTAS     = 0x1,
   /** Ring pulse. */
   IFX_TAPI_CID_ALERT_ETSI_RP       = 0x2,
   /** Line reversal (alias polarity reversal), followed by DTAS. */
   IFX_TAPI_CID_ALERT_ETSI_LRDTAS   = 0x3
} IFX_TAPI_CID_ALERT_ETSI_t;

/** List of CID standards.*/
typedef enum
{
   /** Bellcore/Telcordia GR-30-CORE; use Bell202 FSK coding of CID
    information.*/
   IFX_TAPI_CID_STD_TELCORDIA    = 0x0,
   /** ETSI 300-659-1/2/3 V1.3.1; use V.23 FSK coding to transmit CID
    information.*/
   IFX_TAPI_CID_STD_ETSI_FSK     = 0x1,
   /** ETSI 300-659-1/2/3 V1.3.1; use DTMF transmission of CID information.*/
   IFX_TAPI_CID_STD_ETSI_DTMF    = 0x2,
   /** SIN 227 Issue 3.4; use V.23 FSK coding of CID information.*/
   IFX_TAPI_CID_STD_SIN          = 0x3,
   /** NTT standard: TELEPHONE SERVICE INTERFACES, edition 5; use a modified
    V.23 FSK coding of CID information.*/
   IFX_TAPI_CID_STD_NTT          = 0x4,
   /** KPN; use DTMF transmission of CID information.*/
   IFX_TAPI_CID_STD_KPN_DTMF     = 0x5,
   /** KPN; use DTMF and FSK transmission of CID information.*/
   IFX_TAPI_CID_STD_KPN_DTMF_FSK = 0x6 ,
   /** Tele-Denmark TS 900 301-1; use DTMF transmission of CID information.*/
   IFX_TAPI_CID_STD_TDC          = 0x7 /* ,
   \todo: Russian caller id
   / ** Russian caller id *  /
   IFX_TAPI_CID_STD_RUSSIAN = 0x7
                              */
} IFX_TAPI_CID_STD_t;

/** Caller ID transmission modes.
 \remarks Information required especially for FSK framing.*/
typedef enum
{
   /** On-hook transmission; applicable to CID type 1 and MWI.*/
   IFX_TAPI_CID_HM_ONHOOK   = 0x00,
   /** Off-hook transmission; applicable to CID type 2 and MWI.*/
   IFX_TAPI_CID_HM_OFFHOOK  = 0x01
} IFX_TAPI_CID_HOOK_MODE_t;

/** Caller ID message type defined in ETSI EN 300 659-3.*/
typedef enum
{
   /** Call setup; corresponds to caller ID type 1 and type 2. */
   IFX_TAPI_CID_MT_CSUP  = 0x80,
   /** Message waiting indicator.*/
   IFX_TAPI_CID_MT_MWI   = 0x82,
   /** Advice of charge.*/
   IFX_TAPI_CID_MT_AOC   = 0x86,
   /** Short message service.*/
   IFX_TAPI_CID_MT_SMS   = 0x89,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES01 = 0xF1,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES02 = 0xF2,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES03 = 0xF3,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES04 = 0xF4,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES05 = 0xF5,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES06 = 0xF6,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES07 = 0xF7,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES08 = 0xF8,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES09 = 0xF9,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0A = 0xFA,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0B = 0xFB,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0C = 0xFC,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0D = 0xFD,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0E = 0xFE,
   /** Reserved for network operator use.*/
   IFX_TAPI_CID_MT_RES0F = 0xFF
} IFX_TAPI_CID_MSG_TYPE_t;

/** Caller ID services (defined in ETSI EN 300 659-3).*/
typedef enum
{
   /** Date and time presentation.*/
   IFX_TAPI_CID_ST_DATE        = 0x01,
   /** Calling line identity (mandatory).*/
   IFX_TAPI_CID_ST_CLI         = 0x02,
   /** Called line identity.*/
   IFX_TAPI_CID_ST_CDLI        = 0x03,
   /** Reason for absence of CLI.*/
   IFX_TAPI_CID_ST_ABSCLI      = 0x04,
   /** Calling line name.*/
   IFX_TAPI_CID_ST_NAME        = 0x07,
   /** Reason for absence of name.*/
   IFX_TAPI_CID_ST_ABSNAME     = 0x08,
   /** Visual indicator.*/
   IFX_TAPI_CID_ST_VISINDIC    = 0x0B,
   /** Message identification.*/
   IFX_TAPI_CID_ST_MSGIDENT    = 0x0D,
   /** Last message CLI.*/
   IFX_TAPI_CID_ST_LMSGCLI     = 0x0E,
   /** Complementary date and time.*/
   IFX_TAPI_CID_ST_CDATE       = 0x0F,
   /** Complementary calling line identity.*/
   IFX_TAPI_CID_ST_CCLI        = 0x10,
   /** Call type.*/
   IFX_TAPI_CID_ST_CT          = 0x11,
   /** First called line identity.*/
   IFX_TAPI_CID_ST_FIRSTCLI    = 0x12,
   /** Number of messages.*/
   IFX_TAPI_CID_ST_MSGNR       = 0x13,
   /** Type of forwarded call.*/
   IFX_TAPI_CID_ST_FWCT        = 0x15,
   /** Type of calling user.*/
   IFX_TAPI_CID_ST_USRT        = 0x16,
   /** Number re-direction.*/
   IFX_TAPI_CID_ST_REDIR       = 0x1A,
   /** Charge.*/
   IFX_TAPI_CID_ST_CHARGE      = 0x20,
   /** Additional charge.*/
   IFX_TAPI_CID_ST_ACHARGE     = 0x21,
   /** Duration of the call.*/
   IFX_TAPI_CID_ST_DURATION    = 0x23,
   /** Network provider ID.*/
   IFX_TAPI_CID_ST_NTID        = 0x30,
   /** Carrier identity.*/
   IFX_TAPI_CID_ST_CARID       = 0x31,
   /** Selection of terminal function.*/
   IFX_TAPI_CID_ST_TERMSEL     = 0x40,
   /** Display information, used as INFO for DTMF.*/
   IFX_TAPI_CID_ST_DISP        = 0x50,
   /** Service information.*/
   IFX_TAPI_CID_ST_SINFO       = 0x55,
   /** Extension for operator use.*/
   IFX_TAPI_CID_ST_XOPUSE      = 0xE0,
   /** Transparent mode.*/
   IFX_TAPI_CID_ST_TRANSPARENT = 0xFF
} IFX_TAPI_CID_SERVICE_TYPE_t;

/** List of VMWI settings.*/
typedef enum
{
   /** Disable VMWI on CPE.*/
   IFX_TAPI_CID_VMWI_DIS = 0x00,
   /** Enable VMWI on CPE.*/
   IFX_TAPI_CID_VMWI_EN  = 0xFF
} IFX_TAPI_CID_VMWI_t;

/** List of ABSCLI/ABSNAME settings.*/
typedef enum
{
   /** Unavailable/unknown.*/
   IFX_TAPI_CID_ABSREASON_UNAV = 0x4F,
   /** Private.*/
   IFX_TAPI_CID_ABSREASON_PRIV = 0x50
} IFX_TAPI_CID_ABSREASON_t;

/** List of NTT reasons for ABSCLI element.*/
typedef enum
{
   /** Private */
   IFX_TAPI_CID_NTT_ABSREASON_PRIV     = 'P',
   /** Unable to provide service */
   IFX_TAPI_CID_NTT_ABSREASON_UNAV     = 'O',
   /** Public telephone originated */
   IFX_TAPI_CID_NTT_ABSREASON_PUBPHONE = 'C',
   /** Service conflict */
   IFX_TAPI_CID_NTT_ABSREASON_CONFLICT = 'S'
} IFX_TAPI_CID_ABSREASON_NTT_t;

/* Definitions for CID receiver. */

/** CID receiver status. */
typedef enum
{
   /** CID receiver is not active.*/
   IFX_TAPI_CID_RX_STATE_INACTIVE   = 0,
   /** CID receiver is active.*/
   IFX_TAPI_CID_RX_STATE_ACTIVE     = 1,
   /** CID receiver is just receiving data.*/
   IFX_TAPI_CID_RX_STATE_ONGOING    = 2,
   /** CID receiver has completed.*/
   IFX_TAPI_CID_RX_STATE_DATA_READY = 3
} IFX_TAPI_CID_RX_STATE_t;

/** CID receiver errors.*/
typedef enum
{
   /** No error during CID receiver operation.*/
   IFX_TAPI_CID_RX_ERROR_NONE = 0,
   /** Reading error during CID receiver operation.*/
   IFX_TAPI_CID_RX_ERROR_READ = 1
} IFX_TAPI_CID_RX_ERROR_t;

/*@}*/ /* TAPI_INTERFACE_CID */

/* ========================================================================= */
/* TAPI Connection Services, enumerations (Group TAPI_INTERFACE_CON)         */
/* ========================================================================= */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Definition of codec algorithms. The enumerated elements should be used
    to select the encoding algorithm and as an array index for the RTP payload
    type configuration.*/
typedef enum
{
   /** Reserved. */
   IFX_TAPI_COD_TYPE_UNKNOWN  = 0,
   /** G723, 6.3 kbit/s. */
   IFX_TAPI_COD_TYPE_G723_63  = 1,
   /** G723, 5.3 kbit/s. */
   IFX_TAPI_COD_TYPE_G723_53  = 2,
   /** G728, 16 kbit/s; not available! */
   IFX_TAPI_COD_TYPE_G728     = 6,
   /** G.729 A and B (silence compression), 8 kbit/s. */
   IFX_TAPI_COD_TYPE_G729     = 7,
   /** G711 u-law, 64 kbit/s. */
   IFX_TAPI_COD_TYPE_MLAW     = 8,
   /** G711 A-Law, 64 kbit/s */
   IFX_TAPI_COD_TYPE_ALAW     = 9,
   /** G.711 u-law, 64 kbit/s; voice band data encoding as defined by V.152. */
   IFX_TAPI_COD_TYPE_MLAW_VBD = 10,
   /** G.711 A-law, 64 kbit/s; voice band data encoding as defined by V.152.*/
   IFX_TAPI_COD_TYPE_ALAW_VBD = 11,
   /** G726, 16 kbit/s. */
   IFX_TAPI_COD_TYPE_G726_16  = 12,
   /** G726, 24 kbit/s. */
   IFX_TAPI_COD_TYPE_G726_24  = 13,
   /** G726, 32 kbit/s. */
   IFX_TAPI_COD_TYPE_G726_32  = 14,
   /** G726, 40 kbit/s .*/
   IFX_TAPI_COD_TYPE_G726_40  = 15,
   /** G729 E, 11.8 kbit/s. */
   IFX_TAPI_COD_TYPE_G729_E   = 16,
   /** iLBC, 13.3 kbit/s. */
   IFX_TAPI_COD_TYPE_ILBC_133 = 17,
   /** iLBC, 15.2 kbit/s. */
   IFX_TAPI_COD_TYPE_ILBC_152 = 18,
   /** Linear codec, 16 bits, 8 kHz. */
   IFX_TAPI_COD_TYPE_LIN16_8  = 19,
   /** Linear codec, 16 bits, 16 kHz. */
   IFX_TAPI_COD_TYPE_LIN16_16 = 20,
   /** AMR, 4.75 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_4_75 = 21,
   /** AMR, 5.15 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_5_15 = 22,
   /** AMR, 5.9 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_5_9  = 23,
   /** AMR, 6.7 kbit/s */
   IFX_TAPI_COD_TYPE_AMR_6_7  = 24,
   /** AMR, 7.4 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_7_4  = 25,
   /** AMR, 7.95 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_7_95 = 26,
   /** AMR, 10.2 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_10_2 = 27,
   /** AMR, 12.2 kbit/s. */
   IFX_TAPI_COD_TYPE_AMR_12_2 = 28,

   /** G.722 (wideband), 64 kbit/s.*/
   IFX_TAPI_COD_TYPE_G722_64  = 31,
   /** G.722.1 (wideband), 24 kbit/s. */
   IFX_TAPI_COD_TYPE_G7221_24 = 32,
   /** G.722.1 (wideband), 32 kbit/s. */
   IFX_TAPI_COD_TYPE_G7221_32 = 33,

   /** Maximum number of codecs. */
   IFX_TAPI_COD_TYPE_MAX
} IFX_TAPI_COD_TYPE_t;

#ifndef TAPI4_DXY_DOC
/** Possible codecs to select for \ref IFX_TAPI_PCK_AAL_PROFILE_t. */
typedef enum
{
   /** G723, 6.3 kbit/s. */
   IFX_TAPI_ENC_TYPE_G723_63  = IFX_TAPI_COD_TYPE_G723_63,
   /** G723, 5.3 kbit/s. */
   IFX_TAPI_ENC_TYPE_G723_53  = IFX_TAPI_COD_TYPE_G723_53,
   /** G728, 16 kbit/s. */
   IFX_TAPI_ENC_TYPE_G728     = IFX_TAPI_COD_TYPE_G728,
   /** G729 A and B, 8 kbit/s. */
   IFX_TAPI_ENC_TYPE_G729     = IFX_TAPI_COD_TYPE_G729,
   /** G711 u-law, 64 kbit/s. */
   IFX_TAPI_ENC_TYPE_MLAW     = IFX_TAPI_COD_TYPE_MLAW,
   /** G711 A-law, 64 kbit/s. */
   IFX_TAPI_ENC_TYPE_ALAW     = IFX_TAPI_COD_TYPE_ALAW,
   /** G711 u-law VBD, 64 kbit/s; voice band data encoding as defined by V.152.*/
   IFX_TAPI_ENC_TYPE_MLAW_VBD = IFX_TAPI_COD_TYPE_MLAW_VBD,
   /** G711 A-law VBD, 64 kbit/s; voice band data encoding as defined by V.152.*/
   IFX_TAPI_ENC_TYPE_ALAW_VBD = IFX_TAPI_COD_TYPE_ALAW_VBD,
   /** G726, 16 kbit/s. */
   IFX_TAPI_ENC_TYPE_G726_16  = IFX_TAPI_COD_TYPE_G726_16,
   /** G726, 24 kbit/s. */
   IFX_TAPI_ENC_TYPE_G726_24  = IFX_TAPI_COD_TYPE_G726_24,
   /** G726, 32 kbit/s. */
   IFX_TAPI_ENC_TYPE_G726_32  = IFX_TAPI_COD_TYPE_G726_32,
   /** G726, 40 kbit/s. */
   IFX_TAPI_ENC_TYPE_G726_40  = IFX_TAPI_COD_TYPE_G726_40,
   /** G729 E, 11.8 kbit/s. */
   IFX_TAPI_ENC_TYPE_G729_E   = IFX_TAPI_COD_TYPE_G729_E,
   /** iLBC, 13.3 kbit/s. */
   IFX_TAPI_ENC_TYPE_ILBC_133 = IFX_TAPI_COD_TYPE_ILBC_133,
   /** iLBC, 15.2 kbit/s. */
   IFX_TAPI_ENC_TYPE_ILBC_152 = IFX_TAPI_COD_TYPE_ILBC_152,
   /** Linear codec, 16 bits 8 kHz. */
   IFX_TAPI_ENC_TYPE_LIN16_8 = IFX_TAPI_COD_TYPE_LIN16_8,
   /** Linear codec, 16 bits, 16 kHz. */
   IFX_TAPI_ENC_TYPE_LIN16_16 = IFX_TAPI_COD_TYPE_LIN16_16,
   /** AMR, 4.75 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_4_75 = IFX_TAPI_COD_TYPE_AMR_4_75,
   /** AMR, 5.15 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_5_15 = IFX_TAPI_COD_TYPE_AMR_5_15,
   /** AMR, 5.9 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_5_9  = IFX_TAPI_COD_TYPE_AMR_5_9,
   /** AMR, 6.7 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_6_7  = IFX_TAPI_COD_TYPE_AMR_6_7,
   /** AMR, 7.4 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_7_4  = IFX_TAPI_COD_TYPE_AMR_7_4,
   /** AMR, 7.95 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_7_95 = IFX_TAPI_COD_TYPE_AMR_7_95,
   /** AMR, 10.2 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_10_2 = IFX_TAPI_COD_TYPE_AMR_10_2,
   /** AMR, 12.2 kbit/s. */
   IFX_TAPI_ENC_TYPE_AMR_12_2 = IFX_TAPI_COD_TYPE_AMR_12_2,
   /** G.722 (wideband), 64 kbit/s. */
   IFX_TAPI_ENC_TYPE_G722_64  = IFX_TAPI_COD_TYPE_G722_64,
   /** G.722.1 (wideband), 24 kbit/s. */
   IFX_TAPI_ENC_TYPE_G7221_24 = IFX_TAPI_COD_TYPE_G7221_24,
   /** G.722.1 (wideband), 32 kbit/s. */
   IFX_TAPI_ENC_TYPE_G7221_32 = IFX_TAPI_COD_TYPE_G7221_32,
   /** Maximum number of codecs, used by \ref IFX_TAPI_PKT_RTP_PT_CFG_t.*/
   IFX_TAPI_ENC_TYPE_MAX
} IFX_TAPI_ENC_TYPE_t;
#endif /* #ifndef TAPI4_DXY_DOC */

/** Packetization length. */
typedef enum
{
   /** Zero packetization length; not supported. */
   IFX_TAPI_COD_LENGTH_ZERO = 0,
   /** 2.5 ms packetization length. */
   IFX_TAPI_COD_LENGTH_2_5  = 1,
   /** 5 ms packetization length. */
   IFX_TAPI_COD_LENGTH_5    = 2,
   /** 5.5 ms packetization length. */
   IFX_TAPI_COD_LENGTH_5_5  = 3,
   /** 10 ms packetization length. */
   IFX_TAPI_COD_LENGTH_10   = 4,
   /** 11 ms packetization length. */
   IFX_TAPI_COD_LENGTH_11   = 5,
   /** 20 ms packetization length. */
   IFX_TAPI_COD_LENGTH_20   = 6,
   /** 30 ms packetization length. */
   IFX_TAPI_COD_LENGTH_30   = 7,
   /** 40 ms packetization length. */
   IFX_TAPI_COD_LENGTH_40   = 8,
   /** 50 ms packetization length.*/
   IFX_TAPI_COD_LENGTH_50   = 9,
   /** 60 ms packetization length. */
   IFX_TAPI_COD_LENGTH_60   = 10
} IFX_TAPI_COD_LENGTH_t;

#if 1 // ctc
typedef enum
{
   /** Zero packetization length. Not supported. */
   IFX_TAPI_ENC_LENGTH_ZERO = IFX_TAPI_COD_LENGTH_ZERO,
   /** Reserved. 2.5 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_2_5  = IFX_TAPI_COD_LENGTH_2_5,
   /** 5 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_5    = IFX_TAPI_COD_LENGTH_5,
   /** 5.5 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_5_5  = IFX_TAPI_COD_LENGTH_5_5,
   /** 10 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_10   = IFX_TAPI_COD_LENGTH_10,
   /** 11 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_11   = IFX_TAPI_COD_LENGTH_11,
   /** 20 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_20   = IFX_TAPI_COD_LENGTH_20,
   /** 30 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_30   = IFX_TAPI_COD_LENGTH_30,
   /** 40 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_40   = IFX_TAPI_COD_LENGTH_40,
   /** Reserved. 50 ms packetization length.*/
   IFX_TAPI_ENC_LENGTH_50   = IFX_TAPI_COD_LENGTH_50,
   /** 60 ms packetization length. */
   IFX_TAPI_ENC_LENGTH_60   = IFX_TAPI_COD_LENGTH_60
} IFX_TAPI_ENC_LENGTH_t;
#endif

/** Possible bit order to select via \ref IFX_TAPI_ENC_CFG_SET and
    \ref IFX_TAPI_DEC_CFG_SET. */
typedef enum
{
   /** Default bit packing/endianness. */
   IFX_TAPI_COD_RTP_BITPACK,
   /** ITU-T I366.2 bit alignment for G.726 codecs (IETF RFC3550). */
   IFX_TAPI_COD_AAL2_BITPACK
} IFX_TAPI_COD_AAL2_BITPACK_t;

/** Enum used to select how to deal with packet loss. */
typedef enum
{
    /** Default; if applicable use codec-specific PLC.
    This is the best setting in case of voice calls. */
    IFX_TAPI_DEC_PLC_CODEC = 0,
    /** Zero insertion: play out zero; may be used
    in case of voice band data transmission. */
    IFX_TAPI_DEC_PLC_ZERO  = 1,
    /** One insertion (linear), corresponding to 0x848 G.711;
    required for G.722 sample over G.711 A-law. */
    IFX_TAPI_DEC_PLC_ONE   = 2
} IFX_TAPI_DEC_PLC_t;

/** Type channel for mapping. */
typedef enum
{
   /** Default; depends on the device and the best applicable is configured. */
   IFX_TAPI_MAP_TYPE_DEFAULT           = 0,
   /** Type is a coder packet channel. */
   IFX_TAPI_MAP_TYPE_CODER             = 1,
   /** Type is a PCM channel. */
   IFX_TAPI_MAP_TYPE_PCM               = 2,
   /** Type is a phone channel. */
   IFX_TAPI_MAP_TYPE_PHONE             = 3,
#ifndef TAPI4_DXY_DOC
   /** Type is a DECT channel. */
   IFX_TAPI_MAP_TYPE_DECT = 4
#endif /* #ifndef TAPI4_DXY_DOC */
} IFX_TAPI_MAP_TYPE_t;

/** Starts/stops information for data channel mapping. */
typedef enum
{
   /** Does not modify the status of the recorder. */
   IFX_TAPI_MAP_DATA_UNCHANGED   = 0,
   /** Recording is started. */
   IFX_TAPI_MAP_DATA_START       = 1,
   /** Recording is stopped. */
   IFX_TAPI_MAP_DATA_STOP        = 2
} IFX_TAPI_MAP_DATA_START_STOP_t;

/** Jitter buffer adaptation. */
typedef enum
{
   /** Local adaptation default. */
   IFX_TAPI_JB_LOCAL_ADAPT_DEFAULT = 0,
   /** Local adaptation ON. */
   IFX_TAPI_JB_LOCAL_ADAPT_ON      = 1,
   /** Local adaptation OFF. */
   IFX_TAPI_JB_LOCAL_ADAPT_OFF     = 2
} IFX_TAPI_JB_LOCAL_ADAPT_t;

#define IFX_TAPI_JB_LOCAL_ADAPT_SI_ON IFX_TAPI_JB_LOCAL_ADAPT_OFF // ctc

/** Enumeration used for the \ref IFX_TAPI_ENC_VAD_CFG_SET ioctl. */
typedef enum
{
   /** No voice activity detection. */
   IFX_TAPI_ENC_VAD_NOVAD     = 0,
   /** Voice activity detection is on; in this case comfort noise and
    spectral information (nicer noise) are also switched on.*/
   IFX_TAPI_ENC_VAD_ON        = 1,
   /** Voice activity detection is on with comfort noise generation, but without
    spectral information. */
   IFX_TAPI_ENC_VAD_G711      = 2,
   /** Voice activity detection is on with comfort noise generation, but without
       silence compression. */
   IFX_TAPI_ENC_VAD_CNG_ONLY  = 3,
   /** Voice activity detection is on with silence compression, but without
       comfort noise generation. */
   IFX_TAPI_ENC_VAD_SC_ONLY   = 4
} IFX_TAPI_ENC_VAD_t;

#ifndef TAPI4_DXY_DOC
/** Used for \ref IFX_TAPI_PCK_AAL_PROFILE_t in case of one coder range. The range
   information is specified as the UUI code-point range in the ATM forum specification: AF-VMOA-0145.000 or ITU-T I.366.2. */
typedef enum
{
   /** One range from 0 to 15. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_15    = 0,
   /** Range from 0 to 7 for a two-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7     = 1,
   /** Range from 8 to 15 for a two-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15    = 2,
   /** Range from 0 to 3 for a four-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_3     = 3,
   /** Range from 4 to 7 for a four-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7     = 4,
   /** Range from 8 to 11 for a four-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11    = 5,
   /** Range from 12 to 15 for a four-range profile entry. */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15   = 6
} IFX_TAPI_PKT_AAL_PROFILE_RANGE_t;
#endif /* #ifndef TAPI4_DXY_DOC */

/** Jitter buffer types. */
typedef enum
{
   /** Fixed jitter buffer. */
   IFX_TAPI_JB_TYPE_FIXED     = 0x1,
   /** Adaptive jitter buffer. */
   IFX_TAPI_JB_TYPE_ADAPTIVE  = 0x2
} IFX_TAPI_JB_TYPE_t;

/** Jitter buffer packet adaptation. */
typedef enum
{
   /** Reserved. */
   IFX_TAPI_JB_PKT_ADAPT_RES1          = 0,
   /** Reserved. */
   IFX_TAPI_JB_PKT_ADAPT_RES2          = 1,
   /** Jitter buffer optimized for voice. */
   IFX_TAPI_JB_PKT_ADAPT_VOICE         = 2,
   /** Jitter buffer optimized for data. */
   IFX_TAPI_JB_PKT_ADAPT_DATA          = 3,
   /** Jitter buffer optimized for data but without doing packet repetition. */
   IFX_TAPI_JB_PKT_ADAPT_DATA_NO_REP   = 4
} IFX_TAPI_JB_PKT_ADAPT_t;

/** Out-of-band or in-band definition. */
typedef enum
{
   /** Device default setting. */
   IFX_TAPI_PKT_EV_OOB_DEFAULT   = 0,
   /** No event packets, DTMF in-band. */
   IFX_TAPI_PKT_EV_OOB_NO        = 1,
   /** Event packets, auto-suppression of DTMF tones.*/
   IFX_TAPI_PKT_EV_OOB_ONLY      = 2,
   /** Event packets, no auto-suppression of DTMF tones. */
   IFX_TAPI_PKT_EV_OOB_ALL       = 3,
   /** Block event transmission: neither in-band nor out-of-band. */
   IFX_TAPI_PKT_EV_OOB_BLOCK     = 4
} IFX_TAPI_PKT_EV_OOB_t;

/** Defines the playout of received RFC 2833 event packets. */
typedef enum
{
   /** Device default setting; not recommended. */
   IFX_TAPI_PKT_EV_OOBPLAY_DEFAULT  = 0,
   /** All RFC 2833 packets coming from the network are played out; upstream and
       downstream RFC 2833 packets have the same payload type. */
   IFX_TAPI_PKT_EV_OOBPLAY_PLAY     = 1,
   /** All RFC 2833 packets coming from the network are muted.*/
   IFX_TAPI_PKT_EV_OOBPLAY_MUTE     = 2,
   /** Alternative payload type (APT); all RFC 2833 packets coming from the network
       are played out; upstream and downstream RFC 2833 packets have
       different payload types. */
   IFX_TAPI_PKT_EV_OOBPLAY_APT_PLAY = 3
}IFX_TAPI_PKT_EV_OOBPLAY_t;

/** Describes the type of packet to configure
    for \ref IFX_TAPI_PKT_NET_CFG_SET for outgoing
    and incoming packets. */
typedef enum
{
   /** Voice (RTP) packets.*/
   Voice,
   /** Fax (T.38) packets. */
   Fax,
   /** Fax trace packets; only used for debug purposes to
       send internal T.38 stack trace states to a specific
       address. */
   FaxTrace
}IFX_TAPI_PKT_NET_TYPE_t;

/** Select the IP protocol type to use for the RTP packet exchange of the
    firmware channel (coder/data pump). */
typedef enum
{
   /** Use IPv4 protocol. */
   IFX_TAPI_PKT_NET_IPV4,
   /** Use IPv6 protocol; not supported yet. */
   IFX_TAPI_PKT_NET_IPV6
}IFX_TAPI_PKT_NET_IP_MODE_t;

/** Start/stop event generation. */
typedef enum
{
   /** Stop event generation. */
   IFX_TAPI_EV_GEN_STOP    = 0,
   /** Start event generation. */
   IFX_TAPI_EV_GEN_START   = 1
} IFX_TAPI_PKT_EV_GEN_ACTION_t;

/** Event volume for RTP event packets. */
typedef enum
{
   /** Use default volume setting. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_DEFAULT  = 0,
   /** Volume: 0 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_0_dB     = 1,
   /** Volume: -1 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_1_dB     = 2,
   /** Volume: -2 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_2_dB     = 3,
   /** Volume: -3 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_3_dB     = 4,
   /** Volume: -4 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_4_dB     = 5,
   /** Volume: -5 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_5_dB     = 6,
   /** Volume: -6 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_6_dB     = 7,
   /** Volume: -7 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_7_dB     = 8,
   /** Volume: -8 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_8_dB     = 9,
   /** Volume: -9 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_9_dB     = 10,
   /** Volume: -10 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_10_dB    = 11,
   /** Volume: -11 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_11_dB    = 12,
   /** Volume: -12 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_12_dB    = 13,
   /** Volume: -13 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_13_dB    = 14,
   /** Volume: -14 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_14_dB    = 15,
   /** Volume: -15 dB */
   IFX_TAPI_PKT_EV_GEN_VOLUME_15_dB    = 16,
   /** Volume: -16 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_16_dB    = 17,
   /** Volume: -17 dB.. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_17_dB    = 18,
   /** Volume: -18 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_18_dB    = 19,
   /** Volume: -19 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_19_dB    = 20,
   /** Volume: -20 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_20_dB    = 21,
   /** Volume: -21 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_21_dB    = 22,
   /** Volume: -22 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_22_dB    = 23,
   /** Volume: -23 dB.. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_23_dB    = 24,
   /** Volume: -24 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_24_dB    = 25,
   /** Volume: -25 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_25_dB    = 26,
   /** Volume: -26 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_26_dB    = 27,
   /** Volume: -27 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_27_dB    = 28,
   /** Volume: -28 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_28_dB    = 29,
   /** Volume: -29 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_29_dB    = 30,
   /** Volume: -30 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_30_dB    = 31,
   /** Volume: -31 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_31_dB    = 32,
   /** Volume: -32 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_32_dB    = 33,
   /** Volume: -33 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_33_dB    = 34,
   /** Volume: -34 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_34_dB    = 35,
   /** Volume: -35 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_35_dB    = 36,
   /** Volume: -36 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_36_dB    = 37,
   /** Volume: -37 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_37_dB    = 38,
   /** Volume: -38 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_38_dB    = 39,
   /** Volume: -39 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_39_dB    = 40,
   /** Volume: -40 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_40_dB    = 41,
   /** Volume: -41 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_41_dB    = 42,
   /** Volume: -42 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_42_dB    = 43,
   /** Volume: -43 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_43_dB    = 44,
   /** Volume: -44 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_44_dB    = 45,
   /** Volume: -45 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_45_dB    = 46,
   /** Volume: -46 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_46_dB    = 47,
   /** Volume: -47 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_47_dB    = 48,
   /** Volume: -48 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_48_dB    = 49,
   /** Volume: -49 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_49_dB    = 50,
   /** Volume: -50 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_50_dB    = 51,
   /** Volume: -51 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_51_dB    = 52,
   /** Volume: -52 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_52_dB    = 53,
   /** Volume: -53 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_53_dB    = 54,
   /** Volume: -54 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_54_dB    = 55,
   /** Volume: -55 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_55_dB    = 56,
   /** Volume: -56 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_56_dB    = 57,
   /** Volume: -57 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_57_dB    = 58,
   /** Volume: -58 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_58_dB    = 59,
   /** Volume: -59 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_59_dB    = 60,
   /** Volume: -60 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_60_dB    = 61,
   /** Volume: -61 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_61_dB    = 62,
   /** Volume: -62 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_62_dB    = 63,
   /** Volume: -63 dB. */
   IFX_TAPI_PKT_EV_GEN_VOLUME_63_dB    = 64
} IFX_TAPI_PKT_EV_GEN_VOLUME_t;

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, enumerations (Group TAPI_INTERFACE_MISC)    */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Debug trace levels. */
typedef enum
{
   /** Report off. */
   IFX_TAPI_DEBUG_REPORT_SET_OFF    = 0,
   /** Low-level report; minor problems. It is recommended that you solve these
      problems. System failure or system crash is not expected. */
   IFX_TAPI_DEBUG_REPORT_SET_LOW    = 1,
   /** Normal level report; problem should be solved. Possible system failure
       or system crash. */
   IFX_TAPI_DEBUG_REPORT_SET_NORMAL = 2,
   /** High-level report; critical problems that should be solved during the
       software integration phase. System failure or system crash expected. */
   IFX_TAPI_DEBUG_REPORT_SET_HIGH   = 3
} IFX_TAPI_DEBUG_REPORT_SET_t;

/** Enumeration used for phone capability types. */
typedef enum
{
   /** Capability type: representation of the vendor. */
   IFX_TAPI_CAP_TYPE_VENDOR      = 0,
   /** Capability type: representation of the underlying device. */
   IFX_TAPI_CAP_TYPE_DEVICE      = 1,
   /** Capability type: information about available ports. */
   IFX_TAPI_CAP_TYPE_PORT        = 2,
   /** Capability type: vocoder type. */
   IFX_TAPI_CAP_TYPE_CODEC       = 3,
   /** Capability type: DSP functionality available. */
   IFX_TAPI_CAP_TYPE_DSP         = 4,
   /** Capability type: number of PCM modules.*/
   IFX_TAPI_CAP_TYPE_PCM         = 5,
   /** Capability type: number of coder modules. */
   IFX_TAPI_CAP_TYPE_CODECS      = 6,
   /** Capability type: number of analog interfaces. */
   IFX_TAPI_CAP_TYPE_PHONES      = 7,
   /** Capability type: number of signaling modules. */
   IFX_TAPI_CAP_TYPE_SIGDETECT   = 8,
   /** Capability type: T.38 support. */
   IFX_TAPI_CAP_TYPE_T38         = 9,
   /** Device version; the version is returned in one integer, where the upper
       byte defines the major and the lower byte the minor version.*/
   IFX_TAPI_CAP_TYPE_DEVVERS     = 10,
   /** Capability type: device type. */
   IFX_TAPI_CAP_TYPE_DEVTYPE     = 11,
   /** Capability type: DECT support. */
   IFX_TAPI_CAP_TYPE_DECT        = 12,
   /** Signal generator. */
   IFX_TAPI_CAP_TYPE_SIGGEN = 13,
   /** LEC. */
   IFX_TAPI_CAP_TYPE_LEC = 14,
   /** Number of peak detectors. */
   IFX_TAPI_CAP_TYPE_PEAKD = 15,
   /** Number of MF R2 detectors. */
   IFX_TAPI_CAP_TYPE_MF_R2 = 16,
   /** Capability type: natively supported FXO lines. */
   IFX_TAPI_CAP_TYPE_FXO = 17
} IFX_TAPI_CAP_TYPE_t;

/** Lists the ports for the capability list. */
typedef enum
{
   /** POTS port available. */
   IFX_TAPI_CAP_PORT_POTS    = 0,
   /** PSTN port available. */
   IFX_TAPI_CAP_PORT_PSTN    = 1,
   /** Handset port available. */
   IFX_TAPI_CAP_PORT_HANDSET = 2,
   /** Speaker port available. */
   IFX_TAPI_CAP_PORT_SPEAKER = 3
} IFX_TAPI_CAP_PORT_t;

/** Lists the signal detectors for the capability list. */
typedef enum
{
   /** Signal detection for CNG is available. */
   IFX_TAPI_CAP_SIG_DETECT_CNG   = 0,
   /** Signal detection for CED is available. */
   IFX_TAPI_CAP_SIG_DETECT_CED   = 1,
   /** Signal detection for DIS is available. */
   IFX_TAPI_CAP_SIG_DETECT_DIS   = 2,
   /** Signal detection for line power is available. */
   IFX_TAPI_CAP_SIG_DETECT_POWER = 3,
   /** Signal detection for CPT is available. */
   IFX_TAPI_CAP_SIG_DETECT_CPT   = 4,
   /** Signal detection for V8.bis is available. */
   IFX_TAPI_CAP_SIG_DETECT_V8BIS = 5
} IFX_TAPI_CAP_SIG_DETECT_t;

/** Defines the device types. */
typedef enum
{
   /** Device not available. */
   IFX_TAPI_DEV_TYPE_NONE = 0,
   /** Device of the DUSLIC-S family (SLICOFI-2S/1S, DUSLIC-2SP/1SP). */
   IFX_TAPI_DEV_TYPE_DUSLIC_S       = 1,
   /** Device of the XWAY DUSLIC-xT family. */
   IFX_TAPI_DEV_TYPE_DUSLIC_XT      = 2,
   /** Device of the XWAY VINETIC family. */
   IFX_TAPI_DEV_TYPE_VINETIC        = 3,
   /** Device of the XWAY INCA-IP family. */
   IFX_TAPI_DEV_TYPE_INCA_IPP       = 4,
   /** Device of the XWAY VINETIC-CPE family. */
   IFX_TAPI_DEV_TYPE_VINETIC_CPE    = 5,
   /** Device of the voice macro type on MIPS
   (e.g. XWAY DANUBE, XWAY TWINPASS-VE, XWAY ARX188). */
   IFX_TAPI_DEV_TYPE_VOICE_MACRO    = 6,
   /** VINETIC-4S device. */
   IFX_TAPI_DEV_TYPE_VINETIC_S      = 7,
   /** Device of the voice gateway type. */
   IFX_TAPI_DEV_TYPE_VOICESUB_GW    = 8,
   /** VINETIC-SVIP device (VINETIC-SVIP16/8/4/0). */
   IFX_TAPI_DEV_TYPE_VIN_SUPERVIP = 9,
   /** Device of the VINETIC-xT16. */
   IFX_TAPI_DEV_TYPE_VIN_XT16 = 10,
   /** Device of the DuSLIC family. */
   IFX_TAPI_DEV_TYPE_DUSLIC  = 11
} IFX_TAPI_DEV_TYPE_t;

/** Firmware module type definition. */
typedef enum
{
   /** Reserved. */
   IFX_TAPI_MODULE_TYPE_NONE = 0x0,
   /** ALM module. */
   IFX_TAPI_MODULE_TYPE_ALM = 0x1,
   /** PCM module. */
   IFX_TAPI_MODULE_TYPE_PCM = 0x2,
   /** COD module (coder module). */
   IFX_TAPI_MODULE_TYPE_COD = 0x3,
   /** Conference module (currently not supported). */
   IFX_TAPI_MODULE_TYPE_CONF = 0x4,
   /** DECT module. */
   IFX_TAPI_MODULE_TYPE_DECT = 0x5,
   /** Applies to all available module types. */
   IFX_TAPI_MODULE_TYPE_ALL = 0xFF
} IFX_TAPI_MODULE_TYPE_t;

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ======================================================================= */
/* TAPI Power Ringing Services, enumerations (Group TAPI_INTERFACE_RINGING)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Ring configuration mode. */
typedef enum
{
   /** Internal balanced. */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_BALANCED        = 0,
   /** Internal unbalanced ROT. */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROT  = 1,
   /** Internal unbalanced ROR. */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROR  = 2,
   /** External SLIC current sense. */
   IFX_TAPI_RING_CFG_MODE_EXTERNAL_IT_CS           = 3,
   /** External IO current sense. */
   IFX_TAPI_RING_CFG_MODE_EXTERNAL_IO_CS           = 4
} IFX_TAPI_RING_CFG_MODE_t;

/** Ring configuration sub-mode. */
typedef enum
{
   /** DC ring trip standard. */
   IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_STANDARD  = 0,
   /** DC ring trip fast. */
   IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_FAST      = 1,
   /** AC ring trip standard. */
   IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_STANDARD  = 2,
   /** AC ring trip fast. */
   IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_FAST      = 3
} IFX_TAPI_RING_CFG_SUBMODE_t;

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ==================================================================== */
/* TAPI PCM Services, enumerations (Group TAPI_INTERFACE_PCM)           */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** Defines the coding for the PCM channel. */
typedef enum
{
   /** G.711 A-law, 8 bits, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_ALAW_8BIT    = 0,
   /** G.711 u-law, 8 bits, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_ULAW_8BIT    = 1,
   /** Linear 16 bits, narrowband.
       This resolution requires 2 consecutive PCM time slots. */
   IFX_TAPI_PCM_RES_NB_LINEAR_16BIT = 2,
   /** G.711 A-law, 8 bits, wideband.
       This resolution requires 2 consecutive PCM time slots. */
   IFX_TAPI_PCM_RES_WB_ALAW_8BIT    = 3,
   /** G.711 u-law, 8 bits, wideband.
       This resolution requires 2 consecutive PCM time slots. */
   IFX_TAPI_PCM_RES_WB_ULAW_8BIT    = 4,
   /** Linear 16 bits, wideband.
       This resolution requires 4 consecutive PCM time slots. */
   IFX_TAPI_PCM_RES_WB_LINEAR_16BIT = 5,
   /** G.722 16 bits, wideband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_WB_G722         = 6,
   /** G.726 16 kbit/s, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_G726_16      = 7,
   /** G.726 24 kbit/s, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_G726_24      = 8,
   /** G.726 32 kbit/s, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_G726_32      = 9,
   /** G.726 40 kbit/s, narrowband.
       This resolution requires 1 PCM time slot. */
   IFX_TAPI_PCM_RES_NB_G726_40      = 10
} IFX_TAPI_PCM_RES_t;

/* Map the old names to the NB names. */
#define IFX_TAPI_PCM_RES_ALAW_8BIT   IFX_TAPI_PCM_RES_NB_ALAW_8BIT
#define IFX_TAPI_PCM_RES_ULAW_8BIT   IFX_TAPI_PCM_RES_NB_ULAW_8BIT
#define IFX_TAPI_PCM_RES_LINEAR_16BIT   IFX_TAPI_PCM_RES_NB_LINEAR_16BIT

/*@}*/ /* TAPI_INTERFACE_PCM */

/* =================================================================== */
/* TAPI Fax T.38 Stack, enumerations (Group TAPI_INTERFACE_FAX_STACK)  */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX_STACK */
/*@{*/

/** T.38 rate management method. */
typedef enum
{
   /** Local TCF; used for TCP and optional for UDP */
   IFX_TAPI_T38_LOC_TCF       = 1,
   /** Transferred TCF; mandatory for UDP, not recommended for TCP */
   IFX_TAPI_T38_TRANS_TCF     = 2,
   /** Both the local and transferred TCF can be used. */
   IFX_TAPI_T38_LOC_TRANS_TCF = 3
}IFX_TAPI_T38_RMM_t;

/** UDP error correction method. */
typedef enum
{
   /** RED (redundancy) will be used during the session. */
   IFX_TAPI_T38_RED = 0,
   /** FEC (forward error correction) will be used during the session. */
   IFX_TAPI_T38_FEC = 1
}IFX_TAPI_T38_EC_MODE_t;

/** UDP error correction method. */
typedef enum
{
   /** RED (redundancy) is supported. */
   IFX_TAPI_T38_CAP_RED       = 1,
   /** FEC (forward error correction) is supported. */
   IFX_TAPI_T38_CAP_FEC       = 2,
   /** Both redundancy and FEC error-correction methods are supported. */
   IFX_TAPI_T38_CAP_RED_FEC   = 3
}IFX_TAPI_T38_EC_CAP_t;

/** T.38 session flags for statistical information. */
typedef enum
{
   /** The remote T.38 peer has used the FEC method in UDP. */
   IFX_TAPI_T38_SESS_FEC      = 1,
   /** The remote T.38 peer has used the redundancy method in UDP. */
   IFX_TAPI_T38_SESS_RED      = 2,
   /** The session has used the ECM mode for facsimile transmission. */
   IFX_TAPI_T38_SESS_ECM      = 4,
   /** The transaction was completed according to T.30. */
   IFX_TAPI_T38_SESS_T30COMPL = 8
}IFX_TAPI_T38_SESS_FLAGS_t;

/** Fax data pump standards used during session; multiple standards can
    be XORed together. */
typedef enum
{
   /** V.27ter 2400 bit/s. */
   IFX_TAPI_T38_FDPSTD_V27_2400  = 0x1,
   /** V.27ter 4800 bit/s. */
   IFX_TAPI_T38_FDPSTD_V27_4800  = 0x2,
   /** V.29 7200 bit/s. */
   IFX_TAPI_T38_FDPSTD_V29_7200  = 0x4,
   /** V.29 7200 bit/s. */
   IFX_TAPI_T38_FDPSTD_V29_9600  = 0x8,
   /** V.17 7200 bit/s. */
   IFX_TAPI_T38_FDPSTD_V17_7200  = 0x10,
   /** V.17 9600 bit/s. */
   IFX_TAPI_T38_FDPSTD_V17_9600  = 0x20,
   /** V.17 12000 bit/s. */
   IFX_TAPI_T38_FDPSTD_V17_12000 = 0x40,
   /** V.17 14400 bit/s. */
   IFX_TAPI_T38_FDPSTD_V17_14400 = 0x80
}IFX_TAPI_T38_FDPSTD_t;

/** State of facsimile transmission for T.38 stack protocol statistics and
    T.38 fax state-change events. */
typedef enum
{
   /** Facsimiles are in the negotiation phase. */
   IFX_TAPI_T38_NEG   = 3,
   /** T.38 starts modulation training. */
   IFX_TAPI_T38_MOD   = 4,
   /** T.38 starts demodulation training. */
   IFX_TAPI_T38_DEM   = 5,
   /** Facsimile page transmission has started. */
   IFX_TAPI_T38_TRANS = 6,
   /** Post-page command is transmitted. */
   IFX_TAPI_T38_PP    = 7,
   /** Facsimile procedure interrupt commands are transmitted. */
   IFX_TAPI_T38_INT   = 8,
   /** Facsimile session finishes with DCN. */
   IFX_TAPI_T38_DCN   = 9
}IFX_TAPI_T38_SESS_STATE_t;

/** T.38 transport protocol. */
typedef enum
{
   /** TCP protocol. */
   IFX_TAPI_T38_TCP     = 1,
   /** UDP protocol. */
   IFX_TAPI_T38_UDP     = 2,
   /** Both the TCP and UDP protocols. */
   IFX_TAPI_T38_TCP_UDP = 3
}IFX_TAPI_T38_PROTOCOL_t;

/** T.38 facsimile image conversion options. */
typedef enum
{
   /** Fill bit removal option. */
   IFX_TAPI_T38_HFBMR   = 0x1,
   /** Transcoding MMR option. */
   IFX_TAPI_T38_HTMMR   = 0x2,
   /** Transcoding JBIG option. */
   IFX_TAPI_T38_TJBIG   = 0x4
}IFX_TAPI_T38_FACSIMILE_CNVT_t;

/** T.38 protocol feature mask. */
typedef enum
{
   /** Non-standard frames are patched; if not set, non-standard
   frames are discarded. */
   IFX_TAPI_T38_FEAT_NON  = 0x02,
   /** ASN.1 notation of 1998 will be used; if not set,
   notation of 2002 will be used. */
   IFX_TAPI_T38_FEAT_ASN1 = 0x04,
   /** Used to combat long round-trip delays. */
   IFX_TAPI_T38_FEAT_LONG = 0x08,
   /** ECM mode of facsimile transmission is disabled. */
   IFX_TAPI_T38_FEAT_ECM  = 0x10
}IFX_TAPI_T38_FEAT_MASK_t;

/** IFP packets send interval used by \ref IFX_TAPI_T38_FDP_CFG_t.
    The firmware sends IFP packets with new data every IFPSI. */
typedef enum
{
   /** 5 ms. */
   IFX_TAPI_T38_IFPSI_MS5  = 5,
   /** 10 ms. */
   IFX_TAPI_T38_IFPSI_MS10 = 10,
   /** 15 ms. */
   IFX_TAPI_T38_IFPSI_MS15 = 15,
   /** 20 ms. */
   IFX_TAPI_T38_IFPSI_MS20 = 20
} IFX_TAPI_T38_IFPSI_t;
/*@}*/ /* TAPI_INTERFACE_FAX_STACK */

/* =================================================================== */
/* TAPI Fax T.38 Services, enumerations (Group TAPI_INTERFACE_FAX)     */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** T38 fax data-pump states. */
typedef enum
{
   /** Fax data pump is not active. */
   IFX_TAPI_FAX_T38_DP_OFF = 0x1,
   /** Fax data pump is active. */
   IFX_TAPI_FAX_T38_DP_ON  = 0x2,
   /** Fax transmission is active. */
   IFX_TAPI_FAX_T38_TX_ON  = 0x4,
   /** Fax transmission is not active. */
   IFX_TAPI_FAX_T38_TX_OFF = 0x8
} IFX_TAPI_FAX_T38_STATUS_t;

/** T38 fax errors. */
typedef enum
{
   /** No error. */
   IFX_TAPI_FAX_T38_ERROR_NONE      = 0,
   /** Error occurred; deactivate data pump. */
   IFX_TAPI_FAX_T38_ERROR_DATAPUMP  = 1,
   /** MIPS overload. */
   IFX_TAPI_FAX_T38_ERROR_MIPS_OVLD = 2,
   /** Error while reading data. */
   IFX_TAPI_FAX_T38_ERROR_READ      = 3,
   /** Error while writing data. */
   IFX_TAPI_FAX_T38_ERROR_WRITE     = 4,
   /** Error while setting up modulator or demodulator. */
   IFX_TAPI_FAX_T38_ERROR_SETUP     = 5
} IFX_TAPI_FAX_T38_ERROR_t;

/** T38 fax standard and rate. */
typedef enum
{
   /** V.21 */
   IFX_TAPI_T38_STD_V21       = 0x1,
   /** V.27/2400 */
   IFX_TAPI_T38_STD_V27_2400  = 0x2,
   /** V.27/4800 */
   IFX_TAPI_T38_STD_V27_4800  = 0x3,
   /** V.29/7200 */
   IFX_TAPI_T38_STD_V29_7200  = 0x4,
   /** V.29/9600 */
   IFX_TAPI_T38_STD_V29_9600  = 0x5,
   /** V.17/7200 */
   IFX_TAPI_T38_STD_V17_7200  = 0x6,
   /** V.17/9600 */
   IFX_TAPI_T38_STD_V17_9600  = 0x7,
   /** V.17/12000 */
   IFX_TAPI_T38_STD_V17_12000 = 0x8,
   /** V.17/14400 */
   IFX_TAPI_T38_STD_V17_14400 = 0x9
} IFX_TAPI_T38_STD_t;

/*@}*/ /* TAPI_INTERFACE_FAX */

#ifndef TAPI4_DXY_DOC
/* ==================================================================== */
/* TAPI DECT Services, enumerations (Group TAPI_INTERFACE_DECT)         */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_DECT */
/*@{*/

/** DECT encoding type. */
typedef enum
{
   IFX_TAPI_DECT_ENC_TYPE_NONE      = 0,
   /** G.711 A-law 64 kbit/s. */
   IFX_TAPI_DECT_ENC_TYPE_G711_ALAW = 1,
   /** G.711 u-law 64 kbit/s. */
   IFX_TAPI_DECT_ENC_TYPE_G711_MLAW = 2,
   /** G.726 32 kbit/s. */
   IFX_TAPI_DECT_ENC_TYPE_G726_32   = 3,
   /** G.722 64 kbit/s (wideband). */
   IFX_TAPI_DECT_ENC_TYPE_G722_64   = 4
} IFX_TAPI_DECT_ENC_TYPE_t;

/** DECT packet sizes. */
typedef enum
{
   /** Packet size 2.5 ms. */
   IFX_TAPI_DECT_ENC_LENGTH_2_5  = 1,
   /** Packet size 5 ms. */
   IFX_TAPI_DECT_ENC_LENGTH_5    = 2,
   /** Packet size 10 ms. */
   IFX_TAPI_DECT_ENC_LENGTH_10   = 3
} IFX_TAPI_DECT_ENC_LENGTH_t;

/*@}*/ /* TAPI_INTERFACE_DECT */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ======================================================================== */
/* TAPI Test Services, enumerations (Group TAPI_INTERFACE_TEST)             */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_TEST */
/*@{*/

/** Specifies the hook event to generate. */
typedef enum
{
   /** Generate an on-hook event. */
   IFX_TAPI_HOOKGEN_ONHOOK = 0,
   /** Generate an off-hook event. */
   IFX_TAPI_HOOKGEN_OFFHOOK = 1
} IFX_TAPI_HOOKGEN_t;

/*@}*/ /* TAPI_INTERFACE_TEST */

#ifndef TAPI4_DXY_DOC
/* ==================================================================== */
/* TAPI GR909 Services, enumerations (Group TAPI_INTERFACE_GR909)       */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_GR909 */
/*@{*/

/** GR909 device type */
typedef enum
{
  /** Product using the voice engine, such as XWAY DANUBE, XWAY ARX188, XWAY GRX188. */
  IFX_TAPI_GR909_DEV_VMMC,
  /** Product of the XWAY VINETIC-CPE, XWAY VINETIC-ATA family. */
  IFX_TAPI_GR909_DEV_VINCPE,
  /** Product of the XWAY DUSLIC-xT family. */
  IFX_TAPI_GR909_DEV_DXT
} IFX_TAPI_GR909_DEV_t;

/** GR909 power-line frequency selection. */
typedef enum
{
   /** EU/Europe-like countries. */
   IFX_TAPI_GR909_EU_50HZ = 0,
   /** US-like countries. */
   IFX_TAPI_GR909_US_60HZ = 1
} IFX_TAPI_GR909_POWERLINE_FREQ_t;

/** GR909 test modes */
typedef enum
{
   /** Hazardous potential test. */
   IFX_TAPI_GR909_HPT  = 0x1,
   /** Foreign electromotive forces test. */
   IFX_TAPI_GR909_FEMF = 0x2,
   /** Resistive faults test. */
   IFX_TAPI_GR909_RFT  = 0x4,
   /** Receiver off-hook test. */
   IFX_TAPI_GR909_ROH  = 0x8,
   /** Ringer impedance test. */
   IFX_TAPI_GR909_RIT  = 0x10
} IFX_TAPI_GR909_TEST_t;

/*@}*/ /* TAPI_INTERFACE_GR909 */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ========================================================================= */
/*                      TAPI Interface Structures                            */
/* ========================================================================= */

/* ======================================================================== */
/* TAPI Initialization Services, structures (Group TAPI_INTERFACE_INIT)     */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** TAPI initialization structure used for \ref IFX_TAPI_CH_INIT. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** TAPI initialization structure used for \ref IFX_TAPI_CH_INIT.

   Channel initialization mode, to be selected from \ref IFX_TAPI_INIT_MODE_t;
   this should be always set to \ref IFX_TAPI_INIT_MODE_VOICE_CODER.*/
   IFX_uint8_t nMode;
   /** Reserved; country selection for future purposes. */
   IFX_uint8_t nCountry;
   /**Pointer to the low-level device initialization structure (for example
      VMMC_IO_INIT for XWAY INCA-IP2). For more details, refer to the device-specific
      driver documentation. */
   IFX_void_t  *pProc;
} IFX_TAPI_CH_INIT_t;

/** Structure used to pass optional parameters with \ref IFX_TAPI_DEV_START. */
typedef struct
{
   /** Device index */
   IFX_uint16_t dev;
   /** Reserved; initialization mode from \ref IFX_TAPI_INIT_MODE_t.
   This should always be set to \ref IFX_TAPI_INIT_MODE_VOICE_CODER. */
   IFX_uint8_t nMode;
} IFX_TAPI_DEV_START_CFG_t;

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================== */
/* TAPI Operation Control Services, structures (Group TAPI_INTERFACE_OP)    */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Structure used to configure volume settings.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Gain setting for the receive path.
       The value is given in dB within the range (-24 dB ... 12 dB), in 1 dB steps.*/
   IFX_int32_t nGainRx;
   /** Gain setting for the transmit path.
       The value is given in dB within the range (-24 dB ... 12 dB), in 1 dB steps. */
   IFX_int32_t nGainTx;
} IFX_TAPI_LINE_VOLUME_t;

/** Structure used to configure gain settings.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Gain setting for the receiving path.
       The value is given in dB within the range (-24 dB ... 24 dB),
       divided by nDiv. */
   IFX_int16_t nGainRx;
   /** Gain setting for the transmitting path.
       The value is given in dB within the range (-24 dB ... 24 dB),
       divided by nDiv. */
   IFX_int16_t nGainTx;
   /** Divider for the gain with multiple of 10. If 1 or 0,
       the gain given in nGainRx and nGainTx is considered as being in dB. Otherwise
       the gain is divided by this divider to yield higher resolutions. */
  IFX_uint16_t nDiv;
} IFX_TAPI_GAIN_t;

/** Packet path volume settings. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Gain setting for the encoding path. The value is given in dB within
       the range (-24 dB ... 12 dB), in 1 dB steps. */
   IFX_int32_t nEnc;
   /** Gain setting for the decoding path. The value is given in dB within
       the range (-24 dB ... 12 dB), in 1 dB steps. */
   IFX_int32_t nDec;
} IFX_TAPI_PKT_VOLUME_t;

/** Structure used for validation times of hook, hook flash and pulse dialing.
    An example of typical timing:
       - 80 ms <= flash time <= 200 ms
       - 30 ms <= digit low time <= 80 ms
       - 30 ms <= digit high time <= 80 ms
       - Interdigit time = 300 ms
       - Off-hook time = 40 ms
       - On-hook time = 400 ms!!! open: only min. time is validated and pre-initialized.
*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Type of validation time setting. */
   IFX_TAPI_LINE_HOOK_VALIDATION_TYPE_t nType;
   /** Minimum time for validation in ms. */
   IFX_uint32_t         nMinTime;
   /** maximum time for validation in ms. */
   IFX_uint32_t         nMaxTime;
} IFX_TAPI_LINE_HOOK_VT_t;

/** Advanced line echo canceller (NLEC/WLEC) configuration. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** LEC type selection.

    - 0x00: IFX_TAPI_WLEC_TYPE_OFF, no LEC.
    - 0x01: IFX_TAPI_WLEC_TYPE_NE, LEC type is NLEC.
    - 0x02: IFX_TAPI_WLEC_TYPE_NFE, LEC type is WLEC.
    - 0x03: IFX_TAPI_WLEC_TYPE_NE_ES, LEC using fixed window + echo suppressor.
    - 0x04: IFX_TAPI_WLEC_TYPE_NFE_ES, LEC using fixed and moving window and
                                       echo suppressor.
    - 0x05: IFX_TAPI_WLEC_TYPE_ES, echo suppressor. */
   IFX_TAPI_WLEC_TYPE_t nType;

   /** Switch the NLP on or off.

    - 1: IFX_TAPI_WLEC_NLP_ON, NLP is on
    - 2: IFX_TAPI_WLEC_NLP_OFF, NLP is off   */
   IFX_TAPI_WLEC_NLP_t bNlp;

   /** Size of the fixed window in narrowband (8 kHz) sampling mode.
       For backward compatibility, a value of 0 defaults to:
        16 ms if nType is \ref IFX_TAPI_WLEC_TYPE_NE, or
         8 ms if nType is \ref IFX_TAPI_WLEC_TYPE_NFE, */
   IFX_TAPI_WLEC_WIN_SIZE_t   nNBNEwindow;

   /** Size of the moving window in narrowband (8 kHz) sampling mode;
       a value of 0 defaults to 8 ms for backward compatibility.
       Note: this is used only if nType is set to \ref IFX_TAPI_WLEC_TYPE_NFE. */
   IFX_TAPI_WLEC_WIN_SIZE_t   nNBFEwindow;

   /** Size of the fixed window in wideband (16 kHz) sampling mode.
       For backward compatibility, a value of 0 defaults to 8 ms. */
   IFX_TAPI_WLEC_WIN_SIZE_t   nWBNEwindow;
} IFX_TAPI_WLEC_CFG_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_LINE_HOOK_STATUS_GET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** This service reads the current hook state. */
      IFX_TAPI_LINE_HOOK_t hookMode;
   } IFX_TAPI_LINE_HOOK_STATUS_GET_t;
#else /* TAPI_ONE_DEVNODE */
   /** type used by \ref IFX_TAPI_LINE_HOOK_STATUS_GET. */
   typedef IFX_boolean_t IFX_TAPI_LINE_HOOK_STATUS_GET_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used to enable or disable a high-level path of a phone channel.
       The high-level path might be required to play howler tones. The structure is
       used by \ref IFX_TAPI_LINE_LEVEL_SET.
   */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Specifies the line high-level path. */
      IFX_TAPI_LINE_LEVEL_t level;
   }IFX_TAPI_LINE_LEVEL_CFG_t;
#else /* TAPI_ONE_DEVNODE */
   /** Type used to enable or disable a high-level path of a phone channel.
       The high-level path might be required to play howler tones. The structure is
       used by \ref IFX_TAPI_LINE_LEVEL_SET.
   */
   typedef IFX_TAPI_LINE_LEVEL_t IFX_TAPI_LINE_LEVEL_CFG_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_OP */

/* ===================================================================== */
/* TAPI Metering Services, structures (Group TAPI_INTERFACE_METER)       */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** Structure for configuration of metering
    used by \ref IFX_TAPI_METER_CFG_SET.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Metering mode:
   - 0: TAPI_METER_MODE_TTX, TTX mode
   - 1: IFX_TAPI_METER_MODE_REVPOL, reverse polarity */
   IFX_TAPI_METER_MODE_t    bMode;
   /** Length of metering pulse (in ms).
       'nPulseLen' must be greater than zero.*/
   IFX_uint32_t             nPulseLen;
   /** Length of pause between two metering pulses (in ms).
       'nPauseLen' must be greater than zero.*/
   IFX_uint32_t             nPauseLen;
} IFX_TAPI_METER_CFG_t;

/** Structure used by \ref IFX_TAPI_METER_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Distance between the metering bursts (in seconds). */
   IFX_uint32_t nPulseDist;
   /** Defines the number of pulses. */
   IFX_uint32_t nPulses;
} IFX_TAPI_METER_START_t;

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_METER_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_METER_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure used by \ref IFX_TAPI_METER_BURST. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Defines the number of pulses in a burst. */
   IFX_uint32_t nPulses;
} IFX_TAPI_METER_BURST_t;

/** Structure used by \ref IFX_TAPI_METER_STATISTICS_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Number of pulse bursts that are requested by the periodic timer or
       application intermediate burst and which are not generated yet.
       These will be generated by the firmware. */
   IFX_uint32_t nPulseOutstanding;
   /** Number of bursts that are transmitted since the last statistic call. */
   IFX_uint32_t nBurstTransmitted;
} IFX_TAPI_METER_STATISTICS_t;

/*@}*/ /* TAPI_INTERFACE_METER */

/* ==================================================================== */
/* TAPI Tone Control Services, structures (Group TAPI_INTERFACE_TONE)   */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Structure used for tone-level configuration. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Defines the tone generator number whose level has to be modified.
   - 0x1: IFX_TAPI_TONE_TG1, generator one is used
   - 0x2: IFX_TAPI_TONE_TG2, generator two is used
   - 0xFF: IFX_TAPI_TONE_TGALL, all tone generators are affected */
   IFX_uint8_t      nGenerator;
   /** Defines the tone level; value 0x100 describes the default record
   level of -38.79 dB. The maximum value is 0x7FFF, which corresponds to a
   record level of approximately 3 dB.
   The tone level in dB can be calculated using the formula:
   level_in_dB = +3.17 + 20 log10 (level / 32767). */
   IFX_uint32_t      nLevel;
} IFX_TAPI_PREDEF_TONE_LEVEL_t;

/** Structure used to define simple tone characteristics. */
typedef struct
{
    /** Indicates the type of the tone descriptor:

    - 1: IFX_TAPI_TONE_TYPE_SIMPLE, the tone descriptor describes a simple tone
    - 2: IFX_TAPI_TONE_TYPE_COMPOSED, the tone descriptor describes a
         composed tone */
    IFX_TAPI_TONE_TYPE_t    format;
    /** Name of the simple tone. */
    IFX_char_t    name[30];
    /** Tone code ID; 0 < ID < 255. */
    IFX_uint32_t  index;
    /** Number of times to play the simple tone, 0 < loop < 8.
        The loop count, if not equal to 0, defines the time of the entire sequence:
        tone_seq = loop * (onA + offA + onB + offB + onC + offC + pause). */
    IFX_uint32_t  loop;
     /** Power level for frequency A in 0.1 dB steps; -300 < levelA < 0. */
    IFX_int32_t   levelA;
     /** Power level for frequency B in 0.1 dB steps; -300 < levelB < 0.*/
    IFX_int32_t   levelB;
     /** Power level for frequency C in 0.1 dB steps; -300 < levelC < 0. */
    IFX_int32_t   levelC;
    /** Power level for frequency D in 0.1 dB steps; -300 < levelD < 0. */
    IFX_int32_t   levelD;
    /** Tone frequency A in Hz; 0 <= Hz < 4000. */
    IFX_uint32_t  freqA;
    /** Tone frequency B in Hz; 0 <= Hz < 4000. */
    IFX_uint32_t  freqB;
    /** Tone frequency C in Hz; 0 <= Hz < 4000. */
    IFX_uint32_t  freqC;
    /** Tone frequency D in Hz; 0 <= Hz < 4000. */
    IFX_uint32_t  freqD;
    /**
      IFX_TAPI_TONE_STEPS_MAX array defining time duration
      for each cadence step, with 1 ms granularity.
      0 <= cadence <= 16383.
      The first cadence[X] = 0 (starting from X = 1) in the array indicates that
      X-1 cadences must be played.
      A tone with cadence[0]=0 cannot be processed! */
    IFX_uint32_t  cadence[IFX_TAPI_TONE_STEPS_MAX];
    /** Active frequencies for the cadence steps; more than one frequency
      can be active in the same cadence step. All active frequencies are
      summed together.

      - 0x0:  IFX_TAPI_TONE_FREQNONE, no frequencies
      - 0x01: IFX_TAPI_TONE_FREQA, frequency A is enabled
      - 0x02: IFX_TAPI_TONE_FREQB, frequency B is enabled
      - 0x04: IFX_TAPI_TONE_FREQC, frequency C is enabled
      - 0x08: IFX_TAPI_TONE_FREQD, frequency D is enabled
      - 0x0F: IFX_TAPI_TONE_FREQALL, all frequencies are enabled */
    IFX_uint32_t  frequencies[IFX_TAPI_TONE_STEPS_MAX];

    /** Array specifying, for each cadence step, whether to
      enable/disable the modulation of frequency A with frequency B.
      Refer to \ref IFX_TAPI_TONE_MODULATION_t enum values.
    */
    IFX_uint32_t  modulation[IFX_TAPI_TONE_STEPS_MAX];

    /** Modulation factor to be applied to cadence steps which activate
        modulation. Refer to \ref IFX_TAPI_TONE_MODULATION_FACTOR_t
        enum values.
    */
    IFX_uint32_t  modulation_factor;
    /** Some tones require an off-time at the end of the tone. The off-time
      is added to the last used cadence. Therefore, the off-time has a maximum value of
      32000 - 'last used cadence' and a granularity of 2 ms;
      0 < 32000 - 'last used cadence' < 32000.*/
    IFX_uint32_t  pause;
} IFX_TAPI_TONE_SIMPLE_t;

/** Structure used for definition of composed tones. */
typedef struct
{
    /** Indicate the type of the tone descriptor:

    - 1: IFX_TAPI_TONE_TYPE_SIMPLE, the tone descriptor describes a simple tone
    - 2: IFX_TAPI_TONE_TYPE_COMPOSED, the tone descriptor describes a composed
        tone */
    IFX_TAPI_TONE_TYPE_t    format;
    /** Name of the composed tone. */
    IFX_char_t    name[30];
    /** Tone code ID; 0< ID <255.*/
    IFX_uint32_t  index;
    /** Number of times to play the tone sequence: 0 for infinite,
        maximum 7. */
    IFX_uint32_t  loop;
    /** Indicate whether the voice path is active between the loops.*/
    IFX_uint32_t  alternatVoicePath;
    /** Number of simple tones used in the composed tone. */
    IFX_uint32_t  count;
    /** Simple tones to be used; the simple tones are played in the same order
        as that in which they are stored in the array.
        Note: in order to create composed tones, only simple tones with a finite
        loop count can be used.*/
    IFX_uint32_t  tones [IFX_TAPI_TONE_SIMPLE_MAX];
} IFX_TAPI_TONE_COMPOSED_t;

#ifndef TAPI_DXY_DOC
/** Structure for definition of dual tones. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   IFX_TAPI_TONE_TYPE_t    format;

   IFX_int32_t    levelA;
   IFX_int32_t    levelB;

   IFX_uint32_t   freqA;
   IFX_uint32_t   freqB;
} IFX_TAPI_TONE_DUAL_t;
#endif /* TAPI_DXY_DOC */

/** Tone descriptor. */
typedef union
{
   /** Pointer to an \ref IFX_TAPI_TONE_SIMPLE_t structure.*/
   IFX_TAPI_TONE_SIMPLE_t   simple;
   /**  Pointer to an \ref IFX_TAPI_TONE_COMPOSED_t structure.*/
   IFX_TAPI_TONE_COMPOSED_t composed;
#ifndef TAPI_DXY_DOC
   /** Descriptor for dual tone. */
   IFX_TAPI_TONE_DUAL_t     dual;
#endif /* TAPI_DXY_DOC */
} IFX_TAPI_TONE_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_TONE_BUSY_PLAY,
       \ref IFX_TAPI_TONE_RINGBACK_PLAY and  \ref IFX_TAPI_TONE_DIALTONE_PLAY. */
   struct IFX_TAPI_PREDEFINED_TONE_s
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
   };

   /** Structure used by \ref IFX_TAPI_TONE_BUSY_PLAY. */
   typedef struct IFX_TAPI_PREDEFINED_TONE_s IFX_TAPI_TONE_BUSY_t;

   /** Structure used by \ref IFX_TAPI_TONE_RINGBACK_PLAY. */
   typedef struct IFX_TAPI_PREDEFINED_TONE_s IFX_TAPI_TONE_RINGBACK_t;

   /** Structure used by \ref IFX_TAPI_TONE_DIALTONE_PLAY. */
   typedef struct IFX_TAPI_PREDEFINED_TONE_s IFX_TAPI_TONE_DIALTONE_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_TONE_LOCAL_PLAY. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** The parameter is the index of the tone to the pre-defined tone
         table (range 1 - 31) or custom tones added previously
         (index 32 - 255). Index 0 means tone stop. Using the upper bits
         modifies the default tone playing source. */
      IFX_int32_t nToneIndex;
   }IFX_TAPI_TONE_IDX_t;
#else /* TAPI_ONE_DEVNODE */
   /** The parameter is the index of the tone to the pre-defined tone
      table (range 1 - 31) or custom tones added previously
      (index 32 - 255). Index 0 means tone stop. Using the upper bits
      modifies the default tone playing source. */
   typedef IFX_int32_t IFX_TAPI_TONE_IDX_t;
#endif /* TAPI_ONE_DEVNODE */

/** MF R2 detector configuration structure.
    The structure is used by \ref IFX_TAPI_TONE_MF_R2_START and
    \ref IFX_TAPI_TONE_MF_R2_STOP. */
typedef struct
{
   IFX_uint16_t dev;
   IFX_uint16_t ch;
   IFX_TAPI_MODULE_TYPE_t module;
   IFX_uint32_t external:1;
   IFX_uint32_t internal:1;
   IFX_uint32_t backward:1;
   IFX_uint32_t reserved:29;
} IFX_TAPI_TONE_MF_R2_CFG_t;

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ======================================================================== */
/* TAPI Signal Detection Services, structures                               */
/* (Group TAPI_INTERFACE_SIGNAL)                                            */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

#ifndef TAPI4_DXY_DOC
/** Structure used to enable and disable signal detection; see
   \ref IFX_TAPI_SIG_DETECT_ENABLE and \ref IFX_TAPI_SIG_DETECT_DISABLE.
   The signals are reported via a TAPI exception service. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
#ifdef TAPI_VERSION4
   /** Firmware module to play the configuration. */
   IFX_TAPI_MODULE_TYPE_t module;
#endif /* TAPI_VERSION4 */
   /** Signals to detect; can be any combination of \ref IFX_TAPI_SIG_t. */
   IFX_uint32_t sig;
   /** Signals to detect; can be any combination of \ref IFX_TAPI_SIG_EXT_t. */
   IFX_uint32_t sig_ext;
} IFX_TAPI_SIG_DETECTION_t;
#endif /* #ifndef TAPI4_DXY_DOC */

/** DTMF receiver coefficients settings.
    The following DTMF receiver coefficients are to be directly programmed
    in the underlying device. Therefore, the passed values must be expressed
    in a format ready for programming, as no interpretation of these values is
    attempted.
 */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
#ifdef TAPI_VERSION4
   /** Firmware module to play the configuration. */
   IFX_TAPI_MODULE_TYPE_t module;
#endif /* TAPI_VERSION4 */
   /** Minimal signal level in dB. */
   IFX_int32_t nLevel;
   /** Maximum allowed signal twist in dB. */
   IFX_int32_t nTwist;
   /** Gain adjustment of the input signal in dB. */
   IFX_int32_t nGain;
   /** Delay inserted in the voice path (in ms). It is possible to insert a delay of up to
       20 ms; the default is 0 ms. A delay in the voice path facilitates a
       better/total suppression of the DTMF tone (if the DTMF auto-suppression
       feature is used).
       Note: this option should be used with caution since the overall
       group delay will increase by the same quantity. */
   IFX_uint8_t nVoicePathDelay;
} IFX_TAPI_DTMF_RX_CFG_t;

#ifdef TAPI_VERSION3
/** Specifies the CPT signal for CPT detection. */
typedef enum
{
   /** Receive direction. */
   IFX_TAPI_TONE_CPTD_DIRECTION_RX = 0x1,
   /** Transmit direction. */
   IFX_TAPI_TONE_CPTD_DIRECTION_TX = 0x2
} IFX_TAPI_TONE_CPTD_DIRECTION_t;
#endif /* TAPI_VERSION3 */

/** Structure used for \ref IFX_TAPI_TONE_CPTD_START and
    \ref IFX_TAPI_TONE_CPTD_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
#ifdef TAPI_VERSION4
   /** Firmware module where to play the tone. */
   IFX_TAPI_MODULE_TYPE_t module;
   /** Tone detection from external
       (e.g. analog line, PCM bus, RTP in-band, etc.). */
   IFX_uint32_t external:1;
   /** Tone detection from internal (connected firmware module). */
   IFX_uint32_t internal:1;
   /** Reserved. */
   IFX_uint32_t reserved:30;
#endif /* TAPI_VERSION4 */
#ifdef TAPI_VERSION3
   /** Specification of the signal. */
   IFX_TAPI_TONE_CPTD_DIRECTION_t signal;
#endif /* TAPI_VERSION3 */
   /** The index of the tone to detect; the tone index must be programmed
   with IFX_TAPI_TONE_TABLE_CFG_SET. */
   IFX_int32_t tone;
} IFX_TAPI_TONE_CPTD_t;

/** This structure is used by the IFX_TAPI_TONE_PLAY and IFX_TAPI_TONE_STOP
    commands to enable/disable tone playout.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Tone index to enable. To disable, the index is set to zero. */
   IFX_uint32_t index;
   /** Firmware module on which to play the tone */
   IFX_TAPI_MODULE_TYPE_t module;
   /** Tone generation towards external
       (e.g. analog line, PCM bus, RTP in-band, etc.). */
   IFX_uint16_t external:1;
   /** Tone generation towards internal (connected firmware module). */
   IFX_uint16_t internal:1;
   /** Reserved. */
   IFX_uint16_t reserved:14;
} IFX_TAPI_TONE_PLAY_t;

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ==================================================================== */
/* TAPI CID Features Service, structures (Group TAPI_INTERFACE_CID)       */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** Structure containing the timing for CID transmission. */
typedef struct
{
   /** Time to wait before data transmission, in ms;
      default is 300 ms. */
   IFX_uint32_t beforeData;
   /** Time to wait after data transmission, in ms, for on-hook services;
      default is 300 ms. */
   IFX_uint32_t dataOut2restoreTimeOnhook;
   /** Time to wait after data transmission, in ms, for off-hook services;
      default is 60 ms. */
   IFX_uint32_t dataOut2restoreTimeOffhook;
   /** Time to wait after ACK detection, in ms;
      default is 55 ms. */
   IFX_uint32_t ack2dataOutTime;
   /** Time-out for ACK detection, in ms;
      default is 200 ms. */
   IFX_uint32_t cas2ackTime;
   /** Time to wait after ACK time-out, in ms;
      default is 0 ms. */
   IFX_uint32_t afterAckTimeout;
   /** Time to wait after the first ring, in ms, typically before data transmission;
      default is 600 ms. */
   IFX_uint32_t afterFirstRing;
   /** Time to wait after ring pulse, in ms, typically before data transmission;
      default is 500 ms. */
   IFX_uint32_t afterRingPulse;
   /** Time to wait after DTAS, in ms, typically before data transmission;
      default is 45 ms. */
   IFX_uint32_t afterDTASOnhook;
   /** Time to wait after line reversal in ms, typically before data transmission;
      default is 100 ms. */
   IFX_uint32_t afterLineReversal;
   /** Time to wait after OSI signal, in ms;
      default is 300 ms. */
   IFX_uint32_t afterOSI;
} IFX_TAPI_CID_TIMING_t;

/** Structure containing the configuration information for the FSK transmitter and
   receiver. */
typedef struct
{
   /** Signal level for FSK transmission in 0.1 dB steps; default is -140
    (-14 dB).*/
   IFX_int32_t    levelTX;
   /** Minimum signal level for FSK reception in 0.1 dB steps; default is -150
    (-15 dB).*/
   IFX_int32_t    levelRX;
   /** Number of seizure bits for FSK transmission; relevant only
   for on-hook transmission. Default values are:
   - NTT standard: 0 bits
   - Other standards: 300 bits*/
   IFX_uint32_t   seizureTX;
   /** Minimum number of seizure bits for FSK reception; relevant
   only for on-hook transmission. Default values are:
   - NTT standard: 0 bits
   - Other standards: 200 bits*/
   IFX_uint32_t   seizureRX;
   /** Number of mark bits for on-hook FSK transmission; default values are:
   - NTT standard: 72 bits
   - Other standards: 180 bits */
   IFX_uint32_t   markTXOnhook;
   /** Number of mark bits for off-hook FSK transmission; default values are:
   - NTT standard: 72 bits
   - Other standards: 80 bits */
   IFX_uint32_t   markTXOffhook;
   /** Minimum number of mark bits for on-hook FSK reception; default values are:
   - NTT standard: 50 bits
   - Other standards: 150 bits */
   IFX_uint32_t   markRXOnhook;
   /** Minimum number of mark bits for off-hook FSK reception; default values are:
   - NTT standard: 50 bits
   - Other standards: 55 bits */
   IFX_uint32_t   markRXOffhook;
   /** Number of additional stop (mark) bits for on-hook FSK transmission.
   The ETSI standard states that one to ten stop bits shall be sent after the
   checksum to avoid corruption of the checksum.
   A value of 0 in this field defaults to 1 additional stop bit being sent. */
   IFX_uint32_t   stopTXOnhook;
   /** Number of additional stop (mark) bits for off-hook FSK transmission.
   The ETSI standard states that one to ten stop bits shall be sent after the
   checksum to avoid corruption of the checksum.
   A value of 0 in this field defaults to 1 additional stop bit being sent. */
   IFX_uint32_t   stopTXOffhook;
} IFX_TAPI_CID_FSK_CFG_t;

/** Structure containing the configuration information for the DTMF CID. */
typedef struct
{
   /** Tone ID for starting tone; default is DTMF A. */
   IFX_int8_t     startTone;
   /** Tone ID for stop tone; default is DTMF C. */
   IFX_int8_t     stopTone;
   /** Tone ID for starting information tone; default is DTMF B. */
   IFX_int8_t     infoStartTone;
   /** Tone ID for starting redirection tone; default is DTMF D. */
   IFX_int8_t     redirStartTone;
   /** Time for DTMF digit duration; default is 50 ms. */
   IFX_uint32_t   digitTime;
   /** Time between DTMF digits in ms; default is 50 ms. */
   IFX_uint32_t   interDigitTime;
} IFX_TAPI_CID_DTMF_CFG_t;

/** Structure containing CID configuration for the ETSI standard using DTMF
 transmission.*/
typedef struct
{
   /** Length of the coded strings.*/
   IFX_uint32_t   len;
   /** String representing code for unavailable/unknown CLI; default 00.*/
   IFX_uint8_t    unavailable[IFX_TAPI_CID_MSG_LEN_MAX];
   /** String representing code for private/withheld CLI; default 01.*/
   IFX_uint8_t    priv[IFX_TAPI_CID_MSG_LEN_MAX];
} IFX_TAPI_CID_ABS_REASON_t;

/** Structure containing CID configuration for the Telcordia standard. */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, \ref IFX_TAPI_CID_TIMING_t default values will be used. */
   IFX_TAPI_CID_TIMING_t   *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_FSK_CFG_t default values will be used.*/
   IFX_TAPI_CID_FSK_CFG_t  *pFSKConf;
   /** Use of OSI for off-hook transmission; default IFX_FALSE.*/
   IFX_uint32_t            OSIoffhook;
   /** Length of the OSI signal in ms; default 200 ms.*/
   IFX_uint32_t            OSItime;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOnhook;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOffhook;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF 'D'. */
   IFX_char_t              ackTone;
   /** Tone table index for the subscriber alerting signal (SAS) to be used
       in off-hook transmissions. By default, the TAPI plays no SAS tone */
   IFX_uint32_t            nSAStone;
   /** Time to wait before a SAS tone, in ms, for off-hook services.
       The default is 20 ms when OSI is not used, and 100 ms when OSI is used. */
   IFX_uint32_t            beforeSAStime;
   /** Time to wait between generation of SAS and CAS tone, in ms;
       default 20 ms. */
   IFX_uint32_t            SAS2CAStime;
} IFX_TAPI_CID_STD_TELCORDIA_t;

/** Structure containing CID configuration for the ETSI standard using FSK
    transmission. */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, \ref IFX_TAPI_CID_TIMING_t default values will be used.*/
   IFX_TAPI_CID_TIMING_t         *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_FSK_CFG_t default values will be used.*/
   IFX_TAPI_CID_FSK_CFG_t        *pFSKConf;
   /** Type of ETSI alert of on-hook services associated with ringing
    (enumerated in IFX_TAPI_CID_ALERT_ETSI_t); default
    IFX_TAPI_CID_ALERT_ETSI_FR.*/
   IFX_TAPI_CID_ALERT_ETSI_t     nETSIAlertRing;
   /** Type of ETSI alert of on-hook services not associated with ringing
    (enumerated in IFX_TAPI_CID_ALERT_ETSI_t); default
     IFX_TAPI_CID_ALERT_ETSI_RP.*/
   IFX_TAPI_CID_ALERT_ETSI_t     nETSIAlertNoRing;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t                  nAlertToneOnhook;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t                  nAlertToneOffhook;
   /** Duration of ring pulse, in ms; default 500 ms.*/
   IFX_uint32_t                  ringPulseTime;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF is D.*/
   IFX_char_t                    ackTone;
} IFX_TAPI_CID_STD_ETSI_FSK_t;

/** Structure containing CID configuration for the ETSI standard using DTMF
 transmission. */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, \ref IFX_TAPI_CID_TIMING_t default values will be used. */
   IFX_TAPI_CID_TIMING_t       *pCIDTiming;
   /** Pointer to a structure containing DTMF configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_DTMF_CFG_t default values will be
   used. */
   IFX_TAPI_CID_DTMF_CFG_t     *pDTMFConf;
   /** Pointer to a structure containing the coding for the
   absence reason of the calling number.*/
   IFX_TAPI_CID_ABS_REASON_t   *pABSCLICode;
   /** Type of ETSI alert of on-hook services associated with ringing (enumerated
    in \ref IFX_TAPI_CID_ALERT_ETSI_t); default \ref IFX_TAPI_CID_ALERT_ETSI_FR.*/
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertRing;
   /** Type of ETSI alert of on-hook services not associated with ringing
    (enumerated in \ref IFX_TAPI_CID_ALERT_ETSI_t); default \ref
    IFX_TAPI_CID_ALERT_ETSI_RP. */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertNoRing;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t                nAlertToneOnhook;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition. */
   IFX_uint32_t                nAlertToneOffhook;
   /** Duration of ring pulse, in ms; default is 500 ms. */
   IFX_uint32_t                ringPulseTime;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF D. */
   IFX_char_t                  ackTone;
} IFX_TAPI_CID_STD_ETSI_DTMF_t;

/** Structure for the configuration of the SIN standard. */
typedef struct
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t   *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_FSK_CFG_t  *pFSKConf;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOnhook;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOffhook;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF 'D'. */
   IFX_char_t              ackTone;
} IFX_TAPI_CID_STD_SIN_t;

/** Structure containing the CID configuration for the NTT standard.*/
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, \ref IFX_TAPI_CID_TIMING_t default values will be used.*/
   IFX_TAPI_CID_TIMING_t   *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_FSK_CFG_t default values will be used.*/
   IFX_TAPI_CID_FSK_CFG_t  *pFSKConf;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOnhook;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t            nAlertToneOffhook;
   /** Ring pulse on time (CAR signal), in ms; default 500 ms.*/
   IFX_uint32_t            ringPulseTime;
   /** Maximum number of ring pulses (CAR signals); default 5.*/
   IFX_uint32_t            ringPulseLoop;
   /** Ring pulse off-time (CAR signal), in ms; default 500 ms.*/
   IFX_uint32_t            ringPulseOffTime;
   /** Time-out for incoming successful signal to arrive after CID data
    transmission is completed; default 7000 ms.*/
   IFX_uint32_t            dataOut2incomingSuccessfulTimeout;
} IFX_TAPI_CID_STD_NTT_t;

/** Structure containing CID configuration for the KPN standard, with DTMF
    and FSK transmission */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, IFX_TAPI_CID_TIMING_t default values will be used.*/
   IFX_TAPI_CID_TIMING_t         *pCIDTiming;
   /** Pointer to a structure containing DTMF configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_DTMF_CFG_t default values will be
   used. */
   IFX_TAPI_CID_DTMF_CFG_t       *pDTMFConf;
   /** Pointer to a structure containing the coding for the
   absence reason of the calling number.*/
   IFX_TAPI_CID_ABS_REASON_t     *pABSCLICode;
   /** Pointer to a structure containing FSK configuration parameters. If the
    parameter is not given, IFX_TAPI_CID_FSK_CFG_t default values will be used.*/
   IFX_TAPI_CID_FSK_CFG_t        *pFSKConf;
   /** Tone table index for the alert tone to be used. Required for automatic
    CID/MWI generation. By default, TAPI uses an internal tone definition.*/
   IFX_uint32_t                  nAlertToneOffhook;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF is D.*/
   IFX_char_t                    ackTone;
} IFX_TAPI_CID_STD_KPN_DTMF_FSK_t;

/** Structure containing CID configuration for the KPN standard, with DTMF
    transmission */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, IFX_TAPI_CID_TIMING_t default values will be used.*/
   IFX_TAPI_CID_TIMING_t         *pCIDTiming;
   /** Pointer to a structure containing DTMF configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_DTMF_CFG_t default values will be
   used. */
   IFX_TAPI_CID_DTMF_CFG_t       *pDTMFConf;
   /** Pointer to a structure containing the coding for the
   absence reason of the calling number.*/
   IFX_TAPI_CID_ABS_REASON_t     *pABSCLICode;
   /** Pointer to a structure containing FSK configuration parameters. If the
    parameter is not given, IFX_TAPI_CID_FSK_CFG_t default values will be used.*/
   IFX_TAPI_CID_FSK_CFG_t        *pFSKConf;
   /** Tone table index for the alert tone to be used; required for automatic
    CID/MWI generation. By default, the TAPI uses an internal tone definition.*/
   IFX_uint32_t                  nAlertToneOffhook;
   /** DTMF ACK after CAS, used for off-hook transmission; default DTMF is D.*/
   IFX_char_t                    ackTone;
} IFX_TAPI_CID_STD_KPN_DTMF_t;

/** Structure containing CID configuration for the TDC standard */
typedef struct
{
   /** Pointer to a structure containing timing information. If the parameter
    is not given, IFX_TAPI_CID_TIMING_t default values will be used.*/
   IFX_TAPI_CID_TIMING_t         *pCIDTiming;
   /** Pointer to a structure containing DTMF configuration parameters. If the
    parameter is not given, \ref IFX_TAPI_CID_DTMF_CFG_t default values will be
   used. */
   IFX_TAPI_CID_DTMF_CFG_t       *pDTMFConf;
   /** Pointer to a structure containing the coding for the
   absence reason of the calling number.*/
   IFX_TAPI_CID_ABS_REASON_t     *pABSCLICode;
} IFX_TAPI_CID_STD_TDC_t;

/** Union of the CID configuration structures for different standards.*/
typedef union
{
   /** Structure defining configuration parameters for the Telcordia standard. */
   IFX_TAPI_CID_STD_TELCORDIA_t     telcordia;
   /** Structure defining configuration parameters for the ETSI standard, with FSK
    transmission.*/
   IFX_TAPI_CID_STD_ETSI_FSK_t      etsiFSK;
   /** Structure defining configuration parameters for the ETSI standard, with DTMF
    transmission.*/
   IFX_TAPI_CID_STD_ETSI_DTMF_t     etsiDTMF;
   /** Structure defining configuration parameters for the BT SIN standard. */
   IFX_TAPI_CID_STD_SIN_t           sin;
   /** Structure defining configuration parameters for the NTT standard. */
   IFX_TAPI_CID_STD_NTT_t           ntt;
   /** Structure defining configuration parameters for the KPN standard, with DTMF
       transmission. */
   IFX_TAPI_CID_STD_KPN_DTMF_t      kpnDTMF;
   /** Structure defining configuration parameters for the KPN standard, with DTMF
       and FSK transmission. */
   IFX_TAPI_CID_STD_KPN_DTMF_FSK_t  kpnDTMF_FSK;
   /** Structure defining configuration parameters for the TDC standard */
   IFX_TAPI_CID_STD_TDC_t           tdc;
} IFX_TAPI_CID_STD_TYPE_t;

/** Structure containing CID configuration possibilities. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Standard used (enumerated in IFX_TAPI_CID_STD_t); default
    IFX_TAPI_CID_STD_TELCORDIA.*/
   IFX_TAPI_CID_STD_t       nStandard;
   /** Union of the different standards; default IFX_TAPI_CID_STD_TELCORDIA_t.*/
   IFX_TAPI_CID_STD_TYPE_t  *cfg;
} IFX_TAPI_CID_CFG_t;

/** Structure for element types (\ref IFX_TAPI_CID_SERVICE_TYPE_t) containing
   date and time information.*/
typedef struct
{
   /** Element type. */
   IFX_TAPI_CID_SERVICE_TYPE_t   elementType;
   /** Month. */
   IFX_uint32_t                  month;
   /** Day. */
   IFX_uint32_t                  day;
   /** Hour. */
   IFX_uint32_t                  hour;
   /** Minute. */
   IFX_uint32_t                  mn;
} IFX_TAPI_CID_MSG_DATE_t;

/** Structure for element types ( \ref IFX_TAPI_CID_SERVICE_TYPE_t) with
 dynamic length (line numbers or names).*/
typedef struct
{
   /** Element type. */
   IFX_TAPI_CID_SERVICE_TYPE_t   elementType;
   /** Length of the message array. */
   IFX_uint32_t                  len;
   /** String containing the message element. */
   IFX_uint8_t                   element[IFX_TAPI_CID_MSG_LEN_MAX];
} IFX_TAPI_CID_MSG_STRING_t;

/** Structure for element types (\ref IFX_TAPI_CID_SERVICE_TYPE_t) with one
 value (length 1).
*/
typedef struct
{
   /** Element type. */
   IFX_TAPI_CID_SERVICE_TYPE_t   elementType;
   /** Value for the message element. */
   IFX_uint8_t                   element;
} IFX_TAPI_CID_MSG_VALUE_t;

/** Structure for service type transparent (\ref IFX_TAPI_CID_SERVICE_TYPE_t).*/
typedef struct
{
   /** Element type. */
   IFX_TAPI_CID_SERVICE_TYPE_t   elementType;
   /** Element length. */
   IFX_uint32_t                  len;
   /** Element buffer. */
   IFX_uint8_t                   *data;
} IFX_TAPI_CID_MSG_TRANSPARENT_t;

/** Union of element types. */
typedef union
{
   /** Message element including date and time information. */
   IFX_TAPI_CID_MSG_DATE_t          date;
   /** Message element formatted as a string.*/
   IFX_TAPI_CID_MSG_STRING_t        string;
   /** Message element formatted as a value.*/
   IFX_TAPI_CID_MSG_VALUE_t         value;
   /** Message element to be sent with transparent transmission. */
   IFX_TAPI_CID_MSG_TRANSPARENT_t   transparent;
} IFX_TAPI_CID_MSG_ELEMENT_t;

/** Structure containing the CID message type and content as well as
   information about transmission mode. This structure contains all information
   required by IFX_TAPI_CID_TX_INFO_START to start CID generation.*/
typedef struct
{
    /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Defines the transmission mode (enumerated in \ref IFX_TAPI_CID_HOOK_MODE_t).
    The default is \ref IFX_TAPI_CID_HM_ONHOOK.*/
   IFX_TAPI_CID_HOOK_MODE_t      txMode;
   /** Defines the message type to be displayed (enumerated in \ref
   IFX_TAPI_CID_MSG_TYPE_t).*/
   IFX_TAPI_CID_MSG_TYPE_t       messageType;
   /** Number of elements of the message array. */
   IFX_uint32_t                  nMsgElements;
   /** Message array. */
   IFX_TAPI_CID_MSG_ELEMENT_t    *message;
} IFX_TAPI_CID_MSG_t;

/** Structure for caller ID receiver status. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Caller ID receiver actual status using \ref IFX_TAPI_CID_RX_STATUS_t

   - 0: IFX_TAPI_CID_RX_STATE_INACTIVE, CID receiver is not active
   - 1: IFX_TAPI_CID_RX_STATE_ACTIVE, CID receiver is active
   - 2: IFX_TAPI_CID_RX_STATE_ONGOING, CID receiver is just receiving data.
   - 3: IFX_TAPI_CID_RX_STATE_DATA_READY, CID receiver has completed
   */
   IFX_uint8_t nStatus;
   /** Caller ID receiver actual error code using \ref IFX_TAPI_CID_RX_ERROR_t

   - 0: IFX_TAPI_CID_RX_ERROR_NONE, no error during CID receiver operation
   - 1: IFX_TAPI_CID_RX_ERROR_READ, reading error during CID receiver
      operation */
   IFX_uint8_t nError;
} IFX_TAPI_CID_RX_STATUS_t;

/** Structure for caller ID receiver data. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Caller ID receiver data. */
   IFX_uint8_t    data [IFX_TAPI_CID_RX_SIZE_MAX];
   /** Caller ID receiver data size in bytes. */
   IFX_uint32_t   nSize;
} IFX_TAPI_CID_RX_DATA_t;

/** Structure used by \ref IFX_TAPI_CID_RX_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Module type. */
   IFX_TAPI_MODULE_TYPE_t module;
   /** Hook mode for the reception. */
   IFX_TAPI_CID_HOOK_MODE_t hookMode;
}IFX_TAPI_CID_RX_CFG_t;

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_CID_TX_INFO_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_CID_TX_INFO_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_CID */

/* ======================================================================== */
/* TAPI Connection Services, structures (Group TAPI_INTERFACE_CON)          */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Structure used for AAL configuration. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Connection identifier.*/
   IFX_uint16_t nCid;
   /** Start value for the time stamp (resolution of 125 s). */
   IFX_uint16_t nTimestamp;
   /** Connection identifier for CPS events. */
   IFX_uint16_t nCpsCid;
} IFX_TAPI_PCK_AAL_CFG_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_PKT_EV_OOB_DTMF_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Specifies the out-of-band mode to set. */
      IFX_TAPI_PKT_EV_OOB_t nTransmitMode;
   }IFX_TAPI_PKT_EV_OOB_DTMF_t;
#else /* TAPI_ONE_DEVNODE */
   /** Specifies the out-of-band mode to set;
       structure used by \ref IFX_TAPI_PKT_EV_OOB_DTMF_SET. */
   typedef IFX_TAPI_PKT_EV_OOB_t IFX_TAPI_PKT_EV_OOB_DTMF_t;
#endif /* TAPI_ONE_DEVNODE */

#ifndef TAPI4_DXY_DOC
/** AAL profile setup structure. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Amount of rows to program. */
   IFX_int8_t rows;
   /** Length of packet in bytes - 1. */
   IFX_int8_t len[10];
   /** UUI code-point range indicator, see \ref IFX_TAPI_PKT_AAL_PROFILE_RANGE_t. */
   IFX_int8_t nUUI[10];
   /** Codec as listed for \ref IFX_TAPI_COD_TYPE_t. */
   IFX_int8_t codec[10];
} IFX_TAPI_PCK_AAL_PROFILE_t;
#endif /* #ifndef TAPI4_DXY_DOC */

/** Structure for RTP configuration; RFC 2833 event payload types (ePT) for the
    encoder and decoder part are also configured. The parameters nPlayEvents and
    nEventPlayPT are used to configure the payload type for the decoder part.

    \remarks
    The parameter nEventPlayPT is ignored if the firmware does not support
    configuration of different payload types for the decoder and encoder. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Start value for the sequence number.  */
   IFX_uint16_t nSeqNr;
   /** Synchronization source value for the voice and SID packets.
       Note: a change in the SSRC leads to a reset of the RTCP statistics.*/
   IFX_uint32_t nSsrc;
   /** Start value for the time stamp (resolution of 125 us).*/
   IFX_uint32_t nTimestamp;
   /** Defines how to transmit detected tone signals in the upstream direction;
       set parameter as defined in the \ref IFX_TAPI_PKT_EV_OOB_t enumerator.
       This applies to all DTMF and MFTD detected tone signals. */
   IFX_uint8_t  nEvents;
   /** Defines whether received RFC 2833 packets are played out locally;
       set parameter as defined in the \ref IFX_TAPI_PKT_EV_OOBPLAY_t enumerator. */
   IFX_uint8_t  nPlayEvents;
   /** Payload type to be used for RFC 2833 frames in the encoder direction
      (upstream). This payload type parameter is also used for downstream in
      case 'nPlayEvents' is configured to use the same payload type for
      upstream and downstream. */
   IFX_uint8_t  nEventPT;
   /** Payload type to be used for RFC 2833 frames in the decoder direction
      (downstream). This parameter is used in case 'nPlayEvents' is configured
      to use different payload types for the upstream and downstream directions.
      Only parameter 'nEventPT' is used in case the upstream and downstream use
      the same payload type.*/
   IFX_uint8_t  nEventPlayPT;
   /** Redundancy according to RFC 2198; describes the amount of redundant
       RTP packets in every generated upstream RTP packet. Encoder redundancy
       is disabled when this parameter is set to zero (default).*/
   IFX_uint8_t  nRedundancy;
   /** Redundancy according to RFC 2198 on the decoder side.
      - IFX_TRUE Enable redundancy packets decoding
      - IFX_FALSE Disable redundancy packets decoding */
   IFX_boolean_t  bRedundancyDecoder;
} IFX_TAPI_PKT_RTP_CFG_t;

/** Structure used to configure the transmission of detected MFTD tone signals.
    MFTD signal events can be configured to be transmitted in-band or out-of-band.
*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Signal transmission of all MFTD signals (CED, CNG, DIS). */
   IFX_TAPI_PKT_EV_OOB_t  nTransmitMode;
} IFX_TAPI_PKT_EV_OOB_MFTD_t;

/** Structure for RTP payload type configuration. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Table with all payload types; the coder type \ref IFX_TAPI_COD_TYPE_t
       is used as an index. */
   IFX_uint8_t nPTup [IFX_TAPI_COD_TYPE_MAX];
   /** Table with all payload types; the coder type \ref IFX_TAPI_COD_TYPE_t
       is used as an index. */
   IFX_uint8_t nPTdown [IFX_TAPI_COD_TYPE_MAX];
   /** Payload type (upstream direction) for RFC 2198 redundancy RTP header.
       This payload type is only used when RFC2198 redundancy is enabled
       using \ref IFX_TAPI_PKT_RTP_CFG_SET.*/
   IFX_uint8_t nRedPTup;
   /** Payload type (downstream direction) for RFC 2198 redundancy RTP header.
       This payload type is only used when RFC 2198 redundancy is enabled
       using \ref IFX_TAPI_PKT_RTP_CFG_SET.*/
   IFX_uint8_t nRedPTdown;
} IFX_TAPI_PKT_RTP_PT_CFG_t;

/** Describes the parameters for configuring header information
    in case the IPv4 protocol is used. */
typedef struct
{
   /** Differentiated services; this field is defined in RFC 2474. */
   IFX_uint8_t             nDSCP;
   /** Flags; the 3 LSBs are used. */
   IFX_uint8_t             nFlags;
   /** Time to live. */
   IFX_uint8_t             nTtl;
   /** IP-layer configuration: destination IP address for sent packets;
       bytes placed in network byte order. */
   IFX_uint32_t            IpDst;
   /** IP-layer configuration: source IP address for sent packets;
       bytes placed in network byte order. */
   IFX_uint32_t            IpSrc;
} IFX_TAPI_PKT_NET_IPV4_CFG_t;

/** Describes the parameters for configuring header information
    in case the IPv6 protocol is used. Not supported yet. */
typedef struct
{
   /** Flow label; used for specifying special router handling from source
       to destination(s) for a sequence of packets*/
   IFX_uint32_t            nFlow;
   /** Traffic class. */
   IFX_uint8_t             nTrafficClass;
   /** Hop limit. */
   IFX_uint8_t             nHopLim;
   /** IP layer configuration: destination IP address for sent packets. */
   IFX_uint8_t             IpDst[IFX_TAPI_IPV6_LEN];
   /** IP layer configuration: source IP address for sent packets. */
   IFX_uint8_t             IpSrc[IFX_TAPI_IPV6_LEN];
} IFX_TAPI_PKT_NET_IPV6_CFG_t;

/** IP protocol parameter for IPv4 and IPv6. */
typedef union
{
   /** IPv4 protocol parameter. */
   IFX_TAPI_PKT_NET_IPV4_CFG_t   v4;
   /** IPv6 protocol parameter. */
   IFX_TAPI_PKT_NET_IPV6_CFG_t   v6;
} IFX_TAPI_PKT_NET_IP_t;

/** VLAN Q-in-Q selection; specifies how many VLAN tags are given in the
    generated network packet. */
typedef enum
{
   /** No VLAN tag. */
   IFX_TAPI_PKT_VLAN_Q_NONE = 0,
   /** One VLAN tag. */
   IFX_TAPI_PKT_VLAN_Q_ONE = 1,
   /** Two VLAN tags, according to Q-in-Q standard. */
   IFX_TAPI_PKT_VLAN_Q_TWO = 2
} IFX_TAPI_PKT_VLAN_Q_t;

/** Structure for Ethernet-MAC/IP/UDP protocol session parameters;
    describes the parameter and configuration for the received and sent
    packets.
    \remarks Currently only IPv4 is supported; there is no support for IPv6.
*/
typedef struct
{
   /** Device to configure. */
   IFX_uint16_t                  dev;
   /** Channel to configure for this RTP session. */
   IFX_uint16_t                  ch;
   /** Configuration can be individually placed for voice traffic (RTP) or
       fax packets (T.38). */
   IFX_TAPI_PKT_NET_TYPE_t       nPktType;
   /** MAC layer configuration: destination MAC address for sent packets.
       In case the firmware does not support exchange of MAC packets,
       this field is ignored by the TAPI. */
   IFX_uint8_t                   MacDst [IFX_TAPI_MAC_LEN];
   /** MAC layer configuration: destination MAC address for sent packets.
       In case the firmware does not support exchange of MAC packets,
       this field is ignored by the TAPI. */
   IFX_uint8_t                   MacSrc [IFX_TAPI_MAC_LEN];
   /** VLAN tagging selector. Specifies how many VLAN tags are part of the
       Ethernet packet. Note that some devices do not offer VLAN support.
       The VLAN 1 tag is the outer tag and the VLAN 2 tag is the inner tag. */
   IFX_TAPI_PKT_VLAN_Q_t         nVLAN_Tagging;
   /** VLAN 1 tag related TPID field. Note that some
       devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging != IFX_TAPI_PKT_VLAN_Q_NONE'. */
   IFX_uint16_t                  VLAN_1TPID;
   /** Priority field of the first VLAN tag for the transmission direction. The
       priority field is ignored for the receiving direction. Note that some
       devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging != IFX_TAPI_PKT_VLAN_Q_NONE'. */
   IFX_uint16_t                  VLAN_1Priority;
   /** CFI field of the first VLAN tag for the transmission direction. Note that
       some devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging != IFX_TAPI_PKT_VLAN_Q_NONE'. */
   IFX_uint16_t                  VLAN_1Cfi;
   /** First VLAN ID. VLAN tag is used for transmitted and received packets in
       case this field is set to a valid ID (1 ... 4094). Note that some
       devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging != IFX_TAPI_PKT_VLAN_Q_NONE'. */
   IFX_uint16_t                  VLAN_1Vid;
   /** VLAN 2 tag related TPID field.
       Only used in case 'nVLAN_Tagging == IFX_TAPI_PKT_VLAN_Q_BOTH'. */
   IFX_uint16_t                  VLAN_2TPID;
   /** Priority field of the second VLAN tag (VLAN stacking) for transmission
       direction. Note that some devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging == IFX_TAPI_PKT_VLAN_Q_BOTH'. */
   IFX_uint16_t                  VLAN_2Priority;
   /** CFI field of the second VLAN tag (VLAN stacking) for transmission
       direction. Note that some devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging == IFX_TAPI_PKT_VLAN_Q_BOTH'. */
   IFX_uint16_t                  VLAN_2Cfi;
   /** Second VLAN ID (VLAN stacking). VLAN tag is used for transmitted and
       received packets. Note that some devices do not offer VLAN support.
       Only used in case 'nVLAN_Tagging == IFX_TAPI_PKT_VLAN_Q_BOTH'. */
   IFX_uint16_t                  VLAN_2Vid;
   /** Set the type of IP protocol to support (IPv4/IPv6).
       In case the firmware does not support IP packet exchange,
       this field is ignored by the TAPI. */
   IFX_TAPI_PKT_NET_IP_MODE_t    nIpMode;
   /** Configure the IP header information for the chosen protocol
       in 'nIpMode'.
       In case the firmware does not support IP packet exchange,
       this field is ignored by the TAPI. */
   IFX_TAPI_PKT_NET_IP_t         nIp;
   /** UDP layer configuration: UDP destination port number
       for sent packets. */
   IFX_uint16_t                  nUdpDst;
   /** UDP layer configuration: UDP source port number for sent packets. */
   IFX_uint16_t                  nUdpSrc;
   /** UDP layer configuration: UDP destination port number
       for received packets. The firmware is able to filter received packets
       on a different UDP port. */
   IFX_uint16_t                  nUdpRcv;
   /** Generation of the UDP checksum for sent packets.
       If disabled, the UDP checksum field is set to zero. */
   IFX_operation_t               bUdpCrcGen;
   /** Check the UDP CRC for received packets. The CRC value is ignored
       in case the check is disabled. */
   IFX_operation_t               bUdpCrcCheck;
} IFX_TAPI_PKT_NET_CFG_t;

/** Structure for RTCP statistics. The structure refers to RFC3550/3551. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Sender generating this report. */
   IFX_uint32_t ssrc;
   /** NTP time stamp higher than 32 bits. This field is not filled by the TAPI.*/
   IFX_uint32_t ntp_sec;
   /** NTP time stamp lower than 32 bits. This field is not filled by the TAPI.*/
   IFX_uint32_t ntp_frac;
   /** RTP time stamp. */
   IFX_uint32_t rtp_ts;
   /** Sent packet count. */
   IFX_uint32_t psent;
   /** Sent octets count. */
   IFX_uint32_t osent;
   /** Data source. */
   IFX_uint32_t rssrc;
   /** Receiver fraction loss. */
   IFX_uint32_t fraction:8;
   /** Receiver packet lost. */
   IFX_int32_t  lost:24;
   /** Extended last sequence number received.*/
   IFX_uint32_t last_seq;
   /** Receiver inter-arrival jitter. */
   IFX_uint32_t jitter;
   /** Last sender report. This field is not filled by the TAPI. */
   IFX_uint32_t lsr;
   /** Delay since the last sender report. This field is not filled by the TAPI.*/
   IFX_uint32_t dlsr;
} IFX_TAPI_PKT_RTCP_STATISTICS_t;

/** Structure used as a parameter by \ref IFX_TAPI_ENC_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Encoder type. */
   IFX_TAPI_COD_TYPE_t           nEncType;
   /** Frame length in ms. */
   IFX_TAPI_COD_LENGTH_t         nFrameLen;
   /** Bit alignment for G.726 codecs; RTP payload bit alignment according
       to either IETF RFC350 or to AAL2, ITU-T, I366.2 can be set. */
   IFX_TAPI_COD_AAL2_BITPACK_t   AAL2BitPack;
} IFX_TAPI_ENC_CFG_t;

/** Structure used to configure the decoder;
    parameter for \ref IFX_TAPI_DEC_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
  /** Bit alignment for G.726 codecs; RTP payload bit alignment according
      to either IETF RFC350 or to AAL2, ITU-T, I366.2 can be set. */
   IFX_TAPI_COD_AAL2_BITPACK_t   AAL2BitPack;
  /** Select PLC type; defined in codec or special pattern. */
   IFX_TAPI_DEC_PLC_t   Plc;
} IFX_TAPI_DEC_CFG_t;

/** This structure is used for RTP event generation
    RFC 2833 or ABCD (RFC 4733 and RFC 524). */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Event code according to RFC 2833
       or ABCD event code according to (RFC 4733 and RFC 5244). */
   IFX_TAPI_PKT_EV_NUM_t event;
   /** Start/tone event generation;
       only for RFC 2833 events, not used for ABCD event generation. */
   IFX_TAPI_PKT_EV_GEN_ACTION_t   action;
   /** Duration of event in units of 10 ms; 0 = forever.
       Used only for RFC 2833 events, not for ABCD event generation. */
   IFX_uint8_t    duration;
   /** Volume of event;
       used only for RFC 2833 events, not for ABCD event generation */
   IFX_TAPI_PKT_EV_GEN_VOLUME_t   volume;
} IFX_TAPI_PKT_EV_GENERATE_t;


/** Re-transmission time selector for ABCD events (upstream direction). */
typedef enum {
   /** Re-transmission time for ABCD events is 5 seconds. */
   IFX_TAPI_PKT_EV_ABCD_TX_5SEC = 0,
   /** Re-transmission time for ABCD events is 1 minute. */
   IFX_TAPI_PKT_EV_ABCD_TX_60SEC = 1
} IFX_TAPI_PKT_EV_ABCD_TIME_TX_t;

/** Timeout selector for incoming ABCD events (downstream direction). */
typedef enum {
   /** Re-transmission time for ABCD events is 5 seconds. */
   IFX_TAPI_PKT_EV_ABCD_RX_6SEC = 0,
   /** Re-transmission time for ABCD events is 1 minute. */
   IFX_TAPI_PKT_EV_ABCD_RX_66SEC = 1
} IFX_TAPI_PKT_EV_ABCD_TIME_RX_t;

/** This structure is used to configure support of ABCD events
    RFC 4733 and RFC 5244. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Disable triple redundancy for ABCD events in the upstream direction. */
   IFX_boolean_t bABCD_DisableRedundancy;
   /** Disable periodical re-transmission for ABCD events
       in the upstream direction. */
   IFX_boolean_t bABCD_DisableRetransmission;
   /** Re-transmission time for ABCD events in the upstream direction. */
   IFX_TAPI_PKT_EV_ABCD_TIME_TX_t nABCD_TimeRetransmission;
   /** Timeout period for incoming ABCD events in the downstream direction. */
   IFX_TAPI_PKT_EV_ABCD_TIME_RX_t nABCD_TimeoutEvent;
   /** Disable timeout event reporting for incoming ABCD events
       (downstream direction). */
   IFX_boolean_t bABCD_DisableEventTimeout;
   /** Disable ABCD event (144 to 159) reporting. */
   IFX_boolean_t bABCD_DisableEventReporting;
} IFX_TAPI_PKT_EV_GENERATE_ABCD_CFG_t;

#ifdef TAPI_ONE_DEVNODE
/** This structure is used to flush the upstream packet FIFO on a channel */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_PKT_FLUSH_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure for jitter buffer configuration used by  \ref IFX_TAPI_JB_CFG_SET.
   \remarks
    This structure may be changed in the future. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Jitter buffer type, value of \ref IFX_TAPI_JB_TYPE_t. */
   IFX_uint8_t    nJbType;
   /** Optimize jitter buffer for voice or data traffic,
       value of \ref IFX_TAPI_JB_PKT_ADAPT_t.  */
   IFX_int8_t     nPckAdpt;
   /** Reserved; local adaptation, value of \ref IFX_TAPI_JB_LOCAL_ADAPT_t.
       This is relevant only for an adaptive jitter buffer. This parameter is only
       used in case 'nJbType' is set to adaptive mode. */
   IFX_int8_t     nLocalAdpt;
   /** Scaling factor multiplied by 16; in adaptive jitter buffer mode, the
       target average playout delay is equal to the estimated jitter multiplied
       by the scaling factor. The default value for the scaling factor is
       about 1.4 (nScaling = 22), meaning that the target average playout delay
       is equal to the estimated jitter. If less packets should be dropped
       because of jitter, the scaling factor has to be increased. An increase
       in the scaling factor will eventually lead to an increased playout
       delay. The supported range is 1 to 16 (nScaling=16 up to 256). */
   IFX_int8_t     nScaling;
   /** Initial size of the jitter buffer in time stamps of 125 us in case of
      an adaptive jitter buffer. */
   IFX_uint16_t   nInitialSize;
   /** Minimum size of the jitter buffer in time stamps of 125 us in case of an
      adaptive jitter buffer. */
   IFX_uint16_t   nMinSize;
   /** Maximum size of the jitter buffer in time stamps of 125 us in case of an
   adaptive jitter buffer. */
   IFX_uint16_t   nMaxSize;
} IFX_TAPI_JB_CFG_t;

/** Structure for jitter buffer statistics used by the
 \ref IFX_TAPI_JB_STATISTICS_GET ioctl.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Jitter buffer type:
      - 1: fixed
      - 2: adaptive */
   IFX_uint8_t  nType;
   /** Incoming time high word total time, in time stamp units, for all packets since
   the start of the connection that could be played out correctly;
   not supported anymore.  */
   IFX_uint32_t nInTime;
   /**  Comfort noise generation; not supported anymore. */
   IFX_uint32_t nCNG;
   /**  Bad frame interpolation; not supported anymore. */
   IFX_uint32_t nBFI;
   /** Current jitter buffer size [125us timestamp units]. */
   IFX_uint16_t nBufSize;
   /** Maximum estimated jitter buffer size [125us timestamp units]. */
   IFX_uint16_t nMaxBufSize;
   /** Minimum estimated jitter buffer size [125us timestamp units]. */
   IFX_uint16_t nMinBufSize;
   /** Maximum packet delay; not supported anymore.  */
   IFX_uint16_t nMaxDelay;
   /**  Minimum packet delay; not supported anymore. */
   IFX_uint16_t nMinDelay;
   /** Network jitter value; not supported anymore. */
   IFX_uint16_t nNwJitter;
   /**  Playout delay [125us timestamp units]. */
   IFX_uint16_t nPODelay;
   /**  Maximum playout delay [125us timestamp units]. */
   IFX_uint16_t nMaxPODelay;
   /**  Minimum playout delay [125us timestamp units]. */
   IFX_uint16_t nMinPODelay;
   /**  Received packet number.  */
   IFX_uint32_t nPackets;
   /** Lost packets number; not supported anymore. */
   IFX_uint16_t nLost;
   /**  Invalid packet number. */
   IFX_uint16_t nInvalid;
   /** Duplication packet number; not supported anymore. */
   IFX_uint16_t nDuplicate;
   /**  Late packets number. */
   IFX_uint16_t nLate;
   /**  Early packets number. */
   IFX_uint16_t nEarly;
   /**  Re-synchronizations number. */
   IFX_uint16_t nResync;
   /** Total number of injected samples since the beginning of the connection
       or since the last statistic reset due to jitter buffer underflows.*/
   IFX_uint32_t nIsUnderflow;
   /** Total number of injected samples since the beginning of the connection or
       since the last statistic reset in case of normal jitter buffer operation,
       that is, when there is no jitter buffer underflow. */
   IFX_uint32_t nIsNoUnderflow;
   /** Total number of injected samples since the beginning of the connection or
       since the last statistic reset in case of jitter buffer increments.*/
   IFX_uint32_t nIsIncrement;
   /** Total number of skipped lost samples since the beginning of the
      connection or since the last statistic reset in case of jitter buffer
      decrements. */
   IFX_uint32_t nSkDecrement;
   /** Total number of dropped samples since the beginning of the connection
      or since the last statistic reset in case of jitter buffer decrements.*/
   IFX_uint32_t nDsDecrement;
   /** Total number of dropped samples since the beginning of the connection
      or since the last statistic reset in case of jitter buffer overflows.*/
   IFX_uint32_t nDsOverflow;
   /** Total number of comfort noise samples since the beginning of the
       connection or since the last statistic reset. */
   IFX_uint32_t nSid;
   /** Number of received bytes high part, including event packets. */
   IFX_uint32_t nRecBytesH;
   /** Number of received bytes low part, including event packets. */
   IFX_uint32_t nRecBytesL;
} IFX_TAPI_JB_STATISTICS_t;

/** Phone channel mapping structure used for \ref IFX_TAPI_MAP_DATA_ADD and
\ref IFX_TAPI_MAP_DATA_REMOVE. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Phone channel number to which this channel should be mapped;
   phone channel numbers start from 0. */
   IFX_uint8_t                      nDstCh;
   /** Type of destination channel, defined in \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t              nChType;
   /** Enables or disables the recording service or leaves it as is.

   - 0: IFX_TAPI_MAP_DATA_UNCHANGED, do not modify the status of the recorder
   - 1: IFX_TAPI_MAP_DATA_START, recording is started, same as
    \ref IFX_TAPI_ENC_START
   - 2: IFX_TAPI_MAP_DATA_STOP, recording is stopped, same as
    \ref IFX_TAPI_ENC_STOP */
   IFX_TAPI_MAP_DATA_START_STOP_t   nRecStart;
   /** Enables or disables the play service or leaves it as is.

   - 0: IFX_TAPI_MAP_DATA_UNCHANGED, do not modify the status of the recorder
   - 1: IFX_TAPI_MAP_DATA_START, playing is started, same as
      \ref IFX_TAPI_DEC_START
   - 2: IFX_TAPI_MAP_DATA_STOP, playing is stopped, same as
      \ref IFX_TAPI_DEC_STOP */
   IFX_TAPI_MAP_DATA_START_STOP_t   nPlayStart;
} IFX_TAPI_MAP_DATA_t;

/** Phone channel mapping structure used for \ref IFX_TAPI_MAP_PHONE_ADD and
   \ref IFX_TAPI_MAP_PHONE_REMOVE. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Phone channel number to which this channel should be mapped;
      phone channel numbers start from 0. */
   IFX_uint8_t                   nPhoneCh;
   /** Type of destination channel, see \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t           nChType;
} IFX_TAPI_MAP_PHONE_t;
/** PCM channel mapping structure used for \ref IFX_TAPI_MAP_PCM_ADD and
\ref IFX_TAPI_MAP_PCM_REMOVE. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Channel number to which this channel should be mapped;
   channel numbers start from 0. */
   IFX_uint8_t                nDstCh;
   /** Type of destination channel, see \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t        nChType;
} IFX_TAPI_MAP_PCM_t;

#ifndef TAPI4_DXY_DOC
/** DECT channel mapping structure used for \ref IFX_TAPI_MAP_DECT_ADD and
\ref IFX_TAPI_MAP_DECT_REMOVE. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Channel number to which this channel should be mapped;
   channel numbers start from 0. */
   IFX_uint8_t                nDstCh;
   /** Type of destination channel, see \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t        nChType;
} IFX_TAPI_MAP_DECT_t;
#endif /* #ifndef TAPI4_DXY_DOC */

/** TAPI initialization structure used for IFX_TAPI_MAP_MON_ADD and
    IFX_TAPI_MAP_MON_REMOVE. Note that this interface is currently not
    implemented and will be implemented in a future TAPI release.
    Changes to the interfaces, parameters and functionality are
    possible. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Monitor module channel; module that monitors the destination module.
       Channel number starts from 0. */
   IFX_uint16_t ch;
   /** Type of the monitoring module, see \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t           nChType;
   /** Type of the destination module, see \ref IFX_TAPI_MAP_TYPE_t. */
   IFX_TAPI_MAP_TYPE_t           nDstChType;
   /** Monitor the voice path of the destination module coming from external
       (e.g. analog line, PCM highway). */
   IFX_uint16_t external:1;
   /** Monitor the voice path coming from the module connected to our
       destination module (for example, coder module, ALM module, PCM module). */
   IFX_uint16_t internal:1;
   /** Reserved (should be set to zero). */
   IFX_uint16_t reserved:6;
   /** Destination channel number to which the monitoring module
       should be mapped. Channel number starts from 0. */
   IFX_uint16_t                 nDstCh;
} IFX_TAPI_MAP_MON_CFG_t;

#ifndef TAPI4_DXY_DOC
/** Structure used as parameter for \ref IFX_TAPI_ENC_ROOM_NOISE_DETECT_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Detection threshold level in minus dB. */
   IFX_uint32_t   nThreshold;
   /** Count of consecutive voice packets required for event. */
   IFX_uint8_t    nVoicePktCnt;
   /** Count of consecutive silence packets required for event. */
   IFX_uint8_t    nSilencePktCnt;
} IFX_TAPI_ENC_ROOM_NOISE_DETECT_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used as parameter for
       \ref IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
   } IFX_TAPI_ENC_ROOM_NOISE_DETECT_STOP_t;
#endif /* TAPI_ONE_DEVNODE */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_COD_DEC_HP_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Enable or disable the HP filter. */
      IFX_operation_t hpMode;
   }IFX_TAPI_COD_DEC_HP_t;
#else /* TAPI_ONE_DEVNODE */
   /** Enable or disable the HP filter.
      Used by \ref IFX_TAPI_COD_DEC_HP_SET. */
   typedef IFX_operation_t IFX_TAPI_COD_DEC_HP_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_DEC_START and \ref IFX_TAPI_DEC_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_DEC_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_DEC_TYPE_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Specifies the codec; the default is G711. */
      IFX_TAPI_COD_TYPE_t  decType;
   }IFX_TAPI_DEC_TYPE_t;
#else /* TAPI_ONE_DEVNODE */
   /** Specifies the codec; the default is G711.
      Structure used by \ref IFX_TAPI_DEC_TYPE_SET. */
   typedef IFX_TAPI_COD_TYPE_t IFX_TAPI_DEC_TYPE_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_ENC_AGC_ENABLE. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Enable or disable the AGC. */
      IFX_TAPI_ENC_AGC_MODE_t agcMode;
   }IFX_TAPI_ENC_AGC_ENABLE_t;
#else /* TAPI_ONE_DEVNODE */
   /** Enable or disable the AGC.
       Type used by \ref IFX_TAPI_ENC_AGC_ENABLE. */
   typedef IFX_TAPI_ENC_AGC_MODE_t IFX_TAPI_ENC_AGC_ENABLE_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_ENC_HOLD. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Enable or disable the hold. */
      IFX_operation_t hold;
   }IFX_TAPI_ENC_HOLD_t;
#else /* TAPI_ONE_DEVNODE */
   /** Enable or disable the hold;
       type used by \ref IFX_TAPI_ENC_HOLD. */
   typedef IFX_operation_t IFX_TAPI_ENC_HOLD_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_ENC_START and \ref IFX_TAPI_ENC_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_ENC_MODE_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_ENC_VAD_CFG_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Select the VAD mode. */
      IFX_TAPI_ENC_VAD_t vadMode;
   }IFX_TAPI_ENC_VAD_CFG_t;
#else /* TAPI_ONE_DEVNODE */
   /** Select the VAD mode;
      type used by \ref IFX_TAPI_ENC_VAD_CFG_SET. */
   typedef IFX_TAPI_ENC_VAD_t IFX_TAPI_ENC_VAD_CFG_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_JB_STATISTICS_RESET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_JB_STATISTICS_RESET_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_PKT_RTCP_STATISTICS_RESET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_PKT_RTCP_STATISTICS_RESET_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure used to retrieve and reset the extended RTP statistics;
    used by \ref IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Number of packets with an invalid packet size is a packet with a
       payload size that is too large for the decoder, is smaller than
       the minimum packet size for the relevant codec, or that is
       not a multiple of the smallest payload size of the codec.*/
   IFX_uint32_t nRxInvalidPacketSize;
   /** Number of packets with wrong payload type
       which is not in the downstream payload type table.. */
   IFX_uint32_t nRxWrongPT;
   /** Number of packets with an 'interleaving time stamp', or
       packets with a time stamp that is more than 10 seconds
       older or newer than the last received packet. */
   IFX_uint32_t nRxInterleavingTimestamp;
   /** Number of wrong RFC2833 event packets
       which the system is not able to play out.
       This means that, for instance, if RFC DTMF event support is disabled
       and a DTMF event is received, this event is counted as being invalid. */
   IFX_uint32_t nRxRFC2833WrongEvt;
} IFX_TAPI_PKT_RTP_EXT_STATISTICS_GET_t;

/** Configures the extended RTP statistics;
    used by \ref IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG.*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Broken connection event time-out in seconds

       - Minimum: 1 s
       - Maximum: 60 s */
   IFX_uint8_t nBrokenConTimeout;
} IFX_TAPI_PKT_RTP_EXT_STATISTICS_CFG_t;

/** Structure used by \ref IFX_TAPI_COD_ANNOUNCE_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t   dev;
   /** Announcement file index in the firmware (counts from zero);
       the download announcement file will be stored in the firmware
       library under this index. The maximum amount of stored announcements
       is provided by the capability list. */
   IFX_uint32_t   nAnnIdx;
   /** Announcement file pointer; this file should be downloaded
       to the firmware. */
   IFX_char_t     *pAnn;
   /** Announcement file size in bytes; the maximum supported size is
       provided by the capability list. The sample size should be a
       multiple of 5 ms play time. */
   IFX_uint32_t   nAnnSize;
} IFX_TAPI_COD_ANNOUNCE_CFG_t;

/** Structure used by \ref IFX_TAPI_COD_ANNOUNCE_START; the firmware only
    supports playback of announcement files that are previously downloaded
    to the firmware memory using \ref IFX_TAPI_COD_ANNOUNCE_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t   dev;
   /** Channel 'module' index of the COD module where to play back the
       announcement file. */
   IFX_uint16_t   ch;
   /** Announcement file index of the announcement stored
       inside the firmware memory storage. The announcement file download
       is done by the TAPI command \ref IFX_TAPI_COD_ANNOUNCE_CFG_SET. This
       announcement file index is used by the TAPI command
       \ref IFX_TAPI_COD_ANNOUNCE_CFG_SET to download the announcement to
       the firmware memory. */
   IFX_uint32_t   nAnnIdx;
   /** The announcement file is always played back towards the firmware module
       that is connected to this COD module (for example, ALM module, PCM module).
       When the loop mode is set, the file is also played back towards the RTP
       stream. */
   IFX_boolean_t bLoop;
} IFX_TAPI_COD_ANNOUNCE_START_t;

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_COD_ANNOUNCE_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_COD_ANNOUNCE_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/**
 * Structure used by \ref IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE.
 */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Announcement index. */
   IFX_uint32_t   nAnnIdx;
} IFX_TAPI_COD_ANNOUNCE_BUFFER_FREE_t;

/** Structure used by \ref IFX_TAPI_EVENT_COD_MOS event and in the struct
    \ref IFX_TAPI_COD_MOS_t when calling \ref IFX_TAPI_COD_MOS_CFG_SET or
    \ref IFX_TAPI_COD_MOS_RESULT_GET.
    These parameters are used by the coder channel to calculate the
    MOS-LQE value from the received voice packet stream. */
typedef struct
{
   /** Default rating factor R [0x0000, ..., 0x7F00];
       calculated using 'Rn = round(R * 256)' where R is element
       of [0.0, ..., 127.0]. */
   IFX_uint16_t nR;
   /** Calculation Time Interval [1 ... 0x003F];
       calculated using 'CTI [s] = (nCTI * 4096) / 8000'.
       Setting nCTI = 0 disables the calculation. */
   IFX_uint16_t nCTI;
   /** Advantage Factor [0x0000 ... 0x7F00];
       calculated using 'nAdvantage = round(factor * 256)'. */
   IFX_uint16_t nAdvantage;
   /** MOS value [10 ... 45]; the
       physical MOS value is calculated using 'MOS = nMOS/10'. */
   IFX_uint16_t nMOS;
} IFX_TAPI_COD_MOS_VALUE_t;

/** Structure used by \ref IFX_TAPI_COD_MOS_CFG_SET and
    \ref IFX_TAPI_COD_MOS_RESULT_GET. */
typedef struct
{
   /** Device index */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Structure of type \ref IFX_TAPI_CID_TIMING_t containing the values
       to calculate the MOS-LQE and the MOS-LQE value itself. */
   IFX_TAPI_COD_MOS_VALUE_t mos;
} IFX_TAPI_COD_MOS_t;

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, structures (Group TAPI_INTERFACE_MISC)      */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Capability structure. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index; not used for this feature. */
   IFX_char_t desc[80];
   /** Defines the capability type, see \ref IFX_TAPI_CAP_TYPE_t.*/
   IFX_TAPI_CAP_TYPE_t captype;
   /** Defines if, what or how many are available. The definition of cap
       depends on the type; see captype. */
   IFX_int32_t cap;
   /** The number of this capability. */
   IFX_int32_t handle;
} IFX_TAPI_CAP_t;

/** Capability structure. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Count of allocated \ref IFX_TAPI_CAP_t elements. */
   IFX_int32_t nCap;
   /** Allocated memory for capability list;
       amount of memory should be equal to (sizeof (IFX_TAPI_CAP_t) * nCap). */
   IFX_TAPI_CAP_t *pList;
} IFX_TAPI_CAP_LIST_t;

/** Structure used for the TAPI version support check. */
typedef struct
{
   /** Major version number supported. */
   IFX_uint8_t majorNumber;
   /** Minor version number supported. */
   IFX_uint8_t minorNumber;
} IFX_TAPI_VERSION_t;

/** Data structure describing the version details;
    used by \ref IFX_TAPI_VERSION_DEV_ENTRY_t and \ref IFX_TAPI_VERSION_CH_ENTRY_t. */
typedef struct
{
   IFX_char_t name[IFX_TAPI_VERSION_LEN];
   IFX_char_t version[IFX_TAPI_VERSION_LEN];
} IFX_TAPI_ENTRY_VERSION_t;

/** Data structure for retrieving the firmware version string of the currently downloaded firmware;
    used by \ref IFX_TAPI_VERSION_DEV_ENTRY_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Version ID starting with 0. */
   IFX_uint16_t nId;
   /** Version string information about the hardware platform. */
   IFX_TAPI_ENTRY_VERSION_t versionEntry;
} IFX_TAPI_VERSION_DEV_ENTRY_t;

/** Structure used to retrieve the version string of the current version;
    used by \ref IFX_TAPI_VERSION_CH_ENTRY_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Version ID starting with 0. */
   IFX_uint16_t nId;
   /** Version string information about the hardware platform. */
   IFX_TAPI_ENTRY_VERSION_t versionEntry;
} IFX_TAPI_VERSION_CH_ENTRY_t;

/** Defines the maximum number of stack entries. */
#define IFX_TAPI_MAX_ERROR_ENTRIES 5
/** Defines the maximum length of a filename in each error-stack entry. */
#define IFX_TAPI_MAX_FILENAME 20
/** Defines the space for additional data in each error-stack entry. */
#define IFX_TAPI_MAX_ERRMSG 16

/** Contains one line of an error source, including the error code, the
source code line and the file name (maximum 32 characters). */
typedef struct
{
   /** High-level error code, which is set at the detection of the error in the
   high-level TAPI driver part.
   The code may change in the flow of the error handling in the upper call stack. */
   IFX_uint16_t nHlCode;
   /** Low-level error code, which is set at the detection of the error in the
   low-level driver part. This code is device-driver specific.
   The code may change in the flow of the error handling in the upper call stack. */
   IFX_uint16_t nLlCode;
   /** Source code line number. */
   IFX_uint32_t nLine;
   /** Source code file name. */
   IFX_char_t   sFile[IFX_TAPI_MAX_FILENAME];
   /** Any additional information depending on the error, such as the last message
       sent to the device, state machine status, etc. */
   IFX_uint32_t msg  [IFX_TAPI_MAX_ERRMSG];
}IFX_TAPI_ErrorLine_t;

/** Error information with the source of the error. It contains a maximum of
    IFX_TAPI_MAX_ERROR_ENTRIES stack entries.
    The item with index 0 is the first error detected. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t nDev;
   /** The channel that causes this error, if any. */
   IFX_uint16_t  nCh;
   /** Error code, which is set at the highest level where the
   error was detected.*/
   IFX_uint32_t         nCode;
   /** Error stack information. */
   IFX_TAPI_ErrorLine_t stack[IFX_TAPI_MAX_ERROR_ENTRIES];
   /** Number of stack entries. */
   IFX_uint8_t          nCnt;
}IFX_TAPI_Error_t;

   /** Structure used by \ref IFX_TAPI_CAP_NR. */
#ifdef TAPI_ONE_DEVNODE
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** This service returns the number of capabilities. */
      IFX_int32_t nCap;
   }IFX_TAPI_CAP_NR_t;
#else /* TAPI_ONE_DEVNODE */
   typedef IFX_int32_t IFX_TAPI_CAP_NR_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_DEBUG_REPORT_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Specifies the debug report level. */
      IFX_TAPI_DEBUG_REPORT_SET_t level;
   }IFX_TAPI_DEBUG_REPORT_t;
#else /* TAPI_ONE_DEVNODE */
   /** Specifies the debug report level;
      type used by \ref IFX_TAPI_DEBUG_REPORT_SET. */
   typedef IFX_TAPI_DEBUG_REPORT_SET_t IFX_TAPI_DEBUG_REPORT_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ===================================================================== */
/* TAPI Power Ringing Services, structures (Group TAPI_INTERFACE_RINGING)      */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Structure for ring cadence used in \ref IFX_TAPI_RING_CADENCE_HR_SET.  */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Array of data bytes containing the encoded periodic cadence sequence.
   One bit represents a ring cadence of 50 ms. A maximum pattern of 40 bytes
   (320 bits) is allowed. */
   IFX_uint8_t     data[IFX_TAPI_RING_CADENCE_MAX_BYTES];
   /** Number of valid data bits in the periodic cadence sequence. The periodic
   cadence may have any length between 1 and 320 bits. Each valid bit corresponds
   to a 50 ms cadence duration, so that the periodic cadence can have a
   duration of between 50 ms and 16 s. */
   IFX_int32_t    nr;
   /** Array of data bytes containing the encoded initial cadence sequence.
   One bit represents a ring cadence of 50 ms. A maximum pattern of 40 bytes
   (320 bits) is allowed.  */
   IFX_uint8_t     initial [IFX_TAPI_RING_CADENCE_MAX_BYTES];
   /** Number of valid data bits in the initial cadence sequence. The initial
   cadence may have any length between 0 and 320 bits. Each valid bit corresponds
   to a 50 ms cadence duration, so that the periodic cadence can have a
   duration of between 0 and 16 s. Set this to 0 if no initial cadence
   should be used. If a length is given, the initial pattern is played once
   when ringing is started. After this, the periodic pattern is played and will
   be repeated for as long as ringing is not stopped. */
   IFX_int32_t    initialNr;
} IFX_TAPI_RING_CADENCE_t;

/** Ringing configuration structure used for the \ref IFX_TAPI_RING_CFG_SET ioctl. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Configures the ringing mode values defined in
   \ref IFX_TAPI_RING_CFG_MODE_t.*/
   IFX_uint8_t    nMode;
   /** Configures the ringing sub-mode values defined in
    \ref IFX_TAPI_RING_CFG_SUBMODE_t. */
   IFX_uint8_t    nSubmode;
} IFX_TAPI_RING_CFG_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_RING_CADENCE_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** The parameter defines the cadence. This value contains the
         encoded cadence sequence. One bit represents the ring cadence voltage
         for 0.5 s. */
      IFX_int32_t nCadence;
   }IFX_TAPI_RING_CADENCE_CFG_t;
#else /* TAPI_ONE_DEVNODE */
   /** The parameter defines the cadence. This value contains the
      encoded cadence sequence. One bit represents the ring cadence voltage
      for 0.5 s.
      Type used by \ref IFX_TAPI_RING_CADENCE_SET. */
   typedef IFX_uint32_t IFX_TAPI_RING_CADENCE_CFG_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_RING_START and \ref IFX_TAPI_RING_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_RING_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_RING_MAX_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Maximum number of cadences. */
      IFX_uint32_t nMaxRings;
   } IFX_TAPI_RING_MAX_t;
#else /* TAPI_ONE_DEVNODE */
   /** Maximum number of cadences.
      Type used by \ref IFX_TAPI_RING_MAX_SET. */
   typedef IFX_uint32_t IFX_TAPI_RING_MAX_t;
#endif /* TAPI_ONE_DEVNODE */


/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ============================================================ */
/* TAPI Calibration Services, structures                        */
/* (Group TAPI_INTERFACE_CALIBRATION)                           */
/* ============================================================ */
/** \addtogroup TAPI_INTERFACE_CALIBRATION */
/*@{*/

#ifdef TAPI_ONE_DEVNODE
/** Structure used to start the analog line calibration process. The structure is used by
    \ref IFX_TAPI_CALIBRATION_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_CALIBRATION_t;
#endif /* TAPI_ONE_DEVNODE */

/** Used by \ref IFX_TAPI_CALIBRATION_CFG_t to set structure parameters to an
    unused and undefined value. */
#define IFX_TAPI_CALIBRATION_UNUSED (0x8000)

/** Defines the current state of the calibration and its results. */
typedef enum
{
   /** No calibration performed; all returned results are invalid. */
   IFX_TAPI_CALIBRATION_STATE_NO       = 0,
   /** Calibration performed successfully. */
   IFX_TAPI_CALIBRATION_STATE_DONE     = 1,
   /** Calibration process failed; the returned calibration results are
       overwritten with default values on the analog line. */
   IFX_TAPI_CALIBRATION_STATE_FAILED   = 2
}IFX_TAPI_CALIBRATION_STATE_t;

/** Structure used to set and get the analog line calibration coefficients;
    used by \ref IFX_TAPI_CALIBRATION_CFG_SET,
    \ref IFX_TAPI_CALIBRATION_CFG_GET and
    \ref IFX_TAPI_CALIBRATION_RESULTS_GET.
    Non-supported values are set to \ref IFX_TAPI_CALIBRATION_UNUSED. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Calibration already performed. This value is used when reading back the
       results or the configuration from the device. */
   IFX_TAPI_CALIBRATION_STATE_t  nState;
   /** TX path offset.
       Range: -1.5 mA ... +1.5 mA (10 uA steps). */
   IFX_int16_t nITransOffset;
   /** Measurement equipment offset.
       Range: -10 V ... +10 V (10 mV steps). */
   IFX_int16_t nMeOffset;
   /** RX path DC offset gain 30.
       Range: -1.5 V ... +1.5 V (10m V steps). */
   IFX_int16_t nUlimOffset30;
   /** RX path DC offset gain 60.
       Range: -3.0 V ... +3.0 V (10 mV steps). */
   IFX_int16_t nUlimOffset60;
   /** IDAC gain correction.
       Range: -10% ... +10% (0.1% steps).*/
   IFX_int16_t nIdacGain;
   /** Long current offset.
       Range: -3 mA ... +3 mA (10 uA steps). */
   IFX_int16_t nILongOffset;
   /** Ring current offset.
       Range: -3 mA ... +3 mA (10 uA steps). */
   IFX_int16_t nIRingOffset;
} IFX_TAPI_CALIBRATION_CFG_t;

/*@}*/ /* TAPI_INTERFACE_CALIBRATION */

/* ============================================================ */
/* TAPI COMTEL Signal Services, structures                      */
/* (Group TAPI_INTERFACE_COMTEL)                                */
/* ============================================================ */
/** \addtogroup TAPI_INTERFACE_COMTEL */
/*@{*/

/** Structure used to start and stop the COMTEL data transmission process.
    The structure is used by \ref IFX_TAPI_COMTEL_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Waveform selection:
       IFX_FALSE - signaling via half wave
       IFX_TRUE - signaling via full wave */
   IFX_boolean_t bWave;
   /** Call forwarding. */
   IFX_boolean_t bCFW;
   /** Test */
   IFX_boolean_t bTest;
   /** Message waiting bit. */
   IFX_boolean_t bMWA;
   /** Mode bit. */
   IFX_boolean_t bCTM;
} IFX_TAPI_COMTEL_t;

/*@}*/ /* TAPI_INTERFACE_COMTEL*/

/* ============================================================ */
/* TAPI PCM Services, structures (Group TAPI_INTERFACE_PCM)     */
/* ============================================================ */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** PCM data clock frequency (alias DCL or PCLK) for the PCM interface.*/
typedef enum
{
   /** 512 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_512         = 0,
   /** 1024 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_1024        = 1,
   /** 1536 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_1536        = 2,
   /** 2048 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_2048        = 3,
   /** 4096 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_4096        = 4,
   /** 8192 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_8192        = 5,
   /** 16384 kHz.*/
   IFX_TAPI_PCM_IF_DCLFREQ_16384       = 6
} IFX_TAPI_PCM_IF_DCLFREQ_t;

/** Drive mode for bit 0, in single clocking mode.*/
typedef enum
{
   /** Bit 0 is driven for the entire clock period.*/
   IFX_TAPI_PCM_IF_DRIVE_ENTIRE        = 0,
   /** Bit 0 is driven for the first half of the clock period.*/
   IFX_TAPI_PCM_IF_DRIVE_HALF          = 1
} IFX_TAPI_PCM_IF_DRIVE_t;

 /** PCM interface mode (master/slave).*/
typedef enum
{
   /** Reserved.*/
   IFX_TAPI_PCM_IF_MODE_SLAVE_AUTOFREQ = 0,
   /** Slave mode; the DCL frequency is explicitly programmed.*/
   IFX_TAPI_PCM_IF_MODE_SLAVE          = 1,
   /** Master mode; the DCL frequency is explicitly programmed.*/
   IFX_TAPI_PCM_IF_MODE_MASTER         = 2
} IFX_TAPI_PCM_IF_MODE_t;

/** PCM interface mode transmit/receive offset.*/
typedef enum
{
   /** No offset.*/
   IFX_TAPI_PCM_IF_OFFSET_NONE         = 0,
   /** Offset: one data period is added.*/
   IFX_TAPI_PCM_IF_OFFSET_1            = 1,
   /** Offset: two data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_2            = 2,
   /** Offset: three data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_3            = 3,
   /** Offset: four data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_4            = 4,
   /** Offset: five data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_5            = 5,
   /** Offset: six data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_6            = 6,
   /** Offset: seven data periods are added.*/
   IFX_TAPI_PCM_IF_OFFSET_7            = 7
} IFX_TAPI_PCM_IF_OFFSET_t;

/** Slope for the PCM interface transmit/receive.
    It is used by \ref IFX_TAPI_PCM_IF_CFG_t.*/
typedef enum
{
   /** Rising edge.*/
   IFX_TAPI_PCM_IF_SLOPE_RISE          = 0,
   /** Falling edge.*/
   IFX_TAPI_PCM_IF_SLOPE_FALL          = 1
} IFX_TAPI_PCM_IF_SLOPE_t;

/** ADPCM bit-packing in PCM time slots. */
typedef enum
{
   /** Default; ADPCM bits in least-significant bits of PCM time slot. */
   IFX_TAPI_PCM_BITPACK_LSB = 0,
   /** ADPCM bits in most-significant bits of PCM time slot. */
   IFX_TAPI_PCM_BITPACK_MSB = 1
} IFX_TAPI_PCM_BITPACK_t;

/** PCM sample order swap. */
typedef enum
{
   /** Default; the older sample is transmitted first (in the time slot with the
       lower number. */
   IFX_TAPI_PCM_SAMPLE_SWAP_DISABLED = 0,
   /** The newer sample is transmitted first (in the time slot with the lower
       number). It should be used only if the attached PCM device has a
       different sample/time-slot mapping. */
   IFX_TAPI_PCM_SAMPLE_SWAP_ENABLED = 1
} IFX_TAPI_PCM_SAMPLE_SWAP_t;

/** Structure for PCM interface configuration.

   \remarks
   Attention: not all products support all features that can be configured
   using this structure (for example, master mode or slave mode without
   automatic clock detection). Refer to the product system release note
   for more information about the supported features.
*/
typedef struct
{
   /** Device index, */
   IFX_uint16_t                  dev;
   /** PCM interface index (starts counting from zero).
       Some devices support multiple PCM interfaces, known as PCM highways.
       This value is always set to zero in case the device has only one
       PCM highway. */
   IFX_uint16_t                  nHighway;
   /** PCM interface mode (master or slave mode). */
   IFX_TAPI_PCM_IF_MODE_t        nOpMode;
   /** DCL frequency to be used in the master and/or slave mode. */
   IFX_TAPI_PCM_IF_DCLFREQ_t     nDCLFreq;
   /** Activation/deactivation of the double clock mode.

   - 0: IFX_DISABLE, (Default) single clocking is used.
   - 1: IFX_ENABLE, Double clocking is used. */
   IFX_operation_t               nDoubleClk;
   /** Slope to be considered for the PCM transmit direction.*/
   IFX_TAPI_PCM_IF_SLOPE_t       nSlopeTX;
   /** Slope to be considered for the PCM receive direction. */
   IFX_TAPI_PCM_IF_SLOPE_t       nSlopeRX;
   /** Bit offset for TX time slot.*/
   IFX_TAPI_PCM_IF_OFFSET_t      nOffsetTX;
   /** Bit offset for RX time slot.*/
   IFX_TAPI_PCM_IF_OFFSET_t      nOffsetRX;
   /** Drive mode for bit 0.*/
   IFX_TAPI_PCM_IF_DRIVE_t       nDrive;
   /** Enable/disable shift access edge; shift the access edges by one clock
      cycle.

   - 0: IFX_DISABLE, No shift takes place.
   - 1: IFX_ENABLE, Shift takes place.
   Note: this setting is defined only in double clock mode.*/
   IFX_operation_t               nShift;
   /** Reserved; PCM chip-specific settings. Set this field to 0x00 if
    not advised otherwise by the Lantiq support team. */
   IFX_uint8_t                   nMCTS;
} IFX_TAPI_PCM_IF_CFG_t;

/** Structure for PCM channel configuration.  */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** PCM time slot for the receive direction. */
   IFX_uint32_t               nTimeslotRX;
   /** PCM time slot for the transmit direction. */
   IFX_uint32_t               nTimeslotTX;
   /** Defines the PCM highway number which is connected to the channel. */
   IFX_uint32_t               nHighway;
   /** Defines the PCM interface coding; values defined in
   \ref IFX_TAPI_PCM_RES_t. */
   IFX_uint32_t               nResolution;
   /** Enable sample to time-slot swap; to be used if the
   attached PCM device has a different sample to time-slot mapping;
   valid values are defined in \ref IFX_TAPI_PCM_SAMPLE_SWAP_t. */
   IFX_TAPI_PCM_SAMPLE_SWAP_t nSampleSwap;
   /** Configure ADPCM bit-packing in PCM time slots.
   Valid values are defined in \ref IFX_TAPI_PCM_BITPACK_t. */
   IFX_TAPI_PCM_BITPACK_t     nBitPacking;
} IFX_TAPI_PCM_CFG_t;

/** Configuration structure for HDLC channel support */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** PCM time slot for PCM HDLC access. */
   IFX_uint32_t      nTimeslot;
   /** Activation/deactivation of PCM HDLC access. */
   IFX_operation_t   nEnable;
} IFX_TAPI_PCM_HDLC_CFG_t;

/** Structure used to set up a PCM loop. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** First PCM time slot read on DD, write on DU. */
   IFX_uint8_t nTimeslot1;
   /** Second PCM time slot read on DU, write on DD. */
   IFX_uint8_t nTimeslot2;
   /** Activation/deactivation of the PCM loop. */
   IFX_operation_t   nEnable;
} IFX_TAPI_PCM_LOOP_CFG_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_PCM_ACTIVATION_GET and
      \ref IFX_TAPI_PCM_ACTIVATION_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** PCM enable or disable. */
      IFX_operation_t mode;
   }IFX_TAPI_PCM_ACTIVATION_t;
#else /* TAPI_ONE_DEVNODE */
   /** PCM enable or disable.
      Type used by \ref IFX_TAPI_PCM_ACTIVATION_GET and
      \ref IFX_TAPI_PCM_ACTIVATION_SET. */
   typedef IFX_operation_t IFX_TAPI_PCM_ACTIVATION_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_PCM_DEC_HP_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Enable or disable the HP filter. */
      IFX_operation_t hpMode;
   }IFX_TAPI_PCM_DEC_HP_CFG_t;
#else /* TAPI_ONE_DEVNODE */
   /** Enable or disable the HP filter.
      Structure used by \ref IFX_TAPI_PCM_DEC_HP_SET. */
   typedef IFX_operation_t IFX_TAPI_PCM_DEC_HP_CFG_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================== */
/* TAPI Fax T.38 Stack, structures (Group TAPI_INTERFACE_FAX_STACK)         */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX_STACK */
/*@{*/

/** T.38 session parameters; used by \ref IFX_TAPI_T38_SESS_START.
   \remarks Only the UDP protocol is supported; TCP connections are not supported.
*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t                  dev;
   /** Channel 'module' index. */
   IFX_uint16_t                  ch;
   /** Rate management method. */
   IFX_TAPI_T38_RMM_t            nRateManagement;
   /** Supported transport protocol. */
   IFX_TAPI_T38_PROTOCOL_t       nProtocol;
   /** Facsimile image conversion options. */
   IFX_TAPI_T38_FACSIMILE_CNVT_t FacConvOpt;
   /** Maximum bit rate (in bit/s). The maximum bit rate can be configured to
     restrict the facsimile page transfer rate. */
   IFX_uint32_t                  nBitRateMax;
   /** UDP error correction method. */
   IFX_TAPI_T38_EC_MODE_t        nUDPErrCorr;
   /** UDP maximum buffer size; the maximum number of octets
      that can be stored on the remote device before
      an overflow occurs. */
   IFX_uint16_t                  nUDPBuffSizeMax;
   /** UDP maximum datagram size; the maximum size of a
      UDPTL packet that is accepted by the remote
      device. */
   IFX_uint16_t                  nUDPDatagramSizeMax;
   /** T.38 ASN.1 version. */
   IFX_uint8_t                   nT38Ver;
} IFX_TAPI_T38_SESS_CFG_t;

/** T.38 session stop; used by \ref IFX_TAPI_T38_SESS_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_T38_SESS_STOP_t;

/** T.38 features used and session status;
    used for \ref IFX_TAPI_T38_SESS_STATISTICS_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t                  dev;
   /** Channel 'module' index. */
   IFX_uint16_t                  ch;
   /** T.38 session flags; selects the standard used for fax T.38. */
   IFX_TAPI_T38_SESS_FLAGS_t     SessInfo;
   /** Fax data pump standards used during the session. */
   IFX_TAPI_T38_FDPSTD_t         nFdpStand;
   /** Number of lost packets. */
   IFX_uint32_t                  nPktLost;
   /** Number of packets recovered by using an error correction
       mechanism (redundancy or FEC). */
   IFX_uint32_t                  nPktRecov;
   /** Maximum number of consecutively lost packets. */
   IFX_uint32_t                  nPktGroupLost;
   /** State of facsimile transmission. */
   IFX_TAPI_T38_SESS_STATE_t     nFaxSessState;
   /** Number of FTT responses.
       Number of training signals that were assessed by the receiving
       fax machine as not having an acceptable quality. */
   IFX_uint16_t                  nFttRsp;
   /** Number of transmitted pages. */
   IFX_uint16_t                  nPagesTx;
   /** Number of scan line breaks during modulation. */
   IFX_uint32_t                  nLineBreak;
   /** Number of facsimile control frame line breaks during modulation. */
   IFX_uint16_t                  nV21FrmBreak;
   /** Number of ECM frame breaks during modulation. */
   IFX_uint16_t                  nEcmFrmBreak;
   /** Major version of T.38 implementation. */
   IFX_uint16_t                  nT38VerMajor;
   /** Minor version of T.38 implementation. */
   IFX_uint16_t                  nT38VerMin;
} IFX_TAPI_T38_SESS_STATISTICS_t;

/** Capabilities of the T.38 stack implementation.
    Used by \ref IFX_TAPI_T38_CAP_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t                  dev;
   /** Channel 'module' index. */
   IFX_uint16_t                  ch;
   /** Supported transport protocol (multiple bits can be ORed together). */
   IFX_TAPI_T38_PROTOCOL_t       Protocol;
   /** UDP error correction method (multiple bits can be ORed together). */
   IFX_TAPI_T38_EC_CAP_t         UDPErrCorr;
   /** Rate management method for UDP. */
   IFX_TAPI_T38_RMM_t            nUDPRateManagement;
   /** Rate management method for TCP; the TAPI ignores this field if
       the firmware version used does not support the TCP mode. */
   IFX_TAPI_T38_RMM_t            nTCPRateManagement;
   /** Facsimile image conversion options
       (multiple bits can be ORed together). */
   IFX_TAPI_T38_FACSIMILE_CNVT_t FacConvOpt;
   /** Maximum bit rate (in bit/s). The maximum bit rate can be configured to
     restrict the facsimile page transfer rate.*/
   IFX_uint32_t                  nBitRateMax;
   /** UDP maximum buffer size; the maximum number of octets that
      can be stored on the remote device before
      an overflow occurs. */
   IFX_uint16_t                  nUDPBuffSizeMax;
   /** UDP maximum datagram size; the maximum size of a
      UDPTL packet which is accepted by the remote
      device. */
   IFX_uint16_t                  nUDPDatagramSizeMax;
   /** T.38 ASN.1 version */
   IFX_uint8_t                   nT38Ver;
} IFX_TAPI_T38_CAP_t;

/** T.38 initial parameters for the stack session;
   used by \ref IFX_TAPI_T38_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t                  dev;
   /** Channel 'module' index. */
   IFX_uint16_t                  ch;
   /** Data pump demodulation gain.
       The value is given in 1 dB steps and should not exceed 12 dB;
       default value IFX_TAPI_T38_CFG_DEFAULT_GAIN 3.14 dB. */
   IFX_int16_t                   nGainRx;
   /** Data pump modulation gain.
       The value is given in 1 dB steps and should not exceed 12dB;
       default value IFX_TAPI_T38_CFG_DEFAULT_GAIN -3.14 dB. */
   IFX_int16_t                   nGainTx;
   /** The T.38 options define protocol features and can
       be ORed. */
   IFX_TAPI_T38_FEAT_MASK_t      OptionMask;
   /** IFP packets send interval (in ms).
       The firmware sends IFP packets with new data every IFPSI ms.
       The parameter is not applicable to IFP packets that
       do not carry data, such as the T30_INDICATOR packet.
   */
   IFX_TAPI_T38_IFPSI_t          nIFPSI;
   /** Number of packets to calculate FEC;
       only valid for UDP mode.
   */
   IFX_uint16_t                  nPktFec;
   /** Data wait time (in ms); time to buffer T.30 data before start of
       modulation. The TAPI rounds the value to a multiple of 10 ms.
       Range:
       - Minimum: 0 ms
       - Maximum: 1000 ms
   */
   IFX_uint16_t                  nDWT;
   /** Time-out for start of T.38 modulation (in ms).
       Range:
       - Minimum: 2000 ms
       - Maximum: 2600 ms
       Only applicable in case the bit IFX_TAPI_T38_EVENT_LONG is set in
       'StateEventMask'.
   */
   IFX_uint16_t                  nModAutoStartTime;
   /** Time to insert spoofing during automatic modulation (in ms).
       The TAPI rounds the value to a multiple of 10 ms.
       Range:
       - Minimum: 10 ms
       - Maximum: 4100 ms
       Only applicable in case the bit IFX_TAPI_T38_EVENT_LONG is set in
       'StateEventMask'.
   */
   IFX_uint16_t                  nSpoofAutoInsTime;
   /** Desired output power level (in -dBm).
       Range:
       -Minimum: -63 dBm
       -Maximum: 0 dBm
   */
   IFX_uint8_t                   nDbm;
   /** Number of additional recovery data packets sent on high-speed
       fax transmissions; only valid for UDP mode.
   */
   IFX_uint8_t                   nPktRecovHiSpeed;
   /** Number of additional recovery data packets sent on low-speed
       fax transmissions; only valid for UDP mode.
   */
   IFX_uint8_t                   nPktRecovLoSpeed;
   /** Number of additional recovery T30_INDICATOR packets;
       value not greater than 'nPktRecovHiSpeed' and 'nPktRecovLoSpeed'.
   */
   IFX_uint8_t                   nPktRecovInd;
   /** Length (bytes) of valid data in the 'aNsx' field;
       value cannot be bigger than \ref IFX_TAPI_T38_NSXLEN. */
   IFX_uint8_t                   nNsxLen;
   /** Data bytes of NSX field; the amount of valid data is set in 'nNsxLen'.
       Only applicable in case the bit IFX_TAPI_T38_EVENT_NON is set in
       'StateEventMask'.
   */
   IFX_uint8_t                   aNsx[IFX_TAPI_T38_NSXLEN];
} IFX_TAPI_T38_FAX_CFG_t;

/** T.38 fax data pump configuration parameter;
    used by \ref IFX_TAPI_T38_FDP_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t                  dev;
   /** Channel 'module' index. */
   IFX_uint16_t                  ch;
   /** Modulation buffer size (in units of 0.625 ms).
       Range:
       - Minimum: 1 (0.625 ms)
       - Maximum: 320 (200 ms)
       */
   IFX_uint16_t                  nMobsz;
   /** Required modulation buffer fill level (in units of 0.625 ms)
       before modulation starts.
       The modulation is started if the input buffer of the modulator
       contains data that represents at least the time frame of 'nMobsm'.
       Range:
       - Minimum: 1 (0.625 ms)
       - Maximum: 320 (200 ms)
       */
   IFX_uint16_t                  nMobsm;
   /** Required modulation buffer fill level (in units of 0.625 ms)
       before the modulation requests more data.
       The fax data pump requests new data if the
       fill level of the input buffer falls below a value of 'nMobrd'.
       Range:
       - Minimum: 1 (0.625 ms)
       - Maximum: 320 (200 ms)
       */
   IFX_uint16_t                  nMobrd;
   /** Required demodulation buffer level (in units of 0.625 ms)
       before the demodulator sends data.
       The fax data pump sends new data to the T.38
       protocol stack or new data to the host if the output buffer of the
       demodulator holds data representing a time frame of at
       least 'nDmbsd'.
       Range:
       - Minimum: 1 (0.625 ms)
       - Maximum: 144 (90 ms)
       */
   IFX_uint16_t                  nDmbsd;
} IFX_TAPI_T38_FDP_CFG_t;

/** T.38 data pump trace event configuration parameter;
    used by \ref IFX_TAPI_T38_TRACE_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Debug mask for different internal T.38 stack trace events; the debug feature
       is disabled when this field is set to zero. */
   IFX_uint32_t DbgMask;
} IFX_TAPI_T38_TRACE_CFG_t;
/*@}*/ /* TAPI_INTERFACE_FAX_STACK */

/* ======================================================================== */
/* TAPI Fax T.38 Services, structures (Group TAPI_INTERFACE_FAX)            */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** Structure used to set up the modulator for T.38 fax; used for
    \ref IFX_TAPI_T38_MOD_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Selects the standard used for fax T.38 \ref IFX_TAPI_T38_STD_t.

   - 0x00: Silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000
   - 0x09: V.17/14400
   - 0x0A: CNG
   - 0x0B: CED
   */
   IFX_uint8_t    nStandard;
   /** Signal duration in ms; used for the tonal signals (CED, CNG) and silence;
   1 < nSigLen < 4000 [ms].*/
   IFX_uint16_t   nSigLen;
   /** Sets the transmission (downstream) gain in dB; range -24 <= nGainTx <= +12 */
   IFX_int16_t    nGainTx;
   /** Desired output signal power in -dBm. */
   IFX_uint8_t    nDbm;
   /** TEP generation flag; used only by V.27, V.29 and V.17 and ignored in all
    other cases.

   - 0: No TEP generation
   - 1: TEP generation */
   IFX_uint8_t    nTEP;
   /** Training sequence flag; used only by V.17 and ignored in all other cases.

   - 0: Short training sequence
   - 1: Long training sequence */
   IFX_uint8_t    nTraining;
   /** Level required before the modulation starts.*/
   IFX_uint16_t   nMobsm;
   /**Level required before the modulation requests more data.*/
   IFX_uint16_t   nMobrd;
} IFX_TAPI_T38_MOD_DATA_t;

/** Structure used to set up the demodulator for T.38 fax; used for
    \ref IFX_TAPI_T38_DEMOD_START. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Selects the standard used for fax T.38 using \ref IFX_TAPI_T38_STD_t.

   - 0x00: silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000
   - 0x09: V.17/14400 */
   IFX_uint8_t    nStandard1;
   /** Selects the alternative standard used for fax T.38 using
       \ref IFX_TAPI_T38_STD_t.

   - 0x00: Silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000
   - 0x09: V.17/14400
   - 0xFF: Use only standard 1 */
   IFX_uint8_t    nStandard2;
#ifndef TAPI4_DXY_DOC
   /** Signal duration in ms; used for the tonal signals (CED, CNG)
       and silence; 1 < nSigLen < 4000 [ms].*/
   IFX_uint16_t   nSigLen;
#endif /* #ifndef TAPI4_DXY_DOC */
   /** Sets the receive gain for the upstream direction. */
   IFX_int16_t    nGainRx;
   /** Equalizer configuration flag.

   - 0: Reset the equalizer
   - 1: Reuse the equalizer coefficients */
   IFX_uint8_t    nEqualizer;
   /** Training sequence flag; used only by V.17 and ignored in all other cases.

   - 0: Short training sequence
   - 1: Long training sequence */
   IFX_uint8_t    nTraining;
   /** Level required before the demodulator sends data.*/
   IFX_uint16_t   nDmbsd;
} IFX_TAPI_T38_DEMOD_DATA_t;

#ifdef TAPI_ONE_DEVNODE
/** Structure used to stop the modulator or demodulator for T.38 fax. It is
    used for \ref IFX_TAPI_T38_STOP. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_T38_DATAPUMP_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure used to read the T.38 fax status; used for
    \ref IFX_TAPI_T38_STATUS_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** T.38 fax status, refer to \ref IFX_TAPI_FAX_T38_STATUS_t.

   - 0: IFX_TAPI_FAX_T38_DP_OFF, data pump is not active
   - 1: IFX_TAPI_FAX_T38_DP_ON, data pump is active
   - 2: IFX_TAPI_FAX_T38_TX_ON, transmission is active
   - 3: IFX_TAPI_FAX_T38_TX_OFF, transmission is finished
   */
   IFX_uint8_t nStatus;
   /** T.38 fax error, refer to \ref IFX_TAPI_FAX_T38_ERROR_t.

   - 0x00: No error occurred
   - 0x01: Fax error occurred; the fax data pump should be deactivated
   - 0x02: MIPS overload
   - 0x03: Error while reading data
   - 0x04: Error while writing data
   - 0x05: Error while setting up the modulator or demodulator
   */
   IFX_uint8_t nError;
} IFX_TAPI_T38_STATUS_t;

/*@}*/ /* TAPI_INTERFACE_FAX */

/** Structure used to switch loops for testing. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Switch an analog loop in the device. If switched on, signals that are
      played to the subscriber are looped back to the receiving side.

      - 0x0: Analog loop off
      - 0x1: Analog loop on */
   IFX_uint8_t bAnalog;
} IFX_TAPI_TEST_LOOP_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_TEST_HOOKGEN. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Specifies the hook state event to generate. */
      IFX_TAPI_HOOKGEN_t hookMode;
   }IFX_TAPI_TEST_HOOKGEN_t;
#else /* TAPI_ONE_DEVNODE */
   /** Specifies the hook state event to generate.
       Type used by \ref IFX_TAPI_TEST_HOOKGEN. */
   typedef IFX_TAPI_HOOKGEN_t IFX_TAPI_TEST_HOOKGEN_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_TEST */

#ifndef TAPI4_DXY_DOC
/* ======================================================================== */
/* TAPI Polling Services (Group TAPI_POLLING_SERVICE)               */
/* ======================================================================== */
/** \addtogroup TAPI_POLLING_SERVICE */
/*@{*/

/** Maximum size of a TAPI packet. */
#define IFX_TAPI_POLL_PKT_SIZE      256

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_TEST_HOOKGEN. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
} IFX_TAPI_POLL_DEV_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure for polling packet handling.*/
typedef struct
{
   /** Packet header section. */
   IFX_uint32_t dev   : 8;  /** Channel identifier [0,1,...].*/
   IFX_uint32_t ch    : 8;  /** Device identifier [0,1,...]. */
   IFX_uint32_t type  : 4;  /** Packet length in bytes. */
   IFX_uint32_t len   : 12; /** Packet type identifier. */

   /** Packet payload section. */
   IFX_uint16_t data[IFX_TAPI_POLL_PKT_SIZE];
} IFX_TAPI_POLL_PKT_t;

/** TAPI driver global polling configuration structure. */
typedef struct
{
   /** Pointer to the buffer pool control structure. The pointer is used
       together with the buffer get (* getBuf) routine to identify the
       buffer pool used.
   */
   IFX_void_t     *pBufPool;
   /** Pointer to a function used to get an empty buffer from the buffer pool.
       The buffer is protected and can be exclusively used by the user. The
       buffer size is pre-defined at buffer pool initialization time. */
   IFX_void_t     *(*get) (IFX_void_t *pBufPool);
   /** Pointer to a function pointer used to return a used buffer back to the
       buffer pool. */
   IFX_int32_t    (*put) (IFX_void_t *pBuf);
} IFX_TAPI_POLL_CONFIG_t;

#ifdef VXWORKS
/**
   Polls for packets for all devices registered for packet polling.

   \param   **ppPktsUp     - Points to an array of buffer pointers to free
                             bufers, provided for the reading of new packets.
   \param   *pPktsUpNum  -   On entry, gives the number of free buffers provided
                             in the array ppPkts. On exit, returns the number of
                             packets read.

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
extern IFX_int32_t TAPI_Poll_Up (IFX_void_t **ppPktsUp, IFX_int32_t *pPktsUpNum);

/**
   Send packets to the VINETIC mailbox (non-interleaved mode)

   \param   **ppPktsDown - Pointer to the array of buffer pointers
   \param   pPktsDownNum   - Number of valid buffer pointers in the array

   \return Returns the following values:
      - IFX_SUCCESS: if successful
      - IFX_ERROR: in case of an error
*/
extern IFX_int32_t TAPI_Poll_Down (IFX_void_t **ppPktsDown, IFX_int32_t *pPktsDownNum);

/**
   Reads interrupt status registers and updates events for each device.

   \remarks C++ does not allow a typecast to void as parameter. Type void must
   be used instead.
*/
extern IFX_void_t  TAPI_Poll_Events(void);
#endif /* VXWORKS */

/*@}*/ /* TAPI_POLLING_SERVICE */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ======================================================================== */
/* TAPI FXO Services, structures (Group TAPI_INTERFACE_FXO)                 */
/* ======================================================================== */
#ifndef TAPI4_DXY_DOC
/** \addtogroup TAPI_INTERFACE_FXO */
/*@{*/

#ifdef TAPI_ONE_DEVNODE
/** Structure including the digits to be dialed used in
    \ref IFX_TAPI_FXO_DIAL_STOP.
*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index.. */
   IFX_uint16_t ch;
} IFX_TAPI_FXO_DIAL_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure including the digits to be dialed used in
    \ref IFX_TAPI_FXO_DIAL_START.
*/
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** The number of digits to be dialed. */
   IFX_uint8_t nDigits;
   /** The string of digits to be dialed.
    Note: only 0-9 and A,B,C,D are supported. */
   IFX_char_t  data [IFX_TAPI_FXO_DIAL_DIGITS];
} IFX_TAPI_FXO_DIAL_t;

/** Structure for FXO dialing configuration, used in
 \ref IFX_TAPI_FXO_DIAL_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Time between two digits in ms; default 100 ms.*/
   IFX_uint16_t nInterDigitTime;
   /** Play time for each digit in ms; default 100 ms.*/
   IFX_uint16_t nDigitPlayTime;
} IFX_TAPI_FXO_DIAL_CFG_t;

/** Hook configuration for FXO, used in \ref IFX_TAPI_FXO_FLASH_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Duration of a flash-hook; default 100 ms.*/
   IFX_uint32_t nFlashTime;
} IFX_TAPI_FXO_FLASH_CFG_t;

/** OSI configuration for FXO, used in \ref IFX_TAPI_FXO_OSI_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Duration of an OSI; default 200 ms.*/
   IFX_uint32_t nOSIMax;
} IFX_TAPI_FXO_OSI_CFG_t;

/** Defines the possible hook status for FXO. */
typedef enum
{
   /** On-hook. */
   IFX_TAPI_FXO_HOOK_ONHOOK  = 0,
   /** Off-hook. */
   IFX_TAPI_FXO_HOOK_OFFHOOK = 1
} IFX_TAPI_FXO_HOOK_t;

/** Defines the possible line modes for FXO, used in \ref IFX_TAPI_FXO_LINE_MODE_t. */
typedef enum
{
   /** Disabled. */
   IFX_TAPI_FXO_LINE_MODE_DISABLED = 0,
   /** Active. */
   IFX_TAPI_FXO_LINE_MODE_ACTIVE = 1
} IFX_TAPI_FXO_LINE_MODES_t;

/** Line mode for FXO, used in \ref IFX_TAPI_FXO_LINE_MODE_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Line mode. */
   IFX_TAPI_FXO_LINE_MODES_t mode;
} IFX_TAPI_FXO_LINE_MODE_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_FXO_APOH_GET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** APOH status:
         - IFX_ENABLE if APOH condition is verified
         - IFX_DISABLE otherwise */
      IFX_enDis_t mode;
   }IFX_TAPI_FXO_APOH_t;
#else /* TAPI_ONE_DEVNODE */
   /** APOH status:
      - IFX_ENABLE if APOH condition is verified
      - IFX_DISABLE otherwise
      Used by \ref IFX_TAPI_FXO_APOH_GET. */
   typedef IFX_enDis_t IFX_TAPI_FXO_APOH_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_FXO_BAT_GET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Battery status:
         - IFX_ENABLE if the FXO port is disconnected from the PSTN
            (battery absent)
         - IFX_DISABLE if the FXO port is connected to the PSTN
            (battery present) */
      IFX_enDis_t mode;
   }IFX_TAPI_FXO_BAT_t;
#else /* TAPI_ONE_DEVNODE */
   /** Battery status:
      - IFX_ENABLE if the FXO port is disconnected from the PSTN
         (battery absent)
      - IFX_DISABLE if the FXO port is connected to the PSTN
         (battery present)
      Used by \ref IFX_TAPI_FXO_BAT_GET */
   typedef IFX_enDis_t IFX_TAPI_FXO_BAT_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
/** Structure used by \ref IFX_TAPI_FXO_FLASH_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
}IFX_TAPI_FXO_FLASH_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_FXO_HOOK_SET and \ref
       IFX_TAPI_FXO_HOOK_GET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Specifies the hook state of the FXO line.*/
      IFX_TAPI_FXO_HOOK_t hookMode;
   }IFX_TAPI_FXO_HOOK_CFG_t;
#else /* TAPI_ONE_DEVNODE */
   /** Type used by \ref IFX_TAPI_FXO_HOOK_SET and \ref
       IFX_TAPI_FXO_HOOK_GET. */
   typedef IFX_TAPI_FXO_HOOK_t IFX_TAPI_FXO_HOOK_CFG_t;
#endif /* TAPI_ONE_DEVNODE */

   /** Structure used by \ref IFX_TAPI_FXO_POLARITY_GET. */
#ifdef TAPI_ONE_DEVNODE
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Polarity mode:
         - IFX_ENABLE reflects normal polarity
         - IFX_DISABLE reflects reversed polarity */
      IFX_enDis_t polarityMode;
   }IFX_TAPI_FXO_POLARITY_t;
#else /* TAPI_ONE_DEVNODE */
   typedef IFX_enDis_t IFX_TAPI_FXO_POLARITY_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure used by \ref IFX_TAPI_FXO_RING_GET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Indicate the ringing status of the FXO line:
         - IFX_ENABLE the line is ringing
         - IFX_DISABLE the line is not ringing */
      IFX_enDis_t ringMode;
   } IFX_TAPI_FXO_RING_STATUS_t;
#else /* TAPI_ONE_DEVNODE */
   /** Type used by \ref IFX_TAPI_FXO_RING_GET. */
   typedef IFX_enDis_t IFX_TAPI_FXO_RING_STATUS_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_FXO */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI_DXY_DOC
/** Return code classes for error handling. */
typedef enum
{
   /** Specifies a generic status result or error. */
   TAPI_statusClassSuccess    = 0x0000,
   /** Specifies a channel-related error in addition to the other classes. */
   TAPI_statusClassCh         = 0x1000,
   /** Specifies a warning or information that does not harm the system
       if handled correctly. The upper layers handle this result as an error and this
       may be signaled to the application. */
   TAPI_statusClassWarn       = 0x4000,
   /** Specifies a general error, which may lead to function failure of at least
       that channel or feature. */
   TAPI_statusClassErr        = 0x6000,
   /** Specifies a critical error; device or driver maybe out of function. */
   TAPI_statusClassCritical   = 0x8000
}TAPI_statusClass_t;
#endif /* TAPI_DXY_DOC */

/** This macro checks whether the returned result is successful or not.
    If the return value is IFX_ERROR, it returns IFX_FALSE. Otherwise the
    code is checked to see whether any of the classes Err, Warn or Critical are set
    and also returns IFX_FALSE if set, otherwise IFX_TRUE.

   \remarks
      With highest probability, the error code will be IFX_SUCCESS. To decrease
      'if' condition calculation we should check the IFX_SUCCESS at the beginning
      and return the pre-defined result immediately.
*/
#define TAPI_SUCCESS(code)                                                   \
({                                                    \
   register IFX_uint32_t _err = (IFX_uint32_t)(code); \
   register IFX_boolean_t _res = IFX_TRUE;            \
   if (IFX_SUCCESS != _err) {                         \
      if (IFX_ERROR == _err) {                        \
         _res = IFX_FALSE;                            \
      } else {                                        \
         _err = (((IFX_uint16_t)(_err)) & 0xFFFF) |   \
                (((IFX_uint32_t)(_err)) >> 16);       \
         if (_err & (TAPI_statusClassErr |            \
                     TAPI_statusClassWarn |           \
                     TAPI_statusClassCritical)) {     \
            _res = IFX_FALSE;                         \
         }                                            \
      }                                               \
   }                                                  \
   _res;                                              \
})
/* \todo: correct error handling */

#ifndef TAPI4_DXY_DOC
/* ===================================================================== */
/* TAPI DECT Services, structures (Group TAPI_INTERFACE_DECT)            */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_DECT */
/*@{*/

   /** Structure for DECT channel activation by \ref IFX_TAPI_DECT_ACTIVATION_SET. */
#ifdef TAPI_ONE_DEVNODE
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /* Specifies activation or deactivation.. */
      IFX_enDis_t nEnable;
   } IFX_TAPI_DECT_ACTIVATION_t;
#else /* TAPI_ONE_DEVNODE */
   typedef IFX_enDis_t IFX_TAPI_DECT_ACTIVATION_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure for DECT channel configuration by \ref IFX_TAPI_DECT_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Delay from the start of the decoder to the start of
   the encoder in steps of 2.5 ms; range 0 ms - 10 ms. */
   IFX_uint32_t   nEncDelay;
   /** Delay from the arrival of the first packet to the start
   of the decoder in steps of 2.5 ms; range 0 ms - 10 ms. */
   IFX_uint32_t   nDecDelay;
} IFX_TAPI_DECT_CFG_t;

/** Structure used as parameter by \ref IFX_TAPI_DECT_ENC_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Encoder type. */
   IFX_TAPI_DECT_ENC_TYPE_t   nEncType;
   /** Frame length. */
   IFX_TAPI_DECT_ENC_LENGTH_t nFrameLen;
} IFX_TAPI_DECT_ENC_CFG_t;

/** Structure to return DECT channel statistics data,
    used by \ref IFX_TAPI_DECT_STATISTICS_GET. */
typedef struct
{
   /** Reset the counters after reading: 0 do not reset <> 0 reset counters. */
   IFX_uint8_t nReset;
   /** Host to DECT handset packet count: the total number of upstream DECT
       data packets (host to DECT handset) transmitted since the start of
       transmission (voice packets). */
   IFX_uint32_t nPktUp;
   /** FP to host-packet count: the total number of downstream DECT data
       packets (DECT handset to host) received since starting transmission
       (voice packets). */
   IFX_uint32_t nPktDown;
   /** Number of SID packets received from the DECT handset. */
   IFX_uint32_t nSid;
   /** Number of PLC packets received from the DECT handset. */
   IFX_uint32_t nPlc;
   /** Number of DECT handset to host buffer overflows: number of packets that
       have to be discarded due to overflow. */
   IFX_uint32_t nOverflows;
   /** Number of DECT handset to host buffer underflows: number of decoder
       buffer underflows every 2.5 ms. The decoder runs on 2.5 ms packets. */
   IFX_uint32_t nUnderflows;
   /** Number of DECT handset to host invalid packets: number of invalid
       packets that arrive downstream from the handset. */
   IFX_uint32_t nInvalid;
} IFX_TAPI_DECT_STATISTICS_t;

/** Structure for DECT echo canceller configuration,
    used by \ref IFX_TAPI_DECT_EC_CFG_SET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** EC type selection:
     - 0x00: IFX_TAPI_EC_TYPE_OFF, no EC or echo suppressor
     - 0x01: IFX_TAPI_EC_TYPE_ES, echo suppressor */
   IFX_TAPI_EC_TYPE_t nType;
} IFX_TAPI_DECT_EC_CFG_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure for DECT channel configuration by \ref IFX_TAPI_TONE_DECT_PLAY_t. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Tone index. */
      IFX_int32_t nToneIndex;
   } IFX_TAPI_TONE_DECT_PLAY_t;
#else /* TAPI_ONE_DEVNODE */
   /** Tone index.
       Type for DECT channel configuration by \ref IFX_TAPI_TONE_DECT_PLAY_t. */
   typedef IFX_int32_t IFX_TAPI_TONE_DECT_PLAY_t;
#endif /* TAPI_ONE_DEVNODE */

#ifdef TAPI_ONE_DEVNODE
   /** Structure for DECT channel configuration by \ref IFX_TAPI_TONE_DECT_STOP. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
   } IFX_TAPI_TONE_DECT_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_DECT */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI4_DXY_DOC
/* ===================================================================== */
/* TAPI GR909 Services, structures (Group TAPI_INTERFACE_GR909)          */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_GR909 */
/*@{*/

/** GR909 test start */
typedef struct
{
   /** Device number (in). */
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
   /** GR909 powerline frequency to use.
       See \ref IFX_TAPI_GR909_POWERLINE_FREQ_t. */
   IFX_TAPI_GR909_POWERLINE_FREQ_t     pl_freq;
   /** GR909 test mask as value or combination of tests in
       \ref IFX_TAPI_GR909_TEST_t. */
   IFX_uint32_t                        test_mask;
} IFX_TAPI_GR909_START_t;

#ifdef TAPI_ONE_DEVNODE
   /** GR909 test stop. */
   typedef struct
   {
      /** Device number (in). */
      IFX_uint16_t dev;
      /** Channel number (in). */
      IFX_uint16_t ch;
   } IFX_TAPI_GR909_STOP_t;
#endif /* TAPI_ONE_DEVNODE */

/** GR909 results. */
typedef struct
{
   /** Device number (in). */
#ifdef TAPI_ONE_DEVNODE
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
#endif /* TAPI_ONE_DEVNODE */
   /** Device type, see \ref IFX_TAPI_GR909_DEV_t. */
   IFX_TAPI_GR909_DEV_t dev_type;
   /** Valid results flag, see \ref IFX_TAPI_GR909_TEST_t. */
   IFX_uint32_t         valid;
   /** Passed flag according to valid flag, see \ref IFX_TAPI_GR909_TEST_t. */
   IFX_uint32_t         passed;
   /** HPT AC RING wire to GND result. */
   IFX_int16_t          HPT_AC_R2G;
   /** HPT AC TIP wire to GND result. */
   IFX_int16_t          HPT_AC_T2G;
   /** HPT AC TIP wire to RING wire result. */
   IFX_int16_t          HPT_AC_T2R;
   /** HPT DC RING wire to GND result. */
   IFX_int16_t          HPT_DC_R2G;
   /** HPT DC TIP wire to GND result. */
   IFX_int16_t          HPT_DC_T2G;
   /** HPT DC TIP wire to RING wire result. */
   IFX_int16_t          HPT_DC_T2R;
   /** FEMF AC RING wire to GND result. */
   IFX_int16_t          FEMF_AC_R2G;
   /** FEMF AC TIP wire to GND result. */
   IFX_int16_t          FEMF_AC_T2G;
   /** FEMF AC TIP wire to RING wire result. */
   IFX_int16_t          FEMF_AC_T2R;
   /** FEMF DC RING wire to GND result. */
   IFX_int16_t          FEMF_DC_R2G;
   /** FEMF DC TIP wire to GND result. */
   IFX_int16_t          FEMF_DC_T2G;
   /** FEMF DC TIP wire to RING wire result.. */
   IFX_int16_t          FEMF_DC_T2R;
   /** RFT RING wire to GND result. */
   IFX_int16_t          RFT_R2G;
   /** RFT TIP wire to GND result. */
   IFX_int16_t          RFT_T2G;
   /** RFT TIP wire to RING wire result. */
   IFX_int16_t          RFT_T2R;
   /** ROH TIP wire to RING wire result for low voltage. */
   IFX_int16_t          ROH_T2R_L;
   /** ROH TIP wire to RING wire result for high voltage. */
   IFX_int16_t          ROH_T2R_H;
   /** RIT result. */
   IFX_int16_t          RIT_RES;
} IFX_TAPI_GR909_RESULT_t;

/*@}*/ /* TAPI_INTERFACE_GR909 */
#endif /* #ifndef TAPI4_DXY_DOC */

#ifndef TAPI4_DXY_DOC
/* ========================================================================== */
/* TAPI FXS Phone Detection Services, structures                              */
/* (Group TAPI_INTERFACE_PHONE_DETECTION)                                     */
/* ========================================================================== */
/** \addtogroup TAPI_INTERFACE_PHONE_DETECTION */
/*@{*/

#ifdef TAPI_ONE_DEVNODE
/** Structure used during start of an analog line capacitance measurement.
    Used by \ref IFX_TAPI_LINE_MEASURE_CAPACITANCE_START
    \ref IFX_TAPI_LINE_MEASURE_CAPACITANCE_STOP. */
typedef struct {
   /** Device number (in). */
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
} IFX_TAPI_LINE_MEASURE_CAPACITANCE_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure used to configure the FXS phone-plug detection state machine.*/
typedef struct {
   /** Telephone capacitance threshold [nF]. A telephone is detected in case
       the measured capacitance is above this given threshold.
       Default: 20 nF. */
   IFX_uint32_t nCapacitance;
   /** Off-hook detection timer (T3) value [ms]. The line feeding power is
       enabled for off-hook detection. It is reduced again in case no off-hook
       event detects a telephone within the given time period.
       Default: 200 ms. */
   IFX_uint32_t nOffHookTime;
   /** Find phone detection period (T2) [ms]. The phone detection period, off-hook detection,
       to find a newly connected phone.
       Default: 3000 ms. */
   IFX_uint32_t nFindPeriod;
   /** Lost phone detection period (T1) [s]. The phone detection period,
       capacitance measurement, to ensure that the telephone is still connected.
       Default: 3600 s. */
   IFX_uint32_t nLostPeriod;
} IFX_TAPI_LINE_PHONE_DETECT_CFG_t;

/*@}*/ /* TAPI_INTERFACE_PHONE_DETECTION */
#endif /* #ifndef TAPI4_DXY_DOC */

/* ===================================================================== */
/* TAPI Network Line-Testing Services, structures                          */
/* (Group TAPI_INTERFACE_NLT)                                            */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_NLT */
/*@{*/

/** List of NLT test identifiers. Used by the TAPI
   ioctl commands IFX_TAPI_NLT_START and IFX_TAPI_NLT_RESULT_GET. */
typedef enum
{
   /** NLT AC meter measurement ID. */
   IFX_TAPI_NLT_AC_METER_ID,
   /** NLT AC transhybrid measurement ID. */
   IFX_TAPI_NLT_AC_TRANSHYBRID_ID,
   /** NLT AC frequency response measurement ID. */
   IFX_TAPI_NLT_AC_FREQRESPONSE_ID,
   /** NLT AC idle noise measurement ID. */
   IFX_TAPI_NLT_AC_IDLENOISE_ID,
   /** NLT AC gain tracking measurement ID. */
   IFX_TAPI_NLT_AC_GAINTRACKING_ID,
   /** NLT DC meter measurement ID. */
   IFX_TAPI_NLT_DC_METER_ID,
   /** NLT current measurement ID. */
   IFX_TAPI_NLT_CURRENT_ID,
   /** NLT voltage measurement ID. */
   IFX_TAPI_NLT_VOLTAGE_ID,
   /** NLT to ground measurement ID. */
   IFX_TAPI_NLT_TOGROUND_ID,
   /** NLT tip-ring measurement ID. */
   IFX_TAPI_NLT_TIPRING_ID,
   /** NLT tip-ring measurement (results 2) ID. */
   IFX_TAPI_NLT_TIPRING2_ID,
   /** NLT network (ultimate) measurement ID. */
   IFX_TAPI_NLT_NETWORK_ID,
   /** NLT admittance measurement ID. */
   IFX_TAPI_NLT_ADMITTANCE_ID,
   /** None. */
   IFX_TAPI_NLT_NONE
} IFX_TAPI_NLT_TESTID_t;

/** Structure used during start of an NLT test. Used by
    \ref IFX_TAPI_NLT_TEST_START. */
typedef struct
{
   /** Device number (in). */
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
   /** Test start status (out). */
   IFX_int32_t nStatus;
   /** When set to IFX_TRUE, the calling process is put to sleep on test start.
       The process is woken up upon reception of the LTEST (in). */
   IFX_boolean_t bBlock;
   /** Identifier of test to be started (in). */
   IFX_TAPI_NLT_TESTID_t testID;
   /** Where required, configuration arguments relative to the test are passed.
       The TAPI does not have to know about the details, therefore the
       details are known by the LL driver and NLT Library, only (in). */
   IFX_void_t *pTestCfg;
} IFX_TAPI_NLT_TEST_START_t;

/** Structure used for reading NLT test results. Used by
    \ref IFX_TAPI_NLT_RESULT_GET. */
typedef struct
{
   /** Device number (in). */
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
   /** Result get status (out). */
   IFX_int32_t nStatus;
   /** Identifier of test from which results are to be read (in). */
   IFX_TAPI_NLT_TESTID_t testID;
   /** When the respective NLT LIB API is called to retrieve the results
       from a test previously started (non-blocking call), it might be necessary,
       depending on the test, to retrieve the configuration parameters which were
       used during the test. These configuration parameters could be required for
       the calculation of the final results. For this reason, the caller
       where needed passes a pointer (else NULL) of the related type, so that the LL
       implementation copies in the cached configuration parameters used by the
       last test started (in). */
   IFX_void_t *pTestCfg;
   /** Pointer to a data structure large enough to accumulate the results
       related to the NLT test. The TAPI does not have to know about the details,
       therefore the details are known by the LL driver and NLT Library (in). */
   IFX_void_t *pTestResults;
} IFX_TAPI_NLT_RESULT_GET_t;

/*@}*/ /* TAPI_INTERFACE_NLT */

/* ======================================================================= */
/* TAPI Message Waiting Lamp Services, structures                          */
/* (Group TAPI_INTERFACE_MWL)                                              */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_MWL */
/*@{*/

/** Structure for message waiting lamp configuration; used by
    \ref IFX_TAPI_MWL_ACTIVATION_SET and \ref IFX_TAPI_MWL_ACTIVATION_GET. */
typedef struct
{
   /** Device number (in). */
   IFX_uint16_t dev;
   /** Channel number (in). */
   IFX_uint16_t ch;
   /** Activation of the message waiting lamp. */
   IFX_enDis_t      nActivation;
} IFX_TAPI_MWL_ACTIVATION_t;

/*@}*/ /* TAPI_INTERFACE_MWL */

/* ======================================================================= */
/* TAPI Level Peak Detector Services, structures                           */
/* (Group TAPI_INTERFACE_PEAKD)                                            */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_PEAKD */
/*@{*/

/** Tone level peak detector configuration structure.
    Used by \ref IFX_TAPI_PEAK_DETECT_START and
    \ref IFX_TAPI_PEAK_DETECT_STOP. */
typedef struct
{
   IFX_uint16_t dev;
   IFX_uint16_t ch;
   IFX_TAPI_MODULE_TYPE_t module;
   IFX_uint32_t external:1;
   IFX_uint32_t internal:1;
   IFX_uint32_t reserved:30;
} IFX_TAPI_PEAK_DETECT_CFG_t;

/** Data structure used to retrieve the measured value of the peak detector.
    Used by \ref IFX_TAPI_PEAK_DETECT_RESULT_GET. */
typedef struct
{
   IFX_uint16_t dev;
   IFX_uint16_t ch;
   IFX_TAPI_MODULE_TYPE_t module;
   IFX_uint32_t external:1;
   IFX_uint32_t internal:1;
   IFX_uint32_t reserved:30;
   IFX_int16_t nResult;
} IFX_TAPI_PEAK_DETECT_RESULT_GET_t;

/*@}*/ /* TAPI_INTERFACE_PEAKD */

/* ===================================================================== */
/* TAPI Event Services, structures (Group TAPI_INTERFACE_EVENT)          */
/* ===================================================================== */

/** \addtogroup TAPI_INTERFACE_EVENT */
/*@{*/

/* =============================== */
/* Macros                          */
/* =============================== */

#define IFX_TAPI_EVENT_TYPE_MASK                              0xFFFF0000
#define IFX_TAPI_EVENT_SUBTYPE_MASK                           0x0000FFFF
#define IFX_TAPI_EVENT_TYPE_FAULT_MASK                        0xF0000000

/* =============================== */
/* enum                            */
/* =============================== */

/** List of event types */
typedef enum
{
   /** Reserved. */
   IFX_TAPI_EVENT_TYPE_NONE                                 = 0x00000000,
   /** Event on GPIOs, channel IOs. */
   IFX_TAPI_EVENT_TYPE_IO_GENERAL                           = 0x10000000,
   /** Reserved; external interrupt. */
   IFX_TAPI_EVENT_TYPE_IO_INTERRUPT                         = 0x11000000,
   /** Ringing, hook events. */
   IFX_TAPI_EVENT_TYPE_FXS                                  = 0x20000000,
   /** Ringing, polarity reversal.*/
   IFX_TAPI_EVENT_TYPE_FXO                                  = 0x21000000,
   /** Line-testing events. */
   IFX_TAPI_EVENT_TYPE_LT                                   = 0x29000000,
   /** Pulse digit detected. */
   IFX_TAPI_EVENT_TYPE_PULSE                                = 0x30000000,
   /** DTMF digit detected. */
   IFX_TAPI_EVENT_TYPE_DTMF                                 = 0x31000000,
   /** Caller ID events. */
   IFX_TAPI_EVENT_TYPE_CID                                  = 0x32000000,
   /** Tone generation event, e.g. tone generation ended. */
   IFX_TAPI_EVENT_TYPE_TONE_GEN                             = 0x33000000,
   /** Tone detection event, e.g. call progress tones. */
   IFX_TAPI_EVENT_TYPE_TONE_DET                             = 0x34000000,
   /** Detection of fax/modem and V.18 signals. */
   IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL                      = 0x35000000,
   /** Calibration events. */
   IFX_TAPI_EVENT_TYPE_CALIBRATION                          = 0x36000000,
   /** Linear channel events. */
   IFX_TAPI_EVENT_TYPE_LIN                                  = 0x37000000,
   /** Metering event. */
   IFX_TAPI_EVENT_METERING                                  = 0x38000000,
   /** E.g. vocoder changed. */
   IFX_TAPI_EVENT_TYPE_COD                                  = 0x40000000,
   /** Reserved. */
   IFX_TAPI_EVENT_TYPE_RTP                                  = 0x41000000,
   /** Reserved for AAL events. */
   IFX_TAPI_EVENT_TYPE_AAL                                  = 0x42000000,
   /** RFC 2833 frame detected. */
   IFX_TAPI_EVENT_TYPE_RFC2833                              = 0x43000000,
#ifndef TAPI4_DXY_DOC
   /** KPI interface. */
   IFX_TAPI_EVENT_TYPE_KPI                                  = 0x44000000,
#endif /* #ifndef TAPI4_DXY_DOC */
   /** T.38 events. */
   IFX_TAPI_EVENT_TYPE_T38                                  = 0x50000000,
   /** JB events; maybe not required. */
   IFX_TAPI_EVENT_TYPE_JB                                   = 0x60000000,
   /** E.g. FW download finished, bad CRC */
   IFX_TAPI_EVENT_TYPE_DOWNLOAD                             = 0x70000000,
   /** Reserved for future use. */
   IFX_TAPI_EVENT_TYPE_INFO                                 = 0xA0000000,
   /** Debug information, e.g. dump of some registers or memory areas. */
   IFX_TAPI_EVENT_TYPE_DEBUG                                = 0xD0000000,
   /** Events of the low-level driver. */
   IFX_TAPI_EVENT_TYPE_LL_DRIVER                            = 0xE0000000,
   /** Reserved. */
   IFX_TAPI_EVENT_TYPE_FAULT_GENERAL                        = 0xF1000000,
   /** E.g. over-temperature, ground key detected. */
   IFX_TAPI_EVENT_TYPE_FAULT_LINE                           = 0xF2000000,
   /** (Reserved) e.g. watchdog, PLL. */
   IFX_TAPI_EVENT_TYPE_FAULT_HW                             = 0xF3000000,
   /** (Reserved) e.g. mailbox error. */
   IFX_TAPI_EVENT_TYPE_FAULT_FW                             = 0xF4000000,
   /** (Reserved). */
   IFX_TAPI_EVENT_TYPE_FAULT_SW                             = 0xF5000000,
   /** HDLC errors. */
   IFX_TAPI_EVENT_TYPE_FAULT_HDLC                           = 0xF6000000
} IFX_TAPI_EVENT_TYPE_t;

/** List of event IDs */
typedef enum
{
   /* NONE (reserved) */
   /** Reserved. */
   IFX_TAPI_EVENT_NONE                 = IFX_TAPI_EVENT_TYPE_NONE | 0x0000,
   /* IO_GENERAL (reserved). */
   /** Reserved. */
   IFX_TAPI_EVENT_IO_GENERAL_NONE      = IFX_TAPI_EVENT_TYPE_IO_GENERAL | 0x0000,
   /* IO_INTERRUPT (Reserved). */
   /** Reserved. */
   IFX_TAPI_EVENT_IO_INTERRUPT_NONE    = IFX_TAPI_EVENT_TYPE_IO_INTERRUPT | 0x0000,

   /* FXS */
   /** No event (reserved). */
   IFX_TAPI_EVENT_FXS_NONE             = IFX_TAPI_EVENT_TYPE_FXS | 0x0000,
   /** FXS line is ringing (reserved). */
   IFX_TAPI_EVENT_FXS_RING             = IFX_TAPI_EVENT_TYPE_FXS | 0x0001,
   /** End of a ring burst detected; this event may be used as a trigger to
       start an FSK transmission (\ref IFX_TAPI_CID_TX_INFO_START). */
   IFX_TAPI_EVENT_FXS_RINGBURST_END    = IFX_TAPI_EVENT_TYPE_FXS | 0x0002,
   /** Indicates that ringing has ended; this event is only generated when
       the configured maximum ring cadences have been played. This event is
       not generated when ringing is stopped by either going off-hook or
       \ref IFX_TAPI_RING_STOP. This is because, for off-hook, there already is
       the off-hook event. Ring stop is initiated by the application and
       does not need an extra confirmation. */
   IFX_TAPI_EVENT_FXS_RINGING_END      = IFX_TAPI_EVENT_TYPE_FXS | 0x0003,
   /** Hook event: on-hook. */
   IFX_TAPI_EVENT_FXS_ONHOOK           = IFX_TAPI_EVENT_TYPE_FXS | 0x0004,
   /** Hook event: off-hook. */
   IFX_TAPI_EVENT_FXS_OFFHOOK          = IFX_TAPI_EVENT_TYPE_FXS | 0x0005,
   /** Hook event: flash hook. */
   IFX_TAPI_EVENT_FXS_FLASH            = IFX_TAPI_EVENT_TYPE_FXS | 0x0006,
   /** Hook event: on-hook detected by interrupt.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_FXS_ONHOOK_INT       = IFX_TAPI_EVENT_TYPE_FXS | 0x0007,
   /** Hook event: off-hook detected by interrupt.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_FXS_OFFHOOK_INT      = IFX_TAPI_EVENT_TYPE_FXS | 0x0008,
   /** Measurement results for continuous measurement are available
       to be read out. */
   IFX_TAPI_EVENT_CONTMEASUREMENT      = IFX_TAPI_EVENT_TYPE_FXS | 0x0009,
   /** Raw hook event: raw on-hook. */
   IFX_TAPI_EVENT_FXS_RAW_ONHOOK       = IFX_TAPI_EVENT_TYPE_FXS | 0x000a,
   /** Raw hook event: raw off-hook. */
   IFX_TAPI_EVENT_FXS_RAW_OFFHOOK      = IFX_TAPI_EVENT_TYPE_FXS | 0x000b,
   /** The line feeding mode has changed. The new line feeding mode is given as an argument.
       This event is used by the driver internally and may be used by the application for test purposes only.
       Reporting of the event is deactivated per default */
   IFX_TAPI_EVENT_FXS_LINE_MODE        = IFX_TAPI_EVENT_TYPE_FXS | 0x000c,
   /** End of COMTEL signal.
       COMTEL data transmission has finished or was terminated by an off-hook event. */
   IFX_TAPI_EVENT_FXS_COMTEL_END       = IFX_TAPI_EVENT_TYPE_FXS | 0x000D,
   /* FXO */
   /** No event (reserved). */
   IFX_TAPI_EVENT_FXO_NONE             = IFX_TAPI_EVENT_TYPE_FXO | 0x0000,
   /** Battery - line is fed from FXO. */
   IFX_TAPI_EVENT_FXO_BAT_FEEDED       = IFX_TAPI_EVENT_TYPE_FXO | 0x0001,
   /** Battery - FXO line is not fed. */
   IFX_TAPI_EVENT_FXO_BAT_DROPPED      = IFX_TAPI_EVENT_TYPE_FXO | 0x0002,
   /** FXO line polarity changed. */
   IFX_TAPI_EVENT_FXO_POLARITY         = IFX_TAPI_EVENT_TYPE_FXO | 0x0003,
   /** Line is ringing, indicates ring bursts. */
   IFX_TAPI_EVENT_FXO_RING_START       = IFX_TAPI_EVENT_TYPE_FXO | 0x0004,
   /** FXO line stopped ringing.*/
   IFX_TAPI_EVENT_FXO_RING_STOP        = IFX_TAPI_EVENT_TYPE_FXO | 0x0005,
   /** OSI signal (short drop of DC voltage, less than 300 ms),
       indicating the start of a CID transmission. */
   IFX_TAPI_EVENT_FXO_OSI              = IFX_TAPI_EVENT_TYPE_FXO | 0x0006,
   /** APOH (another phone off-hook). */
   IFX_TAPI_EVENT_FXO_APOH             = IFX_TAPI_EVENT_TYPE_FXO | 0x0007,
   /** NOPOH (no other phone off-hook). */
   IFX_TAPI_EVENT_FXO_NOPOH            = IFX_TAPI_EVENT_TYPE_FXO | 0x0008,

   /** GR-909 test results ready. */
   IFX_TAPI_EVENT_LT_GR909_RDY         = IFX_TAPI_EVENT_TYPE_LT  | 0x0001,
   /** Line testing finished (results are ready, where applicable). */
   IFX_TAPI_EVENT_NLT_END              = IFX_TAPI_EVENT_TYPE_LT  | 0x0002,
   IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_RDY     = IFX_TAPI_EVENT_TYPE_LT  | 0x0003,
   /** Internal event: end of capacitance measurement.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_RDY_INT = IFX_TAPI_EVENT_TYPE_LT  | 0x0004,
   /** Internal event: start of capacitance measurement
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_START_INT = IFX_TAPI_EVENT_TYPE_LT | 0x0005,

   /* PULSE */
   /** No event (reserved). */
   IFX_TAPI_EVENT_PULSE_NONE           = IFX_TAPI_EVENT_TYPE_PULSE | 0x0000,
   /** Pulse digit detected. */
   IFX_TAPI_EVENT_PULSE_DIGIT          = IFX_TAPI_EVENT_TYPE_PULSE | 0x0001,
   /** Indicates start of pulse dialing. This event can be used to stop
       the dial tone. */
   IFX_TAPI_EVENT_PULSE_START          = IFX_TAPI_EVENT_TYPE_PULSE | 0x0002,

   /* DTMF */
   /** No event (reserved). */
   IFX_TAPI_EVENT_DTMF_NONE            = IFX_TAPI_EVENT_TYPE_DTMF | 0x0000,
   /** DTMF tone detected. */
   IFX_TAPI_EVENT_DTMF_DIGIT           = IFX_TAPI_EVENT_TYPE_DTMF | 0x0001,
   /** DTMF tone end detected. */
   IFX_TAPI_EVENT_DTMF_END             = IFX_TAPI_EVENT_TYPE_DTMF | 0x0002,

   /* Calibration */
   /** Reserved. */
   IFX_TAPI_EVENT_CALIBRATION_NONE     = IFX_TAPI_EVENT_TYPE_CALIBRATION | 0x0000,
   /** End of calibration.
       The calibration process has finished or was stopped because of an error. */
   IFX_TAPI_EVENT_CALIBRATION_END      = IFX_TAPI_EVENT_TYPE_CALIBRATION | 0x0001,
   /** Internal event: end of calibration.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_CALIBRATION_END_INT  = IFX_TAPI_EVENT_TYPE_CALIBRATION | 0x0002,
   /** Internal event: end of calibration.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_CALIBRATION_END_SINT = IFX_TAPI_EVENT_TYPE_CALIBRATION | 0x0003,

   /* Metering */
   /** Reserved. */
   IFX_TAPI_EVENT_METERING_NONE        = IFX_TAPI_EVENT_METERING |0x0000,
   /** Metering event. */
   IFX_TAPI_EVENT_METERING_END         = IFX_TAPI_EVENT_METERING |0x0001,

   /* CID */
   /* TX */
   /** TX no event (reserved). */
   IFX_TAPI_EVENT_CID_TX_NONE          = IFX_TAPI_EVENT_TYPE_CID | 0x0000,
   /** Reserved; start of CID TX sequence (reserved). */
   IFX_TAPI_EVENT_CID_TX_SEQ_START     = IFX_TAPI_EVENT_TYPE_CID | 0x0001,
   /** CID TX protocol sequence ended. The event indicates that the CID
       sequence that was started with \ref IFX_TAPI_CID_TX_SEQ_START has
       ended in a regular manner. The event is sent immediately after the CID
       data transmission phase (FSK or DTMF). At the time in which the event is
       received by application software, the attached telephone should
       already display the CID data. */
   IFX_TAPI_EVENT_CID_TX_SEQ_END       = IFX_TAPI_EVENT_TYPE_CID | 0x0002,
   /** Start of CID TX information (reserved). */
   IFX_TAPI_EVENT_CID_TX_INFO_START    = IFX_TAPI_EVENT_TYPE_CID | 0x0003,
   /** CID TX FSK message sent. This event indicates that the CID data
       transmission that was started with \ref IFX_TAPI_CID_TX_INFO_START
       has ended. At the time in which the event is received by the application software,
       the attached telephone should already display the CID data
       (in case of CID type 1 or type 2). */
   IFX_TAPI_EVENT_CID_TX_INFO_END      = IFX_TAPI_EVENT_TYPE_CID | 0x0004,
   /** CID TX sequence error: acknowledge not received. No ack was received
       from the CID receiver during the time-out phase. The sequence ended
       with an error. The ACK is required in most CID type 2 protocols
       (all standards but NTT) and in CID type 1 for NTT
       ('Primary Answer Signal Detected'). */
   IFX_TAPI_EVENT_CID_TX_NOACK_ERR     = IFX_TAPI_EVENT_TYPE_CID | 0x0005,
   /** Error in ring cadence setting used for the CID type 1 TX sequence,
       in case of alert type 'first ring' (Telcordia or ETSI).
       The programmed time between the first and second ring bursts is too short
       to transmit the CID data (FSK or DTMF). Check ring cadence
       and CID timing settings and ensure that enough time is reserved for
       the CID data transmission. */
   IFX_TAPI_EVENT_CID_TX_RINGCAD_ERR   = IFX_TAPI_EVENT_TYPE_CID | 0x0006,
   /** Buffer underrun on the CID FSK sender. This indicates that, after the
       request for more data, the host controller was too slow to refill data
       into the transmission buffer. This can happen if interrupts are
       not processed on the host controller for some time. */
   IFX_TAPI_EVENT_CID_TX_UNDERRUN_ERR  = IFX_TAPI_EVENT_TYPE_CID | 0x0007,
   /** CID TX sequence error: second acknowledge not received
       ('Incoming Successful Signal Detected', only for NTT CID type 1).
       No ACK was received during the time out, from an NTT CID receiver
       that acknowledges the FSK transmission. */
   IFX_TAPI_EVENT_CID_TX_NOACK2_ERR    = IFX_TAPI_EVENT_TYPE_CID | 0x0008,
   /** CID transmission stopped: this event indicates that the CID state machine
       reached the end of the sequence. It can be used to clean up some data or
       restore previous states. */
   IFX_TAPI_EVENT_CIDSM_END            = IFX_TAPI_EVENT_TYPE_CID | 0x0009,
   /** Internal event: CID data transmission ended.
       The event is sent immediately after data transmission and translated
       into either a ...SEQ_END or ...INFO_END event, depending on the current
       transmission mode.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_CID_TX_END           = IFX_TAPI_EVENT_TYPE_CID | 0x0010,
   /* RX */
   /** CID RX no event (reserved). */
   IFX_TAPI_EVENT_CID_RX_NONE          = IFX_TAPI_EVENT_TYPE_CID | 0x0020,
   /** CAS signal detected. Deprecated: this is just an alias for
       \ref IFX_TAPI_EVENT_FAXMODEM_CAS_BELL.
       Use \ref IFX_TAPI_EVENT_FAXMODEM_CAS_BELL instead. */
   IFX_TAPI_EVENT_CID_RX_CAS           = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0014,
   /** FSK message received (CID RX).
       The CID FSK receiver has successfully finished receiving an FSK signal.
       The application should stop the FSK receiver and analyze the
       received data. */
   IFX_TAPI_EVENT_CID_RX_END           = IFX_TAPI_EVENT_TYPE_CID | 0x0022,
   /** FSK carrier detected (reserved). */
   IFX_TAPI_EVENT_CID_RX_CD            = IFX_TAPI_EVENT_TYPE_CID | 0x0023,
   /** Error during CID RX reception. This event is raised when, during the
       reception of FSK data, no more space is available in the reception buffers.
       It indicates that data has been lost during the reception. The error is
       sent only once during a reception process. The error is cleared by
       either restarting the CID FSK receiver or reading all buffers that
       have been received. */
   IFX_TAPI_EVENT_CID_RX_ERROR_READ    = IFX_TAPI_EVENT_TYPE_CID | 0x0024,
   /** Error during CID reception (reserved). */
   IFX_TAPI_EVENT_CID_RX_ERROR1        = IFX_TAPI_EVENT_TYPE_CID | 0x0025,
   /** Error during CID reception (reserved). */
   IFX_TAPI_EVENT_CID_RX_ERROR2        = IFX_TAPI_EVENT_TYPE_CID | 0x0026,

   /* TONE_GEN */
   /** No event (reserved). */
   IFX_TAPI_EVENT_TONE_GEN_NONE        = IFX_TAPI_EVENT_TYPE_TONE_GEN | 0x0000,
   /** Tone generator busy (reserved). */
   IFX_TAPI_EVENT_TONE_GEN_BUSY        = IFX_TAPI_EVENT_TYPE_TONE_GEN | 0x0001,
   /** Tone generation ended. */
   IFX_TAPI_EVENT_TONE_GEN_END         = IFX_TAPI_EVENT_TYPE_TONE_GEN | 0x0002,
   /** Tone generation end event used internally to trigger the state machines.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_TONE_GEN_END_RAW     = IFX_TAPI_EVENT_TYPE_TONE_GEN | 0x0003,

   /* TONE_DET */
   /** No event (reserved). */
   IFX_TAPI_EVENT_TONE_DET_NONE        = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0000,
   /** Tone detect receive (reserved). */
   IFX_TAPI_EVENT_TONE_DET_RECEIVE     = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0001,
   /** Tone detect transmit (reserved). */
   IFX_TAPI_EVENT_TONE_DET_TRANSMIT    = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0002,
   /** Call progress tone detected. */
   IFX_TAPI_EVENT_TONE_DET_CPT         = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0003,
   /** Call progress tone detection ended. */
   IFX_TAPI_EVENT_TONE_DET_CPT_END     = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0004,
   /** MF R2 tone detected. */
   IFX_TAPI_EVENT_TONE_DET_MF_R2_START = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0005,
   /** MF R2 tone end detected. */
   IFX_TAPI_EVENT_TONE_DET_MF_R2_END   = IFX_TAPI_EVENT_TYPE_TONE_DET | 0x0006,

   /* FAXMODEM_SIGNAL */
   /** No event (reserved). */
   IFX_TAPI_EVENT_FAXMODEM_NONE        = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0000,
   /** DIS preamble signal. */
   IFX_TAPI_EVENT_FAXMODEM_DIS         = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0001,
   /** 2100 Hz (CED) answering tone (ANS). */
   IFX_TAPI_EVENT_FAXMODEM_CED         = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0002,
   /** Phase reversal. */
   IFX_TAPI_EVENT_FAXMODEM_PR          = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0003,
   /** Amplitude modulation. */
   IFX_TAPI_EVENT_FAXMODEM_AM          = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0006,
   /** 1100 Hz single tone (CNG fax). */
   IFX_TAPI_EVENT_FAXMODEM_CNGFAX      = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0008,
   /** 1300 Hz single tone (CNG modem). It can indicate CT, V.18 XCI mark
    sequence. */
   IFX_TAPI_EVENT_FAXMODEM_CNGMOD      = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0009,
   /** 980 Hz single tone (V.21L mark sequence). */
   IFX_TAPI_EVENT_FAXMODEM_V21L        = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000a,
   /** 1400 Hz single tone (V.18A mark sequence). */
   IFX_TAPI_EVENT_FAXMODEM_V18A        = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000b,
   /** 1800 Hz single tone (V.27, V.32 carrier). */
   IFX_TAPI_EVENT_FAXMODEM_V27         = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000c,
   /** 2225 Hz single tone (Bell answering tone). */
   IFX_TAPI_EVENT_FAXMODEM_BELL        = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000d,
   /** 2250 Hz single tone (V.22 unscrambled binary ones). */
   IFX_TAPI_EVENT_FAXMODEM_V22         = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000e,
   /** 2225 Hz or 2250 Hz single tone, not possible to distinguish. */
   IFX_TAPI_EVENT_FAXMODEM_V22ORBELL   = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x000f,
   /** 600 Hz + 300 Hz dual tone (V.32 AC). */
   IFX_TAPI_EVENT_FAXMODEM_V32AC       = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0010,
   /** 1375 Hz + 2002 Hz dual tone (V.8bis initiating segment 1). */
   IFX_TAPI_EVENT_FAXMODEM_V8BIS       = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0011,
   /** Hold characteristic. */
   IFX_TAPI_EVENT_FAXMODEM_HOLDEND     = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0012,
   /** End of CED signal.*/
   IFX_TAPI_EVENT_FAXMODEM_CEDEND      = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0013,
   /** CAS signal detected;
       2130 + 2750 Hz dual tone (CPE alert signal bell caller ID type 2 alert tone). */
   IFX_TAPI_EVENT_FAXMODEM_CAS_BELL    = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0014,
   /** 1650 Hz single tone (V.21H mark sequence). */
   IFX_TAPI_EVENT_FAXMODEM_V21H        = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0015,
   /** Voice modem discriminator. */
   IFX_TAPI_EVENT_FAXMODEM_VMD         = IFX_TAPI_EVENT_TYPE_FAXMODEM_SIGNAL | 0x0016,

   /* LINear channel */
   /** Linear channel no event. */
   IFX_TAPI_EVENT_LIN_NONE             = IFX_TAPI_EVENT_TYPE_LIN | 0x0000,
   /** Linear channel input buffer underflow event. */
   IFX_TAPI_EVENT_LIN_UNDERFLOW        = IFX_TAPI_EVENT_TYPE_LIN | 0x0001,

   /* CODer */
   /** COD no event (reserved). */
   IFX_TAPI_EVENT_COD_NONE             = IFX_TAPI_EVENT_TYPE_COD | 0x0000,
   /** Decoder change event. */
   IFX_TAPI_EVENT_COD_DEC_CHG          = IFX_TAPI_EVENT_TYPE_COD | 0x0001,
   /** Room noise detection: noise detected;
       currently used by XWAY VINETIC-CPE only */
   IFX_TAPI_EVENT_COD_ROOM_NOISE       = IFX_TAPI_EVENT_TYPE_COD | 0x0002,
   /** Room noise detection: silence detected;
       currently used by XWAY VINETIC-CPE only */
   IFX_TAPI_EVENT_COD_ROOM_SILENCE     = IFX_TAPI_EVENT_TYPE_COD | 0x0003,
   /** Playback of the announcement has finished or has been stopped. */
   IFX_TAPI_EVENT_COD_ANNOUNCE_END     = IFX_TAPI_EVENT_TYPE_COD | 0x0004,
   /** MOS-LEQ threshold under-crossing event.
       This event is generated in case the calculated MOS-LQE value has fallen
       below the configured MOS-LQE threshold value.
       This event is generated only once until \ref IFX_TAPI_COD_MOS_RESULT_GET
       is called to reset the event generation. */
   IFX_TAPI_EVENT_COD_MOS              = IFX_TAPI_EVENT_TYPE_COD | 0x0005,

   /* RTP */
   /** RTP no event (reserved). */
   IFX_TAPI_EVENT_RTP_NONE             = IFX_TAPI_EVENT_TYPE_RTP | 0x0000,
   /** RTP activity event. */
   IFX_TAPI_EVENT_RTP_FIRST            = IFX_TAPI_EVENT_TYPE_RTP | 0x0001,
   /** RTP broken connection event. */
   IFX_TAPI_EVENT_RTP_EXT_BROKEN       = IFX_TAPI_EVENT_TYPE_RTP | 0x0010,
   /** RTP SSRC changed event. */
   IFX_TAPI_EVENT_RTP_EXT_SSRC_CHANGED = IFX_TAPI_EVENT_TYPE_RTP | 0x0012,

   /* AAL */
   /** AAL no event (reserved). */
   IFX_TAPI_EVENT_AAL_NONE             = IFX_TAPI_EVENT_TYPE_AAL | 0x0000,

   /* RFC 2833 */
   /** No event (reserved). */
   IFX_TAPI_EVENT_RFC2833_NONE         = IFX_TAPI_EVENT_TYPE_RFC2833 | 0x0000,
   /** RFC 2833 event. */
   IFX_TAPI_EVENT_RFC2833_EVENT        = IFX_TAPI_EVENT_TYPE_RFC2833 | 0x0001,

#ifndef TAPI4_DXY_DOC
   /** KPI interface */
   /** No event (reserved). */
   IFX_TAPI_EVENT_KPI_NONE             = IFX_TAPI_EVENT_TYPE_KPI | 0x0000,
   /** Ingress FIFO full. */
   IFX_TAPI_EVENT_KPI_INGRESS_FIFO_FULL= IFX_TAPI_EVENT_TYPE_KPI | 0x0001,
#endif /* #ifndef TAPI4_DXY_DOC */

   /* T38 */
   /** No event (reserved). */
   IFX_TAPI_EVENT_T38_NONE             = IFX_TAPI_EVENT_TYPE_T38 | 0x0000,
   /** Generic error (reserved). */
   IFX_TAPI_EVENT_T38_ERROR_GEN        = IFX_TAPI_EVENT_TYPE_T38 | 0x0001,
   /** Overload error. */
   IFX_TAPI_EVENT_T38_ERROR_OVLD       = IFX_TAPI_EVENT_TYPE_T38 | 0x0002,
   /** Read error; currently used by XWAY VINETIC-CPE only. */
   IFX_TAPI_EVENT_T38_ERROR_READ       = IFX_TAPI_EVENT_TYPE_T38 | 0x0003,
   /** Write error. */
   IFX_TAPI_EVENT_T38_ERROR_WRITE      = IFX_TAPI_EVENT_TYPE_T38 | 0x0004,
   /** Error in data transmission. The datapump is switched off. */
   IFX_TAPI_EVENT_T38_ERROR_DATA       = IFX_TAPI_EVENT_TYPE_T38 | 0x0005,
   /** Setup error. */
   IFX_TAPI_EVENT_T38_ERROR_SETUP      = IFX_TAPI_EVENT_TYPE_T38 | 0x0006,
   /** Fax data pump request, used only in polling mode. */
   IFX_TAPI_EVENT_T38_FDP_REQ          = IFX_TAPI_EVENT_TYPE_T38 | 0x0007,
   /** State changes for the T.38 stack implementation. */
   IFX_TAPI_EVENT_T38_STATE_CHANGE     = IFX_TAPI_EVENT_TYPE_T38 | 0x0008,

   /* JB */
   /** Jitter events (reserved). */
   IFX_TAPI_EVENT_JB_NONE              = IFX_TAPI_EVENT_TYPE_JB | 0x0000,

   /* DOWNLOAD */
   /** Download events (reserved). */
   IFX_TAPI_EVENT_DOWNLOAD_NONE        = IFX_TAPI_EVENT_TYPE_DOWNLOAD | 0x0000,

   /* INFORMATION */
   /** Information events (reserved). */
   IFX_TAPI_EVENT_INFO_NONE            = IFX_TAPI_EVENT_TYPE_INFO | 0x0000,
   /** Information mailbox congestion in downstream direction;
       packet was dropped. */
   IFX_TAPI_EVENT_INFO_MBX_CONGESTION  = IFX_TAPI_EVENT_TYPE_INFO | 0x0001,

   /* DEBUG */
   /** For debug purposes (reserved). */
   IFX_TAPI_EVENT_DEBUG_NONE           = IFX_TAPI_EVENT_TYPE_DEBUG | 0x0000,
   /** Debug command error event. */
   IFX_TAPI_EVENT_DEBUG_CERR           = IFX_TAPI_EVENT_TYPE_DEBUG | 0x0001,
   /** GPIO pin status change from high to low, channel is pin number. */
   IFX_TAPI_EVENT_GPIO_HL              = IFX_TAPI_EVENT_TYPE_DEBUG | 0x0002,
   /** GPIO pin status change from low to high, channel is pin number. */
   IFX_TAPI_EVENT_GPIO_LH              = IFX_TAPI_EVENT_TYPE_DEBUG | 0x0003,

   /* Low-level driver events. */
   /** Device-specific events (reserved). */
   IFX_TAPI_EVENT_LL_DRIVER_NONE       = IFX_TAPI_EVENT_TYPE_LL_DRIVER | 0x0000,
   /** Device-specific events, currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_LL_DRIVER_WD_FAIL = IFX_TAPI_EVENT_TYPE_LL_DRIVER | 0x0001,

   /* FAULT_GENERAL */
   /** Generic fault, no event (reserved). */
   IFX_TAPI_EVENT_FAULT_GENERAL_NONE   = IFX_TAPI_EVENT_TYPE_FAULT_GENERAL | 0x0000,
   /** General system fault (reserved). */
   IFX_TAPI_EVENT_FAULT_GENERAL        = IFX_TAPI_EVENT_TYPE_FAULT_GENERAL | 0x1,
   /** General system fault (reserved). */
   IFX_TAPI_EVENT_FAULT_GENERAL_CHINFO = IFX_TAPI_EVENT_TYPE_FAULT_GENERAL | 0x2,
   /** General device fault (reserved). */
   IFX_TAPI_EVENT_FAULT_GENERAL_DEVINFO = IFX_TAPI_EVENT_TYPE_FAULT_GENERAL | 0x3,
   /** General fault, channel event FIFO overflow. */
   IFX_TAPI_EVENT_FAULT_GENERAL_EVT_FIFO_OVERFLOW =
                                         IFX_TAPI_EVENT_TYPE_FAULT_GENERAL | 0x4,

   /* FAULT_LINE */
   /** Reserved; line fault, no event. */
   IFX_TAPI_EVENT_FAULT_LINE_NONE      = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0000,
   /** Ground key, positive polarity, currently used by DUSLIC only. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_POS    = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0001,
   /** Ground key, negative polarity, currently used by DUSLIC only. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_NEG    = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0002,
   /** Ground key low. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_LOW    = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0003,
   /** Ground key high. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_HIGH   = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0004,
   /** Over-temperature. */
   IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP  = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0005,
   /** Over-current; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_LINE_OVERCURRENT  = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0006,
   /** Internal event: ground key low.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_LOW_INT = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0007,
   /** Internal event: ground key high.
       - This (internal) event is not available to the application. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_HIGH_INT = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0008,
   /** Ground key low end. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_LOW_END = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x0009,
   /** Ground key high end. */
   IFX_TAPI_EVENT_FAULT_LINE_GK_HIGH_END = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x000a,
   /** Over-temperature end. */
   IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP_END = IFX_TAPI_EVENT_TYPE_FAULT_LINE | 0x000b,

   /* FAULT_HW */
   /** Reserved. */
   IFX_TAPI_EVENT_FAULT_HW_NONE        = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0000,
   /** SPI access error; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_SPI_ACCESS  = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0001,
   /** Clock failure; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_CLOCK_FAIL  = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0002,
   /** Clock failure end; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_CLOCK_FAIL_END  = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0003,
   /** Hardware failure; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_FAULT       = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0004,
   /** Hardware synchronization event; currently used by XWAY VINETIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_SYNC        = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0005,
   /** A HW reset occurred; currently used by XWAY VINETIC-xT only. */
   IFX_TAPI_EVENT_FAULT_HW_RESET       = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0006,
   /** A communication error on the SSI is detected. */
   IFX_TAPI_EVENT_FAULT_HW_SSI_ERR     = IFX_TAPI_EVENT_TYPE_FAULT_HW | 0x0007,

   /* FAULT_FW */
   /** Reserved. */
   IFX_TAPI_EVENT_FAULT_FW_NONE        = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0000,
   /** Event mailbox out underflow; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_FW_EBO_UF      = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0001,
   /** Event mailbox out overflow; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_FW_EBO_OF      = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0002,
   /** Command mailbox out underflow. */
   IFX_TAPI_EVENT_FAULT_FW_CBO_UF      = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0003,
   /** Command mailbox out overflow; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_FW_CBO_OF      = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0004,
   /** Command mailbox in overflow; currently used by XWAY DUSLIC-xT only. */
   IFX_TAPI_EVENT_FAULT_FW_CBI_OF      = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0005,
   /** FW watchdog time-out. */
   IFX_TAPI_EVENT_FAULT_FW_WATCHDOG    = IFX_TAPI_EVENT_TYPE_FAULT_FW | 0x0006,

   /* FAULT_SW */
   /** Reserved. */
   IFX_TAPI_EVENT_FAULT_SW_NONE        = IFX_TAPI_EVENT_TYPE_FAULT_SW | 0x0000,

   /** HDLC errors */
   /** Reserved. */
   IFX_TAPI_EVENT_FAULT_HDLC_NONE = IFX_TAPI_EVENT_TYPE_FAULT_HDLC | 0x0000,
   /** HDLC packets are too large or have a non-even number of bytes. */
   IFX_TAPI_EVENT_FAULT_HDLC_FRAME_LENGTH = IFX_TAPI_EVENT_TYPE_FAULT_HDLC | 0001,
   /** HDLC packets received from FW, but no KPI channel is configured. */
   IFX_TAPI_EVENT_FAULT_HDLC_NO_KPI_PATH = IFX_TAPI_EVENT_TYPE_FAULT_HDLC | 0002,
   /** HDLC Tx overflow reported by the FW - for debugging purposes only. */
   IFX_TAPI_EVENT_FAULT_HDLC_TX_OVERFLOW = IFX_TAPI_EVENT_TYPE_FAULT_HDLC | 0003,
   /** HDLC frame handlers are not enabled on the channel. */
   IFX_TAPI_EVENT_FAULT_HDLC_DISABLED = IFX_TAPI_EVENT_TYPE_FAULT_HDLC | 0004

}IFX_TAPI_EVENT_ID_t;

/** Extended T.38 event types for IFX_TAPI_EVENT_T38_ERROR_SETUP. */
typedef enum
{
   /** Error in data pump disabling. */
   IFX_TAPI_EVENT_T38_ERROR_SETUP_DPOFF   = 0x0001,
   /** Error in modulator enabling. */
   IFX_TAPI_EVENT_T38_ERROR_SETUP_MODON   = 0x0002,
   /** Error in demodulator enabling. */
   IFX_TAPI_EVENT_T38_ERROR_SETUP_DEMODON = 0x0003
} IFX_TAPI_EVENT_T38_ERROR_SETUP_t;

/** Extended T.38 event types for IFX_TAPI_EVENT_T38_ERROR_DATA. */
typedef enum
{
   /** Error modulator signal buffer underrun. */
   IFX_TAPI_EVENT_T38_ERROR_DATA_MBSU   = 0x0001,
   /** Error demodulator signal buffer overflow. */
   IFX_TAPI_EVENT_T38_ERROR_DATA_DBSO   = 0x0002,
   /** Error modulator data buffer overflow. */
   IFX_TAPI_EVENT_T38_ERROR_DATA_MBDO   = 0x0003,
   /** Error modulator data buffer underrun. */
   IFX_TAPI_EVENT_T38_ERROR_DATA_MBDU   = 0x0004,
   /** Error demodulator data buffer overflow. */
   IFX_TAPI_EVENT_T38_ERROR_DATA_DBDO   = 0x0005,
   /** Wrong packet passed form IP. */
   IFX_TAPI_EVENT_T38_ERROR_IP_PACKET   = 0x0006,
   /** Command buffer overflow. */
   IFX_TAPI_EVENT_T38_ERROR_CB_OVERFLOW = 0x0007,
   /** Data buffer overflow. */
   IFX_TAPI_EVENT_T38_ERROR_DB_OVERFLOW = 0x0008,
   /** Wrong command in command buffer detected. */
   IFX_TAPI_EVENT_T38_ERROR_BAD_CMD     = 0x0009,
   /** T.38 activation failed. */
   IFX_TAPI_EVENT_T38_ERROR_SESS_START  = 0x000A,
   /** T.38 deactivation failed. */
   IFX_TAPI_EVENT_T38_ERROR_SESS_STOP   = 0x000B,
   /** T.38 trace flush finished. */
   IFX_TAPI_EVENT_T38_ERROR_FLUSH_FIN   = 0x000C
} IFX_TAPI_EVENT_T38_ERROR_DATA_t;

/** This structure extends the SPI hardware access events
    of \ref IFX_TAPI_EVENT_FAULT_HW_SPI_ACCESS. */
typedef enum
{
   /** SPI communication synchronization failure detected between the TAPI and
       the device. */
   IFX_TAPI_EVENT_FAULT_HW_SYNC_SPI_FAILURE,
   /** The TAPI starts a recovery process to the device over SPI. */
   IFX_TAPI_EVENT_FAULT_HW_SYNC_SPI_RECOVERY,
   /** The TAPI-to-device communication over SPI is synchronized (again). */
   IFX_TAPI_EVENT_FAULT_HW_SYNC_SPI_NORMAL,
   /** Based on an unexpected error, the TAPI could not re-establish communication to the device. */
   IFX_TAPI_EVENT_FAULT_HW_SYNC_SPI_ABORT
} IFX_TAPI_EVENT_FAULT_HW_SYNC_t;

/* =============================== */
/* type definition                 */
/* =============================== */
/** This structure contains data specific to the pulse dialing event. */
typedef struct
{
   /** Reserved.*/
   IFX_uint16_t reserved:8;
   /** Pulse digit number information (0 - 9).

   - 1: Key_1, key '1' detected
   - 2: Key_2, key '2' detected
   - 3: Key_3, key '3' detected
   - 4: Key_4, key '4' detected
   - 5: Key_5, key '5' detected
   - 6: Key_6, key '6' detected
   - 7: Key_7, key '7' detected
   - 8: Key_8, key '8' detected
   - 9: Key_9, key '9' detected
   - 11: Key_0, key '0' detected
   */
   IFX_uint16_t digit:8;
} IFX_TAPI_EVENT_DATA_PULSE_t;

/** This structure contains data specific to the DTMF event. */
typedef struct
{
#ifdef TAPI_VERSION4
   /** Detected from external (e.g. analog line, PCM bus, RTP in-band, etc.).*/
   IFX_uint32_t external:1;
   /** Detected from internal. */
   IFX_uint32_t internal:1;
#endif /* TAPI_VERSION4 */
#ifdef TAPI_VERSION3
   /** Detected on the local side.*/
   IFX_uint32_t local:1;
   /** Detected on the network side. */
   IFX_uint32_t network:1;
#endif /* TAPI_VERSION3 */
   /** Reserved. */
   IFX_uint32_t reserved:6;
   /** DTMF digit number information.

   - 0: No_Key, no key detected
   - 11: Key_0, DTMF key '0' detected
   - 1: Key_1, DTMF key '1' detected
   - 2: Key_2, DTMF key '2' detected
   - 3: Key_3, DTMF key '3' detected
   - 4: Key_4, DTMF key '4' detected
   - 5: Key_5, DTMF key '5' detected
   - 6: Key_6, DTMF key '6' detected
   - 7: Key_7, DTMF key '7' detected
   - 8: Key_8, DTMF key '8' detected
   - 9: Key_9, DTMF key '9' detected
   - 10: Key_*, DTMF key '*' detected
   - 12: Key_#, DTMF key '#' detected
   - 28: Key_A, DTMF key 'A' detected
   - 29: Key_B, DTMF key 'B' detected
   - 30: Key_C, DTMF key 'C' detected
   - 31: Key_D, DTMF key 'D' detected
   */
   IFX_uint32_t digit:8;
   /** DTMF digit in ASCII representation.

   - 0: No_Key, no key detected
   - 48: Key_0, DTMF key '0' detected
   - 49: Key_1, DTMF key '1' detected
   - 50: Key_2, DTMF key '2' detected
   - 51: Key_3, DTMF key '3' detected
   - 52: Key_4, DTMF key '4' detected
   - 53: Key_5, DTMF key '5' detected
   - 54: Key_6, DTMF key '6' detected
   - 55: Key_7, DTMF key '7' detected
   - 56: Key_8, DTMF key '8' detected
   - 57: Key_9, DTMF key '9' detected
   - 42: Key_*, DTMF key '*' detected
   - 35: Key_#, DTMF key '#' detected
   - 65: Key_A, DTMF key 'A' detected
   - 66: Key_B, DTMF key 'B' detected
   - 67: Key_C, DTMF key 'C' detected
   - 68: Key_D, DTMF key 'D' detected
   */
   IFX_uint32_t ascii:8;
} IFX_TAPI_EVENT_DATA_DTMF_t;

/** This structure contains data specific to the tone generation event.
    This event is generated when a tone generation is stopped or has stopped
    automatically. The 'index' field contains the tone table index of the
    tone that was played out. */
typedef struct
{
#ifdef TAPI_VERSION4
   /** Generation on the external (e.g. analog line, PCM bus, RTP in-band, etc.).*/
   IFX_uint32_t external:1;
   /** Generation on the internal. */
   IFX_uint32_t internal:1;
#endif /* TAPI_VERSION4 */
#ifdef TAPI_VERSION3
   /** Generation on the local side.*/
   IFX_uint32_t local:1;
   /** Generation on the network side. */
   IFX_uint32_t network:1;
#endif /* TAPI_VERSION3 */
   /** Reserved. */
   IFX_uint32_t reserved:6;
   /** Tone table index of the tone that has stopped being played out. */
   IFX_uint32_t index:8;
} IFX_TAPI_EVENT_DATA_TONE_GEN_t;


/** This structure contains data specific to the tone detection event.
    It is used by \ref IFX_TAPI_EVENT_DATA_t. */
typedef struct
{
   /** Detected from external (e.g. analog line, PCM bus, RTP in-band, etc.).*/
   IFX_uint32_t external:1;
   /** Detected from local side. */
   IFX_uint32_t internal:1;
   /** Reserved. */
   IFX_uint32_t reserved:14;
} IFX_TAPI_EVENT_DATA_TONE_DET_t;

/** This structure contains data specific to the fax event. It is used by
    \ref IFX_TAPI_EVENT_DATA_t. */
typedef struct
{
#ifdef TAPI_VERSION4
   /** Detected from external (e.g. analog line, PCM bus, RTP in-band, etc.).*/
   IFX_uint32_t external:1;
   /** Detected from internal. */
   IFX_uint32_t internal:1;
#endif /* TAPI_VERSION4 */
#ifdef TAPI_VERSION3
   /** Detected on the local side.*/
   IFX_uint32_t local:1;
   /** Detected on the network side. */
   IFX_uint32_t network:1;
#endif /* TAPI_VERSION3 */
   /** Last event or not; only to be used in ioctl.*/
   IFX_uint32_t reserved:6;
   /** Fax or modem signal. */
   IFX_uint32_t signal:8;
} IFX_TAPI_EVENT_DATA_FAX_SIG_t;

/** This structure contains data specific to the received
    RFC 2833, RFC 4733 and RFC 5244 packet. */
typedef struct
{
   /** Event code contained in the RFC 2833,
       or ABCD event code according to (RFC 4733 and RFC 5244). */
   IFX_TAPI_PKT_EV_NUM_t event;
} IFX_TAPI_EVENT_DATA_RFC2833_t;

/** This structure contains data specific to the decoder change event. */
typedef struct
{
   /** Type of the coder used; refer to coding of
   \ref IFX_TAPI_COD_TYPE_t.*/
   IFX_uint32_t dec_type        : 8;
   /** Frame length; refer to coding of \ref IFX_TAPI_COD_LENGTH_t. */
   IFX_uint32_t dec_framelength : 8;
} IFX_TAPI_EVENT_DATA_DEC_CHG_t;

/** This structure contains data specific to the CID RX end event. */
typedef struct
{
#ifdef TAPI_VERSION4
   /** Detected from external (e.g. analog line, PCM bus, RTP in-band, etc.).*/
   IFX_uint32_t external:1;
   /** Detected from internal. */
   IFX_uint32_t internal:1;
#endif /* TAPI_VERSION4 */
#ifdef TAPI_VERSION3
   /** Detected on the local side.*/
   IFX_uint32_t local:1;
   /** Detected on the network side. */
   IFX_uint32_t network:1;
#endif /* TAPI_VERSION3 */
   /** Reserved. */
   IFX_uint32_t reserved:6;
   /** Number of bytes received. */
   IFX_uint32_t number:16;
} IFX_TAPI_EVENT_DATA_CID_RX_END_t;

/** This structure contains data specific to the command error event. */
typedef struct
{
   /** Firmware family identifier used to decode the reason field. */
   IFX_uint16_t fw_id;
   /** Reason given by the firmware for the command error. */
   IFX_uint16_t reason;
   /** Header of error command. */
   IFX_uint32_t command;
} IFX_TAPI_EVENT_DATA_CERR_t;

/** This event is generated when the T.38 session state has changed.
    It is used together with the event ID
    \ref IFX_TAPI_EVENT_T38_STATE_CHANGE.
    This enumeration is only used during an event report. The TAPI ignores this
    field during \ref IFX_TAPI_EVENT_ENABLE and
    \ref IFX_TAPI_EVENT_DISABLE operation. Only all T.38 state change
    event reports can be enabled or disabled. Note that the end of a fax session
event can not be disabled. */
typedef enum
{
   /** T.38 debug state change event (for internal use only). */
   IFX_TAPI_EVENT_T38_INTERNAL0  = 0,
   /** T.38 debug state change event (for internal use only). */
   IFX_TAPI_EVENT_T38_INTERNAL1  = 1,
   /** T.38 debug state change event (for internal use only). */
   IFX_TAPI_EVENT_T38_INTERNAL2  = 2,
   /** Facsimiles are in the negotiation phase. */
   IFX_TAPI_EVENT_T38_NEG        = 3,
   /** T.38 starts modulation training. */
   IFX_TAPI_EVENT_MOD            = 4,
   /** T.38 starts demodulation training. */
   IFX_TAPI_EVENT_DEM            = 5,
   /** Facsimile page transmission has started. */
   IFX_TAPI_EVENT_TRANS          = 6,
   /** Post-page command is transmitted. */
   IFX_TAPI_EVENT_PP             = 7,
   /** Facsimile procedure interrupt commands are transmitted. */
   IFX_TAPI_EVENT_INT            = 8,
   /** Facsimile session finishes with DCN. */
   IFX_TAPI_EVENT_DCN            = 9
} IFX_TAPI_EVENT_T38_STATE_t;

/** Event generated when the automatic calibration stops for an analog line.
    This event provides an indication of whether the calibration process was completed
    successfully or with an error. The enumeration value provides failure
    details (defined in the future; for example, IFX_TAPI_EVENT_DATA_CALIBRATION_ERROR_xx).
*/
typedef enum
{
   /** Calibration completed successfully. The calibration results are applied
       to the analog line. */
   IFX_TAPI_EVENT_CALIBRATION_SUCCESS        = 0,
   /** The calibration process returns without an error. The results are out of
       range. The TAPI overwrites the results with default values on the analog line.*/
   IFX_TAPI_EVENT_CALIBRATION_ERROR_RANGE    = 1,
   /** The calibration process failed because of a time-out. */
   IFX_TAPI_EVENT_CALIBRATION_ERROR_TIMEOUT  = 2
} IFX_TAPI_EVENT_CALIBRATION_t;

/** This structure describes the number of lost events. */
typedef struct
{
   /** Number of events lost. */
   IFX_uint32_t lost;
} IFX_TAPI_EVENT_DATA_EVT_FIFO_t;

/** Event generated when the metering transmission was stopped by calling
   \ref IFX_TAPI_METER_STOP and the last metering pulse is transmitted on the
   analog line. It is also generated when \ref IFX_TAPI_METER_BURST is
   called and non-periodic metering is started by \ref IFX_TAPI_METER_BURST.
*/
typedef enum {
   /** Metering finished successfully. */
   IFX_TAPI_EVENT_DATA_METERING_SUCCESS = 0
} IFX_TAPI_EVENT_METERING_t;

/** Event generated when new continuous measurement results are available
    to be read out by IFX_TAPI_CONTMEASUREMENT_GET. */
typedef enum
{
   IFX_TAPI_EVENT_DATA_CONTMEASUREMENT_SUCCESS = 0
} IFX_TAPI_EVENT_CONTMEASUREMENT_t;

/** This structure contains data specific to the MF R2 event. */
typedef struct
{
   /** Reserved. */
   IFX_uint32_t reserved:28;
   /** MF R2 code information. */
   IFX_uint32_t code:4;
} IFX_TAPI_EVENT_DATA_MF_R2_t;

/** This structure contains the data specific to the announcement complete event. */
typedef struct
{
   /** Announcement ID. */
   IFX_uint32_t   nAnnIdx;
} IFX_TAPI_EVENT_ANNOUNCE_t;

/** This enum lists the possible values reported in the TAPI event
    \ref IFX_TAPI_EVENT_FXO_POLARITY. */
typedef enum
{
   /** Reversed polarity (ring more positive than tip). */
   IFX_TAPI_EVENT_DATA_FXO_POLARITY_REVERSED = 0,
   /** Normal polarity (tip more positive than ring). */
   IFX_TAPI_EVENT_DATA_FXO_POLARITY_NORMAL = 1
} IFX_TAPI_EVENT_DATA_FXO_POLARITY_t;

/** Structure to read out the analog line capacitance measurement results. */
typedef struct
{
   /** Measured capacitance [nF]. */
   IFX_uint32_t nCapacitance;
   /** Return code of the measurement.
       The measurement result is invalid in case the return code
       is not set to IFX_SUCCESS. */
   IFX_int32_t nReturnCode;
} IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_t;

/** Union for events that can be reported. */
typedef union
{
   /** Pulse digit information.*/
   IFX_TAPI_EVENT_DATA_PULSE_t         pulse;
   /** DTMF digit information. */
   IFX_TAPI_EVENT_DATA_DTMF_t          dtmf;
   /** Tone generation index. */
   IFX_TAPI_EVENT_DATA_TONE_GEN_t      tone_gen;
   /** Tone detection information. */
   IFX_TAPI_EVENT_DATA_TONE_DET_t      tone_det;
   /** Fax/modem signal information. */
   IFX_TAPI_EVENT_DATA_FAX_SIG_t       fax_sig;
   /** T.38 fax stack state changes. */
   IFX_TAPI_EVENT_T38_STATE_t t38;
   /** RFC 2833 and ABCD (RFC 4733 and RFC 5244) event information. */
   IFX_TAPI_EVENT_DATA_RFC2833_t       rfc2833;
   /** Decoder change event details. */
   IFX_TAPI_EVENT_DATA_DEC_CHG_t       dec_chg;
   /** CID RX end information. */
   IFX_TAPI_EVENT_DATA_CID_RX_END_t    cid_rx_end;
   /** Command error event details. */
   IFX_TAPI_EVENT_DATA_CERR_t          cerr;
   /** Synchronization details. */
   IFX_TAPI_EVENT_FAULT_HW_SYNC_t hw_sync;
   /** Event FIFO details.*/
   IFX_TAPI_EVENT_DATA_EVT_FIFO_t      event_fifo;
   /** Announcement information. */
   IFX_TAPI_EVENT_ANNOUNCE_t           announcement;
   /** MOS-LEQ threshold event details. */
   IFX_TAPI_COD_MOS_VALUE_t            mos;
   /** Analog line calibration process. */
   IFX_TAPI_EVENT_CALIBRATION_t calibration;
   /** Metering event. */
   IFX_TAPI_EVENT_METERING_t metering;
   /** Event generated when new continuous measurement results are available
    to be read out by IFX_TAPI_CONTMEASUREMENT_GET. */
   IFX_TAPI_EVENT_CONTMEASUREMENT_t contmeasurement;
    /** Error line. */
   IFX_TAPI_ErrorLine_t*               error;
   /** Line mode change event information. */
   IFX_TAPI_LINE_MODE_t linemode;
   /** MF R2 tone detection information. */
   IFX_TAPI_EVENT_DATA_MF_R2_t         mf_r2;
   /** FXO line polarity event details. */
   IFX_TAPI_EVENT_DATA_FXO_POLARITY_t  fxo_polarity;
   /** Analog line capacitance measurement results. */
   IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_t lcap;
   /** Reserved. */
   IFX_uint16_t                        value;
} IFX_TAPI_EVENT_DATA_t;

/** This structure is reported by an 'EVENT_GET' ioctl. For event masking
 'EVENT_MASK' re-using IFX_TAPI_EVENT_t should be used. */
typedef struct
{
   /** Device index of event. */
   IFX_uint16_t dev;
   /** Channel where the event occurred or \ref IFX_TAPI_EVENT_ALL_CHANNELS to
       indicate 'any channel' and for events that cannot be assigned to a
       specific channel (such as hardware errors). */
   IFX_uint16_t ch;
#ifdef TAPI_VERSION4
   /** Module type. */
   IFX_TAPI_MODULE_TYPE_t module;
#endif /* TAPI_VERSION4 */
   /** Indicates that more TAPI events are waiting to be read out
       with the \ref IFX_TAPI_EVENT_GET ioctl. It has to be set to zero
       for \ref IFX_TAPI_EVENT_ENABLE or \ref IFX_TAPI_EVENT_DISABLE.  */
   IFX_uint16_t            more;
   /** Reserved. */
   IFX_uint16_t reserved;
   /** Event type and sub-type. */
   IFX_TAPI_EVENT_ID_t id;
   /** Specific data of individual event. */
   IFX_TAPI_EVENT_DATA_t   data;
#ifdef EVENT_COUNTER
   /* Debugging feature; per device/channel event sequence number. */
   IFX_uint32_t            nEventSeqNo;
#endif /* EVENT_COUNTER */
} IFX_TAPI_EVENT_t;

/** List entry of events to mask (enable/disable). This structure is used in
    \ref IFX_TAPI_EVENT_MULTI_t to enable or disable a single or multiple
    events due to one single TAPI call. The event types are listed in
    \ref IFX_TAPI_EVENT_ID_t. */
typedef struct
{
   /** Event type and sub-type. */
   IFX_TAPI_EVENT_ID_t id;
   /** Specific data of individual event (for the event type/subtype). */
   IFX_TAPI_EVENT_DATA_t data;
}IFX_TAPI_EVENT_ENTRY_t;

/** This structure is used to mask one or multiple events of the TAPI using
    \ref IFX_TAPI_EVENT_ENABLE or \ref IFX_TAPI_EVENT_DISABLE. */
typedef struct
{
   /** Device index of event. If set to IFX_TAPI_EVENT_ALL_DEVICES this
       information is global. All masked events are related to this device. */
   IFX_uint16_t dev;
   /** Channel information of event.
       If set to IFX_TAPI_EVENT_ALL_CHANNELS this information is global.
       All masked events are related to this module channel. */
   IFX_uint16_t ch;
#ifdef TAPI_VERSION4
   /** Module type. All masked events are related to this module type.*/
   IFX_TAPI_MODULE_TYPE_t module;
#endif /* TAPI_VERSION4 */
   /** Number of events in the pEvent. */
   IFX_uint16_t nCount;
   /** List of events to enable or disable;
       the amount of memory should be equal
       to (sizeof (IFX_TAPI_EVENT_ENTRY_t) * nCount). */
   IFX_TAPI_EVENT_ENTRY_t *pEvent;
} IFX_TAPI_EVENT_MULTI_t;

/*@}*/ /* TAPI_INTERFACE_EVENT */

/* ======================================================================= */
/* TAPI Analog Line Continuous Measurement Services, structures            */
/* (Group TAPI_INTERFACE_CONTMEASUREMENT)                                  */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CONTMEASUREMENT */
/*@{*/

#ifdef TAPI_ONE_DEVNODE
/** Structure used to request or reset the results of the continuous analog
    line measurement process;
    used by \ref IFX_TAPI_CONTMEASUREMENT_REQ and \ref IFX_TAPI_CONTMEASUREMENT_RESET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
} IFX_TAPI_CONTMEASUREMENT_t;
#endif /* TAPI_ONE_DEVNODE */

/** Structure used to read out the results of the continuous analog line
    measurement process.
    For some parameters, the physical value has to be multiplied by the steps to yield the parameter value.
    For example: Voltage = nVLineWireRing/Steps = 1000(IFX_int16_t)/10(mV) = 100(V);
    used by \ref IFX_TAPI_CONTMEASUREMENT_GET. */
typedef struct
{
   /** Device index. */
   IFX_uint16_t dev;
   /** Channel 'module' index. */
   IFX_uint16_t ch;
   /** Line voltage on RING wire.
       Value of the actual line voltage;
       range (10 mV steps):
       - Minimum: -144 V
       - Maximum: +144 V */
   IFX_int16_t nVLineWireRing;
   /** Line voltage on TIP wire.
       Value of the actual line voltage;
       range (10 mV steps):
       - Minimum: -144 V
       - Maximum: +144 V */
   IFX_int16_t nVLineWireTip;
   /** Desired line voltage.
       Output voltage of DC regulation;
       range (10 mV steps):
       - Minimum: -144 V
       - Maximum: +144 V */
   IFX_int16_t nVLineDesired;
   /** Line current.
       Value of the actual line current;
       range (10 uA steps):
       - Minimum: -100 mA
       - Maximum: +100 mA */
   IFX_int16_t nILine;
   /** Line longitudinal current.
       Value of the actual longitudinal current;
       range (10 uA steps):
       - Minimum: -100 mA
       - Maximum: +100 mA */
   IFX_int16_t nILineLong;
   /** Line ring current.
       Value of the last ring burst current;
       absolute peak value within the last ring period.
       Range (10 uA steps):
       - Minimum: -100 mA
       - Maximum: +100 mA */
   IFX_int16_t nILineRingPeak;
   /** Line ring voltage.
       Value of the last ring burst voltage; peak value of VDAC sine.
       Range (10 mV steps):
       - Minimum: -144 V
       - Maximum: +144 V */
   IFX_int16_t nVLineRingPeak;
   /** TTX metering adaptation coefficient real part.
       Value of the last TTX adaptation;
       value does not have a physical unit.
       Additional user space libraries are needed for further calculations.
       Range (steps of 1):
       - Minimum: -32768
       - Maximum: +32767 */
   IFX_int16_t nTtxMeterReal;
   /** TTX metering adaptation coefficient imaginary part.
       Value of the last TTX adaptation;
       value does not have a physical unit.
       Additional user-space libraries are needed for further calculations.
       Range (steps of 1):
       - Minimum: -32768
       - Maximum: +32767 */
   IFX_uint16_t nTtxMeterImag;
   /** TTX burst length.
       Length of the last TTX burst;
       Range (1 ms steps):
       - Minimum: 0 s
       - Maximum: 10 s */
   IFX_uint16_t nTtxMeterLen;
   /** TTX Current.
       Value of the last TTX current;
       value does not have a physical unit.
       Additional user space libraries are needed for further calculations.
      Range (steps of 1):
       - Minimum: -32768
       - Maximum: +32767 */
   IFX_int16_t nITtxMeter;
   /** TTX Voltage.
       Value of the last TTX voltage;
       value does not have a physical unit.
       Additional user space libraries are needed for further calculations.
       Range (steps of 1):
       - Minimum: -32768
       - Maximum: +32767 */
   IFX_int16_t nVTtxMeter;
   /** Standard battery voltage (Vbath). The allowed parameter range depends
       on the used SLIC hardware; proposed TAPI range (10 mV steps):
       - Minimum: -144 V
       - Maximum: 0 V */
   IFX_int16_t nVBat;
   /** SLIC temperature (in Celsius). */
   IFX_int16_t nSlicTemp;
} IFX_TAPI_CONTMEASUREMENT_GET_t;

#ifdef TAPI_ONE_DEVNODE
   /** Structure used to access the analog line battery voltage.
      It is used by \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_GET and \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_SET. */
   typedef struct
   {
      /** Device index. */
      IFX_uint16_t dev;
      /** Channel 'module' index. */
      IFX_uint16_t ch;
      /** Standard battery voltage (Vbath). The allowed parameter range depends
          on the used SLIC hardware; proposed TAPI range (10 mV steps):
          - Minimum: -144 V
          - Maximum: 0 V */
      IFX_int32_t nVBat;
   } IFX_TAPI_LINE_BATTERY_VOLTAGE_t;
#else /* TAPI_ONE_DEVNODE */
   /** Type used to access the analog line battery voltage.
      It is used by \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_GET and \ref IFX_TAPI_LINE_BATTERY_VOLTAGE_SET. */
   typedef IFX_int32_t IFX_TAPI_LINE_BATTERY_VOLTAGE_t;
#endif /* TAPI_ONE_DEVNODE */

/*@}*/ /* TAPI_INTERFACE_CONTMEASUREMENT */

/* ========================================================================= */
/*                      TAPI Old Interface Adaptations                       */
/* ========================================================================= */

#ifndef IFX_TAPI_ENC_CFG_SET_t
#define IFX_TAPI_ENC_CFG_SET_t  IFX_TAPI_ENC_CFG_t
#endif /* IFX_TAPI_ENC_CFG_SET_t */

#ifndef IFX_TAPI_DECT_ENC_CFG_SET_t
#define IFX_TAPI_DECT_ENC_CFG_SET_t  IFX_TAPI_DECT_ENC_CFG_t
#endif /* IFX_TAPI_DECT_ENC_CFG_SET_t */

#endif  /* DRV_TAPI_IO_H */
