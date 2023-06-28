## 1.3.4

Version 1.3.4 of OpenJournal comes with several internal fixes.

### Fixes

* Fix lock file

## 1.3.3

Version 1.3.3 of OpenJournal comes with several internal fixes and the Qalculate support.

### Fixes

* Fix qalculate seg fault
* Fix memory leaks
* Fix Linux installation

### Features

* Support qalculate
* New icon

## 1.3.2

Version 1.3.2 of OpenJournal comes with several internal fixes.

### Fixes

* GHSA-rrrm-qjm4-v8hf
* Check for new release

### Features

* Support action
* Win and Mac tests
* Refactore database connection
* Refactore window geometry saving


## 1.3.1

Version 1.3.1 of OpenJournal comes with several fixes. Journal in the cloud can now be successfully exported as PDF. Exporting a journal with images in PDF is now possible. Image display for cloud journal is now improved and does not necessitate refreshing the page after first loading.

### Fixes

* Compilation error for C++17
* Cloud journal exportation
* Images journal exportation
* Images loading on the cloud

### Features

* Refactore alarm

## 1.3.0

Version 1.3.0 of OpenJournal comes with several efficiency improvements. The user interface is now smoother with no flickering at refresh in low bandwidth conditions. Version 1.3.0 comes with a new feature called the OpenJournal cloud. It is now possible to store journals on the cloud (after registration) and to update and retrieve journal entries using POST HTTPS requests. This version sees a change in concurrency modification. True concurrency is not supported in OpenJournal. You can’t edit the same journal on several computers. However, one journal open on several computers will update itself if the two subsequent changes on two different computers are separated by more than twenty seconds. With this new behavior, an OpenJournal inactive on some computer can't overwrite the cloud journal opened and possibly modified by another computer.

### Fixes

* Maximal image size
* Database unique key
* Flickering at refresh
* Low bandwidth usability

### Features

* Connection to cloud to store journals
* System ressources optimization
* Database query optimization

## 1.2.0

### Fixes

* LaTeX rendering
* Cursor freezing
* Qss style

### Features

* Markdown syntax (button and keyboard) shortcuts to format text with smart cursor behavior
* Image support: local or remote images are downloaded and stored in the journal and referenced in the journal entry
* Smart drag and drop for images and text
* Each journal entry can be exported in pdf or Markdown file
* Markdown file can be inserted directly inside a journal entry
* User manual at https://openjournal.readthedocs.io/en/latest/
