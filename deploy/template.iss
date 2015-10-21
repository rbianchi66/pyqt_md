[Setup]
AppName=@name@
AppVerName=@name@ @version@
AppPublisher=@publisher@
AppPublisherURL=@website@
AppSupportURL=@website@
AppUpdatesURL=@website@
DefaultDirName={pf}\@name@
DefaultGroupName=@publisher@\@name@
OutputBaseFilename=@name@-@version@-setup
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "eng"; MessagesFile: "compiler:Default.isl"
Name: "bas"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "bra"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "cat"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cze"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "dan"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dut"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "fin"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fre"; MessagesFile: "compiler:Languages\French.isl"
Name: "ger"; MessagesFile: "compiler:Languages\German.isl"
Name: "heb"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "hun"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "ita"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "nor"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pol"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "por"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "rus"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slk"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slo"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spa"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "@tree_dir@\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "@@com_server@@"; DestDir: "{sys}"; Flags: restartreplace sharedfile regserver

[Icons]
Name: "{group}\@name@"; Filename: "{app}\@exe@"; WorkingDir: "{app}"
Name: "{userdesktop}\@name@"; Filename: "{app}\@exe@"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
;#Filename: "{app}\@postinstall_run@"; Flags: postinstall nowait skipifsilent
;#Filename: "{app}\@postinstall_changelog@"; Description: "Display latest news about @name@ @version@"; Flags: postinstall nowait skipifsilent shellexec
