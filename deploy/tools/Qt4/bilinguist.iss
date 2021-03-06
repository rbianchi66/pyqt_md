; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Linguist for Comelz
AppVerName=Linguist for Comelz 4.3.0
AppPublisher=Comelz S.p.a
AppPublisherURL=http://www.comelz.com/
AppSupportURL=http://www.comelz.com/
AppUpdatesURL=http://www.comelz.com/
DefaultDirName={pf}\Linguist for Comelz
DefaultGroupName=Linguist for Comelz
AllowNoIcons=yes
OutputBaseFilename=comelz-linguist
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "basque"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "bilinguist.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "msvcp71.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "msvcr71.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Linguist for Comelz"; Filename: "{app}\bilinguist.exe"
Name: "{group}\{cm:UninstallProgram,Linguist for Comelz}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Linguist for Comelz"; Filename: "{app}\bilinguist.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\bilinguist.exe"; Description: "{cm:LaunchProgram,Linguist for Comelz}"; Flags: nowait postinstall skipifsilent

