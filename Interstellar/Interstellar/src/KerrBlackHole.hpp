#pragma once
#include <imgui/imgui.h>
#include <aini.hpp>
#include <units/units.h>


//https://fr.wikipedia.org/wiki/M%C3%A9trique_de_Schwarzschild#Coordonn%C3%A9es_de_Schwarzschild
struct SchwarzschildBlackHole
{
    int x = 0, y = 0;

    double M = 10.0f; // mass

    void showEditor()
    {
        ImGui::TextUnformatted("SchwarzschildBlackHole");

        ImGui::DragInt("pos x", &x);
        ImGui::DragInt("pos y", &y);

        ImGui::DragDouble("mass", &M, 10e12, 0, 0, "%.10e");

        ImGui::Text("Schwartzchild radius : %.10e m", SchwarzschildRadius());
    }

    void serialize(aini::Writer& w)
    {
        w.set_int("pos_x", x, "SchwarzschildBlackHole");
        w.set_int("pos_y", y, "SchwarzschildBlackHole");
        w.set_float("mass", M, "SchwarzschildBlackHole");
    }

    void deserialize(aini::Reader& reader)
    {
        x = reader.get_int("pos_x", "SchwarzschildBlackHole");
        y = reader.get_int("pos_y", "SchwarzschildBlackHole");
        M = reader.get_float("mass", "SchwarzschildBlackHole");
    }

    double SchwarzschildRadius() const
    {
        return  (2.0 * units::constants::G.value() * M) / (units::constants::c.value() * units::constants::c.value());
    }
};
