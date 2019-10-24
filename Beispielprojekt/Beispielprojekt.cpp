#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "Vektor2d.h"

// Simulationsgeschwindigkeit
const double DT = 100.0;

class Player
{
	Gosu::Image bild;
	double pos_x, pos_y, vel_x, vel_y, angle;

public:
	Player()
		: bild("rakete.png")
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
		angle -= 3;
	}

	void turn_right()
	{
		angle += 3;
	}

	void accelerate()
	{
		vel_x += Gosu::offset_x(angle, 0.5);
		vel_y += Gosu::offset_y(angle, 0.5);
	}

	void move()
	{
		pos_x = Gosu::wrap(pos_x + vel_x, 0.0, 800.0);
		pos_y = Gosu::wrap(pos_y + vel_y, 0.0, 600.0);

		vel_x *= 0.9;
		vel_y *= 0.99;
	}

	void draw() const
	{
		bild.draw_rot(pos_x, pos_y, 1, angle);
	}
};

class GameWindow : public Gosu::Window
{
	Player p1, p2;

public:
	Gosu::Image bild;
	GameWindow()
		: Window(800, 600)
	{
		set_caption("Gosu Tutorial Game mit HanZ");
		p1.warp(400, 300);
		p2.warp(100, 200);
	}

	void update() override
	{
		if (Gosu::Input::down(Gosu::KB_LEFT)) 
		{
			p1.turn_left();
		}
		if (Gosu::Input::down(Gosu::KB_RIGHT)) 
		{
			p1.turn_right();
		}
		if (Gosu::Input::down(Gosu::KB_UP)) 
		{
			p1.accelerate();
		}
		p1.move();

		if (Gosu::Input::down(Gosu::KB_A))
		{
			p2.turn_left();
		}
		if (Gosu::Input::down(Gosu::KB_D))
		{
			p2.turn_right();
		}
		if (Gosu::Input::down(Gosu::KB_W))
		{
			p2.accelerate();
		}
		p2.move();
	}

	void draw() override
	{
		p1.draw();
		p2.draw();
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}
