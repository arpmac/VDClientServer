//---------------------------------------------------------------------------

#ifndef CommonH
#define CommonH

#include <string>

#define TCP_FRAME_MAXLENGHT 8192

typedef unsigned short ushort;

typedef struct _sTcpPacket {
	ushort MsgIdx;
 	ushort ThrdCmd;
	ushort ManCmd;
	ushort ManCmd2[2];

	char Data[TCP_FRAME_MAXLENGHT-5*sizeof(short)];
} sTcpPacket;

template<typename T> class safeptr {
    T *curr_adr, *start_adr, *end_adr;
    int dim;

public:
    safeptr(T* _startadr, int _dim):start_adr(_startadr), dim(_dim){
        curr_adr = _startadr;
        end_adr = _startadr+dim;
    }

    T* operator+=(const int& rv){
        if (end_adr < curr_adr + rv)
            throw std::runtime_error("Pointer out of range");

        return curr_adr += rv;
    }

    T* currptr(){return curr_adr;}
    void begin(){curr_adr = start_adr;}
    int distance(){return curr_adr - start_adr;}
};

enum eTcpipCmd {
    ticHandshake	/* handshake inizio connessione */,
	ticReadCh		/* Lettura canali */,
    ticReadPrg		/* Lettura programma */,
    ticCycleAdv		/* Avanzamento ciclo */,
    ticStepAdv		/* Avanzamento passo */,
    ticStepTimeAdv	/* Avanzamento tempo passo*/,
    ticChangePar	/* Cambio parametri passo (in corso) */,
    ticDelStep		/* Cancella passo  */,
    ticAddStep		/* Aggiungi passo */,
    ticDosing		/* Dosaggio */,
    ticNewPrg		/* Invio nuovo prg */,
    ticNewCycle		/* Invio nuovo ciclo */,
    ticSendCmts	    /* Invio commenti */,
    ticSendMsg      /* Invio messaggio custom*/
};


typedef struct _machineInfo {
	int machine_type;
	bool has_prep_tank;
	bool has_dyeing_kitchen;
	bool has_reduced_load;
	std::wstring mac_id;
} MachineInfo;

enum eCnxStatus {csDisconnected, csConnected, csWaiting};

#define DO_NOTHING  0
#define READ        1
#define WRITE       2

#endif
