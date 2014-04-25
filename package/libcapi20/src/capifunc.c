/*
 * $Id: capifunc.c,v 1.9 2005/03/08 07:26:47 keil Exp $
 *
 * $Log: capifunc.c,v $
 * Revision 1.9  2005/03/08 07:26:47  keil
 * - add SENDING_COMPLETE to INFO_REQ CONNECT_REQ and CONNECT_IND
 * - remove SENDING_COMPLETE parameter (always NULL) from capi_fill_DISCONNECT_REQ
 *
 * Revision 1.8  2005/02/22 11:39:43  keil
 * for backward compatibility the libcapi20 can now compiled to support the
 * old (buggy) version2 ABI. This is not for future developments. This is only
 * to support old binaries, which are linked against the old V2 lib.
 *
 * Revision 1.7  2005/02/21 17:37:06  keil
 * libcapi20 version 3.0.0
 *  - add SENDING COMPLETE in ALERT_REQ
 *  - add Globalconfiguration to CONNECT_REQ/RESP and SELECT_B_PROTOCOL_REQ
 *
 * * NOTE: incompatible to 2.X.Y versions
 *
 * Revision 1.6  2004/10/06 15:24:43  calle
 * - "SendingComplete"-Patch reverted => 2.0.8 was not binaer compartible
 * - Bugfix: capi20_register() with MaxB3Connection == 0 results in a
 *   core dump. Now at least one buffer is allocated.
 *
 * Revision 1.5  2004/06/14 11:23:48  calle
 * Erweiterungen fuer ALERT_REQ.
 *
 * Revision 1.4  1999/09/10 17:20:33  calle
 * Last changes for proposed standards (CAPI 2.0):
 * - AK1-148 "Linux Extention"
 * - AK1-155 "Support of 64-bit Applications"
 *
 * Revision 1.3  1998/08/30 09:57:17  calle
 * I hope it is know readable for everybody.
 *
 * Revision 1.1  1998/08/25 16:33:19  calle
 * Added CAPI2.0 library. First Version.
 *
 */

#include "capi20.h"

unsigned ALERT_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                    _cdword adr,
                    _cstruct BChannelinformation,
                    _cstruct Keypadfacility,
                    _cstruct Useruserdata,
                    _cstruct Facilitydataarray
#ifndef CAPI_LIBRARY_V2
                    ,_cstruct SendingComplete
#endif
                    ) {
    capi_cmsg_header (cmsg,ApplId,0x01,0x80,Messagenumber,adr);
    cmsg->BChannelinformation = BChannelinformation;
    cmsg->Keypadfacility = Keypadfacility;
    cmsg->Useruserdata = Useruserdata;
    cmsg->Facilitydataarray = Facilitydataarray;
#ifndef CAPI_LIBRARY_V2
    cmsg->SendingComplete = SendingComplete;
#endif
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                      _cdword adr,
                      _cword CIPValue,
                      _cstruct CalledPartyNumber,
                      _cstruct CallingPartyNumber,
                      _cstruct CalledPartySubaddress,
                      _cstruct CallingPartySubaddress,
                      _cword B1protocol,
                      _cword B2protocol,
                      _cword B3protocol,
                      _cstruct B1configuration,
                      _cstruct B2configuration,
                      _cstruct B3configuration,
#ifndef CAPI_LIBRARY_V2
                      _cstruct Globalconfiguration,
#endif
                      _cstruct BC,
                      _cstruct LLC,
                      _cstruct HLC,
                      _cstruct BChannelinformation,
                      _cstruct Keypadfacility,
                      _cstruct Useruserdata,
                      _cstruct Facilitydataarray
#ifndef CAPI_LIBRARY_V2
                     ,_cstruct SendingComplete
#endif
                      ) {
    capi_cmsg_header (cmsg,ApplId,0x02,0x80,Messagenumber,adr);
    cmsg->CIPValue = CIPValue;
    cmsg->CalledPartyNumber = CalledPartyNumber;
    cmsg->CallingPartyNumber = CallingPartyNumber;
    cmsg->CalledPartySubaddress = CalledPartySubaddress;
    cmsg->CallingPartySubaddress = CallingPartySubaddress;
    cmsg->B1protocol = B1protocol;
    cmsg->B2protocol = B2protocol;
    cmsg->B3protocol = B3protocol;
    cmsg->B1configuration = B1configuration;
    cmsg->B2configuration = B2configuration;
    cmsg->B3configuration = B3configuration;
#ifndef CAPI_LIBRARY_V2
    cmsg->Globalconfiguration = Globalconfiguration;
#endif
    cmsg->BC = BC;
    cmsg->LLC = LLC;
    cmsg->HLC = HLC;
    cmsg->BChannelinformation = BChannelinformation;
    cmsg->Keypadfacility = Keypadfacility;
    cmsg->Useruserdata = Useruserdata;
    cmsg->Facilitydataarray = Facilitydataarray;
#ifndef CAPI_LIBRARY_V2
    cmsg->SendingComplete = SendingComplete;
#endif
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_B3_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                         _cdword adr,
                         _cstruct NCPI) {
    capi_cmsg_header (cmsg,ApplId,0x82,0x80,Messagenumber,adr);
    cmsg->NCPI = NCPI;
    return capi_put_cmsg (cmsg);
}

unsigned DATA_B3_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                      _cdword adr,
                      void *Data,
                      _cword DataLength,
                      _cword DataHandle,
                      _cword Flags) {
    capi_cmsg_header (cmsg,ApplId,0x86,0x80,Messagenumber,adr);
    cmsg->Data = Data;
    cmsg->DataLength = DataLength;
    cmsg->DataHandle = DataHandle;
    cmsg->Flags = Flags;
    return capi_put_cmsg (cmsg);
}

unsigned DISCONNECT_B3_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                            _cdword adr,
                            _cstruct NCPI) {
    capi_cmsg_header (cmsg,ApplId,0x84,0x80,Messagenumber,adr);
    cmsg->NCPI = NCPI;
    return capi_put_cmsg (cmsg);
}

unsigned DISCONNECT_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                         _cdword adr,
                         _cstruct BChannelinformation,
                         _cstruct Keypadfacility,
                         _cstruct Useruserdata,
                         _cstruct Facilitydataarray) {
    capi_cmsg_header (cmsg,ApplId,0x04,0x80,Messagenumber,adr);
    cmsg->BChannelinformation = BChannelinformation;
    cmsg->Keypadfacility = Keypadfacility;
    cmsg->Useruserdata = Useruserdata;
    cmsg->Facilitydataarray = Facilitydataarray;
#ifndef CAPI_LIBRARY_V2
    cmsg->SendingComplete = NULL;
#endif
    return capi_put_cmsg (cmsg);
}

unsigned FACILITY_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                       _cdword adr,
                       _cword FacilitySelector,
                       _cstruct FacilityRequestParameter) {
    capi_cmsg_header (cmsg,ApplId,0x80,0x80,Messagenumber,adr);
    cmsg->FacilitySelector = FacilitySelector;
    cmsg->FacilityRequestParameter = FacilityRequestParameter;
    return capi_put_cmsg (cmsg);
}

unsigned INFO_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                   _cdword adr,
                   _cstruct CalledPartyNumber,
                   _cstruct BChannelinformation,
                   _cstruct Keypadfacility,
                   _cstruct Useruserdata,
                   _cstruct Facilitydataarray
#ifndef CAPI_LIBRARY_V2
                  ,_cstruct SendingComplete
#endif
                   ) {
    capi_cmsg_header (cmsg,ApplId,0x08,0x80,Messagenumber,adr);
    cmsg->CalledPartyNumber = CalledPartyNumber;
    cmsg->BChannelinformation = BChannelinformation;
    cmsg->Keypadfacility = Keypadfacility;
    cmsg->Useruserdata = Useruserdata;
    cmsg->Facilitydataarray = Facilitydataarray;
#ifndef CAPI_LIBRARY_V2
    cmsg->SendingComplete = SendingComplete;
#endif
    return capi_put_cmsg (cmsg);
}

unsigned LISTEN_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                     _cdword adr,
                     _cdword InfoMask,
                     _cdword CIPmask,
                     _cdword CIPmask2,
                     _cstruct CallingPartyNumber,
                     _cstruct CallingPartySubaddress) {
    capi_cmsg_header (cmsg,ApplId,0x05,0x80,Messagenumber,adr);
    cmsg->InfoMask = InfoMask;
    cmsg->CIPmask = CIPmask;
    cmsg->CIPmask2 = CIPmask2;
    cmsg->CallingPartyNumber = CallingPartyNumber;
    cmsg->CallingPartySubaddress = CallingPartySubaddress;
    return capi_put_cmsg (cmsg);
}

unsigned MANUFACTURER_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                           _cdword adr,
                           _cdword ManuID,
                           _cdword Class,
                           _cdword Function,
                           _cstruct ManuData) {
    capi_cmsg_header (cmsg,ApplId,0xff,0x80,Messagenumber,adr);
    cmsg->ManuID = ManuID;
    cmsg->Class = Class;
    cmsg->Function = Function;
    cmsg->ManuData = ManuData;
    return capi_put_cmsg (cmsg);
}

unsigned RESET_B3_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                       _cdword adr,
                       _cstruct NCPI) {
    capi_cmsg_header (cmsg,ApplId,0x87,0x80,Messagenumber,adr);
    cmsg->NCPI = NCPI;
    return capi_put_cmsg (cmsg);
}

unsigned SELECT_B_PROTOCOL_REQ (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                                _cdword adr,
                                _cword B1protocol,
                                _cword B2protocol,
                                _cword B3protocol,
                                _cstruct B1configuration,
                                _cstruct B2configuration,
                                _cstruct B3configuration
#ifndef CAPI_LIBRARY_V2
                                ,_cstruct Globalconfiguration
#endif
                                ) {
    capi_cmsg_header (cmsg,ApplId,0x41,0x80,Messagenumber,adr);
    cmsg->B1protocol = B1protocol;
    cmsg->B2protocol = B2protocol;
    cmsg->B3protocol = B3protocol;
    cmsg->B1configuration = B1configuration;
    cmsg->B2configuration = B2configuration;
    cmsg->B3configuration = B3configuration;
#ifndef CAPI_LIBRARY_V2
    cmsg->Globalconfiguration = Globalconfiguration;
#endif
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                       _cdword adr,
                       _cword Reject,
                       _cword B1protocol,
                       _cword B2protocol,
                       _cword B3protocol,
                       _cstruct B1configuration,
                       _cstruct B2configuration,
                       _cstruct B3configuration,
#ifndef CAPI_LIBRARY_V2
                       _cstruct Globalconfiguration,
#endif
                       _cstruct ConnectedNumber,
                       _cstruct ConnectedSubaddress,
                       _cstruct LLC,
                       _cstruct BChannelinformation,
                       _cstruct Keypadfacility,
                       _cstruct Useruserdata,
                       _cstruct Facilitydataarray) {
    capi_cmsg_header (cmsg,ApplId,0x02,0x83,Messagenumber,adr);
    cmsg->Reject = Reject;
    cmsg->B1protocol = B1protocol;
    cmsg->B2protocol = B2protocol;
    cmsg->B3protocol = B3protocol;
    cmsg->B1configuration = B1configuration;
    cmsg->B2configuration = B2configuration;
    cmsg->B3configuration = B3configuration;
#ifndef CAPI_LIBRARY_V2
    cmsg->Globalconfiguration = Globalconfiguration;
#endif
    cmsg->ConnectedNumber = ConnectedNumber;
    cmsg->ConnectedSubaddress = ConnectedSubaddress;
    cmsg->LLC = LLC;
    cmsg->BChannelinformation = BChannelinformation;
    cmsg->Keypadfacility = Keypadfacility;
    cmsg->Useruserdata = Useruserdata;
    cmsg->Facilitydataarray = Facilitydataarray;
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_ACTIVE_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                              _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x03,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_B3_ACTIVE_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                                 _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x83,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_B3_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                          _cdword adr,
                          _cword Reject,
                          _cstruct NCPI) {
    capi_cmsg_header (cmsg,ApplId,0x82,0x83,Messagenumber,adr);
    cmsg->Reject = Reject;
    cmsg->NCPI = NCPI;
    return capi_put_cmsg (cmsg);
}

unsigned CONNECT_B3_T90_ACTIVE_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                                     _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x88,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned DATA_B3_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                       _cdword adr,
                       _cword DataHandle) {
    capi_cmsg_header (cmsg,ApplId,0x86,0x83,Messagenumber,adr);
    cmsg->DataHandle = DataHandle;
    return capi_put_cmsg (cmsg);
}

unsigned DISCONNECT_B3_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                             _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x84,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned DISCONNECT_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                          _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x04,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned FACILITY_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                        _cdword adr,
                        _cword FacilitySelector,
                        _cstruct FacilityResponseParameters) {
    capi_cmsg_header (cmsg,ApplId,0x80,0x83,Messagenumber,adr);
    cmsg->FacilitySelector = FacilitySelector;
    cmsg->FacilityResponseParameters = FacilityResponseParameters;
    return capi_put_cmsg (cmsg);
}

unsigned INFO_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                    _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x08,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}

unsigned MANUFACTURER_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                            _cdword adr,
                            _cdword ManuID,
                            _cdword Class,
                            _cdword Function,
                            _cstruct ManuData) {
    capi_cmsg_header (cmsg,ApplId,0xff,0x83,Messagenumber,adr);
    cmsg->ManuID = ManuID;
    cmsg->Class = Class;
    cmsg->Function = Function;
    cmsg->ManuData = ManuData;
    return capi_put_cmsg (cmsg);
}

unsigned RESET_B3_RESP (_cmsg *cmsg, _cword ApplId, _cword Messagenumber,
                        _cdword adr) {
    capi_cmsg_header (cmsg,ApplId,0x87,0x83,Messagenumber,adr);
    return capi_put_cmsg (cmsg);
}
