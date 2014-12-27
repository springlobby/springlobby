/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef AUTOHOSTMANAGER_H
#define AUTOHOSTMANAGER_H

#include <wx/string.h>
class IBattle;


class AutohostHandler
{
    public:
        AutohostHandler();
        virtual ~AutohostHandler();

        virtual void Balance(){};
        virtual void SetRandomMap(){};
        virtual void SetMap(const wxString& /*map*/){};
        virtual void ClearStartBoxes(){};
        virtual void AddStartBox(int /*posx*/,int /*posy*/,int /*w*/,int /*h*/){};
        virtual void Notify(){};
        virtual void Start(){};
    protected:
        IBattle* m_battle;

    private:
        void SetBattle(IBattle* battle);


        friend class AutohostManager;
};

class SpringieHandler: public AutohostHandler
{
    public:
        SpringieHandler();
        ~SpringieHandler();

        void Balance();
        void SetRandomMap();
        void SetMap(const wxString& map);
        void ClearStartBoxes();
        void AddStartBox(int posx,int posy,int w,int h);
        void Notify();
	void Start();
};

class SpadsHandler: virtual public AutohostHandler
{
    public:
        SpadsHandler();
        ~SpadsHandler();

        void Balance();
        void SetRandomMap();
        void SetMap(const wxString& map);
        void ClearStartBoxes();
        void AddStartBox(int posx,int posy,int w,int h);
        void Notify();
        void Start();
};

class AutohostManager
{
    public:

        enum AutohostType
        {
        	AUTOHOSTTYPE_NONE,
            AUTOHOSTTYPE_UNKNOWN,
            AUTOHOSTTYPE_SPRINGIE,
            AUTOHOSTTYPE_SPADS
        };

        AutohostManager();
        ~AutohostManager();

        void SetBattle(IBattle* bt);

        bool RecognizeAutohost(const wxString& type);

        AutohostType GetAutohostType();

        AutohostHandler& GetAutohostHandler();

        SpringieHandler& GetSpringie();
        SpadsHandler& GetSpads();
    private:
        SpringieHandler m_springie;
        SpadsHandler m_spads;
        AutohostHandler m_emptyhandler;

        AutohostType m_type;
        IBattle* m_battle;
};

#endif // AUTOHOSTMANAGER_H
