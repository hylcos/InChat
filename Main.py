from pyems import DestinationType
from gi.repository import Gtk
import pyems
import sys
import thread
import argparse
class Handler:
	def onDeleteWindow(self, *args):
		sys.exit()
	def onSendButton(self,button):
		__msg = builder.get_object("msg").get_text()
		if __msg == "quit":
			sys.exit()
		if __msg[0] == '/':
			commando(__msg[1:])
		else:
			msg = pyems.Message(__msg)
			prod.sendMessage(msg)
		builder.get_object("msg").set_text("")

def printConsole(txt):
	textbuffer.insert(textbuffer.get_end_iter(),str(txt))
	adj = builder.get_object("scrolledwindow1").get_vadjustment()
	adj.set_value( adj.get_upper() - adj.get_page_size() )
def printLogo():
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("             _________ _        _______           _______ _________             \n");
	printConsole("             \\__   __/( (    /|(  ____ \\|\\     /|(  ___  )\\__   __/            \n");
	printConsole("                 ) (   |  \\  ( || (    \\/| )   ( || (   ) |   ) (               \n");
	printConsole("                 | |   |   \\ | || |      | (___) || (___) |   | |               \n");
	printConsole("                 | |   | (\\ \\) || |      |  ___  ||  ___  |   | |               \n");
	printConsole("                 | |   | | \\   || |      | (   ) || (   ) |   | |               \n");
	printConsole("              ___) (___| )  \\  || (____/\\| )   ( || )   ( |   | |               \n");
	printConsole("              \\_______/|/    )_)(_______/|/     \\||/     \\|   )_(                \n");
	printConsole("                                                                                \n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
	printConsole("################################################################################\n");
		
'''COMMANDOS'''
def commando(msg_):
	msg_ = msg_.split(" ")
	isCommando = False
	for cmd in commandos:
		if cmd == msg_[0]:
			commandos[msg_[0]](msg_[1:])
			isCommando = True
	if not isCommando:
		printConsole(msg_[0] + " isn't a commando")
def room(a):
	roomspt = room.split(".")
	printConsole("_-_-_-_-_-_-_-_-_-_-_-_-_-__-_-_-_-_-_-_-_-_-_-_-_-_-_"+
				"_-_-_-_-_-_-_-_-_-_-_-_-_-__-_-_-_-_-_-_-_-_-_-_-_-_-__-"+
				"_-_-_-_-_-_-_-_-_-_-_-_-_\nYou are "+roomspt[2]+" room "+ roomspt[3] +" \n_-_-_-_-_-_"+
				"-_-_-_-_-_-_-_-__-_-_-_-_-_-_-_-_-_-_-_-_-__-_-_-_-_-_-_-"+
				"_-_-_-_-_-_-__-_-_-_-_-_-_-_-_-_-_-_-_-__-_-_-_-_-_-_-_-_-"+
				"_-_-_-_-_\n")
def help(a):
	printConsole("===========================================\n");
	printConsole("Commando's zijn: \n");
	printConsole("ignoreList : Laat de lijst zien van emsnen die je negeert\n");
	printConsole("ignore <NAME> : Laat geen berichten van deze persoon meer zien\n");
	printConsole("clear : Maakt het scherm leeg\n");
	printConsole("users/whoison :  Laat zien welke users in jou kamer online zijn\n");
	printConsole("rooms	:  Laat zien in welke public kamers er al users zitten\n");
	printConsole("room/pwd : Laat zien in welke kamer je zit\n");
	printConsole("changeRoom/cr <ROOM_NAME> : Wisselen tussen chat ruimtes\n");
	printConsole("changeUsername/cu <NAME> : gebruikersnaam veranderen\n");
	printConsole("quit : Programma afsluiten\n");
	printConsole("version : Laat de InChat versie zien\n");
	printConsole("info: Laat informatie over InChat zien\n");
	printConsole("help/h : Dit scherm\n");
	printConsole("===========================================\n");
def changeRoom(a):
	print"l"
def clear(a):
	textbuffer.set_text("")
def rooms(a):
	print(a)
def changeUsername(a):
	username = a[0]

	
'''MAIN RECEIVE LOOP'''
def receiveLoop():
	while(1):
		_msg = cons.receiveMessageTimeout(1)
		while Gtk.events_pending():
				Gtk.main_iteration()
		if _msg != 50:
			printConsole(str(_msg) + "\n")

commandos = {"room":room,"help":help,"clear":clear,"changeRoom":changeRoom,"changeUsername":changeUsername,"rooms":rooms}
			
if __name__ == "__main__":
	'''Argument Parser'''
	parser = argparse.ArgumentParser()
	parser.add_argument("username", help ="Username")
	parser.add_argument("-s","--server",help ="With server should a connection be made with",default="itp035")
	parser.add_argument("-p", "--port", help ="With which port should be a connection made", default='7222')
	parser.add_argument("-t", "--topic", help ="With which topic should be a connection made with? (Default is Main)" , default='Main')
	parser.add_argument("--private", help ="Private?",action="store_true")
	args = parser.parse_args()
	
	'''SETTINGS'''
	server = args.server +":"+args.port
	username = args.username
	if args.private:
		room = "InChat.Rooms.Private." + args.topic
	else:
		room = "InChat.Rooms.Public." + args.topic
	print server + ' ' + username + ' '	+room
	
	
	'''GTK CODE'''
	builder = Gtk.Builder()
	builder.add_from_file("ui.glade")
	builder.connect_signals(Handler())
	window = builder.get_object("window1")
	window.show_all()
	builder.get_object("Console").set_editable(False)
	textbuffer = builder.get_object("textbuffer1")
	
	'''EMS CODE'''
	conn = pyems.Connection(server,"admin","admin",args.username)
	sess = conn.createSession()
	dest = pyems.Destination(room, DestinationType.TIBEMS_TOPIC)
	prod = sess.createProducer(dest)
	cons = sess.createConsumer(dest)
	conn.start()
	
	printLogo()
	'''STARTING THREADS'''
	thread.start_new_thread(receiveLoop,())
	Gtk.main()
	
	while 1:
		while Gtk.events_pending():
				Gtk.main_iteration()	
