object Saver: TSaver
  Left = 109
  Top = 93
  BorderIcons = []
  BorderStyle = bsNone
  Caption = 'Saver'
  ClientHeight = 358
  ClientWidth = 559
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMaximized
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label: TLabel
    Left = 169
    Top = 111
    Width = 50
    Height = 28
    Caption = 'Text'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWhite
    Font.Height = -25
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 20
    OnTimer = Timer1Timer
    Left = 456
    Top = 48
  end
end
