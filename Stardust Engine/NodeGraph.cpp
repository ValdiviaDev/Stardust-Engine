
#include "NodeGraph.h"



NodeGraph::NodeGraph() {

}

NodeGraph::~NodeGraph() {
	
	for (uint i = 0; i < nodes.size(); i++) {

		RELEASE(nodes[i]);

	}
	nodes.clear();

}

void NodeGraph::Draw() {

	

	bool open_context_menu = false;
	static int node_selected = -1;
	
	static int node_hovered_in_scene = -1;

	//DELETE THIS AFTER ***************
	static bool inited = false;
	if (!inited)
	{
		
		AddNode("MainTex", ImVec2(40, 50), 1, 1, 0.5f, ImColor(255, 100, 100));
		AddNode("BumpMap", ImVec2(40, 150), 1, 1, 0.42f, ImColor(200, 100, 200));
		AddNode("Combine", ImVec2(270, 80), 3, 3, 1.0f, ImColor(0, 200, 100));
		
		AddLink(0, 0, 2, 0);
		AddLink(1, 0, 2, 1);
		inited = true;
	}//**********************************

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
		ImGui::InputText("##namenode", node->Name, IM_ARRAYSIZE(node->Name));
		ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
		ImGui::ColorEdit3("##color", &node->Color.x);
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

		ImU32 node_bg_color = (node_hovered_in_scene == node->ID || (node_selected == node->ID)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
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
					DeleteLink(node_idx, slot_idx);
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
					DeleteLink(node_idx, slot_idx);
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
			if (ImGui::MenuItem("Add")) { nodes.push_back(new Node(nodes.size(), "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2)); }
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
}

void NodeGraph::Update(float dt)
{
	for (int i = 0; i < nodes.size(); ++i) {
		nodes[i]->Update(dt);
	}
}


Node* NodeGraph::AddNode(const char* name, const ImVec2& pos, int inputs_count, int outputs_count, float value, const ImVec4& color) {

	last_node_id++;

	Node* n = new Node(last_node_id, name, pos, value, color, inputs_count, outputs_count);
	nodes.push_back(n);

	return n;
}

void NodeGraph::AddLink(int input_idx, int input_slot, int output_idx, int output_slot) {

	
	if (input_idx != output_idx) {

		//If on the same slot of another link, delete it
		for (int i = links.size() - 1; i >= 0; i--) {

			if (links[i].InputIdx == input_idx && links[i].InputSlot == input_slot)
				DeleteLink(input_idx, input_slot);
			else {
				if (links[i].OutputIdx == output_idx && links[i].OutputSlot == output_slot)
					DeleteLink(output_idx, output_slot);
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
		}

		NodeLink l(input_idx, input_slot, output_idx, output_slot);
		links.push_back(l);

	}
}


void NodeGraph::DeleteLink(int node_id, int slot_num) {


	for (uint i = 0; i < links.size(); i++) {


		if (links[i].InputIdx == node_id && links[i].InputSlot == slot_num || links[i].OutputIdx == node_id && links[i].OutputSlot == slot_num) {

			
			Node* aux = GetNodeByID(links[i].OutputIdx);

			//Look for link's Out. Then if it exists in the Out vector of the Input, erase it. Same the other way around later.
			if (aux) {
				std::vector<Node*>::iterator it = std::find(nodes[links[i].InputIdx]->outputs.begin(), nodes[links[i].InputIdx]->outputs.end(), aux);

				if (it != nodes[links[i].InputIdx]->outputs.end())
					nodes[links[i].InputIdx]->outputs.erase(it);
					
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

	//Delete node from nodes vec
	std::vector<Node*>::const_iterator it_n = std::find(nodes.begin(), nodes.end(), node);

	if(it_n != nodes.end()){
		nodes.erase(it_n);
		nodes.shrink_to_fit();
	}

	//reduce id if avobe nodeID and delete node from outputs and inputs lists
	for (it = nodes.begin(); it != nodes.end(); it++) {

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

