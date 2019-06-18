object frReturnForm: TfrReturnForm
  Left = 209
  Top = 164
  Width = 815
  Height = 564
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'Возврат по чеку'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  OnCreate = FormCreate
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object gbReturnCheck: TGroupBox
    Left = 16
    Top = 32
    Width = 769
    Height = 209
    Caption = 'Возврат по чеку'
    TabOrder = 0
    object sgCheckList: TStringGrid
      Left = 0
      Top = 56
      Width = 761
      Height = 153
      RowCount = 2
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 0
      OnDblClick = sgCheckListDblClick
      OnDrawCell = sgCheckListDrawCell
    end
    object fldFilter: TEdit
      Left = 8
      Top = 16
      Width = 225
      Height = 21
      Enabled = False
      TabOrder = 1
      Visible = False
    end
    object sgOneCheck: TStringGrid
      Left = 0
      Top = 56
      Width = 769
      Height = 153
      RowCount = 2
      ScrollBars = ssVertical
      TabOrder = 2
      Visible = False
      OnDblClick = sgCheckListDblClick
      ColWidths = (
        64
        121
        139
        113
        112)
    end
    object DateTimePicker: TDateTimePicker
      Left = 576
      Top = 16
      Width = 186
      Height = 21
      CalAlignment = dtaLeft
      Date = 43348.4707296875
      Time = 43348.4707296875
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkDate
      ParseInput = False
      TabOrder = 3
      OnChange = DateTimePickerChange
    end
  end
  object rbReturnCheck: TRadioButton
    Left = 8
    Top = 16
    Width = 113
    Height = 17
    Checked = True
    TabOrder = 1
    TabStop = True
  end
  object rbReturnFree: TRadioButton
    Left = 8
    Top = 256
    Width = 113
    Height = 17
    TabOrder = 2
  end
  object gbReturnFree: TGroupBox
    Left = 16
    Top = 272
    Width = 769
    Height = 121
    Caption = 'Возврат по сумме'
    TabOrder = 3
  end
  object ReturnBtn: TButton
    Left = 56
    Top = 424
    Width = 137
    Height = 81
    Caption = 'Возврат'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnClick = ReturnBtnClick
  end
  object CancelBtn: TButton
    Left = 552
    Top = 424
    Width = 147
    Height = 81
    Caption = 'Закрыть'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    OnClick = CancelBtnClick
  end
  object plConfirm: TPanel
    Left = 88
    Top = 8
    Width = 633
    Height = 505
    Color = clHighlightText
    TabOrder = 6
    Visible = False
    object lbSum: TLabel
      Left = 104
      Top = 248
      Width = 52
      Height = 23
      Caption = 'ИТОГ'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object btnFRandDB: TButton
      Left = 32
      Top = 424
      Width = 185
      Height = 49
      Caption = 'Возврат ФР с удалением из БД'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -14
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = btnFRandDBClick
    end
    object btnFRonly: TButton
      Left = 256
      Top = 424
      Width = 193
      Height = 49
      Caption = 'Возврат только ФР'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -14
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = btnFRonlyClick
    end
    object btnCancelP: TButton
      Left = 488
      Top = 424
      Width = 121
      Height = 49
      Caption = 'Отмена'
      TabOrder = 2
      OnClick = btnCancelPClick
    end
    object edBillNumber: TMaskEdit
      Left = 160
      Top = 16
      Width = 305
      Height = 33
      Cursor = crIBeam
      BorderStyle = bsNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
    end
    object sgThingsList: TStringGrid
      Left = 56
      Top = 56
      Width = 505
      Height = 193
      BorderStyle = bsNone
      ColCount = 3
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Options = []
      ParentColor = True
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 4
      ColWidths = (
        363
        118
        64)
    end
    object sgPays: TStringGrid
      Left = 64
      Top = 288
      Width = 505
      Height = 121
      BorderStyle = bsNone
      ColCount = 2
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Options = [goFixedHorzLine, goHorzLine, goRowSelect]
      ParentColor = True
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 5
      ColWidths = (
        363
        118)
    end
    object edSum: TEdit
      Left = 416
      Top = 240
      Width = 121
      Height = 32
      BorderStyle = bsNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 6
    end
  end
end
