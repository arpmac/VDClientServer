//---------------------------------------------------------------------------

#ifndef LocalMainH
#define LocalMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.Win.ScktComp.hpp>
//---------------------------------------------------------------------------

class TSocketConn {
private:
	TClientSocket* FClientSocket;
	sTcpPacket net_pack;

	int SocketRead(void* _buffer);
	void PurgeSocket(void);
	void SocketWrite(void* _buffer, const int &_size){FClientSocket->Socket->SendBuf(_buffer, _size);}
	void DoWhatRequired(const int& _cmd, const int& _bufflen);
	int FormatTheAnswer(int);

public:
	bool IsConnected;
	std::wstring msg;

	void TimerTCP(void);
	__fastcall TSocketConn();
	__fastcall ~TSocketConn();
};
//---------------------------------------------------------------------------

class TClientForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TClientSocket *ClientSocket1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TEdit *Edit2;
	TEdit *Edit3;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Button3;
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);

//    TClientSocket *CSocket1;
private:	// User declarations
	String Str;
public:		// User declarations
	__fastcall TClientForm(TComponent* Owner);
	__fastcall ~TClientForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TClientForm *ClientForm;
//---------------------------------------------------------------------------
#endif
