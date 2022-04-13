#pragma once

#include <units/units.h>

using namespace units::literals;

using gravity = units::compound_unit<
					units::cubed<units::length::meter>,
					units::inverse<units::mass::kg>,
					units::inverse<units::squared<units::time::second>>
					>;

using gravity_t = units::unit_t<gravity, double>;

constexpr units::mass::kilogram_t SM = 1.988'4e30_kg;
constexpr units::mass::kilogram_t operator""_SM(long double d)
{
	return units::mass::kilogram_t(d * SM);
}

double constexpr toRad = 0.0174533;