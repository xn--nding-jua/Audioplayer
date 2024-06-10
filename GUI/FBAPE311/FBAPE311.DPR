program Fbape311;

uses
  Forms,
  Mainfrm in 'MAINFRM.PAS' {mainform};

{$R *.RES}

begin
  Application.Title := 'fbape Config Tool';
  Application.CreateForm(Tmainform, mainform);
  Application.Run;
end.
