;ZipDLL include file for NSIS
;Written by Tim Kosse (mailto:tim.kosse@gmx.de)
;some improvements by deguix

;Supported languages with their translators in alphabetical order:

;Arabic translation by asdfuae
;Brazilian Portuguese translation by "deguix"
;Chinese, Simplified translation by Kii Ali <kiiali@cpatch.org>
;Chinese, Traditional traslation by "matini" and Kii Ali <kiiali@cpatch.org>
;Croatian translation by "iostriz"
;Danish translation by Claus Futtrup
;French translation by "veekee"
;German translation by Tim Kosse
;Hungarian translation by Toth Laszlo
;Korean translation by Seongab Kim
;Lithuanian translation by Vytautas Krivickas
;Polish translation by Krzysztof Galuszka
;Russion translation by Sergey
;Spanish translation by "dark_boy"

!ifndef ZIPDLL_USED

!define ZIPDLL_USED

!macro ZIPDLL_EXTRACT SOURCE DESTINATION FILE

  !define "FILE_${FILE}"

  !ifndef FILE_<ALL>
    Push "${FILE}"
  !endif

  IfFileExists "${DESTINATION}" +2
    CreateDirectory "${DESTINATION}"

  Push "${DESTINATION}"

  IfFileExists "${SOURCE}" +2
    SetErrors

  Push "${SOURCE}"


  !ifdef FILE_<ALL>
    ZipDLL::extractall
  !else
    ZipDLL::extractfile
  !endif

  !undef "FILE_${FILE}"

!macroend

!endif
