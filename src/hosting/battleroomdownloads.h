#ifndef BATTLEROOMDOWNLOADS_H
#define BATTLEROOMDOWNLOADS_H
#include <wx/panel.h>


class wxGauge;
class wxStaticText;
class ObserverDownloadInfo;
class IBattle;
class wxBoxSize;

class BattleRoomDownloads: public wxPanel
{
    public:
        BattleRoomDownloads(wxWindow* parent, IBattle* battle);
        ~BattleRoomDownloads();
        void OnUpdate();
        void SetBattle( IBattle* battle );
    private:

        wxGauge* m_map_progress;
        wxStaticText* m_map_info;
        wxStaticText* m_map_text;

        wxGauge* m_game_progress;
        wxStaticText* m_game_info;
        wxStaticText* m_game_text;

        IBattle* m_battle;

        void SetProgress(ObserverDownloadInfo obi,wxStaticText* txt,wxGauge* g);
        void NoDownload(wxStaticText* txt,wxGauge* g);
};

#endif // BATTLEROOMDOWNLOADS_H
