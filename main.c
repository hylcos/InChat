#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <pwd.h>

#include "tibemsUtilities.h"

#define MAXBUF 1024
/*-----------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
char							url_a[MAXBUF];
char							port_a[MAXBUF];
char							topic_a[MAXBUF];
char							Messages[MAXBUF][MAXBUF];

char * 							username	 = NULL;
char * 							my_string	 = "";
tibems_bool                     useTopic     = TIBEMS_TRUE;
tibems_bool                     useAsync      = TIBEMS_FALSE;
tibemsAcknowledgeMode           ackMode      = TIBEMS_AUTO_ACKNOWLEDGE;

tibemsConnectionFactory         factory      = NULL;
tibemsConnection                connection   = NULL;
tibemsSession                   session      = NULL;
tibemsMsgConsumer               msgConsumer  = NULL;
tibemsMsgProducer          		msgProducer  = NULL;
tibemsDestination               destination  = NULL;
tibemsSSLParams                 sslParams    = NULL;
tibems_int                      receive      = 1;
tibemsErrorContext              errorContext = NULL;

/*-----------------------------------------------------------------------
 * Needs to go away
 *----------------------------------------------------------------------*/
char*                           numberMsg    = "1";
char*                           userName     = NULL;
char*                           password     = NULL;
char*                           pk_password  = NULL;
int								counter      = 1;

char* stradd(const char* a, const char* b)
{
    size_t len = strlen(a) + strlen(b);
    char *ret = (char*)malloc(len * sizeof(char) + 1);
    *ret = '\0';
    return strcat(strcat(ret, a) ,b);
}

void printlogo()
{
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	printf("             _________ _        _______           _______ _________             ");
	printf("             \\__   __/( (    /|(  ____ \\|\\     /|(  ___  )\\__   __/            ");
	printf("                 ) (   |  \\  ( || (    \\/| )   ( || (   ) |   ) (               ");
	printf("                 | |   |   \\ | || |      | (___) || (___) |   | |               ");
	printf("                 | |   | (\\ \\) || |      |  ___  ||  ___  |   | |               ");
	printf("                 | |   | | \\   || |      | (   ) || (   ) |   | |               ");
	printf("              ___) (___| )  \\  || (____/\\| )   ( || )   ( |   | |               ");
	printf("              \\_______/|/    )_)(_______/|/     \\||/     \\|   )_(                ");
	printf("                                                                                ");
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	printf("##############  Created by Hylco Uding # Intraffic \xa9            ################");
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	printf("###################         Press ENTER to Continue          ###################");
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	
}

void ParseCfgFile(char * filename)
{
	
	FILE *fp = fopen(filename,"r");
	 if (fp != NULL)
        { 
                char line[MAXBUF];
                int i = 0;

                while(fgets(line, sizeof(line), fp) != NULL)
                {
                        char *cfline;
                        cfline = strstr((char *)line,"= " );
                        cfline = strstr((char *)cfline," " );
    
                        if (i == 0)
						{
                                memcpy(url_a,cfline,strlen(cfline));
                        } 
						else if (i == 1)
						{
                                 memcpy(port_a,cfline,strlen(cfline));
                        } 
						else if (i == 2){
                                 memcpy(topic_a,cfline,strlen(cfline));
						}
                        i++;
                }
		  fclose(fp);  
	 	} 
		else 
		{
			
			printf("No Config File Found\n\n");
			printf("This is how you make a Config File: \n");
			printf("url = <serverUrl>\n");
			printf("port = <portNumber>\n");
			printf("topic = <topic>\n");
			fflush(stdout);
			exit(1);
		}
		int i;
		for (i = 0; i < strlen(topic_a); ++i)
		{
			if(topic_a[i] == ' ' && i==0)
				for (i = 0; i < strlen(topic_a); ++i)
				{
						topic_a[i] = topic_a[i+1];
						if(topic_a[i] == '\n')
							topic_a[i] = '\0';
				}
		}
		
        
}

void fail( const char* message, tibemsErrorContext errContext)
{
    tibems_status               status = TIBEMS_OK;
    const char*                 str    = NULL;

    printf("ERROR: %s\n",message);

    status = tibemsErrorContext_GetLastErrorString(errContext, &str);
    printf("\nLast error message =\n%s\n", str);
    status = tibemsErrorContext_GetLastErrorStackTrace(errContext, &str);
    printf("\nStack trace = \n%s\n", str);

    exit(1);
}

void onException(tibemsConnection conn,tibems_status reason,void*  closure)
{
    /* print the connection exception status */

    if (reason == TIBEMS_SERVER_NOT_CONNECTED)
    {
        printf(" CONNECTION EXCEPTION: Server Disconnected\n");
        receive = 0;
    }
}

static void onCompletion(tibemsMsg msg, tibems_status status, void* closure)
{
    const char *text;

    if (tibemsTextMsg_GetText((tibemsTextMsg)msg, &text) != TIBEMS_OK)
    {
        printf("Error retrieving message!\n");
        return;
    }

    if (status == TIBEMS_OK)
    {
        printf("Successfully sent message %s.\n", text);
    }
    else
    {
        printf("Error sending message %s.\n", text);
        printf("Error:  %s.\n", tibemsStatus_GetText(status));
    }
}

void * RecieveMessages(void * ptr)
{
	tibems_status               status      = TIBEMS_OK;
    tibemsMsg                   msg         = NULL;
    const char*                 txt         = NULL;
    tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
    char*                       msgTypeName = "UNKNOWN";
	while(1){
		status = tibemsMsgConsumer_Receive(msgConsumer,&msg);
		printf("\n");
		printf("\033[24;0H"); 
		//
		//printf("\033[A"); 
        if (status != TIBEMS_OK)
        {
            if (status == TIBEMS_INTR)
            {
                /* this means receive has been interrupted. This
                 * could happen if the connection has been terminated
                 * or the program closed the connection or the session.
                 * Since this program does not close anything, this 
                 * means the connection to the server is lost, it will
                 * be printed in the connection exception. So ignore it
                 * here.
                 */
                return;
            }
            fail("Error receiving message", errorContext);
        }
        if (!msg)
            break;

        /* acknowledge the message if necessary */
        if (ackMode == TIBEMS_CLIENT_ACKNOWLEDGE ||
            ackMode == TIBEMS_EXPLICIT_CLIENT_ACKNOWLEDGE ||
            ackMode == TIBEMS_EXPLICIT_CLIENT_DUPS_OK_ACKNOWLEDGE)
        {
            status = tibemsMsg_Acknowledge(msg);
            if (status != TIBEMS_OK)
            {
                fail("Error acknowledging message", errorContext);
            }
        }

        /* check message type */
        status = tibemsMsg_GetBodyType(msg,&msgType);
        if (status != TIBEMS_OK)
        {
            fail("Error getting message type", errorContext);
        }

        switch(msgType)
        {
            case TIBEMS_MESSAGE:
                msgTypeName = "MESSAGE";
                break;

            case TIBEMS_BYTES_MESSAGE:
                msgTypeName = "BYTES";
                break;

            case TIBEMS_OBJECT_MESSAGE:
                msgTypeName = "OBJECT";
                break;

            case TIBEMS_STREAM_MESSAGE:
                msgTypeName = "STREAM";
                break;

            case TIBEMS_MAP_MESSAGE:
                msgTypeName = "MAP";
                break;

            case TIBEMS_TEXT_MESSAGE:
                msgTypeName = "TEXT";
                break;

            default:
                msgTypeName = "UNKNOWN";
                break;
        }

        /* publish sample sends TEXT message, if received other
         * just print entire message
         */
        if (msgType != TIBEMS_TEXT_MESSAGE)
        {
            //printf(" Received %s message:\n",msgTypeName);
            //tibemsMsg_Print(msg);
        }
        else
        {
            /* get the message text */
            status = tibemsTextMsg_GetText(msg,&txt);
            if (status != TIBEMS_OK)
            {
                fail("Error getting tibemsTextMsg text", errorContext);
            }

            printf("%s", txt ? txt : "<text is set to NULL>");
        }

        /* destroy the message */
        status = tibemsMsg_Destroy(msg);
        if (status != TIBEMS_OK)
        {
            fail("Error destroying tibemsMsg", errorContext);
        }
		printf("\033[25;0H"); 
		puts(my_string);
		fflush(stdout);
	}
}

void run() 
{
    tibems_status               status      = TIBEMS_OK;
    tibemsMsg                   msg         = NULL;
    const char*                 txt         = NULL;
    tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
    char*                       msgTypeName = "UNKNOWN";
    
    if (!topic_a) {
        printf("***Error: must specify destination name\n");
       
    }
    
    //printf(" Subscribing to destination: '%s'\n\n",topic_a);
    
    status = tibemsErrorContext_Create(&errorContext);

    if (status != TIBEMS_OK)
    {
        printf(" ErrorContext create failed: %s\n", tibemsStatus_GetText(status));
        exit(1);
    }
    factory = tibemsConnectionFactory_Create();
    if (!factory)
    {
        fail("Error creating tibemsConnectionFactory", errorContext);
    }
    status = tibemsConnectionFactory_SetServerURL(factory,url_a);
    if (status != TIBEMS_OK) 
    {
        fail("Error setting server url", errorContext);
    }
    /* create the connection, use ssl if specified */
    if(sslParams)
    {
        status = tibemsConnectionFactory_SetSSLParams(factory,sslParams);
        if (status != TIBEMS_OK) 
        {
            fail("Error setting ssl params", errorContext);
        }
        status = tibemsConnectionFactory_SetPkPassword(factory,pk_password);
        if (status != TIBEMS_OK) 
        {
            fail("Error setting pk password", errorContext);
        }
    }
    status = tibemsConnectionFactory_CreateConnection(factory,&connection,
                                                      userName,password);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsConnection", errorContext);
    }
    /* set the exception listener */
    status = tibemsConnection_SetExceptionListener(connection,
            onException, NULL);
    if (status != TIBEMS_OK)
    {
        fail("Error setting exception listener", errorContext);
    }
    /* create the destination */
    if (useTopic)
        status = tibemsTopic_Create(&destination,topic_a );
    else
        status = tibemsQueue_Create(&destination,topic_a);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsDestination", errorContext);
    }
    /* create the session */
    status = tibemsConnection_CreateSession(connection,
            &session,TIBEMS_FALSE,ackMode);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsSession", errorContext);
	}
    /* create the consumer */
    status = tibemsSession_CreateConsumer(session,
            &msgConsumer,destination,NULL,TIBEMS_FALSE);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsMsgConsumer", errorContext);
    }
	status = tibemsSession_CreateProducer(session,
            &msgProducer,destination);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsMsgProducer", errorContext);
    }
    /* start the connection */
    status = tibemsConnection_Start(connection);
    if (status != TIBEMS_OK)
    {
        fail("Error starting tibemsConnection", errorContext);
    }
	pthread_t thread1;
	int iret1;
	
	iret1 = pthread_create( &thread1, NULL, RecieveMessages,(void *)"");
	
	
	int nbytes = 100;
	while(1)
	{
		int bytes_read;
		char c = ' ';
		my_string[0] = '\0';
		
		printf("\033[25;1H"); 
		while(c != '\n')
		{
			size_t len = strlen(my_string);
			printf("%s",my_string);
			fflush(stdout);
			my_string[len++] = c;
			my_string[len] = '\0';
			c = getchar();
			
		}
  		//bytes_read = getline (&my_string, &nbytes, stdin);
		my_string = stradd(username, my_string);
		status = tibemsTextMsg_Create(&msg);
        if (status != TIBEMS_OK)
        {
            fail("Error creating tibemsTextMsg", errorContext);
        }
        
        /* set the message text */
        status = tibemsTextMsg_SetText(msg,my_string);
        if (status != TIBEMS_OK)
        {
            fail("Error setting tibemsTextMsg text", errorContext);
        }
		my_string[0] = '\0';
        /* publish the message */
        if (useAsync)
            status = tibemsMsgProducer_AsyncSend(msgProducer, msg, onCompletion, NULL);
        else
            status = tibemsMsgProducer_Send(msgProducer,msg);

        if (status != TIBEMS_OK)
        {
            fail("Error publishing tibemsTextMsg", errorContext);
        }
        //printf("Published message: %s\n",my_string);
        
        /* destroy the message */
        status = tibemsMsg_Destroy(msg);
        if (status != TIBEMS_OK)
        {
            fail("Error destroying tibemsTextMsg", errorContext);
        }
		usleep(10);
	}

            
    /* destroy the destination */
    status = tibemsDestination_Destroy(destination);
    if (status != TIBEMS_OK)
    {
        fail("Error destroying tibemsDestination", errorContext);
    }

    /* close the connection */
    status = tibemsConnection_Close(connection);
    if (status != TIBEMS_OK)
    {
        fail("Error closing tibemsConnection", errorContext);
    }

    /* destroy the ssl params */
    if (sslParams) 
    {
        tibemsSSLParams_Destroy(sslParams);
    }

    tibemsErrorContext_Close(errorContext);
}

int main ()
{
	my_string = (char *) malloc (100 + 1);
	register struct passwd *pw;
  	register uid_t uid;
  	int c;

  	uid = geteuid ();
  	pw = getpwuid (uid);
	
	username = pw->pw_name;
	username = stradd(username, ": ");
  	printlogo();
  	getchar();
	ParseCfgFile("Inchat.cfg");
	printf("\e[1;1H\e[2J");
	fflush(stdout);
	//serverUrl = server_a + port_a;
	run();
  	return 0;
}