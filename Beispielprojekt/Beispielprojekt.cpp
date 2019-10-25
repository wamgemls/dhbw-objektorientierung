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

	void warp(double x, double y)
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
		return Gosu::offset_x(angle - 90, vfaktor); // Geschwindigkeit

	}

	double offsety()
	{

		return Gosu::offset_y(angle - 90, vfaktor); // Geschwindigkeit
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
		set_caption("Gosu Tutorial Game mit HanZ");
		p1.warp(400, 300);
	}

	void update() override
	{
		if (Gosu::Input::down(Gosu::KB_LEFT) && vfaktor != 0)
		{
			p1.turn_left();

		}
		if (Gosu::Input::down(Gosu::KB_RIGHT) && vfaktor != 0)
		{

			p1.turn_right();

		}
		if (Gosu::Input::down(Gosu::KB_UP)) {

			vfaktor = vfaktor + 0.3;

			if (vfaktor >= 5)
			{
				vfaktor = 5;
			}

		}
		else 
		{
			vfaktor = vfaktor * 0.97;
			if (vfaktor < 0.3 && vfaktor > -0.01) 
			{
				vfaktor = 0;
			}
		}
		
		if (Gosu::Input::down(Gosu::KB_DOWN)) {

			vfaktor = vfaktor - 0.3;

			if (vfaktor <= -2)
			{
				vfaktor = -2;
			}

		}
		else
		{
			vfaktor = vfaktor * 0.97;
			if (vfaktor > -0.3 && vfaktor < 0.01)
			{
				vfaktor = 0;
			}
		}

		p1.move();
	}

	void draw() override
	{
		p1.draw();
		bild.draw(0,0,0.0,1,1);
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}