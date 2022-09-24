# OpenJournal

 ![Test](https://github.com/bgallois/openjournal/workflows/Tests/badge.svg) ![Clang Format](https://github.com/bgallois/openjournal/workflows/Clang%20Format/badge.svg) ![Continous Build](https://github.com/bgallois/openjournal/workflows/Continous%20Builds/badge.svg) [![License: GPL v2](https://img.shields.io/badge/License-GPLv2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

OpenJournal is a simple journal, note-taking, and assistant application supporting Markdown syntax and Markdown rendering thanks to [qmarkdowntextedit](https://github.com/pbek/qmarkdowntextedit). You can write your thoughts, to-do list and never forget a meeting by setting visual and audible alerts.

See the complete documentation at [https://openjournal.readthedocs.io/en/latest/](https://openjournal.readthedocs.io/en/latest/).
See the presentation website at [https://gallois.cc/openjournal/](https://gallois.cc/openjournal/).

## For who?

* **For writers**: as detailed in *Description & Setting* by Ron Rozelle, keeping a writer's journal is a good way to improve writing abilities. Writers can record plots outlines, observations, descriptions, details, dialog lines, etc... 
* **For anyone**: keeping a daily diary can help you achieve several goals. It can help you organize your thoughts, records daily success, and set your next goals to keep the insensitive high. Writing down feelings can help you self-reflect and relieve stress, keeping a trace of situations and feelings.
* **For professionals**: writing down tasks to achieve and expected jobs duration can help you organize your day. The possibility to set alarms can help to manage available time and not forget any meeting.

## Technology

OpenJournal uses a database to store and retrieve user notes. This flexible approach offers several advantages. Local journals can be created using [SQlite](https://www.sqlite.org/index.html) and stored in a single file that the user can copy and move between computers. Remote journals can be accessed using [MariaDB](https://mariadb.org/), first, the user has to create a database server, then OpenJournal can access the database server to store journals. In this way, a user can access its journal locally and add a layer of security by password protection.

## Screenshot

![preview](resources/readme.png)


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
- [x] Markdown editor
- [x] Local image insertion
- [x] Drag'n Drop images insertion
- [x] Drag'n Drop text files insertion
- [x] Smart Markdown insertion cursor
- [x] LaTeX expression
- [x] Qalculator expression

## Installation
OpenJournal binary is available for Linux, Windows (installer and portable version), and MacOs.


