object ClientForm: TClientForm
  Left = 0
  Top = 0
  Caption = 'ClientForm'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 56
    Top = 27
    Width = 24
    Height = 13
    Caption = 'Type'
  end
  object Label2: TLabel
    Left = 56
    Top = 54
    Width = 11
    Height = 13
    Caption = 'ID'
  end
  object Memo1: TMemo
    Left = 320
    Top = 51
    Width = 185
    Height = 89
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object CheckBox1: TCheckBox
    Left = 104
    Top = 82
    Width = 97
    Height = 17
    Caption = 'has_prep_tank'
    TabOrder = 1
  end
  object CheckBox2: TCheckBox
    Left = 104
    Top = 105
    Width = 97
    Height = 17
    Caption = 'has_dyeing_kitchen'
    TabOrder = 2
  end
  object CheckBox3: TCheckBox
    Left = 104
    Top = 128
    Width = 97
    Height = 17
    Caption = 'has_reduced_load'
    TabOrder = 3
  end
  object Edit2: TEdit
    Left = 104
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 4
  end
  object Edit3: TEdit
    Left = 104
    Top = 51
    Width = 121
    Height = 21
    TabOrder = 5
  end
  object Button3: TButton
    Left = 120
    Top = 160
    Width = 75
    Height = 25
    Caption = 'Update'
    TabOrder = 6
    OnClick = Button3Click
  end
  object ClientSocket1: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    Left = 112
    Top = 216
  end
end
