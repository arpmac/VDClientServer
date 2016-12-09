object ServerForm: TServerForm
  Left = 0
  Top = 0
  Caption = 'ServerForm'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 59
    Width = 24
    Height = 13
    Caption = 'Type'
  end
  object Label2: TLabel
    Left = 40
    Top = 86
    Width = 11
    Height = 13
    Caption = 'ID'
  end
  object Label3: TLabel
    Left = 80
    Top = 24
    Width = 78
    Height = 13
    Caption = 'INFO from client'
  end
  object Memo1: TMemo
    Left = 264
    Top = 135
    Width = 185
    Height = 89
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object Edit2: TEdit
    Left = 88
    Top = 56
    Width = 121
    Height = 21
    TabOrder = 1
  end
  object Edit3: TEdit
    Left = 88
    Top = 83
    Width = 121
    Height = 21
    TabOrder = 2
  end
  object CheckBox1: TCheckBox
    Left = 88
    Top = 114
    Width = 97
    Height = 17
    Caption = 'has_prep_tank'
    TabOrder = 3
  end
  object CheckBox2: TCheckBox
    Left = 88
    Top = 137
    Width = 97
    Height = 17
    Caption = 'has_dyeing_kitchen'
    TabOrder = 4
  end
  object CheckBox3: TCheckBox
    Left = 88
    Top = 160
    Width = 97
    Height = 17
    Caption = 'has_reduced_load'
    TabOrder = 5
  end
  object Timer1: TTimer
    Interval = 500
    OnTimer = Timer1Timer
    Left = 376
    Top = 32
  end
  object ServerSocket1: TServerSocket
    Active = False
    Port = 0
    ServerType = stNonBlocking
    Left = 496
    Top = 160
  end
end
