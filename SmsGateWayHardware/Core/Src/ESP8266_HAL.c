/*
 * ESP8266_HAL.c
 *
 *  Created on: 1-Des-2025
 *      Author: theloginest
 */


#include "UartRingbuffer_multi.h"
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

#define wifi_uart &huart1
#define pc_uart &huart2


char buffer[20];



// 		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
// 		<title>LED CONTROL</title>\n<style>html { font-family: Helvetica; \
// 		display: inline-block; margin: 0px auto; text-align: center;}\n\
// 		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
// 		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
// 		width: 80px;background-color: #1abc9c;border: none;color: white;\
// 		padding: 13px 30px;text-decoration: none;font-size: 25px;\
// 		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
// 		.button-on {background-color: #1abc9c;}\n.button-on:active \
// 		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
// 		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
// 		</style>\n</head>\n<body>\n<h1>ESP8266 LED CONTROL</h1>\n";

// char *LED_ON = "<p>LED Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>";
// char *LED_OFF = "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>";
// char *Terminate = "</body></html>";



/*****************************************************************************************************************************************/

void ESP_Init (char *SSID, char *PASSWD)
{
	char data[80];
	//HAL_UART_Transmit(&huart2, "Check\r\n",7, HAL_MAX_DELAY);

	Ringbuf_init();

	Uart_sendstring("RESETTING.", pc_uart);
	Uart_sendstring("AT+RST\r\n", wifi_uart);


	for (int i=0; i<5; i++)
	{
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}

	/********* AT **********/
	Uart_sendstring("AT\r\n", wifi_uart);

	while(!(Wait_for("AT\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT---->OK\n\n", pc_uart);


	/********* AT+CWMODE=1 **********/
	Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
	while (!(Wait_for("AT+CWMODE=1\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("CW MODE---->1\n\n", pc_uart);


	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_sendstring("connecting... to the provided AP\n", pc_uart);
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("WIFI GOT IP\r\n\r\nOK\r\n", wifi_uart)));
	sprintf (data, "Connected to,\"%s\"\n\n", SSID);
	Uart_sendstring(data,pc_uart);


	/********* AT+CIFSR **********/
	Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
	while (!(Wait_for("CIFSR:STAIP,\"", wifi_uart)));
	while (!(Copy_upto("\"",buffer, wifi_uart)));
	while (!(Wait_for("OK\r\n", wifi_uart)));
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	sprintf (data, "IP ADDR: %s\n\n", buffer);
	Uart_sendstring(data, pc_uart);


	// Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
	// while (!(Wait_for("AT+CIPMUX=1\r\r\n\r\nOK\r\n", wifi_uart)));
	// Uart_sendstring("CIPMUX---->OK\n\n", pc_uart);

	// Uart_sendstring("AT+CIPSERVER=1,80\r\n", wifi_uart);
	// while (!(Wait_for("OK\r\n", wifi_uart)));
	// Uart_sendstring("CIPSERVER---->OK\n\n", pc_uart);

	Uart_sendstring("Now Connect to the IP ADRESS\n\n", pc_uart);

}

int HTTP_GET_Request(char *host, int port, char *path)
{
	Uart_sendstring("Get Send\n", pc_uart);
    char cmd[128];
    char request[256];
    char response[512] = {0};

    Uart_flush(wifi_uart);

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for_timeout("OK\r\n", wifi_uart, 5000)) {
            Uart_sendstring("GET: Connection failed!\n", pc_uart);
            return 0;
        }

    sprintf(request,
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host, port);

    int len = strlen(request);
    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for_timeout(">", wifi_uart ,5000)) {
        Uart_sendstring("GET: Send prompt timeout!\n", pc_uart);
        return 0;
    }
    Uart_sendstring(request, wifi_uart);
    if (!Wait_for_timeout("SEND OK", wifi_uart , 5000)) {
        Uart_sendstring("GET: Send failed!\n", pc_uart);
        return 0;
    }

    // خواندن پاسخ — با تایم‌اوت بهتر
    if (Wait_for_timeout("+IPD,", wifi_uart, 5000)) {
        // عبور از +IPD,0,123:
        while (IsDataAvailable(wifi_uart)) {
            char c = Uart_read(wifi_uart);
            if (c == ':') break;
        }

        // خواندن همه داده‌ها با حلقه و تاخیر کوچک
        int i = 0;
        uint32_t last_rx = HAL_GetTick();
        while (i < 511) {
            if (IsDataAvailable(wifi_uart)) {
                response[i++] = Uart_read(wifi_uart);
                last_rx = HAL_GetTick();
            }
            // اگر 100ms داده‌ای نیومد، فرض کن انتهاست
            if ((HAL_GetTick() - last_rx) > 100) break;
            HAL_Delay(1);
        }
        response[i] = '\0';

        Uart_sendstring("GET Response:\n", pc_uart);
        Uart_sendstring(response, pc_uart);
        Uart_sendstring("\n", pc_uart);
    } else {
        Uart_sendstring("GET: No response received (timeout).\n", pc_uart);
    }

    // ⏱️ کمی صبر کن قبل از بستن
    HAL_Delay(100);
    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    Wait_for_timeout("OK\r\n", wifi_uart ,5000);
    return 1;
}
/**
 * ارسال درخواست HTTP POST با بدنه JSON
 * مثال: HTTP_POST_Request("192.168.1.3", 8000, "/api/receive", "{...}");
 */
/*
int HTTP_POST_Request(char *host, int port, char *path)
{
	Uart_sendstring("PostSend\n", pc_uart);
    char cmd[128];
    char request[512];
    char response[512] = {0};

    Uart_flush(wifi_uart); // پاک‌سازی بافر

    // اتصال TCP
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for("OK\r\n", wifi_uart)) {
        Uart_sendstring("POST: Connection failed!\n", pc_uart);
        return 0;
    }


    int json_len = strlen(json_data);
    sprintf(request,
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, host, port, json_len, json_data);

    sprintf(request,
        "POST %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host, port);

    int len = strlen(request);
    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for_timeout(">", wifi_uart,5000)) {
        Uart_sendstring("POST: Send prompt timeout!\n", pc_uart);
        return 0;
    }

    Uart_sendstring(request, wifi_uart);
    if (!Wait_for_timeout("SEND OK", wifi_uart,5000)) {
        Uart_sendstring("POST: Send failed!\n", pc_uart);
        return 0;
    }

    // خواندن پاسخ سرور
    if (Wait_for_timeout("+IPD,", wifi_uart, 5000)) {
        // عبور از +IPD,0,123: تا ":"
        while (IsDataAvailable(wifi_uart)) {
            char c = Uart_read(wifi_uart);
            if (c == ':') break;
        }

        // خواندن بدنه HTTP
        int i = 0;
        while (i < 511 && IsDataAvailable(wifi_uart)) {
            response[i++] = Uart_read(wifi_uart);
            HAL_Delay(1);
        }
        response[i] = '\0';

        Uart_sendstring("POST Response:\n", pc_uart);
        Uart_sendstring(response, pc_uart);
        Uart_sendstring("\n", pc_uart);
    } else {
        Uart_sendstring("POST: No response received (timeout).\n", pc_uart);
    }

    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    if (!Wait_for_timeout("OK", wifi_uart, 2000)) {
        Uart_sendstring("Warning: CIPCLOSE not confirmed\n", pc_uart);
    }
    HAL_Delay(500);
}
*/
int HTTP_POST_Request(char *host, int port, char *path)
{
    Uart_sendstring("PostSend\n", pc_uart);
    char cmd[128];
    char request[256]; // کوچک‌تر — چون بدون بدنه
    char response[512] = {0};

    Uart_flush(wifi_uart);

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for_timeout("OK\r\n", wifi_uart, 5000)) {
        Uart_sendstring("POST: Connection failed!\n", pc_uart);
        return 0;
    }

    // ✅ بدون پورت در Host
    sprintf(request,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host);

    int len = strlen(request);
    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for_timeout(">", wifi_uart, 5000)) {
        Uart_sendstring("POST: Send prompt timeout!\n", pc_uart);
        return 0;
    }

    Uart_sendstring(request, wifi_uart);
    if (!Wait_for_timeout("SEND OK", wifi_uart, 5000)) {
        Uart_sendstring("POST: Send failed!\n", pc_uart);
        return 0;
    }

    if (Wait_for_timeout("+IPD,", wifi_uart, 5000)) {
        while (IsDataAvailable(wifi_uart)) {
            char c = Uart_read(wifi_uart);
            if (c == ':') break;
        }
        int i = 0;
        while (i < 511 && IsDataAvailable(wifi_uart)) {
            response[i++] = Uart_read(wifi_uart);
            HAL_Delay(1);
        }
        response[i] = '\0';
        Uart_sendstring("POST Response:\n", pc_uart);
        Uart_sendstring(response, pc_uart);
        Uart_sendstring("\n", pc_uart);
    } else {
        Uart_sendstring("POST: No response received (timeout).\n", pc_uart);
    }

    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    Wait_for_timeout("OK", wifi_uart, 2000);
    HAL_Delay(500);
    return 1;
}

// int Server_Send (char *str, int Link_ID)
// {
// 	int len = strlen (str);
// 	char data[80];
// 	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
// 	Uart_sendstring(data, wifi_uart);
// 	while (!(Wait_for(">", wifi_uart)));
// 	Uart_sendstring (str, wifi_uart);
// 	while (!(Wait_for("SEND OK", wifi_uart)));
// 	sprintf (data, "AT+CIPCLOSE=5\r\n");
// 	Uart_sendstring(data, wifi_uart);
// 	while (!(Wait_for("OK\r\n", wifi_uart)));
// 	return 1;
// }

// void Server_Handle (char *str, int Link_ID)
// {
// 	char datatosend[1024] = {0};
// 	if (!(strcmp (str, "/ledon")))
// 	{
// 		sprintf (datatosend, Basic_inclusion);
// 		strcat(datatosend, LED_ON);
// 		strcat(datatosend, Terminate);
// 		Server_Send(datatosend, Link_ID);
// 	}

// 	else if (!(strcmp (str, "/ledoff")))
// 	{
// 		sprintf (datatosend, Basic_inclusion);
// 		strcat(datatosend, LED_OFF);
// 		strcat(datatosend, Terminate);
// 		Server_Send(datatosend, Link_ID);
// 	}

// 	else
// 	{
// 		sprintf (datatosend, Basic_inclusion);
// 		strcat(datatosend, LED_OFF);
// 		strcat(datatosend, Terminate);
// 		Server_Send(datatosend, Link_ID);
// 	}

// }
/**
 * ارسال درخواست HTTP GET و بازگرداندن پاسخ (تا 512 بایت)
 * مثال: HTTP_GET_Request("192.168.1.3", 8000, "/api/receive?input=5");
 */




// void Server_Start (void)
// {
// 	char buftocopyinto[64] = {0};
// 	char Link_ID;
// 	while (!(Get_after("+IPD,", 1, &Link_ID, wifi_uart)));
// 	Link_ID -= 48;
// 	while (!(Copy_upto(" HTTP/1.1", buftocopyinto, wifi_uart)));
// 	if (Look_for("/ledon", buftocopyinto) == 1)
// 	{
// 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
// 		Server_Handle("/ledon",Link_ID);
// 	}

// 	else if (Look_for("/ledoff", buftocopyinto) == 1)
// 	{
// 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
// 		Server_Handle("/ledoff",Link_ID);
// 	}

// 	else if (Look_for("/favicon.ico", buftocopyinto) == 1);

// 	else
// 	{
// 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
// 		Server_Handle("/ ", Link_ID);
// 	}
// }
