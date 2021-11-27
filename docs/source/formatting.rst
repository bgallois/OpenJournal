***************
Text formatting
***************

Markdown text
----------------

Paragraph
~~~~~~~~~

Paragraphs are separated by a blank line.

Style
~~~~~~~~

.. code-block:: markdown

    *italic*,
    **bold**

Link
~~~~~~~~

.. code-block:: markdown

    [OpenJournal](https://github.com/bgallois/OpenJournal)

Header
~~~~~~~~

.. code-block:: markdown

    # Title
    ## Subtitle
    ### SubSubtitle

List
~~~~~~~~

.. code-block:: markdown

    Bullet list:
    * Start a line with an asterisk
    * Food
    * Fruits
        * Oranges
        * Apples

    Numbered list:
    1. One
    2. Two
    3. Three

    To do list:
    - [x] Task one
    - [ ] Task two
    - [ ] Task three

Code
~~~~~~~~

.. code-block:: markdown

    `inline`
    ```
    block
    ```

Formulas
~~~~~~~~

.. code-block:: markdown

    \\( inline \\)
    \\[ block \\]

Quote
~~~~~~~~

.. code-block:: markdown

    > This is a blockquote.

Separator
~~~~~~~~~

.. code-block:: markdown

    ---


Markdown image
-----------------
Images can be added by dropping a file in the editor or using the Markdown syntax `![label](path)`. The image will be stored in the journal database and the reference will be updated to `![OpenJournal_label](tmp/label)`
and **should not** be modified afterward. Image files are temporarily copied in the system temporary folder for rendering and deleted when no longer useful.
