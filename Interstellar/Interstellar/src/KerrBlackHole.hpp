#pragma once
#include <imgui/imgui.h>
#include <aini.hpp>

//https://fr.wikipedia.org/wiki/M%C3%A9trique_de_Schwarzschild#Coordonn%C3%A9es_de_Schwarzschild
struct SchwarzschildBlackHole
{
    int x = 0, y = 0;

    double M = 10.0f; // mass
    double J = 1.0f;  // angular momentum

    void showEditor()
    {
        ImGui::TextUnformatted("KerrBlackHole");

        ImGui::DragInt("pos x", &x);
        ImGui::DragInt("pos y", &y);

        ImGui::DragDouble("mass", &M);
        ImGui::DragDouble("angular momentum", &J);


        ImGui::Text("a : %f", kerrParameter());
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted("a is kerrParameter: a = mass/angular momentum");
            ImGui::EndTooltip();
        }

        ImGui::Text("spin : %f", getSpin());
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted("spin is the ");
            ImGui::EndTooltip();
        }
    }

    void serialize(aini::Writer& w)
    {
        w.set_int("pos_x", x, "SchwarzschildBlackHole");
        w.set_int("pos_y", y, "SchwarzschildBlackHole");

        w.set_float("mass", M, "SchwarzschildBlackHole");
        w.set_float("angular_m", J, "SchwarzschildBlackHole");
    }

    void deserialize(aini::Reader& reader)
    {
        x = reader.get_int("pos_x", "SchwarzschildBlackHole");
        y = reader.get_int("pos_y", "SchwarzschildBlackHole");

        M = reader.get_float("mass", "SchwarzschildBlackHole");
        J = reader.get_float("angular_m", "SchwarzschildBlackHole");
    }

    double kerrParameter() const
    {
        return J / M;
    }

    double getSpin() const
    {
        return kerrParameter() / M;
    }
};
