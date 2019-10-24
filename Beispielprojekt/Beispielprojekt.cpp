#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "Vektor2d.h"

// Simulationsgeschwindigkeit
const double DT = 100.0;

float vfaktor=0;

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
		angle -= 1;
	}

	void turn_right()
	{
		angle += 1;
	}

	double offsetx()
	{
		return Gosu::offset_x(angle-90, vfaktor); // Geschwindigkeit
		
	}

	double offsety()
	{
		 
		return Gosu::offset_y(angle-90, vfaktor); // Geschwindigkeit
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
		: Window(1800, 1000)
	{
		set_caption("Gosu Tutorial Game mit HanZ");				
		p1.warp(400, 300);
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
		if (Gosu::Input::down(Gosu::KB_UP)) {
			
			vfaktor = 3;
			
		}
		else {
			vfaktor = vfaktor * 0.97;
		}
		
		p1.move();
	}

	void draw() override
	{
		p1.draw();
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}