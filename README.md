##  What is InChat
In chat i basicly a program that connects to a local/remote [EMS server.](http://www.tibco.com/products/automation/enterprise-messaging/enterprise-message-service) 


##  Setup
There are (like almost every programs) actions to be done to be actually able to run the program
### Server Side
You need a tibco ems server running on one of your servers. Use tibemsadmin or any other Tibco EMS Administration Tool to login to the server.

Make a new topic called InChat.> with the commando:
>create topic InChat.>

The greater then symbol that users can generate dynamic rooms like InChat.Rooms.Public.Main2. If this topic isn't created nobody will be able to make new room.

You'll also need a tibco user called admin with the password admin for login from the program. You can use your own users but you'll have to change the source code for that.

### Client Side

#### Compiling from source
Download the latest release version from this program.

Unzip the downloaded file and add (LOCATION TO GIT)/lib/linux/64 to your PATH.

Run make (If it doesn't compile send me a message)
#### Downloading the RPM (Not Done Yet)
