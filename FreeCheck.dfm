object FreeCheckForm: TFreeCheckForm
  Left = 264
  Top = 117
  Width = 769
  Height = 444
  ActiveControl = Text
  Caption = '������ ���������� ����'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 16
    Width = 132
    Height = 24
    Caption = '����� �� ����'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 16
    Top = 56
    Width = 114
    Height = 24
    Caption = '����� ����'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Text: TEdit
    Left = 160
    Top = 16
    Width = 441
    Height = 30
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object Summ: TEdit
    Left = 160
    Top = 56
    Width = 441
    Height = 30
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    Text = '0.00'
  end
  object Cancel: TButton
    Left = 64
    Top = 296
    Width = 97
    Height = 25
    Caption = '������'
    TabOrder = 2
    OnClick = CancelClick
  end
  object Ok: TButton
    Left = 200
    Top = 296
    Width = 105
    Height = 25
    Caption = '�������'
    TabOrder = 3
    OnClick = OkClick
  end
  object RadioGroupNDS: TRadioGroup
    Left = 40
    Top = 112
    Width = 137
    Height = 161
    Caption = '���'
    ItemIndex = 2
    Items.Strings = (
      '��� 0%'
      '��� 10%'
      '���18%')
    TabOrder = 4
  end
  object RadioGroupPayType: TRadioGroup
    Left = 200
    Top = 112
    Width = 137
    Height = 161
    Caption = '����� ������'
    ItemIndex = 0
    Items.Strings = (
      '���������'
      '������')
    TabOrder = 5
  end
end
