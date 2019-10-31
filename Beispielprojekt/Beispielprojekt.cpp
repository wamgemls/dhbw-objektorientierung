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


typedef std::vector<Gosu::Image> Animation; 


class item
{
	Animation animation;
	Gosu::Color color;
	
	double pos_x, pos_y;
	bool shown;

public:

	item(Animation animation, double pos_x, double pos_y, bool shown): animation(animation), pos_x(pos_x), pos_y(pos_y), shown(shown){}

	double x() const {
		return pos_x;
	}

	double y() const {
		return pos_y;
	}

	bool isshown() const {
		return shown;
	}

	void show() {
		shown = true;
	}

	void hide() {
		shown = false;
	}

	void draw() const {
		const Gosu::Image& image = animation.at(Gosu::milliseconds() / 100 % animation.size());

		image.draw(pos_x - image.width() / 2.0, pos_y - image.height() / 2.0, 1, 1, Gosu::AM_ADD);
	}
	
};


class Player
{
	Gosu::Image bild;
	
	double pos_x, pos_y, vel_x, vel_y, angle, vfaktor;
	int armed;

public:

	Player()
		: bild("car.png")
	{
		pos_x = pos_y = vel_x = vel_y = angle = vfaktor = 0;
	}

	double x() const {
		return pos_x;
	}

	double y() const {
		return pos_y;
	}

	void warp(double x, double y) { // Teleport an Postition x y
		pos_x = x;
		pos_y = y;
	}

	void turn_left() {
		if (vfaktor != 0 && vfaktor > 0) {

			angle -= 1.5;
		}

		if (vfaktor != 0 && vfaktor < 0) {

			angle += 1.5;
		}
	}

	void turn_right(){

		if (vfaktor != 0 && vfaktor > 0) {

			angle += 1.5;
		}

		if (vfaktor != 0 && vfaktor < 0) {

			angle -= 1.5;
		}
	}

	void accelerate() {
		
		vfaktor = vfaktor + 0.2;

		if (vfaktor >= 5)
		{
			vfaktor = 5;
		}

	}

	void reverse() {

		vfaktor = vfaktor - 0.4;

		if (vfaktor <= -2)
		{
			vfaktor = -2;
		}

	}

	void deceleration() {

		vfaktor = vfaktor * 0.97;

		if (abs(vfaktor) < 0.3)
		{
			vfaktor = 0;
		}
	}

	double offsetx() {

		return Gosu::offset_x(angle - 90, vfaktor); // Geschwindigkeit X_Richtung
	}

	double offsety() {
	
		return Gosu::offset_y(angle - 90, vfaktor); // Geschwindigkeit Y_Richtung
	}
	
	void move() {
	
		pos_x = Gosu::clamp(pos_x + offsetx(), 0.0, 1920.0);
		pos_y = Gosu::clamp(pos_y + offsety(), 0.0, 1080.0);
	}
	
	void kollision() {

		vfaktor = 0;

	}

	void collect_stars(std::list<item>& items) {
		
		for (item& element : items) {

			if (element.isshown() == true && Gosu::distance(pos_x, pos_y, element.x(), element.y()) < 30) {

				element.hide();
			}
		}
	}

	void draw() const {
	
		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.5, 0.5); // PNG-Center
	}

};

class hitbox : public Player {

public:




};



class GameWindow : public Gosu::Window
{
	int kolrad = 30; //Kollisionsradius
	Player p1, p2;

	struct item_pos {
		double pos_x, pos_y;
	};

	Animation item_anim;

	std::list<item> items;
	
	item_pos a, b, c, d, e, f;


public:

	Gosu::Image bild;
	GameWindow()
		: Window(1920, 1080), bild("map_1.png")
	{
		set_caption("Need for Gosu");
		p1.warp(400, 300);
		p2.warp(600, 700);

		item_anim = Gosu::load_tiles("Star.png", 60, 60);

		a.pos_x = 1682;
		a.pos_y = 500;

		b.pos_x = 1618;
		b.pos_y = 500;

		c.pos_x = 1080;
		c.pos_y = 130;

		d.pos_x = 1080;
		d.pos_y = 194;

		e.pos_x = 240;
		e.pos_y = 520;

		f.pos_x = 304;
		f.pos_y = 520;

		
		//Star item(star_anim, 50, 50);
		items.push_back(item(item_anim, a.pos_x,a.pos_y,true));
		items.push_back(item(item_anim, b.pos_x,b.pos_y,true));
		items.push_back(item(item_anim, c.pos_x,c.pos_y,true));
		items.push_back(item(item_anim, d.pos_x,d.pos_y,true));
		items.push_back(item(item_anim, e.pos_x,e.pos_y,true));
		items.push_back(item(item_anim, f.pos_x,f.pos_y,true));

	}
	

	void update() override
	{
		// Player 1

		
		if (((Gosu::Input::down(Gosu::KB_LEFT)) || (Gosu::Input::down(Gosu::GP_0_LEFT)))) { // Links (Pfeiltase) (Steuerkreuz oder Stick)
			
			p1.turn_left();
		}
		
		if (((Gosu::Input::down(Gosu::KB_RIGHT)) || (Gosu::Input::down(Gosu::GP_0_RIGHT)))) { // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
			
			p1.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_UP)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_2))) { // Vorwärts (Pfeiltase) (A/X)
			p1.accelerate();

			if (Gosu::distance(p1.x(),p1.y(),p2.x(),p2.y()) < kolrad) {

				p1.kollision();

			}
		}

		if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_1))) { // Rückwärts (Pfeiltase) (B/O)

			p1.reverse();

		}
		
		if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN)) { // Entschleunigung

			p1.deceleration();
		}

		p1.move();



		//Player 2

		if (((Gosu::Input::down(Gosu::KB_A)) || (Gosu::Input::down(Gosu::GP_1_LEFT)))) { // Links (Pfeiltase) (Steuerkreuz oder Stick)
		
			p2.turn_left();
		}

		if (((Gosu::Input::down(Gosu::KB_D)) || (Gosu::Input::down(Gosu::GP_1_RIGHT)))) { // Rechts (Pfeiltase) (Steuerkreuz oder Stick)
		
			p2.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_W) || (Gosu::Input::down(Gosu::GP_1_BUTTON_2)))) { // Vorwärts (Pfeiltase) (A/X)
		
			p2.accelerate();
		
			if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) < kolrad) {

				p2.kollision();
			
			}
		
		}

		if ((Gosu::Input::down(Gosu::KB_S)) || (Gosu::Input::down(Gosu::GP_1_BUTTON_1))) { // Rückwärts (Pfeiltase) (B/O)
		
			p2.reverse();

		}

		if (!Gosu::Input::down(Gosu::KB_W) && !Gosu::Input::down(Gosu::KB_S)) { // Entschleunigung
		
			p2.deceleration();
		}

		p2.move();	



		p1.collect_stars(items);
		p2.collect_stars(items);

		
		for (item& element : items) {
					
			if (!element.isshown() && std::rand() % 1000 == 0) {

				element.show();

			}
		}		
	}

	void draw() override {

		p1.draw(); // Car
		p2.draw(); // Car2
		bild.draw(0,0,0.0,1,1); // Racetrack

		for (item& element : items) {

			if (element.isshown()) {

				element.draw();

			}
		}


	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}