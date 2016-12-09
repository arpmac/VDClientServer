//---------------------------------------------------------------------------

#ifndef Thread_TCP_MH
#define Thread_TCP_MH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class ThreadTCP_M : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall ThreadTCP_M(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
