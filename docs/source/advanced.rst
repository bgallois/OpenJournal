************
Advanced
************

MariaDB journal
----------------

.. note::

    OpenJournal can access remotely created journals that are hosted on a MariaDB/MySQL server. For security purposes, do not expose databases to the internet!.

Server creation
~~~~~~~~~~~~~~~
.. code-block:: bash

    sudo apt install mariadb-server
    sudo mysql_secure_installation
    mysql -u root -p
    CREATE USER 'username'@'hostname' IDENTIFIED BY 'password';
    GRANT ALL ON *.* TO 'username'@'%' IDENTIFIED BY 'password';
    FLUSH PRIVILEGES;
    EXIT;

Connection
~~~~~~~~~~
* Go to File -> Connect to a remote planner.
* Enter username@hostname:port
* Enter journalname@password. If username has write privilege on the server, journalname database will be created if it doesn't exist, else journalname has to be created on the server by a user with write privilege.
