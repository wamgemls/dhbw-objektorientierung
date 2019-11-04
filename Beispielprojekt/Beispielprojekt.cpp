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

class rocketlauncher {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;
	
	double pos_x, pos_y, angle, vfaktor;
	int cartridge;
	

public:

	rocketlauncher(double in_pos_x, double in_pos_y,double in_angle, int cartridge) : bild("rakete.png")

	{
		this->pos_x = in_pos_x;
		this->pos_y = in_pos_y;
		this->angle = in_angle;
		this->cartridge = cartridge;
		vfaktor = 10;
		
	}

	~rocketlauncher() {


	}
	
	double x() const {
		return pos_x;
	}

	double y() const {
		return pos_y;
	}

	
	double offsetx() {

		return Gosu::offset_x(angle - 90, vfaktor); // Geschwindigkeit X_Richtung
	}

	double offsety() {

		return Gosu::offset_y(angle - 90, vfaktor); // Geschwindigkeit Y_Richtung
	}

	void move() {

		pos_x = pos_x + offsetx();
		pos_y = pos_y + offsety();
	}

	void draw() const {

		bild.draw_rot(pos_x, pos_y, 0.5, angle-90, 0.5, 0.5); // PNG-Center
	}


};

class boost {

	double vfaktor;


public:



};


class Player
{
	enum weapon {

		unarmed, rocketlauncher, granade, protection, boost,
	};

	Gosu::Image bild;
	Gosu::Sample s_item_roll;
	double pos_x, pos_y, angle, vfaktor;
	weapon arming;
	bool firstcoll;
	int reloadtime;

	
public:

	Player(): bild("car.png"), s_item_roll("item_roll.wav")
		
	{
		pos_x = pos_y = angle = vfaktor = 0;
		arming = unarmed;
		firstcoll = true;
		reloadtime = 0;
	}

	double x() const {
		return pos_x;
	}

	double y() const {
		return pos_y;
	}

	double an() const {
		return angle;
	}

	bool isrocketarmed() const {

		if (arming == rocketlauncher) {
			return true;
		}
	}

	void setvfaktor(double vfaktor_neu) {
		vfaktor = vfaktor_neu;
	}

	int isreloadtime() const {
		return reloadtime;
	}

	void setreloadtime(int reloadtime_neu) {
		reloadtime = reloadtime_neu;
	}
	
	void resetreloadtime() {
		
		switch (arming)
		{
		case unarmed:
			reloadtime = 0;
			break;

		case rocketlauncher:
			reloadtime = 100;
			break;
			
		}
		
	}

	bool firstcollision() {
		return firstcoll;
	}
	
	void firstcollisionON() {
		firstcoll = true;
	}

	void firstcollisionOFF() {
		firstcoll = false;
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
	
	void collision() {

		vfaktor = 0;	
	}

	void collect_items(std::list<item>& items) {
		
		for (item& element : items) {

			if (element.isshown() == true && Gosu::distance(pos_x, pos_y, element.x(), element.y()) < 30 && arming == unarmed) {

				element.hide();
				s_item_roll.play();
				//arming = weapon(rand() % 2 + 3);
				arming = rocketlauncher;
				
			}
		}
	}

	void draw() const {
	
		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.5, 0.5); // PNG-Center
	}

};

class GameWindow : public Gosu::Window
{
	
	Gosu::Sample s_crash;
	int kolrad = 40; //Kollisionsradius
	Player p1, p2;

	struct item_pos {
		double pos_x, pos_y;
	};

	Animation item_anim;

	std::list<item> items;
	
	item_pos a, b, c, d, e, f;

	std::vector<rocketlauncher> rockets;
	//std::vector<granade> granade;

public:

	Gosu::Image bild;
	GameWindow()
		: Window(1920, 1080), bild("map_1.png"), s_crash("crash.wav")
	{
		set_caption("Need for Gosu");
		p1.warp(400, 300);
		p2.warp(600, 700);

		item_anim = Gosu::load_tiles("Star.png", 60, 60);

		//Star item(star_anim, 50, 50);
		items.push_back(item(item_anim, 1682, 500,true));
		items.push_back(item(item_anim, 1618, 500,true));
		items.push_back(item(item_anim, 1080, 130,true));
		items.push_back(item(item_anim, 1080, 194,true));
		items.push_back(item(item_anim, 240, 520,true));
		items.push_back(item(item_anim, 304, 520,true));

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

		}

		{ //Kollsionsprüfung p1

			if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) < kolrad) {

				p1.collision();

				if (p1.firstcollision() == true) {

					s_crash.play();
					p1.firstcollisionOFF();
					p2.firstcollisionOFF();

				}
			}

			if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) > 60) {

				p1.firstcollisionON();

			}

		}


		if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_1))) { // Rückwärts (Pfeiltase) (B/O)

			p1.reverse();

		}

		if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN)) { // Entschleunigung

			p1.deceleration();
		}

		if (Gosu::Input::down(Gosu::KB_SPACE) && p1.isreloadtime() == 0 && p1.isrocketarmed()) {
			p1.resetreloadtime();
			rockets.push_back(rocketlauncher(p1.x(), p1.y(), p1.an(), 10));

		}

		p1.move();
		p1.collect_items(items);
		
		if (p1.isreloadtime() > 0) {
			p1.setreloadtime(p1.isreloadtime() - 1);
		}
		





		//Player 2

		if (((Gosu::Input::down(Gosu::KB_A)) || (Gosu::Input::down(Gosu::GP_1_LEFT)))) { // Links (Pfeiltase) (Steuerkreuz oder Stick)

			p2.turn_left();
		}

		if (((Gosu::Input::down(Gosu::KB_D)) || (Gosu::Input::down(Gosu::GP_1_RIGHT)))) { // Rechts (Pfeiltase) (Steuerkreuz oder Stick)

			p2.turn_right();
		}

		if ((Gosu::Input::down(Gosu::KB_W) || (Gosu::Input::down(Gosu::GP_1_BUTTON_2)))) { // Vorwärts (Pfeiltase) (A/X)

			p2.accelerate();

		}

		{ //Kollsionsprüfung p2

			if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) < kolrad) {

				p2.collision();

				if (p2.firstcollision() == true) {

					s_crash.play();
					p1.firstcollisionOFF();
					p2.firstcollisionOFF();

				}
			}

			if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) > 60) {

				p2.firstcollisionON();

			}
		
		}

		if ((Gosu::Input::down(Gosu::KB_S)) || (Gosu::Input::down(Gosu::GP_1_BUTTON_1))) { // Rückwärts (Pfeiltase) (B/O)

			p2.reverse();

		}

		if (!Gosu::Input::down(Gosu::KB_W) && !Gosu::Input::down(Gosu::KB_S)) { // Entschleunigung

			p2.deceleration();
		}

		p2.move();
		p2.collect_items(items);

		
		
		
		
		
		
		//General Updates
		
		for (item& element : items) {

			if (!element.isshown() && std::rand() % 1000 == 0) {

				element.show();

			}
		}	


		for (rocketlauncher& element : rockets) {

			element.move();
		}

	
		auto iter = rockets.begin();

		while (iter != rockets.end()) {

			
			if ((*iter).x() < 0 || (*iter).x() > 1920 || (*iter).y() < 0 || (*iter).y() > 1080){

				rockets.erase(iter);
				break;
			}

			iter++;
		}
	

	}

	void draw() override {

		p1.draw(); // Car
		p2.draw(); // Car2
		bild.draw(0,0,0.0,1,1); // Racetrack

		for (rocketlauncher& element : rockets) {

			element.draw();
		}

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