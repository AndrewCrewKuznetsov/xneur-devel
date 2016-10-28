/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  Copyright (C) 2006-2012 XNeur Team
 *
 */

#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "log.h"

#define CMDCOUNT 6
char *mail[CMDCOUNT] = {	"EHLO xneur.ru\n",
							"MAIL FROM:xneurlog@xneur.ru\n",
				      		"RCPT TO:",
							"DATA\n",
							".\n",
		  				    "QUIT\n"	};

#define FROM 		"From: <xneurlog@xneur.ru>\n"
#define SUBJ 		"Subject: Log Archive\n"
#define MIME 		"MIME-Version: 1.0\n"
#define CONT_MIX 	"Content-Type: multipart/mixed; boundary=\"----------D675117161112F6\"\n\n"
#define BOUN 		"------------D675117161112F6\n"
#define BOUN_END	"------------D675117161112F6--\n\n"
#define CONT_TXT 	"Content-Type: text/plain; charset=utf-8\n\n"
#define TEXT		"Xneur log in attachment\n\n"
#define CONT_APP 	"Content-Type: application/x-gzip; name=\"%s\"\nContent-Transfer-Encoding: base64\nContent-Disposition: attachment; filename=\"%s\"\n\n"

#define DEFAULT_BUFLEN 1024

void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
	const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void encode_base64(FILE *infile, char *base64text)
{
	int linesize = 72;

	unsigned char in[3], out[4];
	int i, j = 0, len, blocksout = 0;

	while (!feof(infile)) 
	{
		len = 0;
		
		for (i = 0; i < 3; i++) 
		{
			in[i] = (unsigned char) getc(infile);
			if (!feof(infile)) 
                len++;
			else
                in[i] = 0;
		}
		if (len) 
		{
			encodeblock(in, out, len);
			for (i = 0; i < 4; i++)
				base64text[j++] = out[i];
			blocksout++;
		}
		if (blocksout >= (linesize/4) || feof(infile)) 
		{
			if (blocksout)
				base64text[j++] = '\n';
			blocksout = 0;
		}
	}
}

static void send_packet(int s, const void *msg, size_t len, int flags)
{
	ssize_t count = send(s, msg, len, flags);
	if (count == -1)
		log_message (ERROR, _("Lost the package when sending mail."));
}

void send_mail_with_attach(char *file, char host[], int port, char rcpt[])
{
	if (host == NULL)
		return;
	if (rcpt == NULL)
		return;
	if (file == NULL)
		return;
	
	int fd, i;

	struct sockaddr_in sock;
	struct hostent *hp;
	
	sock.sin_family = AF_INET;
	//sock.sin_addr.s_addr = inet_addr(host);
	if (inet_aton(host, &sock.sin_addr) != 1) 
	{
		hp = gethostbyname(host);
		if (!hp) 
		{
			log_message(ERROR, _("Unknown host %s\n"), host);
			return;
		}
		memcpy(&sock.sin_addr, hp->h_addr, 4);
	}
	sock.sin_port = htons(port);
	memset(&sock.sin_zero, '\0', 8);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		log_message(ERROR, _("Unable open socket!"));
		return;
	}

	if (connect(fd, (struct sockaddr *)&sock, sizeof(struct sockaddr)) == -1)
	{
		close(fd);
		log_message(ERROR, _("Unable to connect to %s"), (char *)inet_ntoa(sock.sin_addr));
		return;
	}
	
	FILE *stream = fopen(file, "rb");
	if (stream == NULL)
	{
		log_message (ERROR, _("Can't open to read file \"%s\""), file);
		close(fd);
		return;
	}

	struct stat sb;
	if (stat(file, &sb) != 0)
	{
		close(fd);
		fclose(stream);
		log_message (ERROR, _("Can't get state of file \"%s\""), file);
		return;
	}
	
	if (sb.st_size < 0)
	{
		close(fd);
		fclose(stream);
		log_message (ERROR, _("Can't get state of file \"%s\""), file);
		return;
	}
	
	do {
		file = strstr(file, "/");
		if (file == NULL)
		{
			close(fd);
			fclose(stream);
			return;
		}
		file++;
	} while (strstr(file, "/") != NULL);
	
	for (i = 0; i < CMDCOUNT; i++)
	{
		send_packet(fd, mail[i], strlen(mail[i]), 0);
		if (i == 2)
		{
			send_packet(fd, rcpt, strlen(rcpt), 0);
			send_packet(fd, "\n", sizeof(char), 0);
		}
		if (i == 3)
		{
			send_packet(fd, FROM, strlen(FROM), 0);
			send_packet(fd, "To: ", 4, 0);
			send_packet(fd, rcpt, strlen(rcpt), 0);
			send_packet(fd, "\n", sizeof(char), 0);
			send_packet(fd, SUBJ, strlen(SUBJ), 0);
			send_packet(fd, MIME, strlen(MIME), 0);
			send_packet(fd, CONT_MIX, strlen(CONT_MIX), 0);
			send_packet(fd, BOUN, strlen(BOUN), 0);
			send_packet(fd, CONT_TXT, strlen(CONT_TXT), 0);
			send_packet(fd, TEXT, strlen(TEXT), 0);
			send_packet(fd, BOUN, strlen(BOUN), 0);
			char *cont_app = malloc((strlen(CONT_APP)+ 2*strlen(file) + 5) * sizeof(char));
			sprintf (cont_app, CONT_APP, file, file);
			send_packet(fd, cont_app, strlen(cont_app), 0);
			free(cont_app);
			char *base64text = malloc((sb.st_size*2) * sizeof(char));
			encode_base64(stream, base64text);
			send_packet(fd, base64text, strlen(base64text), 0);
			free(base64text);
			
			send_packet(fd, BOUN_END, strlen(BOUN_END), 0);
		}
	}	
	
	sleep(5);
	char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	int result = 0;
	do {
		result = recv(fd, recvbuf, recvbuflen, 0);
		if (result < 0)
		{
			close(fd);
			fclose(stream);
			log_message (ERROR, _("Mail server return Error %d"), result);
			return;
		}
		recvbuf[result] = '\0';
		/*if ( result > 0 )
			log_message (DEBUG, "Mail server return answer:\n%s\n", recvbuf, result);
		else if ( result == 0 )
			log_message (ERROR, "Connection closed");
		else
			log_message (ERROR, "Connection Error %d", result);*/
	} while( result > 0 );
	
	close(fd);
	fclose(stream);
}
