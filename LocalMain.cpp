//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Common.h"
#include "Thread_TCP.h"
#include "LocalMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

using namespace std;

TClientForm *ClientForm;

ThreadTCP *ComServerThread = NULL;
TSocketConn *Socket = NULL;
MachineInfo MI;

//---------------------------------------------------------------------------
__fastcall TClientForm::TClientForm(TComponent* Owner)
	: TForm(Owner)
{
	MI.machine_type = 0;
	MI.mac_id = String("").c_str();
	MI.has_prep_tank = true;
	MI.has_dyeing_kitchen = true;
	MI.has_reduced_load = true;

	Socket = new TSocketConn();

	ComServerThread = new ThreadTCP(true);

	ComServerThread->Priority = tpLower;
	ComServerThread->Resume();
}
//---------------------------------------------------------------------------

__fastcall TClientForm::~TClientForm()
{
	ComServerThread->Terminate();
	ComServerThread->WaitFor();

	delete ComServerThread;

	delete Socket;
}
//---------------------------------------------------------------------------

void __fastcall TClientForm::Button3Click(TObject *Sender)
{
	MI.machine_type = this->Edit2->Text.ToInt();
	MI.mac_id = this->Edit3->Text.c_str();
	MI.has_prep_tank = this->CheckBox1->Checked;
	MI.has_dyeing_kitchen = this->CheckBox2->Checked;
	MI.has_reduced_load = this->CheckBox3->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TClientForm::FormActivate(TObject *Sender)
{
	;
}
//---------------------------------------------------------------------------

//************************  TSocketConn ******************************
__fastcall TSocketConn::TSocketConn(){

	FClientSocket = new TClientSocket(NULL);
	FClientSocket->ClientType = ctBlocking;

	FClientSocket->Address = "127.0.0.1";
	FClientSocket->Port = 10000;

	IsConnected = false;
}
//------------------------------------------------------------------------------

__fastcall TSocketConn::~TSocketConn(){

	delete FClientSocket;
}
//------------------------------------------------------------------------------

void TSocketConn::TimerTCP(void){
	int l_msg_length;
	time_t l_time_now;
	static time_t s_memo_time = 0;

	if (FClientSocket->Active == true){
		time(&l_time_now);

		try {
			if (l_msg_length = SocketRead(reinterpret_cast<void*>(&net_pack.MsgIdx))){
				DoWhatRequired(net_pack.ThrdCmd, l_msg_length);

				l_msg_length = FormatTheAnswer(net_pack.ThrdCmd);

				SocketWrite((void*)&net_pack,l_msg_length);
			}
		} __finally {
			time(&s_memo_time);
		}

		if (abs((long)difftime(l_time_now,s_memo_time)) > 2){
			PurgeSocket();
			FClientSocket->Close();
		}
	} else { // Se non è connesso cerca di collegarsi
		try {
			FClientSocket->Open();
		} catch (...){
			FClientSocket->Active = false; // just to be sure
		}
	}

	IsConnected = FClientSocket->Active;
}
//------------------------------------------------------------------------------

int TSocketConn::SocketRead(void* _buffer){
	int j;

	try {
		if (!(j = FClientSocket->Socket->ReceiveLength()))
			return 0;

		FClientSocket->Socket->ReceiveBuf(_buffer,j);
	} catch (...){
		return 0;
	}

	return j;
}
//------------------------------------------------------------------------------

void TSocketConn::PurgeSocket(void){
	int j;

	try {
		if (!(j = FClientSocket->Socket->ReceiveLength()))
			return;

		char* aux = new char[j];
		FClientSocket->Socket->ReceiveBuf(aux, j);
		delete[] aux;
	} catch (...){
		return;
	}

	return;
}
//------------------------------------------------------------------------------

void TSocketConn::DoWhatRequired(const int& _cmd, const int& _bufflen){
	ushort i, j, k, m;
	safeptr<char> buff_ptr(net_pack.Data, _bufflen - 5*sizeof(short));

	switch (_cmd){
		case ticSendMsg:
			msg = (wchar_t*)buff_ptr.currptr();
			break;
	}
}
//------------------------------------------------------------------------------

int TSocketConn::FormatTheAnswer(int command){
	safeptr<char> buff_ptr(net_pack.Data, TCP_FRAME_MAXLENGHT-5*sizeof(short));
	UINT ws_size; // mod. 117

	memset(net_pack.Data, '\0', sizeof(net_pack.Data));

	switch(command){
		case ticSendMsg:
			// Caratterisiche macchina
			memcpy(buff_ptr.currptr(), &(MI.machine_type), sizeof(int));
			buff_ptr += sizeof(int);
			memcpy(buff_ptr.currptr(), &MI.has_prep_tank, sizeof(bool));
			buff_ptr += sizeof(bool);
			memcpy(buff_ptr.currptr(), &MI.has_dyeing_kitchen, sizeof(bool));
			buff_ptr += sizeof(bool);
			memcpy(buff_ptr.currptr(), &MI.has_reduced_load, sizeof(bool));
			buff_ptr += sizeof(bool);
			ws_size = MI.mac_id.length()+1;
			memcpy(buff_ptr.currptr(), &ws_size, sizeof(UINT));
			buff_ptr += sizeof(UINT);
			wcsncpy((wchar_t*)buff_ptr.currptr(), MI.mac_id.c_str(), ws_size); // mod. 117
			buff_ptr += ws_size*2;
			break;
	}

 	return sizeof(net_pack.MsgIdx) + sizeof(net_pack.ThrdCmd) +
		   sizeof(net_pack.ManCmd) +
		   sizeof(net_pack.ManCmd2) + // mod. 120
		   buff_ptr.distance();
}
//------------------------------------------------------------------------------
