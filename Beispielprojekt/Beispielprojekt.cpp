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

bool regiontouched(double ch_posx1, double ch_posy1, double ch_posx2, double ch_posy2, double p_posx, double p_posy) {

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
	int lapstatus,lap;
	
	
	
public:
	
	Player(): bild("car.png"), s_item_roll("item_roll.wav") {
		
		pos_x = pos_y = vfaktor = 0;
		angle = 180;
		arming = a_unarmed;
		firstcoll = true;
		isprotected = false;
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

	void setarming(weapon in_arming) {
		arming = in_arming;
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

	bool currentprotection() {
		return isprotected;
	}

	void setprotected() {
		isprotected = true;
	}

	void setunprotected() {
		isprotected = false;
	}

	void setaccboost() {
		bfaktor = 0.8;
		vmax = 8;
	}

	void setaccstandard() {
		bfaktor = 0.4;
		vmax = 5;
	}

	
	void setacclow() {
		bfaktor = 0.1;
		vmax = 1;
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
				arming = weapon(rand() % 4 + 1);
				//arming = a_gun;
				
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

	Player* giveowner() {
		return owner;
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

		bild.draw_rot(pos_x, pos_y, 0.5, angle-90, 0.5, 0.5, 0.35, 0.35); // PNG-Center
	}


};

class gun {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;

	Player* owner;

	int cartridge;
	double pos_x, pos_y, angle, vfaktor;

public:

	gun(double in_pos_x, double in_pos_y, double in_angle, Player* in_owner) : bild("kugel.png") {
		pos_x = in_pos_x;
		pos_y = in_pos_y;
		angle = in_angle;
		vfaktor = 10;
		owner = in_owner;
		cartridge = 10;
	
	}

	double x() const {
		return pos_x;
	}

	double y() const {
		return pos_y;
	}

	Player* giveowner() {
		return owner;
	}

	int givecartridge() {
		return cartridge;
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

		bild.draw_rot(pos_x, pos_y, 0.5, angle, 0.5, 0.5,1.8,1.8); // PNG-Center
	}


};

class boost {

	
	Gosu::Image bild;
	Gosu::Sample s_machinegun;

	Player* owner;
	double deletetime;
	bool Hboost;


public:

	boost(Player* in_owner, double in_deletetime, bool in_Hboost):bild("boost_back.png") {
		
		owner = in_owner;
		deletetime = in_deletetime;
		Hboost = in_Hboost;
	}

	boost(Player* in_owner, double in_deletetime):bild("boost_back_lowboost.png") {

		owner = in_owner;
		deletetime = in_deletetime;
		Hboost = false;
		
	}
	
	~boost() {
		
		owner->setaccstandard();
	}

	Player* giveowner() {
		return owner;
	}

	bool giveHboost() {
		return Hboost;
	}

	void setHighboost() {
		owner->setaccboost();
	}

	void setLowboost() {
		owner->setacclow();
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
		owner->setprotected();
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
	
	Gosu::Sample s_crash, s_start, s_rocket_launch, s_rocket_hit, s_nitro, s_shield, s_pistol, s_pistol_hit;
	int kolrad = 27; //Kollisionsradius
	Player p1, p2, p3, p4;
	ui ui;
	Animation item_anim;
	Gosu::Image map1;
	Gosu::Image ui_b,ui_rt,ui_prot,ui_weapon;
	bool freigabe;
	double currenttime;
	bool gamestarted;

	Gosu::Font p1round, p2round, p3round, p4round, time_counter, start_condition, start_countdown, win_result;



	struct checkline {

		double x1, y1, x2, y2;
	};

	checkline stafi, c1, c2, c3;

	std::list<item> items;
	std::vector<rocketlauncher> rockets;
	std::vector<boost> boosts;
	std::vector<protection> protections;
	std::vector<gun> guns;

public:

	
	GameWindow()
		: Window(1920, 1080), map1("map_1_C.png"), s_crash("crash.wav"), s_start("start.wav"), s_rocket_launch("rocket_launch.wav"), s_rocket_hit("rocket_hit.wav"), s_nitro("nitro.wav"), s_shield("shield.wav"), s_pistol("pistol.wav"), s_pistol_hit("pistol_hit.wav"), ui_rt("item_r.png"), ui_b("item_b.png"), ui_prot("item_s.png"),ui_weapon("item_w.png"), 
			p1round(40), p2round(40), p3round(40), p4round(40), time_counter(30), start_condition(30), start_countdown(30), win_result(60) {
		
		globalcounter = globaltime=0;
		 

		set_caption("Need for Gosu");
		p1.warp(1010, 864);
		p2.warp(1010, 928);
		//p3.warp(900, 858);
		//p4.warp(900, 918);


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

		freigabe = false;
		gamestarted = false;
		currenttime = 99999;

	}
	
	void update() override
	{
		globalcounter += 1;
		globaltime = double(globalcounter) / double(60);

		if (Gosu::Input::down(Gosu::KB_U) && gamestarted == false) {
			currenttime = globaltime;
			gamestarted = true;
			s_start.play();
		}

		

		if ((currenttime+3) < globaltime) {


			
			// Player 1


			if (((Gosu::Input::down(Gosu::KB_LEFT)) || (Gosu::Input::down(Gosu::GP_0_LEFT)))) { // Links (Pfeiltase) (Steuerkreuz oder Stick)

				p1.turn_left();
			}

			if (((Gosu::Input::down(Gosu::KB_RIGHT)) || (Gosu::Input::down(Gosu::GP_0_RIGHT)))) { // Rechts (Pfeiltase) (Steuerkreuz oder Stick)

				p1.turn_right();
			}

			if ((Gosu::Input::down(Gosu::KB_UP)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_10) || (Gosu::Input::down(Gosu::GP_0_BUTTON_12)) )) { // Vorw�rts (Pfeiltase) (A/X)

				p1.accelerate();

			}

			{ //Kollsionspr�fung p1

				if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) < kolrad || Gosu::distance(p1.x(), p1.y(), p3.x(), p3.y()) < kolrad || Gosu::distance(p1.x(), p1.y(), p4.x(), p4.y()) < kolrad) {

					p1.collision();

					if (p1.firstcollision() == true) {

						s_crash.play();
						p1.firstcollisionOFF();
						p2.firstcollisionOFF();
						p3.firstcollisionOFF();
						p4.firstcollisionOFF();
					}
				}

				if (Gosu::distance(p1.x(), p1.y(), p2.x(), p2.y()) > 60 && Gosu::distance(p1.x(), p1.y(), p3.x(), p3.y()) > 60 && Gosu::distance(p1.x(), p1.y(), p4.x(), p4.y()) > 60) {

					p1.firstcollisionON();

				}

			}


			if ((Gosu::Input::down(Gosu::KB_DOWN)) || (Gosu::Input::down(Gosu::GP_0_BUTTON_9) || (Gosu::Input::down(Gosu::GP_0_BUTTON_11)) )) { // R�ckw�rts (Pfeiltase) (B/O)

				p1.reverse();

			}

			if (!Gosu::Input::down(Gosu::KB_UP) && !Gosu::Input::down(Gosu::KB_DOWN) && !Gosu::Input::down(Gosu::GP_0_BUTTON_10) && !Gosu::Input::down(Gosu::GP_0_BUTTON_9) && !Gosu::Input::down(Gosu::GP_0_BUTTON_11) && !Gosu::Input::down(Gosu::GP_0_BUTTON_12)) { // Entschleunigung

				p1.deceleration();
			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_1)) && p1.currentarming() == a_rocketlauncher) {

				rockets.push_back(rocketlauncher(p1.x(), p1.y(), p1.an(), &p1));
				p1.setunarmed();
				s_rocket_launch.play();

			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_1)) && p1.currentarming() == a_gun) {

				guns.push_back(gun(p1.x(), p1.y(), p1.an(), &p1));
				p1.setunarmed();
				s_pistol.play();
			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_1)) && p1.currentarming() == a_boost) {

				boosts.push_back(boost(&p1, globaltime+2, true));
				p1.setunarmed();
				s_nitro.play();

			}

			if ((Gosu::Input::down(Gosu::KB_SPACE) || Gosu::Input::down(Gosu::GP_0_BUTTON_1)) && p1.currentarming() == a_protection) {

				protections.push_back(protection(&p1, globaltime + 5));
				p1.setunarmed();
				s_shield.play();
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

			if ((Gosu::Input::down(Gosu::KB_W) || (Gosu::Input::down(Gosu::GP_1_BUTTON_5))) || (Gosu::Input::down(Gosu::GP_1_BUTTON_7)) ) { // Vorw�rts (Pfeiltase) (A/X)

				p2.accelerate();

			}

			{ //Kollsionspr�fung p2

				if (Gosu::distance(p2.x(), p2.y(), p1.x(), p1.y()) < kolrad || Gosu::distance(p2.x(), p2.y(), p3.x(), p3.y()) < kolrad || Gosu::distance(p2.x(), p2.y(), p4.x(), p4.y()) < kolrad) {

					p2.collision();

					if (p2.firstcollision() == true) {

						s_crash.play();
						p1.firstcollisionOFF();
						p2.firstcollisionOFF();
						p3.firstcollisionOFF();
						p4.firstcollisionOFF();
					}
				}

				if (Gosu::distance(p2.x(), p2.y(), p1.x(), p1.y()) > 60 && Gosu::distance(p2.x(), p2.y(), p3.x(), p3.y()) > 60 && Gosu::distance(p2.x(), p2.y(), p4.x(), p4.y()) > 60) {

					p2.firstcollisionON();

				}

			}


			if ((Gosu::Input::down(Gosu::KB_S)) || (Gosu::Input::down(Gosu::GP_1_BUTTON_4)) || (Gosu::Input::down(Gosu::GP_1_BUTTON_6)) ) { // R�ckw�rts (Pfeiltase) (B/O)

				p2.reverse();

			}

			if (!Gosu::Input::down(Gosu::KB_W) && !Gosu::Input::down(Gosu::KB_S) && !Gosu::Input::down(Gosu::GP_1_BUTTON_4) && !Gosu::Input::down(Gosu::GP_1_BUTTON_5) && !Gosu::Input::down(Gosu::GP_1_BUTTON_6) && !Gosu::Input::down(Gosu::GP_1_BUTTON_7)) { // Entschleunigung

				p2.deceleration();
			}

			if ((Gosu::Input::down(Gosu::KB_C) || Gosu::Input::down(Gosu::GP_1_BUTTON_1)) && p2.currentarming() == a_rocketlauncher) {

				rockets.push_back(rocketlauncher(p2.x(), p2.y(), p2.an(), &p2));
				p2.setunarmed();
				s_rocket_launch.play();

			}

			if ((Gosu::Input::down(Gosu::KB_C) || Gosu::Input::down(Gosu::GP_1_BUTTON_1)) && p2.currentarming() == a_gun) {

				guns.push_back(gun(p2.x(), p2.y(), p2.an(), &p2));
				p2.setunarmed();
				s_pistol.play();
			}

			if ((Gosu::Input::down(Gosu::KB_C) || Gosu::Input::down(Gosu::GP_1_BUTTON_1)) && p2.currentarming() == a_boost) {

				boosts.push_back(boost(&p2, globaltime + 2, true));
				p2.setunarmed();
				s_nitro.play();

			}

			if ((Gosu::Input::down(Gosu::KB_C) || Gosu::Input::down(Gosu::GP_1_BUTTON_1)) && p2.currentarming() == a_protection) {

				protections.push_back(protection(&p2, globaltime + 5));
				p2.setunarmed();
				s_shield.play();
			}

			p2.move();
			p2.collect_items(items);



			//player 3

			{ // Kollisionpr�fung p3
				/*if (Gosu::distance(p3.x(), p3.y(), p1.x(), p1.y()) < kolrad || Gosu::distance(p3.x(), p3.y(), p2.x(), p2.y()) < kolrad || Gosu::distance(p3.x(), p3.y(), p4.x(), p4.y()) < kolrad) {

					p3.collision();

					if (p3.firstcollision() == true) {

						s_crash.play();
						p1.firstcollisionOFF();
						p2.firstcollisionOFF();
						p3.firstcollisionOFF();
						p4.firstcollisionOFF();
					}
				}

				if (Gosu::distance(p3.x(), p3.y(), p1.x(), p1.y()) > 60 && Gosu::distance(p3.x(), p3.y(), p2.x(), p2.y()) > 60 && Gosu::distance(p3.x(), p3.y(), p4.x(), p4.y()) > 60) {

					p3.firstcollisionON();

				}*/
			}
			


			

			//player 4

			{ //Kollisionpr�fung p4

				/*if (Gosu::distance(p4.x(), p4.y(), p1.x(), p1.y()) < kolrad || Gosu::distance(p4.x(), p4.y(), p2.x(), p2.y()) < kolrad || Gosu::distance(p4.x(), p4.y(), p3.x(), p3.y()) < kolrad) {

					p4.collision();

					if (p4.firstcollision() == true) {

						s_crash.play();
						p1.firstcollisionOFF();
						p2.firstcollisionOFF();
						p3.firstcollisionOFF();
						p4.firstcollisionOFF();
					}
				}

				if (Gosu::distance(p4.x(), p4.y(), p1.x(), p1.y()) > 60 && Gosu::distance(p4.x(), p4.y(), p2.x(), p2.y()) > 60 && Gosu::distance(p4.x(), p4.y(), p3.x(), p3.y()) > 60) {

					p4.firstcollisionON();

				}*/
			}
			


			




			//General Updates



			if (regiontouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p1.x(), p1.y())) {
				p1.setstafi();
			}

			if (regiontouched(c1.x1, c1.y1, c1.x2, c1.y2, p1.x(), p1.y())) {
				p1.setch1();
			}

			if (regiontouched(c2.x1, c2.y1, c2.x2, c2.y2, p1.x(), p1.y())) {
				p1.setch2();
			}

			if (regiontouched(c3.x1, c3.y1, c3.x2, c3.y2, p1.x(), p1.y())) {
				p1.setch3();
			}

			p1.roundcounter();



			if (regiontouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p2.x(), p2.y())) {
				p2.setstafi();
			}

			if (regiontouched(c1.x1, c1.y1, c1.x2, c1.y2, p2.x(), p2.y())) {
				p2.setch1();
			}

			if (regiontouched(c2.x1, c2.y1, c2.x2, c2.y2, p2.x(), p2.y())) {
				p2.setch2();
			}

			if (regiontouched(c3.x1, c3.y1, c3.x2, c3.y2, p2.x(), p2.y())) {
				p2.setch3();
			}

			p2.roundcounter();



			if (regiontouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p3.x(), p3.y())) {
				p3.setstafi();
			}

			if (regiontouched(c1.x1, c1.y1, c1.x2, c1.y2, p3.x(), p3.y())) {
				p3.setch1();
			}

			if (regiontouched(c2.x1, c2.y1, c2.x2, c2.y2, p3.x(), p3.y())) {
				p3.setch2();
			}

			if (regiontouched(c3.x1, c3.y1, c3.x2, c3.y2, p3.x(), p3.y())) {
				p3.setch3();
			}

			p3.roundcounter();

			if (regiontouched(stafi.x1, stafi.y1, stafi.x2, stafi.y2, p4.x(), p4.y())) {
				p4.setstafi();
			}

			if (regiontouched(c1.x1, c1.y1, c1.x2, c1.y2, p4.x(), p4.y())) {
				p4.setch1();
			}

			if (regiontouched(c2.x1, c2.y1, c2.x2, c2.y2, p4.x(), p4.y())) {
				p4.setch2();
			}

			if (regiontouched(c3.x1, c3.y1, c3.x2, c3.y2, p4.x(), p4.y())) {
				p4.setch3();
			}

			p4.roundcounter();


			{ //Raketenkollision
				auto iter = rockets.begin();

				while (iter != rockets.end()) {

					if ((Gosu::distance(iter->x(), iter->y(), p1.x(), p1.y()) < 35) && (iter->giveowner() != &p1)) {
						
						if (p1.currentprotection() == true) {
							p1.setunprotected();
						}
						else {
							boosts.push_back(boost(&p1, globaltime + 2));
							rockets.erase(iter);
							s_rocket_hit.play();
							break;
						}
					}

					if ((Gosu::distance(iter->x(), iter->y(), p2.x(), p2.y()) < 35) && (iter->giveowner() != &p2)) {
						
						if (p2.currentprotection() == true) {
							p2.setunprotected();
						}
						else {
							boosts.push_back(boost(&p2, globaltime + 2));
							rockets.erase(iter);
							s_rocket_hit.play();
							break;
						}
					}

					if ((Gosu::distance(iter->x(), iter->y(), p3.x(), p3.y()) < 35) && (iter->giveowner() != &p3)) {
						
						if (p3.currentprotection() == true) {
							p3.setunprotected();
						}
						else {
							boosts.push_back(boost(&p3, globaltime + 2));
							rockets.erase(iter);
							s_rocket_hit.play();
							break;
						}
					}
						

					if ((Gosu::distance(iter->x(), iter->y(), p4.x(), p4.y()) < 35) && (iter->giveowner() != &p4)) {
						
						if (p4.currentprotection() == true) {
							p4.setunprotected();
						}
						else {
							boosts.push_back(boost(&p4, globaltime + 2));
							rockets.erase(iter);
							s_rocket_hit.play();
							break;
						}
					}

					iter++;
				}	
			}

			{ //Gunkollision
				auto iter = guns.begin();

				while (iter != guns.end()) {

					if ((Gosu::distance(iter->x(), iter->y(), p1.x(), p1.y()) < 35) && (iter->giveowner() != &p1)) {
						
						if (p1.currentprotection() == true) {
							p1.setunprotected();
						}
						else {
							boosts.push_back(boost(&p1, globaltime + 0.8));
							guns.erase(iter);
							s_pistol_hit.play();
							break;
						}
					}

					if ((Gosu::distance(iter->x(), iter->y(), p2.x(), p2.y()) < 35) && (iter->giveowner() != &p2)) {
						
						if (p2.currentprotection() == true) {
							p2.setunprotected();
						}
						else {
							boosts.push_back(boost(&p2, globaltime + 0.8));
							guns.erase(iter);
							s_pistol_hit.play();
							break;
						}
					}

					if ((Gosu::distance(iter->x(), iter->y(), p3.x(), p3.y()) < 35) && (iter->giveowner() != &p3)) {
						
						if (p3.currentprotection() == true) {
							p3.setunprotected();
						}
						else {
							boosts.push_back(boost(&p3, globaltime + 0.8));
							guns.erase(iter);
							s_pistol_hit.play();
							break;
						}
					}

					if ((Gosu::distance(iter->x(), iter->y(), p4.x(), p4.y()) < 35) && (iter->giveowner() != &p4)) {
						
						if (p4.currentprotection() == true) {
							p4.setunprotected();
						}
						else {
							boosts.push_back(boost(&p4, globaltime + 0.8));
							guns.erase(iter);
							s_pistol_hit.play();
							break;
						}
					}

					iter++;
				}
			}

			for (item& element : items) {

				if (!element.isshown() && std::rand() % 1000 == 0) {

					element.show();
				}
			}

			for (rocketlauncher& element : rockets) {

				element.move();
			}

			for (gun& element : guns) {

				element.move();
			}

			{
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
			}
			
			{
				auto iter = boosts.begin();

				while (iter != boosts.end()) {


					if (globaltime > iter->givedeletetime()) {

						boosts.erase(iter);
						break;
					}
					else {

						if (iter->giveHboost() == true) {
							iter->setHighboost();
						}

						if (iter->giveHboost() == false) {
							iter->setLowboost();
						}
					}

					iter++;
				}
			}

			{ //L�schung von nicht mehr sichbaren Raketen
				auto iter = rockets.begin();

				while (iter != rockets.end()) {


					if ((*iter).x() < 0 || (*iter).x() > 1920 || (*iter).y() < 0 || (*iter).y() > 1080) {

						rockets.erase(iter);
						break;
					}

					iter++;
				}
			}

			{ //L�schung von nicht mehr sichbaren Kugel
				auto iter = guns.begin();

				while (iter != guns.end()) {


					if ((*iter).x() < 0 || (*iter).x() > 1920 || (*iter).y() < 0 || (*iter).y() > 1080) {

						guns.erase(iter);
						break;
					}

					iter++;
				}
			}

		}
	}

	void draw() override {
		
		p1round.draw((std::to_string(p1.currentround() + 1) + " / 3"), 245, 975, 4, 1, 1);
		p1round.draw((std::to_string(p1.currentround() + 1) + " / 3"), 247, 973, 3, 1, 1, Gosu::Color::BLACK);

		p2round.draw((std::to_string(p2.currentround() + 1) + " / 3"), 1605, 975, 4, 1, 1);
		p2round.draw((std::to_string(p2.currentround() + 1) + " / 3"), 1607, 973, 3, 1, 1, Gosu::Color::BLACK);

		p3round.draw((std::to_string(p3.currentround() + 1) + " / 3"), 1605, 65, 4, 1, 1);
		p3round.draw((std::to_string(p3.currentround() + 1) + " / 3"), 1607, 63, 3, 1, 1, Gosu::Color::BLACK);

		p4round.draw((std::to_string(p4.currentround() + 1) + " / 3"), 245, 65, 4, 1, 1);
		p4round.draw((std::to_string(p4.currentround() + 1) + " / 3"), 247, 63, 3, 1, 1, Gosu::Color::BLACK);



		time_counter.draw_rel("Spielzeit: "+(std::to_string(int(globaltime))+"s"), 960, 560, 3, 0.5, 0.5, 1, 1, Gosu::Color::BLACK);

		if (!gamestarted) {
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
				ui_rt.draw(15, 876, 1, 1, 1);
				break;

			case a_boost:
				ui_b.draw(15, 876, 1, 1, 1);
				break;

			case a_protection:
				ui_prot.draw(15, 876, 1, 1, 1);
				break;

			case a_gun:
				ui_weapon.draw(15, 876, 1, 1, 1);
				break;
			}
		}

		if (p2.currentarming() != a_unarmed) {

			weapon arming;

			arming = p2.currentarming();
			switch (arming)
			{
			case a_rocketlauncher:
				ui_rt.draw(1714, 876, 1, 1, 1);
				break;

			case a_boost:
				ui_b.draw(1714, 876, 1, 1, 1);
				break;

			case a_protection:
				ui_prot.draw(1714, 876, 1, 1, 1);
				break;

			case a_gun:
				ui_weapon.draw(1714, 876, 1, 1, 1);
				break;
			}
		}

		p1.draw(); // Car
		p2.draw(); // Car2
		//p3.draw();
		//p4.draw();

		map1.draw(0,0,0.0,1,1); // Racetrack


		

		for (protection& element : protections) {
			element.draw();
		}

		for (boost& element : boosts) {
			element.draw();
		}

		for (gun& element : guns) {
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



		// Win-Condition

		if (p1.currentround() == 3) {
			win_result.draw_rel("Player 1 wins ! Very Nais !", 960, 540, 6, 0.5, 0.5, 1, 1, Gosu::Color::FUCHSIA);
		}
		if (p2.currentround() == 3) {
			win_result.draw_rel("Player 2 wins ! Very Nais !", 960, 540, 6, 0.5, 0.5, 1, 1, Gosu::Color::FUCHSIA);
		}
		if (p3.currentround() == 3) {
			win_result.draw_rel("Player 2 wins ! Very Nais !", 960, 540, 6, 0.5, 0.5, 1, 1, Gosu::Color::FUCHSIA);
		}
		if (p4.currentround() == 3) {
			win_result.draw_rel("Player 2 wins ! Very Nais !", 960, 540, 6, 0.5, 0.5, 1, 1, Gosu::Color::FUCHSIA);
		}
		
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();

}