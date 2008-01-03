#include "lobbysounds.h"
#include "settings.h"
//may need conditional include
#include <wx/sound.h>
#include <wx/utils.h>

const wxSound ringSound (_T("/home/kosh/projekte/settings/src/sounds/battle020.wav"));

SelectSoundsDialog::SelectSoundsDialog()
{
}

SelectSoundsDialog::~SelectSoundsDialog()
{
}

void SelectSoundsDialog::SetSoundFilename(wxString& filename, SoundType soundtype)
{
	
}

void PlaySound ( SoundType soundtype )
{
//	wxSound sound( sett().GetSoundFilename(soundtype) );
	if ( !ringSound.Play() )
		wxBell();
	
}
