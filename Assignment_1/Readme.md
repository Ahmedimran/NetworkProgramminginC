PROGRAMMING PORTION :  Read Beej’s Guide to Network Programming thoroughly and modify the 
talker/listener (talker.c and listener.c) according to the following specifications: (Listerer 
and talker are communicating using UDP socket) 
----------------------------Listener--------------------------- 
 The listener should run in continuous mode, i.e. it should handle one talker after 
another. (Make Listener Iterative Server ) 
 The listener will bind on port # 4500 
 The listener should maintain a log, named “TalkerMsgLog.txt” of all the talkers and 
their messages. The log should contain the flowing information: 
o IP address of the talker 
o Port number of the talker 
o Message sent by the talker 
 When the listener receives a message from the talker, it saves the talkers message in 
the log and also sends a reply to the talker containing the following welcome 
message: 
“Yes Welcome: TALKER-IP-ADDRESS, TALKER- PORTNO” 
(Extract from talker message) 
 When the listener receives a message from the Talker, it saves the Talker’s message 
in the log “FirstDataMsg: TALKER-IP-ADDRESS, TALKER-PORTNO: This is first 
formal data messages from Talker” and also sends a reply to the talker containing the 
following message: 
“IDMsg: Your First Data Message is received, Please send your name
and ID No” 
 When the listener receives a message from the Talker, it saves the 
Talker’s message in the log “ IDMsgReply :TALKER-IP-ADDRESS, 
TALKER-PORTNO: I am Muhammad-Bilal – SU-04-020 ” and also 
sends a reply to the talker containing the following message: 
“TermMsg: you’re Name and ID is logged on Listener, comm terminated” 
 Listener then waits for some new talker ….. and doesn’t terminates 
----------------------------Talker-------------------------------- 
 The talker takes the following command line arguments from the user: 
o IP address of the Listener 
o Port No of Listener 
 The command line arguments entered by the user will be: ./talker 127.0.0.1 4500 
 Create a Log file “ListenerMsgLog.txt” 
 Talker formats and Message and sends to Listener (on port and IP taken as command 
line arguments ) “ Is there some Listener exist on this port”. 
 When the talker receives the welcome message as a reply from the listener, it displays 
the message on screen and writes in Log file. 
 Talker formats and Message and sends to Listener “ This is first formal data messages 
from Talker” 
 When the Talker receives the IDMsg message from the listener, it displays the 
message on screen and writes in Log file. 
 Talker formats and Message and sends to Listener “IDMsgReply: I am MuhammadBilal – SU-04-020 “. 
 When the Talker receives the TermMsg message from the listener, it displays the 
message on screen and writes in Log file. Terminates the communication and exit.
