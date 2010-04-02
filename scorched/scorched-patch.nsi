; Helper defines
!define PRODUCT_NAME "Scorched3D"
!define PRODUCT_VERSION "43"
!define PRODUCT_PUBLISHER "Scorched"
!define PRODUCT_WEB_SITE "http://www.scorched3d.co.uk"
!define PRODUCT_DONATE_WEB_SITE "https://www.paypal.com/xclick/business=donations%40scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "data\images\tank2.ico"
!define MUI_UNICON "data\images\tank2.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Scorched3D-${PRODUCT_VERSION}-patch.exe"
InstallDir "$PROGRAMFILES\Scorched3D"
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" 0 done
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${PRODUCT_NAME} is not already installed, do you wish to patch anyway?" IDOK done
  Abort
done:
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try
  
File "/oname=$INSTDIR\.\CHANGELOG" ".\CHANGELOG" 
File "/oname=$INSTDIR\.\data\accessories.xml" ".\data\accessories.xml" 
File "/oname=$INSTDIR\.\data\custominfo.xml" ".\data\custominfo.xml" 
File "/oname=$INSTDIR\.\data\fonts\dejavusans.ttf" ".\data\fonts\dejavusans.ttf" 
File "/oname=$INSTDIR\.\data\fonts\dejavusconbd.ttf" ".\data\fonts\dejavusconbd.ttf" 
File "/oname=$INSTDIR\.\data\fonts\dejavusmobd.ttf" ".\data\fonts\dejavusmobd.ttf" 
Delete "$INSTDIR\.\data\fonts\test.ttf" 
Delete "$INSTDIR\.\data\fonts\VERA-COPYRIGHT.TXT" 
Delete "$INSTDIR\.\data\fonts\vera.ttf" 
Delete "$INSTDIR\.\data\fonts\veramobd.ttf" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\accessories.xml" ".\data\globalmods\apoc\data\accessories.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnapoccity.xml" ".\data\globalmods\apoc\data\landscapes\defnapoccity.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnarizonaislands.xml" ".\data\globalmods\apoc\data\landscapes\defnarizonaislands.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnbighill.xml" ".\data\globalmods\apoc\data\landscapes\defnbighill.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnbullseye.xml" ".\data\globalmods\apoc\data\landscapes\defnbullseye.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defncastle.xml" ".\data\globalmods\apoc\data\landscapes\defncastle.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defncolosseum.xml" ".\data\globalmods\apoc\data\landscapes\defncolosseum.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defncraters.xml" ".\data\globalmods\apoc\data\landscapes\defncraters.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defndesertmesa.xml" ".\data\globalmods\apoc\data\landscapes\defndesertmesa.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defndonuthills.xml" ".\data\globalmods\apoc\data\landscapes\defndonuthills.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnhexisle.xml" ".\data\globalmods\apoc\data\landscapes\defnhexisle.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnmaze.xml" ".\data\globalmods\apoc\data\landscapes\defnmaze.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnmoon.xml" ".\data\globalmods\apoc\data\landscapes\defnmoon.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnmountains.xml" ".\data\globalmods\apoc\data\landscapes\defnmountains.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnnexuscanyon.xml" ".\data\globalmods\apoc\data\landscapes\defnnexuscanyon.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnpillars.xml" ".\data\globalmods\apoc\data\landscapes\defnpillars.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnplateau.xml" ".\data\globalmods\apoc\data\landscapes\defnplateau.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnpyramids.xml" ".\data\globalmods\apoc\data\landscapes\defnpyramids.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnpyramids2.xml" ".\data\globalmods\apoc\data\landscapes\defnpyramids2.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnruffles.xml" ".\data\globalmods\apoc\data\landscapes\defnruffles.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnsteppes.xml" ".\data\globalmods\apoc\data\landscapes\defnsteppes.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnterraces.xml" ".\data\globalmods\apoc\data\landscapes\defnterraces.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defntwotowers.xml" ".\data\globalmods\apoc\data\landscapes\defntwotowers.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnusa.xml" ".\data\globalmods\apoc\data\landscapes\defnusa.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnvolcano.xml" ".\data\globalmods\apoc\data\landscapes\defnvolcano.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnweathered.xml" ".\data\globalmods\apoc\data\landscapes\defnweathered.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\defnworld.xml" ".\data\globalmods\apoc\data\landscapes\defnworld.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texarizona_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texarizona_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texasianspring_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texasianspring_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcastle_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texcastle_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texchina_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texchina_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity1_storm.xml" ".\data\globalmods\apoc\data\landscapes\texcity1_storm.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity1_summer.xml" ".\data\globalmods\apoc\data\landscapes\texcity1_summer.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity1_winter.xml" ".\data\globalmods\apoc\data\landscapes\texcity1_winter.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity2_storm.xml" ".\data\globalmods\apoc\data\landscapes\texcity2_storm.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity2_summer.xml" ".\data\globalmods\apoc\data\landscapes\texcity2_summer.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity2_winter.xml" ".\data\globalmods\apoc\data\landscapes\texcity2_winter.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity3_storm.xml" ".\data\globalmods\apoc\data\landscapes\texcity3_storm.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity3_summer.xml" ".\data\globalmods\apoc\data\landscapes\texcity3_summer.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texcity3_winter.xml" ".\data\globalmods\apoc\data\landscapes\texcity3_winter.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texdefault_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texdefault_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texdenali_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texdenali_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texdesert.xml" ".\data\globalmods\apoc\data\landscapes\texdesert.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texhell.xml" ".\data\globalmods\apoc\data\landscapes\texhell.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texice.xml" ".\data\globalmods\apoc\data\landscapes\texice.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texice_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texice_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texmoon.xml" ".\data\globalmods\apoc\data\landscapes\texmoon.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texnight_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texnight_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texsand_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texsand_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texsnow_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texsnow_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texstorm_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texstorm_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\textropical_bldg.xml" ".\data\globalmods\apoc\data\landscapes\textropical_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\textuscan_bldg.xml" ".\data\globalmods\apoc\data\landscapes\textuscan_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texusa_summer.xml" ".\data\globalmods\apoc\data\landscapes\texusa_summer.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\texvulcano_bldg.xml" ".\data\globalmods\apoc\data\landscapes\texvulcano_bldg.xml" 
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\modinfo.xml" ".\data\globalmods\apoc\data\modinfo.xml" 
File "/oname=$INSTDIR\.\data\html\server\account.html" ".\data\html\server\account.html" 
File "/oname=$INSTDIR\.\data\html\server\adminmenu.html" ".\data\html\server\adminmenu.html" 
File "/oname=$INSTDIR\.\data\html\server\game.html" ".\data\html\server\game.html" 
File "/oname=$INSTDIR\.\data\html\server\menu.html" ".\data\html\server\menu.html" 
File "/oname=$INSTDIR\.\data\html\server\player.html" ".\data\html\server\player.html" 
File "/oname=$INSTDIR\.\data\html\server\server.html" ".\data\html\server\server.html" 
File "/oname=$INSTDIR\.\data\html\server\sessions.html" ".\data\html\server\sessions.html" 
File "/oname=$INSTDIR\.\data\keys.xml" ".\data\keys.xml" 
File "/oname=$INSTDIR\.\data\landscapes\ascencion2.bmp" ".\data\landscapes\ascencion2.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\boidslargenumber.xml" ".\data\landscapes\boidslargenumber.xml" 
File "/oname=$INSTDIR\.\data\landscapes\circle.bmp" ".\data\landscapes\circle.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\defnascencion.xml" ".\data\landscapes\defnascencion.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnascencion2.xml" ".\data\landscapes\defnascencion2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defncanyon.xml" ".\data\landscapes\defncanyon.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defncanyon2.xml" ".\data\landscapes\defncanyon2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defncavern.xml" ".\data\landscapes\defncavern.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defncavern2.xml" ".\data\landscapes\defncavern2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defndenali.xml" ".\data\landscapes\defndenali.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defndenali2.xml" ".\data\landscapes\defndenali2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defndenali3.xml" ".\data\landscapes\defndenali3.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnglaciation.xml" ".\data\landscapes\defnglaciation.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnglaciation2.xml" ".\data\landscapes\defnglaciation2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnglaciation3.xml" ".\data\landscapes\defnglaciation3.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnhalfed.xml" ".\data\landscapes\defnhalfed.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnhilly.xml" ".\data\landscapes\defnhilly.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnicebergs2.xml" ".\data\landscapes\defnicebergs2.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnicebergs3.xml" ".\data\landscapes\defnicebergs3.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnislands.xml" ".\data\landscapes\defnislands.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnlowlands.xml" ".\data\landscapes\defnlowlands.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnoldstyle.xml" ".\data\landscapes\defnoldstyle.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnoldstyleerrosion.xml" ".\data\landscapes\defnoldstyleerrosion.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnridge.xml" ".\data\landscapes\defnridge.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnspike.xml" ".\data\landscapes\defnspike.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defntest.xml" ".\data\landscapes\defntest.xml" 
File "/oname=$INSTDIR\.\data\landscapes\defnvalley.xml" ".\data\landscapes\defnvalley.xml" 
File "/oname=$INSTDIR\.\data\landscapes\denali2.bmp" ".\data\landscapes\denali2.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\denali3.bmp" ".\data\landscapes\denali3.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\fjords.bmp" ".\data\landscapes\fjords.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\glaciation2.bmp" ".\data\landscapes\glaciation2.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\glaciation3.bmp" ".\data\landscapes\glaciation3.bmp" 
File "/oname=$INSTDIR\.\data\landscapes\placepinelargenumber.xml" ".\data\landscapes\placepinelargenumber.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texarizona.xml" ".\data\landscapes\texarizona.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texasianspring.xml" ".\data\landscapes\texasianspring.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texcastle.xml" ".\data\landscapes\texcastle.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texcavern.xml" ".\data\landscapes\texcavern.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texchina.xml" ".\data\landscapes\texchina.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texdefault.xml" ".\data\landscapes\texdefault.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texdenali.xml" ".\data\landscapes\texdenali.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texdenalilandperf.xml" ".\data\landscapes\texdenalilandperf.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texhell.xml" ".\data\landscapes\texhell.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texice.xml" ".\data\landscapes\texice.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texnight.xml" ".\data\landscapes\texnight.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texplage.xml" ".\data\landscapes\texplage.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texsahara.xml" ".\data\landscapes\texsahara.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texsand.xml" ".\data\landscapes\texsand.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texsnow.xml" ".\data\landscapes\texsnow.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texstorm.xml" ".\data\landscapes\texstorm.xml" 
File "/oname=$INSTDIR\.\data\landscapes\textropical.xml" ".\data\landscapes\textropical.xml" 
File "/oname=$INSTDIR\.\data\landscapes\textuscan.xml" ".\data\landscapes\textuscan.xml" 
File "/oname=$INSTDIR\.\data\landscapes\texvulcano.xml" ".\data\landscapes\texvulcano.xml" 
File "/oname=$INSTDIR\.\data\landscapes.xml" ".\data\landscapes.xml" 
CreateDirectory "$INSTDIR\.\data\lang"
File "/oname=$INSTDIR\.\data\lang\lang.resource" ".\data\lang\lang.resource" 
File "/oname=$INSTDIR\.\data\lang\lang_undefined.resource" ".\data\lang\lang_undefined.resource" 
CreateDirectory "$INSTDIR\.\data\lua"
CreateDirectory "$INSTDIR\.\data\lua\accessories"
File "/oname=$INSTDIR\.\data\lua\accessories\test.lua" ".\data\lua\accessories\test.lua" 
File "/oname=$INSTDIR\.\data\modinfo.xml" ".\data\modinfo.xml" 
File "/oname=$INSTDIR\.\data\mysql.xml" ".\data\mysql.xml" 
File "/oname=$INSTDIR\.\data\shaders\land-splat.fshader" ".\data\shaders\land-splat.fshader" 
File "/oname=$INSTDIR\.\data\shaders\land.fshader" ".\data\shaders\land.fshader" 
File "/oname=$INSTDIR\.\data\shaders\land.vshader" ".\data\shaders\land.vshader" 
File "/oname=$INSTDIR\.\data\shaders\water.fshader" ".\data\shaders\water.fshader" 
File "/oname=$INSTDIR\.\data\shaders\watersimple.fshader" ".\data\shaders\watersimple.fshader" 
File "/oname=$INSTDIR\.\data\shaders\watersimple.vshader" ".\data\shaders\watersimple.vshader" 
File "/oname=$INSTDIR\.\data\singlecustom.xml" ".\data\singlecustom.xml" 
File "/oname=$INSTDIR\.\data\singleeasy.xml" ".\data\singleeasy.xml" 
File "/oname=$INSTDIR\.\data\singlehard.xml" ".\data\singlehard.xml" 
File "/oname=$INSTDIR\.\data\singlenormal.xml" ".\data\singlenormal.xml" 
File "/oname=$INSTDIR\.\data\tanks.xml" ".\data\tanks.xml" 
File "/oname=$INSTDIR\.\data\textures\circle.bmp" ".\data\textures\circle.bmp" 
File "/oname=$INSTDIR\.\data\textures\circlem.bmp" ".\data\textures\circlem.bmp" 
File "/oname=$INSTDIR\.\data\textures\circlew.bmp" ".\data\textures\circlew.bmp" 
Delete "$INSTDIR\.\data\textures\landscape\china\texture2.jpg" 
File "/oname=$INSTDIR\.\data\tutorial.xml" ".\data\tutorial.xml" 
File "/oname=$INSTDIR\.\data\windows\perf.bmp" ".\data\windows\perf.bmp" 
File "/oname=$INSTDIR\.\data\windows\perfa.bmp" ".\data\windows\perfa.bmp" 
File "/oname=$INSTDIR\.\data\windows\rotation.png" ".\data\windows\rotation.png" 
File "/oname=$INSTDIR\.\data\windows.xml" ".\data\windows.xml" 
File "/oname=$INSTDIR\.\libexpat.dll" ".\libexpat.dll" 
File "/oname=$INSTDIR\.\libpng13.dll" ".\libpng13.dll" 
File "/oname=$INSTDIR\.\Microsoft.VC90.CRT.manifest" ".\Microsoft.VC90.CRT.manifest" 
File "/oname=$INSTDIR\.\msvcm90.dll" ".\msvcm90.dll" 
File "/oname=$INSTDIR\.\msvcp90.dll" ".\msvcp90.dll" 
File "/oname=$INSTDIR\.\msvcr90.dll" ".\msvcr90.dll" 
File "/oname=$INSTDIR\.\scorched.exe" ".\scorched.exe" 
File "/oname=$INSTDIR\.\Scorched3D-docs.url" ".\Scorched3D-docs.url" 
File "/oname=$INSTDIR\.\Scorched3D.url" ".\Scorched3D.url" 
File "/oname=$INSTDIR\.\scorchedc.exe" ".\scorchedc.exe" 
File "/oname=$INSTDIR\.\scorcheds.exe" ".\scorcheds.exe" 
File "/oname=$INSTDIR\.\uninst.exe" ".\uninst.exe" 
Delete "$INSTDIR\.\Microsoft.VC80.CRT.manifest" 
Delete "$INSTDIR\.\msvcm80.dll" 
Delete "$INSTDIR\.\msvcp80.dll" 
Delete "$INSTDIR\.\msvcr80.dll" 
Delete "$INSTDIR\.\scorched-patch.nsi" 
Delete "$INSTDIR\.\TODO" 






SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-docs.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}\wiki"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-donate.url" "InternetShortcut" "URL" "${PRODUCT_DONATE_WEB_SITE}"
  
  CreateDirectory "$SMPROGRAMS\Scorched3D"
  
  CreateShortCut "$SMPROGRAMS\Scorched3D\Uninstall Scorched3D.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D.lnk" "$INSTDIR\scorched.exe" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Documentation.lnk" "$INSTDIR\${PRODUCT_NAME}-docs.url"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Homepage.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Donations.lnk" "$INSTDIR\${PRODUCT_NAME}-donate.url"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Var DEL_USER
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES remove
  Abort
remove:

  StrCpy $DEL_USER "FALSE"
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you want to remove all $(^Name) user data.  Warning: this includes STATS, saved games, mods etc?" IDNO nodel
  StrCpy $DEL_USER "TRUE";
nodel:
FunctionEnd

Section Uninstall

  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\documentation"
  RMDir /r "$SMPROGRAMS\Scorched3D"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
  
  StrCmp $DEL_USER "FALSE" nodel
  RMDir /r "$INSTDIR\.scorched3d"
  RMDir /r "$PROFILE\.scorched3d"
nodel:

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

SectionEnd
