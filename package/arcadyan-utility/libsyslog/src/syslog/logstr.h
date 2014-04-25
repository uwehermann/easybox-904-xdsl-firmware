
#ifndef _PPP_SYS_LOG_H
#define _PPP_SYS_LOG_H

// log language define
#define SYS_LOG_LANG_EN						0
#define SYS_LOG_LANG_DE						1

#ifdef _GERMAN_SYS_LOG
#define MAX_LANGUAGE_SUPPORT				2
#else
#define MAX_LANGUAGE_SUPPORT				1
#endif // _GERMAN_SYS_LOG
#define DEFAULT_SYS_LOG_LANG				SYS_LOG_LANG_EN


extern char *getLogStrFmt(unsigned char lang_type, unsigned short msg_type);


struct logStrTbl_s {
	unsigned short msg_type;
	char *str;
};

#ifdef _GERMAN_SYS_LOG
#define de_PPP_SYS_LOG_START				"startet PPP"
#define de_PPP_SYS_LOG_STOP					"beendet PPP"
#define de_PPP_SYS_LOG_ON_DEMAND			"Einwahl"
#define de_PPP_SYS_LOG_PASS_OK				"Benutzername und Passwort: OK"
#define de_PPP_SYS_LOG_PASS_FAIL			"PAP Benutzername und Passwort: fehlgeschlagen"
#define de_PPP_SYS_LOG_CHAP_PASS_FAIL		"CHAP Benutzername und Passwort: fehlgeschlagen"
#define de_PPP_SYS_LOG_PASS_TIMEOUT			"PAP Authentifizierung ausgelaufen"
#define de_PPP_SYS_LOG_CHAP_TIMEOUT			"CHAP Authentifizierung ausgelaufen"
#define de_PPP_SYS_LOG_PEER_REJECT			"PPP Verbindung von Gegenstelle beendet"
#define de_PPP_SYS_LOG_LCP_TIMEOUT			"PPP-LCP Aushandlung abgelaufen"
#define de_PPP_SYS_LOG_MODEM_BROKE			"Modemverbindung unterbrochen"
#define de_PPP_SYS_LOG_MODEM_ERR			"Modemfehler"
#define de_PPP_SYS_LOG_MODEM_START			"Modem beginnt mit Einwahl"
#define de_PPP_SYS_LOG_MODEM_STOP			"Modem beginnt mit Abwahl"
#define de_PPP_SYS_LOG_MODEM_BUSY			"Die Telefonleitung zur Einwahl ist belegt"
#define de_PPP_SYS_LOG_MODEM_NO_TONE		"Die Telefonleitung zur Einwahl erhält kein Freizeichen"
#define de_PPP_SYS_LOG_MODEM_BAUDRATE		"Modem-Übertragungsrate"
#define de_PPP_SYS_LOG_CONN_OK				"If(%s) PPP Verbindung hergestellt !"	// %s is interface description
#define de_PPP_SYS_LOG_CONN_FAIL			"If(%s) PPP Fehler : %s"	// %s, %s is interface description, error message
#define de_PPP_SYS_LOG_DIAL_TIMEOUT			"If(%s) PPP Zeituberschreitung bei Einwahl : %lu"	// %s, %lu is interface description, dialing timeout
#define de_PPPOE_SYS_LOG_PADI				"If(%s) sende PADI"
#define de_PPPOE_SYS_LOG_PADR				"If(%s) sende PADR"
#define de_PPPOE_SYS_LOG_PADO				"If(%s) empfange PADO"
#define de_PPPOE_SYS_LOG_PADS				"If(%s) empfange PADS"
#define de_PPPOE_SYS_LOG_PADT				"If(%s) empfange PADT"
#define de_PPPOE_SYS_LOG_STOP				"If(%s) Stopp"
#define de_PPPOE_SYS_LOG_DNS1				"If(%s) erhalte erste DNS-IP:%u.%u.%u.%u"
#define de_PPPOE_SYS_LOG_DNS2				"If(%s) erhalte zweite DNS-IP:%u.%u.%u.%u"
#define de_PPPOE_SYS_LOG_RETRY_FAIL			"PPPoE Verbindungsfehler nach %d Versuch(en). Wiederherstellung des PPPoE Standardprofils."

#define de_PPPOE_SYS_LOG_ACS_TIMEOUT		"TR69 connection timeout. Restore to original PPPoE profile."
#define de_PPTP_SYS_LOG_LOGIN_SUCC			"%s Einwahl unter der Nummer %d des PPTP Servers mit der IP %s erfolgreich" // %s, %d, %s are username, line number, ipaddress
#define de_PPTP_SYS_LOG_LOGIN_FAIL			"%s keine Einwahl zum PPTP Server mit der IP %s." // %s, %s are username, ipaddress
#define de_PPTP_SYS_LOG_SERVER				"PPTP Server"
#define de_PPTP_SYS_LOG_CLIENT				"PPTP Client"
#define de_PPTP_SYS_LOG_CONNECT				"If(%s) mit %d verbunden" // %s, %d are server/client and line number
#define de_PPTP_SYS_LOG_GET_IP				"If(%s) erhält IP:%u.%u.%u.%u" //
#define de_PPTP_SYS_LOG_DESTROY				"%s Tunnelverbindung abgebrochen"// %s is peer IP
#define de_PPTP_SYS_LOG_CREATE				"%s hat erfolgreich einen Tunnel aufgebaut"// %s is peer IP
#define de_L2TP_SYS_LOG_LOGIN_SUCC			"%s login Line %d of L2TP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress,  // PTR-TI-0113
#define de_L2TP_SYS_LOG_LOGIN_FAIL			"%s unable to log into L2TP Server from %s."				// %s, %s are username, ipaddress
#define de_L2TP_SYS_LOG_SERVER				"L2TP server"
#define de_L2TP_SYS_LOG_CLIENT				"L2TP client"
#define de_L2TP_SYS_LOG_CONNECT				"If(%s) Line %d connected"
#define de_L2TP_SYS_LOG_GET_IP				"If(%s) get IP:%u.%u.%u.%u"
#define de_L2TP_SYS_LOG_DESTROY				"%s destroy tunnel successfully"// %s is peer IP
#define de_L2TP_SYS_LOG_CREATE				"%s creates tunnel successfully"// %s is peer IP

#define de_DYNDNS_SYS_LOG_COMPLETE			"Vorgang abgeschlossen"
#define de_DYNDNS_SYS_LOG_PASS_FAIL			"Ungültiges Konto / Passwort."
#define de_DYNDNS_SYS_LOG_SERVER_ERR		"Unerwarteter Server-Fehler."
#define de_DYNDNS_SYS_LOG_DOMAIN_ERR		"Ungültiger Domänen-Name."
#define de_DYNDNS_SYS_LOG_DOMAIN_DUP		"Der Domänen-Namen existiert bereits."
#define de_DYNDNS_SYS_LOG_CHECK_UPDATE		"Prüfen und Aktualisieren .."
#define de_DYNDNS_SYS_LOG_SESSION_DISABLE	"PPPoE Verbindung ist nicht aktiviert."
#define de_DYNDNS_SYS_LOG_REGISTED			"DDNS> Diese Domäne ist derzeit folgender IP zugeordnet:%s"
#define de_DYNDNS_SYS_LOG_DNS_ERR			"DDNS> Keine Rückmeldung vom DNS-Server. Angefragter Name des Host:%s"
#define de_TZODDNS_SYS_LOG_EXPIRE			"Das Benutzer-Konto ist abgelaufen."
#define de_TZODDNS_SYS_LOG_KEY_ERR			"Ungültiger Schlüssel."
#define de_TZODDNS_SYS_LOG_ERR				"Ein unerwarteter Fehler ist aufgetreten."
#define de_TZODDNS_SYS_LOG_SERVER_ERR		"Unerwarteter Server-Fehler."
#define de_TZODDNS_SYS_LOG_COMPLETE			"Vorgang abgeschlossen"
#define de_TZODDNS_SYS_LOG_DOMAIN_ERR		"Ungültiger Domänen-Name"
#define de_TZODDNS_SYS_LOG_MAIL_ERR			"Ungültige eMail-Adresse"
#define de_TZODDNS_SYS_LOG_ACTION_ERR		"Ungültiger Vorgang."
#define de_TZODDNS_SYS_LOG_IP_ERR			"Ungültige IP-Adresse"
#define de_TZODDNS_SYS_LOG_DOMAIN_DUP		"Doppelter Domänen-Name"
#define de_TZODDNS_SYS_LOG_DOMAIN_EXIST		"Der Domänen-Namen existiert bereits."
#define de_IVYDNS_SYS_LOG_COMPLETE			"Operation complete"	// PTR-TI-0113

#define de_DHCPCLT_SYS_LOG_DOMAIN_NAME		"Domänen-Name"
#define de_DHCPCLT_SYS_LOG_RECV_OFFER		"Empfange 'Offer' von"
#define de_DHCPCLT_SYS_LOG_DISCOVER			"Sende 'Discover'"
#define de_DHCPCLT_SYS_LOG_REQUEST_IP		"Sende 'Request', Request-IP"
#define de_DHCPCLT_SYS_LOG_RELEASE			"Sende 'Release'"
#define de_DHCPCLT_SYS_LOG_DECLINE			"Sende 'Decline'"
#define de_DHCPCLT_SYS_LOG_GET_INFO_ERR		"Der DHCP-Daemon konnte nicht gefunden werden. Keine Informationen erhalten."
#define de_DHCPCLT_SYS_LOG_RECV_ACK			"Empfange 'Ack' von"
#define de_DHCPCLT_SYS_LOG_LEASE_TIME		"Lease time"
#define de_DHCPD_SYS_LOG_SEND_OFFER			"sende OFFER an"
#define de_DHCPD_SYS_LOG_SEND_ACK			"sende ACK an"
#define de_DHCPD_SYS_LOG_RELEASE			"%02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define de_DHCPD_SYS_LOG_WLAN_RELEASE		"Wireless %02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define de_DHCPD_SYS_LOG_EXCLUDE_MAC		"Ausschluß der MAC-Adresse %02X-%02X-%02X-%02X-%02X-%02X" // %02X.. are MAC address
#define de_BIGPOND_SYS_LOG_LOGIN_TERMIN		"BigPond: BigPond Einwahlvorgang abgebrochen."
#define de_BIGPOND_SYS_LOG_LOGIN_FAIL		"BigPond: Start der BigPond-Einwahl nicht möglich."
#define de_BIGPOND_SYS_LOG_LOGIN_SUCC		"BigPond: Start der BigPond Einwahl erfolgreich."
#define de_SMTP_SYS_LOG_FAIL				"Fehlermeldung"
#define de_SMTP_SYS_LOG_POP_CONNECT_ERR		"SMTP> Fehler beim Verbinden mit POP-Server."
#define de_SMTP_SYS_LOG_SEND_OK				"SMTP> Senden der Alarmmeldung war erfolgreich."
#define de_SMTP_SYS_LOG_SEND_FAIL			"SMTP> Fehler beim Senden der Alarmmeldung. Wartezeit 10 Sekunden."
#define de_SMTP_MAIL_ALERT1					"Guten Tag \r\nIhr Router hat einen Versuch des Eindringens in Ihr Netzwerk festgestellt und abgewehrt. Es könnte ein Hacker-Angriff gewesen sein, aber auch nur die harmlose Wartungsarbeit Ihres Service Providers.\r\nÜber die meisten dieser Netzwerktests muss man sich keine Gedanken machen. Es handelt sich um Versuche, die nach dem Zufallsprinzip durchgeführt werden und NICHT gemeldet werden sollten. Heben Sie diese eMail auf und vergleichen Sie sie mit zukünftigen Alarmmeldungen\r\nAlarmmeldung Ihres Routers\r\n\r\n"
#define de_SMTP_MAIL_ALERT2					"\r\nBesuchen Sie die UNX Combat Spam Website, um mehr über den Eindringling zu erfahren: http://combat.uxn.com/\r\n1. Geben Sie in der WHOIS-Suchmaschine die IP Adresse des Eindringlings ein\r\n2. Klicken Sie auf den Query-(Untersuchen-) Button\r\n3. Detaillierte Informationen über das Netz und die Administration werden angezeigt\r\n\r\n"
#define de_NAT_CGI_SYS_LOG					"Formatfehler am öffentlichen Port"
#define de_W1X_SYS_LOG_LOGON				"802.1X Client %s Anmeldung"					// %s is MAC address string
#define de_W1X_SYS_LOG_REAUTH				"802.1X Client %s neu beglaubigt."				// %s is MAC address string
#define de_W1X_SYS_LOG_LOGOFF				"802.1X Client %s Abmeldung."					// %s is MAC address string
#define de_W1X_SYS_LOG_TIMEOUT				"802.1X Client %s Abmeldung nach Time-Out."		// %s is MAC address string
#define de_W1X_SYS_LOG_DISCONNECT			"Trenne 802.1x Client %s, Grund:%d"				// %s is MAC address string, %d is error code
#define de_NTP_SYS_LOG_QUERY				"NTP-Anfrage gestartet."
#define de_NTP_SYS_LOG_UPDATE				"NTP Datum und Uhrzeit wurden aktualisiert."
#define de_NTP_SYS_LOG_CANNT_FIND			"NTP Zeit nicht feststellbar."
#define de_NTP_SYS_LOG_SERVIP				"Verwenden Sie die Zeit des NTP-Servers für die Systemzeit:%s."
#define de_HTTP_SYS_LOG_USER_TIMEOUT		"IP-Adresse %s: Abmeldung nach Time-Out."		// %s is IP address
#define de_HTTP_SYS_LOG_DUP_USER			"Doppelte Benutzeranmeldung von IP-Adresse %s"	// %s is IP address
#define de_HTTP_SYS_LOG_LOGIN_SUCCESS		"System: %s Anmeldung erfolgreich"
#define de_USB_SYS_LOG_NO_DEV				"[USB] kein Gerät angeschlossen! "
#define de_USB_SYS_LOG_SET_INPUT_CONNECT	"[USBLPT] Set Input Connection .. "
#define de_USB_SYS_LOG_SET_OUTPUT_ONLY		"[USBLPT] Set Output Only .. "
#define de_USB_SYS_LOG_OUTPUT_ONLY			"[USBLPT] This Printer is Output Only "
#define de_USB_SYS_LOG_PR_INIT_OK			"[USBLPT] Initialisierung des Druckers erfolgreich. "
#define de_USB_SYS_LOG_PR_WR_FAIL			"[USBLPT] Fehler: Keine Verbindung zum Drucker "
#define de_USB_SYS_LOG_PR_WR_DATA_FAIL		"[USBLPT] Fehler beim schreiben der Druckdaten! "
#define de_USB_SYS_LOG_PR_RETRY				"[USBLPT] Erneut versuchen .. "
#define de_USB_SYS_LOG_PR_INIT_RESET_OK		"[USBLPT] Zurücksetzen des Druckers erfolgreich. "
#define de_USB_SYS_LOG_PR_INIT_RESET_FAIL	"[USBLPT] Fehler beim zurücksetzen des Druckers"
#define de_USB_SYS_LOG_PR_RESET_FAIL		"[USBLPT] Fehler : Kann Drucker nicht zurücksetzen"
#define de_USB_SYS_LOG_PR_RESET_OK			"[USBLPT] Zurücksetzen des Druckers erfolgreich. "
#define de_USB_SYS_LOG_PR_EPSON_INIT		"[USBLPT] Init Epson (or OEM) Printer .. "
#define de_USB_SYS_LOG_PR_RESER_DUMMY		"[USBLPT] Dummy Reset .. "
#define de_USB_SYS_LOG_PR_STATUS_ERR		"[USBLPT] Fehlermeldung des Druckers, Tinte leer / ohne Papier / Papierstau ? "
#define de_USB_SYS_LOG_PR_UNSUPP			"[USBLPT] Fehler: Die Firmware unterstützt diesen Drucker nicht! "
#define de_USB_SYS_LOG_PR_NO_RESET			"[USBLPT] This Printer Has not Reset Command "
#define de_USB_SYS_LOG_PR_RESET_11			"[USBLPT] This Printer Only Accept Version 1.1 Reset Command "
#define de_USB_SYS_LOG_PR_FOUND				"[USB] Drucker gefunden"
#define de_USB_SYS_LOG_PR_REMOVE			"[USBLPT] USB Drucker entfernt "
#define de_USB_SYS_LOG_PR_PROTOCOL			"[USBLPT] USB LPT Protokoll eingestellt auf %d \n" // %d is protocol number
#define de_USB_SYS_LOG_PR_ID_ERR			"[USBLPT] Fehler beim auslesen der IEEE-1284 konformen Typenkennung! "
#define de_USB_SYS_LOG_PR_ID				"[USBLPT] Typenkennung"
#define de_USB_SYS_LOG_MAC_REMOVE			"[USBMAC] USB MAC entfernt "
#define de_USB_SYS_LOG_MAC_NO_DEV			"[USBMAC] Kein USB MAC Gerät! "
#define de_USB_SYS_LOG_MAC_FOUND			"[USB] MAC gefunden"
#define de_USB_SYS_LOG_DEV_NUM				"[USB] Anzahl externer Geräte = %d " // %d is device number
#define de_USB_SYS_LOG_DEV_RESET_OK			"[USB] Zurücksetzen des Controllers erfolgreich. "
#define de_USB_SYS_LOG_DEV_UNSUPP			"[USB] Gerät wird nicht untersützt"
#define de_USB_SYS_LOG_DEV_SPEED			"[USB] Geschwindigkeit des Geräts"
#define de_USB_SYS_LOG_DEV_DISCONN			"[USB] abgemeldet an Anschluß %d " // %d is device number
#define de_USB_SYS_LOG_DEV_REMOVE			"[USB] Gerät wurde entfernt! "
#define de_USB_SYS_LOG_DEV_TYPE_CHECK		"[USB] Prüfe den Gerätetyp .. "
#define de_USB_SYS_LOG_DEV_NEW				"[USB] Neuem USB Gerät wurde folgende Adresse zugewiesen %d "// %d is device number
#define de_USB_SYS_LOG_HUB_PORT_RST			"[USB] HUB : (USB Gerät %d) Anchluß zurückgesetzt %d "// %d, %d are device number &amp; reset port
#define de_USB_SYS_LOG_HUB_PORT_CHANGE		"[USB] HUB Anschluß %d aktiviert, Statusänderung %x "// %d, %x are device number and status code
#define de_UBS_SYS_LOG_HUB_FOUND			"[USB] Externen HUB gefunden"
#define de_USB_SYS_LOG_MOUSE_FOUND			"[USB] Maus gefunden"
#define de_USB_SYS_LOG_BULK_TIMEOUT			"[USB] Fehler: Zeitüberschreitung bei der Bulk Kommunikation! "
#define de_USB_SYS_LOG_INTR_TIMEOUT			"[USB] Fehler: Zeitüberschreitung bei der Interrupt Kommunikation! "
#define de_USB_SYS_LOG_REACT_TIMEOUT		"[USB] Fehler: raced timeout, Status %d timeout %d " // %d, %d are status code, timeout
#define de_USB_SYS_LOG_ERR_TIMEOUT			"[USB] Fehler: Zeitüberschreitung! "
#define de_USB_SYS_LOG_CTRL_TIMEOUT			"[USB] Fehler: Zeitüberschreitung bei der Kontrollkommunikation! "
#define de_ADSL_SYS_LOG_MEDIA_UP			"ADSL Medium verbunden (DS/US - %d/%d kbps.) !"
#define de_ADSL_SYS_LOG_MEDIA_DOWN			"ADSL Medium nicht verbunden !"
#define de_CBAC_SYS_LOG_REJECT				"Zurückgewiesenes"
#define de_CBAC_SYS_LOG_RELAY				"Weitergeleitetes"
#define de_CBAC_SYS_LOG_FROM				"von"
#define de_CBAC_SYS_LOG_TO					"bis"
#define de_CBAC_SYS_LOG_PACKET				"Paket"
#define de_CBAC_SYS_LOG_INBOUND				"- Eingang"
#define de_CBAC_SYS_LOG_OUTBOUND			"- Ausgang"
#define de_CBAC_SYS_LOG_MSG					"Message: %s%s\r\n"
#define de_CBAC_SYS_LOG_MSG_TIME			"Uhrzeit: %s\r\Message>: %s%s\r\n"
#define de_CBAC_SYS_LOG_MSG_SRC				"Message: %s\r\nVon: %s\r\nZum:%s%s\r\n"
#define de_CBAC_SYS_LOG_MSG_SRC_TIME		"Uhrzeit: %s\r\nMessage: %s\r\nVon: %s\r\nZum:%s%s\r\n"
//#define de_VOIP_SYS_LOG_VC2_TO_VC1			"[VoIP] VC2 fehlgeschlagen, registrieren des VoIP Kontos %d zu VC1 mit einer Verzögerung von %d Sekunden."
#define de_VOIP_SYS_LOG_VC2_TO_VC1			"[VoIP] Registrieren des Kontos %d zu VC2 fehlgeschlagen%s, registrieren zu VC1 mit einer Verzögerung von %d Sekunden."
#define de_VOIP_SYS_LOG_VC1_TO_VC2			"[VoIP] Registrierung des VoIP Kontos %d zurück auf VC2"
#define de_VOIP_SYS_LOG_REG_OK_FROM_VC		"[VoIP] SIP Konto %d erfolgreich registriert bei VC%d"
#define de_VOIP_SYS_LOG_REG_FAIL			"[VoIP] SIP Konto %d fehlgeschlagen registriert(%d)"
#define de_VOIP_SYS_LOG_REG_TIMEOUT			"[VoIP] SIP Konto %d registriert ausgelaufen"
#define de_FTPD_SYS_LOG_LOGIN_OK			"[FTPD] Benutzer %s erfolgreich anmeldebildschirm bei %s"
#define de_FTPD_SYS_LOG_LOGIN_FAIL			"[FTPD] Benutzer %s fehlgeschlagen anmeldebildschirm bei %s"
#define de_FTPD_SYS_LOG_LOGOUT				"[FTPD] Benutzer %s(%s) abmelden"
#define de_FTPD_SYS_LOG_PUT					"[FTPD] Benutzer %s(%s) die feile anbringen:%s"
#define de_FTPD_SYS_LOG_GET					"[FTPD] Benutzer %s(%s) die feile erhalten:%s"
#endif // _GERMAN_SYS_LOG

#ifdef _ENGLISH_SYS_LOG
#define en_PPP_SYS_LOG_START				"start PPP"
#define en_PPP_SYS_LOG_STOP					"stop PPP"
#define en_PPP_SYS_LOG_ON_DEMAND			"Dial On Demand"
#define en_PPP_SYS_LOG_PASS_OK				"Username and Password: OK"
#define en_PPP_SYS_LOG_PASS_FAIL			"PAP Username and Password: failed"
#define en_PPP_SYS_LOG_CHAP_PASS_FAIL		"CHAP Username and Password: failed"
#define en_PPP_SYS_LOG_PASS_TIMEOUT			"PAP Authentication: timeout"
#define en_PPP_SYS_LOG_CHAP_TIMEOUT			"CHAP Authentication: timeout"
#define en_PPP_SYS_LOG_PEER_REJECT			"PPP connection terminated by peer"
#define en_PPP_SYS_LOG_LCP_TIMEOUT			"PPP-LCP negotiation timeout"
#define en_PPP_SYS_LOG_MODEM_BROKE			"Modem connection broken"
#define en_PPP_SYS_LOG_MODEM_ERR			"Modem is error"
#define en_PPP_SYS_LOG_MODEM_START			"Modem start to dial up"
#define en_PPP_SYS_LOG_MODEM_STOP			"Modem start to hang up"
#define en_PPP_SYS_LOG_MODEM_BUSY			"The phone line of dial-up is busy"
#define en_PPP_SYS_LOG_MODEM_NO_TONE		"The phone line of dial-up has no dial tone"
#define en_PPP_SYS_LOG_MODEM_BAUDRATE		"Modem connection baud rate"
#define en_PPP_SYS_LOG_CONN_OK				"If(%s) PPP connection ok !"	// %s is interface description
#define en_PPP_SYS_LOG_CONN_FAIL			"If(%s) PPP fail : %s"	// %s, %s is interface description, error message
#define en_PPP_SYS_LOG_DIAL_TIMEOUT			"If(%s) PPP Dial timeout : %lu"	// %s, %lu is interface description, dialing timeout
#define en_PPPOE_SYS_LOG_PADI				"If(%s) send PADI"
#define en_PPPOE_SYS_LOG_PADR				"If(%s) send PADR"
#define en_PPPOE_SYS_LOG_PADO				"If(%s) receive PADO"
#define en_PPPOE_SYS_LOG_PADS				"If(%s) receive PADS"
#define en_PPPOE_SYS_LOG_PADT				"If(%s) receive PADT"
#define en_PPPOE_SYS_LOG_STOP				"If(%s) stop"
#define en_PPPOE_SYS_LOG_DNS1				"If(%s) get primary DNS IP:%u.%u.%u.%u"
#define en_PPPOE_SYS_LOG_DNS2				"If(%s) get secondary DNS IP:%u.%u.%u.%u"
#define en_PPPOE_SYS_LOG_RETRY_FAIL			"PPPoE connect fail after %d times retry. Recover to default PPPoE profile."
#define en_PPPOE_SYS_LOG_ACS_TIMEOUT		"TR69 connection timeout. Restore to original PPPoE profile."
#define en_PPTP_SYS_LOG_LOGIN_SUCC			"%s login Line %d of PPTP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress
#define en_PPTP_SYS_LOG_LOGIN_FAIL			"%s unable to log into PPTP Server from %s."				// %s, %s are username, ipaddress
#define en_PPTP_SYS_LOG_SERVER				"PPTP server"
#define en_PPTP_SYS_LOG_CLIENT				"PPTP client"
#define en_PPTP_SYS_LOG_CONNECT				"If(%s) Line %d connected"
#define en_PPTP_SYS_LOG_GET_IP				"If(%s) get IP:%u.%u.%u.%u"
#define en_PPTP_SYS_LOG_DESTROY				"%s destroy tunnel successfully"// %s is peer IP
#define en_PPTP_SYS_LOG_CREATE				"%s creates tunnel successfully"// %s is peer IP
#define en_L2TP_SYS_LOG_LOGIN_SUCC			"%s login Line %d of L2TP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress,  // PTR-TI-0113
#define en_L2TP_SYS_LOG_LOGIN_FAIL			"%s unable to log into L2TP Server from %s."				// %s, %s are username, ipaddress
#define en_L2TP_SYS_LOG_SERVER				"L2TP server"
#define en_L2TP_SYS_LOG_CLIENT				"L2TP client"
#define en_L2TP_SYS_LOG_CONNECT				"If(%s) Line %d connected"
#define en_L2TP_SYS_LOG_GET_IP				"If(%s) get IP:%u.%u.%u.%u"
#define en_L2TP_SYS_LOG_DESTROY				"%s destroy tunnel successfully"// %s is peer IP
#define en_L2TP_SYS_LOG_CREATE				"%s creates tunnel successfully"// %s is peer IP
#define en_DYNDNS_SYS_LOG_COMPLETE			"Operation complete"
#define en_DYNDNS_SYS_LOG_PASS_FAIL			"Invalid Account(username or password)"
#define en_DYNDNS_SYS_LOG_SERVER_ERR		"An unexpected server error."
#define en_DYNDNS_SYS_LOG_DOMAIN_ERR		"Invalid Domain Name."
#define en_DYNDNS_SYS_LOG_DOMAIN_DUP		"Domain Name has already been created for this domain name."
#define en_DYNDNS_SYS_LOG_CHECK_UPDATE     	"Check and Update .."
#define en_DYNDNS_SYS_LOG_SESSION_DISABLE	"PPPoE Session is not Enabled."
#define en_DYNDNS_SYS_LOG_REGISTED			"DDNS> This domain is currently registed by other IP:%s"
#define en_DYNDNS_SYS_LOG_DNS_ERR			"DDNS> No response from DNS server. Request hostname:%s"
#define en_TZODDNS_SYS_LOG_EXPIRE			"The account has expired."
#define en_TZODDNS_SYS_LOG_KEY_ERR			"Invalid Key"
#define en_TZODDNS_SYS_LOG_ERR				"An unexpected error has occurred."
#define en_TZODDNS_SYS_LOG_SERVER_ERR		"An unexpected server error."
#define en_TZODDNS_SYS_LOG_COMPLETE			"Operation complete"
#define en_TZODDNS_SYS_LOG_DOMAIN_ERR		"Invalid Domain Name"
#define en_TZODDNS_SYS_LOG_MAIL_ERR			"Invalid Email"
#define en_TZODDNS_SYS_LOG_ACTION_ERR		"Invalid Action"
#define en_TZODDNS_SYS_LOG_IP_ERR			"Invalid IP address"
#define en_TZODDNS_SYS_LOG_DOMAIN_DUP		"Duplicate Domain Name"
#define en_TZODDNS_SYS_LOG_DOMAIN_EXIST		"Domain Name has already been created for this domain name."
#define en_IVYDNS_SYS_LOG_COMPLETE			"Operation complete"	// PTR-TI-0113
#define en_DHCPCLT_SYS_LOG_DOMAIN_NAME		"Domain name"
#define en_DHCPCLT_SYS_LOG_RECV_OFFER		"Receive Offer from"
#define en_DHCPCLT_SYS_LOG_DISCOVER			"Send Discover"
#define en_DHCPCLT_SYS_LOG_REQUEST_IP		"Send Request, Request IP"
#define en_DHCPCLT_SYS_LOG_RELEASE			"Send Release"
#define en_DHCPCLT_SYS_LOG_DECLINE			"Send Decline"
#define en_DHCPCLT_SYS_LOG_GET_INFO_ERR		"Could not find DHCP daemon to get information"
#define en_DHCPCLT_SYS_LOG_RECV_ACK			"Receive Ack from"
#define en_DHCPCLT_SYS_LOG_LEASE_TIME		"Lease time"
#define en_DHCPD_SYS_LOG_SEND_OFFER			"sending OFFER to"
#define en_DHCPD_SYS_LOG_SEND_ACK			"sending ACK to"
#define en_DHCPD_SYS_LOG_RELEASE			"%02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define en_DHCPD_SYS_LOG_WLAN_RELEASE		"Wireless %02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define en_DHCPD_SYS_LOG_EXCLUDE_MAC		"Exclude MAC %02X-%02X-%02X-%02X-%02X-%02X"		// %02X.. are MAC address
#define en_BIGPOND_SYS_LOG_LOGIN_TERMIN		"BigPond: BigPond login process terminated."
#define en_BIGPOND_SYS_LOG_LOGIN_FAIL		"BigPond: Unable to start BigPond login process."
#define en_BIGPOND_SYS_LOG_LOGIN_SUCC		"BigPond: Start BigPond login process successfully."
#define en_SMTP_SYS_LOG_FAIL				"fail message"
#define en_SMTP_SYS_LOG_POP_CONNECT_ERR		"SMTP> Fail to connect pop server."
#define en_SMTP_SYS_LOG_SEND_OK				"SMTP> Succeed in sending alert mail."
#define en_SMTP_SYS_LOG_SEND_FAIL			"SMTP> Fail to send alert mail. Delay 10 seconds."
#define en_SMTP_MAIL_ALERT1					"Dear User\r\nYour router has detected and protected you against an attempt to gain access to your network.  This may have been an attempted hacker intrusion, or perhaps just your Internet Service Provider doing routine network maintenance.\r\nMost of these network probes are nothing to be worried about - these types of random probes should NOT be reported, but you may want to report repeated intrusions attempts.  Save this email for comparison with future alert messages.\r\nYour router Alert Information\r\n\r\n"
#define en_SMTP_MAIL_ALERT2					"\r\nVisit the UXN Combat Spam web site to get more detailed information about the intruder - http://combat.uxn.com/\r\n1. Type the intruder's IP address into the IP WHOIS search engine\r\n2. Click the Query Button\r\n3. Detailed network and administration information will be displayed\r\n\r\n"
#define en_NAT_CGI_SYS_LOG					"Public Port with error format"
#define en_W1X_SYS_LOG_LOGON				"802.1X supplicant %s logon"					// %s is MAC address string
#define en_W1X_SYS_LOG_REAUTH				"802.1X supplicant %s re-authenticated"			// %s is MAC address string
#define en_W1X_SYS_LOG_LOGOFF				"802.1X supplicant %s logoff"					// %s is MAC address string
#define en_W1X_SYS_LOG_TIMEOUT				"802.1X supplicant %s idle timeout"				// %s is MAC address string
#define en_W1X_SYS_LOG_DISCONNECT			"Disconnect 802.1X supplicant %s, cause:%d"		// %s is MAC address string, %d is error code
#define en_NTP_SYS_LOG_QUERY				"Begin to query NTP"
#define en_NTP_SYS_LOG_UPDATE				"NTP Date/Time updated."
#define en_NTP_SYS_LOG_CANNT_FIND			"Can't find NTP time."
#define en_NTP_SYS_LOG_SERVIP				"Get system time from NTP server:%s."
#define en_HTTP_SYS_LOG_USER_TIMEOUT		"User from %s timed out"						// %s is IP address
#define en_HTTP_SYS_LOG_DUP_USER			"Duplicate user login from %s"					// %s is IP address
#define en_HTTP_SYS_LOG_LOGIN_SUCCESS		"System: %s login successfully"
#define en_USB_SYS_LOG_NO_DEV				"[USB] No Device !! "
#define en_USB_SYS_LOG_SET_INPUT_CONNECT	"[USBLPT] Set Input Connection .. "
#define en_USB_SYS_LOG_SET_OUTPUT_ONLY		"[USBLPT] Set Output Only .. "
#define en_USB_SYS_LOG_OUTPUT_ONLY			"[USBLPT] This Printer is Output Only "
#define en_USB_SYS_LOG_PR_INIT_OK			"[USBLPT] Printer Initialize Ok. "
#define en_USB_SYS_LOG_PR_WR_FAIL			"[USBLPT] Error : Cannot write to printer "
#define en_USB_SYS_LOG_PR_WR_DATA_FAIL		"[USBLPT] Error : Print Data write Fail !! "
#define en_USB_SYS_LOG_PR_RETRY				"[USBLPT] Retry .. "
#define en_USB_SYS_LOG_PR_INIT_RESET_OK		"[USBLPT] Printer Initial Reset OK. "
#define en_USB_SYS_LOG_PR_INIT_RESET_FAIL	"[USBLPT] Error : Initial Cannot Reset Printer"
#define en_USB_SYS_LOG_PR_RESET_FAIL		"[USBLPT] Error : Cannot Reset Printer"
#define en_USB_SYS_LOG_PR_RESET_OK			"[USBLPT] Printer Reset OK. "
#define en_USB_SYS_LOG_PR_EPSON_INIT		"[USBLPT] Init Epson (or OEM) Printer .. "
#define en_USB_SYS_LOG_PR_RESER_DUMMY		"[USBLPT] Dummy Reset .. "
#define en_USB_SYS_LOG_PR_STATUS_ERR		"[USBLPT] Printer Status Error , Out of Ink / Paper End / Paper Jam ?? "
#define en_USB_SYS_LOG_PR_UNSUPP			"[USBLPT] Error : This Printer is Not Supported by Firmware !! "
#define en_USB_SYS_LOG_PR_NO_RESET			"[USBLPT] This Printer Has not Reset Command "
#define en_USB_SYS_LOG_PR_RESET_11			"[USBLPT] This Printer Only Accept Version 1.1 Reset Command "
#define en_USB_SYS_LOG_PR_FOUND				"[USB] Found Printer"
#define en_USB_SYS_LOG_PR_REMOVE			"[USBLPT] USB Printer Removed "
#define en_USB_SYS_LOG_PR_PROTOCOL			"[USBLPT] set USB LPT Protocol %d \n"		// %d is protocol number
#define en_USB_SYS_LOG_PR_ID_ERR			"[USBLPT] Error : reading IEEE-1284 Device ID string Device ID !! "
#define en_USB_SYS_LOG_PR_ID				"[USBLPT] Device ID"
#define en_USB_SYS_LOG_MAC_REMOVE			"[USBMAC] USB MAC Device Removed "
#define en_USB_SYS_LOG_MAC_NO_DEV			"[USBMAC] Not USB MAC device !! "
#define en_USB_SYS_LOG_MAC_FOUND			"[USB] Found MAC"
#define en_USB_SYS_LOG_DEV_NUM				"[USB] Total External Device = %d "				// %d is device number
#define en_USB_SYS_LOG_DEV_RESET_OK			"[USB] Reset Controller OK. "
#define en_USB_SYS_LOG_DEV_UNSUPP			"[USB] unsupported device"
#define en_USB_SYS_LOG_DEV_SPEED			"[USB] Device Speed"
#define en_USB_SYS_LOG_DEV_DISCONN			"[USB] disconnect on device %d "			// %d is device number
#define en_USB_SYS_LOG_DEV_REMOVE			"[USB] Device Removed !! "
#define en_USB_SYS_LOG_DEV_TYPE_CHECK		"[USB] Check Device Type .. "
#define en_USB_SYS_LOG_DEV_NEW				"[USB] new USB device , assigned address %d "// %d is device number
#define en_USB_SYS_LOG_HUB_PORT_RST			"[USB] HUB : (USB Device %d) Reset Port %d "// %d, %d are device number & reset port
#define en_USB_SYS_LOG_HUB_PORT_CHANGE		"[USB] HUB port %d enable change, status %x "// %d, %x are device number and status code
#define en_UBS_SYS_LOG_HUB_FOUND			"[USB] Found External HUB"
#define en_USB_SYS_LOG_MOUSE_FOUND			"[USB] Found Mouse"
#define en_USB_SYS_LOG_BULK_TIMEOUT			"[USB] Error : Bulk Communication Time Out !! "
#define en_USB_SYS_LOG_INTR_TIMEOUT			"[USB] Error : Interrupt Communication Time Out !! "
#define en_USB_SYS_LOG_REACT_TIMEOUT		"[USB] Error : raced timeout, status %d timeout %d "	// %d, %d are status code, timeout
#define en_USB_SYS_LOG_ERR_TIMEOUT			"[USB] Error : timeout !! "
#define en_USB_SYS_LOG_CTRL_TIMEOUT			"[USB] Error : Control Communication Time Out !! "
#define en_ADSL_SYS_LOG_MEDIA_UP			"ADSL Media Up (DS/US - %d/%d kbps.) !"
#define en_ADSL_SYS_LOG_MEDIA_DOWN			"ADSL Media Down !"
#define en_CBAC_SYS_LOG_REJECT				"Reject"
#define en_CBAC_SYS_LOG_RELAY				"Relay"
#define en_CBAC_SYS_LOG_FROM				"from"
#define en_CBAC_SYS_LOG_TO					"to"
#define en_CBAC_SYS_LOG_PACKET				"packet"
#define en_CBAC_SYS_LOG_INBOUND				"Inbound"
#define en_CBAC_SYS_LOG_OUTBOUND			"Outbound"
#define en_CBAC_SYS_LOG_MSG					"Message: %s%s\r\n"
#define en_CBAC_SYS_LOG_MSG_TIME			"Time: %s\r\nMessage: %s%s\r\n"
#define en_CBAC_SYS_LOG_MSG_SRC				"Message: %s\r\nSource: %s\r\nDestination:%s%s\r\n"
#define en_CBAC_SYS_LOG_MSG_SRC_TIME		"Time: %s\r\nMessage: %s\r\nSource: %s\r\nDestination:%s%s\r\n"
//#define en_VOIP_SYS_LOG_VC2_TO_VC1			"[VoIP] VC2 fail, Regist VoIP account %d to VC1 with delay %d sec."
#define en_VOIP_SYS_LOG_VC2_TO_VC1			"[VoIP] Register account %d via VC2 fail%s, register via VC1 with delay %d sec."
#define en_VOIP_SYS_LOG_VC1_TO_VC2			"[VoIP] Register VoIP account %d back to VC2"
#define en_VOIP_SYS_LOG_REG_OK_FROM_VC		"[VoIP] SIP account %d register success from VC%d"
#define en_VOIP_SYS_LOG_REG_FAIL			"[VoIP] SIP account %d register fail(%d)"
#define en_VOIP_SYS_LOG_REG_TIMEOUT			"[VoIP] SIP account %d register timeout"
#define en_FTPD_SYS_LOG_LOGIN_OK			"[FTPD] User %s login OK from %s"
#define en_FTPD_SYS_LOG_LOGIN_FAIL			"[FTPD] User %s login Fail from %s"
#define en_FTPD_SYS_LOG_LOGOUT				"[FTPD] User %s(%s) logout"
#define en_FTPD_SYS_LOG_PUT					"[FTPD] User %s(%s) put file:%s"
#define en_FTPD_SYS_LOG_GET					"[FTPD] User %s(%s) get file:%s"
#endif // _ENGLISH_SYS_LOG

#define PPP_SYS_LOG_START				    en_PPP_SYS_LOG_START				// "start PPP"
#define PPP_SYS_LOG_STOP					en_PPP_SYS_LOG_STOP					// "stop PPP"
#define PPP_SYS_LOG_ON_DEMAND			    en_PPP_SYS_LOG_ON_DEMAND			// "Dial On Demand"
#define PPP_SYS_LOG_PASS_OK				    en_PPP_SYS_LOG_PASS_OK				// "Username and Password: OK"
#define PPP_SYS_LOG_PASS_FAIL			    en_PPP_SYS_LOG_PASS_FAIL			// "PAP Username and Password: failed"
#define PPP_SYS_LOG_CHAP_PASS_FAIL		    en_PPP_SYS_LOG_CHAP_PASS_FAIL		// "CHAP Username and Password: failed"
#define PPP_SYS_LOG_PASS_TIMEOUT			en_PPP_SYS_LOG_PASS_TIMEOUT			// "PAP Authentication: timeout"
#define PPP_SYS_LOG_CHAP_TIMEOUT			en_PPP_SYS_LOG_CHAP_TIMEOUT			// "CHAP Authentication: timeout"
#define PPP_SYS_LOG_PEER_REJECT			    en_PPP_SYS_LOG_PEER_REJECT			// "PPP connection terminated by peer"
#define PPP_SYS_LOG_LCP_TIMEOUT			    en_PPP_SYS_LOG_LCP_TIMEOUT			// "PPP-LCP negotiation timeout"
#define PPP_SYS_LOG_MODEM_BROKE			    en_PPP_SYS_LOG_MODEM_BROKE			// "Modem connection broken"
#define PPP_SYS_LOG_MODEM_ERR			    en_PPP_SYS_LOG_MODEM_ERR			// "Modem is error"
#define PPP_SYS_LOG_MODEM_START			    en_PPP_SYS_LOG_MODEM_START			// "Modem start to dial up"
#define PPP_SYS_LOG_MODEM_STOP			    en_PPP_SYS_LOG_MODEM_STOP			// "Modem start to hang up"
#define PPP_SYS_LOG_MODEM_BUSY			    en_PPP_SYS_LOG_MODEM_BUSY			// "The phone line of dial-up is busy"
#define PPP_SYS_LOG_MODEM_NO_TONE		    en_PPP_SYS_LOG_MODEM_NO_TONE		// "The phone line of dial-up has no dial tone"
#define PPP_SYS_LOG_MODEM_BAUDRATE		    en_PPP_SYS_LOG_MODEM_BAUDRATE		// "Modem connection baud rate"
#define PPP_SYS_LOG_CONN_OK				    en_PPP_SYS_LOG_CONN_OK				// "If(%s) PPP connection ok !"	// %s is interface description
#define PPP_SYS_LOG_CONN_FAIL			    en_PPP_SYS_LOG_CONN_FAIL			// "If(%s) PPP fail : %s"	// %s, %s is interface description, error message
#define PPP_SYS_LOG_DIAL_TIMEOUT			en_PPP_SYS_LOG_DIAL_TIMEOUT			// "If(%s) PPP Dial timeout : %lu"	// %s, %lu is interface description, dialing timeout
#define PPPOE_SYS_LOG_PADI				    en_PPPOE_SYS_LOG_PADI				// "PPPoE send PADI"
#define PPPOE_SYS_LOG_PADR				    en_PPPOE_SYS_LOG_PADR				// "PPPoE send PADR"
#define PPPOE_SYS_LOG_PADO				    en_PPPOE_SYS_LOG_PADO				// "PPPoE receive PADO"
#define PPPOE_SYS_LOG_PADS				    en_PPPOE_SYS_LOG_PADS				// "PPPoE receive PADS"
#define PPPOE_SYS_LOG_PADT				    en_PPPOE_SYS_LOG_PADT				// "PPPoE receive PADT"
#define PPPOE_SYS_LOG_STOP				    en_PPPOE_SYS_LOG_STOP				// "PPPoE stop"
#define PPPOE_SYS_LOG_DNS1				    en_PPPOE_SYS_LOG_DNS1				// "%s get primary DNS IP:%u.%u.%u.%u"
#define PPPOE_SYS_LOG_DNS2				    en_PPPOE_SYS_LOG_DNS2				// "%s get secondary DNS IP:%u.%u.%u.%u"
#define PPPOE_SYS_LOG_RETRY_FAIL			en_PPPOE_SYS_LOG_RETRY_FAIL			// "PPPoE connect fail after %d times retry. Recover to default PPPoE profile."
#define PPPOE_SYS_LOG_ACS_TIMEOUT			en_PPPOE_SYS_LOG_ACS_TIMEOUT		// "TR69 connection timeout. Restore to original PPPoE profile."
#define PPTP_SYS_LOG_LOGIN_SUCC			    en_PPTP_SYS_LOG_LOGIN_SUCC			// "%s login Line %d of PPTP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress
#define PPTP_SYS_LOG_LOGIN_FAIL			    en_PPTP_SYS_LOG_LOGIN_FAIL			// "%s unable to log into PPTP Server from %s."				// %s, %s are username, ipaddress
#define PPTP_SYS_LOG_SERVER				    en_PPTP_SYS_LOG_SERVER				// "PPTP server"
#define PPTP_SYS_LOG_CLIENT				    en_PPTP_SYS_LOG_CLIENT				// "PPTP client"
#define PPTP_SYS_LOG_CONNECT				en_PPTP_SYS_LOG_CONNECT				// "%s: Line %d connected"
#define PPTP_SYS_LOG_GET_IP				    en_PPTP_SYS_LOG_GET_IP				// "%s get IP:%u.%u.%u.%u"
#define PPTP_SYS_LOG_DESTROY				en_PPTP_SYS_LOG_DESTROY				// "%s destroy tunnel successfully"// %s is peer IP
#define PPTP_SYS_LOG_CREATE				    en_PPTP_SYS_LOG_CREATE				// "%s creates tunnel successfully"// %s is peer IP
#define L2TP_SYS_LOG_LOGIN_SUCC			    en_L2TP_SYS_LOG_LOGIN_SUCC			// "%s login Line %d of L2TP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress,  // PTR-TI-0113
#define L2TP_SYS_LOG_LOGIN_FAIL			    en_L2TP_SYS_LOG_LOGIN_FAIL			// "%s unable to log into L2TP Server from %s."				// %s, %s are username, ipaddress
#define L2TP_SYS_LOG_SERVER				    en_L2TP_SYS_LOG_SERVER				// "L2TP server"
#define L2TP_SYS_LOG_CLIENT				    en_L2TP_SYS_LOG_CLIENT				// "L2TP client"
#define L2TP_SYS_LOG_CONNECT				en_L2TP_SYS_LOG_CONNECT				// "%s: Line %d connected"
#define L2TP_SYS_LOG_GET_IP				    en_L2TP_SYS_LOG_GET_IP				// "%s get IP:%u.%u.%u.%u"
#define L2TP_SYS_LOG_DESTROY				en_L2TP_SYS_LOG_DESTROY				// "%s destroy tunnel successfully"// %s is peer IP
#define L2TP_SYS_LOG_CREATE				    en_L2TP_SYS_LOG_CREATE				// "%s creates tunnel successfully"// %s is peer IP
#define DYNDNS_SYS_LOG_COMPLETE			    en_DYNDNS_SYS_LOG_COMPLETE			// "Operation complete"
#define DYNDNS_SYS_LOG_PASS_FAIL			en_DYNDNS_SYS_LOG_PASS_FAIL			// "Invalid Account(username or password)"
#define DYNDNS_SYS_LOG_SERVER_ERR		    en_DYNDNS_SYS_LOG_SERVER_ERR		// "An unexpected server error."
#define DYNDNS_SYS_LOG_DOMAIN_ERR		    en_DYNDNS_SYS_LOG_DOMAIN_ERR		// "Invalid Domain Name."
#define DYNDNS_SYS_LOG_DOMAIN_DUP		    en_DYNDNS_SYS_LOG_DOMAIN_DUP		// "Domain Name has already been created for this domain name."
#define DYNDNS_SYS_LOG_CHECK_UPDATE     	en_DYNDNS_SYS_LOG_CHECK_UPDATE     	// "Check and Update .."
#define DYNDNS_SYS_LOG_SESSION_DISABLE	    en_DYNDNS_SYS_LOG_SESSION_DISABLE	// "PPPoE Session is not Enabled."
#define DYNDNS_SYS_LOG_REGISTED			    en_DYNDNS_SYS_LOG_REGISTED			// "DDNS> This domain is currently registed by other IP:%s"
#define DYNDNS_SYS_LOG_DNS_ERR			    en_DYNDNS_SYS_LOG_DNS_ERR			// "DDNS> No response from DNS server. Request hostname:%s"
#define TZODDNS_SYS_LOG_EXPIRE			    en_TZODDNS_SYS_LOG_EXPIRE			// "The account has expired."
#define TZODDNS_SYS_LOG_KEY_ERR			    en_TZODDNS_SYS_LOG_KEY_ERR			// "Invalid Key"
#define TZODDNS_SYS_LOG_ERR				    en_TZODDNS_SYS_LOG_ERR				// "An unexpected error has occurred."
#define TZODDNS_SYS_LOG_SERVER_ERR		    en_TZODDNS_SYS_LOG_SERVER_ERR		// "An unexpected server error."
#define TZODDNS_SYS_LOG_COMPLETE			en_TZODDNS_SYS_LOG_COMPLETE			// "Operation complete"
#define TZODDNS_SYS_LOG_DOMAIN_ERR		    en_TZODDNS_SYS_LOG_DOMAIN_ERR		// "Invalid Domain Name"
#define TZODDNS_SYS_LOG_MAIL_ERR			en_TZODDNS_SYS_LOG_MAIL_ERR			// "Invalid Email"
#define TZODDNS_SYS_LOG_ACTION_ERR		    en_TZODDNS_SYS_LOG_ACTION_ERR		// "Invalid Action"
#define TZODDNS_SYS_LOG_IP_ERR			    en_TZODDNS_SYS_LOG_IP_ERR			// "Invalid IP address"
#define TZODDNS_SYS_LOG_DOMAIN_DUP		    en_TZODDNS_SYS_LOG_DOMAIN_DUP		// "Duplicate Domain Name"
#define TZODDNS_SYS_LOG_DOMAIN_EXIST		en_TZODDNS_SYS_LOG_DOMAIN_EXIST		// "Domain Name has already been created for this domain name."
#define IVYDNS_SYS_LOG_COMPLETE			    en_IVYDNS_SYS_LOG_COMPLETE			// "Operation complete"	// PTR-TI-0113
#define DHCPCLT_SYS_LOG_DOMAIN_NAME		    en_DHCPCLT_SYS_LOG_DOMAIN_NAME		// "Domain name"
#define DHCPCLT_SYS_LOG_RECV_OFFER		    en_DHCPCLT_SYS_LOG_RECV_OFFER		// "Receive Offer from"
#define DHCPCLT_SYS_LOG_DISCOVER			en_DHCPCLT_SYS_LOG_DISCOVER			// "Send Discover"
#define DHCPCLT_SYS_LOG_REQUEST_IP		    en_DHCPCLT_SYS_LOG_REQUEST_IP		// "Send Request, Request IP"
#define DHCPCLT_SYS_LOG_RELEASE			    en_DHCPCLT_SYS_LOG_RELEASE			// "Send Release"
#define DHCPCLT_SYS_LOG_DECLINE			    en_DHCPCLT_SYS_LOG_DECLINE			// "Send Decline"
#define DHCPCLT_SYS_LOG_GET_INFO_ERR		en_DHCPCLT_SYS_LOG_GET_INFO_ERR		// "Could not find DHCP daemon to get information"
#define DHCPCLT_SYS_LOG_RECV_ACK			en_DHCPCLT_SYS_LOG_RECV_ACK			// "Receive Ack from"
#define DHCPCLT_SYS_LOG_LEASE_TIME		    en_DHCPCLT_SYS_LOG_LEASE_TIME		// "Lease time"
#define DHCPD_SYS_LOG_SEND_OFFER			en_DHCPD_SYS_LOG_SEND_OFFER			// "sending OFFER to"
#define DHCPD_SYS_LOG_SEND_ACK			    en_DHCPD_SYS_LOG_SEND_ACK			// "sending ACK to"
#define DHCPD_SYS_LOG_RELEASE			    en_DHCPD_SYS_LOG_RELEASE			// "%02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define DHCPD_SYS_LOG_WLAN_RELEASE		    en_DHCPD_SYS_LOG_WLAN_RELEASE		// "%02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define DHCPD_SYS_LOG_EXCLUDE_MAC		    en_DHCPD_SYS_LOG_EXCLUDE_MAC		// "Exclude MAC %02X-%02X-%02X-%02X-%02X-%02X"		// %02X.. are MAC address
#define BIGPOND_SYS_LOG_LOGIN_TERMIN		en_BIGPOND_SYS_LOG_LOGIN_TERMIN		// "BigPond: BigPond login process terminated."
#define BIGPOND_SYS_LOG_LOGIN_FAIL		    en_BIGPOND_SYS_LOG_LOGIN_FAIL		// "BigPond: Unable to start BigPond login process."
#define BIGPOND_SYS_LOG_LOGIN_SUCC		    en_BIGPOND_SYS_LOG_LOGIN_SUCC		// "BigPond: Start BigPond login process successfully."
#define SMTP_SYS_LOG_FAIL				    en_SMTP_SYS_LOG_FAIL				// "fail message"
#define SMTP_SYS_LOG_POP_CONNECT_ERR		en_SMTP_SYS_LOG_POP_CONNECT_ERR		// "SMTP> Fail to connect pop server."
#define SMTP_SYS_LOG_SEND_OK				en_SMTP_SYS_LOG_SEND_OK				// "SMTP> Succeed in sending alert mail."
#define SMTP_SYS_LOG_SEND_FAIL			    en_SMTP_SYS_LOG_SEND_FAIL			// "SMTP> Fail to send alert mail. Delay 10 seconds."
#define SMTP_MAIL_ALERT1					en_SMTP_MAIL_ALERT1					// "Dear User\r\nYour router has detected and protected you against an attempt to gain access to your network.  This may have been an attempted hacker intrusion, or perhaps just your Internet Service Provider doing routine network maintenance.\r\nMost of these network probes are nothing to be worried about - these types of random probes should NOT be reported, but you may want to report repeated intrusions attempts.  Save this email for comparison with future alert messages.\r\nYour router Alert Information\r\n\r\n"
#define SMTP_MAIL_ALERT2					en_SMTP_MAIL_ALERT2					// "\r\nVisit the UXN Combat Spam web site to get more detailed information about the intruder - http://combat.uxn.com/\r\n1. Type the intruder's IP address into the IP WHOIS search engine\r\n2. Click the Query Button\r\n3. Detailed network and administration information will be displayed\r\n\r\n"
#define NAT_CGI_SYS_LOG					    en_NAT_CGI_SYS_LOG					// "Public Port with error format"
#define W1X_SYS_LOG_LOGON				    en_W1X_SYS_LOG_LOGON				// "802.1X supplicant %s logon"					// %s is MAC address string
#define W1X_SYS_LOG_REAUTH				    en_W1X_SYS_LOG_REAUTH				// "802.1X supplicant %s re-authenticated"			// %s is MAC address string
#define W1X_SYS_LOG_LOGOFF				    en_W1X_SYS_LOG_LOGOFF				// "802.1X supplicant %s logoff"					// %s is MAC address string
#define W1X_SYS_LOG_TIMEOUT				    en_W1X_SYS_LOG_TIMEOUT				// "802.1X supplicant %s idle timeout"				// %s is MAC address string
#define W1X_SYS_LOG_DISCONNECT			    en_W1X_SYS_LOG_DISCONNECT			// "Disconnect 802.1X supplicant %s, cause:%d"		// %s is MAC address string, %d is error code
#define NTP_SYS_LOG_QUERY				    en_NTP_SYS_LOG_QUERY				// "Begin to query NTP"
#define NTP_SYS_LOG_UPDATE				    en_NTP_SYS_LOG_UPDATE				// "NTP Date/Time updated."
#define NTP_SYS_LOG_CANNT_FIND			    en_NTP_SYS_LOG_CANNT_FIND			// "Can't find NTP time."
#define NTP_SYS_LOG_SERVIP				    en_NTP_SYS_LOG_SERVIP				// "Get system time from NTP server:%s."
#define HTTP_SYS_LOG_USER_TIMEOUT		    en_HTTP_SYS_LOG_USER_TIMEOUT		// "User from %s timed out"						// %s is IP address
#define HTTP_SYS_LOG_DUP_USER			    en_HTTP_SYS_LOG_DUP_USER			// "Duplicate user login from %s"					// %s is IP address
#define HTTP_SYS_LOG_LOGIN_SUCCESS		    en_HTTP_SYS_LOG_LOGIN_SUCCESS		// "System: %s login successfully"
#define USB_SYS_LOG_NO_DEV				    en_USB_SYS_LOG_NO_DEV				// "[USB] No Device !! "
#define USB_SYS_LOG_SET_INPUT_CONNECT	    en_USB_SYS_LOG_SET_INPUT_CONNECT	// "[USBLPT] Set Input Connection .. "
#define USB_SYS_LOG_SET_OUTPUT_ONLY		    en_USB_SYS_LOG_SET_OUTPUT_ONLY		// "[USBLPT] Set Output Only .. "
#define USB_SYS_LOG_OUTPUT_ONLY			    en_USB_SYS_LOG_OUTPUT_ONLY			// "[USBLPT] This Printer is Output Only "
#define USB_SYS_LOG_PR_INIT_OK			    en_USB_SYS_LOG_PR_INIT_OK			// "[USBLPT] Printer Initialize Ok. "
#define USB_SYS_LOG_PR_WR_FAIL			    en_USB_SYS_LOG_PR_WR_FAIL			// "[USBLPT] Error : Cannot write to printer "
#define USB_SYS_LOG_PR_WR_DATA_FAIL		    en_USB_SYS_LOG_PR_WR_DATA_FAIL		// "[USBLPT] Error : Print Data write Fail !! "
#define USB_SYS_LOG_PR_RETRY				en_USB_SYS_LOG_PR_RETRY				// "[USBLPT] Retry .. "
#define USB_SYS_LOG_PR_INIT_RESET_OK		en_USB_SYS_LOG_PR_INIT_RESET_OK		// "[USBLPT] Printer Initial Reset OK. "
#define USB_SYS_LOG_PR_INIT_RESET_FAIL	    en_USB_SYS_LOG_PR_INIT_RESET_FAIL	// "[USBLPT] Error : Initial Cannot Reset Printer"
#define USB_SYS_LOG_PR_RESET_FAIL		    en_USB_SYS_LOG_PR_RESET_FAIL		// "[USBLPT] Error : Cannot Reset Printer"
#define USB_SYS_LOG_PR_RESET_OK			    en_USB_SYS_LOG_PR_RESET_OK			// "[USBLPT] Printer Reset OK. "
#define USB_SYS_LOG_PR_EPSON_INIT		    en_USB_SYS_LOG_PR_EPSON_INIT		// "[USBLPT] Init Epson (or OEM) Printer .. "
#define USB_SYS_LOG_PR_RESER_DUMMY		    en_USB_SYS_LOG_PR_RESER_DUMMY		// "[USBLPT] Dummy Reset .. "
#define USB_SYS_LOG_PR_STATUS_ERR		    en_USB_SYS_LOG_PR_STATUS_ERR		// "[USBLPT] Printer Status Error , Out of Ink / Paper End / Paper Jam ?? "
#define USB_SYS_LOG_PR_UNSUPP			    en_USB_SYS_LOG_PR_UNSUPP			// "[USBLPT] Error : This Printer is Not Supported by Firmware !! "
#define USB_SYS_LOG_PR_NO_RESET			    en_USB_SYS_LOG_PR_NO_RESET			// "[USBLPT] This Printer Has not Reset Command "
#define USB_SYS_LOG_PR_RESET_11			    en_USB_SYS_LOG_PR_RESET_11			// "[USBLPT] This Printer Only Accept Version 1.1 Reset Command "
#define USB_SYS_LOG_PR_FOUND				en_USB_SYS_LOG_PR_FOUND				// "[USB] Found Printer"
#define USB_SYS_LOG_PR_REMOVE			    en_USB_SYS_LOG_PR_REMOVE			// "[USBLPT] USB Printer Removed "
#define USB_SYS_LOG_PR_PROTOCOL			    en_USB_SYS_LOG_PR_PROTOCOL			// "[USBLPT] set USB LPT Protocol %d \n"		// %d is protocol number
#define USB_SYS_LOG_PR_ID_ERR			    en_USB_SYS_LOG_PR_ID_ERR			// "[USBLPT] Error : reading IEEE-1284 Device ID string Device ID !! "
#define USB_SYS_LOG_PR_ID				    en_USB_SYS_LOG_PR_ID				// "[USBLPT] Device ID"
#define USB_SYS_LOG_MAC_REMOVE			    en_USB_SYS_LOG_MAC_REMOVE			// "[USBMAC] USB MAC Device Removed "
#define USB_SYS_LOG_MAC_NO_DEV			    en_USB_SYS_LOG_MAC_NO_DEV			// "[USBMAC] Not USB MAC device !! "
#define USB_SYS_LOG_MAC_FOUND			    en_USB_SYS_LOG_MAC_FOUND			// "[USB] Found MAC"
#define USB_SYS_LOG_DEV_NUM				    en_USB_SYS_LOG_DEV_NUM				// "[USB] Total External Device = %d "				// %d is device number
#define USB_SYS_LOG_DEV_RESET_OK			en_USB_SYS_LOG_DEV_RESET_OK			// "[USB] Reset Controller OK. "
#define USB_SYS_LOG_DEV_UNSUPP			    en_USB_SYS_LOG_DEV_UNSUPP			// "[USB] unsupported device"
#define USB_SYS_LOG_DEV_SPEED			    en_USB_SYS_LOG_DEV_SPEED			// "[USB] Device Speed"
#define USB_SYS_LOG_DEV_DISCONN			    en_USB_SYS_LOG_DEV_DISCONN			// "[USB] disconnect on device %d "			// %d is device number
#define USB_SYS_LOG_DEV_REMOVE			    en_USB_SYS_LOG_DEV_REMOVE			// "[USB] Device Removed !! "
#define USB_SYS_LOG_DEV_TYPE_CHECK		    en_USB_SYS_LOG_DEV_TYPE_CHECK		// "[USB] Check Device Type .. "
#define USB_SYS_LOG_DEV_NEW				    en_USB_SYS_LOG_DEV_NEW				// "[USB] new USB device , assigned address %d "// %d is device number
#define USB_SYS_LOG_HUB_PORT_RST			en_USB_SYS_LOG_HUB_PORT_RST			// "[USB] HUB : (USB Device %d) Reset Port %d "// %d, %d are device number & reset port
#define USB_SYS_LOG_HUB_PORT_CHANGE		    en_USB_SYS_LOG_HUB_PORT_CHANGE		// "[USB] HUB port %d enable change, status %x "// %d, %x are device number and status code
#define UBS_SYS_LOG_HUB_FOUND			    en_UBS_SYS_LOG_HUB_FOUND			// "[USB] Found External HUB"
#define USB_SYS_LOG_MOUSE_FOUND			    en_USB_SYS_LOG_MOUSE_FOUND			// "[USB] Found Mouse"
#define USB_SYS_LOG_BULK_TIMEOUT			en_USB_SYS_LOG_BULK_TIMEOUT			// "[USB] Error : Bulk Communication Time Out !! "
#define USB_SYS_LOG_INTR_TIMEOUT			en_USB_SYS_LOG_INTR_TIMEOUT			// "[USB] Error : Interrupt Communication Time Out !! "
#define USB_SYS_LOG_REACT_TIMEOUT		    en_USB_SYS_LOG_REACT_TIMEOUT		// "[USB] Error : raced timeout, status %d timeout %d "	// %d, %d are status code, timeout
#define USB_SYS_LOG_ERR_TIMEOUT			    en_USB_SYS_LOG_ERR_TIMEOUT			// "[USB] Error : timeout !! "
#define USB_SYS_LOG_CTRL_TIMEOUT			en_USB_SYS_LOG_CTRL_TIMEOUT			// "[USB] Error : Control Communication Time Out !! "
#define ADSL_SYS_LOG_MEDIA_UP			    en_ADSL_SYS_LOG_MEDIA_UP			// "ADSL Media Up (DS/US - %d/%d kbps.) !"
#define ADSL_SYS_LOG_MEDIA_DOWN			    en_ADSL_SYS_LOG_MEDIA_DOWN			// "ADSL Media Down !"
#define CBAC_SYS_LOG_REJECT				    en_CBAC_SYS_LOG_REJECT				// "Reject"
#define CBAC_SYS_LOG_RELAY				    en_CBAC_SYS_LOG_RELAY				// "Relay"
#define CBAC_SYS_LOG_FROM				    en_CBAC_SYS_LOG_FROM				// "from"
#define CBAC_SYS_LOG_TO					    en_CBAC_SYS_LOG_TO					// "to"
#define CBAC_SYS_LOG_PACKET				    en_CBAC_SYS_LOG_PACKET				// "packet"
#define CBAC_SYS_LOG_INBOUND				en_CBAC_SYS_LOG_INBOUND				// "Inbound"
#define CBAC_SYS_LOG_OUTBOUND			    en_CBAC_SYS_LOG_OUTBOUND			// "Outbound"
#define CBAC_SYS_LOG_MSG					en_CBAC_SYS_LOG_MSG					// "Message: %s%s\r\n"
#define CBAC_SYS_LOG_MSG_TIME			    en_CBAC_SYS_LOG_MSG_TIME			// "Time: %s\r\nMessage: %s%s\r\n"
#define CBAC_SYS_LOG_MSG_SRC				en_CBAC_SYS_LOG_MSG_SRC				// "Message: %s\r\nSource: %s\r\nDestination:%s%s\r\n"
#define CBAC_SYS_LOG_MSG_SRC_TIME		    en_CBAC_SYS_LOG_MSG_SRC_TIME		// "Time: %s\r\nMessage: %s\r\nSource: %s\r\nDestination:%s%s\r\n"
#define VOIP_SYS_LOG_VC2_TO_VC1			    en_VOIP_SYS_LOG_VC2_TO_VC1			// "[VoIP] VC2 fail, Regist VoIP account %d to VC1 with delay %d sec."
#define VOIP_SYS_LOG_VC1_TO_VC2			    en_VOIP_SYS_LOG_VC1_TO_VC2			// "[VoIP] Regist VoIP account %d back to VC2"
#define VOIP_SYS_LOG_REG_OK_FROM_VC		    en_VOIP_SYS_LOG_REG_OK_FROM_VC		// "[VoIP] SIP account %d regist success from VC%d"
#define VOIP_SYS_LOG_REG_FAIL		    	en_VOIP_SYS_LOG_REG_FAIL			// "[VoIP] SIP account %d regist fail"
#define VOIP_SYS_LOG_REG_TIMEOUT		   	en_VOIP_SYS_LOG_REG_TIMEOUT			// "[VoIP] SIP account %d regist timeout"
#define FTPD_SYS_LOG_LOGIN_OK				en_FTPD_SYS_LOG_LOGIN_OK			// "[FTPD] User %s login OK from %s"
#define FTPD_SYS_LOG_LOGIN_FAIL				en_FTPD_SYS_LOG_LOGIN_FAIL			// "[FTPD] User %s login Fail from %s"
#define FTPD_SYS_LOG_LOGOUT					en_FTPD_SYS_LOG_LOGOUT				// "[FTPD] User %s(%s) logout"
#define FTPD_SYS_LOG_PUT					en_FTPD_SYS_LOG_PUT					// "[FTPD] User %s(%s) put file:%s"
#define FTPD_SYS_LOG_GET					en_FTPD_SYS_LOG_GET					// "[FTPD] User %s(%s) get file:%s"


// syslog message type define
#define num_PPP_SYS_LOG_START				10001		// "start PPP"
#define num_PPP_SYS_LOG_STOP				10002		// "stop PPP"
#define num_PPP_SYS_LOG_ON_DEMAND			10003		// "Dial On Demand"
#define num_PPP_SYS_LOG_PASS_OK				10004		// "Username and Password: OK"
#define num_PPP_SYS_LOG_PASS_FAIL			10005		// "PAP Username and Password: failed"
#define num_PPP_SYS_LOG_CHAP_PASS_FAIL		10006		// "CHAP Username and Password: failed"
#define num_PPP_SYS_LOG_PASS_TIMEOUT		10007		// "PAP Authentication: timeout"
#define num_PPP_SYS_LOG_CHAP_TIMEOUT		10008		// "CHAP Authentication: timeout"
#define num_PPP_SYS_LOG_PEER_REJECT			10009		// "PPP connection terminated by peer"
#define num_PPP_SYS_LOG_LCP_TIMEOUT			10010		// "PPP-LCP negotiation timeout"
#define num_PPP_SYS_LOG_MODEM_BROKE			10011		// "Modem connection broken"
#define num_PPP_SYS_LOG_MODEM_ERR			10012		// "Modem is error"
#define num_PPP_SYS_LOG_MODEM_START			10013		// "Modem start to dial up"
#define num_PPP_SYS_LOG_MODEM_STOP			10014		// "Modem start to hang up"
#define num_PPP_SYS_LOG_MODEM_BUSY			10015		// "The phone line of dial-up is busy"
#define num_PPP_SYS_LOG_MODEM_NO_TONE		10016		// "The phone line of dial-up has no dial tone"
#define num_PPP_SYS_LOG_MODEM_BAUDRATE		10017		// "Modem connection baud rate"
#define num_PPP_SYS_LOG_CONN_OK				10018		// "If(%s) PPP connection ok !"	// %s is interface description
#define num_PPP_SYS_LOG_CONN_FAIL			10019		// "If(%s) PPP fail : %s"	// %s, %s is interface description, error message
#define num_PPP_SYS_LOG_DIAL_TIMEOUT		10020		// "If(%s) PPP Dial timeout : %lu"	// %s, %lu is interface description, dialing timeout

#define num_PPPOE_SYS_LOG_PADI				11001		// "PPPoE send PADI"
#define num_PPPOE_SYS_LOG_PADR				11002		// "PPPoE send PADR"
#define num_PPPOE_SYS_LOG_PADO				11003		// "PPPoE receive PADO"
#define num_PPPOE_SYS_LOG_PADS				11004		// "PPPoE receive PADS"
#define num_PPPOE_SYS_LOG_PADT				11005		// "PPPoE receive PADT"
#define num_PPPOE_SYS_LOG_STOP				11006		// "PPPoE stop"
#define num_PPPOE_SYS_LOG_DNS1				11007		// "%s get primary DNS IP:%u.%u.%u.%u"
#define num_PPPOE_SYS_LOG_DNS2				11008		// "%s get secondary DNS IP:%u.%u.%u.%u"
#define num_PPPOE_SYS_LOG_RETRY_FAIL		11009		// "PPPoE connect fail after %d times retry. Recover to default PPPoE profile."
#define num_PPPOE_SYS_LOG_ACS_TIMEOUT		11010		// "TR69 connection timeout. Restore to original PPPoE profile."

#define num_PPTP_SYS_LOG_LOGIN_SUCC			12001		// "%s login Line %d of PPTP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress
#define num_PPTP_SYS_LOG_LOGIN_FAIL			12002		// "%s unable to log into PPTP Server from %s."				// %s, %s are username, ipaddress
#define num_PPTP_SYS_LOG_SERVER				12003		// "PPTP server"
#define num_PPTP_SYS_LOG_CLIENT				12004		// "PPTP client"
#define num_PPTP_SYS_LOG_CONNECT			12005		// "%s: Line %d connected"
#define num_PPTP_SYS_LOG_GET_IP				12006		// "%s get IP:%u.%u.%u.%u"
#define num_PPTP_SYS_LOG_DESTROY			12007		// "%s destroy tunnel successfully"// %s is peer IP
#define num_PPTP_SYS_LOG_CREATE				12008		// "%s creates tunnel successfully"// %s is peer IP

#define num_L2TP_SYS_LOG_LOGIN_SUCC			13001		// "%s login Line %d of L2TP Server from %s successfully"		// %s, %d, %s are username, line number, ipaddress,  // PTR-TI-0113
#define num_L2TP_SYS_LOG_LOGIN_FAIL			13002		// "%s unable to log into L2TP Server from %s."				// %s, %s are username, ipaddress
#define num_L2TP_SYS_LOG_SERVER				13003		// "L2TP server"
#define num_L2TP_SYS_LOG_CLIENT				13004		// "L2TP client"
#define num_L2TP_SYS_LOG_CONNECT			13005		// "%s: Line %d connected"
#define num_L2TP_SYS_LOG_GET_IP				13006		// "%s get IP:%u.%u.%u.%u"
#define num_L2TP_SYS_LOG_DESTROY			13007		// "%s destroy tunnel successfully"// %s is peer IP
#define num_L2TP_SYS_LOG_CREATE				13008		// "%s creates tunnel successfully"// %s is peer IP

#define num_DYNDNS_SYS_LOG_COMPLETE			14001		// "Operation complete"
#define num_DYNDNS_SYS_LOG_PASS_FAIL		14002		// "Invalid Account(username or password)"
#define num_DYNDNS_SYS_LOG_SERVER_ERR		14003		// "An unexpected server error."
#define num_DYNDNS_SYS_LOG_DOMAIN_ERR		14004		// "Invalid Domain Name."
#define num_DYNDNS_SYS_LOG_DOMAIN_DUP		14005		// "Domain Name has already been created for this domain name."
#define num_DYNDNS_SYS_LOG_CHECK_UPDATE    	14006		// "Check and Update .."
#define num_DYNDNS_SYS_LOG_SESSION_DISABLE	14007		// "PPPoE Session is not Enabled."
#define num_DYNDNS_SYS_LOG_REGISTED			14008		// "DDNS> This domain is currently registed by other IP:%s"
#define num_DYNDNS_SYS_LOG_DNS_ERR			14009		// "DDNS> No response from DNS server. Request hostname:%s"

#define num_TZODDNS_SYS_LOG_EXPIRE			15001		// "The account has expired."
#define num_TZODDNS_SYS_LOG_KEY_ERR			15002		// "Invalid Key"
#define num_TZODDNS_SYS_LOG_ERR				15003		// "An unexpected error has occurred."
#define num_TZODDNS_SYS_LOG_SERVER_ERR		15004		// "An unexpected server error."
#define num_TZODDNS_SYS_LOG_COMPLETE		15005		// "Operation complete"
#define num_TZODDNS_SYS_LOG_DOMAIN_ERR		15006		// "Invalid Domain Name"
#define num_TZODDNS_SYS_LOG_MAIL_ERR		15007		// "Invalid Email"
#define num_TZODDNS_SYS_LOG_ACTION_ERR		15008		// "Invalid Action"
#define num_TZODDNS_SYS_LOG_IP_ERR			15009		// "Invalid IP address"
#define num_TZODDNS_SYS_LOG_DOMAIN_DUP		15010		// "Duplicate Domain Name"
#define num_TZODDNS_SYS_LOG_DOMAIN_EXIST	15011		// "Domain Name has already been created for this domain name."

#define num_IVYDNS_SYS_LOG_COMPLETE			16001		// "Operation complete"	// PTR-TI-0113

#define num_DHCPCLT_SYS_LOG_DOMAIN_NAME		17001		// "Domain name"
#define num_DHCPCLT_SYS_LOG_RECV_OFFER		17002		// "Receive Offer from"
#define num_DHCPCLT_SYS_LOG_DISCOVER		17003		// "Send Discover"
#define num_DHCPCLT_SYS_LOG_REQUEST_IP		17004		// "Send Request, Request IP"
#define num_DHCPCLT_SYS_LOG_RELEASE			17005		// "Send Release"
#define num_DHCPCLT_SYS_LOG_DECLINE			17006		// "Send Decline"
#define num_DHCPCLT_SYS_LOG_GET_INFO_ERR	17007		// "Could not find DHCP daemon to get information"
#define num_DHCPCLT_SYS_LOG_RECV_ACK		17008		// "Receive Ack from"
#define num_DHCPCLT_SYS_LOG_LEASE_TIME		17009		// "Lease time"

#define num_DHCPD_SYS_LOG_SEND_OFFER		18001		// "sending OFFER to"
#define num_DHCPD_SYS_LOG_SEND_ACK			18002		// "sending ACK to"
#define num_DHCPD_SYS_LOG_RELEASE			18003		// "%02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define num_DHCPD_SYS_LOG_WLAN_RELEASE		18004		// "Wireless %02X:%02X:%02X:%02X:%02X:%02X released"		// %02X.. are MAC address
#define num_DHCPD_SYS_LOG_EXCLUDE_MAC		18005		// "Exclude MAC %02X-%02X-%02X-%02X-%02X-%02X"		// %02X.. are MAC address

#define num_BIGPOND_SYS_LOG_LOGIN_TERMIN	19001		// "BigPond: BigPond login process terminated."
#define num_BIGPOND_SYS_LOG_LOGIN_FAIL		19002		// "BigPond: Unable to start BigPond login process."
#define num_BIGPOND_SYS_LOG_LOGIN_SUCC		19003		// "BigPond: Start BigPond login process successfully."

#define num_SMTP_SYS_LOG_FAIL				20001		// "fail message"
#define num_SMTP_SYS_LOG_POP_CONNECT_ERR	20002		// "SMTP> Fail to connect pop server."
#define num_SMTP_SYS_LOG_SEND_OK			20003		// "SMTP> Succeed in sending alert mail."
#define num_SMTP_SYS_LOG_SEND_FAIL			20004		// "SMTP> Fail to send alert mail. Delay 10 seconds."
#define num_SMTP_MAIL_ALERT1				20005		// "Dear User\r\nYour router has detected and protected you against an attempt to gain access to your network.  This may have been an attempted hacker intrusion, or perhaps just your Internet Service Provider doing routine network maintenance.\r\nMost of these network probes are nothing to be worried about - these types of random probes should NOT be reported, but you may want to report repeated intrusions attempts.  Save this email for comparison with future alert messages.\r\nYour router Alert Information\r\n\r\n"
#define num_SMTP_MAIL_ALERT2				20006		// "\r\nVisit the UXN Combat Spam web site to get more detailed information about the intruder - http://combat.uxn.com/\r\n1. Type the intruder's IP address into the IP WHOIS search engine\r\n2. Click the Query Button\r\n3. Detailed network and administration information will be displayed\r\n\r\n"

#define num_NAT_CGI_SYS_LOG					21001		// "Public Port with error format"

#define num_W1X_SYS_LOG_LOGON				22001		// "802.1X supplicant %s logon"					// %s is MAC address string
#define num_W1X_SYS_LOG_REAUTH				22002		// "802.1X supplicant %s re-authenticated"			// %s is MAC address string
#define num_W1X_SYS_LOG_LOGOFF				22003		// "802.1X supplicant %s logoff"					// %s is MAC address string
#define num_W1X_SYS_LOG_TIMEOUT				22004		// "802.1X supplicant %s idle timeout"				// %s is MAC address string
#define num_W1X_SYS_LOG_DISCONNECT			22005		// "Disconnect 802.1X supplicant %s, cause:%d"		// %s is MAC address string, %d is error code

#define num_NTP_SYS_LOG_QUERY				23001		// "Begin to query NTP"
#define num_NTP_SYS_LOG_UPDATE				23002		// "NTP Date/Time updated."
#define num_NTP_SYS_LOG_CANNT_FIND			23003		// "Can't find NTP time."
#define num_NTP_SYS_LOG_SERVIP				23004		// "Get system time from NTP server:%s."

#define num_HTTP_SYS_LOG_USER_TIMEOUT		24001		// "User from %s timed out"						// %s is IP address
#define num_HTTP_SYS_LOG_DUP_USER			24002		// "Duplicate user login from %s"					// %s is IP address
#define num_HTTP_SYS_LOG_LOGIN_SUCCESS		24003		// "System: %s login successfully"

#define num_USB_SYS_LOG_NO_DEV				25001		// "[USB] No Device !! "
#define num_USB_SYS_LOG_SET_INPUT_CONNECT	25002		// "[USBLPT] Set Input Connection .. "
#define num_USB_SYS_LOG_SET_OUTPUT_ONLY		25003		// "[USBLPT] Set Output Only .. "
#define num_USB_SYS_LOG_OUTPUT_ONLY			25004		// "[USBLPT] This Printer is Output Only "
#define num_USB_SYS_LOG_PR_INIT_OK			25005		// "[USBLPT] Printer Initialize Ok. "
#define num_USB_SYS_LOG_PR_WR_FAIL			25006		// "[USBLPT] Error : Cannot write to printer "
#define num_USB_SYS_LOG_PR_WR_DATA_FAIL		25007		// "[USBLPT] Error : Print Data write Fail !! "
#define num_USB_SYS_LOG_PR_RETRY			25008		// "[USBLPT] Retry .. "
#define num_USB_SYS_LOG_PR_INIT_RESET_OK	25009		// "[USBLPT] Printer Initial Reset OK. "
#define num_USB_SYS_LOG_PR_INIT_RESET_FAIL	25010		// "[USBLPT] Error : Initial Cannot Reset Printer"
#define num_USB_SYS_LOG_PR_RESET_FAIL		25011		// "[USBLPT] Error : Cannot Reset Printer"
#define num_USB_SYS_LOG_PR_RESET_OK			25012		// "[USBLPT] Printer Reset OK. "
#define num_USB_SYS_LOG_PR_EPSON_INIT		25013		// "[USBLPT] Init Epson (or OEM) Printer .. "
#define num_USB_SYS_LOG_PR_RESER_DUMMY		25014		// "[USBLPT] Dummy Reset .. "
#define num_USB_SYS_LOG_PR_STATUS_ERR		25015		// "[USBLPT] Printer Status Error , Out of Ink / Paper End / Paper Jam ?? "
#define num_USB_SYS_LOG_PR_UNSUPP			25016		// "[USBLPT] Error : This Printer is Not Supported by Firmware !! "
#define num_USB_SYS_LOG_PR_NO_RESET			25017		// "[USBLPT] This Printer Has not Reset Command "
#define num_USB_SYS_LOG_PR_RESET_11			25018		// "[USBLPT] This Printer Only Accept Version 1.1 Reset Command "
#define num_USB_SYS_LOG_PR_FOUND			25019		// "[USB] Found Printer"
#define num_USB_SYS_LOG_PR_REMOVE			25020		// "[USBLPT] USB Printer Removed "
#define num_USB_SYS_LOG_PR_PROTOCOL			25021		// "[USBLPT] set USB LPT Protocol %d \n"		// %d is protocol number
#define num_USB_SYS_LOG_PR_ID_ERR			25022		// "[USBLPT] Error : reading IEEE-1284 Device ID string Device ID !! "
#define num_USB_SYS_LOG_PR_ID				25023		// "[USBLPT] Device ID"
#define num_USB_SYS_LOG_MAC_REMOVE			25024		// "[USBMAC] USB MAC Device Removed "
#define num_USB_SYS_LOG_MAC_NO_DEV			25025		// "[USBMAC] Not USB MAC device !! "
#define num_USB_SYS_LOG_MAC_FOUND			25026		// "[USB] Found MAC"
#define num_USB_SYS_LOG_DEV_NUM				25027		// "[USB] Total External Device = %d "				// %d is device number
#define num_USB_SYS_LOG_DEV_RESET_OK		25028		// "[USB] Reset Controller OK. "
#define num_USB_SYS_LOG_DEV_UNSUPP			25029		// "[USB] unsupported device"
#define num_USB_SYS_LOG_DEV_SPEED			25030		// "[USB] Device Speed"
#define num_USB_SYS_LOG_DEV_DISCONN			25031		// "[USB] disconnect on device %d "			// %d is device number
#define num_USB_SYS_LOG_DEV_REMOVE			25032		// "[USB] Device Removed !! "
#define num_USB_SYS_LOG_DEV_TYPE_CHECK		25033		// "[USB] Check Device Type .. "
#define num_USB_SYS_LOG_DEV_NEW				25034		// "[USB] new USB device , assigned address %d "// %d is device number
#define num_USB_SYS_LOG_HUB_PORT_RST		25035		// "[USB] HUB : (USB Device %d) Reset Port %d "// %d, %d are device number & reset port
#define num_USB_SYS_LOG_HUB_PORT_CHANGE		25036		// "[USB] HUB port %d enable change, status %x "// %d, %x are device number and status code
#define num_UBS_SYS_LOG_HUB_FOUND			25037		// "[USB] Found External HUB"
#define num_USB_SYS_LOG_MOUSE_FOUND			25038		// "[USB] Found Mouse"
#define num_USB_SYS_LOG_BULK_TIMEOUT		25039		// "[USB] Error : Bulk Communication Time Out !! "
#define num_USB_SYS_LOG_INTR_TIMEOUT		25040		// "[USB] Error : Interrupt Communication Time Out !! "
#define num_USB_SYS_LOG_REACT_TIMEOUT		25041		// "[USB] Error : raced timeout, status %d timeout %d "	// %d, %d are status code, timeout
#define num_USB_SYS_LOG_ERR_TIMEOUT			25042		// "[USB] Error : timeout !! "
#define num_USB_SYS_LOG_CTRL_TIMEOUT		25043		// "[USB] Error : Control Communication Time Out !! "

#define num_ADSL_SYS_LOG_MEDIA_UP			26001		// "ADSL Media Up (DS/US - %d/%d kbps.) !"
#define num_ADSL_SYS_LOG_MEDIA_DOWN			26002		// "ADSL Media Down !"

#define num_CBAC_SYS_LOG_REJECT				27001		// "Reject"
#define num_CBAC_SYS_LOG_RELAY				27002		// "Relay"
#define num_CBAC_SYS_LOG_FROM				27003		// "from"
#define num_CBAC_SYS_LOG_TO					27004		// "to"
#define num_CBAC_SYS_LOG_PACKET				27005		// "packet"
#define num_CBAC_SYS_LOG_INBOUND			27006		// "Inbound"
#define num_CBAC_SYS_LOG_OUTBOUND			27007		// "Outbound"
#define num_CBAC_SYS_LOG_MSG				27008		// "Message: %s%s\r\n"
#define num_CBAC_SYS_LOG_MSG_TIME			27009		// "Time: %s\r\nMessage: %s%s\r\n"
#define num_CBAC_SYS_LOG_MSG_SRC			27010		// "Message: %s\r\nSource: %s\r\nDestination:%s%s\r\n"
#define num_CBAC_SYS_LOG_MSG_SRC_TIME		27011		// "Time: %s\r\nMessage: %s\r\nSource: %s\r\nDestination:%s%s\r\n"

#define num_VOIP_SYS_LOG_VC2_TO_VC1			28001		// "[VoIP] VC2 fail, Regist VoIP account %d to VC1 with delay %d sec."
#define num_VOIP_SYS_LOG_VC1_TO_VC2			28002		// "[VoIP] Regist VoIP account %d back to VC2"
#define num_VOIP_SYS_LOG_REG_OK_FROM_VC		28003		// "[VoIP] SIP account %d regist success from VC%d"
#define num_VOIP_SYS_LOG_REG_FAIL			28004		// "[VoIP] SIP account %d regist fail"
#define num_VOIP_SYS_LOG_REG_TIMEOUT		28005		// "[VoIP] SIP account %d regist timeout"

#define num_FTPD_SYS_LOG_LOGIN_OK			29001		// "[FTPD] User %s login OK from %s"
#define num_FTPD_SYS_LOG_LOGIN_FAIL			29002		// "[FTPD] User %s login Fail from %s"
#define num_FTPD_SYS_LOG_LOGOUT				29003		// "[FTPD] User %s(%s) logout"
#define num_FTPD_SYS_LOG_PUT				29004		// "[FTPD] User %s(%s) put file:%s"
#define num_FTPD_SYS_LOG_GET				29005		// "[FTPD] User %s(%s) get file:%s"

#endif // _PPP_SYS_LOG_H
