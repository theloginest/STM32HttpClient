/*
 * ESP8266_HAL.h
 *
 *  Created on: 1-Des-2025
 *      Author: theloginest
 */

#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_


void ESP_Init (char *SSID, char *PASSWD);

// void Server_Start (void);
int HTTP_GET_Request(char *host, int port, char *path);
int HTTP_POST_Request(char *host, int port, char *path);

#endif /* INC_ESP8266_HAL_H_ */
