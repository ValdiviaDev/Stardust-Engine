#include "Application.h"
#include "NodeToggleActiveComp.h"
#include "NodeToggleActiveObject.h"
#include "GameObject.h"
#include "Component.h"
#include "ModuleGui.h"



NodeToggleActiveComp::NodeToggleActiveComp(int id, const ImVec2& pos) : Node(id, "Action: ToggleActiveComp", pos, 1, 1, Node_Type_Action, Func_ToggleActiveComp)
{
	active_set = Toggle;
	comp_type = Comp_Mesh;
}


NodeToggleActiveComp::~NodeToggleActiveComp()
{
}

bool NodeToggleActiveComp::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	//If reference gets deleted, reference is the original object
	if (obj_indx >= BB_objects.size()) {
		obj_using_this = true;
		obj_indx = 0;
	}

	if (dt == 0.0f) {
		updating = true;
		return updating; //Show that node is updating, but nothing happens, because the simulation is paused
	}

	if (BB_objects[obj_indx]) {
		error = false;
		Component* comp = BB_objects[obj_indx]->GetComponent(comp_type);
		
		if (comp) {
			switch (active_set) {
			case Toggle:
				comp->SetActive(!comp->IsActive());
				break;

			case True:
				comp->SetActive(true);
				break;

			case False:
				comp->SetActive(false);
				break;
			}

			updating = true;
		}
		else {
			error = true;
			App->gui->AddLogToConsole("ERROR: Can't toggle active component: Component is NULL");
		}
	}
	else {
		error = true;
		App->gui->AddLogToConsole("ERROR: Can't toggle active component: Object is NULL");
	}

	return updating;
}

void NodeToggleActiveComp::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	if (ImGui::Checkbox("This object", &obj_using_this)) {
		if (obj_using_this)
			obj_indx = 0;
	}

	if (!obj_using_this) {
		if (ImGui::BeginCombo("GameObject reference", BB_objects[obj_indx]->GetName())) {
			//If reference gets deleted, reference is the original object
			if (obj_indx >= BB_objects.size()) {
				obj_using_this = true;
				obj_indx = 0;
			}

			for (int i = 1; i < BB_objects.size(); ++i) {
				if (ImGui::Selectable(BB_objects[i]->GetName()))
					obj_indx = i;
			}

			ImGui::EndCombo();
		}
	}

	//Component to toggle
	static const char* comp_set_str[] = { "Mesh", "Material", "Script" };

	if (ImGui::BeginCombo("Set component", comp_str)) {
		if (ImGui::Selectable("Mesh")) {
			comp_str = comp_set_str[0];
			comp_type = Comp_Mesh;
		}

		if (ImGui::Selectable("Material")) {
			comp_str = comp_set_str[1];
			comp_type = Comp_Material;
		}
		
		if (ImGui::Selectable("Graph Script")) {
			comp_str = comp_set_str[2];
			comp_type = Comp_Graph_Script;
		}

		ImGui::EndCombo();
	}

	//Set active options
	static const char* active_set_str[] = { "Toggle", "True", "False" };

	if (ImGui::BeginCombo("Set active", curr_active_set_str)) {

		if (ImGui::Selectable("Toggle")) {
			curr_active_set_str = active_set_str[0];
			active_set = Toggle;
		}
		if (ImGui::Selectable("True")) {
			curr_active_set_str = active_set_str[1];
			active_set = True;
		}
		if (ImGui::Selectable("False")) {
			curr_active_set_str = active_set_str[2];
			active_set = False;
		}

		ImGui::EndCombo();
	}

}
