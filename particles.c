/*
 * particles.c: a C implementation of particles.js
 *
 * TODO:
 *  - CLI argument parsing for customizable simulation variables
 *  - Optimizations (I have only tested this over SSH on Wifi,
 *      so any lag may be due to the connection bottlenecking
 *      rather than this application)
 *  - Investigate odd velocity behavior (i.e. p->vel_x and p->vel_y
 *      are bounded yet somehow exceed MAX_VELOCITY)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <curses.h>

/*
 * CONSTANTS
 */
#define N_PARTICLES 100
#define N_TYPES 5
#define MAX_VELOCITY 0.000000001
#define MAX_RADIUS 10
#define MAX_FORCE 10
#define GRAVITATIONAL_CONSTANT 1.0
#define RUN_SPEED 10000

/*
 * MATH UTILITY FUNCTIONS
 */
double drand(double low, double high){
  return (((double)rand()*(high-low))/(double)RAND_MAX)+low;
}

double dist(double x1, double y1, double x2, double y2){
  return sqrt(
    pow(x2-x1, 2) +
    pow(y2-y1, 2)
  );
}

double sign(double val){
  if(val == 0.0){
    return 0;
  }
  return abs(val)/val;
}

double constrain_abs(double val, double max){
  if(abs(val) > max){
    return max * sign(val);
  }
  return val;
}

/*
 * SIMULATION
 */
typedef struct particle {
  char type;
  double pos_x;
  double pos_y;
  double pos_x_last;
  double pos_y_last;
  double vel_x;
  double vel_y;
  double mass;
} particle;

#define PARTICLES_FOREACH() for(i=0;i<N_PARTICLES;i++)

particle *particle_init(){
  particle *p = malloc(sizeof(particle));
  p->type = rand()%N_TYPES;
  p->pos_x = drand(0, LINES);
  p->pos_y = drand(0, COLS);
  p->pos_x_last = 0.0;
  p->pos_y_last = 0.0;
  p->vel_x = drand(-MAX_VELOCITY, MAX_VELOCITY);
  p->vel_y = drand(-MAX_VELOCITY, MAX_VELOCITY);
  p->mass = 1.0;

  return p;
}

double particle_force(char type1, char type2){
  /* This is entirely arbitrary, a possible TODO would be to make this more customizable */
  if((type1 % 2 == 0 &&
      type2 % 2 == 1) ||
      type1 % 2 == 1 &&
      type2 % 2 == 0){
    return -1.0;
  } else if(type1 == type2){
    return 1.0;
  }
  return 0.0;
}

void particle_update(particle *p, particle **sim, int flip){
  double f_net_x = 0.0,
         f_net_y = 0.0,
         r = 0.0,
         theta = 0.0;
  int i;
  particle *cmp;

  /* Position */
  p->pos_x_last = p->pos_x;
  p->pos_y_last = p->pos_y;
  p->pos_x += p->vel_x;
  p->pos_y += p->vel_y;
  
  /* Wrapping */
  if(p->pos_x > LINES){
    p->pos_x = 0.0;
  } else if(p->pos_x < 0){
    p->pos_x = LINES;
  }

  if(p->pos_y > COLS){
    p->pos_y = 0.0;
  } else if(p->pos_y < 0){
    p->pos_y = COLS;
  }

  /* Gravitation */
  PARTICLES_FOREACH(){
    if(sim[i] != p){
      cmp = sim[i];
      r = dist(
        p->pos_x, p->pos_y,
        cmp->pos_x, cmp->pos_y
      );
      theta = atan(r);
      if(r == 0.0){
        r = 0.1;
      }
      if(r <= MAX_RADIUS){
        f_net_x += sign(cmp->pos_x-p->pos_x)*GRAVITATIONAL_CONSTANT*particle_force(p->type, cmp->type)*flip/r;
        f_net_y += sign(cmp->pos_y-p->pos_y)*GRAVITATIONAL_CONSTANT*particle_force(p->type, cmp->type)*flip/r;
        /*
         * This is a slightly different approach to gravitation which avoids the discontinuity due to the
         *   sign() function
         * f_net_x += GRAVITATIONAL_CONSTANT*particle_force(p->type, cmp->type)*flip/(cmp->pos_x-p->pos_x);
         * f_net_y += GRAVITATIONAL_CONSTANT*particle_force(p->type, cmp->type)*flip/(cmp->pos_y-p->pos_y);
        */
      }
    }
  }
  f_net_x = constrain_abs(f_net_x, MAX_FORCE);
  f_net_y = constrain_abs(f_net_y, MAX_FORCE);

  p->vel_x += f_net_x*cos(theta)/p->mass;
  p->vel_y += f_net_y*cos(theta)/p->mass;

  p->vel_x = constrain_abs(p->vel_x, MAX_VELOCITY);
  p->vel_y = constrain_abs(p->vel_y, MAX_VELOCITY);

  /* Failsafe for unexpected behavior, TODO investigate */
  if(abs(p->vel_x) > MAX_VELOCITY || abs(p->vel_y) > MAX_VELOCITY){
    p->vel_x = 0.0;
    p->vel_y = 0.0;
  }
}

void particle_free(particle *p){
  free(p);
}

particle **sim_init(){
  int i;
  particle **sim = malloc(sizeof(particle)*N_PARTICLES);
  PARTICLES_FOREACH(){
    sim[i] = particle_init();
  }
  return sim;
}

void sim_update(particle **sim, int flip){
  int i;
  PARTICLES_FOREACH(){
    particle_update(sim[i], sim, flip);
  }
}

void sim_free(particle **sim){
  int i;
  PARTICLES_FOREACH(){
    particle_free(sim[i]);
  }
  free(sim);
}

/*
 * RENDERING
 */
void render_draw(particle **ps){
  int i;
  particle *p;
  PARTICLES_FOREACH(){
    p = ps[i];
    attron(COLOR_PAIR(1));
    mvaddch(
      (int)p->pos_x_last,
      (int)p->pos_y_last,
      ' '
    );
    mvaddch(
      (int)p->pos_x_last,
      (int)p->pos_y_last-1,
      ' '
    );
    attron(COLOR_PAIR(p->type));
    mvaddch(
      (int)p->pos_x,
      (int)p->pos_y,
      ' '
    );
    mvaddch(
      (int)p->pos_x,
      (int)p->pos_y-1,
      ' '
    );
  }
  refresh();
}

int main(){
  int running = 1,
      flip = 1,
      i;
  particle **sim;

  srand(time(NULL));

  /* ncurses setup */
  initscr();
  cbreak();
  nodelay(stdscr, 1);
  clear();
  curs_set(0);
  start_color();
  init_pair(1, 0, 0);
  for(i=0;i<N_TYPES;i++){
    init_pair(i+1, i, i);
  }

  sim = sim_init();

  while(running){
    switch(getch()){
      case 'q':
        running = 0;
        break;
      case 'f':
        flip *= -1;
        break;
    }
    sim_update(sim, flip);
    render_draw(sim);
    usleep(RUN_SPEED);
  }

  endwin();
  sim_free(sim);

  return 0;
}
