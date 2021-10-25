# OpenJournal

OpenJournal is a simple journal, note-taking, and assistant application supporting Markdown syntax and Markdown rendering thanks to [qmarkdowntextedit](https://github.com/pbek/qmarkdowntextedit). Write your thoughts, never forget a meeting or simply keep a daily diary.

## Supported features
- [x] Alarms
- [x] Live markdown renderer
- [x] Markdown highlinkting
- [x] Latex renderer
- [x] Privacy mode
- [x] Journal exportation in pdf
- [x] Tray integration
- [x] Remote journal access
- [x] Secure journal
- [ ] Keyword searching
- [ ] Traduction
- [ ] Start at boot

## Screenshot

![preview](http://gallois.cc/images/openjournal.png)


## Installation
OpenJournal binary is available for Linux, Windows (installer and portable version), and MacOs.

## Command
OpenJournal supports commands that can be directly entered inside the note:
* Set an alarm `setAlarm(hh:mm,message);`

## Remote access
OpenJournal can access remotely created journals that are hosted on a mysqsl server (locally, do not expose database to the internet!).

### Server creation
```
sudo apt install mariadb-server
sudo mysql_secure_installation
mysql -u root -p
CREATE USER 'username'@'hostname' IDENTIFIED BY 'password';
GRANT ALL ON *.* TO 'username'@'%' IDENTIFIED BY 'password';
FLUSH PRIVILEGES;
EXIT;
```

### Connection
* Go to File -> Connect to a remote planner.
* Enter `username@hostname:port`
* Enter `journalname@password`. If *username* has write privilege on the server, *journalname* database will be created if it doesn't exist, else *journalname* has to be created on the server by a user with write privilege.

