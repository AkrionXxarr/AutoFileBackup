# AutoFileBackup
Project History - August 2011

This project marks the second time that I put multithreading and socketing to some serious use (the first time being Akicomm), as well as the first time I made good use of a library (boost for file backup). It's also the first project to be put to actual use for a good length of time. This was made for a friend who was running a Minecraft server at the time and after losing a map I wrote this system to backup future maps. 

The system runs 2 threads, 3 if operating in online mode. A thread for handling the file operations (backup, restore, etc.), a thread for handling user input, and a thread for managing a connected client. The user would set up a config file that would specify such things as whether or not to use online mode, the path to a server-start batch file, the map source path and backup destination path, the name of the backup directory, the interval of the backups, and how many unique backups to maintain. A backup log file is maintained which shows the date and time of backup attempts, restore attempts, and whether or not those attempts were successful.

The backup system would copy the files to one of X unique folders sequentially (e.g. 10 backups at 30 minute intervals gives you a backup history of 5 hours). It also allowed the user to command a forced backup, which would go into a unique folder untouched by the auto backup system. The user was also capable of instructing the the system to restore a backup, show status information, and even instruct it to execute a batch file that would be used to start the server. A remote client was also capable of executing the same instructions, with the addition of requesting the server's backup log.

Demonstration video:<br>
https://www.youtube.com/watch?v=NguAWKe64-k
