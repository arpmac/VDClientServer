//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "Common.h"
#include "MasterMain.h"
#include "Thread_TCP_M.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TcpThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

extern TConnection *Con;

__fastcall ThreadTCP_M::ThreadTCP_M(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall ThreadTCP_M::Execute()
{
	//---- Place thread code here ----
	time_t act_time;

	while (!Terminated){
		time(&act_time);
		Con->LoopTcp(act_time);
		Sleep(500);
	}
}
//---------------------------------------------------------------------------
