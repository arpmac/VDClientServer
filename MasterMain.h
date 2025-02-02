//---------------------------------------------------------------------------

#ifndef MasterMainH
#define MasterMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.Win.ScktComp.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------

class TConnection {
private:
	eCnxStatus  FCnxStatus;
	time_t      FCnxTime;
	bool        FReadPhase;

	sTcpPacket net_pack;

	TServerSocket* FServerSocket;

	short cmd_id;
	short man_cmd;

	const int WhatShallIDo(const time_t& act_time);
	const int FormatTheMessage(void);
	void VerifyConnIsAlive(const time_t& act_time);
	bool SendTheMsg(void* buf, const int size);
	int ReceiveTheMsg(void* buf);
	bool VerifyPackage(const int msg_idx, const int cmd);
	void MessageDecode(const int &_cmd, const int &_bufflen);

	void EmptySocketBuffer(void);

	void __fastcall ServerSocketClientConnect(TObject *Sender,
		  TCustomWinSocket *Socket);
	void __fastcall ServerSocketClientDisconnect(TObject *Sender,
		  TCustomWinSocket *Socket);
	void __fastcall ServerSocketClientError(TObject *Sender,
		  TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
		  int &ErrorCode);
	void __fastcall ServerSocketClientAccept(TObject *Sender, TCustomWinSocket *Socket);


public:
	short act_msg_index;

	void LoopTcp(const time_t& act_time);

	bool IsConnected(){
		if (FCnxStatus != csDisconnected && act_msg_index > 5)
			return true;
		return false;
	}

	__fastcall TConnection();
	__fastcall ~TConnection();
};

class TServerForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TLabel *Label1;
	TEdit *Edit2;
	TLabel *Label2;
	TEdit *Edit3;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TLabel *Label3;
	TTimer *Timer1;
	TServerSocket *ServerSocket1;
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);




private:	// User declarations
    String Str;
public:		// User declarations
	__fastcall TServerForm(TComponent* Owner);
	__fastcall ~TServerForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TServerForm *ServerForm;
//---------------------------------------------------------------------------
#endif
