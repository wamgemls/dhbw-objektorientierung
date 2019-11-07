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
int globalcounter;
double globaltime;

typedef std::vector<Gosu::Image> Animation;


enum weapon {

	a_unarmed, a_rocketlauncher, a_gun, a_protection, a_boost,
};

bool linetouched(double ch_posx1, double ch_posy1, double ch_posx2, double ch_posy2, double p_posx, double p_posy) {

	bool rueck = false;

	for (size_t i = ch_posx1; i <= ch_posx2; i++) {

		for (size_t j = ch_posy1; j <= ch_posy2; j++) {


			if (Gosu::distance(i, j, p_posx, p_posy) < 3) {

				rueck = true;
				break;
			}

		}
	}

	return rueck;
}


class ui {
	
	Gosu::Image p1, p2, p3, p4;
	Gosu::Image Zeit;

public:

	ui() :p1("ui_p1.png"), p2("ui_p2.png"), p3("ui_p3.png"), p4("ui_p4.png") {

	}


	void draw() {
		
		p1.draw(0, 780, 0.5, 1, 1);
		p2.draw(1520, 780, 0.5, 1, 1);
		p3.draw(1520, 0, 0.5, 1, 1);
		p4.draw(0, 0, 0.5, 1, 1);

	}
};



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

class Player {

	Gosu::Image bild;
	Gosu::Sample s_item_roll;
	double pos_x, pos_y, angle, vfaktor,bfaktor,efaktor,vmax,vmin;
	weapon arming;
	bool firstcoll;
	bool isprotected;
	bool stafi, ch_1, ch_2, ch_3;
	int reloadtime;
	int lapstatus,lap;
	
	
public:
	
	Player(): bild("car.png"), s_item_roll("item_roll.wav") {
		
		pos_x = pos_y = vfaktor = 0;
		angle = 180;
		arming = a_unarmed;
		firstcoll = true;
		isprotected = false;
		reloadtime = 0;
		stafi = ch_1 = ch_2 = ch_3 = false;
		lapstatus = 0;
		bfaktor = 0.2;
		efaktor = 0.4;
		vmax = 5;
		vmin = -2;
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

	weapon currentarming() const {
		return arming;
	}

	void setunarmed() {
		arming = a_unarmed;
	}

	void setvfaktor(double vfaktor_neu) {
		vfaktor = vfaktor_neu;
	}

	void setstafi() {
		stafi = true;
	}

	void setch1() {
		ch_1 = true;
	}

	void setch2() {
		ch_2 = true;
	}

	void setch3() {
		ch_3 = true;
	}

	void roundcounter() {

		if (stafi == true && ch_1 == false && ch_2 == false && ch_3 == false && lapstatus == 0) {
			lapstatus = 1;
		}

		if (stafi == false && ch_1 == true && ch_2 == false && ch_3 == false && lapstatus == 1) {
			lapstatus = 2;
		}

		if (stafi == false && ch_1 == false && ch_2 == true && ch_3 == false && lapstatus == 2) {
			lapstatus = 3;
		}

		if (stafi == false && ch_1 == false && ch_2 == false && ch_3 == true && lapstatus == 3) {
			lapstatus = 4;
		}

		if (stafi == true && ch_1 == false && ch_2 == false && ch_3 == false && lapstatus == 4) {
			lap += 1;
			lapstatus = 1;
		}

		stafi = ch_1 = ch_2 = ch_3 = false;

	}

	int currentround() {
		return lap;
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

	void setprotected() {
		isprotected = true;
	}

	void setunprotected() {
		isprotected = false;
	}

	void setaccvboost() {
		bfaktor = 0.8;
		vmax = 8;
	}

	void setaccstandard() {
		bfaktor = 0.4;
		vmax = 5;
	}

	void accelerate() {
		
	vfaktor = vfaktor + bfaktor;

		if (vfaktor >= vmax)
		{
			vfaktor = vmax;
		}
	}

	void reverse() {

	vfaktor = vfaktor - efaktor;

		if (vfaktor <= vmin)
		{
			vfaktor = vmin;
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

			if (element.isshown() == true && Gosu::distance(pos_x, pos_y, element.x(), element.y()) < 30 && arming == a_unarmed) {

				element.hide();
				s_item_roll.play();
				//arming = weapon(rand() % 2 + 3);
				arming = a_protection;
				
			}
		}
	}

	
	void draw() const {
	
		bild.draw_rot(pos_x, pos_y, 5, angle, 0.5, 0.5); // PNG-Center
	}

};

class rocketlauncher {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;
	
	Player* owner;
	
	double pos_x, pos_y, angle, vfaktor;

public:
	

	rocketlauncher(double in_pos_x, double in_pos_y, double in_angle, Player* in_owner) : bild("item_r.png") {
		pos_x = in_pos_x;
		pos_y = in_pos_y;
		angle = in_angle;
		vfaktor = 10;
		owner = in_owner;
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

class gun {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;

	Player* owner;

	int cartridge;
	double pos_x, pos_y, angle, vfaktor;

public:

	gun(double in_pos_x, double in_pos_y, double in_angle, int cartridge, Player* in_owner) : bild("item_r.png") {
		pos_x = in_pos_x;
		pos_y = in_pos_y;
		angle = in_angle;
		cartridge = cartridge;
		vfaktor = 10;
		owner = in_owner;
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

		bild.draw_rot(pos_x, pos_y, 0.5, angle - 90, 0.5, 0.5); // PNG-Center
	}


};

class boost {

	
	Gosu::Image bild;
	Gosu::Sample s_machinegun;

	Player* owner;
	double deletetime;


public:

	boost(Player* in_owner, double in_deletetime):bild("boost_back.png") {
		
		owner = in_owner;
		deletetime = in_deletetime;
	}
	
	~boost() {
		
		owner->setaccstandard();
	}

	Player* giveowner() {
		return owner;
	}

	void setboost() {
		owner->setaccvboost();
	}
	
	double givedeletetime() {
		return deletetime;
	}

	void draw() const {

		bild.draw_rot(owner->x(),owner->y() , 3, owner->an(), 0.5, 0.5);
	}

};

class protection {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;
	Player* owner;
	double deletetime;


public:

	protection(Player* in_owner, double in_deletetime) :bild("schild.png") {

		owner = in_owner;
		deletetime = in_deletetime;
	}

	~protection() {

		owner->setunprotected();
	}

	Player* giveowner() {
		return owner;
	}

	void setprotection() {
		owner->setprotected();
	}

	void setunprotected() {
		owner->setunprotected();
	}

	double givedeletetime() {
		return deletetime;
	}

	void draw() const {

		bild.draw_rot(owner->x(), owner->y(), 3, owner->an(), 0.5, 0.5);
	}

};


class GameWindow : public Gosu::Window
{
	
	Gosu::Sample s_crash, s_start, s_rocket_launch, s_nitro;
	int kolrad = 40; //Kollisionsradius
	Player p1, p2, p3, p4;
	ui ui;
	Animation item_anim;
	Gosu::Image map1;
	Gosu::Image ui_b,ui_rt,ui_prot,ui_weapon;
	bool freigabe;
	double currenttime;
	bool timerstart;

	Gosu::Font p1round, p2round, p3round, p4round, time_counter, start_condition, start_countdown;



	struct checkline {

		double x1, y1, x2, y2;
	};

	checkline stafi, c1, c2, c3;

	std::list<item> items;
	std::vector<rocketlauncher> rockets;
	std::vector<boost> boosts;
	std::vector<protection> protections;

public:

	
	GameWindow()
		: Window(1920, 1080), map1("map_1_C.png"), s_crash("crash.wav"), s_start("start.wav"), s_rocket_launch("rocket_launch.wav"), s_nitro("nitro.wav"), ui_rt("item_r.png"), ui_b("item_b.png"), ui_prot("item_s.png"),ui_weapon("item_w.png"), 
			p1round(40), p2round(40), p3round(40), p4round(40), time_counter(30), start_condition(30), start_countdown(30) {
		
		globalcounter = 0;

		set_caption("Need for Gosu");
		p1.warp(1010, 858);
		p2.warp(1010, 918);



		item_anim = Gosu::load_tiles("Star.png", 60, 60);

		//Star item(star_anim, 50, 50);
		items.push_back(item(item_anim, 1510, 646,true));
		items.push_back(item(item_anim, 1510, 711,true));
		items.push_back(item(item_anim, 739, 296,true));
		items.push_back(item(item_anim, 739, 359,true));
		items.push_back(item(item_anim, 586, 791,true));
		items.push_back(item(item_anim, 641, 791,true));

		stafi.x1 = 1115;
		stafi.y1 = 830;
		stafi.x2 = 1115;
		stafi.y2 = 961;

		c1.x1 = 1584;
		c1.y1 = 505;
		c1.x2 = 1715;
		c1.y2 = 505;

		c2.x1 = 1088;
		c2.y1 = 94;
		c2.x2 = 1088;
		c2.y2 = 226; 

		c3.x1 = 200;
		c3.y1 = 505;
		c3.x2 = 336;
		c3.y2 = 505;

	}
	
	void update() override
	{
		

		if (Gosu::Input::down(Gosu::KB_U)) {
			currenttime = globaltime;
			timerstart = true;
			s_start.play();
		}

		if (timerstart == true)
		{
			globalcounter += 1;
			globaltime = double(globalcounter) / double(60);
		}

		if (globaltime > (currenttime + 3)) {
			freigabe = true;
		}

		if (freigabe) {



			// Player 1


			if (((Gosu::Input::down(Gosu::KB_LEFT)) || (Gosu::Input::down(Gosu::GP_0_LEFT)))) { // Links (Pfeiltase) (Steuerkreuz oder Stick)

				p1.turn_left();
			}

			if (((Gosu::Input::down(Gosu::KB_RIGHT)) || (Gosu::Input::down(Gosu::GP_0_RIGHT)))) { // Rechts (Pfeiltase) (Steuerkreuz oder Stick)

				p1.turn_right();
			}

			if ((Gosu::Input::down(Gosu::KB_UP)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_1))) { // Vorwärts (Pfeiltase) (A/X)

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


			if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_0))) { // Rückwärts (Pfeiltase) (B/O)

				p1.reverse();

			}

			if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN) && !Gosu::Input::down(Gosu::GP_0_BUTTON_0) && !Gosu::Input::down(Gosu::GP_0_BUTTON_1)) { // Entschleunigung

				p1.deceleration();
			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_10)) && p1.currentarming() == a_rocketlauncher) {

				rockets.push_back(rocketlauncher(p1.x(), p1.y(), p1.an(), &p1));
				p1.setunarmed();
				s_rocket_launch.play();

			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_10)) && p1.currentarming() == a_boost) {

				boosts.push_back(boost(&p1,globaltime+2));
				p1.setunarmed();
				s_nitro.play();

			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_10)) && p1.currentarming() == a_protection) {

				protections.push_back(protection(&p1, globaltime + 5));
				p1.setunarmed();

			}

			p1.move();
			p1.collect_items(items);

			






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

			if (linetouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p1.x(), p1.y())) {
				p1.setstafi();
			}

			if (linetouched(c1.x1, c1.y1, c1.x2, c1.y2, p1.x(), p1.y())) {
				p1.setch1();
			}

			if (linetouched(c2.x1, c2.y1, c2.x2, c2.y2, p1.x(), p1.y())) {
				p1.setch2();
			}

			if (linetouched(c3.x1, c3.y1, c3.x2, c3.y2, p1.x(), p1.y())) {
				p1.setch3();
			}

			p1.roundcounter();

			if (linetouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p2.x(), p2.y())) {
				p2.setstafi();
			}

			if (linetouched(c1.x1, c1.y1, c1.x2, c1.y2, p2.x(), p2.y())) {
				p2.setch1();
			}

			if (linetouched(c2.x1, c2.y1, c2.x2, c2.y2, p2.x(), p2.y())) {
				p2.setch2();
			}

			if (linetouched(c3.x1, c3.y1, c3.x2, c3.y2, p2.x(), p2.y())) {
				p2.setch3();
			}

			p2.roundcounter();





			for (item& element : items) {

				if (!element.isshown() && std::rand() % 1000 == 0) {

					element.show();

				}
			}


			for (rocketlauncher& element : rockets) {

				element.move();
			}

			
			
			auto iter = protections.begin();

			while (iter != protections.end()) {

				if (globaltime > iter->givedeletetime()) {

					protections.erase(iter);
					break;
				}
				else {
					iter->setprotection();
				}

				iter++;
			}
			
			{ //Löschung von nicht mehr sichbaren Raketen
				auto iter = boosts.begin();

				while (iter != boosts.end()) {


					if (globaltime > iter->givedeletetime()) {

						boosts.erase(iter);
						break;
					}
					else {
						iter->setboost();
					}

					iter++;
				}
			}

			{ //Löschung von nicht mehr sichbaren Raketen
				auto iter = rockets.begin();

				while (iter != rockets.end()) {


					if ((*iter).x() < 0 || (*iter).x() > 1920 || (*iter).y() < 0 || (*iter).y() > 1080) {

						rockets.erase(iter);
						break;
					}

					iter++;
				}
			}

		}
	}

	void draw() override {
		
		p1round.draw((std::to_string(p1.currentround() + 1) + " / 5"), 245, 975, 4, 1, 1);
		p1round.draw((std::to_string(p1.currentround() + 1) + " / 5"), 247, 973, 3, 1, 1, Gosu::Color::BLACK);

		p2round.draw((std::to_string(p2.currentround() + 1) + " / 5"), 1605, 975, 4, 1, 1);
		p2round.draw((std::to_string(p2.currentround() + 1) + " / 5"), 1607, 973, 3, 1, 1, Gosu::Color::BLACK);

		p3round.draw((std::to_string(p3.currentround() + 1) + " / 5"), 1605, 65, 4, 1, 1);
		p3round.draw((std::to_string(p3.currentround() + 1) + " / 5"), 1607, 63, 3, 1, 1, Gosu::Color::BLACK);

		p4round.draw((std::to_string(p4.currentround() + 1) + " / 5"), 245, 65, 4, 1, 1);
		p4round.draw((std::to_string(p4.currentround() + 1) + " / 5"), 247, 63, 3, 1, 1, Gosu::Color::BLACK);



		time_counter.draw_rel(std::to_string(globaltime), 960, 560, 3, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);

		if (freigabe == (currenttime)) {
			start_condition.draw_rel("Press 'U' to start !", 960, 500, 3, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);
		}
		if (globaltime > (currenttime + 0) && globaltime < (currenttime + 1)) {
			start_countdown.draw_rel("3", 960, 500, 4, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);
		}
		if (globaltime > (currenttime + 1) && globaltime < (currenttime + 2)) {
			start_countdown.draw_rel("2", 960, 500, 4, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);
		}
		if (globaltime > (currenttime + 2) && globaltime < (currenttime + 3)) {
			start_countdown.draw_rel("1", 960, 500, 4, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);
		}
		if (globaltime > (currenttime + 3) && globaltime < (currenttime + 4)) {
			start_countdown.draw_rel("GO!", 960, 500, 4, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);
		}
		
		ui.draw();

		if (p1.currentarming() != a_unarmed) {

			weapon arming;

			arming = p1.currentarming();
			switch (arming)
			{
			case a_rocketlauncher:
				ui_rt.draw(20, 880, 1, 0.8, 0.8);
				break;

			case a_boost:
				ui_b.draw(20, 880, 1, 0.8, 0.8);
				break;

			case a_protection:
				ui_prot.draw(20, 880, 1, 0.8, 0.8);
				break;

			case a_gun:
				ui_weapon.draw(20, 880, 1, 0.8, 0.8);
				break;
			}
		}

		

		p1.draw(); // Car
		p2.draw(); // Car2

		map1.draw(0,0,0.0,1,1); // Racetrack


		for (protection& element : protections) {


			element.draw();

		}

		for (boost& element : boosts) {

			
			element.draw();
			
		}

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