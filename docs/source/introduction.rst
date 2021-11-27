Design Philosophy
=================
OpenJournal is a simple note-taking application. Each day represents a separate entry that can be navigated using a calendar widget. Each journal is stored inside a database that can be either a local file (`*.jnl`) or a database inside a MariaDB server.

User Interface
----------------
The user interface of OpenJournal is designed to be as simple as possible. It is constituted of 3 resizable panels:

- The editor panel.
- The calendar panel.
- The preview panel.

A toolbox provides shortcuts to the Markdown syntax and a menu bar to create and open journals.

Editor
~~~~~~~~~~~~~
The editor is based on QMarkdowTextEdit and supports the Markdown syntax and find and replace. It supports smart drag and drop for images and texts. Dropping an image from a local folder or from an Html link will copy the image in the journal and reference it locally. Plain text can be added by dropping a text file in the editor. The text will be inserted at the cursor location.

Calendar
~~~~~~~~~~~~~
The calendar provides a quick way to navigate through the journal.

Preview
~~~~~~~~~~~~~
The preview provides a way to visualize the rendered Markdown document. It can be used to previsualize the result of an entry and to comfortably read a journal.
