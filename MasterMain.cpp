//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <sstream>
#include "Common.h"
#include "Thread_TCP_M.h"
#include "MasterMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TServerForm *ServerForm;

ThreadTCP_M* TCPIP_Thread = NULL;
TConnection *Con = NULL;
MachineInfo MI;

#include <fstream>
void VDHLogger(AnsiString msg){ // mod. 035
	AnsiString aaa = DateTimeToStr(Now());
	std::string bbb = std::string(aaa.c_str());
	std::string ccc = std::string(msg.c_str());

	std::ofstream VDHlogfile;

	VDHlogfile.open("c:\\temp\\VDHLogger.txt", std::ofstream::app);

	VDHlogfile << bbb << " - " << ccc << std::endl;

	VDHlogfile.close();
}


void FormatMsgForLogger(std::stringstream &msg, eCnxStatus FCnxStatus){

	switch (FCnxStatus) {
		case csWaiting:
			msg << "Waiting";
			break;

		case csDisconnected:
			msg << "Disconnected";
			break;

		case csConnected:
			msg << "Connected";
			break;

		default:
			msg << "sconosciuto";
	}
}

//---------------------------------------------------------------------------
__fastcall TServerForm::TServerForm(TComponent* Owner)
	: TForm(Owner)
{
	MI.machine_type = 0;
	MI.mac_id = String("Not set").c_str();
	MI.has_prep_tank = false;
	MI.has_dyeing_kitchen = false;
	MI.has_reduced_load = false;

	Con = new TConnection();

	TCPIP_Thread = new ThreadTCP_M(true);

	TCPIP_Thread->Priority = tpLower;
	TCPIP_Thread->Resume();
}
//---------------------------------------------------------------------------

__fastcall TServerForm::~TServerForm()
{
	TCPIP_Thread->Terminate();
	TCPIP_Thread->WaitFor();

	delete TCPIP_Thread;

	delete Con;
}
//---------------------------------------------------------------------------

void __fastcall TServerForm::Timer1Timer(TObject *Sender)
{
	this->Edit2->Text = String(MI.machine_type);
	this->Edit3->Text = String(MI.mac_id.c_str());
	this->CheckBox1->Checked = MI.has_prep_tank;
	this->CheckBox2->Checked = MI.has_dyeing_kitchen;
	this->CheckBox3->Checked = MI.has_reduced_load;
}
//---------------------------------------------------------------------------

//****************************** TConnection ***********************************
_fastcall TConnection::TConnection(){

	FCnxStatus = csDisconnected;
	FReadPhase = false;
	act_msg_index = 1;

	// Crea il componente ServerSocket
	FServerSocket = new TServerSocket(NULL);
	FServerSocket->Port = 10000;
	FServerSocket->ThreadCacheSize = 1;
	FServerSocket->OnClientConnect = ServerSocketClientConnect;
	FServerSocket->OnClientDisconnect = ServerSocketClientDisconnect;
	FServerSocket->OnClientError = ServerSocketClientError;
	FServerSocket->OnAccept = ServerSocketClientAccept;
	FServerSocket->Active = true;
}
//----------------------------------------------------------------------------

__fastcall TConnection::~TConnection(){

	delete FServerSocket;
}
//----------------------------------------------------------------------------

//===============  Eventi generati dagli oggetti socket  =======================
void __fastcall TConnection::ServerSocketClientConnect(TObject *Sender,
	  TCustomWinSocket *Socket){

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- ServerSocketClientConnect ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	EmptySocketBuffer();
	FCnxStatus = csConnected;
	time(&FCnxTime);
	FReadPhase = false;
}
//---------------------------------------------------------------------------

void __fastcall TConnection::ServerSocketClientDisconnect(TObject *Sender,
	  TCustomWinSocket *Socket){

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- ServerSocketClientDisconnect ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	FCnxStatus = csWaiting;
}
//---------------------------------------------------------------------------

void __fastcall TConnection::ServerSocketClientError(TObject *Sender,
	  TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode){

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- ServerSocketClientError ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	ErrorCode = 0;
}
//---------------------------------------------------------------------------

void __fastcall TConnection::ServerSocketClientAccept(TObject *Sender, TCustomWinSocket *Socket){
/*
	std::stringstream ss;
	FormatMsgForLogger(ss, FCnxStatus);

	String aaa = String("--- ServerSocketClientAccept ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));
*/
//	VDHLogger(AnsiString("--- ServerSocketClientAccept ---"));
	;
}
//---------------------------------------------------------------------------

void TConnection::EmptySocketBuffer(void){
	int j;
	char aux[TCP_FRAME_MAXLENGHT];

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- EmptySocketBuffer ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	try {
		if (!(j = FServerSocket->Socket->Connections[0]->ReceiveLength()))
			return;

		FServerSocket->Socket->Connections[0]->ReceiveBuf((void *)aux,j);
	} catch (...){
		return;
	}

	return;
}
//---------------------------------------------------------------------------

// ema: FIXME
void TConnection::LoopTcp(const time_t& _act_time){ // mod. 124
	int k;

//	VDHLogger(AnsiString("--- LoopTcp ---"));

	switch (WhatShallIDo(_act_time)){
		case WRITE:
			cmd_id = ticSendMsg;
			man_cmd = 0;
			k = FormatTheMessage();
			SendTheMsg((void*)&net_pack, k);
/*
			if (!SendTheMsg((void*)&net_pack, k)){
				FCnxStatus = csDisconnected;
				FReadPhase = false;
				act_msg_index = 1;
			}
*/
			break;

		case READ:
			k = ReceiveTheMsg((void *)&(net_pack.MsgIdx));
			if (k){
				if (VerifyPackage(net_pack.MsgIdx, net_pack.ThrdCmd)){
					try {
						MessageDecode(cmd_id, k);
					} catch (...){
						// SE in fase di codifica del messaggio ho un errore
						// passa in scrittura ripetendo l'ultimo comando
						EmptySocketBuffer();
					}
				}
			} else // Lettura incoerente o nessuna lettura
				// Se in lettura da troppo tempo si predispone alla disconnessione
				VerifyConnIsAlive(_act_time);
			break;
	}
}
//----------------------------------------------------------------------------

// ema: FIXME
const int TConnection::WhatShallIDo(const time_t& _act_time){

	// Posso procedere al ciclo di lettura/scrittura solo se connesso e non sto aspettando client
	// ema FIXME il csDisconnected e il csWaiting
//	VDHLogger(AnsiString("--- WhatShallIDo ---"));
	std::stringstream ss;
	String aaa;

	if (FCnxStatus == csDisconnected)
		return DO_NOTHING;
	else if (FCnxStatus == csWaiting){

		FormatMsgForLogger(ss, FCnxStatus);
		aaa = String("--- WhatShallIDo ---") + String(ss.str().c_str());
		VDHLogger(AnsiString(aaa));

		if (abs((long)difftime(_act_time,FCnxTime)) >= 2){
			act_msg_index = 1;
			time(&FCnxTime);
			FCnxStatus = csDisconnected;
		}
		return DO_NOTHING;
	} else {
		if (FReadPhase)
			return READ;
		else
			return WRITE;
	}
}
//---------------------------------------------------------------------------

const int TConnection::FormatTheMessage(void){
	wchar_t* l_code;
	safeptr<char> buff_ptr(net_pack.Data, TCP_FRAME_MAXLENGHT-5*sizeof(short));

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- FormatTheMessage ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	l_code = String("Welcome client").c_str();

	// Incremento l'indice del messaggio
	if (++act_msg_index > 10000)
		act_msg_index = 100;

	// Preparo il messaggio
	net_pack.MsgIdx = act_msg_index;
	net_pack.ThrdCmd = cmd_id;
	net_pack.ManCmd = man_cmd;

	net_pack.ManCmd2[0] = 0;
	net_pack.ManCmd2[1] = 0;
	memset(net_pack.Data, '\0', sizeof(net_pack.Data));

	switch(cmd_id){
		case ticSendMsg:
			wcsncpy((wchar_t*)buff_ptr.currptr(), l_code, wcslen(l_code)+1);
			buff_ptr += (wcslen(l_code)+1)*2;
			break;
	}

	return sizeof(net_pack.MsgIdx) + sizeof(net_pack.ThrdCmd) +
		   sizeof(net_pack.ManCmd) +
		   sizeof(net_pack.ManCmd2) +
		   buff_ptr.distance();
}
//----------------------------------------------------------------------------

// ema: FIXME
bool TConnection::SendTheMsg(void* _buf, const int _size){

	int r_size = 0;
	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- SendTheMsg ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	if (!FServerSocket->Socket->Connected)
		return 0;

	try {
		r_size = FServerSocket->Socket->Connections[0]->SendBuf(_buf, _size);

		if (r_size != _size){
			aaa = String("--- SendTheMsg ---") + " ret_size = " + String(r_size) + " - size = " + String(_size);
			VDHLogger(AnsiString(aaa));

			return 0;
		}
	} catch(...){
		aaa = String("--- SendTheMsg in catch ---") + " ret_size = " + String(r_size) + " - size" + String(_size);
		VDHLogger(AnsiString(aaa));

		return 0;
	}

	time(&FCnxTime);
	FReadPhase = true;
	return 1;
}
//---------------------------------------------------------------------------

// ema: FIXME
int TConnection::ReceiveTheMsg(void* _buf){
	int l_size;

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- ReceiveTheMsg ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	if (!FServerSocket->Socket->Connected)
		return 0;

	try {
		l_size = FServerSocket->Socket->Connections[0]->ReceiveLength();

		if (!l_size)
			return 0;

		FServerSocket->Socket->Connections[0]->ReceiveBuf(_buf, l_size);
	} catch (...) {
		return 0;
	}

	return l_size;
}
//---------------------------------------------------------------------------

// ema: FIXME
bool TConnection::VerifyPackage(const int _msg_idx, const int _cmd){
	bool res = true;

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- VerifyPackage ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	if (_msg_idx != act_msg_index || _cmd != cmd_id)
		res = false;
	else {
		// passo in lettura
		FReadPhase = false;
		time(&FCnxTime);
	}

	return res;
}
//---------------------------------------------------------------------------

// ema: FIXME
void TConnection::MessageDecode(const int &_cmd, const int& _bufflen){
	int firstmac, lastmac;
	ushort i, j, k, m, n;
	safeptr<char> buff_ptr(net_pack.Data, _bufflen-5*sizeof(short)); // mod. 120
	char tmp[8] = {'A','\0','P','\0','V','\0','\0','\0'};
	UINT ws_size; // mod. 117

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- MessageDecode ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	switch(_cmd){
		case ticSendMsg:    // Invio di un messaggio custom
			MI.machine_type = *(reinterpret_cast<int*>(buff_ptr.currptr()));
			buff_ptr += sizeof(int);
			MI.has_prep_tank = *(reinterpret_cast<bool*>(buff_ptr.currptr()));
			buff_ptr += sizeof(bool);
			MI.has_dyeing_kitchen = *(reinterpret_cast<bool*>(buff_ptr.currptr()));
			buff_ptr += sizeof(bool);
			MI.has_reduced_load = *(reinterpret_cast<bool*>(buff_ptr.currptr()));
			buff_ptr += sizeof(bool);
			ws_size = *(reinterpret_cast<UINT*>(buff_ptr.currptr()));
			buff_ptr += sizeof(UINT);
			MI.mac_id = (wchar_t*)(buff_ptr.currptr());
			break;
	}

	// Comandi del client
/*
	switch(net_pack.ManCmd){
		case tccReadPrg:
			EnqueueNewCmd(ticReadPrg,0,NULL,NULL,NULL);
            break;
        case tccReqBatch:
            // va a vedere il primo prg in coda e lo spedisce
            FMachineData->NewPrgRequest();
            break;
	}
*/
}
//---------------------------------------------------------------------------

void TConnection::VerifyConnIsAlive(const time_t& _act_time){

	std::stringstream ss;

	FormatMsgForLogger(ss, FCnxStatus);
	String aaa = String("--- VerifyConnIsAlive ---") + String(ss.str().c_str());
	VDHLogger(AnsiString(aaa));

	if (abs((long)difftime(_act_time,FCnxTime)) > 10 && FReadPhase){ // mod. 101
		EmptySocketBuffer();
		time(&FCnxTime);
		FCnxStatus = csWaiting;
	}
}
//---------------------------------------------------------------------------

void __fastcall TServerForm::FormActivate(TObject *Sender)
{

	String aaa = String("--- VisualDyeServer Started ---");
	VDHLogger(AnsiString(aaa));

}
//---------------------------------------------------------------------------

void __fastcall TServerForm::FormClose(TObject *Sender, TCloseAction &Action)
{

	String aaa = String("--- VisualDyeServer Stopped ---");
	VDHLogger(AnsiString(aaa));

}
//---------------------------------------------------------------------------

