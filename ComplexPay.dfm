object ComplexPayForm: TComplexPayForm
  Left = 960
  Top = 159
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Комбинированный расчет'
  ClientHeight = 347
  ClientWidth = 529
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ComplexPayGrid: TStringGrid
    Left = 8
    Top = 16
    Width = 513
    Height = 169
    ColCount = 4
    RowCount = 2
    TabOrder = 0
  end
  object ButtonNalPay: TButton
    Left = 8
    Top = 200
    Width = 145
    Height = 41
    Caption = 'Наличные'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = 17
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = ButtonNalPayClick
  end
  object ButtonBezPay: TButton
    Left = 8
    Top = 248
    Width = 145
    Height = 41
    Caption = 'Банковская карта'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = ButtonBezPayClick
  end
  object ButtonPKPay: TButton
    Left = 8
    Top = 296
    Width = 145
    Height = 41
    Caption = 'Подарочная карта'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
  end
  object PayGroupBox: TGroupBox
    Left = 288
    Top = 224
    Width = 233
    Height = 105
    TabOrder = 4
    object PayEdit: TEdit
      Left = 8
      Top = 24
      Width = 217
      Height = 57
      BiDiMode = bdRightToLeftNoAlign
      BorderStyle = bsNone
      Color = clMenu
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -50
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentBiDiMode = False
      ParentFont = False
      ReadOnly = True
      TabOrder = 0
      Text = '0.00'
      OnKeyPress = PayEditKeyPress
    end
  end
  object PayButton: TButton
    Left = 160
    Top = 200
    Width = 121
    Height = 137
    Caption = 'Пробить чек'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    OnClick = PayButtonClick
  end
  object ApplicationEvents1: TApplicationEvents
    OnMessage = ApplicationEvents1Message
    Left = 488
    Top = 192
  end
end
