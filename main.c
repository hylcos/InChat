
#define _GNU_SOURCE
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#ifdef __linux__
#include <pwd.h>
#elif _WIN32
#include <conio.h>
#include <windows.h>
#endif 


#include "tibemsUtilities.h"

#define MAXBUF 1024
/*-----------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
char							url_a[MAXBUF];
char							port_a[MAXBUF];
char							topic_a[MAXBUF];
char 							Messages[25][MAXBUF];

char * 							username	 = NULL;
char * 							prefix 	 	 = NULL;
char * 							my_string	 = "a";
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


//for $sys.monitor.producer.destroy
tibemsConnectionFactory         d_factory      = NULL;
tibemsConnection                d_connection   = NULL;
tibemsSession                   d_session      = NULL; 
tibemsMsgConsumer               d_msgConsumer  = NULL;
tibemsDestination               d_destination  = NULL;

/*-----------------------------------------------------------------------
 * Needs to go away
 *----------------------------------------------------------------------*/
char*                           numberMsg    = "1";
char*                           userName     = NULL;
char*                           password     = NULL;
char*                           pk_password  = NULL;
int								counter      = 1;

/*----------------------------------------------------------------------- 
 * variables for receiving files
 *----------------------------------------------------------------------*/
char *							remoteUsername 		= NULL;
bool							busyWithFiles 	= false;
char *							fileLocation 	= NULL;

/*-----------------------------------------------------------------------
 * variables for checking who is in the same room as you
 *----------------------------------------------------------------------*/
int 							userCount	 = 0;
int								userCountHad = 0;
char 							userNames[MAXBUF][MAXBUF];
tibemsTopicInfo 				topicInfo	 = NULL;	

/*-----------------------------------------------------------------------
 * Ignore and IgnoreList
 *----------------------------------------------------------------------*/
 char 							ignoreList[MAXBUF][50];
 int 							ignoreCount	 = 0;
/*-----------------------------------------------------------------------
 * Windows Only variables
 *----------------------------------------------------------------------*/
 #ifdef _WIN32
HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
WORD saved_attributes;
#endif

void sendMessage(char * message);
void commandoLocal(const char * message);

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

char* stradd(const char* a, const char* b)
{
    size_t len = strlen(a) + strlen(b);
    char *ret = (char*)malloc(len * sizeof(char) + 1);
    *ret = '\0';
    return strcat(strcat(ret, a) ,b);
}

void sig_handler(int signo)
{
	// tibems_status               status      = TIBEMS_OK;	
	// printf("Caught signal %d\n",signo);
	// sendMessage(" heeft zich afgemeld");
	// if (signo == SIGINT){
	// printf("SIGINT ONtvangen");
		// status = tibemsDestination_Destroy(destination);
		// if (status != TIBEMS_OK)
		// {
			// fail("Error destroying tibemsDestination", errorContext);
		// }

		// /* close the connection */
		// status = tibemsConnection_Close(connection);
		// if (status != TIBEMS_OK)
		// {
			// fail("Error closing tibemsConnection", errorContext);
		// }
	// }
	// exit(1);

}

#ifdef __linux__
char *itoa(long i, char* s, int dummy_radix) {
    sprintf(s, "%ld", i);
    return s;
}
#endif

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
	printf("##############  Created by Hylco Uding #        \xa9            ################");
	printf("################################################################################");
	printf("################################################################################");
	printf("################################################################################");
	printf("###################         Press ENTER to Continue          ###################");
	printf("################################################################################");
	printf("################################################################################");
	printf("##################################################################################");
	
}

void ParseCfgFile(char * filename)
{
	
	FILE *fp = fopen(filename,"r");
	if (fp == NULL)
		fp = fopen(stradd("/etc/inchat/",filename),"r");
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
                                memcpy(url_a,cfline,strlen(cfline));
						else if (i == 1)
                                 memcpy(port_a,cfline,strlen(cfline));
						else if (i == 2)
                                 memcpy(topic_a,cfline,strlen(cfline));
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
			printf("topic = <topic>\n\n");
			printf("The Config file should be placed here: /etc/inchat/Inchat.cfg");
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

void xor_encrypt(const char * string, char * value)
{
	int i, string_length = strlen(string);
	char key = 'K';
	for (i = 0; i<string_length;i++)
	{
		value[i] = string[i] ^ key;
	}
}

void print_bytes(const void *object, size_t size)
{
  size_t i;

  printf("[");
  for(i = 0; i < size; i++)
  {
    printf("%02x", ((const unsigned char *) object)[i] & 0xff);
  }
  printf("]");
}

void addMessage( const char * message)
{
	int i;
	for( i = 23; i > 0; i--) 
	{
		fflush(stdout);
		if(Messages[i-1] != NULL){
			strcpy(Messages[i],Messages[i-1]);
		}
	}
	strcpy(Messages[0],message);
	Messages[0][strlen(message)] ='\0';
	
	
}

void printMessage( const char * message)
{
	int i; 
	#ifdef __linux__
	printf("\e[H\e[2J");
	#elif _WIN64
	system("cls");
	#endif 
	addMessage(message);
	for( i = 0 ; i < 23; i++)
	{
		#ifdef __linux__
		printf("\033[%d;1H",i+2);
		#endif 
		int j;
		for(j = 0;j < strlen(Messages[i]);j++)
		{
			if(Messages[i][j] != '~')
				printf("%c",Messages[i][j]);
			else
				j+=strlen(Messages[i]);
		}	
		#ifdef _WIN64
		printf("\n");
		#endif 
		fflush(stdout);
	}
	fflush(stdout);
	#ifdef __linux__
	printf("\033[H");
	#endif 
	fflush(stdout);
	
}

void changeRoom(char * roomAdress)
{
    tibems_status               status      = TIBEMS_OK;
	sendMessage(stradd(username," has left the room\n"));
	
	
    status = tibemsDestination_Destroy(destination);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgConsumer", errorContext);
	status = tibemsMsgConsumer_Close(msgConsumer);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgConsumer", errorContext);
     status = tibemsTopic_Create(&destination,roomAdress);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgConsumer", errorContext);
    status = tibemsSession_CreateConsumer(session,&msgConsumer,destination,NULL,TIBEMS_FALSE);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgConsumer", errorContext);
	
	status = tibemsMsgProducer_Close(msgProducer);
	if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgProducer", errorContext);
	strcpy(topic_a,roomAdress);
	status = tibemsSession_CreateProducer(session,&msgProducer,destination);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgProducer", errorContext);
	//commandoLocal("/clear");
	sendMessage(stradd(username," has entered the room\n"));
	
	
	#ifdef __linux__
	printf("%c]0;%s%c", '\033', topic_a, '\007');
	#elif _WIN64
	SetConsoleTitle(topic_a);
    #endif
}

void FileTransfer(char * fileName, bool side) ///////////////////////////////////TODDDDOOOOOO///////////////////////////// Moeilijk heden
{
	/*if(side == true) //Sender of the file
	{
		int fd_from = open(fileName, O_RDONLY);
		 while (nread = read(fd_from, buf, sizeof buf), nread > 0)
		{
			
		}
	} 
	else 
	{ //receiver of the file
		int fd_to = open(stradd("received/",to), O_WRONLY | O_CREAT | O_EXCL, 0666);
	}*/
    
}

void commandoRemote(const char * message)
{
    //printMessage(stradd("Remote Commando: ",message));
	char * commando = strtok((char *)message, " /\n~");
	if(strcmp(commando,"cmd" ) == 0)
	{
		commando =  strtok(NULL, " /\n");
		if(strcmp(commando,"sendFile" ) == 0) //NOT DONE
		{
			commando =  strtok(NULL, " /\n");
			if(strcmp(commando,username ) == 0)
			{
				remoteUsername =  strtok(NULL, " /\n");
				if(remoteUsername != NULL){
					fileLocation = strtok(NULL, " /\n");
                    if(fileLocation != NULL)
                    {
                        printMessage(stradd(stradd(stradd("Do you want to recieve:  ",fileLocation)," from "),remoteUsername));
                        printMessage(("If so type: \" /receiveFile accept\" or else \" /receiveFile deny \""));
                    }
                }
			}					  
		}
        else if (strcmp(commando, "receiveFile")==0) //NOT DONE
        {
            printMessage(stradd("Remote Commando: ",commando));
            commando =  strtok(NULL, " /\n");
            if(strcmp(commando, username)==0)
            {
                 printMessage(stradd("Remote 2Commando: ",commando));
                commando =  strtok(NULL, " /\n");
                if(strcmp(commando,"accept" )== 0)
			    {
					printMessage(stradd(remoteUsername," accepted your file transfer"));
                    changeRoom(stradd("InChat.FileTransfer.",remoteUsername));
					busyWithFiles = true;
			    }
		 	    else if(strcmp(commando,"deny")== 0)
			    {
					printMessage(("If so type: \" /receiveFile accept\" or else \" /receiveFile deny \""));
			    }
            }
        }
		else if (strcmp(commando, "whoison")==0)
        {
            char * whoasks =  strtok(NULL, " /\n~");
			sendMessage(stradd(stradd(stradd(stradd(stradd("/cmd ","iamon "),whoasks), " "),username), " ~"));
        }
		else if (strcmp(commando, "iamon")==0)
        {
			 char * whoasks =  strtok(NULL, " /\n~");
			 if(strcmp(whoasks,username)==0)
			 { 
				char * whosays =  strtok(NULL, " /\n~");
				strcpy(userNames[userCountHad],whosays);				
				userCountHad+=1;
				
			 }
		}
		
	}
	else 
	{
		return;
	}
}

void commandoLocal(const char * message) //Commando's quit/help/changeUsername/chnageRoom/room/rooms/users/clear/ignore/ignoreList
{
	char * commando = strtok((char *)message, " /\n");
	char * wow = " quit";
	if(commando == NULL)
		return;
	if(strcmp(commando,"quit" ) == 0)
	{
		sendMessage("/cmd userCountChanged down");
		exit(1);
	}
	else if (strcmp(commando,"help") == 0 || strcmp(commando,"h") == 0)
	{
		addMessage("===========================================");
		addMessage("ignoreList : Laat de lijst zien van emsnen die je negeert");
		addMessage("ignore <NAME> : Laat geen berichten van deze persoon meer zien");
		addMessage("clear : Maakt het scherm leeg");
		addMessage("users/whoison :  Laat zien welke users in jou kamer online zijn");
		addMessage("rooms	:  Laat zien in welke public kamers er al users zitten");
		addMessage("room/pwd : Laat zien in welke kamer je zit");
		addMessage("changeRoom/cr <ROOM_NAME> : Wisselen tussen chat ruimtes");
		addMessage("changeUsername/cu <NAME> : gebruikersnaam veranderen");
		addMessage("quit : Programma afsluiten");
		addMessage("help/h : Dit scherm");
		addMessage("Commando's zijn: ");
		printMessage("===========================================");
	}
	else if(strcmp(commando,"changeUsername" ) == 0 || strcmp(commando,"cu" ) == 0 )
	{
		commando =  strtok(NULL, " /\n");
		if(commando != NULL)
		{
			sendMessage(stradd(username," heeft zich afgemeld!\n"));
			strcpy(username,commando);
			prefix = stradd(username, ": ");
			printMessage("Username Changed");
			sendMessage(stradd(username," heeft zich aangemeld!\n"));
		}
		else
		{
			printMessage("Give a new username");
		}
	}
	else if(strcmp(commando,"sendFile" ) == 0) //NOT DONE
	{
		remoteUsername =  strtok(NULL, " /\n");
		if(remoteUsername != NULL)
		{
			fileLocation = strtok(NULL, " /\n");
			if(fileLocation != NULL)
				sendMessage(stradd(stradd("/cmd ",stradd("sendFile",stradd(" ", stradd(stradd(remoteUsername, " "),stradd(stradd(username," "),fileLocation)))))," ~"));
			else
				printMessage("Send file command syntax is: /sendFile <<username>/accept/deny> <file>");
		}
		else
			printMessage("Send file command syntax is: /sendFile <to> <file>");
	}
    else if(strcmp(commando,"receiveFile" ) == 0)//NOT DONE
    {
        char * awnser =  strtok(NULL, " /\n");
        if(awnser != NULL)
        {
            if(strcmp(awnser,"accept") == 0)
            {
                sendMessage(stradd(stradd(stradd(stradd("/cmd ","receiveFile "),remoteUsername), " accept "), "~"));
                changeRoom(stradd("InChat.FileTransfer.",username));
				busyWithFiles = true;
            }
            else if(strcmp(awnser,"deny") == 0)
                sendMessage(stradd(stradd(stradd(stradd("/cmd ","receiveFile "),remoteUsername), " deny " ), "~"));
        }
    }
    else if(strcmp(commando,"changeRoom" ) == 0  || strcmp(commando,"cr") == 0)
    {
        
        commando =  strtok(NULL, " /\n");
		
        if(commando != NULL)
           changeRoom(stradd("InChat.Rooms.Public.",commando));
		else
			printMessage("Give a new room name");
        
    }
	else if(strcmp(commando,"room" ) == 0 || strcmp(commando,"pwd") == 0)
    {
        char topic_copy[MAXBUF];
		strcpy(topic_copy,topic_a);
        char * server = strtok((char *)topic_copy, ".");
		char * type =  strtok(NULL, ".");
		char * type2 =  strtok(NULL, ".");
        char * roomname =  strtok(NULL, ".");
		printMessage("=====================================");
		printMessage(stradd(stradd(stradd("You are in the ",type2), " room "), roomname));
		printMessage("=====================================");
	}
	else if(strcmp(commando,"rooms" ) == 0 )
    {
		tibems_status              		status      = TIBEMS_OK;
		tibemsAdmin  					admin		= NULL;
		tibemsCollection                topicInfos  = NULL;
		tibemsTopicInfo                 topicInfo	= NULL;
		tibems_int                  	count;
		char                       		nameBuf[MAXBUF];
		char							intBuf[MAXBUF];
		
		status = tibemsAdmin_Create(&admin,url_a,"admin","admin",sslParams);
		if (status != TIBEMS_OK)
			fail("Creating a new Admin", errorContext);
			
		status = tibemsAdmin_GetTopics(admin, &topicInfos, "InChat.Rooms.Public.>", TIBEMS_DEST_GET_NOTEMP);
		if (status != TIBEMS_OK)
			fail("getting the topics", errorContext);status = tibemsAdmin_GetTopics(admin, &topicInfos, "InChat.Rooms.Public.>", TIBEMS_DEST_GET_NOTEMP);
		
		
		status = tibemsCollection_GetFirst(topicInfos, (&topicInfo));
		if (status != TIBEMS_OK)
		{
			fail("Error getting first queue in collection", errorContext);
		}

		status = tibemsTopicInfo_GetName(topicInfo, nameBuf, sizeof(nameBuf));
		if (status != TIBEMS_OK)
		{
			fail("Error getting first topic name", errorContext);
		}
		status = tibemsTopicInfo_GetSubscriberCount(topicInfo ,&count);
		if (status != TIBEMS_OK)
			fail("getting the topics", errorContext);
		
		
		addMessage("======================================================");
		if (count > 0)
		{
			itoa(count, intBuf,10);
			addMessage(stradd(stradd(nameBuf, "  ::::: Users Online: "),intBuf));	
		}
		while (status != TIBEMS_NOT_FOUND)
		{
			status = tibemsCollection_GetNext(topicInfos, &topicInfo);
			if (status == TIBEMS_NOT_FOUND)
			{
				status = TIBEMS_OK;
				break;
			}
			if (status != TIBEMS_OK)
			{
				fail("Error getting next topic in collection", errorContext);
			}

			status = tibemsTopicInfo_GetName(topicInfo, nameBuf, sizeof(nameBuf));
			if (status != TIBEMS_OK)
			{
				fail("Error getting next topic name", errorContext);
			}
			
			status = tibemsTopicInfo_GetSubscriberCount(topicInfo ,&count);
			if (status != TIBEMS_OK)
				fail("getting the topics", errorContext);
			if (count > 0)
			{
				itoa(count, intBuf,10);
				addMessage(stradd(stradd(nameBuf, "  ::::: Users Online: "),intBuf));	
			}
			tibemsTopicInfo_Destroy(topicInfo);
		}
		addMessage("All the Public InChat Rooms");
		printMessage("======================================================");
	status = tibemsAdmin_Close(admin);
	if (status != TIBEMS_OK)
		fail("Error closing tibemsAdmin connection", errorContext);
	
	tibemsCollection_Destroy(topicInfos); 
	}
	else if(strcmp(commando,"users") == 0 || strcmp(commando,"whoison") == 0)
    {
		
		sendMessage(stradd(stradd(stradd("/cmd ","whoison "),username), "  ~"));
		char * dest = NULL;
		usleep(300000);
		int i = 0;
		printMessage("-------------------------------------------------------");
		
		for(i = 0 ; i < userCountHad;i++)
		{
			printMessage(userNames[i]);
		}
		if(userCountHad == 0){
			printMessage("NOBODY IS ONLINE");
		}
		printMessage("Users who are online: ");
		printMessage("-------------------------------------------------------");
		userCountHad = 0;
        
    }
	else if (strcmp(commando,"clear") == 0)
	{
		int i;
		for( i = 0 ; i < 23; i++)
		{
			strcpy(Messages[i],"");
		}
		printMessage("");
	}
	else if (strcmp(commando,"ignore") == 0)
	{ 
		bool inList = false;
		int indexInList;
		commando =  strtok(NULL, " /\n");
		
        if(commando != NULL)
		{
			int i;
			for( i = 0 ; i < ignoreCount && !inList; i++)
			{
				if ( strcmp(commando,ignoreList[i]) == 0)
				{
					inList = true;
					int j;
					ignoreCount--;
					for( j = i ; j < ignoreCount; j++)
					{
						strcpy(ignoreList[j],ignoreList[j+1]);
					}
					addMessage("===========================================");
					addMessage(stradd(commando, " Deleted to the ignorelist "));
					printMessage("===========================================");
				}
			}
			if (!inList){
				strcpy(ignoreList[ignoreCount],commando);
				addMessage("===========================================");
				addMessage(stradd(ignoreList[ignoreCount], " added to the ignorelist "));
				printMessage("===========================================");
				ignoreCount++;
			}
		}
		else
		{
			printMessage("Give the user name of the person you want to Ignore");
		}
	}
	else if (strcmp(commando,"ignoreList") == 0)
	{
		int i;
		printMessage("===========================================");
		for( i = 0 ; i < ignoreCount; i++)
		{
			addMessage(ignoreList[i]);
		}
		printMessage("List of people you are ignoring");
		printMessage("===========================================");
	}
	else
	{
		printMessage("Unknown Commando");
	}
	
}

void * recieveMessage(void * ptr)
{
	const char*                 txt         = NULL;
	
	
	while(1){
		tibems_status               status      = TIBEMS_OK;
		tibemsMsg                   msg         = NULL;

		tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
		char*                       msgTypeName = "UNKNOWN";
		
		status = tibemsMsgConsumer_Receive(msgConsumer,&msg);
        if (status != TIBEMS_OK)
        {
            if (status == TIBEMS_INTR)
            {
            }
            //fail("Error receiving message", errorContext);
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

            //printMessage(txt);
        }
		
		//*_txt = *txt;
        /* destroy the message */
       //
		
        if (status != TIBEMS_OK)
        {
            fail("Error destroying tibemsMsg", errorContext);
        } 
		char value[1024];
		
		xor_encrypt((char *)txt,value);
		
		if(strchr(value,'@' )!= NULL){
			char * _txt = strstr((char *)value,"@" );
			if(strncmp(_txt, stradd("@",username),strlen(username)+1) == 0)
				printMessage(value);
		}
		else if(strstr(value,"/cmd") != NULL){
			commandoRemote(value);
		}
		else
		{
			bool inList = false;
			int i;
			for( i = 0 ; i < ignoreCount; i++)
			{
				if(strncmp(value,ignoreList[i],strlen(ignoreList[i])) == 0)
					inList = true;
			}
			if(!inList)
				printMessage(value);
	
		}
		status = tibemsMsg_Destroy(msg);
		
		#ifdef __linux__
		
		#elif _WIN64
		FLASHWINFO info = { sizeof(info), GetConsoleWindow(), FLASHW_TIMERNOFG | FLASHW_TRAY, 3, 0 };
		FlashWindowEx(&info);
		#endif
	}			  
}

void * monitorMessages(void * ptr)
{
	
	// tibems_status               status      = TIBEMS_OK;
    // tibemsMsg                   msg         = NULL;
    // tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
	// tibemsMsgField				field;
	// tibemsMsgEnum  				enumeration;
	// const char * 				name;
	
	// while(1){
		
		// status = tibemsMsgConsumer_Receive(d_msgConsumer,&msg);
		// #ifdef __linux__
		// printf("\e[H\e[2J");
		// #endif 


        // status = tibemsMsg_GetBodyType(msg,&msgType);
        // if (status != TIBEMS_OK)
            // fail("Error getting message type", errorContext);

 

       // status = tibemsMsg_GetPropertyNames(msg,&enumeration);
   
		// while((status = tibemsMsgEnum_GetNextName(enumeration,&name)) == TIBEMS_OK)
		// {
			// status = tibemsMsg_GetProperty(msg,name,&field);
			// if (status != TIBEMS_OK)
				// fail("Error trying to get property by name", errorContext);

			// if(strcmp(name,"conn_username")==0)
			// {
				// tibemsMsgField * fld = &field;
				// printMessage(stradd(fld->data.utf8Value, " heeft zich afgemeld"));
				// //userCount--;
				// printf("\n");
			// }
		
    	// }

    // tibemsMsgEnum_Destroy(enumeration);
    
	// }
}

void sendMessage(char * message)
{	
		int i= 0;
		while(message[i]!='\0')
			{
				   if(message[i]=='\n')
				   {
					   message[i]='~';
				   }  
				   i++; 
			 }
		char value[1024];
		xor_encrypt(message,value);
			
		
	 	tibems_status               status      = TIBEMS_OK;
    	tibemsMsg                   msg         = NULL;
		status = tibemsTextMsg_Create(&msg);
        if (status != TIBEMS_OK)
        {
            fail("Error creating tibemsTextMsg", errorContext);
        }
        
        /* set the message text */
        status = tibemsTextMsg_SetText(msg,value); 
        if (status != TIBEMS_OK)
        {
            fail("Error setting tibemsTextMsg text", errorContext);
        }
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
		#ifdef __linux__
		printf("\033[24;1H"); 
		#endif 
}

void run() 
{
    tibems_status               status      = TIBEMS_OK;
    const char*                 txt         = NULL;
    tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
    char*                       msgTypeName = "UNKNOWN";
    
    status = tibemsErrorContext_Create(&errorContext);

    if (status != TIBEMS_OK)
    {
        printf(" ErrorContext create failed: %s\n", tibemsStatus_GetText(status));
        //exit(1);
    }
    factory = tibemsConnectionFactory_Create();
    if (!factory)
        fail("Error creating tibemsConnectionFactory", errorContext);
	
    status = tibemsConnectionFactory_SetServerURL(factory,url_a);
	 if (status != TIBEMS_OK) 
        fail("Error setting server url", errorContext);
	
    if(sslParams)
    {
        status = tibemsConnectionFactory_SetSSLParams(factory,sslParams);
		if (status != TIBEMS_OK)
            fail("Error setting ssl params", errorContext);
	
        status = tibemsConnectionFactory_SetPkPassword(factory,pk_password);
        if (status != TIBEMS_OK) 
            fail("Error settin24g pk password", errorContext);
    }
    status = tibemsConnectionFactory_CreateConnection(factory,&connection,"admin","admin");
	//status = tibemsConnectionFactory_CreateConnection(factory,&d_connection,"admin","admin" );
    if (status != TIBEMS_OK)
        fail("Error creating tibemsConnection", errorContext);
	
    status = tibemsConnection_SetExceptionListener(connection, onException, NULL);
	//status = tibemsConnection_SetExceptionListener(d_connection, onException, NULL);
    if (status != TIBEMS_OK)
        fail("Error setting exception listener", errorContext);
	
  
    status = tibemsTopic_Create(&destination,topic_a );
   if (status != TIBEMS_OK)
        fail("Error creating tibemsDestination", errorContext);
	// status = tibemsTopic_Create(&d_destination,"$sys.monitor.producer.destroy");
    // if (status != TIBEMS_OK)
        // fail("Error creating tibemsDestination", errorContext);
	
    status = tibemsConnection_CreateSession(connection,&session,TIBEMS_FALSE,ackMode);
	 if (status != TIBEMS_OK)
        fail("Error creating tibemsSession", errorContext);
	// status = tibemsConnection_CreateSession(d_connection,&d_session,TIBEMS_FALSE,ackMode);
    // if (status != TIBEMS_OK)
        // fail("Error creating tibemsSession", errorContext);
	
    status = tibemsSession_CreateConsumer(session,&msgConsumer,destination,NULL,TIBEMS_FALSE);
	if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgConsumer", errorContext);
	// status = tibemsSession_CreateConsumer(d_session,&d_msgConsumer,d_destination,NULL,TIBEMS_FALSE);
    // if (status != TIBEMS_OK)
        // fail("Error creating tibemsMsgConsumer", errorContext);
	
	status = tibemsSession_CreateProducer(session,&msgProducer,destination);
    if (status != TIBEMS_OK)
        fail("Error creating tibemsMsgProducer", errorContext);
	
    status = tibemsConnection_Start(connection);
	  if (status != TIBEMS_OK)
        fail("Error starting tibemsConnection", errorContext);
	
    // status = tibemsConnection_Start(d_connection);
    // if (status != TIBEMS_OK)
        // fail("Error starting tibemsConnection", errorContext);
	
	pthread_t thread1;
	int iret1 = pthread_create( &thread1, NULL, recieveMessage,(void *)""); 
	
	// pthread_t thread2;
	// int iret2 = pthread_create( &thread2, NULL, monitorMessages,(void *)""); 
	#ifdef _WIN32	
	commandoLocal("/users");
	#endif
	sendMessage(stradd(stradd(username," heeft zich aangemeld!"),"~"));
	
	sendMessage("/cmd userCountChanged up ~");
	size_t nbytes = 1024;
	while(1)
	{
		
		int bytes_read;
		
		#ifdef __linux__
		char  * my_string;	
		printf("\033[H"); 
		bytes_read = getline(&my_string,&nbytes,stdin);
		#elif _WIN32
		char my_string[MAXBUF];
		fgets(my_string, sizeof(my_string), stdin);
		#endif
		if(my_string[0] == '/')
			commandoLocal(my_string);
		else 
		{
		
			sendMessage(stradd(prefix, my_string));
			#ifdef __linux__
			printf("\033[2K");
			#endif 
			fflush(stdout);
		}
	}

            
    status = tibemsDestination_Destroy(destination);
    if (status != TIBEMS_OK)
        fail("Error destroying tibemsDestination", errorContext);

    status = tibemsConnection_Close(connection);
    if (status != TIBEMS_OK)
        fail("Error closing tibemsConnection", errorContext);

    if (sslParams) 
        tibemsSSLParams_Destroy(sslParams);

    tibemsErrorContext_Close(errorContext);
}

int main (int argc, char * argv[])
{

	if(argc == 1)
	{
		//system("gnome-terminal --geometry=80x24 -e \"./Main true\" ");
		//printf("\033[2J");
		//exit(1);
	}	
	
	#ifdef __linux__
	username = getpwuid(geteuid ())->pw_name;
	#elif _WIN64
	username = getenv("USERNAME");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	#endif
	prefix = stradd(username, ": ");
	
  	printlogo();
  	getchar();
	ParseCfgFile("Inchat.cfg");
	#ifdef __linux__
	printf("\033[2J");
	printf("%c]0;%s%c", '\033', topic_a, '\007');
	#elif _WIN64
	SetConsoleTitle(topic_a);
	#endif 
	fflush(stdout);
	
	run();
  	return 0;
}
