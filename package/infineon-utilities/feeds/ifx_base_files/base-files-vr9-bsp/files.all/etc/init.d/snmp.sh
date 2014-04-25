#!/bin/sh /etc/rc.common

#START=27

start() {
	#/etc/snmp/snmpd.conf
	if [ "$CONFIG_FEATURE_SNMPv1" = "1" ]; then
		CONFIG_FEATURE_IFX_SNMP_AGENT="1"
	elif [ "$CONFIG_FEATURE_SNMPv3" = "1" ]; then
		CONFIG_FEATURE_IFX_SNMP_AGENT="1"
	elif [ "$CONFIG_FEATURE_IFX_DSL_MANAGEMENT" = "1" ]; then
		CONFIG_FEATURE_IFX_SNMP_AGENT="1"
	fi
	
	if [ "$CONFIG_FEATURE_IFX_SNMP_AGENT" = "1" ]; then
		echo "######  V1 Configuration  ######" > /etc/snmp/snmpd.conf
        	echo "sysname       $MIB2sysName" >> /etc/snmp/snmpd.conf
        	echo "syslocation   $MIB2sysLocation" >> /etc/snmp/snmpd.conf
        	echo "syscontact    $MIB2sysContact" >> /etc/snmp/snmpd.conf
		echo "rocommunity   $SNMProcommunity" >> /etc/snmp/snmpd.conf
		echo "rwcommunity   $SNMPrwcommunity" >> /etc/snmp/snmpd.conf
		if [ "$SNMPTrapEnable" = "1" ]; then
			echo "trapsink  $SNMPTrapIP  $SNMPTrapCommunity  $SNMPTrapPort" >> /etc/snmp/snmpd.conf
			echo "trapcommunity $SNMPTrapCommunity " >> /etc/snmp/snmpd.conf
			echo "authtrapenable    1" >> /etc/snmp/snmpd.conf
		else
			echo "#trapsink $SNMPTrapIP  $SNMPTrapCommunity	$SNMPTrapPort" >> /etc/snmp/snmpd.conf
		fi

#Forming the SNMP Transport string - for agent to listen on
        if [ "$SNMPTransportUDPEnable" = "1" ]; then
                echo -n "agentaddress   $SNMPTransportUDP" >> /etc/snmp/snmpd.conf
        fi
        if [ "$IFX_SNMP_TRANSPORT_ATMPVC_DOMAIN" = "1" ]; then
                if [ "$SNMPTransportPVCEnable" = "1" ]; then
                        if [ "$SNMPTransportUDPEnable" = "1" ]; then
                                echo -n ",$SNMPTransportPVC" >> /etc/snmp/snmpd.conf
                        else
                                echo -n "agentaddress   $SNMPTransportPVC" >> /etc/snmp/snmpd.conf
                        fi
                fi
                if [ "$IFX_SNMP_TRANSPORT_EOC_DOMAIN" = "1" ]; then
                        if [ "$SNMPTransportEOCEnable" = "1" ]; then
                                if [  "$SNMPTransportUDPEnable" = "0" -a "$SNMPTransportPVCEnable" = "0" ]; then
                                        echo "agentaddress      $SNMPTransportEOC" >> /etc/snmp/snmpd.conf
                                else
                                        echo ",$SNMPTransportEOC" >> /etc/snmp/snmpd.conf
                                fi
                        fi
                fi
        fi
        echo "" >> /etc/snmp/snmpd.conf

		echo "######  V3 Configuration  ######"	>> /etc/snmp/snmpd.conf
	fi
}
