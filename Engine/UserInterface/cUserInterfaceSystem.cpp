#include "cUserInterfaceSystem.h"

cUserInterfaceSystem::cUserInterfaceSystem(GLFWwindow* i_window, const char* i_openglVersion)
{
	m_window = i_window;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(i_window, true);
	ImGui_ImplOpenGL3_Init(i_openglVersion);
}

void cUserInterfaceSystem::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void cUserInterfaceSystem::DrawUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	for(sPanel* i_panel : m_panels)
	{
		if (!i_panel->b_active) continue;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, i_panel->m_color);
		ImGui::SetNextWindowPos(i_panel->m_position);
		ImGui::SetNextWindowSize(i_panel->m_size);
		ImGui::Begin(i_panel->m_name, nullptr,
			ImGuiWindowFlags_NoTitleBar |        // Hides the title bar
			ImGuiWindowFlags_NoResize |          // Prevents resizing
			ImGuiWindowFlags_NoMove |            // Prevents dragging
			ImGuiWindowFlags_NoCollapse |        // Hides the collapse button
			ImGuiWindowFlags_NoScrollbar         // Hides scrollbars
		);
		for (sUIComponent* i_comp : i_panel->m_components) {
			if (i_comp->b_active) {
				i_comp->Render();
			}
		}

		ImGui::End();
		ImGui::PopStyleColor();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void sImageButton::Render()
{
	ImGui::SetCursorPos(m_position);
	std::string button_id = "##button_" + std::to_string(reinterpret_cast<uintptr_t>(this));
	if (ImGui::ImageButton(button_id.c_str(), m_texture, m_size)) {
		if (m_callback) {
			m_callback(); 
		}
	}
}

void sButton::Render()
{
	ImGui::SetCursorPos(ImVec2(m_position));

	if (ImGui::Button(m_text.c_str(), m_size)) {
		if (m_callback) {
			m_callback();
		}
	}
}

void sRgbPicker::Render()
{
	ImGui::SetCursorPos(ImVec2(m_position));
	ImGui::PushItemWidth(m_size.x);
	ImGui::ColorEdit3("Color", m_color, ImGuiColorEditFlags_NoInputs);
}
