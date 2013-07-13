#ifndef AUTOHOSTMANAGER_H
#define AUTOHOSTMANAGER_H

#include <wx/string.h>
#include <exception>
class Battle;


class AutohostHandler
{
    public:
        AutohostHandler();
        virtual ~AutohostHandler();

        virtual void Balance(){};
        virtual void SetRandomMap(){};
        virtual void SetMap(wxString& /*map*/){};
        virtual void ClearStartBoxes(){};
        virtual void AddStartBox(int /*posx*/,int /*posy*/,int /*w*/,int /*h*/){};
        virtual void Notify(){};
        virtual void Start(){};
    protected:
        Battle* m_battle;

    private:
        void SetBattle(Battle* battle);


        friend class AutohostManager;
};

class SpringieHandler: public AutohostHandler
{
    public:
        SpringieHandler();
        ~SpringieHandler();

        void Balance();
        void SetRandomMap();
        void SetMap(wxString& map);
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
        void SetMap(wxString& map);
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
            AUTOHOSTTYPE_UNKNOWN,
            AUTOHOSTTYPE_SPRINGIE,
            AUTOHOSTTYPE_SPADS
        };

        AutohostManager();
        ~AutohostManager();

        void SetBattle(Battle* bt);

        void RecnognizeAutohost();
        void RecnognizeAutohost(const wxString& who, const wxString& message);

        AutohostType GetAutohostType();

        AutohostHandler& GetAutohostHandler();

        SpringieHandler& GetSpringie();
        SpadsHandler& GetSpads();
    private:
        SpringieHandler m_springie;
        SpadsHandler m_spads;
        AutohostHandler m_emptyhandler;

        AutohostType m_type;
        Battle* m_battle;
};

#endif // AUTOHOSTMANAGER_H
