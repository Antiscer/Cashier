object MainWindow: TMainWindow
  Left = 6
  Top = 142
  BiDiMode = bdLeftToRight
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Колорлон.Кассир.7.7 Онлайн'
  ClientHeight = 657
  ClientWidth = 1346
  Color = clActiveBorder
  DockSite = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  Menu = Menu
  OldCreateOrder = False
  ParentBiDiMode = False
  Visible = True
  WindowState = wsMaximized
  OnClose = FormClose
  OnCreate = FormCreate
  OnDblClick = FormDblClick
  OnKeyPress = FormKeyPress
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 10
    Top = 481
    Width = 62
    Height = 19
    Anchors = [akLeft, akBottom]
    Caption = 'Сумма:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 10
    Top = 534
    Width = 87
    Height = 19
    Anchors = [akLeft, akBottom]
    Caption = 'Получено:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 10
    Top = 631
    Width = 57
    Height = 19
    Anchors = [akLeft, akBottom]
    Caption = 'Сдача:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 13
    Top = 13
    Width = 37
    Height = 19
    Caption = 'Код:'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlue
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 232
    Top = 13
    Width = 46
    Height = 19
    Caption = 'Цена:'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlue
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 395
    Top = 13
    Width = 63
    Height = 19
    Caption = 'Кол-во:'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlue
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Unit: TLabel
    Left = 555
    Top = 21
    Width = 30
    Height = 19
    Caption = '      '
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Name: TLabel
    Left = 13
    Top = 46
    Width = 43
    Height = 39
    AutoSize = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object Label7: TLabel
    Left = 346
    Top = 537
    Width = 66
    Height = 19
    Anchors = [akLeft, akBottom]
    Caption = 'Скидка:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object IDNom: TLabel
    Left = 808
    Top = 0
    Width = 201
    Height = 33
    AutoSize = False
    Caption = '123'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object Label8: TLabel
    Left = 1031
    Top = 473
    Width = 242
    Height = 19
    Anchors = [akRight, akBottom]
    Caption = 'Подарочные карты к оплате:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label9: TLabel
    Left = 10
    Top = 590
    Width = 57
    Height = 19
    Anchors = [akLeft, akBottom]
    Caption = 'Сдача:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PresentLabel: TLabel
    Left = 1019
    Top = 61
    Width = 243
    Height = 24
    Alignment = taRightJustify
    Anchors = [akRight]
    Caption = 'Сканирование подарков'
    Color = clRed
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    Visible = False
  end
  object Label12: TLabel
    Left = 824
    Top = 440
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label13: TLabel
    Left = 792
    Top = 440
    Width = 43
    Height = 19
    Caption = 'Итог:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object TSum: TEdit
    Left = 104
    Top = 464
    Width = 235
    Height = 53
    TabStop = False
    Anchors = [akLeft, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -39
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
    Text = '0.00'
  end
  object Recived: TEdit
    Left = 104
    Top = 520
    Width = 235
    Height = 53
    TabStop = False
    Anchors = [akLeft, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -39
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 4
    Text = '0.00'
  end
  object Change: TEdit
    Left = 104
    Top = 573
    Width = 235
    Height = 53
    TabStop = False
    Anchors = [akLeft, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -39
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
    Text = '0.00'
  end
  object Grid: TStringGrid
    Left = 0
    Top = 91
    Width = 1574
    Height = 70
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    ColCount = 16
    DefaultColWidth = 80
    DefaultRowHeight = 20
    RowCount = 2
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    ScrollBars = ssNone
    TabOrder = 1
    ColWidths = (
      80
      100
      80
      80
      80
      80
      80
      80
      80
      80
      80
      80
      80
      80
      80
      80)
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 632
    Width = 1346
    Height = 25
    Panels = <
      item
        Width = 50
      end
      item
        Alignment = taCenter
        Width = 250
      end
      item
        Alignment = taCenter
        Width = 170
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 140
      end
      item
        Width = 120
      end>
    SimplePanel = False
  end
  object Nnum: TEdit
    Left = 52
    Top = 7
    Width = 149
    Height = 32
    Hint = '213'
    TabStop = False
    Color = clBtnFace
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Pitch = fpFixed
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ReadOnly = True
    ShowHint = False
    TabOrder = 2
  end
  object Price: TEdit
    Left = 284
    Top = 7
    Width = 98
    Height = 32
    TabStop = False
    Color = clBtnFace
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Pitch = fpFixed
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 7
  end
  object Qnty: TEdit
    Left = 460
    Top = 7
    Width = 85
    Height = 32
    TabStop = False
    Color = clBtnFace
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Pitch = fpFixed
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 8
  end
  object TOff: TEdit
    Left = 344
    Top = 560
    Width = 235
    Height = 53
    TabStop = False
    Anchors = [akLeft, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -39
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 9
    Text = '0.00'
    Visible = False
  end
  object GiftCardM: TMemo
    Left = 911
    Top = 489
    Width = 454
    Height = 129
    TabStop = False
    Anchors = [akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      '')
    ParentFont = False
    ReadOnly = True
    TabOrder = 10
    WordWrap = False
  end
  object PresentGrid: TStringGrid
    Left = 417
    Top = 461
    Width = 537
    Height = 165
    TabStop = False
    Anchors = [akBottom]
    ColCount = 9
    DefaultColWidth = 80
    DefaultRowHeight = 20
    RowCount = 2
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    GridLineWidth = 2
    ParentFont = False
    ScrollBars = ssNone
    TabOrder = 0
  end
  object ComboPayGrid: TStringGrid
    Left = 1002
    Top = 451
    Width = 337
    Height = 177
    Anchors = [akRight, akBottom]
    DefaultRowHeight = 20
    RowCount = 2
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 11
    OnDblClick = ComboPayGridDblClick
    OnDrawCell = ComboPayGridDrawCell
    OnKeyPress = ComboPayGridKeyPress
  end
  object panelDelivery: TPanel
    Left = 192
    Top = 64
    Width = 993
    Height = 433
    TabOrder = 12
    Visible = False
    object lbPickup: TLabel
      Left = 16
      Top = 8
      Width = 153
      Height = 18
      Caption = 'Забирает покупатель'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object lbDelivery: TLabel
      Left = 512
      Top = 8
      Width = 80
      Height = 18
      Caption = 'В доставку'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Bevel11: TBevel
      Left = 64
      Top = 376
      Width = 433
      Height = 15
      Shape = bsTopLine
    end
    object Bevel21: TBevel
      Left = 544
      Top = 352
      Width = 433
      Height = 15
      Shape = bsTopLine
    end
    object lbItog1: TLabel
      Left = 360
      Top = 384
      Width = 43
      Height = 19
      Caption = 'Итог:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbItog2: TLabel
      Left = 832
      Top = 360
      Width = 43
      Height = 19
      Caption = 'Итог:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel12: TBevel
      Left = 64
      Top = 408
      Width = 433
      Height = 15
      Shape = bsTopLine
    end
    object Bevel22: TBevel
      Left = 544
      Top = 384
      Width = 433
      Height = 15
      Shape = bsTopLine
    end
    object PickupSum: TEdit
      Left = 409
      Top = 384
      Width = 88
      Height = 21
      BiDiMode = bdLeftToRight
      BorderStyle = bsNone
      Color = clMenu
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentBiDiMode = False
      ParentFont = False
      TabOrder = 0
      Text = '0.00'
    end
    object DeliverySum: TEdit
      Left = 881
      Top = 360
      Width = 88
      Height = 21
      BiDiMode = bdLeftToRight
      BorderStyle = bsNone
      Color = clMenu
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentBiDiMode = False
      ParentFont = False
      TabOrder = 1
      Text = '0.00'
    end
    object PickupGrid: TStringGrid
      Left = 16
      Top = 32
      Width = 465
      Height = 321
      RowCount = 2
      TabOrder = 2
      OnDrawCell = PickupGridDrawCell
    end
    object DeliveryGrid: TStringGrid
      Left = 488
      Top = 32
      Width = 489
      Height = 305
      ColCount = 6
      RowCount = 2
      TabOrder = 3
      OnDrawCell = DeliveryGridDrawCell
    end
  end
  object Menu: TMainMenu
    Left = 664
    Top = 176
    object N1: TMenuItem
      Caption = 'Действие'
      ShortCut = 16469
      object N5: TMenuItem
        Caption = 'Количество'
        ShortCut = 113
        OnClick = MeasureClick
      end
      object N6: TMenuItem
        Caption = 'Цена'
        ShortCut = 115
        OnClick = PriceChangeClick
      end
      object N7: TMenuItem
        Caption = 'Удаление позиции'
        ShortCut = 16462
        OnClick = DeleteNnumClick
      end
      object N8: TMenuItem
        Caption = 'Удаление списка'
        ShortCut = 49232
        OnClick = DeleteAllClick
      end
      object ComboPayCancel: TMenuItem
        Caption = 'Очистка списка платежей'
        ShortCut = 49234
        OnClick = ComboPayCancelClick
      end
      object L1: TMenuItem
        Caption = 'Дисплей инициализировать'
        ShortCut = 16452
      end
      object L2: TMenuItem
        Caption = 'Проверка связи с сервером'
        ShortCut = 16460
        OnClick = L2Click
      end
      object ClearPresent: TMenuItem
        Caption = 'Очистить подарки'
        ShortCut = 16455
        OnClick = ClearPresentClick
      end
      object N9: TMenuItem
        Caption = 'Выход'
        ShortCut = 16453
        OnClick = ExitClick
      end
    end
    object N10: TMenuItem
      Caption = 'Регистратор'
      object N11: TMenuItem
        Caption = 'Открыть смену'
        ShortCut = 16463
        OnClick = S_OpenClick
      end
      object N12: TMenuItem
        Caption = 'Закрыть смену'
        ShortCut = 16474
        OnClick = S_CloseClick
      end
      object N13: TMenuItem
        Caption = 'Промежуточный отчет'
        ShortCut = 16472
        OnClick = X_ReportClick
      end
      object N19: TMenuItem
        Caption = 'Итог подарочные карты'
        ShortCut = 16464
        OnClick = N19Click
      end
      object N14: TMenuItem
        Caption = 'Открыть ящик'
        ShortCut = 116
        OnClick = BoxClick
      end
      object N15: TMenuItem
        Caption = 'Копия чека на принтере'
        ShortCut = 118
        OnClick = CopyCheckClick
      end
      object COL: TMenuItem
        Caption = 'Копия на чековой ленте'
        ShortCut = 16502
        OnClick = COLClick
      end
      object DelLastBill: TMenuItem
        Caption = 'Отмена последнего чека'
        ShortCut = 16503
        OnClick = DelLastBillClick
      end
      object N17: TMenuItem
        Caption = 'Подвести часы ФР'
        OnClick = N17Click
      end
      object N18: TMenuItem
        Caption = 'Большой чек'
        Hint = 'Перевод ФР в режим печати чека построчно'
        ShortCut = 16504
        OnClick = N18Click
      end
    end
    object Bill: TMenuItem
      Caption = 'Чек'
      object Cash: TMenuItem
        Caption = 'Наличные'
        ShortCut = 121
        OnClick = CashClick
      end
      object SB: TMenuItem
        Caption = 'Магнитные карты'
        ShortCut = 16450
        OnClick = SBClick
      end
      object SBManual: TMenuItem
        Caption = 'Терминал СБ (автономно)'
        OnClick = SBManualClick
      end
      object Return: TMenuItem
        Caption = 'Возврат ошибочного чека'
        ShortCut = 49238
        OnClick = ReturnClick
      end
      object N4: TMenuItem
        AutoHotkeys = maAutomatic
        Caption = 'Сверка итогов'
        ShortCut = 49235
        OnClick = N4Click
      end
      object N16: TMenuItem
        Caption = 'Запись в учет'
        ShortCut = 49239
        OnClick = N16Click
      end
      object FreeCheck: TMenuItem
        Caption = 'Пробить свободный чек'
        ShortCut = 49222
        OnClick = FreeCheckClick
      end
    end
    object N2: TMenuItem
      Caption = 'Терминал СБ'
      object SBDayClose: TMenuItem
        Caption = 'Закрытие дня'
        ShortCut = 49220
        OnClick = SBDayCloseClick
      end
      object CheckRepeat: TMenuItem
        Caption = 'Печать последнего чека'
        ShortCut = 49234
        OnClick = CheckRepeatClick
      end
      object ReturnBC: TMenuItem
        Caption = 'Возврат на БК'
      end
    end
    object N3: TMenuItem
      Caption = 'Доставка'
      object DeliveryInit: TMenuItem
        Caption = 'Вернуть к первоначальному'
        OnClick = DeliveryInitClick
      end
      object DeliveryDoc: TMenuItem
        Caption = 'На доставку'
        ShortCut = 16506
        OnClick = DeliveryDocClick
      end
      object InvertGrids: TMenuItem
        Caption = 'Обменять панели'
        ShortCut = 49228
        OnClick = InvertGridsClick
      end
      object DeliveryDocRepeat: TMenuItem
        Caption = 'Повторить документ'
        ShortCut = 122
        OnClick = DeliveryDocRepeatClick
      end
    end
  end
  object PriceQuery: TADOQuery
    AutoCalcFields = False
    CacheSize = 28672
    Connection = CashConnection
    LockType = ltReadOnly
    Parameters = <
      item
        Size = -1
        Value = Null
      end>
    SQL.Strings = (
      '')
    Left = 592
    Top = 176
  end
  object CashConnection: TADOConnection
    ConnectionTimeout = 30
    DefaultDatabase = 'DB_Cash'
    LoginPrompt = False
    Provider = 'SQLOLEDB'
    Left = 544
    Top = 176
  end
  object CentralQuery: TADOQuery
    AutoCalcFields = False
    CacheSize = 28672
    Connection = CentralConnection
    LockType = ltReadOnly
    Parameters = <>
    SQL.Strings = (
      '')
    Left = 592
    Top = 216
  end
  object CentralConnection: TADOConnection
    KeepConnection = False
    LoginPrompt = False
    Provider = 'SQLOLEDB'
    Left = 544
    Top = 216
  end
end
