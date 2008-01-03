#ifndef SPRINGLOBBY_HEADERGUARD_LOBBYSOUNDS_H_
#define SPRINGLOBBY_HEADERGUARD_LOBBYSOUNDS_H_

#include <wx/dialog.h>

typedef int SoundType;

enum {
	SOUND_RING = 1,
	SOUND_MISC = 2
};

class SelectSoundsDialog : wxDialog
{
public:
	SelectSoundsDialog();
	virtual ~SelectSoundsDialog();
	
protected:
	void SetSoundFilename(wxString& filename, SoundType soundtype);
};

void PlaySound ( SoundType soundtype );

#endif /*LOBBYSOUNDS_H_*/
