//---------------------------------------------------------------------------

#ifndef Thread_TCPH
#define Thread_TCPH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class ThreadTCP : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall ThreadTCP(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
