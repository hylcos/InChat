#  What is InChat
In chat is basically a program that connects to a local/remote [EMS server.](http://www.tibco.com/products/automation/enterprise-messaging/enterprise-message-service) 

* [Setup](#setup)
  * [Server Side](#server-side)
  * [Client Side](#client-side)
    * [Linux](#Linux)
      * [Compiling From Source](#compiling-from-source)
      * [Downloading the RPM ](#downloading-the-RPM)
    * [Windows](#windows)
      * [Compiling From Source](#compiling-from-source-1)
      * [Install](#install)
* [Config File](#config-file)
* [Commando's](#commandos)
  * [Commando's where you'll have to accept stuff](#commandos-where-youll-have-to-accept-stuff) 
    
##  Setup
There are (like almost every programs) actions to be done to be actually able to run the program
### Server Side
You need a tibco ems server running on one of your servers. Use tibemsadmin or any other Tibco EMS Administration Tool to login to the server.

Make a new topic called InChat.> with the commando:
>create topic InChat.>

The greater then symbol that users can generate dynamic rooms like InChat.Rooms.Public.Main2. If this topic isn't created nobody will be able to make new room.

You'll also need a tibco user called admin with the password admin for login from the program. You can use your own users but you'll have to change the source code for that.

### Client Side
#### Linux
##### Compiling from source
Download the latest release version from this program.
Unzip the downloaded file and add this to your .Bash_profile

> LD_LIBRARY_PATH=/home/udingh/Projects/InChat/lib/Linux/64

> export LD_LIBRARY_PATH

Run make (If it doesn't compile send me a message)
##### Downloading the RPM 
(Not Done Yet)
#### Windows
[This is the latest Windows Released Version](https://github.com/hylcos/InChat/releases/tag/v0.31-alpha-Windows)
##### Compiling from source
Download the latest release version from this program.

Unzip the downloaded file and add (LOCATION TO GIT)/bin to your PATH.

Run make (If it doesn't compile send me a message)
##### Install
Download the latest release version from this program.

Unzip the downloaded file and add (LOCATION TO GIT)/bin to your PATH.
> setx path "%path%;D:\InChat\bin"

## Config File
Added in the RPM(Not Done Yet) and release versions is a InChat.cfg. This config file contains three basic settings
* url  : which is the url on which the EMS Server is
* port : which is the port number on which the EMS Server runs. (Default number is 7222)
* topic: the room you want to start in. (Default topic address is InChat.Rooms.Public.Main)

If you want to enter a private room you will have to change the topic to InChat.Rooms.Private. (PRIVATE_ROOM_NAME)
You have to change to topic here because the /changeRoom commando only transfers you to Public rooms.

## Commando's
There are several different basic commandos in InChat, these are all executed with /(COMMANDO) (PARAMETERS):
* changeUsername / cu (NAME): This commando lets you change username.
* exit                      : exits the program
* help                      : show the help menu with all the commandos and parameters
* changeRoom/cr (ROOM NAME)    : lets you change rooms. Works only with public rooms. If you want to switch to a private room, you'll have to change the InChat.cfg file and restart the program
* sendFile                  :
* receiveFile               :
* users                     : Shows you which users are online
* pwd / room                : Show you in which room you are
* clear                     : Clears the chat screen for you
* rooms                     : Shows you all the available Public rooms that you can join
* ignore (USERNAME)         : Adds the username that is given to a list of people that should be ignored 
* ignoreList                : Show the list of people being ignored

### Commando's where you'll have to accept stuff
Some functions of this program need two people. But what if one person wanted to execute a function but the other person doesn't want to do that. Here is how all that is taken care of in this program.

You have several commando's like:
 * privateChat (username)   : Starts a private Chat with you and another person
 * 
 
All these commando's have one shared thing, they can be accepted/denied or canceled. 
 * accept (commandoName)    : Accepts the pending request   (Can only be done by the receiver of the request)
 * deny (commandoName)      : Denies the pending request  (Can only be done by the receiver of the request)
 * cancel (commandoName)    : Cancels the request (Can only be done by the sender of the request)
 
The commandoName could be privateChat or one of the other commando's where you'll need two people for.
