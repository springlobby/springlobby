#ifndef BATTLEROOMDOWNLOADS_H
#define BATTLEROOMDOWNLOADS_H
#include <wx/panel.h>


class wxGauge;
class wxStaticText;
class ObserverDownloadInfo;
class IBattle;

class BattleRoomDownloads: public wxPanel
{
    public:
        BattleRoomDownloads(wxWindow* parent, IBattle* battle);
        ~BattleRoomDownloads();
        void OnUpdate();
        void SetBattle( IBattle* battle );
    private:
        wxGauge* m_map_progress;
        wxStaticText* m_map_text;

        wxGauge* m_mod_progress;
        wxStaticText* m_mod_text;

        IBattle* m_battle;

        void SetProgress(ObserverDownloadInfo obi,wxGauge* g);
        void NoDownload(wxGauge* g);
};

#endif // BATTLEROOMDOWNLOADS_H
