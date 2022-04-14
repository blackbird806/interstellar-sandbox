#pragma once
#include <imgui/imgui.h>
#include <aini.hpp>
#include <units/units.h>


//https://fr.wikipedia.org/wiki/M%C3%A9trique_de_Schwarzschild#Coordonn%C3%A9es_de_Schwarzschild
struct SchwarzschildBlackHole
{
    Eigen::Vector2<double> pos;

    double M = 10.0f; // mass

    void showEditor()
    {
        ImGui::TextUnformatted("SchwarzschildBlackHole");

        ImGui::DragDouble("pos x", &pos[0]);
        ImGui::DragDouble("pos y", &pos[1]);

        ImGui::DragDouble("mass", &M, 10e12, 0, 0, "%.10e");

        ImGui::Text("Schwartzchild radius : %.10e m", SchwarzschildRadius());
    }

void serialize(aini::Writer& w)
    {
        w.set_float("pos_x", pos[0], "SchwarzschildBlackHole");
        w.set_float("pos_y", pos[1], "SchwarzschildBlackHole");
        w.set_float("mass", M, "SchwarzschildBlackHole");
    }

    void deserialize(aini::Reader& reader)
    {
        pos[0] = reader.get_float("pos_x", "SchwarzschildBlackHole");
        pos[1] = reader.get_float("pos_y", "SchwarzschildBlackHole");
        M = reader.get_float("mass", "SchwarzschildBlackHole");
    }

    double SchwarzschildRadius() const
    {
        return  (2.0 * units::constants::G.value() * M) / (units::constants::c.value() * units::constants::c.value());
    }
};
