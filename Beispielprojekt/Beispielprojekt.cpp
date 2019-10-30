#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <list>

#include "Vektor2d.h"

// Simulationsgeschwindigkeit
const double DT = 100.0;

//Animation 


typedef std::vector<Gosu::Image> Animation;

class Star
{
	Animation animation;
	Gosu::Color color;
	double pos_x, pos_y;

public:
	Star(Animation animation)
		: animation(animation)
	{
		//color.set_alpha(255);
		/*double red = Gosu::random(40, 255);
		color.set_red(static_cast<Gosu::Color::Channel>(red));
		double green = Gosu::random(40, 255);
		color.set_green(static_cast<Gosu::Color::Channel>(green));
		double blue = Gosu::random(40, 255);
		color.set_blue(static_cast<Gosu::Color::Channel>(blue));*/

		pos_x = Gosu::random(0, 1920);
		pos_y = Gosu::random(0, 1080);
	}

	double x() const
	{
		return pos_x;
	}

	double y() const
	{
		return pos_y;
	}

	void draw() const
	{
		const Gosu::Image& image = animation.at(Gosu::milliseconds() / 100 % animation.size());

		image.draw(pos_x - image.width() / 2.0, pos_y - image.height() / 2.0,
			1, 1, Gosu::AM_ADD);
	}

};


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

		pos_x = Gosu::clamp(pos_x + offsetx(), 0.0, 1920.0);
		pos_y = Gosu::clamp(pos_y + offsety(), 0.0, 1080.0);
	}

	void draw() const
	{
		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.5, 0.5); // PNG-Center
	}

	
	void kollision() {

		vfaktortest = 0;

	}

	void collect_stars(std::list<Star>& stars)
	{
		std::list<Star>::iterator cur = stars.begin();
		while (cur != stars.end()) {
			if (Gosu::distance(pos_x, pos_y, cur->x(), cur->y()) < 35) {
				cur = stars.erase(cur);
			}
			else {
				++cur;
			}
		}
	}
};

class hitbox : public Player {

public:





};

class projektil {

	



public:

	double pos_x, pos_y, vel_x, vel_y, angle, vfaktorPro;


	



	double offsetx()
	{
		return Gosu::offset_x(angle - 90, vfaktorPro); // Geschwindigkeit X_Richtung

	}

	double offsety()
	{

		return Gosu::offset_y(angle - 90, vfaktorPro); // Geschwindigkeit Y_Richtung
	}




};

class machinegun : public projektil {

	Gosu::Image bild;
	
public:

	machinegun()
		: bild("car.png")
	{
		pos_x = pos_y = vel_x = vel_y = angle = 0;
	}
	

};



class GameWindow : public Gosu::Window
{
	int kolrad = 80; //Kollisionsradius
	Player p1, p2;

	Animation star_anim;

	std::list<Star> stars;

public:
	Gosu::Image bild;
	GameWindow()
		: Window(1920, 1080), bild("map_1.png")
	{
		set_caption("Need for Gosu");
		p1.warp(400, 300);
		p2.warp(600, 700);

		star_anim = Gosu::load_tiles("Star.png", 60, 60);
	}

	void update() override
	{
		// Player 1

		if (((Gosu::Input::down(Gosu::KB_LEFT)) || (Gosu::Input::down(Gosu::GP_0_LEFT)))) // Links (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p1.turn_left();
		}
		
		if (((Gosu::Input::down(Gosu::KB_RIGHT)) || (Gosu::Input::down(Gosu::GP_0_RIGHT)))) // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p1.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_UP)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_2))) // Vorwärts (Pfeiltase) (A/X)
		{ 
			p1.accelerate();

			if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

				p1.kollision();

			}
		}


		if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_1))) // Rückwärts (Pfeiltase) (B/O)
		{ 
			p1.reverse();

			/*if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

				p1.kollision();

			}*/
		}
		
		if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN)) // Entschleunigung
		{
			p1.deceleration();
		}

		p1.move();

		//Player 2

		if (((Gosu::Input::down(Gosu::KB_A)) || (Gosu::Input::down(Gosu::GP_1_LEFT)))) // Links (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p2.turn_left();
		}

		if (((Gosu::Input::down(Gosu::KB_D)) || (Gosu::Input::down(Gosu::GP_1_RIGHT)))) // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
		{
			p2.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_W) || (Gosu::Input::down(Gosu::GP_1_BUTTON_2)))) // Vorwärts (Pfeiltase) (A/X)
		{
			p2.accelerate();
		
			if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

				p2.kollision();
			
			}
		
		}


		if ((Gosu::Input::down(Gosu::KB_S)) || (Gosu::Input::down(Gosu::GP_1_BUTTON_1))) // Rückwärts (Pfeiltase) (B/O)
		{
			p2.reverse();

			/*if (Gosu::distance(p1.pos_x, p1.pos_y, p2.pos_x, p2.pos_y) < kolrad) {

				p2.kollision();

			}*/
		}

		if (!Gosu::Input::down(Gosu::KB_W) && !Gosu::Input::down(Gosu::KB_S)) // Entschleunigung
		{
			p2.deceleration();
		}

		p2.move();	


		p1.collect_stars(stars);
		p2.collect_stars(stars);

		if (std::rand() % 25 == 0 && stars.size() < 25) { // spawngeschwindigkeit, spawnmaximum
			stars.push_back(Star(star_anim));
		}
	}

	void draw() override
	{
		p1.draw(); // Car
		p2.draw(); // Car2
		//bild.draw(0,0,0.0,1,1); // Racetrack
		
		for (Star& star : stars) {
			star.draw();
		}
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}