#include <iostream>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdint.h>
#include <fstream>
using namespace std;

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(const char *errorMessage);  /* External error handling function */
void buscar (char *array,char *echo, int longitud,int palabra);
void escribir(char *frec,char *vel,char *vol,char *temp,char *tiemp);

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /*   In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX + 1];    /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    char ser[]= "186.155.208.171";
    int longitud_recibido;
    char frec[ECHOMAX]=" ",vel[ECHOMAX]=" ",vol[ECHOMAX]=" ",temp[ECHOMAX]=" ",tiemp[ECHOMAX]=" ";

    if ((argc < 2) || (argc > 3)) /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    servIP = &ser[0] ;       /* First arg' server IP address (dotted quad)*/
    echoString = argv[1] ;   /* Second arg' string to echo */

    if ((echoStringLen = strlen(echoString)) > ECHOMAX) /* Check input length */
        DieWithError("Echo word too long");


    echoServPort = 65000; /* 7 is the well-known port for the echo service */

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);       /* Server port */

    /* Send the string to the server */
    if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected");

    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
        (struct sockaddr *) &fromAddr, &fromSize)) == echoStringLen)
        DieWithError("recvfrom() failed") ;

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }

    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0' ;
    printf("Received: %s\n", echoBuffer); /* Print the echoed arg */

    longitud_recibido=strlen(echoBuffer);
    buscar (&frec[0],&echoBuffer[0],longitud_recibido,1);
    buscar (&vel[0],&echoBuffer[0],longitud_recibido,2);
    buscar (&vol[0],&echoBuffer[0],longitud_recibido,3);
    buscar (&temp[0],&echoBuffer[0],longitud_recibido,4);
    buscar (&tiemp[0],&echoBuffer[0],longitud_recibido,5);
    escribir(&frec[0],&vel[0],&vol[0],&temp[0],&tiemp[0]);

    //printf("La frecunecia es: %s HZ\n",frec );
    //printf("La velocidad es: %s m/s\n",vel );
    //printf("EL voltaje es: %s V\n",vol );
    //printf("La temperatura es: %s °c\n",temp );
    //printf("El tiempo es: %s timestamp\n",tiemp );


    close(sock);
    exit(0);
}

void DieWithError(const char *errorMessage){
	perror(errorMessage);
	exit(1);
}


void buscar (char *array,char *echo, int longitud, int palabra){
  int j=0;
  int operador=0;
  for (int i=0;i<longitud;i++){
    if (*(echo+i) == 'f' && *(echo+i+9) == 'a' && palabra ==1)
        operador = 1;
    if (*(echo+i) == 'v' && *(echo+i+8) == 'd' && palabra == 2)
          operador = 2;
    if (*(echo+i) == 'v' && *(echo+i+6) == 'e' && palabra == 3)
          operador = 3;
    if (*(echo+i) == 't' && *(echo+i+10) == 'a' && palabra == 4)
          operador = 4;
    if (*(echo+i) == 't' && *(echo+i+13) == 'p' && palabra == 5)
          operador = 5;
       switch (operador) {
         case 1:
               for (int t=i+12;t<longitud;t++){
                 if (*(echo+t)!= ',' && operador==1){
                   *(array+j)=*(echo+t);
                   j++;
                 }else{
                      operador = 0;
                      j=0;
                    }
                }
                break;
          case 2:
                for (int t=i+11;t<longitud;t++){
                  if (*(echo+t)!= ',' && operador==2){
                    *(array+j)=*(echo+t);
                    j++;
                  }else{
                       operador = 0;
                       j=0;
                     }
                 }
                 break;
          case 3:
               for (int t=i+9;t<longitud;t++){
                 if (*(echo+t)!= ',' && operador==3){
                   *(array+j)=*(echo+t);
                   j++;
                 }else{
                      operador = 0;
                      j=0;
                    }
                }
                break;
          case 4:
               for (int t=i+13;t<longitud;t++){
                 if (*(echo+t)!= ',' && operador==4){
                   *(array+j)=*(echo+t);
                   j++;
                 }else{
                      operador = 0;
                      j=0;
                    }
                }
                break;
          case 5:
               for (int t=i+16;t<longitud;t++){
                 if (*(echo+t)!= '}' && operador==5){
                   *(array+j)=*(echo+t);
                   j++;
                 }else{
                      operador = 0;
                      j=0;
                    }
                }
                break;
          default:
                break;
      }
  }
}

void escribir(char *frec,char *vel,char *vol,char *temp,char *tiemp){
    ofstream archivo("/home/alse/Documents/project/datos.txt");

    if(archivo.fail()){
        cout<< "No se pudo abrir el archivo";
        exit(1);
    }

    archivo << frec
            << '\n'
            << vel
            << '\n'
            << vol
            << '\n'
            << temp
            << '\n'
            << tiemp
            << '\n';


    archivo.close();
}
