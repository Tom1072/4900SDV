// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netinet/in.h>

// #define RPI_BOARD_IP_ADDR "172.16.41.129"
// #define UDP_PORT 5001
// #define BUFFER_SIZE 1000

// int udpListen(int argc, char *argv[]) {
// 	int udp_fd;
// 	int port_number = UDP_PORT;
// 	int rc;
// 	ssize_t recv_length;
// 	ssize_t send_length;
// 	struct sockaddr_in server_address;
// 	struct sockaddr_in board_address;
// 	unsigned char buffer[BUFFER_SIZE];
// 	char cstrBuf[BUFFER_SIZE];

// 	printf("UDP client app receive data demo \n");

// 	printf("Creating UDP socket on port: %d\n", port_number);
// 	udp_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
// 	if (udp_fd < 0) {
// 		printf("Unable to create socket.");
// 	}

// 	memset(&board_address, 0, sizeof( board_address ) );
// 	board_address.sin_family = AF_INET;
// 	board_address.sin_addr.s_addr = inet_addr(RPI_BOARD_IP_ADDR) ;
// 	board_address.sin_port = htons( port_number );

// 	rc = bind(udp_fd, (struct sockaddr *)&board_address, sizeof(board_address));
// 	if(rc<0) {
// 		printf("bind failed rc=%d \r\n", rc);
// 		return EXIT_FAILURE;
// 	}

// 	recv_length = recvfrom( udp_fd, buffer, sizeof(buffer), 0, NULL, NULL);
// 	if ( recv_length < 0 ) {
// 		printf("recvfrom failed recv_length=%d \r\n”", recv_length);
// 	} else {
// 		rc = 0;
// 		while ( rc < recv_length ) {
// 			sprintf( &cstrBuf[rc*2], "%02x", buffer[rc] );
// 			rc += 1;
// 		}
// 		printf("recvfrom successful! recv data : %s\n", cstrBuf);
// 	}

// 	close( udp_fd );

// 	return EXIT_SUCCESS;

// }
