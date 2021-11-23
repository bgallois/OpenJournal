# OpenJournal

 ![Test](https://github.com/bgallois/openjournal/workflows/Tests/badge.svg) ![Clang Format](https://github.com/bgallois/openjournal/workflows/Clang%20Format/badge.svg) ![Continous Build](https://github.com/bgallois/openjournal/workflows/Continous%20Builds/badge.svg) [![License: GPL v2](https://img.shields.io/badge/License-GPLv2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

OpenJournal is a simple journal, note-taking, and assistant application supporting Markdown syntax and Markdown rendering thanks to [qmarkdowntextedit](https://github.com/pbek/qmarkdowntextedit). You can write your thoughts, to-do list and never forget a meeting by setting visual and audible alerts.

## Technology

OpenJournal uses a database to store and retrieve user notes. This flexible approach offers several advantages. Local journals can be created using [SQlite](https://www.sqlite.org/index.html) and stored in a single file that the user can copy and move between computers. Remote journals can be accessed using [MariaDB](https://mariadb.org/), first, the user has to create a database server, then OpenJournal can access the database server to store journals. In this way, a user can access its journal locally and add a layer of security by password protection.

## Supported features

- [x] Alarms
- [x] Live markdown renderer
- [x] Markdown highlighting
- [x] Latex renderer
- [x] Privacy mode
- [x] Journal exportation in pdf
- [x] Tray integration
- [x] Remote journal access
- [x] Secure journal
- [x] Lock
- [x] Keyword searching Ctrl + F
- [x] Traductions
- [x] Mardown editor

## Screenshot

![preview](resources/readme.png)


## Installation
OpenJournal binary is available for Linux, Windows (installer and portable version), and MacOs.

## Command
OpenJournal supports commands that can be directly entered inside the note:
* Set an alarm `setAlarm(hh:mm,message);`

## LaTeX
LaTeX equation can be displayed as equation with ` \\[  \\]` or inline by ` \\(  \\)`.

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

