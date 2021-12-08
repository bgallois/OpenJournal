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
