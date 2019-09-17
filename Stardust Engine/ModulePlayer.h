#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

struct VehicleProperties {
	float turn;
	float acceleration;
	float brake;
};


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//Vehicle info
	VehicleInfo DefineDefaultVehicleInfo();

	//Create car
	void CreateCar(uint carNum); //Uint is the number of the car
	void ResetCar(uint carNum);

	void CarDeath(uint carNum);

	//Update cars
	void UpdateCar01(float dt);
	void UpdateCar02(float dt);

	//Get car
	PhysVehicle3D* GetCar(uint carNum); 

	//Get is in podium
	bool IsGameFinished();

	//Set the ending game podium / end screen
	void SetEndPodium(int carLost);

	//Reset game
	void ResetGame();
	void InGameReset();

	//Boost

	void boostControl1(float dt);
	void boostControl2(float dt);

	int boostcont01 = 0;
	int boostcont02 = 0;


private:

	PhysVehicle3D* car01;
	PhysVehicle3D* car02;
	VehicleProperties car01Prop;
	VehicleProperties car02Prop;

	int livesCar01 = 3;
	int livesCar02 = 3;
	bool inpodium = false;
	
	float boostTimer1=0.0f;
	float maxBoostTime1= 20.0f;
	float boostTimer2=0.0f;
	float maxBoostTime2 = 20.0f;

	Color colourCar01 = Color(1.0f, 1.0f, 1.0f);
	Color colourCar02 = Color(0.2f, 0.2f, 0.2f);

	mat4x4 rot180 = mat4x4(
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

};