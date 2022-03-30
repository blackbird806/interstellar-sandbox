#pragma once
#include <imgui/imgui.h>
#include <aini.hpp>

struct KerrBlackHole
{
    int x = 0, y = 0;

    double M = 10.0f;       // mass
    double J = 1.0f;        // angular momentum
    double r = 0.0f;        // radial coordinate
    double theta = 0.0f;    // colatitude
    double phi = 0.0f;      // longitude

    void showEditor()
    {
        ImGui::TextUnformatted("KerrBlackHole");

        ImGui::DragInt("pos x", &x);
        ImGui::DragInt("pos y", &y);

        ImGui::DragDouble("mass", &M);
        ImGui::DragDouble("angular momentum", &J);
        ImGui::DragDouble("radial coordinate", &r);


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
        w.set_int("pos_x", x, "KerrBlackHole");
        w.set_int("pos_y", y, "KerrBlackHole");

        w.set_float("mass", M, "KerrBlackHole");
        w.set_float("angular_m", J, "KerrBlackHole");
        w.set_float("radial", r, "KerrBlackHole");
    }

    void deserialize(aini::Reader& reader)
    {
        x = reader.get_int("pos_x", "KerrBlackHole");
        y = reader.get_int("pos_y", "KerrBlackHole");

        M = reader.get_float("mass", "KerrBlackHole");
        J = reader.get_float("angular_m", "KerrBlackHole");
        r = reader.get_float("radial", "KerrBlackHole");
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
