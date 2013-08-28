#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>           /*  for string comparing      */


#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)
#define LISTENQ        (1024)


int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    char      interface[16];         /*  interface to listen to    */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
    char      keyw[50] = "connect\n";/*  connection keyword        */
    char      rx_path[124];          /*  absolute path to rx file  */
    char      tx_path[124];          /*  absolute path to tx file  */
    char      line[80];
    char      rx_str[80];
    char      tx_str[80];
    int       rx1, rx2, rx_bps, rx_kbps, tx1, tx2, tx_bps, tx_kbps;


    /*  Get interface (required) and port (optional) 
	from arguments and set to default port if no given */

    if ( argc == 3 ) {
	strcpy( interface, argv[1] );
	port = strtol(argv[2], &endptr, 0);
	if ( *endptr ) {
	    fprintf(stderr, "tm-tacho-server: ERROR: Invalid port number.\n");
	    exit(EXIT_FAILURE);
	}
    }
    else if ( argc == 2 ) {
      if ( strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "-help", 5) == 0 ) {
        fprintf(stderr, "Usage: ./tm-tacho-server [interface to monitor] (port to listen to)\nex: ./tm-tacho-server eth1 2002\nInterface is required, port is optional, and defaults to 2002\n");
        exit(EXIT_FAILURE);
      }
	else{
        strcpy( interface, argv[1] );
	   port = ECHO_PORT;
      }
    }
    else if ( argc < 2 ) {
	fprintf(stderr, "tm-tacho-server:  ERROR: Interface required. Use -h to get help\n");
	exit(EXIT_FAILURE);
    }
    else {
	fprintf(stderr, "tm-tacho-server: ERROR: Invalid arguments. Use -h to get help\n");
	exit(EXIT_FAILURE);
    }


    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "tm-tacho-server: ERROR: Could not create socket\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
	fprintf(stderr, "tm-tacho-server: ERROR: Could not bind to interface\n");
	exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
	fprintf(stderr, "tm-tacho-server: ERROR: Could not start listener\n");
	exit(EXIT_FAILURE);
    }


    /*  Initialize file paths  */

    strcpy( rx_path, "/sys/class/net/");
    strcat( rx_path, interface);
    strcat( rx_path, "/statistics/rx_bytes");

    strcpy( tx_path, "/sys/class/net/");
    strcat( tx_path, interface);
    strcat( tx_path, "/statistics/tx_bytes");

    /*  Enter an infinite loop to respond
        to client requests and respond with data  */

    while ( 1 ) {

	/*  Wait for a connection, then accept() it  */

	if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
	    fprintf(stderr, "tm-tacho-server: ERROR: Could not accept connection\n");
	    exit(EXIT_FAILURE);
	}


	/* feeding loop */

	while ( 1 ) {
		rx1 = Readfile( rx_path );
		tx1 = Readfile( tx_path );
		sleep(1);
		rx2 = Readfile( rx_path );
		tx2 = Readfile( tx_path );

		//fprintf(stderr, "tm-tacho-server DEBUG:::\n");
		//fprintf(stderr, "%u | %u\n", rx1, rx2);
		//fprintf(stderr, "%u | %u\n", tx1, tx2);

		rx_bps = rx2 - rx1;
		rx_kbps = rx_bps / 1024;
		tx_bps = tx2 - tx1;
		tx_kbps = tx_bps / 1024;

		//fprintf(stderr, "%u | %u\n", rx_kbps, rx_bps);
		//fprintf(stderr, "%u | %u\n", tx_kbps, tx_bps);

		sprintf( rx_str, "%d", rx_kbps );
		sprintf( tx_str, "%d", tx_kbps );

		strcpy( line, rx_str );
		strcat( line, "|" );
		strcat( line, tx_str );
		strcat( line, "\n" );

		Writeline(conn_s, line, strlen(line));
	}

	/*  Close the connected socket  */

	if ( close(conn_s) < 0 ) {
	    fprintf(stderr, "tm-tacho-server: ERROR: Could not close connection\n");
	    exit(EXIT_FAILURE);
	}
    }
}
