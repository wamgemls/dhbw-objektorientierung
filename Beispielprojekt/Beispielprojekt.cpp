#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "Vektor2d.h"

// Simulationsgeschwindigkeit
const double DT = 100.0;

float vfaktor = 0;

typedef std::vector<Gosu::Image> Animation; // Animation Def.

class Player
{
	Gosu::Image bild;
	double pos_x, pos_y, vel_x, vel_y, angle;

public:
	Player()
		: bild("car.png")
	{
		pos_x = pos_y = vel_x = vel_y = angle = 0;
	}

	void warp(double x, double y) // Teleport an Postition x y
	{
		pos_x = x;
		pos_y = y;
	}

	void turn_left()
	{
		angle -= 1.5;
	}

	void turn_right()
	{
		angle += 1.5;
	}

	double offsetx()
	{
		return Gosu::offset_x(angle - 90, vfaktor); // Geschwindigkeit X_Richtung

	}

	double offsety()
	{

		return Gosu::offset_y(angle - 90, vfaktor); // Geschwindigkeit Y_Richtung
	}


	void move()
	{

		pos_x = Gosu::clamp(pos_x + offsetx(), 0.0, 1800.0);
		pos_y = Gosu::clamp(pos_y + offsety(), 0.0, 1000.0);
	}

	void draw() const
	{
		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.7, 0.5); // PNG-Center
	}
};

class GameWindow : public Gosu::Window
{
	Player p1;

public:
	Gosu::Image bild;
	GameWindow()
		: Window(1800, 1000), bild("map_1.png")
	{
		set_caption("Need for Gosu");
		p1.warp(400, 300);
	}

	void update() override
	{
		if (Gosu::Input::down(Gosu::KB_LEFT) && vfaktor != 0) // Links
		{
			p1.turn_left();
		}
		
		if (Gosu::Input::down(Gosu::KB_RIGHT) && vfaktor != 0) // Rechts
		{
			p1.turn_right();
		}

		if (Gosu::Input::down(Gosu::KB_UP)) // Vorwärts
		{ 
			vfaktor = vfaktor + 0.2;

			if (vfaktor >= 8)
			{
				vfaktor = 8;
			}
		}

		if (Gosu::Input::down(Gosu::KB_DOWN)) // Rückwärts
		{ 
			vfaktor = vfaktor - 0.4;

			if (vfaktor <= -2)
			{
				vfaktor = -2;
			}
		}

		if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN)) // Entschleunigung
		{
			vfaktor = vfaktor * 0.97;

			if (abs(vfaktor) < 0.3)
			{
				vfaktor = 0;
			}
		}

		p1.move();
	}

	void draw() override
	{
		p1.draw(); // Car
		bild.draw(0,0,0.0,1,1); // Racetrack
	}
};

class Objekt
{
	Animation animation;
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}