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

	a_unarmed, a_rocketlauncher, a_granade, a_protection, a_boost,
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
	bool stafi, ch_1, ch_2, ch_3;
	int reloadtime;
	int lapstatus,lap;
	
	
public:
	
	Player(): bild("car.png"), s_item_roll("item_roll.wav") {
		
		pos_x = pos_y = vfaktor = 0;
		angle = 180;
		arming = a_unarmed;
		firstcoll = true;
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

	int isreloadtime() const {
		return reloadtime;
	}

	void setreloadtime(int reloadtime_neu) {
		reloadtime = reloadtime_neu;
	}
	
	/*void resetreloadtime() {
		
		switch (arming)
		{
		case a_unarmed:
			reloadtime = 0;
			break;

		case a_rocketlauncher:
			reloadtime = 100;
			break;

		case a_boost:
			reloadtime = 100;
			break;
			
		}
		
	}*/

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

	void setaccvboost() {
		bfaktor = 0.8;
		vmax = 8;
		//std::cout << "test" << std::endl;
		//std::cout << vmax << std::endl;
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
				arming = a_boost;
				
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
	
	int cartridge;
	double pos_x, pos_y, angle, vfaktor;

public:
	

	rocketlauncher(double in_pos_x, double in_pos_y,double in_angle, int cartridge,Player* in_owner) : bild("item_r.png") {
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

		bild.draw_rot(pos_x, pos_y, 0.5, angle-90, 0.5, 0.5); // PNG-Center
	}


};

class boost {

	
	Gosu::Image bild;
	Gosu::Sample s_machinegun;

	Player* owner;
	double activationtime;


public:

	boost(Player* in_owner, double in_activationtime):bild("boost_back.png") {
		
		owner = in_owner;
		activationtime = in_activationtime;
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
	
	

	void draw_b() const {

		bild.draw_rot(owner->x(),owner->y() , 3, owner->an(), 0.5, 0.5);
	}

};

class protection {

	Gosu::Image bild;
	Gosu::Sample s_machinegun;
	double pos_x;
	double pos_y;
	double angle;
	double vfaktor;

public:

	protection(double f_pos_x, double f_pos_y, double f_angle) {

		pos_x = f_pos_x;
		pos_y = f_pos_y;
		angle = f_angle;
	}

	void setboost() {

		//setvfaktor(10);

	}

	void move() {

		//pos_x = x();
		//pos_y = y();

	}
};


class GameWindow : public Gosu::Window
{
	
	Gosu::Sample s_crash, s_start, s_rocket_launch;
	int kolrad = 40; //Kollisionsradius
	Player p1, p2, p3, p4;
	ui ui;
	Animation item_anim;
	Gosu::Image map1;
	Gosu::Image ui_b,ui_rt;
	bool freigabe;
	double currenttime;
	bool timerstart;

	Gosu::Font p1round, p2round, p3round, p4round, time_counter, start_condition;



	struct checkline {

		double x1, y1, x2, y2;
	};

	checkline stafi, c1, c2, c3;

	std::list<item> items;
	std::vector<rocketlauncher> rockets;
	std::vector<boost> boosts;
	
public:

	
	GameWindow()
		: Window(1920, 1080), map1("map_1_C.png"), s_crash("crash.wav"), s_start("start.wav"), s_rocket_launch("rocket_launch.wav"), ui_rt("item_r.png"), ui_b("item_b.png"), p1round(40), p2round(40), p3round(40), p4round(40), time_counter(40), start_condition(40) {
		
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

				rockets.push_back(rocketlauncher(p1.x(), p1.y(), p1.an(), 10, &p1));
				p1.setunarmed();
				s_rocket_launch.play();

			}

			if ((Gosu::Input::down(Gosu::KB_SPACE)) && p1.currentarming() == a_boost) {

				boosts.push_back(boost(&p1,globaltime));
				//boosts.at(0).setboost();
				p1.setunarmed();
				

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
		
		p1round.draw((std::to_string(p1.currentround() + 1) + " / 5"), 245, 975, 3, 1, 1);
		p2round.draw((std::to_string(p2.currentround() + 1) + " / 5"), 1605, 975, 3, 1, 1);
		p3round.draw((std::to_string(p3.currentround() + 1) + " / 5"), 1605, 65, 3, 1, 1);
		p4round.draw((std::to_string(p4.currentround() + 1) + " / 5"), 245, 65, 3, 1, 1);


		time_counter.draw(std::to_string(globaltime), 900, 20, 3, 1, 1);
		if (freigabe == (currenttime)){
			start_condition.draw("Press 'U' to start !", 900, 480, 3, 1, 1);
		}
		if (globaltime > (currenttime + 0) && globaltime < (currenttime + 1)) {
			start_condition.draw("3", 900, 480, 3, 1, 1);
		}
		if (globaltime > (currenttime + 1) && globaltime < (currenttime + 2)) {
			start_condition.draw("2", 900, 480, 3, 1, 1);
		}
		if (globaltime > (currenttime + 2) && globaltime < (currenttime + 3)) {
			start_condition.draw("1", 900, 480, 3, 1, 1);
		}
		if (globaltime > (currenttime + 3) && globaltime < (currenttime + 4)) {
			start_condition.draw("GO!", 900, 480, 3, 1, 1);
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

			}
		}

		for (boost& element : boosts) {

			
			element.draw_b();
			
		}

		p1.draw(); // Car
		p2.draw(); // Car2

		map1.draw(0,0,0.0,1,1); // Racetrack
		

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