#ifndef _ARCADYAN_SYSLOG_DEFINE_FOR_OPENSOURCE_H_
#define _ARCADYAN_SYSLOG_DEFINE_FOR_OPENSOURCE_H_

// define syslog type
#define LOG_TYPE_OTHER				0				// other type
#define LOG_TYPE_AP_BIGPOND			1				// big pong service
#define LOG_TYPE_AP_DDNS_TZO		2				// DDNS-TZO service
#define LOG_TYPE_AP_DDNS_DYNDNS		3				// DDNS-DYNDNS service
#define LOG_TYPE_AP_DDNS_DHIS		4				// DDNS-DHIS service
#define LOG_TYPE_AP_DDNS_SMC		5				// DDNS-TZO for SMC service
#define LOG_TYPE_AP_HTTPD			6				// http daemon
#define LOG_TYPE_AP_IGD1			7				// IGD 1.0 service, original version
#define LOG_TYPE_AP_IGD1_A			8				// IGD 1.0 service, accton version
#define LOG_TYPE_AP_LPD				9				// LPD service
#define LOG_TYPE_AP_DHCPD			10				// DHCP daemon service
#define LOG_TYPE_AP_DHCP_CLT		11				// DHCP client service
#define LOG_TYPE_AP_DNS_PROXY		12				// DNS proxy service
#define LOG_TYPE_AP_MAIL			13				// mail proxy service
#define LOG_TYPE_AP_PING			14				// ping service
#define LOG_TYPE_AP_POP3			15				// POP3 service
#define LOG_TYPE_AP_RIP				16				// RIP service
#define LOG_TYPE_AP_SMTP			17				// SMTP service
#define LOG_TYPE_AP_SNTP			18				// SNTP service
#define LOG_TYPE_AP_SWUP			19				// Software upgrade/TFTP service
#define LOG_TYPE_AP_SYSLOG_CLT		20				// syslog client service
#define LOG_TYPE_AP_XMODEM			21				// xmodem service
#define LOG_TYPE_AP_NBIOS			22				// netbios service
#define LOG_TYPE_AP_SNMP			23				// snmp service
#define LOG_TYPE_AP_TELNET			24				// telnet/rs232 daemon
#define LOG_TYPE_AP_UPNP			25				// UPnP service, original version
#define LOG_TYPE_AP_UPNP_A			26				// UPnP service, accton version
#define LOG_TYPE_AP_CGI				27				// cgi module

#define LOG_TYPE_HW_8947			28				// 8947
#define LOG_TYPE_HW_8947_EDIMAX		29				// 8947 for Edimax
#define LOG_TYPE_HW_8957			30				// 8957
#define LOG_TYPE_HW_ADM5106			31				// ADM5106
#define LOG_TYPE_HW_ADM5106_2		32				// ADM5106 for Edimax 2WAN
#define LOG_TYPE_HW_ADM5120			33				// ADM5120
#define LOG_TYPE_HW_CX82100			34				// Conexent 82100
#define LOG_TYPE_HW_2510			35				// 2510
#define LOG_TYPE_HW_USB_CORE		36				// usb driver core
#define LOG_TYPE_HW_USB_MAC			37				// usb driver for MAC
#define LOG_TYPE_HW_USB_STORAGE		38				// usb driver for storage
#define LOG_TYPE_HW_USB_HOST		39				// usb driver for host
#define LOG_TYPE_HW_USB_DEVICE		40				// usb driver for device
#define LOG_TYPE_HW					41				// hardware module

#define LOG_TYPE_KERNEL_SOCKET		42				// socket module
#define LOG_TYPE_KERNEL_TCPUDP		43				// TCP/UDP layer
#define LOG_TYPE_KERNEL_FAT			44				// FAT file system
#define LOG_TYPE_KERNEL_TSD			45				// TSD file system
#define LOG_TYPE_KERNEL_NAT			46				// NAT module
#define LOG_TYPE_KERNEL				47				// kernel module

#define LOG_TYPE_LIB_CRYPTO			48				// crypto library
#define LOG_TYPE_LIB_GMP			49				// big number operation library
#define LOG_TYPE_LIB_NEWLIB			50				// new standard library
#define LOG_TYPE_LIB_DIAL			51				// library for dial-up
#define LOG_TYPE_LIB_UNZIP			52				// library for unzip
#define LOG_TYPE_LIB_UI				53				// library for UI
#define LOG_TYPE_LIB_UTIL			54				// library for some utility

#define LOG_TYPE_LINK_AME150		55				// ame150 module for st20166
#define LOG_TYPE_LINK_AME150_A		56				// ame150 annex A module for st20156
#define LOG_TYPE_LINK_AME150_B		57				// ame150 annex B module for st20156
#define LOG_TYPE_LINK_ATM			58				// ATM module
#define LOG_TYPE_LINK_BRIDGE		59				// bridge module
#define LOG_TYPE_LINK_11g			60				// wireless 11g module
#define LOG_TYPE_LINK_11b_ATMEL		61				// wireless 11b atmel module
#define LOG_TYPE_LINK_11b_ATMEL_2	62				// wireless 11b atmel module for edimax
#define LOG_TYPE_LINK_11b_INSL_CX	63				// wireless 11b intersil module for conexent CPU
#define LOG_TYPE_LINK_11b_INSL		64				// wireless 11b intersil module
#define LOG_TYPE_LINK_PPP			65				// ppp module
#define LOG_TYPE_LINK_PPPOE			66				// pppoe module

#define LOG_TYPE_SECURE_1X			67				// 802.1x module
#define LOG_TYPE_SECURE_WPA			68				// WPA module
#define LOG_TYPE_CBAC				69				// firewall module
#define LOG_TYPE_CBAC_SPI			70				// firewall SPI module
#define LOG_TYPE_CBAC_DOS			71				// firewall DoS module
#define LOG_TYPE_CBAC_URL			72				// firewall URl blocking module
#define LOG_TYPE_CBAC_MAC			73				// firewall MAC filter module
#define LOG_TYPE_SECURE_IKE			74				// ike module
#define LOG_TYPE_SECURE_IPSEC		75				// ipsec module
#define LOG_TYPE_SECURE_IPSEC_IKE	76				// ipsec with ike module
#define LOG_TYPE_SECURE_PPTP		77				// pptp module
#define LOG_TYPE_SECURE_L2TP		78				// L2TP module
#define LOG_TYPE_LINK_11ag			79				// wireless 11ag module

#define LOG_TYPE_VoIP				80				// VoIP module

#define LOG_TYPE_AP_FTP_FTPD		81				// FTP server module
#define LOG_TYPE_AP_FTP_CLT			82				// FTP client module

#define LOG_TYPE_CLEAR_SYSLOG		255

// define syslog level
#define LOG_LEVEL_DEFAULT			0		// Other
#define LOG_LEVEL_DEFAULT_STR		"Default"
#define LOG_LEVEL_DEBUG				1
#define LOG_LEVEL_DEBUG_STR			"Debug"
#define LOG_LEVEL_INFO				2
#define LOG_LEVEL_INFO_STR			"Info"
#define LOG_LEVEL_WARNING			3	// default display
#define LOG_LEVEL_WARNING_STR		"Warning"
#define LOG_LEVEL_ERROR				4	// default display
#define LOG_LEVEL_ERROR_STR			"Error"

#define LOG_LEVEL_NUMBER			5
#define MAX_LOG_LEVEL_STR_LENGTH		10

// define syslog facilities
#define LOG_MESSAGE_TYPE_DEFAULT		0		// Other
#define LOG_MESSAGE_TYPE_DEFAULT_STR	"Default"
#define LOG_MESSAGE_TYPE_SYSTEM			1
#define LOG_MESSAGE_TYPE_SYSTEM_STR		"System"
#define LOG_MESSAGE_TYPE_WAN			2
#define LOG_MESSAGE_TYPE_WAN_STR		"Wan"
#define LOG_MESSAGE_TYPE_LAN			3
#define LOG_MESSAGE_TYPE_LAN_STR		"Lan"
#define LOG_MESSAGE_TYPE_VOICE			4
#define LOG_MESSAGE_TYPE_VOICE_STR		"Voice"
#define LOG_MESSAGE_TYPE_DATA			5
#define LOG_MESSAGE_TYPE_DATA_STR		"Data"
#define LOG_MESSAGE_TYPE_ACS			6
#define LOG_MESSAGE_TYPE_ACS_STR		"Acs"
#define LOG_MESSAGE_TYPE_UMTS			7
#define LOG_MESSAGE_TYPE_UMTS_STR		"Umts"
#define LOG_MESSAGE_TYPE_USB			8
#define LOG_MESSAGE_TYPE_USB_STR		"Usb"
#define LOG_MESSAGE_TYPE_FIREWALL		9
#define LOG_MESSAGE_TYPE_FIREWALL_STR	"Firewall"

#define LOG_MESSAGE_TYPE_CONNECTIONS	10
#define LOG_MESSAGE_TYPE_CONNECTIONS_STR	"Connections"
#define LOG_MESSAGE_TYPE_UPNP			11
#define LOG_MESSAGE_TYPE_UPNP_STR			"Upnp"
#define LOG_MESSAGE_TYPE_SECURITY		12
#define LOG_MESSAGE_TYPE_SECURITY_STR		"Security"
#define LOG_MESSAGE_TYPE_SERVICE		13
#define LOG_MESSAGE_TYPE_SERVICE_STR		"Service"
#define LOG_MESSAGE_TYPE_IPTV			14
#define LOG_MESSAGE_TYPE_IPTV_STR			"IPTV"

#define LOG_MESSAGE_TYPE_NUMBER			15
#define MAX_LOG_MESSAGE_STR_LENGTH		20

#endif // _ARCADYAN_SYSLOG_DEFINE_FOR_OPENSOURCE_H_