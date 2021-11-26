/*
Copyright (C) 2021 Benjamin Gallois benjamin at gallois.cc

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef EDITOR_H
#define EDITOR_H

#include "qmarkdowntextedit.h"

class Editor : public QMarkdownTextEdit {
 public:
  using QMarkdownTextEdit::QMarkdownTextEdit;
};

#endif  // EDITOR_H
