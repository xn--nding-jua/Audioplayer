{*****************************************
 * f/bape Config Tool                    *
 * v1.0.2 from 20.05.2024                *
 * (c) 2024 by Dr.-Ing. Christian Nöding *
 * christian@noeding-online.de           *
 * https://www.github.com/xn--nding-jua  *
 *****************************************}

unit Mainfrm;

interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, TabNotBk, ExtCtrls, StdCtrls, Buttons,
  {$IFDEF VER80}
    ComDrv16,
  {$ELSE}
    CPDrv, ComCtrls,
  {$ENDIF}
  Mask, Spin;

type
  TEQ = record
    eq_type: byte;
    fc: single;
    Q: single;
    gain: single;
  end;
  Tmainform = class(TForm)
    tabnotebook: TTabbedNotebook;
    Timer1: TTimer;
    Panel4: TPanel;
    Panel1: TPanel;
    Panel3: TPanel;
    Panel2: TPanel;
    GroupBox1: TGroupBox;
    eq_box: TPaintBox;
    Panel5: TPanel;
    Panel6: TPanel;
    Panel7: TPanel;
    Panel8: TPanel;
    GroupBox2: TGroupBox;
    comp_box: TPaintBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    eq_type_dropdown: TComboBox;
    eq_freq_slider: TScrollBar;
    eq_freq_lbl: TLabel;
    eq_q_slider: TScrollBar;
    eq_q_lbl: TLabel;
    eq_gain_slider: TScrollBar;
    eq_gain_lbl: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    comp_threshold_slider: TScrollBar;
    comp_ratio_dropdown: TComboBox;
    comp_makeup_dropdown: TComboBox;
    comp_attack_slider: TScrollBar;
    comp_hold_slider: TScrollBar;
    comp_release_slider: TScrollBar;
    comp_threshold_lbl: TLabel;
    comp_attack_lbl: TLabel;
    comp_hold_lbl: TLabel;
    comp_release_lbl: TLabel;
    GroupBox3: TGroupBox;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    GroupBox4: TGroupBox;
    volumeanalogslider: TScrollBar;
    volumesdcardslider: TScrollBar;
    volumespdifslider: TScrollBar;
    volume_ultranet_slider1: TScrollBar;
    volume_ultranet_slider2: TScrollBar;
    volume_ultranet_slider3: TScrollBar;
    volume_ultranet_slider4: TScrollBar;
    volume_ultranet_slider8: TScrollBar;
    volume_ultranet_slider5: TScrollBar;
    volume_ultranet_slider6: TScrollBar;
    volume_ultranet_slider7: TScrollBar;
    volume_ultranet_slider16: TScrollBar;
    volume_ultranet_slider13: TScrollBar;
    volume_ultranet_slider14: TScrollBar;
    volume_ultranet_slider15: TScrollBar;
    volume_ultranet_slider9: TScrollBar;
    volume_ultranet_slider10: TScrollBar;
    volume_ultranet_slider11: TScrollBar;
    volume_ultranet_slider12: TScrollBar;
    Label18: TLabel;
    Label19: TLabel;
    Label20: TLabel;
    Label21: TLabel;
    GroupBox5: TGroupBox;
    Label22: TLabel;
    volumemainslider: TScrollBar;
    volumesubslider: TScrollBar;
    Label23: TLabel;
    GroupBox6: TGroupBox;
    Label25: TLabel;
    Label26: TLabel;
    comport_dropdown: TComboBox;
    connectbtn: TButton;
    disconnectbtn: TButton;
    RxMemo: TMemo;
    GroupBox7: TGroupBox;
    Label27: TLabel;
    syncsourcedropdown: TComboBox;
    Label28: TLabel;
    sampleratedropdown: TComboBox;
    playermemo: TMemo;
    Label34: TLabel;
    Label35: TLabel;
    Label36: TLabel;
    Label37: TLabel;
    Label38: TLabel;
    Label39: TLabel;
    Label40: TLabel;
    Label41: TLabel;
    Label44: TLabel;
    Label42: TLabel;
    Label43: TLabel;
    Label45: TLabel;
    comport: TCommPortDriver;
    Panel9: TPanel;
    Panel10: TPanel;
    Panel11: TPanel;
    GroupBox8: TGroupBox;
    Label30: TLabel;
    loopcheckbox: TCheckBox;
    playbtn: TButton;
    pausebtn: TButton;
    stopbtn: TButton;
    loadbtn: TButton;
    filenameedit: TEdit;
    tocbtn: TButton;
    GroupBox9: TGroupBox;
    btonbtn: TButton;
    btoffbtn: TButton;
    btplaybtn: TButton;
    btpausebtn: TButton;
    btstopbtn: TButton;
    btrwbtn: TButton;
    btffbtn: TButton;
    btprevbtn: TButton;
    btnextbtn: TButton;
    Panel12: TPanel;
    Panel13: TPanel;
    Panel14: TPanel;
    GroupBox10: TGroupBox;
    resetmanibtn: TButton;
    initsystembtn: TButton;
    bootloadbtn: TButton;
    GroupBox11: TGroupBox;
    mixerresetbtn: TButton;
    reseteqbtn: TButton;
    resetdynamicsbtn: TButton;
    GroupBox12: TGroupBox;
    Label31: TLabel;
    Label32: TLabel;
    Label33: TLabel;
    wifissidedit: TEdit;
    wifipasswordedit: TEdit;
    wifiapcheckbox: TRadioButton;
    wificlientcheckbox: TRadioButton;
    wifionbtn: TButton;
    wifioffbtn: TButton;
    wifisendbtn: TButton;
    wifireadipbtn: TButton;
    infobtn: TButton;
    Image1: TImage;
    GroupBox13: TGroupBox;
    Label29: TLabel;
    Label48: TLabel;
    Label49: TLabel;
    Label50: TLabel;
    gate_threshold_lbl: TLabel;
    gate_attack_lbl: TLabel;
    gate_hold_lbl: TLabel;
    gate_release_lbl: TLabel;
    gate_threshold_slider: TScrollBar;
    gate_attack_slider: TScrollBar;
    gate_hold_slider: TScrollBar;
    gate_release_slider: TScrollBar;
    gate_range_lbl: TLabel;
    gate_range_slider: TScrollBar;
    Label47: TLabel;
    vol_spdif_lbl: TLabel;
    vol_sdcard_lbl: TLabel;
    vol_analog_lbl: TLabel;
    vol_main_lbl: TLabel;
    vol_sub_lbl: TLabel;
    cmdbtn: TButton;
    cmdmemo: TMemo;
    baudrate_dropdown: TComboBox;
    freq_lp: TSpinEdit;
    freq_hp: TSpinEdit;
    procedure Timer1Timer(Sender: TObject);
    procedure eq_type_dropdownChange(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure comp_threshold_sliderChange(Sender: TObject);
    procedure connectbtnClick(Sender: TObject);
    procedure disconnectbtnClick(Sender: TObject);
    {$IFDEF VER80}
    procedure comportReceiveData(Sender: TObject; DataPtr: Pointer;
      DataSize: Integer);
    {$ELSE}
    procedure comportReceiveData(Sender: TObject; DataPtr: Pointer;
      DataSize: Cardinal);
    {$ENDIF}
    procedure tocbtnClick(Sender: TObject);
    procedure bootloadbtnClick(Sender: TObject);
    procedure infobtnClick(Sender: TObject);
    procedure freq_hpChange(Sender: TObject);
    procedure freq_lpChange(Sender: TObject);
    procedure volumeanalogsliderChange(Sender: TObject);
    procedure volumesdcardsliderChange(Sender: TObject);
    procedure volumespdifsliderChange(Sender: TObject);
    procedure volumemainsliderChange(Sender: TObject);
    procedure volumesubsliderChange(Sender: TObject);
    procedure volume_ultranet_slider1Change(Sender: TObject);
    procedure volume_ultranet_slider2Change(Sender: TObject);
    procedure volume_ultranet_slider3Change(Sender: TObject);
    procedure volume_ultranet_slider4Change(Sender: TObject);
    procedure volume_ultranet_slider5Change(Sender: TObject);
    procedure volume_ultranet_slider6Change(Sender: TObject);
    procedure volume_ultranet_slider7Change(Sender: TObject);
    procedure volume_ultranet_slider8Change(Sender: TObject);
    procedure volume_ultranet_slider9Change(Sender: TObject);
    procedure volume_ultranet_slider10Change(Sender: TObject);
    procedure volume_ultranet_slider11Change(Sender: TObject);
    procedure volume_ultranet_slider12Change(Sender: TObject);
    procedure volume_ultranet_slider13Change(Sender: TObject);
    procedure volume_ultranet_slider14Change(Sender: TObject);
    procedure volume_ultranet_slider15Change(Sender: TObject);
    procedure volume_ultranet_slider16Change(Sender: TObject);
    procedure syncsourcedropdownChange(Sender: TObject);
    procedure sampleratedropdownChange(Sender: TObject);
    procedure mixerresetbtnClick(Sender: TObject);
    procedure reseteqbtnClick(Sender: TObject);
    procedure resetdynamicsbtnClick(Sender: TObject);
    procedure initsystembtnClick(Sender: TObject);
    procedure resetmanibtnClick(Sender: TObject);
    procedure playbtnClick(Sender: TObject);
    procedure loadbtnClick(Sender: TObject);
    procedure pausebtnClick(Sender: TObject);
    procedure stopbtnClick(Sender: TObject);
    procedure loopcheckboxMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure loopcheckboxKeyUp(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure btonbtnClick(Sender: TObject);
    procedure btoffbtnClick(Sender: TObject);
    procedure btplaybtnClick(Sender: TObject);
    procedure btpausebtnClick(Sender: TObject);
    procedure btstopbtnClick(Sender: TObject);
    procedure btprevbtnClick(Sender: TObject);
    procedure btffbtnClick(Sender: TObject);
    procedure btnextbtnClick(Sender: TObject);
    procedure btrwbtnClick(Sender: TObject);
    procedure wifioffbtnClick(Sender: TObject);
    procedure wifionbtnClick(Sender: TObject);
    procedure wifisendbtnClick(Sender: TObject);
    procedure wifireadipbtnClick(Sender: TObject);
    procedure tabnotebookClick(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure gate_threshold_sliderChange(Sender: TObject);
    procedure cmdbtnClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private-Deklarationen}
    FirstStart:boolean;
    UpdatingGUI:boolean;
    flag_StartComplete: boolean;
    flag_recalcEQs: boolean;
    flag_recalcComp: boolean;
    flag_recalcGate: boolean;
    EQs: array[0..4] of TEQ;
    CurrentEQ: byte;
    eqbitmap, compbitmap: TBitmap;
    function IntPower(base: single; exponent: integer): single;
    function Power(base, exponent: single): single;
    function log10(x: single): single;
    function tan(x: single): single;

    procedure calcIIRCoefficients(eq_type:byte; fc:single; Q:single; gain:single; fs:single; var a0, a1, a2, b1, b2: single);
    function calcFrequencyResponse(a0, a1, a2, b1, b2: single; f:single; fs:word):single;
    function lin2log(value:single; minp,maxp,minv,maxv:integer):single;
    function log2lin(value:single; minp,maxp,minv,maxv: integer):single;
    procedure recalcEQs;
    procedure drawEQbackground;
    procedure redrawEQs;
    procedure UpdateEqControls;
    procedure recalcCOMP;
    procedure recalcGATE;

    function value2str(value: single): String;
    procedure SendVolume(sliderPos: Word; chStr: String);
    procedure SendCommand(cmd: string);
  public
    { Public-Deklarationen }
  end;

var
  mainform: Tmainform;

implementation

{$R *.DFM}

procedure Tmainform.Timer1Timer(Sender: TObject);
var
  i:integer;
begin
  if not flag_StartComplete then
  begin
    eqbitmap := TBitmap.Create;
    compbitmap := TBitmap.Create;
    eqbitmap.width := eq_box.width;
    eqbitmap.height := eq_box.height;
    compbitmap.width := comp_box.width;
    compbitmap.height := comp_box.height;

    comport_dropdown.itemindex := 0; {set to COM1}
    baudrate_dropdown.itemindex := 4; {set to 115200}
    syncsourcedropdown.itemindex := 0; {set to Analog input}
    sampleratedropdown.itemindex := 1; {set to 48kHz}

    CurrentEQ := 0;
    for i:=0 to 4 do
    begin
      EQs[i].eq_type:=0;
      EQs[i].fc:=1000;
      EQs[i].gain:=0;
      EQs[i].Q:=2;
    end;
    UpdateEqControls;
    flag_recalcEQs := true;
    flag_recalcCOMP := true;
    flag_recalcGate := true;

    comp_ratio_dropdown.ItemIndex:=0;
    comp_makeup_dropdown.ItemIndex:=0;

    flag_StartComplete := true;
  end;

  if flag_recalcEQs then
  begin
    flag_recalcEQs := false;
    recalcEQs;
  end;

  if flag_recalcComp then
  begin
    flag_recalcComp := false;
    recalcCOMP;
  end;

  if flag_recalcGate then
  begin
    flag_recalcGate := false;
    recalcGATE;
  end;
end;

function Tmainform.IntPower(base: single; exponent: integer): single;
var
  Y:integer;
begin
  Y := abs(Exponent);
  Result := 1.0;
  while Y > 0 do
  begin
    while not Odd(Y) do
    begin
      Y := Y shr 1;
      Base := Base * Base
    end;
    Dec(Y);
    Result := Result * Base
  end;
  if Exponent < 0 then Result := 1.0 / Result
end;

function Tmainform.Power(base, exponent: single): single;
begin
  if Exponent = 0.0 then
    Result := 1.0
  else if (Base = 0.0) and (Exponent > 0.0) then
    Result := 0.0
  else if (Frac(Exponent) = 0.0) and (Abs(Exponent) <= MaxInt) then
    Result := IntPower(Base, Integer(Trunc(Exponent)))
  else
    Result := Exp(Exponent * Ln(Base))
end;

function Tmainform.log10(x: single): single;
begin
  Result := ln(x) / ln(10);
end;

function Tmainform.tan(x: single): single;
begin
  Result := sin(x) / cos(x);
end;

procedure Tmainform.calcIIRCoefficients(eq_type:byte; fc:single;
  Q:single; gain:single; fs:single; var a0, a1, a2, b1, b2: single);
var
  V, K, norm: single;
begin
  V := power(10, abs(gain)/20);
  K := tan(PI * fc/fs);

  case eq_type of
    0:
    begin
      norm := 1.0 / (1.0 + K * 1.0/Q + power(K, 2));
      a0 := (1.0 - K * 1.0/Q + power(K, 2)) * norm;
      a1 := 2.0 * (power(K, 2) - 1.0) * norm;
      a2 := 1.0;
      b1 := a1;
      b2 := a0;
    end;
    1:
    begin
      if (gain>=0) then
      begin
	norm := 1.0 / (1.0 + (1.0/Q) * K + power(K,2));

	a0 := (1.0 + (V/Q) * K + power(K,2)) * norm;
	a1 := 2.0 * (power(K,2) - 1.0) * norm;
	a2 := (1.0 - (V/Q) * K + power(K,2)) * norm;
	b1 := a1;
	b2 := (1.0 - (1.0/Q) * K + power(K,2)) * norm;
      end else
      begin
	norm := 1.0 / (1.0 + (V/Q) * K + power(K,2));

	a0 := (1.0 + (1.0/Q) * K + power(K,2)) * norm;
	a1 := 2.0 * (power(K,2) - 1.0) * norm;
	a2 := (1.0 - (1.0/Q) * K + power(K,2)) * norm;
	b1 := a1;
	b2 := (1.0 - (V/Q) * K + power(K,2)) * norm;
      end;
    end;
    2:
    begin
      if (gain>=0) then
      begin
	norm := 1.0 / (1.0 + sqrt(2.0) * K + power(K, 2));
	a0 := (1.0 + sqrt(2.0*V) * K + V * power(K, 2)) * norm;
	a1 := 2.0 * (V * power(K, 2) - 1.0) * norm;
	a2 := (1.0 - sqrt(2.0*V) * K + V * power(K, 2)) * norm;
	b1 := 2.0 * (power(K, 2) - 1.0) * norm;
	b2 := (1.0 - sqrt(2.0) * K + power(K, 2)) * norm;
      end else
      begin
	norm := 1.0 / (1.0 + sqrt(2.0*V) * K + V * power(K, 2));
	a0 := (1.0 + sqrt(2.0) * K + power(K, 2)) * norm;
	a1 := 2.0 * (power(K, 2) - 1.0) * norm;
	a2 := (1.0 - sqrt(2) * K + power(K, 2)) * norm;
	b1 := 2.0 * (V * power(K, 2) - 1.0) * norm;
	b2 := (1.0 - sqrt(2.0*V) * K + V * power(K, 2)) * norm;
      end;
    end;
    3:
    begin
      if (gain>=0) then
      begin
	norm := 1.0 / (1.0 + sqrt(2.0) * K + power(K, 2));
	a0 := (V + sqrt(2.0*V) * K + power(K, 2)) * norm;
	a1 := 2.0 * (power(K, 2) - V) * norm;
	a2 := (V - sqrt(2.0*V) * K + power(K, 2)) * norm;
	b1 := 2.0 * (power(K, 2) - 1.0) * norm;
	b2 := (1.0 - sqrt(2.0) * K + power(K, 2)) * norm;
      end else
      begin
	norm := 1.0 / (V + sqrt(2.0*V) * K + power(K, 2));
	a0 := (1.0 + sqrt(2.0) * K + power(K, 2)) * norm;
	a1 := 2.0 * (power(K, 2) - 1.0) * norm;
	a2 := (1.0 - sqrt(2.0) * K + power(K, 2)) * norm;
	b1 := 2.0 * (power(K, 2) - V) * norm;
	b2 := (V - sqrt(2.0*V) * K + power(K, 2)) * norm;
      end;
    end;
    4:
    begin
      norm := 1.0 / (1.0 + K / Q + power(K, 2));
      a0 := (K / Q) * norm;
      a1 := 0;
      a2 := -a0;
      b1 := 2.0 * (power(K, 2) - 1.0) * norm;
      b2 := (1.0 - K / Q + power(K, 2)) * norm;
    end;
    5:
    begin
      norm := 1.0 / (1.0 + K / Q + power(K, 2));
      a0 := (1.0 + power(K, 2)) * norm;
      a1 := 2.0 * (power(K, 2) - 1.0) * norm;
      a2 := a0;
      b1 := a1;
      b2 := (1.0 - K / Q + power(K, 2)) * norm;
    end;
    6:
    begin
      norm := 1.0 / (1.0 + K / Q + power(K, 2));
      a0 := power(K, 2) * norm;
      a1 := 2.0 * a0;
      a2 := a0;
      b1 := 2.0 * (power(K, 2) - 1.0) * norm;
      b2 := (1.0 - K / Q + power(K, 2)) * norm;
    end;
    7:
    begin
      norm := 1.0 / (1.0 + K / Q + power(K, 2));
      a0 := 1.0 * norm;
      a1 := -2.0 * a0;
      a2 := a0;
      b1 := 2.0 * (power(K, 2) - 1.0) * norm;
      b2 := (1.0 - K / Q + power(K, 2)) * norm;
    end;
  end;
end;

function Tmainform.calcFrequencyResponse(a0, a1, a2, b1, b2: single; f:single; fs:word):single;
var
  w, phi, b0: single;
  function square(x: single):single;
  begin
    result := x*x;
  end;
begin
  w := (2.0 * PI * f)/fs;
  phi := power(sin(w/2.0),2);
  b0 := 1.0;

  result := ( 10.0 * log10( IntPower(a0+a1+a2, 2)
         - 4.0*(a0*a1 + 4.0*a0*a2 + a1*a2) * phi
         + 16.0*a0*a2*IntPower(phi,2) )
         - 10.0 * log10( IntPower(b0+b1+b2, 2)
         - 4.0*(b0*b1 + 4.0*b0*b2 + b1*b2)*phi
         + 16.0*b0*b2*IntPower(phi, 2) ) );
  {result := 20*log((sqrt(square(a0*square(cos(w))-a0*square(sin(w))+a1*cos(w)+a2)
  + square(2*a0*cos(w)*sin(w)+a1*(sin(w)))) / Math.sqrt(square(square(cos(w))
  - square(sin(w))+b1*cos(w)+b2)+square(2*   cos(w)*sin(w)+b1*(sin(w))))))}
end;

function Tmainform.lin2log(value: single; minp,maxp,minv,maxv:integer):single;
var
  scale: single;
begin
  scale := (ln(maxv)-ln(minv)) / (maxp-minp);

  result := exp(ln(minv) + scale*(value-minp));
end;

function Tmainform.log2lin(value:single; minp,maxp,minv,maxv: integer):single;
var
  scale: single;
begin
  if (maxv<>0) and (minv<>0) and (value<>0) and ((maxp-minp) <> 0) then
  begin
    scale := (ln(maxv)-ln(minv)) / (maxp-minp);

    result := ((ln(value)-ln(minv))/scale)+minp;
  end else
    result := 0;
end;

procedure Tmainform.recalcEQs;
begin
  if UpdatingGUI then exit;

  {read GUI and store current settings}
  EQs[CurrentEQ].eq_type := eq_type_dropdown.ItemIndex;
  EQs[CurrentEQ].fc := lin2log(eq_freq_slider.Position,0,10000,15,20000);
  EQs[CurrentEQ].Q := eq_q_slider.Position/10;
  EQs[CurrentEQ].gain := (eq_gain_slider.Position-5000)/250;

  {send data to f/bape}
  SendCommand('mixer:eq:peq' + inttostr(CurrentEQ+1) + '@' +
    inttostr(EQs[CurrentEQ].eq_type) + ',' +
    value2str(EQs[CurrentEQ].fc) + ',' +
    value2str(EQs[CurrentEQ].Q) + ',' +
    value2str(EQs[CurrentEQ].gain));

  {update GUI}
  if round(EQs[CurrentEQ].fc)<1000 then
    eq_freq_lbl.caption := floattostrf(EQs[CurrentEQ].fc, ffFixed, 7, 1) + ' Hz'
  else if round(EQs[CurrentEQ].fc)<10000 then
    eq_freq_lbl.caption := floattostrf(EQs[CurrentEQ].fc/1000, ffFixed, 7, 2) + ' kHz'
  else
    eq_freq_lbl.caption := floattostrf(EQs[CurrentEQ].fc/1000, ffFixed, 7, 1) + ' kHz';
  eq_q_lbl.caption := floattostrf(EQs[CurrentEQ].Q, ffFixed, 15, 1);
  eq_gain_lbl.caption := floattostrf(EQs[CurrentEQ].gain, ffFixed, 15, 3);

  redrawEQs;
end;

procedure Tmainform.drawEQbackground;
var
  i:integer;
begin
  {draw EQ-response}
  {clear drawing}
  eqbitmap.Canvas.Brush.Color := clWhite;
  eqbitmap.Canvas.Brush.Style := bsSolid;
  eqbitmap.Canvas.Rectangle(0,0, eq_box.width, eq_box.height);
  {horizontal lines}
  eqbitmap.Canvas.Pen.Color := clSilver;
  eqbitmap.Canvas.Pen.Style := psDot;
  for i:=1 to 10 do
  begin
    eqbitmap.Canvas.MoveTo(0, round(i * eq_box.height/8));
    eqbitmap.Canvas.LineTo(eq_box.width, round(i * eq_box.height/8));
  end;
  {draw vertical lines}
  for i:=2 to 9 do
  begin
    eqbitmap.Canvas.MoveTo(round(log2lin(i*10,0,eq_box.width,15,20000)), 0);
    eqbitmap.Canvas.LineTo(round(log2lin(i*10,0,eq_box.width,15,20000)), eq_box.height);
  end;
  for i:=2 to 9 do
  begin
    eqbitmap.Canvas.MoveTo(round(log2lin(i*100,0,eq_box.width,15,20000)), 0);
    eqbitmap.Canvas.LineTo(round(log2lin(i*100,0,eq_box.width,15,20000)), eq_box.height);
  end;
  for i:=2 to 10 do
  begin
    eqbitmap.Canvas.MoveTo(round(log2lin(i*1000,0,eq_box.width,15,20000)), 0);
    eqbitmap.Canvas.LineTo(round(log2lin(i*1000,0,eq_box.width,15,20000)), eq_box.height);
  end;
  eqbitmap.Canvas.Pen.Color := clBlack;
  eqbitmap.Canvas.MoveTo(round(log2lin(100,0,eq_box.width,15,20000)), 0);
  eqbitmap.Canvas.LineTo(round(log2lin(100,0,eq_box.width,15,20000)), eq_box.height);
  eqbitmap.Canvas.MoveTo(round(log2lin(1000,0,eq_box.width,15,20000)), 0);
  eqbitmap.Canvas.LineTo(round(log2lin(1000,0,eq_box.width,15,20000)), eq_box.height);
  eqbitmap.Canvas.MoveTo(round(log2lin(10000,0,eq_box.width,15,20000)), 0);
  eqbitmap.Canvas.LineTo(round(log2lin(10000,0,eq_box.width,15,20000)), eq_box.height);

  {horizontal text}
  eqbitmap.Canvas.TextOut(round(log2lin(20,0,eq_box.width,15,20000))-15, eq_box.height-17, '20Hz');
  eqbitmap.Canvas.TextOut(round(log2lin(100,0,eq_box.width,15,20000))-25, eq_box.height-17, '100Hz');
  eqbitmap.Canvas.TextOut(round(log2lin(1000,0,eq_box.width,15,20000))-25, eq_box.height-17, '1kHz');
  eqbitmap.Canvas.TextOut(round(log2lin(10000,0,eq_box.width,15,20000))-25, eq_box.height-17, '10kHz');
  {vertical text}
  eqbitmap.Canvas.TextOut(10, round(1 * eq_box.height/8 - 8), '+30dB');
  eqbitmap.Canvas.TextOut(10, round(2 * eq_box.height/8 - 8), '+20dB');
  eqbitmap.Canvas.TextOut(10, round(3 * eq_box.height/8 - 8), '+10dB');
  eqbitmap.Canvas.TextOut(10, round(4 * eq_box.height/8 - 8), '  0dB');
  eqbitmap.Canvas.TextOut(10, round(5 * eq_box.height/8 - 8), '-10dB');
  eqbitmap.Canvas.TextOut(10, round(6 * eq_box.height/8 - 8), '-20dB');
  eqbitmap.Canvas.TextOut(10, round(7 * eq_box.height/8 - 8), '-30dB');
end;

procedure Tmainform.redrawEQs;
var
  fs, f_min, f_max:word;
  a0, a1, a2, b1, b2: single;
  i, j:integer;
  sj: single;

  x: array[0..100] of word;
  y: array[0..100] of single;

  points: array[0..100] of TPoint;
begin
  {general configuration of system}
  fs := 48000;
  f_min := 15;
  f_max := 20000;

  {calculate all PEQs}
  for i:= 0 to 100 do
  begin
    {Linear distribution of pixel}
    {sj := i/100;}
    {Nonlinear distribution - better display in LowFreq-area}
    sj := power(i/100, 4);
    x[i] := round(f_min + sj*(f_max-f_min));
    {set values to zero}
    y[i] := 0;
  end;

  for i:=0 to 4 do
  begin
    calcIIRCoefficients(EQs[i].eq_type, EQs[i].fc, EQs[i].Q, EQs[i].gain, fs, a0, a1, a2, b1, b2);
    for j:= 0 to 100 do
    begin
      y[j] := y[j] + calcFrequencyResponse(a0, a1, a2, b1, b2, x[j], fs);
    end;
  end;

  {convert dB to pixel}
  for i:=0 to 100 do
  begin
    points[i].X := round(log2lin(x[i],0,eq_box.width,15,20000));
    points[i].Y := round((eq_box.height / 2) - (eq_box.height / 2)*(y[i]/40));
  end;

  {draw the logarithmic background}
  drawEQbackground;

  {draw curve}
  eqbitmap.Canvas.Pen.Style := psSolid;
  eqbitmap.Canvas.Pen.Width := 5;
  eqbitmap.Canvas.Pen.Color := clRed;
  eqbitmap.Canvas.MoveTo(0, eq_box.height div 2);
  eqbitmap.Canvas.PolyLine(points);

  {box}
  eqbitmap.Canvas.Brush.Style := bsClear;
  eqbitmap.Canvas.Pen.Width := 1;
  eqbitmap.Canvas.Pen.Style := psSolid;
  eqbitmap.Canvas.Pen.Color := clBlack;
  eqbitmap.Canvas.Rectangle(0,0, eq_box.width, eq_box.height);

  if flag_StartComplete then
  begin
    bitblt(eq_box.canvas.handle, 0, 0, eq_box.Width, eq_box.Height, eqbitmap.canvas.Handle, 0, 0, SRCCOPY);
  end;
end;

procedure Tmainform.UpdateEqControls;
begin
  UpdatingGUI:=true;

  eq_type_dropdown.ItemIndex:=EQs[CurrentEQ].eq_type;
  eq_freq_slider.Position:=round(log2lin(EQs[CurrentEQ].fc,0,10000,15,20000));
  eq_q_slider.position:=round(EQs[CurrentEQ].Q*10);
  eq_gain_slider.position:=round((EQs[CurrentEQ].gain*250)+5000);

  {eq_lbl.caption := 'Selected EQ #' + inttostr(CurrentEQ + 1);}
  if round(EQs[CurrentEQ].fc)<1000 then
    eq_freq_lbl.caption := inttostr(round(EQs[CurrentEQ].fc)) + ' Hz'
  else
    eq_freq_lbl.caption := floattostrf(EQs[CurrentEQ].fc/1000, ffFixed, 7, 1) + ' kHz';
  eq_q_lbl.caption := floattostrf(EQs[CurrentEQ].Q, ffFixed, 15, 1);
  eq_gain_lbl.caption := floattostrf(EQs[CurrentEQ].gain, ffFixed, 15, 1);

  UpdatingGUI:=false;
end;

procedure Tmainform.recalcCOMP;
var
  threshold, ratio, makeup, attack, hold, release2: single;
  ratioInt : byte;

  i:integer;
  overshoot, reduction:single;
  x,y: array[0..60] of single;

  points: array[0..60] of TPoint;
begin
  {copy values}
  threshold := (comp_threshold_slider.position-600)/10;
  if comp_ratio_dropdown.ItemIndex = comp_ratio_dropdown.Items.Count-1 then
  begin
    ratio := 1/1000000;
    ratioInt := 0;
  end else
  begin
    ratioInt := round(power(2, comp_ratio_dropdown.ItemIndex));
    ratio := 1/ratioInt;
  end;
  makeup := comp_makeup_dropdown.ItemIndex * 6;
  attack := comp_attack_slider.Position/1000;
  hold := comp_hold_slider.Position/1000;
  release2 := comp_release_slider.Position/1000;

  {send data to f/bape}
  SendCommand('mixer:comp1@' +
    value2str(threshold) + ',' +
    inttostr(ratioInt) + ',' +
    inttostr(round(makeup)) + ',' +
    value2str(comp_attack_slider.Position) + ',' +
    value2str(comp_hold_slider.Position) + ',' +
    value2str(comp_release_slider.Position) );

  {udpate GUI}
  comp_threshold_lbl.caption := floattostrf(threshold, ffFixed, 7, 1) + 'dB';
  comp_attack_lbl.caption := inttostr(round(attack*1000)) + 'ms';
  comp_hold_lbl.caption := inttostr(round(hold*1000)) + 'ms';
  comp_release_lbl.caption := inttostr(round(release2*1000)) + 'ms';

  {calculate compressor}
  for i:= 0 to 60 do
  begin
    x[i] := i-60;
    y[i] := i-60;

    overshoot := y[i] - threshold;
    if overshoot < 0 then
      overshoot := 0;
    reduction := overshoot * (1 - ratio);
    y[i] := (y[i] - reduction);

    y[i] := y[i] + makeup;

    {convert dB to pixel}
    points[i].x := round(((60+x[i])/60) * comp_box.width);
    points[i].y := comp_box.height - round(((60+y[i])/60) * comp_box.height);
  end;

  {clear drawing}
  compbitmap.Canvas.Brush.Color := clWhite;
  compbitmap.Canvas.Brush.Style := bsSolid;
  compbitmap.Canvas.Rectangle(0,0, comp_box.width, comp_box.height);
  {horizontal lines}
  compbitmap.Canvas.Pen.Color := clSilver;
  compbitmap.Canvas.Pen.Style := psDot;
  for i:=1 to 5 do
  begin
    compbitmap.Canvas.MoveTo(0, round(i * comp_box.height/6));
    compbitmap.Canvas.LineTo(comp_box.width, round(i * comp_box.height/6));
  end;
  {draw vertical lines}
  for i:=1 to 5 do
  begin
    compbitmap.Canvas.MoveTo(round(i * comp_box.width/6), 0);
    compbitmap.Canvas.LineTo(round(i * comp_box.width/6), comp_box.height);
  end;
  {horizontal text}
  compbitmap.Canvas.TextOut(round(1 * comp_box.width/6)-25, comp_box.height-17, '-50dB');
  compbitmap.Canvas.TextOut(round(2 * comp_box.width/6)-25, comp_box.height-17, '-40dB');
  compbitmap.Canvas.TextOut(round(3 * comp_box.width/6)-25, comp_box.height-17, '-30dB');
  compbitmap.Canvas.TextOut(round(4 * comp_box.width/6)-25, comp_box.height-17, '-20dB');
  compbitmap.Canvas.TextOut(round(5 * comp_box.width/6)-25, comp_box.height-17, '-10dB');
  {vertical text}
  compbitmap.Canvas.TextOut(10, round(1 * comp_box.height/6 - 8), '-10dB');
  compbitmap.Canvas.TextOut(10, round(2 * comp_box.height/6 - 8), '-20dB');
  compbitmap.Canvas.TextOut(10, round(3 * comp_box.height/6 - 8), '-30dB');
  compbitmap.Canvas.TextOut(10, round(4 * comp_box.height/6 - 8), '-40dB');
  compbitmap.Canvas.TextOut(10, round(5 * comp_box.height/6 - 8), '-50dB');

  {draw curve}
  compbitmap.Canvas.Pen.Style := psSolid;
  compbitmap.Canvas.Pen.Width := 5;
  compbitmap.Canvas.Pen.Color := clRed;
  compbitmap.Canvas.MoveTo(0, comp_box.height div 2);
  compbitmap.Canvas.PolyLine(points);

  {box}
  compbitmap.Canvas.Brush.Style := bsClear;
  compbitmap.Canvas.Pen.Width := 1;
  compbitmap.Canvas.Pen.Style := psSolid;
  compbitmap.Canvas.Pen.Color := clBlack;
  compbitmap.Canvas.Rectangle(0,0, comp_box.width, comp_box.height);

  if flag_StartComplete then
  begin
    bitblt(comp_box.Canvas.handle, 0, 0, comp_box.Width, comp_box.Height, compbitmap.canvas.Handle, 0, 0, SRCCOPY);
  end;

  {
  comp_chartsource.Clear;
  for i:= 0 to 60 do
  begin
    comp_chartsource.Add(x[i], y[i], '', clRed);
  end;
}
end;

procedure Tmainform.recalcGATE;
var
  threshold, range, attack, hold, release2: single;
begin
  {copy values}
  threshold := (gate_threshold_slider.position-600)/10;
  range := (gate_range_slider.position)/10;
  attack := gate_attack_slider.Position/1000;
  hold := gate_hold_slider.Position/1000;
  release2 := gate_release_slider.Position/1000;

  {send data to f/bape}
  SendCommand('mixer:gate1@' +
    value2str(threshold) + ',' +
    value2str(range) + ',' +
    value2str(gate_attack_slider.Position) + ',' +
    value2str(gate_hold_slider.Position) + ',' +
    value2str(gate_release_slider.Position) );

  {udpate GUI}
  gate_threshold_lbl.caption := floattostrf(threshold, ffFixed, 7, 1) + 'dB';
  gate_range_lbl.caption := floattostrf(range, ffFixed, 7, 1) + 'dB';
  gate_attack_lbl.caption := inttostr(round(attack*1000)) + 'ms';
  gate_hold_lbl.caption := inttostr(round(hold*1000)) + 'ms';
  gate_release_lbl.caption := inttostr(round(release2*1000)) + 'ms';
end;

function Tmainform.value2str(value: single): String;
var
  s: string;
begin
  s := floattostrf(value, ffFixed, 15, 1);
  s[length(s)-1] := '.';
  result := s;
end;

procedure Tmainform.SendVolume(sliderPos: Word; chStr: String);
begin
  {Convert slidervalue between 0...600 to +6dB and -48dB}
  SendCommand('mixer:volume:' + chStr + '@' + value2str((60-sliderPos)/10));
end;

procedure Tmainform.SendCommand(cmd: string);
var
  cmd_crlf: string;
begin
  if comport.Connected then
  begin
    cmd_crlf := cmd + #13 + #10;
    comport.SendData( pchar(@cmd_crlf[1]), length(cmd_crlf) );
  end;
end;

procedure Tmainform.eq_type_dropdownChange(Sender: TObject);
begin
  flag_recalcEQs:=true;
end;

procedure Tmainform.Button1Click(Sender: TObject);
begin
  CurrentEQ := 0;
  UpdateEqControls;
end;

procedure Tmainform.Button2Click(Sender: TObject);
begin
  CurrentEQ := 1;
  UpdateEqControls;
end;

procedure Tmainform.Button3Click(Sender: TObject);
begin
  CurrentEQ := 2;
  UpdateEqControls;
end;

procedure Tmainform.Button4Click(Sender: TObject);
begin
  CurrentEQ := 3;
  UpdateEqControls;
end;

procedure Tmainform.Button5Click(Sender: TObject);
begin
  CurrentEQ := 4;
  UpdateEqControls;
end;

procedure Tmainform.comp_threshold_sliderChange(Sender: TObject);
begin
  flag_recalcComp := true;
end;

procedure Tmainform.connectbtnClick(Sender: TObject);
begin
  {$IFDEF VER80}
    comport.comport := TComPortNumber(ord(comport_dropdown.itemindex));
    case baudrate_dropdown.itemindex of
      0: comport.comportspeed := br9600;
      1: comport.comportspeed := br19200;
      2: comport.comportspeed := br38400;
      3: comport.comportspeed := br57600;
      4: comport.comportspeed := br115200;
    end;
  {$ELSE}
    comport.port := pnCustom;
    comport.portname := 'COM' + inttostr(comport_dropdown.itemindex + 1);
    case baudrate_dropdown.itemindex of
      0: comport.baudrate := br9600;
      1: comport.baudrate := br19200;
      2: comport.baudrate := br38400;
      3: comport.baudrate := br57600;
      4: comport.baudrate := br115200;
    end;
  {$ENDIF}

  if comport.Connect then
  begin
    connectbtn.enabled:=false;
    disconnectbtn.enabled:=true;
    comport_dropdown.enabled:=false;
  end else
  begin
    connectbtn.enabled:=true;
    disconnectbtn.enabled:=false;
    comport_dropdown.enabled:=true;
  end;
end;

procedure Tmainform.disconnectbtnClick(Sender: TObject);
begin
  comport.disconnect;
  connectbtn.enabled:=true;
  disconnectbtn.enabled:=false;
  comport_dropdown.enabled:=true;
end;

{$IFDEF VER80}
procedure Tmainform.comportReceiveData(Sender: TObject; DataPtr: Pointer;
  DataSize: Integer);
{$ELSE}
procedure Tmainform.comportReceiveData(Sender: TObject; DataPtr: Pointer;
  DataSize: Cardinal);
{$ENDIF}
var
  p: pchar;
  s: string;
begin
  if tabnotebook.pageindex = 1 then
  begin
    if PlayerMemo.Lines.Count > 0 then
      s := PlayerMemo.Lines[PlayerMemo.Lines.Count-1]
    else
      s := '';
  end else
  begin
    if RxMemo.Lines.Count > 0 then
      s := RxMemo.Lines[RxMemo.Lines.Count-1]
    else
      s := '';
  end;

  p := DataPtr;
  while DataSize > 0 do
  begin
    case p^ of
      #10:; { LF }
      #13:
        begin
          if tabnotebook.pageindex = 1 then
          begin
            if PlayerMemo.Lines.Count <> 0 then
              PlayerMemo.Lines[PlayerMemo.Lines.Count-1] := s
            else
              PlayerMemo.Lines.Add( s );
            PlayerMemo.Lines.Add( '' );
          end else
          begin
            if RxMemo.Lines.Count <> 0 then
              RxMemo.Lines[RxMemo.Lines.Count-1] := s
            else
              RxMemo.Lines.Add( s );
            RxMemo.Lines.Add( '' );
          end;
          s := '';
        end;
      #8:  { Backspace }
        delete( s, length(s), 1 );
      else { Any other char }
        s := s + p^;
    end;
    dec( DataSize );
    inc( p );
  end;

  if (s<>'') then
  begin
    if tabnotebook.pageindex = 1 then
    begin
      if PlayerMemo.Lines.Count > 0 then
        PlayerMemo.Lines[PlayerMemo.Lines.Count-1] := s
      else
        PlayerMemo.Lines.Add( s );
    end else
    begin
      if RxMemo.Lines.Count > 0 then
        RxMemo.Lines[RxMemo.Lines.Count-1] := s
      else
        RxMemo.Lines.Add( s );
    end;
  end;
end;

procedure Tmainform.tocbtnClick(Sender: TObject);
begin
  SendCommand('player:toc?');
end;

procedure Tmainform.bootloadbtnClick(Sender: TObject);
begin
  SendCommand('samd:update:nina');
end;

procedure Tmainform.infobtnClick(Sender: TObject);
begin
  SendCommand('samd:info?');
  SendCommand('info?');
  SendCommand('fpga:*IDN?');
end;

procedure Tmainform.freq_hpChange(Sender: TObject);
begin
  SendCommand('mixer:eq:hp@' + inttostr(round(freq_hp.value)));
end;

procedure Tmainform.freq_lpChange(Sender: TObject);
begin
  SendCommand('mixer:eq:lp@' + inttostr(round(freq_lp.value)));
end;

procedure Tmainform.volumeanalogsliderChange(Sender: TObject);
var
  s: String;
begin
  s := value2str((60-volumeanalogslider.position)/10);
  SendCommand('mixer:volume:ch1@' + s);
  SendCommand('mixer:volume:ch2@' + s);
  vol_analog_lbl.caption := s + 'dB';
  {
  SendVolume(volumeanalogslider.position, 'ch1');
  SendVolume(volumeanalogslider.position, 'ch2');
  vol_analog_lbl.caption := value2str((60-volumeanalogslider.position)/10) + 'dB';
  }
end;

procedure Tmainform.volumesdcardsliderChange(Sender: TObject);
var
  s:string;
begin
  s := value2str((60-volumesdcardslider.position)/10);
  SendCommand('mixer:volume:ch3@' + s);
  SendCommand('mixer:volume:ch4@' + s);
  vol_sdcard_lbl.caption := s + 'dB';
  {
  SendVolume(volumesdcardslider.position, 'ch3');
  SendVolume(volumesdcardslider.position, 'ch4');
  vol_sdcard_lbl.caption := value2str((60-volumesdcardslider.position)/10) + 'dB';
  }
end;

procedure Tmainform.volumespdifsliderChange(Sender: TObject);
var
  s:string;
begin
  s := value2str((60-volumespdifslider.position)/10);
  SendCommand('mixer:volume:ch5@' + s);
  SendCommand('mixer:volume:ch6@' + s);
  vol_spdif_lbl.caption := s + 'dB';
  {
  SendVolume(volumespdifslider.position, 'ch5');
  SendVolume(volumespdifslider.position, 'ch6');
  vol_spdif_lbl.caption := value2str((60-volumespdifslider.position)/10) + 'dB';
  }
end;

procedure Tmainform.volumemainsliderChange(Sender: TObject);
var
  s:string;
begin
  s := value2str((60-volumemainslider.position)/10);
  SendCommand('mixer:volume:main@' + s);
  vol_main_lbl.caption := s + 'dB';
  {
  SendVolume(volumemainslider.position, 'main');
  vol_main_lbl.caption := value2str((60-volumemainslider.position)/10) + 'dB';
  }
end;

procedure Tmainform.volumesubsliderChange(Sender: TObject);
var
  s:string;
begin
  s := value2str((60-volumesubslider.position)/10);
  SendCommand('mixer:volume:sub@' + s);
  vol_sub_lbl.caption := s + 'dB';
  {
  SendVolume(volumesubslider.position, 'sub');
  vol_sub_lbl.caption := value2str((60-volumesubslider.position)/10) + 'dB';
  }
end;

procedure Tmainform.volume_ultranet_slider1Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider1.position, 'ch7');
end;

procedure Tmainform.volume_ultranet_slider2Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider2.position, 'ch8');
end;

procedure Tmainform.volume_ultranet_slider3Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider3.position, 'ch9');
end;

procedure Tmainform.volume_ultranet_slider4Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider4.position, 'ch10');
end;

procedure Tmainform.volume_ultranet_slider5Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider5.position, 'ch11');
end;

procedure Tmainform.volume_ultranet_slider6Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider6.position, 'ch12');
end;

procedure Tmainform.volume_ultranet_slider7Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider7.position, 'ch13');
end;

procedure Tmainform.volume_ultranet_slider8Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider8.position, 'ch14');
end;

procedure Tmainform.volume_ultranet_slider9Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider9.position, 'ch15');
end;

procedure Tmainform.volume_ultranet_slider10Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider10.position, 'ch16');
end;

procedure Tmainform.volume_ultranet_slider11Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider11.position, 'ch17');
end;

procedure Tmainform.volume_ultranet_slider12Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider12.position, 'ch18');
end;

procedure Tmainform.volume_ultranet_slider13Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider13.position, 'ch19');
end;

procedure Tmainform.volume_ultranet_slider14Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider14.position, 'ch20');
end;

procedure Tmainform.volume_ultranet_slider15Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider15.position, 'ch21');
end;

procedure Tmainform.volume_ultranet_slider16Change(Sender: TObject);
begin
  SendVolume(volume_ultranet_slider16.position, 'ch22');
end;

procedure Tmainform.syncsourcedropdownChange(Sender: TObject);
begin
  SendCommand('mixer:sync@' + inttostr(syncsourcedropdown.itemindex));
end;

procedure Tmainform.sampleratedropdownChange(Sender: TObject);
begin
  case sampleratedropdown.itemindex of
    0: SendCommand('mixer:samplerate@44100');
    1: SendCommand('mixer:samplerate@48000');
    2: SendCommand('mixer:samplerate@96000');
  end;
end;

procedure Tmainform.mixerresetbtnClick(Sender: TObject);
begin
  SendCommand('mixer:reset');
end;

procedure Tmainform.reseteqbtnClick(Sender: TObject);
begin
  SendCommand('mixer:eq:reset');
end;

procedure Tmainform.resetdynamicsbtnClick(Sender: TObject);
begin
  SendCommand('mixer:dynamics:reset');
end;

procedure Tmainform.initsystembtnClick(Sender: TObject);
begin
  SendCommand('system:init');
end;

procedure Tmainform.resetmanibtnClick(Sender: TObject);
begin
  SendCommand('samd:reset:nina');
end;

procedure Tmainform.playbtnClick(Sender: TObject);
begin
  SendCommand('player:play');
end;

procedure Tmainform.loadbtnClick(Sender: TObject);
begin
  SendCommand('player:file@' + filenameedit.text);
end;

procedure Tmainform.pausebtnClick(Sender: TObject);
begin
  SendCommand('player:pause');
end;

procedure Tmainform.stopbtnClick(Sender: TObject);
begin
  SendCommand('player:stop');
end;

procedure Tmainform.loopcheckboxMouseUp(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if loopcheckbox.checked then
    SendCommand('player:loop@1')
  else
    SendCommand('player:loop@0');
end;

procedure Tmainform.loopcheckboxKeyUp(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if loopcheckbox.checked then
    SendCommand('player:loop@1')
  else
    SendCommand('player:loop@0');
end;

procedure Tmainform.btonbtnClick(Sender: TObject);
begin
  SendCommand('wifi:enabled@0');
  SendCommand('bt:enabled@1');
end;

procedure Tmainform.btoffbtnClick(Sender: TObject);
begin
  SendCommand('bt:enabled@0');
  SendCommand('wifi:enabled@1');
end;

procedure Tmainform.btplaybtnClick(Sender: TObject);
begin
  SendCommand('bt:play');
end;

procedure Tmainform.btpausebtnClick(Sender: TObject);
begin
  SendCommand('bt:pause');
end;

procedure Tmainform.btstopbtnClick(Sender: TObject);
begin
  SendCommand('bt:stop');
end;

procedure Tmainform.btprevbtnClick(Sender: TObject);
begin
  SendCommand('bt:prev');
end;

procedure Tmainform.btffbtnClick(Sender: TObject);
begin
  SendCommand('bt:ff');
end;

procedure Tmainform.btnextbtnClick(Sender: TObject);
begin
  SendCommand('bt:next');
end;

procedure Tmainform.btrwbtnClick(Sender: TObject);
begin
  SendCommand('bt:rw');
end;

procedure Tmainform.wifioffbtnClick(Sender: TObject);
begin
  SendCommand('wifi:enabled@0');
end;

procedure Tmainform.wifionbtnClick(Sender: TObject);
begin
  SendCommand('wifi:enabled@1');
end;

procedure Tmainform.wifisendbtnClick(Sender: TObject);
begin
  SendCommand('wifi:ssid@' + wifissidedit.text);
  SendCommand('wifi:password@' + wifipasswordedit.text);
  if wifiapcheckbox.checked then
    SendCommand('wifi:mode@1')
  else
    SendCommand('wifi:mode@0');
end;

procedure Tmainform.wifireadipbtnClick(Sender: TObject);
begin
  SendCommand('wifi:ip?');
end;

procedure Tmainform.tabnotebookClick(Sender: TObject);
begin
  eqbitmap.width := eq_box.width;
  eqbitmap.height := eq_box.height;
  compbitmap.width := comp_box.width;
  compbitmap.height := comp_box.height;

  if tabnotebook.pageindex = 2 then
  begin
    flag_recalcEQs := true;
  end else if tabnotebook.pageindex = 3 then
  begin
    flag_recalcComp := true;
  end;
end;

procedure Tmainform.FormResize(Sender: TObject);
begin
  if flag_StartComplete then
  begin
    eqbitmap.width := eq_box.width;
    eqbitmap.height := eq_box.height;
    compbitmap.width := comp_box.width;
    compbitmap.height := comp_box.height;

    if (tabnotebook.pageindex = 2) then
    begin
      flag_recalcEQs := true;
    end;
    if (tabnotebook.pageindex = 3) then
    begin
      flag_recalcCOMP := true;
    end;
  end;
end;

procedure Tmainform.gate_threshold_sliderChange(Sender: TObject);
begin
  flag_recalcGate := true;
end;

procedure Tmainform.cmdbtnClick(Sender: TObject);
var
   i:integer;
begin
  for i:=0 to cmdmemo.lines.count-1 do
  begin
    SendCommand(cmdmemo.lines[i]);
  end;
end;

procedure Tmainform.FormShow(Sender: TObject);
begin
  if FirstStart then
  begin
    FirstStart:=false;

    {go to first tab}
    tabnotebook.pageindex:=0;

    {check if we can load some commands}
    if FileExists('config.txt') then
    begin
      cmdmemo.Lines.LoadFromFile('config.txt');
    end;
  end;
end;

procedure Tmainform.FormCreate(Sender: TObject);
begin
  FirstStart:=true;
end;

end.
