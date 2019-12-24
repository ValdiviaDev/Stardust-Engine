#include "NodeGraph.h"
#include "GameObject.h"

#include "Node.h"
#include "NodeKeyInput.h"
#include "NodeMoveObject.h"
#include "NodeMouseInput.h"
#include "NodeMouseMove.h"
#include "NodeToggleActiveObject.h"
#include "NodeRotateObject.h"
#include "NodeToggleActiveComp.h"
#include "NodeLogToConsole.h"
#include "NodeInstantiateObject.h"
#include "NodeDeleteObject.h"
#include "NodeLoadScene.h"
#include "NodeTimer.h"

NodeGraph::NodeGraph() {

}

NodeGraph::~NodeGraph() {
	
	for (uint i = 0; i < nodes.size(); i++) {

		RELEASE(nodes[i]);

	}
	nodes.clear();
	fst_ev_nodes.clear();
}

void NodeGraph::Draw(std::vector<GameObject*> BB_objects, bool go_active) {

	bool open_context_menu = false;
	static int node_selected = -1;
	static bool open_win = true;

	static int node_hovered_in_scene = -1;

	//DELETE THIS AFTER ***************
	/*
	if (!inited)
	{
		AddNode(Func_KeyInput, ImVec2(40, 50));
		AddNode(Func_MoveObject, ImVec2(500, 50));

		AddLink(0, 0, 1, 0);
		inited = true;
	}*/

	ImGui::Begin("Graph", &open_win);

	ImGui::BeginGroup();
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 100);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	
	if (show_grid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 32.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
	}


	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background
	for (int link_idx = 0; link_idx < links.size(); link_idx++)
	{
		NodeLink* link = &links[link_idx];
		Node* node_inp = nodes[link->InputIdx];
		Node* node_out = nodes[link->OutputIdx];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}

	// Display nodes
	for (int node_idx = 0; node_idx < nodes.size(); node_idx++)
	{
		Node* node = nodes[node_idx];
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		
		ImGui::Text(node->Name);
		node->Draw(BB_objects);

		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
			node->Pos = node->Pos + ImGui::GetIO().MouseDelta;

		//Node color if selected and if updating
		uint bg_opatcity = 255;
		if (!go_active)
			bg_opatcity = 100;

		ImU32 node_bg_color = (node_hovered_in_scene == node->ID || (node_selected == node->ID)) ? IM_COL32(75, 75, 75, bg_opatcity) : IM_COL32(60, 60, 60, bg_opatcity);
		ImU32 node_outline_color = IM_COL32(100, 100, 100, 255);
		float node_outline_thickness = 4.0f;
		
		if (go_active) {
			switch (node->node_state)
			{
			case Node_State_Updating:
				node_outline_color = IM_COL32(0, 255, 0, 255); //Green
				node_outline_thickness = 8.0f;
				break;

			case Node_State_ToUpdate:
				node_outline_color = IM_COL32(255, 255, 0, 255); //Yellow
				node_outline_thickness = 8.0f;
				break;

			case Node_State_Error:
				node_outline_color = IM_COL32(255, 0, 0, 255); //Red
				node_outline_thickness = 8.0f;
				break;

			default:
				break;
			}
		}

		//Print node
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, node_outline_color, node_outline_thickness);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++) {
		
			if (node_idx == input_id_clicked && slot_idx == input_slot_clicked) 
				draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(200, 200, 200, 200));
			else 
				draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		

			//Check if input/output circles where pressed
			ImVec2 input_num = node->GetInputSlotPos(slot_idx) + offset;
			if (ImGui::IsMouseHoveringRect(ImVec2(input_num.x - NODE_SLOT_RADIUS, input_num.y - NODE_SLOT_RADIUS), ImVec2(input_num.x + NODE_SLOT_RADIUS, input_num.y + NODE_SLOT_RADIUS))) {
				if (ImGui::IsMouseClicked(0)) {
					
					LOG("Node input: %i, Input num: %i", node_idx, slot_idx);
					input_id_clicked = node_idx;
					input_slot_clicked = slot_idx;
				}

				if (ImGui::IsMouseClicked(1)) {
					DeleteLink(node_idx, slot_idx, true);
				}
			}
		}

		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++) {
		
			if (node_idx == output_id_clicked && slot_idx == output_slot_clicked) 
				draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(200, 200, 200, 200));
			else 
				draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
			

			ImVec2 input_num = node->GetOutputSlotPos(slot_idx) + offset;
			if (ImGui::IsMouseHoveringRect(ImVec2(input_num.x - NODE_SLOT_RADIUS, input_num.y - NODE_SLOT_RADIUS), ImVec2(input_num.x + NODE_SLOT_RADIUS, input_num.y + NODE_SLOT_RADIUS))) {
				if (ImGui::IsMouseClicked(0)) {

					LOG("Node output: %i, Output num: %i", node_idx, slot_idx);
					output_id_clicked = node_idx;
					output_slot_clicked = slot_idx;
				}

				if (ImGui::IsMouseClicked(1)) {
					DeleteLink(node_idx, slot_idx, false);
				}
			}
		}

		ImGui::PopID();
	}
	draw_list->ChannelsMerge();

	//Create link if input and output where clicked
	if (output_id_clicked != -1 && input_id_clicked != -1) {

		AddLink(output_id_clicked, output_slot_clicked, input_id_clicked, input_slot_clicked);

		input_id_clicked = -1;
		input_slot_clicked = -1;
		output_id_clicked = -1;
		output_slot_clicked = -1;

	}

	// Open context menu (Right Click options)
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
	{
		node_selected = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu (Right Click options)
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = node_selected != -1 ? nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->Name);
			ImGui::Separator();
			if (ImGui::MenuItem("Delete", NULL, false, true)) {
				
				DeleteNode(node);
				node_selected = -1;
			}

			if (ImGui::MenuItem("Copy", NULL, false, false)) {}

			ImGui::Separator();
			if (ImGui::MenuItem("Add input", NULL, false, true))
				node->SetInputsCount(node->InputsCount + 1);
			if (ImGui::MenuItem("Reduce input", NULL, false, true))
				node->SetInputsCount(node->InputsCount - 1);
			if (ImGui::MenuItem("Add output", NULL, false, true)) 
				node->SetOutputsCount(node->OutputsCount + 1);
			if (ImGui::MenuItem("Reduce output", NULL, false, true)) 
				node->SetOutputsCount(node->OutputsCount - 1);
		
		}
		else
		{
			if (ImGui::BeginMenu("Add")) {
				if (ImGui::BeginMenu("Event")) {
					if (ImGui::MenuItem("Key Input"))
						AddNode(Func_KeyInput, scene_pos);
					if (ImGui::MenuItem("Mouse Input"))
						AddNode(Func_MouseInput, scene_pos);
					if (ImGui::MenuItem("Mouse Move"))
						AddNode(Func_MouseMove, scene_pos);
					if (ImGui::MenuItem("Timer"))
						AddNode(Func_Timer, scene_pos);

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Action")) {
					if (ImGui::MenuItem("Move GameObject"))
						AddNode(Func_MoveObject, scene_pos);
					if (ImGui::MenuItem("Rotate GameObject"))
						AddNode(Func_RotateObject, scene_pos);
					if (ImGui::MenuItem("Active GameObject"))
						AddNode(Func_ToggleActiveObject, scene_pos);
					if(ImGui::MenuItem("Active Component"))
						AddNode(Func_ToggleActiveComp, scene_pos);
					if (ImGui::MenuItem("Instantiate GameObject"))
						AddNode(Func_InstObject, scene_pos);
					if (ImGui::MenuItem("Delete GameObject"))
						AddNode(Func_DelObject, scene_pos);
					if (ImGui::MenuItem("Load Scene"))
						AddNode(Func_LoadScene, scene_pos);
					if (ImGui::MenuItem("Log to Console"))
						AddNode(Func_LogToConsole, scene_pos);
					
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
			
			
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

void NodeGraph::Update(float dt, std::vector<GameObject*> BB_objects)
{
	for (int i = 0; i < fst_ev_nodes.size(); ++i) {
		//This nodes update always (first event nodes)
		fst_ev_nodes[i]->Update(dt, BB_objects);
			
		//Look for every output update
		for (int j = 0; j < fst_ev_nodes[i]->outputs.size(); ++j)
			UpdateOutputNodes(dt, BB_objects, fst_ev_nodes[i]->outputs[j], fst_ev_nodes[i]->node_state);
	}
}

void NodeGraph::UpdateOutputNodes(float dt, std::vector<GameObject*> BB_objects, Node* output, NodeState input_state)
{
	if (input_state == Node_State_Updating)
		output->Update(dt, BB_objects); //Update of the node
	else
		output->node_state = Node_State_Idle;

	for (int i = 0; i < output->outputs.size(); ++i)
		UpdateOutputNodes(dt, BB_objects, output->outputs[i], output->node_state);

}


Node* NodeGraph::AddNode(NodeFunction node_function, const ImVec2& pos) {

	last_node_id++;

	Node* node = nullptr;
	
	switch (node_function) {
	case Func_KeyInput:
		node = (Node*)new NodeKeyInput(last_node_id, pos);
		break;

	case Func_MoveObject:
		node = (Node*)new NodeMoveObject(last_node_id, pos);
		break;

	case Func_MouseInput:
		node = (Node*)new NodeMouseInput(last_node_id, pos);
		break;

	case Func_MouseMove:
		node = (Node*)new NodeMouseMove(last_node_id, pos);
		break;
		
	case Func_ToggleActiveObject:
		node = (Node*)new NodeToggleActiveObject(last_node_id, pos);
		break;

	case Func_RotateObject:
		node = (Node*)new NodeRotateObject(last_node_id, pos);
		break;

	case Func_ToggleActiveComp:
		node = (Node*)new NodeToggleActiveComp(last_node_id, pos);
		break;
		
	case Func_LogToConsole:
		node = (Node*)new NodeLogToConsole(last_node_id, pos);
		break;

	case Func_InstObject:
		node = (Node*)new NodeInstantiateObject(last_node_id, pos);
		break;

	case Func_DelObject:
		node = (Node*)new NodeDeleteObject(last_node_id, pos);
		break;

	case Func_LoadScene:
		node = (Node*)new NodeLoadScene(last_node_id, pos);
		break;

	case Func_Timer:
		node = (Node*)new NodeTimer(last_node_id, pos);
		break;

	case Func_Default:
		node = new Node(last_node_id, "Default", pos, 2, 2, Node_Type_Default, node_function);
		break;
	}

	if (node) {
		nodes.push_back(node);

		if (node->type == Node_Type_Event)
			fst_ev_nodes.push_back(node);
	}

	return node;
}

void NodeGraph::AddLink(int input_idx, int input_slot, int output_idx, int output_slot) {

	
	if (input_idx != output_idx) {

		//If on the same slot of another link, delete it
		for (int i = links.size() - 1; i >= 0; i--) {

			if (links[i].InputIdx == input_idx && links[i].InputSlot == input_slot)
				DeleteLink(input_idx, input_slot, true);
			else {
				if (links[i].OutputIdx == output_idx && links[i].OutputSlot == output_slot)
					DeleteLink(output_idx, output_slot, false);
			}
		}


		//Create link
		Node* in = nullptr;
		Node* out = nullptr;

		for (uint i = 0; i < nodes.size(); i++) {

			if (input_idx == nodes[i]->ID) {
				in = nodes[i];
			}

			if (output_idx == nodes[i]->ID) {
				out = nodes[i];
			}

		}

		if (in != nullptr && out != nullptr) {

			in->outputs.push_back(out);
			out->inputs.push_back(in);

			//Erase from the first event nodes if the output is an event
			if (out->type == Node_Type_Event && out->inputs.size() == 1) {
				std::vector<Node*>::const_iterator it_fst_ev_n = std::find(fst_ev_nodes.begin(), fst_ev_nodes.end(), out);

				if (it_fst_ev_n != fst_ev_nodes.end()) {
					fst_ev_nodes.erase(it_fst_ev_n);
					fst_ev_nodes.shrink_to_fit();
				}
			}
		}

		NodeLink l(input_idx, input_slot, output_idx, output_slot);
		links.push_back(l);

	}
}


void NodeGraph::DeleteLink(int node_id, int slot_num, bool input) {


	for (uint i = 0; i < links.size(); i++) {


		if ((input == false && links[i].InputIdx == node_id && links[i].InputSlot == slot_num) || (input == true && links[i].OutputIdx == node_id && links[i].OutputSlot == slot_num )) {

			
			Node* aux = GetNodeByID(links[i].OutputIdx);

			//Look for link's Out. Then if it exists in the Out vector of the Input, erase it. Same the other way around later.
			if (aux) {
				std::vector<Node*>::iterator it = std::find(nodes[links[i].InputIdx]->outputs.begin(), nodes[links[i].InputIdx]->outputs.end(), aux);

				if (it != nodes[links[i].InputIdx]->outputs.end())
					nodes[links[i].InputIdx]->outputs.erase(it);


				//Check if the out is an event node, if it is and no other node points to it, add it to first event nodes
				if (aux->type == Node_Type_Event && aux->outputs.size() == 0)
					fst_ev_nodes.push_back(aux);
			
			}

			aux = GetNodeByID(links[i].InputIdx);
			if (aux) {
				std::vector<Node*>::iterator it = std::find(nodes[links[i].OutputIdx]->inputs.begin(), nodes[links[i].OutputIdx]->inputs.end(), aux);

				if (it != nodes[links[i].OutputIdx]->inputs.end())
					nodes[links[i].OutputIdx]->inputs.erase(it);

			}
			
			std::vector<NodeLink>::const_iterator link_to_delete = links.begin() + i;

			links.erase(link_to_delete);
			links.shrink_to_fit();
			
			i = 0;

		}
	}
}


Node* NodeGraph::GetNodeByID(int ID) {

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {

		if ((*it)->ID == ID)
			return (*it);
	}

	return nullptr;
}

void NodeGraph::DeleteNode(Node* node) {

	std::vector<Node*>::iterator it = std::find(nodes.begin(), nodes.end(), node);
	std::vector<int> deleted_links; 
	
	//If we find node->ID delete that link later. If the link id is bigger than node->ID, then lower it 1 or we will have gaps
	for (uint i = 0; i < links.size(); i++){

		if (links[i].InputIdx == node->ID)
			deleted_links.push_back(i);

		if (links[i].OutputIdx == node->ID)
			deleted_links.push_back(i);

		if (links[i].InputIdx > node->ID)
			links[i].InputIdx--;

		if (links[i].OutputIdx > node->ID)
			links[i].OutputIdx--;

	}


	for (int i = deleted_links.size() - 1; i >= 0; i--) {

		std::vector<NodeLink>::const_iterator it_link = links.begin() + deleted_links[i];
		links.erase(it_link);
		links.shrink_to_fit();

	}

	int nodeID = node->ID;

	//Delete from first event nodes, if it is in there
	std::vector<Node*>::const_iterator it_fst_ev_n = std::find(fst_ev_nodes.begin(), fst_ev_nodes.end(), node);

	if (it_fst_ev_n != fst_ev_nodes.end()) {
		fst_ev_nodes.erase(it_fst_ev_n);
		fst_ev_nodes.shrink_to_fit();
	}


	//reduce id if above nodeID and delete node from outputs and inputs lists
	for (it = nodes.begin(); it != nodes.end(); it++) {

		if ((*it) != node) {
			bool vectors_clear = false;
			while (!vectors_clear) {

				vectors_clear = true;

				for (std::vector<Node*>::const_iterator it_in = (*it)->inputs.begin(); it_in != (*it)->inputs.end(); it_in++) {

					if ((*it_in)->ID == nodeID) {
						(*it)->inputs.erase(it_in);
						(*it)->inputs.shrink_to_fit();
						vectors_clear = false;
						break;
					}

				}

				for (std::vector<Node*>::const_iterator it_out = (*it)->outputs.begin(); it_out != (*it)->outputs.end(); it_out++) {

					if ((*it_out)->ID == nodeID) {
						(*it)->outputs.erase(it_out);
						(*it)->outputs.shrink_to_fit();
						vectors_clear = false;
						break;
					}
				}

			}

			if ((*it)->ID >= nodeID)
				(*it)->ID--;

		}
	}

	//Delete node from nodes vec
	std::vector<Node*>::const_iterator it_n = std::find(nodes.begin(), nodes.end(), node);

	if (it_n != nodes.end()) {
		nodes.erase(it_n);
		RELEASE(node);
		nodes.shrink_to_fit();
		last_node_id--;
	}

}



void NodeGraph::SaveFile(JSON_Array* arr) const {

	

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {

		JSON_Value* value = json_value_init_object();
		JSON_Object* obj = json_value_get_object(value);

		json_object_set_number(obj, "NodeID", (*it)->ID);
		json_object_set_string(obj, "Name", (*it)->Name);

		json_array_append_value(arr, value);

	}


}