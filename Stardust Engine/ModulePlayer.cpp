#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), car01(NULL), car02(NULL)
{
	car01Prop.turn = car01Prop.acceleration = car01Prop.brake = 0.0f;
	car02Prop.turn = car02Prop.acceleration = car02Prop.brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	//Car creation
	CreateCar(1);
	CreateCar(2);
		
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	UpdateCar01(dt);
	UpdateCar02(dt);

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		if (inpodium)
			ResetGame();
		else
			InGameReset();
	}

	//Title
    char title[80];
    sprintf_s(title, "%.1f Km/h, %.1f Km/h", car01->GetKmh(), car02->GetKmh());
    App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}


VehicleInfo ModulePlayer::DefineDefaultVehicleInfo()
{
	VehicleInfo car;

	// Car properties ----------------------------------------
	//Chassis
	car.chassis_size.Set(2, 2, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	//Shovel
	car.shovel_size.Set(car.chassis_size.x, car.chassis_size.y * 0.5f, car.chassis_size.z * 0.5f);
	car.shovel_offset.Set(car.chassis_offset.x, car.chassis_offset.y - 1, car.chassis_offset.z + 3);

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	//Wheel colour
	car.wheelColour = Blue;

	return car;
}

void ModulePlayer::CreateCar(uint carNum)
{
	VehicleInfo car = DefineDefaultVehicleInfo();

	switch (carNum) {
	case 1:
		car.chassisColour = car.shovelColour = colourCar01; //Colour for car 1
		car01 = App->physics->AddVehicle(car);
		car01->SetTransform(rot180.M);
		car01->SetPos(0, 0, 10);
		break;

	case 2:
		car.chassisColour = car.shovelColour = colourCar02;	//Colour for car 2
		car02 = App->physics->AddVehicle(car);
		car02->SetPos(0, 0, -10);
		break;

	default:
		break;
	}

}

void ModulePlayer::ResetCar(uint carNum)
{
	switch (carNum) {
	case 1:
		car01->SetTransform(IdentityMatrix.M);
		car01->SetTransform(rot180.M);
		car01->SetPos(0, 0, 10);
		car01->GetBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		car01->GetBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		break;

	case 2:
		car02->SetTransform(IdentityMatrix.M);
		car02->SetPos(0, 0, -10);
		car02->GetBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		car02->GetBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		break;

	default:
		break;
	}

}

void ModulePlayer::CarDeath(uint carNum)
{
	App->audio->PlayFx(App->audio->GetFX().deathSound, 0); //Death sound

	switch (carNum) {
	case 1:
		livesCar01--;
		if (App->player->livesCar01 == 0) {
			LOG("PLAYER 1 LOSES");
			SetEndPodium(1);
		}
		else if(App->player->livesCar01 > 0)
			ResetCar(1);
		break;
	case 2:
		livesCar02--;
		if (App->player->livesCar02 == 0) {
			LOG("PLAYER 2 LOSES");
			SetEndPodium(2);
		}
		else if(App->player->livesCar02 > 0)
			ResetCar(2);
		break;
	default:
		break;
	}

	//Change color of shovels to show which player is winning
	if (livesCar01 > livesCar02) 
	{
		car01->info.shovelColour = Green;
		car02->info.shovelColour = Red;
	}
	else if (livesCar01 < livesCar02) 
	{
		car01->info.shovelColour = Red;
		car02->info.shovelColour = Green;
	}
	else //Lives are equal
	{
		car01->info.shovelColour = colourCar01;
		car02->info.shovelColour = colourCar02;
	}



}

void ModulePlayer::UpdateCar01(float dt)
{
	car01Prop.turn = car01Prop.acceleration = car01Prop.brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && !inpodium)
	{
		car01Prop.acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !inpodium)
	{
		if (car01Prop.turn < TURN_DEGREES)
			car01Prop.turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !inpodium)
	{
		if (car01Prop.turn > -TURN_DEGREES)
			car01Prop.turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !inpodium)
	{
		car01Prop.acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && !inpodium) {
		car01Prop.brake = BRAKE_POWER;
	}

	car01->ApplyEngineForce(car01Prop.acceleration);
	car01->Turn(car01Prop.turn);
	car01->Brake(car01Prop.brake);

		//Boost

	boostControl1(dt);


	car01->Render();
}

void ModulePlayer::UpdateCar02(float dt)
{
	car02Prop.turn = car02Prop.acceleration = car02Prop.brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && !inpodium)
	{
		car02Prop.acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && !inpodium)
	{
		if (car02Prop.turn < TURN_DEGREES)
			car02Prop.turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && !inpodium)
	{
		if (car02Prop.turn > -TURN_DEGREES)
			car02Prop.turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && !inpodium)
	{
		car02Prop.acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT && !inpodium) {
		car02Prop.brake = BRAKE_POWER;
	}

	car02->ApplyEngineForce(car02Prop.acceleration);
	car02->Turn(car02Prop.turn);
	car02->Brake(car02Prop.brake);
		
	//Boost
	
	boostControl2(dt);
	car02->Render();
}

PhysVehicle3D * ModulePlayer::GetCar(uint carNum)
{
	switch (carNum) {
	case 1:
		return car01;
		break;
	case 2:
		return car02;
		break;
	default:
		return nullptr;
		break;
	}
	return nullptr;
}

bool ModulePlayer::IsGameFinished()
{
	return inpodium;
}

void ModulePlayer::SetEndPodium(int carLost)
{
	inpodium = true;

	//Stop the cars from moving
	car01->GetBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	car01->GetBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	car02->GetBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	car02->GetBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	car01->SetTransform(IdentityMatrix.M);
	car02->SetTransform(IdentityMatrix.M);
	//Set cars' positions
	switch (carLost) 
	{
	case 1:
		car01->SetPos(200.0f, 207.0f, 220.0f);
		car02->SetPos(200.0f, 215.0f, 200.0f);
		break;
	case 2:
		car01->SetPos(200.0f, 215.0f, 200.0f);
		car02->SetPos(200.0f, 207.0f, 220.0f);
		break;
	default:
		car01->SetPos(200.0f, 207.0f, 220.0f);
		car02->SetPos(200.0f, 215.0f, 200.0f);
		break;
	}

	//Camera
	App->camera->Move(vec3(190.0f, 200.0f, 209.0f));
	App->camera->LookAt(vec3(157.0f, 217.0f, 209.0f));
	//App->camera->GetViewMatrix();

	//Reset the capsule
	App->scene_intro->isCapsuleRendering = false;
	App->scene_intro->capsuleTimer = 0.0f;

	//Play the ending theme and GAME finished sound
	App->audio->PlayMusic("Assets/Audio/Music/ending_theme.ogg", 1.0F);
	App->audio->PlayFx(App->audio->GetFX().endGameSound, 0);

	//Boost stuff
	boostcont01 = 0;
	boostcont02 = 0;
	boostTimer1 = 0.0f;
	boostTimer2 = 0.0f;
	car01->info.wheelColour = Blue;
	car02->info.wheelColour = Blue;

}

void ModulePlayer::ResetGame()
{
	LOG("Reseting Game");
	//Cars
	ResetCar(1);
	ResetCar(2);
	livesCar01 = 3;
	livesCar02 = 3;
	//Return shovel colours to its original state
	car01->info.shovelColour = colourCar01;
	car02->info.shovelColour = colourCar02;

	//Camera
	App->camera->Position.x = 0;
	App->camera->Position.y = 0;
	App->camera->Position.z = 0;
	App->camera->Move(vec3(75.0f, 60.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	inpodium = false;

	//Play the level theme and restart FX
	App->audio->PlayFx(App->audio->GetFX().restartSound, 0);
	App->audio->PlayMusic("Assets/Audio/Music/level_theme.ogg", 1.0F);
}

void ModulePlayer::InGameReset()
{
	ResetGame();

	//Boost stuff
	boostcont01 = 0;
	boostcont02 = 0;
	boostTimer1 = 0.0f;
	boostTimer2 = 0.0f;
	car01->info.wheelColour = Blue;
	car02->info.wheelColour = Blue;

	//Reset the capsule
	App->scene_intro->isCapsuleRendering = false;
	App->scene_intro->capsuleTimer = 0.0f;
}

void ModulePlayer::boostControl1(float dt)
{

	if (boostcont01 == 0 && !inpodium)
		boostTimer1 += dt;

	if (boostTimer1 >= maxBoostTime1 && !inpodium) {
		boostcont01 = 3;
		boostTimer1 = 0.0f;
		car01->info.wheelColour = Green;

		//Sound Get Boost
		App->audio->PlayFx(App->audio->GetFX().getBoost);
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !inpodium && boostcont01 > 0) {
		App->audio->PlayFx(App->audio->GetFX().boostSound, 0);
		car01->ApplyEngineForce(100000000.0f);
		boostcont01--;

		if (boostcont01 == 0) {
			//Wheel colour change and sound
			car01->info.wheelColour = Blue;
			App->audio->PlayFx(App->audio->GetFX().runOutBoost);
		}

	}
	

}

void ModulePlayer::boostControl2(float dt)
{
	if (boostcont02 == 0 && !inpodium)
		boostTimer2 += dt;
	

	if (boostTimer2 >= maxBoostTime2 && !inpodium) {
		boostcont02 = 3;
		boostTimer2 = 0.0f;
		car02->info.wheelColour = Green;

		//Sound Get Boost
		App->audio->PlayFx(App->audio->GetFX().getBoost);
	}
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN && !inpodium && boostcont02 > 0) {
		App->audio->PlayFx(App->audio->GetFX().boostSound, 0);
		car02->ApplyEngineForce(100000000.0f);
		boostcont02--;

		if (boostcont02 == 0) {
			//Wheel colour change and sound
			car02->info.wheelColour = Blue;
			App->audio->PlayFx(App->audio->GetFX().runOutBoost);
		}
	}


}