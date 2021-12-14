#pragma once
#include "stdafx.h"
#include <d3d9.h>
#include "GameFrontEnd.h"

// TODO: 
// 1) find weals rotaion, gas, break, nos, gear and e-brake
// 2) traffic cars
// 3) find end of race (finish, crash, restart ect)
// 4) explore race starting init
// Next V
// 5) Replays in menu
// 6) Input in menu...

// DONE:
// 1, 2, 3, 4, 5

/* need: 
	1)
	(Options _fastcall cFrontendDatabase::GetOptions(CurrentRaceMode) )
	CurrentRaceMode, DefaultSeed, SkipFETrackNumber (Options +  0x8), NumberOfOpponents (Options + 0x0 = number of racing cars), isBackwardRaceTrack(Options + 0xC)
	LapsNumberToSet (Options +  0x10), (Options +  0x14) OponentsSkill, (Options +  0x18) Traffic, (Options +  0x1F) Catch Up

	2)
	0x6F9E4C

*/

struct RigidBody;
struct Player;
struct mbRacerInfo;
struct Race;
struct CarTypeInfo;
struct RideInfo;
struct DriverInfo;
struct CarCollisionBody;
struct CarState;
struct PhysicsMover;
struct RealDriver;
struct Car;
struct RaceCoordinator;
struct World;

struct unkObj2;
struct unkObj3;
struct unkObj4;

enum RCState;
enum CarCameraTypes;

// size 0x30
struct SlotPool {
	SlotPool* next;					// +0x0
	SlotPool* prev;					// +0x4
	DWORD unk3;						// +0x8
	const char* name;				// +0xC
	void* curr_ptr;					// +0x10
	DWORD flags;					// +0x14
	size_t allocated_elements;		// +0x18
	size_t mbMaxElWhereAllocated;	// +0x1C
	DWORD unk4;						// +x020
	DWORD mbCapacity;				// +0x24 / =0x80, 0x40 / or step size
	size_t element_size;			// +0x28
	size_t mbCapacity2;				// +0x30 =mbInitialCapacity on init
};

struct Body {
	Body *next;
	Body *prev;
	char mbtypeName[8];
};

// inh of Body obj vtbl 0x6C8584 - pure

// allocated in block size - 0x19000
// 1) size 0x1F0, vtlb 0x6B9654, typeName "Rigid"
// 2) size 0x190, vtlb 0x6B9544, typeName "Wheel"
struct RigidBody /* old name Object3D */ {
	void *Vtable;							// + 0x0
	// 7: CalcSpeedmul or something
	
	RigidBody *next_plus4;					// + 0x4 / base object
	RigidBody *prev_plus4;					// + 0x8
	char mbtypeName[8];						// + 0xC
	BYTE unk0x14[0xC];						// + 0x14 / dseg*
	
	vector3a position;						// + 0x20
	float RotationMatrix4x4[4][4];			// + 0x30
	
	vector3a velocity;						// + 0x70
	vector3a unk0x80;						// + 0x80
	float unk0x90;							// + 0x90, mb time elapsed*??
	BYTE align0x94[0xC];
	vector3a velocity_multiplied;			// + 0xA0
	vector3a mbAceleration1;				// + 0xB0
	vector3a unk0xC0;						// + 0xC0 
	float matrix0xD0[3][4];					// + 0xD0	/ almost same as Rotation matrix
	vector3a unk0x100;						// + 0x100
	vector3a unk0x110;						// + 0x110
	BYTE unk0x120[0x10];					// + 0x120
	DWORD flag_mb0x130;						// + 0x130
	DWORD flag_mb0x134;						// + 0x134
	DWORD flag_mb0x138;						// + 0x138
	BYTE unk0x13C[0x8];						// + 0x13C

	float magic3;							// + 0x144 / number, than devide speed =1.22
	float magic3_inversed;					// + 0x148
	DWORD align0x14C;
	float unk0x150[0x10];					// + 0x150
	float unkMatrix0x190[4][4];				// + 0x190
	// +0x1E8 float
};

// size 0x18, no vtlb
struct eModel{
	void *unk1;
	void *unk2;
	DWORD mbID;
	void* unk3;
	void* unk4;
	DWORD unk5;								// +x014 / =0x34
};

//size 0xA70, from 0x4222FF
struct Player {
	BYTE unk0x0[0x4];		// +0x0 / =0
	Car* car;				// +0x4
	BYTE unk0x8[0x8];
	DWORD playernumber;		// +0x10
	BYTE unk0x14[0x20];		
	// +0x21 BYTE / =0
	char name[16];			// +0x34
	// +0xA8 - Average* / size 0x28
	// +0x108 pobj / size 0x30, from 0042BA25
	// +0x190 peModelSlopPool ??
	// +0x1AC float 5
	// +0x1B0 float 1
	// +0x1E0 same class pobj / size 0x90
	// +0x270 BYTE mb Nos flag idk
	// +0x27C - CameraMover
	// +0x28A - peModel maybe / size 0x140
	// +0x55C - pobj / size 0xA4
	// +0x5C0 - pobj / size 0x200
	// +0x914 int WorldTimer (start ?)
	// +0x95C - HUD ptr / size 0x90
};

// size 0x13C
struct mbRacerInfo {
	Car* car;
	// not a single pointer anymore
	// +0x8 BYTE
	// +0x10 WORD
	// +0x14 float unk0x1C 
	BYTE unk0x4[0x138];
};

// size 0x568
struct Race {
	DWORD unk0x0;				// +0x0 =4
	// +0xC float
	// +0x18 int
	
	// +0x30 RacerInfo array?? / Player Car*
	mbRacerInfo RacersInfo[4];	// +0x30, total size 0x4F0
	// +0x520 0x48 bytes left
	// +0x530 - int laps number
	// +0x53F Byte
	// +0x540 float unk0x540[3 or 4];
};

struct CameraSettings {
	CarCameraTypes type;
	float lag;
	float height;
	float lat_offset;
};

// size 0xC90
struct CarTypeInfo {
	char Name[32];
	char Name_for_bin_key[32];			// +0x20
	char geometry_bin_path[32];			// +0x40
	char geometry_lzc_path[32];			// +0x60
	BYTE unk0x80[64];					// +0x80
	char Manufacturer[16];				// +0xC0
	DWORD BinKey;
	// +0xB60 num of PlayerCameras
	// +0xB60 - CameraSettings PlayerCameras[5]
	// chek binary for additional information, GlobasB.lzc (not only), 0x19DDE0 - offset for RX7
};

struct RideInfo {
	DWORD CarType; // 0 - GOLF, 8 = RX-7 FD, 0x13 - Skyline R34 GTR?
	// +0x14 arr of objs size 0x30
	// 0x60 obj1 / mostly floats, 
	// obj1 + 0x2A0 float (1)
	// obj1 + 0x2A4 int (6)
	// +0x434 - BYTE 
	// +0x64B - obj?
};

// global addr 0x77C790 (for player I guess)
// size 0x750
struct DriverInfo {
	BYTE mbFlag;
	BYTE unk0x2[2];				// +0x1
	BYTE mbNumberOfDriver;		// +0x3
	int unk0x4;					// +0x4  mb control mode (1 for player, 2 for AI)
	BYTE unk0x8[8];				
	RideInfo rideinfo;			// +0x10		
	
	// +0x734 - BYTE mb control mode
};

// 0x80
struct Collision {
	void *punk0;				// +0x0 World +0x10
	void *punk1;				// +0x4 =punk0
	DWORD mb_timers[2];			// +0x8 =0x0000F348,
	DWORD unk0x10[4];			// +0x10 =0
	vector3a mbCollisionPoint;	// +0x20
	vector3a same0x30;			// +0x30
	DWORD unk0x40[4];			// +0x40 =0
	vector3a unkv3;				// +0x50
	float module_of_some_vector;// +0x60
	DWORD mbAlign[3];			// +0x64
	vector3a ImpulseNormal;		// +0x70
};

// size 0x40 in slotpool
struct Contact {
	void* punk0;				// +0x0 stack
	void* punk1;				// +0x4 =punk0
	float timeaftercontactmb;	// +0x8 =0.0113932
	CarCollisionBody* collbody1;// +0xC
	void* collbody2;			// +0x10 TrackPolygonCollisionBody for example
	void* CollisionBodyWitness;	// +0x14

};

// struct DynamicCollisionBody : CollisionBody {};

// TrackPolygonCollisionBody size - 0x270
// WorldObjectCollisionBody, size 0x410, vtbl 0x6B8D88
// size 0x190, all cars traffic including have this
struct CarCollisionBody /*: DynamicCollisionBody */{
	virtual void* __thiscall Destroy(bool free);							// 0:  +0x0
	virtual void __thiscall SaveInReplaySnapshot(void* ReplaySnapshot);		// 1:  +0x4
	virtual vector3* __thiscall GetPosition();								// 2:  +0x8
	virtual vector3* __thiscall GetVelocityMultiplied();					// 3:  +0xC
	virtual void __thiscall unkVF0x4();										// 4:  +0x10
	virtual float* __thiscall GetRotationMatrix4x4();						// 5:  +0x14
	virtual vector3a* __thiscall GetmbAceleration();						// 6:  +0x18
	virtual void __thiscall SetPosition(vector3* pos);						// 7:  +0x1C
	virtual float __thiscall GetSomeFloat();								// 8:  +0x20
	virtual float __thiscall GetmbTimeElapsed();							// 9:  +0x24
	virtual void __thiscall DoTimestep(float time);							// 10: +0x28

	// 29 entries
	//void *Vtable;					// 0x6C8798, size 0x190 

	CarCollisionBody* next;			// +0x4, ptr + 4 (no vtlb)
	CarCollisionBody* prev;			// +0x8
	void* unk0x8;					// +0xC data seg - 0x7377F0
	BYTE unk0x10[6];				
	bool bunk0x16;					// +0x16 mbIsMovable?
	bool unkflag0x17;				// +0x17 

	int unk0x18;					// +0x18 =7
	DWORD unk0x1C;					// +0x1C =0x100
	DWORD unk0x20;					// +0x20 =0
	float mbVector4[4];				// +0x24 seems like a position
	char Name[8];					// +0x34 "Car0", "Car51" etc.

	BYTE unk0x3C[0x24];				// +0x3C =0

	CarState* state;				// +0x60 / null mostly / or just state or something idk / or 1 / isHaveRigidBody
	DWORD unk0x64;
	void* OBJ_in_mover;				// +0x68 / that obj with vtlb 0x6C845C in WorldObject + 8
	
	RigidBody* body;				// +0x6C

	float mbTimeElaplsed;			// +0x70
	DWORD mbAligment0x74[3];		

	vector3a Normals[3];			// +0x80

	float unk0xB0[0x24];			

	Car* car;						// +0x140
	// +0x80 Vector3;
	// +0xE8 float
	// +0xC0 vector3
	// +0x110 vector3
	// +0120 Matrix4x4 / what???
	// +0x140 DWORD
};

struct CarState {
	void* RideInfoPlus0x60;		// initial value
	BYTE unk0x4[0xC];
	vector3a somePos;			// +0x10
	vector3a somePos2;			// +0x20 / RW? / for camera
	// +0x30 - max engine torque 
	vector3a unk0x30;
	float unk0x40[3];			// +0x40
	BYTE unk0x4C[4];			
	float Matrix4x4_0x50[4][4];	// +0x50 returns by CarCollisionBody::GetRotationMatrix4x4
	vector3a mbAceleration;		// +0x90
	float unk0xA0[4];				
	float Matrix4x4_0xB0[4][4];	// +0xB0

	BYTE unk0xF0[0x284];
	// +0x1B0 obj_size0x80[4];	// wut??
	// +0x2C0 float unk[??];
	WORD angle1_R;				// +0x374 = TrafficMover.angle * 10430.378
	WORD unk0x376;				// +0x376
	BYTE unk0x378[0x1C];
	// +0x38C DWORD mbMovementMode
	// +0x390 float mbPrevSpeed mps R

	float speed_mps_R;			// +0x394 meters per second, for HUD
	DWORD GearCode_R;			// +0x398 for HUD
	DWORD IsClutch_R;			// +0x39C for HUD
	float RPM_R;				// +0x3A0 for HUD
	BYTE unk0x3A4[0x10];		// +0x3A4
	DWORD Nos1;					// +0x3B4
	DWORD Nos2;					// +0x3B8
	void* NosTimer;				// +0x3BC

	BYTE unk0x3C0[0x18];		// +0x3C0
	DWORD mbControlMode;		// +0x3D8
	// +0x420 - BYTE
};

// old name WorldObject
struct Mover {
	// vtbl 5 entries
	virtual void* __thiscall Destroy(bool free);						// 0: +0x0
	virtual void __thiscall mbDoTimeStepMove(float TimeStep);			// 1: +0x4
	// ??, pos, rotation, speed
	virtual void __thiscall SaveInReplaySnapshot(void* ReplaySnapshot);	// 2: +0x8
	virtual RigidBody* __thiscall GetBody();							// 3: +0xC
	virtual DWORD __thiscall GetUnk();									// 4: +0x10
	// etc	
	// Mover_Vtbl* Vtable;		// 0x6CAADC - pure
	RealDriver* driver;		// +0x4
};

// size 0xC, vtbl 0x6CAAF0
struct NullMover : Mover {
	Car* car;//mb CarState?				// +0x8
};

// size 0x24 vtlb 0x6BA714, no RigidBody
struct TrafficMover : NullMover {
	float vector2_Pos[2];			// +0xC / RW
	float angle;					// +0x14 in radians mb
	float speed;					// +0x18  m/s
	DWORD unk0x1C[2];				// +0x1C / =0
};

/* Unknown obj size 0x2C, vtbl 0x6CAB08, from 0x42566C*/

// size 0x48, vtbl 0x6C89DC, no RigidBody
struct BeltMover : NullMover {
	DWORD unk0xC[2];				// +0xC =0
	void* unkobj0x14;				// +0x14, size 0x2C, no vtbl
	void* unkobj0x18;				// +0x18, size 0x30
	DWORD unk0x1C[3];				// +0x1C =0
	void* unkobj0x28;				// +0x28, size 0x60
	void* unkobj0x2C;				// +0x2C, size 0x60
	void* Average;					// +0x30, size 0x28
	BYTE unk0x34;					// +0x34 =0
	BYTE mbAlign0x35[3];			// +0x35 =BAADF0
	void* Wheels[4];				// +0x38 (for what?)
};

// double inhiritace i guess
// size 0x50 (in SlotPool), vtbl 0x6C8468
struct MellowMover : Mover {
	void* Vtlb;						// +0x8 some obj start here vtbl 0x6C845C (base 0x6C85A8) (3 entries) size 0x48
	char Name[8];					// +0xC
	void* Object3D_plus4;			// +0x14
	void* Object3D2_plus4;			// +0x18 Mover
	int unk;						// +0x1C
	RigidBody* body;				// +0x20
	void* Mellowator;				// +0x24
	void* Aerodynamics;				// +0x28
	void* dseg;						// +0x2C / 0x77CF50 floats
	CarState* state;				// +0x30
	Car* car;						// +0x34
	void* Wheels[4];				// +0x38
	DWORD unk0x48[2];				// +0x48 =0
};

/*	size 0x90 (vtlb 0x6C8364) */
struct PhysicsMover : Mover {				// old name: Worldobject
	void* mbVtable;					// +0x8 / 0x6C8358 (3 entries)
	char Name[8];					// +0xC / ="PhysMov"
	void* Object3D_plus4;			// +0x14
	void* Object3D2_plus4;			// +0x18 / DriveTrain + 4
	void* unk_dataseg0x1C;			// +0x1C / vtlb-0x6C8468, number 0x24, 32
	RigidBody* body;				// +0x20 / body or something / RigidBody
	void* TwoWheelAckermanSteering; // +0x24 size 0xA0
	void* Suspension[4];			// +0x28 size 0xC0
	void* Wheels[4];				// +0x38 size 0x190
	void* Engine;					// +0x48 size 0xC0
	void* DriveTrain;				// +0x4C size 0xE8
	void* Aerodynamics;				// +0x50 size 0x40
	void* dseg;						// +0x54
	CarState* carstate;				// +0x58
	Car* car;						// +0x5C
	// +0x78 float / =1.0
	// ect.
};

// size ??
struct CarDriver {
	void* vtbl;						// +0x0 33 entries
	/*
	0:  Destroy
	2:  SaveInReplaySnapshot
	9:  GetGearCode
	11: SetGearCode
	17: GetDriverNumber
	*/
};

/*	RealDriver -	size 0x2C0, vtbl 0x6C8AD0, from 0x59C011 - player
	PsychoDriver -	size 0x250, vtbl 0x6C8C18, from 0x46218B - AI Racer,
	CarDriver -		size ??, vtbl 0x6CAA58
	DragDriver -	size 0x520, vtbl 0x6C8B90 (DragDriver + 0x250 - RealDriver)
	AIDriver -		size 0x500, vtbl 0x6C89F0
	0x6B94F0 */
struct RealDriver {
	void* Vtable;				// 
	void* mbCarLoader;			// +0x4 size 0x140, from 0x42B4EE, Car* for AIDriver
	void* unk_obj0x8;			// +0x8 size mb 8, allocated in block (size 0x4E20A8)
	void* unk_obj0xC;			// +0xC addr same as prev + 8

	// size 0x1C0 some buffer or something
	int unknnum;				// +0x10 / =0xA mb driver number
	DWORD mbAlign[3];			// +0x1C / =0
	vector3a vecs[10];			// +0x20 / idk why so much, seem like pos, vector3a.align = 0
	DWORD unk0xC0[40];			// +0xC0 / =0
	DWORD unk0x160[28];			// +0x160 / random shit

	DWORD unk0x1D0;				// mb actual car steering
	DWORD GearCode_RW;			// +0x1D4
	BYTE unk0x1D8[0x18];	

	// some object starts here
	Car* car_p1;				// +0x1F0
	BYTE unk0x1F4[4];		
	BYTE AveragUnk0x1F8[0X34];	// +0x1F8 AverageWindow (size 0x28) Vtable, so there may be AverageWindow object
	// +0x208 void* unk_obj size 0x84, from 0x05841B9 (Average::???)
	float unk0x22C;
	float unk0x230;

	BYTE unk0x234[0x2C];

	WORD actual_steering;		// +0x260
	BYTE unk0x262[0xA];

	Car* car_p2;				// +0x26C

	BYTE unk0x270[0x18];

	BYTE SteeringRight_R;		// +288
	BYTE mbAlign0x289[3];			
	float Steering_RW;			// +0x28C 1 = Left, -1 = Right

	BYTE Gas_R;					// +0x290
	BYTE mbAlign0x291[3];
	float Gas_RW;// GAS GAS GAS	// +0x294 1 = full throttle

	BYTE Break_R;				// +0x298
	BYTE mbAlign0x299[3];
	float Break_RW;				// +0x29C

	BYTE unk0x2A0[8];			

	BYTE E_Brake_R;				// +0x2A8
	BYTE mbAlign0x2A9[3];
	float E_Break_RW;			// +0x2AC
	// 16 bytes left
};


// mb Hud
// size 0x210
struct unkObj4 {
	void* unk0x0;			// allocated in memblock size 0x19000
	void* unk0x4;			// allocated in same mem block
	char* str;				// HUD_SingleRace.fng

};

// size 1) 0x6C - base (for player), 2) AI Driver size 0x500 vtbl 0x6C89F0
struct unkObj2 {
	void* Vtable;			// 2) 0x6C89F0 ()
	Car* car;				// +0x4
	unkObj3* unkob3;		// +0x8 obj size 0x250 from 0x46218B mb Driver
	// +0x3F8 - ptr slot
	// +0x428 - obj (not a pointer)
	// +0x4D8 - DWORD
	
};

enum CarMovmentMode {
	Null =		0,
	Physics =	1,
	Traffic =	2,
	Mellow =	3,
	Belt =		4
};
// AI's car allocated from  	004224D6
// player's from 				00421F7D
// size 0x8F0
struct Car {
	void* not_sure;					// next Car or World or ultra base object idk 
	void* not_sure2;				// too  
	BYTE unk1[0x2];
	short driver_number;			// +0xA / (or car number)
	CarTypeInfo* cartypeinfo;		// +0xC
	RideInfo* rideinfo;				// +0x10
	DriverInfo* driverinfo;			// +0x14
	Player* player;					// +0x18
	mbRacerInfo* mbracerinfo;		// +0x1C
	void* unk0x20;					// +0x20  / initial value / dataseg
/*	1/1 - player,					RealDriver / Physics Mover
	2/1 - player					DragDriver / Physics Mover
	3/1 - AI racer					PsychoDriver / Physics Mover
	3/3 - AI racer					Mellow Mover
	3/4 - AI racer					BeltMover
	4/0 - traffic car				Null Mover
	4/2 - traffic car				PsychoDriver (wtf) / Traffic Mover
	4/3 - traffic car with invalid *mover / Mellow mover what the fuck?	*/
	int DriverType;					// +0x24 / =1 for player 
	CarMovmentMode MovmentMode;		// +0x28 / =1 for player, 3 for AI, 4 if AI out of view / mb CurrentMovementMode
	void* unkObj0x2C;				// +0x2C / size 0x6C from 0x426040 / (AI without world object) size 0x500 from 0x42608E / unkObj2
	RealDriver* Driver;				// +0x30
	PhysicsMover* mover;			// +0x34 / or CarWorldObject / sometimes 0, sometimes object size 0x48 / 0x10F89E00 insted for Traffic car
	DWORD unk0x38;					// +0x38 / =0,2; on collision = 1
	CarCollisionBody* collision;	// +0x3C - 0 or some value sometimes
	CarState state;					// +0x40 // size 0x421 as minimum
	// 
	// +0x404 float / =1.0
	// +0x478 float
	// +0x47C float
	// +0x4D0 BYTE
	// +0x4D1 BYTE
	// +0x540 obj / size may be 0x2C0, no vtbl
	// +0x7F0 TrackPolygon* in SlotPool, size 0x80
	// +0x800 TrackPathCoordinate
	// +0x818 float
	// +0x828 BYTE
	// +0x82C obj ptr, size 0xA0, from 0x57A1D0
	// +0x868 obj ptr (mb Loader) / size 0xF8, 
	// +0x86C byte (bool?)
	// +0x878 EAXAITunerCar*, size 0x1FE0, from 0x51D70F
	// +0x8A0 Vector2?
};

// size 0xB0
struct RaceCoordinator {
	RCState state;			
	void* RCQueuedMessage;	// +0x4
	void* unk_ptr0x8;		// +0x8, ptr to itself
	void* unk_ptr0xC;		// +0xC, same value as unk_ptr0x8
	// int num of ?
};

// size 0x17A0 from 0x421FA4
struct World {
	void *mbNext;				// +0x0 / ptr to itself or to Car
	void *mbPrev;				// +0x4 / ptr to itself or to Car

	void *optr1;				// +0x8 / next node in list mb / ptr to itself
	void *optr1_2;				// +0xC / prev node in list mb / same ptr as prev field

	void *optr2;				// +0x10 / ptr to itself
	void *optr2_2;				// +0x14 / same ptr as prev field

	int CarsNumber;				// + 0x18

	int mbPlayersNumber;		// +0x1C
	int mbRacingCarsNumber;		// +0x20
	int mbRacingCarsNumber2;	// +0x24

	Car* Cars[16];				// + 0x28
	// +0x280 +0x280 Player pos
	// +0x2C0 vector3 ?
	// +0x2E0
	// +0x2F0 some obj ptr / size 0x1A0

	// +0x300 obj?
	// +0x940 same

	// +0x1410 0bj2?
	// +0x15D0 same
	// ...
};

// size 
struct TrackInfo{
	WORD SkipFENumber;
	
};

enum CameraMoverTypes {
	// data from Player::ChangeCameraMover
	CamMovT_Drive = 1,	// size 0x3A0, vtbl 0x6C7FC8
	CamMovT_2 = 2,		// size  0x4C, vtbl 0x6C7D80
	CamMovT_Base = 3,	// size  0x2C, vtbl 0x6C7C50, like in CameraMover
	CamMovT_4 = 4,		// size  0xD0, vtbl 0x6C8018
	CamMovT_Bumper = 5,	// size 0x45C, vtbl 0x6C8018
	CamMovT_6 = 6,		// size  0x30, vtbl 0x6C7E10, mb CarCameraMover

	CamMovT_9 = 9,		// size 0x19C, vtbl 0x6C7EF0

	CamMovT_11 = 11,	// size 0x1A0, vtbl 0x6C7C98, Garage Camera mover

	CamMovT_13 = 13,	// size 0x368, vtbl 0x6C7B98, in prop 12
	CamMovT_14 = 14,	// size 0x450, vtbl 0x6C7B50, also 12

	CamMovT_15 = 15,	// size 0x470, vtbl 0x6C7D28, mb 7
	CamMovT_16 = 16,	// size 0x470, vtbl 0x6C7D28

	CamMovT_17 = 17,	// size  0x54, vtbl 0x6C7E58, mbRaceCameraMover
	CamMovT_18 = 18,	// size  0x2C, vtbl 0x6C7F80
	CamMovT_19 = 19,	// size  0x38, vtbl 0x6C7EA8
	CamMovT_20 = 20,	// size  0x58, vtbl 0x6C7DC8
	CamMovT_21 = 21,	// size  0xA0, vtbl 0x6C7CE0
	CamMovT_22 = 22,	// size 0x310, vtbl 0x6C7C08, mb before start
};

// size 0x2C, vtbl 0x6C7C50
struct CameraMover {
	virtual CameraMover* __thiscall Destroy(bool free);		// 0:  +0x0
	virtual void __thiscall Update();						// 1:  +0x4
	virtual void __thiscall UnkV2(DWORD);					// 2:  +0x8
	virtual void __thiscall UnkV3();						// 3:  +0xC
	virtual void __thiscall UnkV4();						// 4:  +0x10
	virtual void __thiscall Set_UnkV5(BYTE);				// 5:  +0x14
	virtual void __thiscall UnkV6(DWORD);					// 6:  +0x18

	virtual void __thiscall Set_UnkV7(bool);				// 7:  +0x1C
	virtual void __thiscall UnkV8(int);						// 8:  +0x20
	virtual BOOL __thiscall UnkV9();						// 9:  +0x24
	virtual BYTE __thiscall UnkV10();						// 10: +0x28
	virtual float __thiscall MinDistToWall();				// 11: +0x2C

	virtual DWORD __thiscall Get_UnkV12();					// 12: +0x30
	virtual void __thiscall UnkV13();						// 13: +0x34
	virtual void __thiscall Enable();						// 14: +0x38
	virtual void __thiscall Disable();						// 15: +0x3C
	virtual Car* __thiscall GetCarToFollow();				// 16: +0x40
	virtual void* __thiscall GetTarget();					// 17: +0x44

	// void* Vtbl					// +0x0 
	void* next_mb_baseobj;			// +0x4
	void* prev;						// +0x8
	CameraMoverTypes type;			// +0xC
	DWORD unk0x10;					// +0x10
	DWORD isEnabled;				// +0x14

	void* unk0x18;					// +0x18 in .data =0x7404C0,
	void* unk0x1C;					// +0x1C in .data =0x78D270,
	Player* player;					// +0x20
	DWORD unk0x24[2];				// +0x24 =0
};

struct PlayerCameraMover : CameraMover {
	Car* car_to_follow;				// +0x2C
};

enum CarCameraTypes {
	CarCameraType_FAR = 0,
	CarCameraType_CLOSE = 1,
	CarCameraType_BUMPER = 2,
	CarCameraType_DRIVER = 3,
	CarCameraType_DRIFT = 4
};

// size 0x54, 0x6C7E58
struct mbRaceCameraMover : PlayerCameraMover {
	void* unkobj0x30;				// +0x30 size 0x30, no Vtbl, from 0x48B077
	void* unkobjs0x34;				// +0x34 size 0x90, no Vtbl, from 0x48B0B8, array of 3
	void* unkobjs0x38;				// +0x38 size 0x90, no Vtbl, from 0x48B0E8
	void* unkobjs0x3C;				// +0x3C same
	CarCameraTypes camtype;			// +0x40
	BYTE unk0x44[4];				// +0x44 
	DWORD unk0x48;					// +0x48 =BAADF00D
	DWORD unk0x4C;					// +0x4C Timer?
	void* unkobj0x50;				// +0x50 size 0x30, Vtbl 0x6C7D7, from 0x48B141
};

// no vtlb, size unknown
struct CarPartsDB {
	CarPartsDB** ds;				// +0x0 / ptr to CarPartsDB's ptr in .data
	CarPartsDB** ds2;				// +0x4 / same
	int unk0x8;						// +0x8
	void* mbDB;						// +0xC / size of a part must be 0x30
	int mbPartsNumber;				// +0x10
};

struct TheGarageManager {
	void *obj1;						// +0x0 obj size 0xC from 0x50432A, "MU_QuickRaceCarSelect.fng"
	void *obj1_2;					// +0x4
	void *FECarConfig;				// +0x8
	// 
};


// enums

enum InputCodesBitsOffsets {
	Gas = 1,
	Break_Rev = 2,
	SteeringLeft = 3,
	SteeringRighgt = 4,
	E_brake = 5,
	NOS = 6,
	ShiftUp = 7,
	ShiftDown = 8,
	SeeBehind = 9,
	SwitchCamera = 10,
	Esc = 12
};

enum RCState {
	RCS_End = 0,
	RCS_Pre_race = 2,
	RCS_Pre_race2 = 3,
	RCS_Race = 4,
	RCS_Pause = 7,
	RCS_Finish = 8
};

enum mbRCAction {
	RCA_Nothing = 0,
	RCA_Continie = 1,
	RCA_Restart = 2,
	RCA_Leave = 3
};

enum RCMessage {
	RCM_mbNewState = 1,
	RCM_mbOldState = 2,
	RCM_idk = 3,
	RCM_InitialStart = 4,
	RCM_mbPause = 5,
	RCM_mbAction = 6,
	RCM_RestartPostRace = 8,
	RCM_LeaveRacePostRace = 10
};

enum GameFlowState {
	GAMEFLOW_STATE_NONE = 0,
	GAMEFLOW_STATE_LOADING_FRONTEND,
	GAMEFLOW_STATE_UNLOADING_FRONTEND,
	GAMEFLOW_STATE_IN_FRONTEND,
	GAMEFLOW_STATE_LOADING_TRACK_COMMON,
	GAMEFLOW_STATE_LOADING_TRACK_SPECIFIC,
	GAMEFLOW_STATE_RACING,
	GAMEFLOW_STATE_UNLOADING_TRACK_SPECIFIC,
	GAMEFLOW_STATE_UNLOADING_TRACK_COMMON,
	GAMEFLOW_STATE_EXIT_DEMO_DISC
};

const float AngleConvert = 10430.378;

// VAs

// .data

HWND* const hWnd = (HWND*)0x736380;
bool* const pIsLostFocus = (bool*)0x7363B6;
IDirect3D9** const ppD3D9 = (IDirect3D9**)0x736368;
IDirect3DDevice9** const ppD3D9Device = (IDirect3DDevice9**)0x73636C;
D3DPRESENT_PARAMETERS* const pPresentationParameters = (D3DPRESENT_PARAMETERS*)0x71A96C;

cFEng** const cFEng_pInstanse = (cFEng**)0x73578C;

void* const CollisionList = (void*)0x779C70;
DWORD* const pPlayersByIndex = (DWORD*)0x7361BC;
World** const pCurrentWorld = (World**)0x7361F8;
Race** const pCurrentRace = (Race**)0x77B240;	// 0, not recommend to use / or not???
DriverInfo** const DriversInfo = (DriverInfo**)0x78A418;
int* const NumberOfCars = (int*)0x78A41C;
int* const NumberOfOpponents = (int*)0x78A324;
int* const TrackNum = (int*)0x73616C; // total
int* const SkipFETrackNumber = (int*)0x78A2F0;
float* const WorldTimeElapsed = (float*)0x73457C;
void** const WorldTimer = (void**)0x73AD34;
void* const FEDatabase = (void*)0x748F70;
int* const CurrentRaceMode = (int*)0x777CC8;
DWORD* const Seed = (DWORD*)0x6F227C;
GameFlowState* const _GameFlowState = (GameFlowState*)0x77A920;
int* const UndergroundRaceDifficulty = (int*)0x75F240;
float* const Tweak_GameSpeed = (float*)0x6B7994;
BOOL* const CameraFollowDefaultCar = (BOOL*)0x6FBF14;
int* const CameraFollowThisCarInstead = (int*)0x6FBF18; // by number of driver
float* const RealTimeElapsed = (float*)0x73AD38;

// CurrentRaceMode, TrackInfo::TrackInfoTable, DefaultSeed


// .rdata
DWORD* const pVirtualPhysicsFunc = (DWORD*)0x6B9670;
const float* const fNFS_one = (float*)0x6CC7BC;

// .text

// after call opcode
void* const call_World_DoTimestep = (void*)0x44796A;
void* const call_RaceStarter_StartRace = (void*)0x4B80EE;
void* const call_InitFERaceEvent = (void*)0x4C5E38;
void* const call_InitFERaceEvent2 = (void*)0x4B383F;
void* const call_RCSendMessage_InRCChangeState = (void*)0x421AF5;
void* const call_FEngUpdate = (void*)0x4479A6;
void* const call_nh_malloc = (void*)0x671225;

void* const call_IDirect3DDevice9_EndScene = (void*)0x40A6CF;
void* const before_call_IDirect3DDevice9_Reset = (void*)0x40A52F;



// = = = = = = = = =
#define __usercall
#define __userpurge
typedef void(* void_void)();
const void_void const RaceStarter_StartRace = (void_void)0x4B5C00;
const void_void const FEngUpdate = (void_void)0x4F6170;

typedef RaceOptions*(_fastcall* T_cFrontendDatabase_GetOptions)(void* FEDatabase, int RaceType);
const T_cFrontendDatabase_GetOptions const cFrontendDatabase_GetOptions = (T_cFrontendDatabase_GetOptions)0x4AB510;

typedef void(__stdcall* T_World_DoTimestep)(World* world, float timeElapsed);
const T_World_DoTimestep const World_DoTimestep = (T_World_DoTimestep)0x420B40;

typedef void(__userpurge* T_RCSendMessage)(RaceCoordinator* ESI_this, int RCMessage, int);
const T_RCSendMessage const RaceCoordinator_RCSendMessage = (T_RCSendMessage)0x421BC0;

typedef char*(_fastcall* T_SearchForString)(DWORD, unsigned int code);
const T_SearchForString const SearchForString = (T_SearchForString)0x59FB80;

typedef int(__usercall* T_sPrintf)(char* EAX_FEString, const char* format, ...);
const T_sPrintf const FEngPrintf = (T_sPrintf)0x4F68A0;

typedef void(__usercall* T_Car_SetMovementMode)(Car* ESI_this, CarMovmentMode ECX_MovementMode);
const T_Car_SetMovementMode const Car_SetMovementMode = (T_Car_SetMovementMode)0x426110;

typedef void(__userpurge* T_Car_UpdateState)(WORD AX_angle, Car* _this, float* Matrix4x4, vector3*, void* unk_obj);
const T_Car_UpdateState const Car_UpdateState = (T_Car_UpdateState)0x4273D0;

__usercall void* const CarState_CalcNos = (void*)0x465530; // (esi - this)
__usercall void* const FECarierManager_InitFERaceEvent = (void*)0x5A0CE0; // (esi - this, edi - CarierRaceNumber)
__usercall void* const RaceStarter_StartUndergroundModeRaceR = (void*)0x4B5DF0; // (ebx - FERaceEvent*)
// = = = = = = = = =


// CRT
typedef void*(_cdecl *T_nh_malloc)(size_t size, int flags);
typedef char*(_cdecl *T_str1)(char* str1, const char* str2);
const T_str1 const NFS_strcat = (T_str1)0x67E1E0;
const T_str1 const NFS_strcpy = (T_str1)0x67E1D0;
typedef void(_cdecl *T_memcpy)(void* dst, const void* src, size_t size);
const T_memcpy const NFS_memcpy = (T_memcpy)0x672630;
typedef void*(_cdecl *T_malloc)(size_t size);
const T_malloc const NFS_malloc = (T_malloc)0x67121A;
typedef void(_cdecl *T_free)(void* ptr);
const T_free const NFS_free = (T_free)0x671102;
typedef void*(_cdecl *T_realloc)(void* old_ptr, size_t new_size);
const T_realloc const NFS_realloc = (T_realloc)0x675C84;

typedef int(_cdecl *T_vsprintf)(char* buf, const char* format, va_list);
const T_vsprintf const NFS_vsprintf = (T_vsprintf)0x671077;


// SkipFETrackNumber, isBackwardRaceTrack, NumberOfCars
// 0x45C3C7 - exception, after calling