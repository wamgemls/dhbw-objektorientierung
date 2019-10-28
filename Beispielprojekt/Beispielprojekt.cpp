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
	

public:

	double pos_x, pos_y, vel_x, vel_y, angle,vfaktortest;

	Player()
		: bild("car.png")
	{
		pos_x = pos_y = vel_x = vel_y = angle = vfaktortest = 0;
	}

	void warp(double x, double y) // Teleport an Postition x y
	{
		pos_x = x;
		pos_y = y;
	}

	void turn_left()
	{
		if (vfaktortest != 0) {

			angle -= 1.5;

		}
			
	}

	void turn_right()
	{
		if (vfaktortest != 0) {

			angle += 1.5;

		}
	}

	void accelerate() {
		
		vfaktortest = vfaktortest + 0.2;

		if (vfaktortest >= 8)
		{
			vfaktortest = 8;
		}

	}

	void reverse() {

		vfaktortest = vfaktortest - 0.4;

		if (vfaktortest <= -2)
		{
			vfaktortest = -2;
		}

	}

	void deceleration() {

		vfaktortest = vfaktortest * 0.97;

		if (abs(vfaktortest) < 0.3)
		{
			vfaktortest = 0;
		}

	}

	double offsetx()
	{
		return Gosu::offset_x(angle - 90, vfaktortest); // Geschwindigkeit X_Richtung

	}

	double offsety()
	{

		return Gosu::offset_y(angle - 90, vfaktortest); // Geschwindigkeit Y_Richtung
	}


	void move()
	{

		pos_x = Gosu::clamp(pos_x + offsetx(), 0.0, 1800.0);
		pos_y = Gosu::clamp(pos_y + offsety(), 0.0, 1000.0);
	}

	void draw() const
	{
		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.5, 0.5); // PNG-Center
	}

	
	void kollision() {

		vfaktortest = 0;

	}


};

class hitbox : public Player {

public:





};

class projektil {

	Gosu::Image bild;

public:

	double pos_x, pos_y, vel_x, vel_y, angle, vfaktortest;



	double offsetx()
	{
		return Gosu::offset_x(angle - 90, vfaktortest); // Geschwindigkeit X_Richtung

	}

	double offsety()
	{

		return Gosu::offset_y(angle - 90, vfaktortest); // Geschwindigkeit Y_Richtung
	}


};



class GameWindow : public Gosu::Window
{
	int kolrad = 80; //Kollisionsradius
	
	Player p1, p2;

public:
	Gosu::Image bild;
	GameWindow()
		: Window(1800, 1000), bild("map_1.png")
	{
		set_caption("Need for Gosu");
		p1.warp(400, 300);
		p2.warp(600, 700);
	}

	void update() override
	{
		// Player 1

		if (((Gosu::Input::down(Gosu::KB_LEFT)) || (Gosu::Input::down(Gosu::GP_LEFT)))) // Links (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p1.turn_left();
		}
		
		if (((Gosu::Input::down(Gosu::KB_RIGHT)) || (Gosu::Input::down(Gosu::GP_RIGHT)))) // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p1.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_UP)) || (Gosu::Input::down(Gosu::GP_BUTTON_0))) // Vorwärts (Pfeiltase) (A/X)
		{ 
			p1.accelerate();
		}

		if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

			p1.kollision();

		}

		if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_BUTTON_1))) // Rückwärts (Pfeiltase) (B/O)
		{ 
			p1.reverse();
		}

		if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN)) // Entschleunigung
		{
			p1.deceleration();
		}

		p1.move();

		//Player 2

		if (((Gosu::Input::down(Gosu::KB_A)) || (Gosu::Input::down(Gosu::GP_LEFT)))) // Links (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p2.turn_left();
		}

		if (((Gosu::Input::down(Gosu::KB_D)) || (Gosu::Input::down(Gosu::GP_RIGHT)))) // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p2.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_W)) || (Gosu::Input::down(Gosu::GP_BUTTON_0))) // Vorwärts (Pfeiltase) (A/X)
		{
			p2.accelerate();
		}

		if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

			p2.kollision();
			
		}

		if ((Gosu::Input::down(Gosu::KB_S)) || (Gosu::Input::down(Gosu::GP_BUTTON_1))) // Rückwärts (Pfeiltase) (B/O)
		{
			p2.reverse();
		}

		if (!Gosu::Input::down(Gosu::KB_W) && !Gosu::Input::down(Gosu::KB_S)) // Entschleunigung
		{
			p2.deceleration();
		}

		p2.move();	

	}


	void draw() override
	{
		p1.draw(); // Car
		p2.draw();
		bild.draw(0,0,0.0,1,1); // Racetrack
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}