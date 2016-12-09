//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "Common.h"
#include "LocalMain.h"
#include "Thread_TCP.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall ThreadTCP::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

extern TSocketConn *Socket;

__fastcall ThreadTCP::ThreadTCP(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------

void __fastcall ThreadTCP::Execute()
{
	//---- Place thread code here ----
	while (!Terminated){
		Socket->TimerTCP();
		if (Socket->IsConnected)
			Sleep(500);
		else
			Sleep(5000);
	}
}
//---------------------------------------------------------------------------
