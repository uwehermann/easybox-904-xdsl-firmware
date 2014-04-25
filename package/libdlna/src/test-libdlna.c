#include <stdlib.h>
#include <stdio.h>

#include "dlna.h"

int
main (int argc, char **argv)
{
  dlna_t *dlna;
  dlna_profile_t *p;
  dlna_org_flags_t flags;

  if (argc < 2)
  {
    printf ("usage: %s media_filename\n", argv[0]);
    return -1;
  }

  flags = DLNA_ORG_FLAG_STREAMING_TRANSFER_MODE |
    DLNA_ORG_FLAG_BACKGROUND_TRANSFERT_MODE |
    DLNA_ORG_FLAG_CONNECTION_STALL |
    DLNA_ORG_FLAG_DLNA_V15;
  
  printf ("Using %s\n", LIBDLNA_IDENT);
  
  dlna = dlna_init ();
  dlna_set_verbosity (dlna, 1);
  dlna_register_all_media_profiles (dlna);
  
  p = dlna_guess_media_profile (dlna, argv[1]);
  if (p)
  {
    char *protocol_info;
    
    printf ("ID: %s\n", p->id);
    printf ("MIME: %s\n", p->mime);
    printf ("Label: %s\n", p->label);
    printf ("Class: %d\n", p->class);
    printf ("UPnP Object Item: %s\n", dlna_profile_upnp_object_item (p));

    protocol_info = dlna_write_protocol_info (DLNA_PROTOCOL_INFO_TYPE_HTTP,
                                              DLNA_ORG_PLAY_SPEED_NORMAL,
                                              DLNA_ORG_CONVERSION_NONE,
                                              DLNA_ORG_OPERATION_RANGE,
                                              flags, p);
    printf ("Protocol Info: %s\n", protocol_info);
    free (protocol_info);
  }
  else
    printf ("Unknown format\n");

  dlna_uninit (dlna);
  
  return 0;
}
