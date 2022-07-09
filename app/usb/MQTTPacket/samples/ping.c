/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - port
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTPacket.h"
#include "transport.h"
#include "MultiTimer.h"

#define LOG_TAG    "app_mqtt_ping"

#include "elog.h"

#define KEEPALIVE_INTERVAL 20

unsigned char mqtt_buf[200];
static MultiTimer mqtt_timer;

static void mqttTimerCallback(MultiTimer* timer, void *userData)
{
	int len = 0;
	int buflen = sizeof(mqtt_buf);

	len = MQTTSerialize_pingreq(mqtt_buf, buflen);
	transport_sendPacketBuffer(0, mqtt_buf, len);
	log_d("Ping...");
	if (MQTTPacket_read(mqtt_buf, buflen, transport_getdata) == PINGRESP)
	{
		log_d("Pong\n");
	} 
	MultiTimerStart(timer, 10000, mqttTimerCallback, "5s mqtt timer");  
}

void mqtt_ping(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	int mysock = 0;
	//unsigned char buf[200];
	int buflen = sizeof(mqtt_buf);
	int len = 0;
	char *host = "192.168.1.2";
	int port = 1883;
	int ret = 0;

	
	log_d("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "me";
	data.keepAliveInterval = KEEPALIVE_INTERVAL;
	data.cleansession = 1;
	data.username.cstring = "testuser";
	data.password.cstring = "testpassword";

	len = MQTTSerialize_connect(mqtt_buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, mqtt_buf, len);

	log_d("Sent MQTT connect\n");
	/* wait for connack */
	ret = MQTTPacket_read(mqtt_buf, buflen, transport_getdata);
	log_d("MQTTPacket_read ret is %d",ret);
	if (ret == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;
		log_d("MQTTPacket_read success");
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buf, buflen) != 1 || connack_rc != 0)
		{
			log_e("Unable to connect, return code %d\n", connack_rc);
			goto exit;
		}
	}
	else
	{
		goto exit;
	}
		

	log_d("MQTT connected\n");

	//MultiTimerStart(&mqtt_timer, 10000, mqttTimerCallback, "5s mqtt timer"); 
	//start_ping_timer();

	// while (!toStop)
	// {
	// 	while(!time_to_ping());
	// 	len = MQTTSerialize_pingreq(buf, buflen);
	// 	transport_sendPacketBuffer(mysock, buf, len);
	// 	printf("Ping...");
	// 	if (MQTTPacket_read(buf, buflen, transport_getdata) == PINGRESP){
	// 		printf("Pong\n");
	// 		start_ping_timer();
	// 	}
	// 	else {
	// 		printf("OOPS\n");
	// 		goto exit;
	// 	}
		
	// }

	//log_d("disconnecting\n");
	//len = MQTTSerialize_disconnect(buf, buflen);
	//rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);

}
