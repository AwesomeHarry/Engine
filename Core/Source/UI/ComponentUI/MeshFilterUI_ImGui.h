#pragma once
#include <imgui.h>
#include "Scene/Components/Native/MeshFilterComponent.h"
#include "UI/UIUtil.h"

namespace Engine {
	class MeshFilterUI_ImGui {
	public:
		static void RenderUI(MeshFilterComponent& meshFilter) {
			if (ImGui::TreeNodeEx("Mesh Filter", ImGuiTreeNodeFlags_Framed)) {
				auto& mesh = *meshFilter.mesh;

				ImGui::Text("Mesh Name: %s", mesh.GetName().c_str());
				if (ImGui::TreeNode("Mesh Info")) {
					for (size_t submeshIndex = 0; submeshIndex < mesh.GetSubmeshCount(); submeshIndex++) {
						auto& va = mesh.GetSubmesh(submeshIndex);

						if (ImGui::TreeNode((void*)submeshIndex, "Submesh %d", submeshIndex)) {
							if (ImGui::TreeNode("Vertex Buffers")) {
								for (size_t vaIndex = 0; vaIndex < va.GetVertexBufferCount(); vaIndex++) {
									auto& vb = va.GetVertexBuffer(vaIndex);

									if (ImGui::TreeNode((void*)vaIndex, "Buffer %d", vaIndex)) {
										if (ImGui::BeginTable("LayoutTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
											ImGui::TableSetupColumn("Name");
											ImGui::TableSetupColumn("Type");
											ImGui::TableSetupColumn("Count");
											ImGui::TableHeadersRow();

											auto& layout = vb.GetLayout();
											auto& components = layout.GetComponents();
											for (auto& component : components) {
												ImGui::TableNextRow();
												ImGui::TableSetColumnIndex(0);
												ImGui::Text(component.Name.c_str());
												ImGui::TableSetColumnIndex(1);
												ImGui::Text(UIUtil::LTypeToString(component.Type).c_str());
												ImGui::TableSetColumnIndex(2);
												ImGui::Text("%d", component.Count);
											}

											ImGui::EndTable();
										}
										ImGui::Text("Count: %d", vb.GetCount());
										ImGui::TreePop();
									}
								}
								ImGui::TreePop();
							}

							if (va.HasIndices()) {
								auto& ib = va.GetIndexBuffer();

								if (ImGui::TreeNode("Index Buffer")) {
									ImGui::Text("Type: %s", UIUtil::LTypeToString(ib.GetType()).c_str());
									ImGui::Text("Count: %d", ib.GetCount());
									ImGui::TreePop();
								}
							}

							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
	};
}