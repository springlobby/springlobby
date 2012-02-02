#ifndef SPRINGLOBBY_SECTIONTREE_H_INCLUDED
#define SPRINGLOBBY_SECTIONTREE_H_INCLUDED

#include "wrapper/set"
#include "wrapper/vector"
#include "wrapper/wx/string.h"

class SectionTree
{
    public:
        typedef std::set<wxString> KeySet;
        struct Node;
        typedef std::vector<Node> ChildVec;

        struct Node
        {
            ChildVec m_children;
            KeySet m_keys;
            wxString name,key,descr;
        };

    public:
        SectionTree();
        virtual ~SectionTree();

        //bool Add
    protected:
    private:
};

#endif // SPRINGLOBBY_SECTIONTREE_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

