/*
 * ushare.c : GeeXboX uShare UPnP Media Server.
 * Originally developped for the GeeXboX project.
 * Parts of the code are originated from GMediaServer from Oskar Liljeblad.
 * Copyright (C) 2005-2007 Benjamin Zores <ben@geexbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#if (defined(BSD) || defined(__FreeBSD__) || defined(__APPLE__))
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if_dl.h>
#endif

#if (defined(__APPLE__))
#include <net/route.h>
#endif

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>

#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#if (defined(HAVE_SETLOCALE) && defined(CONFIG_NLS))
# include <locale.h>
#endif

#include "config.h"
#include "ushare.h"
#include "services.h"
#include "http.h"
#include "metadata.h"
#include "util_iconv.h"
#include "content.h"
#include "cfgparser.h"
#include "gettext.h"
#include "trace.h"
#include "buffer.h"
#include "ctrl_telnet.h"

struct ushare_t *ut = NULL;

static struct ushare_t * ushare_new (void)
    __attribute__ ((malloc));

static struct ushare_t *
ushare_new (void)
{
  struct ushare_t *ut = (struct ushare_t *) malloc (sizeof (struct ushare_t));
  if (!ut)
    return NULL;

  ut->name = strdup (DEFAULT_USHARE_NAME);
  ut->interface = strdup (DEFAULT_USHARE_IFACE);
  ut->model_name = strdup (DEFAULT_USHARE_NAME);
  ut->contentlist = NULL;
  ut->rb = rbinit (rb_compare, NULL);
  ut->root_entry = NULL;
  ut->nr_entries = 0;
  ut->starting_id = STARTING_ENTRY_ID_DEFAULT;
  ut->init = 0;
  ut->dev = 0;
  ut->udn = NULL;
  ut->ip = NULL;
  ut->port = 0; /* Randomly attributed by libupnp */
  ut->telnet_port = CTRL_TELNET_PORT;
  ut->presentation = NULL;
  ut->use_presentation = true;
  ut->use_telnet = true;
#ifdef HAVE_DLNA
  ut->dlna_enabled = false;
  ut->dlna = NULL;
  ut->dlna_flags = DLNA_ORG_FLAG_STREAMING_TRANSFER_MODE |
                   DLNA_ORG_FLAG_BACKGROUND_TRANSFERT_MODE |
                   DLNA_ORG_FLAG_CONNECTION_STALL |
                   DLNA_ORG_FLAG_DLNA_V15;
#endif /* HAVE_DLNA */
  ut->xbox360 = false;
  ut->verbose = false;
  ut->daemon = false;
  ut->override_iconv_err = false;
  ut->cfg_file = NULL;

  pthread_mutex_init (&ut->termination_mutex, NULL);
  pthread_cond_init (&ut->termination_cond, NULL);

  return ut;
}

static void
ushare_free (struct ushare_t *ut)
{
  if (!ut)
    return;

  if (ut->name)
    free (ut->name);
  if (ut->interface)
    free (ut->interface);
  if (ut->model_name)
    free (ut->model_name);
  if (ut->contentlist)
    content_free (ut->contentlist);
  if (ut->rb)
    rbdestroy (ut->rb);
  if (ut->root_entry)
    upnp_entry_free (ut, ut->root_entry);
  if (ut->udn)
    free (ut->udn);
  if (ut->ip)
    free (ut->ip);
  if (ut->presentation)
    buffer_free (ut->presentation);
#ifdef HAVE_DLNA
  if (ut->dlna_enabled)
  {
    if (ut->dlna)
      dlna_uninit (ut->dlna);
    ut->dlna = NULL;
  }
#endif /* HAVE_DLNA */
  if (ut->cfg_file)
    free (ut->cfg_file);

  pthread_cond_destroy (&ut->termination_cond);
  pthread_mutex_destroy (&ut->termination_mutex);

  free (ut);
}

static void
ushare_signal_exit (void)
{
  pthread_mutex_lock (&ut->termination_mutex);
  pthread_cond_signal (&ut->termination_cond);
  pthread_mutex_unlock (&ut->termination_mutex);
}

static void
handle_action_request (struct Upnp_Action_Request *request)
{
  struct service_t *service;
  struct service_action_t *action;
  char val[256];
  uint32_t ip;

  if (!request || !ut)
    return;

  if (request->ErrCode != UPNP_E_SUCCESS)
    return;

  if (strcmp (request->DevUDN + 5, ut->udn))
    return;

  ip = request->CtrlPtIPAddr.s_addr;
  ip = ntohl (ip);
  sprintf (val, "%d.%d.%d.%d",
           (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

  if (ut->verbose)
  {
    DOMString str = ixmlPrintDocument (request->ActionRequest);
    log_verbose ("***************************************************\n");
    log_verbose ("**             New Action Request                **\n");
    log_verbose ("***************************************************\n");
    log_verbose ("ServiceID: %s\n", request->ServiceID);
    log_verbose ("ActionName: %s\n", request->ActionName);
    log_verbose ("CtrlPtIP: %s\n", val);
    log_verbose ("Action Request:\n%s\n", str);
    ixmlFreeDOMString (str);
  }
  
  if (find_service_action (request, &service, &action))
    {
      struct action_event_t event;

      event.request = request;
      event.status = true;
      event.service = service;

      if (action->function (&event) && event.status)
        request->ErrCode = UPNP_E_SUCCESS;

      if (ut->verbose)
      {
        DOMString str = ixmlPrintDocument (request->ActionResult);
        log_verbose ("Action Result:\n%s", str);
        log_verbose ("***************************************************\n");
        log_verbose ("\n");
        ixmlFreeDOMString (str);
      }
      
      return;
    }

  if (service) /* Invalid Action name */
    strcpy (request->ErrStr, "Unknown Service Action");
  else /* Invalid Service name */
    strcpy (request->ErrStr, "Unknown Service ID");

  request->ActionResult = NULL;
  request->ErrCode = UPNP_SOAP_E_INVALID_ACTION;
}

static int
device_callback_event_handler (Upnp_EventType type, void *event,
                               void *cookie __attribute__((unused)))
{
  switch (type)
    {
    case UPNP_CONTROL_ACTION_REQUEST:
      handle_action_request ((struct Upnp_Action_Request *) event);
      break;
    case UPNP_CONTROL_ACTION_COMPLETE:
    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
    case UPNP_CONTROL_GET_VAR_REQUEST:
      break;
    default:
      break;
    }

  return 0;
}

static int
finish_upnp (struct ushare_t *ut)
{
  if (!ut)
    return -1;

  log_info (_("Stopping UPnP Service ...\n"));
  UpnpUnRegisterRootDevice (ut->dev);
  UpnpFinish ();

  return UPNP_E_SUCCESS;
}

static int
init_upnp (struct ushare_t *ut)
{
  char *description = NULL;
  int res;
  size_t len;

  if (!ut || !ut->name || !ut->udn || !ut->ip)
    return -1;

#ifdef HAVE_DLNA
  if (ut->dlna_enabled)
  {
    len = 0;
    description =
      dlna_dms_description_get (ut->name,
                                "GeeXboX Team",
                                "http://ushare.geexbox.org/",
                                "uShare : DLNA Media Server",
                                ut->model_name,
                                "001",
                                "http://ushare.geexbox.org/",
                                "USHARE-01",
                                ut->udn,
                                "/web/ushare.html",
                                "/web/cms.xml",
                                "/web/cms_control",
                                "/web/cms_event",
                                "/web/cds.xml",
                                "/web/cds_control",
                                "/web/cds_event");
    if (!description)
      return -1;
  }
  else
  {
#endif /* HAVE_DLNA */ 
  len = strlen (UPNP_DESCRIPTION) + strlen (ut->name)
    + strlen (ut->model_name) + strlen (ut->udn) + 1;
  description = (char *) malloc (len * sizeof (char));
  memset (description, 0, len);
  sprintf (description, UPNP_DESCRIPTION, ut->name, ut->model_name, ut->udn);
#ifdef HAVE_DLNA
  }
#endif /* HAVE_DLNA */

  log_info (_("Initializing UPnP subsystem ...\n"));
  res = UpnpInit (ut->ip, ut->port);
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot initialize UPnP subsystem\n"));
    return -1;
  }

  if (UpnpSetMaxContentLength (UPNP_MAX_CONTENT_LENGTH) != UPNP_E_SUCCESS)
    log_info (_("Could not set Max content UPnP\n"));

  if (ut->xbox360)
    log_info (_("Starting in XboX 360 compliant profile ...\n"));

#ifdef HAVE_DLNA
  if (ut->dlna_enabled)
  {
    log_info (_("Starting in DLNA compliant profile ...\n"));
    ut->dlna = dlna_init ();
    dlna_set_verbosity (ut->dlna, ut->verbose ? 1 : 0);
    dlna_set_extension_check (ut->dlna, 1);
    dlna_register_all_media_profiles (ut->dlna);
  }
#endif /* HAVE_DLNA */
  
  ut->port = UpnpGetServerPort();
  log_info (_("UPnP MediaServer listening on %s:%d\n"),
            UpnpGetServerIpAddress (), ut->port);

  UpnpEnableWebserver (TRUE);

  res = UpnpSetVirtualDirCallbacks (&virtual_dir_callbacks);
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot set virtual directory callbacks\n"));
    free (description);
    return -1;
  }

  res = UpnpAddVirtualDir (VIRTUAL_DIR);
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot add virtual directory for web server\n"));
    free (description);
    return -1;
  }

  res = UpnpRegisterRootDevice2 (UPNPREG_BUF_DESC, description, 0, 1,
                                 device_callback_event_handler,
                                 NULL, &(ut->dev));
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot register UPnP device\n"));
    free (description);
    return -1;
  }

  res = UpnpUnRegisterRootDevice (ut->dev);
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot unregister UPnP device\n"));
    free (description);
    return -1;
  }

  res = UpnpRegisterRootDevice2 (UPNPREG_BUF_DESC, description, 0, 1,
                                 device_callback_event_handler,
                                 NULL, &(ut->dev));
  if (res != UPNP_E_SUCCESS)
  {
    log_error (_("Cannot register UPnP device\n"));
    free (description);
    return -1;
  }

  log_info (_("Sending UPnP advertisement for device ...\n"));
  UpnpSendAdvertisement (ut->dev, 1800);

  log_info (_("Listening for control point connections ...\n"));

  if (description)
    free (description);

  return 0;
}

static bool
has_iface (char *interface)
{
#ifdef HAVE_IFADDRS_H
  struct ifaddrs *itflist, *itf;

  if (!interface)
    return false;

  if (getifaddrs (&itflist) < 0)
  {
    perror ("getifaddrs");
    return false;
  }

  itf = itflist;
  while (itf)
  {
    if ((itf->ifa_flags & IFF_UP)
        && !strncmp (itf->ifa_name, interface, IFNAMSIZ))
    {
      log_error (_("Interface %s is down.\n"), interface);
      log_error (_("Recheck uShare's configuration and try again !\n"));
      freeifaddrs (itflist);
      return true;
    }
    itf = itf->ifa_next;
  }

  freeifaddrs (itf);
#else  
  int sock, i, n;
  struct ifconf ifc;
  struct ifreq ifr;
  char buff[8192];

  if (!interface)
    return false;

  /* determine UDN according to MAC address */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket");
    return false;
  }

  /* get list of available interfaces */
  ifc.ifc_len = sizeof (buff);
  ifc.ifc_buf = buff;

  if (ioctl (sock, SIOCGIFCONF, &ifc) < 0)
  {
    perror ("ioctl");
    close (sock);
    return false;
  }

  n = ifc.ifc_len / sizeof (struct ifreq);
  for (i = n - 1 ; i >= 0 ; i--)
  {
    ifr = ifc.ifc_req[i];

    if (strncmp (ifr.ifr_name, interface, IFNAMSIZ))
      continue;

    if (ioctl (sock, SIOCGIFFLAGS, &ifr) < 0)
    {
      perror ("ioctl");
      close (sock);
      return false;
    }

    if (!(ifr.ifr_flags & IFF_UP))
    {
      /* interface is down */
      log_error (_("Interface %s is down.\n"), interface);
      log_error (_("Recheck uShare's configuration and try again !\n"));
      close (sock);
      return false;
    }

    /* found right interface */
    close (sock);
    return true;
  }
  close (sock);
#endif
  
  log_error (_("Can't find interface %s.\n"),interface);
  log_error (_("Recheck uShare's configuration and try again !\n"));

  return false;
}

static char *
create_udn (char *interface)
{
  int sock = -1;
  char *buf;
  unsigned char *ptr;

#if (defined(BSD) || defined(__FreeBSD__) || defined(__APPLE__))
  int mib[6];
  size_t len;
  struct if_msghdr *ifm;
  struct sockaddr_dl *sdl;
#else /* Linux */
  struct ifreq ifr;
#endif

  if (!interface)
    return NULL;

#if (defined(BSD) || defined(__FreeBSD__) || defined(__APPLE__))
  mib[0] = CTL_NET;
  mib[1] = AF_ROUTE;
  mib[2] = 0;
  mib[3] = AF_LINK;
  mib[4] = NET_RT_IFLIST;

  mib[5] = if_nametoindex (interface);
  if (mib[5] == 0)
  {
    perror ("if_nametoindex");
    return NULL;
  }

  if (sysctl (mib, 6, NULL, &len, NULL, 0) < 0)
  {
    perror ("sysctl");
    return NULL;
  }

  buf = malloc (len);
  if (sysctl (mib, 6, buf, &len, NULL, 0) < 0)
  {
    perror ("sysctl");
    return NULL;
  }

  ifm = (struct if_msghdr *) buf;
  sdl = (struct sockaddr_dl*) (ifm + 1);
  ptr = (unsigned char *) LLADDR (sdl);
#else /* Linux */
  /* determine UDN according to MAC address */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket");
    return NULL;
  }

  strcpy (ifr.ifr_name, interface);
  strcpy (ifr.ifr_hwaddr.sa_data, "");

  if (ioctl (sock, SIOCGIFHWADDR, &ifr) < 0)
  {
    perror ("ioctl");
    return NULL;
  }

  buf = (char *) malloc (64 * sizeof (char));
  memset (buf, 0, 64);
  ptr = (unsigned char *) ifr.ifr_hwaddr.sa_data;
#endif /* (defined(BSD) || defined(__FreeBSD__)) */

  snprintf (buf, 64, "%s-%02x%02x%02x%02x%02x%02x", DEFAULT_UUID,
            (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
            (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

  if (sock)
    close (sock);

  return buf;
}

static char *
get_iface_address (char *interface)
{
  int sock;
  uint32_t ip;
  struct ifreq ifr;
  char *val;

  if (!interface)
    return NULL;

  /* determine UDN according to MAC address */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket");
    return NULL;
  }

  strcpy (ifr.ifr_name, interface);
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl (sock, SIOCGIFADDR, &ifr) < 0)
  {
    perror ("ioctl");
    close (sock);
    return NULL;
  }

  val = (char *) malloc (16 * sizeof (char));
  ip = ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr;
  ip = ntohl (ip);
  sprintf (val, "%d.%d.%d.%d",
           (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

  close (sock);

  return val;
}

static int
restart_upnp (struct ushare_t *ut)
{
  finish_upnp (ut);

  if (ut->udn)
    free (ut->udn);
  ut->udn = create_udn (ut->interface);
  if (!ut->udn)
    return -1;

  if (ut->ip)
    free (ut->ip);
  ut->ip = get_iface_address (ut->interface);
  if (!ut->ip)
    return -1;

  return (init_upnp (ut));
}

static void
UPnPBreak (int s __attribute__ ((unused)))
{
  ushare_signal_exit ();
}

static void
reload_config (int s __attribute__ ((unused)))
{
  struct ushare_t *ut2;
  bool reload = false;

  log_info (_("Reloading configuration...\n"));

  ut2 = ushare_new ();
  if (!ut || !ut2)
    return;

  if (parse_config_file (ut2) < 0)
    return;

  if (ut->name && strcmp (ut->name, ut2->name))
  {
    free (ut->name);
    ut->name = ut2->name;
    ut2->name = NULL;
    reload = true;
  }

  if (ut->interface && strcmp (ut->interface, ut2->interface))
  {
    if (!has_iface (ut2->interface))
    {
      ushare_free (ut2);
      raise (SIGINT);
    }
    else
    {
      free (ut->interface);
      ut->interface = ut2->interface;
      ut2->interface = NULL;
      reload = true;
    }
  }

  if (ut->port != ut2->port)
  {
    ut->port = ut2->port;
    reload = true;
  }

  if (reload)
  {
    if (restart_upnp (ut) < 0)
    {
      ushare_free (ut2);
      raise (SIGINT);
    }
  }

  if (ut->contentlist)
    content_free (ut->contentlist);
  ut->contentlist = ut2->contentlist;
  ut2->contentlist = NULL;
  ushare_free (ut2);

  if (ut->contentlist)
  {
    free_metadata_list (ut);
    build_metadata_list (ut);
  }
  else
  {
    log_error (_("Error: no content directory to be shared.\n"));
    raise (SIGINT);
  }
}

inline void
display_headers (void)
{
  printf (_("%s (version %s), a lightweight UPnP A/V and DLNA Media Server.\n"),
          PACKAGE_NAME, VERSION);
  printf (_("Benjamin Zores (C) 2005-2007, for GeeXboX Team.\n"));
  printf (_("See http://ushare.geexbox.org/ for updates.\n"));
}

inline static void
setup_i18n(void)
{
#ifdef CONFIG_NLS
#ifdef HAVE_SETLOCALE
  setlocale (LC_ALL, "");
#endif
#if (!defined(BSD) && !defined(__FreeBSD__))
  bindtextdomain (PACKAGE, LOCALEDIR);
#endif
  textdomain (PACKAGE);
#endif
}

#define SHUTDOWN_MSG _("Server is shutting down: other clients will be notified soon, Bye bye ...\n")

static void
ushare_kill (ctrl_telnet_client *client,
             int argc __attribute__((unused)),
             char **argv __attribute__((unused)))
{
  if (ut->use_telnet)
  {
    ctrl_telnet_client_send (client, SHUTDOWN_MSG);
    client->exiting = true;
  }
  ushare_signal_exit ();
}

int
main (int argc, char **argv)
{
  ut = ushare_new ();
  if (!ut)
    return EXIT_FAILURE;

  setup_i18n ();
  setup_iconv ();

  /* Parse args before cfg file, as we may override the default file */
  if (parse_command_line (ut, argc, argv) < 0)
  {
    ushare_free (ut);
    return EXIT_SUCCESS;
  }

  if (parse_config_file (ut) < 0)
  {
    /* fprintf here, because syslog not yet ready */
    fprintf (stderr, _("Warning: can't parse file \"%s\".\n"),
             ut->cfg_file ? ut->cfg_file : SYSCONFDIR "/" USHARE_CONFIG_FILE);
  }

  if (ut->xbox360)
  {
    char *name;

    name = malloc (strlen (XBOX_MODEL_NAME) + strlen (ut->model_name) + 4);
    sprintf (name, "%s (%s)", XBOX_MODEL_NAME, ut->model_name);
    free (ut->model_name);
    ut->model_name = strdup (name);
    free (name);

    ut->starting_id = STARTING_ENTRY_ID_XBOX360;
  }

  if (ut->daemon)
  {
    /* starting syslog feature as soon as possible */
    start_log ();
  }

  if (!ut->contentlist)
  {
    log_error (_("Error: no content directory to be shared.\n"));
    ushare_free (ut);
    return EXIT_FAILURE;
  }

  if (!has_iface (ut->interface))
  {
    ushare_free (ut);
    return EXIT_FAILURE;
  }

  ut->udn = create_udn (ut->interface);
  if (!ut->udn)
  {
    ushare_free (ut);
    return EXIT_FAILURE;
  }

  ut->ip = get_iface_address (ut->interface);
  if (!ut->ip)
  {
    ushare_free (ut);
    return EXIT_FAILURE;
  }

  if (ut->daemon)
  {
    int err;
    err = daemon (0, 0);
    if (err == -1)
    {
      log_error (_("Error: failed to daemonize program : %s\n"),
                 strerror (err));
      ushare_free (ut);
      return EXIT_FAILURE;
    }
  }
  else
  {
    display_headers ();
  }

  signal (SIGINT, UPnPBreak);
  signal (SIGHUP, reload_config);

  if (ut->use_telnet)
  {
    if (ctrl_telnet_start (ut->telnet_port) < 0)
    {
      ushare_free (ut);
      return EXIT_FAILURE;
    }
    
    ctrl_telnet_register ("kill", ushare_kill,
                          _("Terminates the uShare server"));
  }
  
  if (init_upnp (ut) < 0)
  {
    finish_upnp (ut);
    ushare_free (ut);
    return EXIT_FAILURE;
  }

  build_metadata_list (ut);

  /* Let main sleep until it's time to die... */
  pthread_mutex_lock (&ut->termination_mutex);
  pthread_cond_wait (&ut->termination_cond, &ut->termination_mutex);
  pthread_mutex_unlock (&ut->termination_mutex);

  if (ut->use_telnet)
    ctrl_telnet_stop ();
  finish_upnp (ut);
  free_metadata_list (ut);
  ushare_free (ut);
  finish_iconv ();

  /* it should never be executed */
  return EXIT_SUCCESS;
}
